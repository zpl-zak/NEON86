#pragma once

#define M(...) #__VA_ARGS__

static const char* _shader_common = M(
    /* NEON structures */
    struct TNEON
    {
    float4x4 World;
    float4x4 WorldView;
    float4x4 InverseWorld;
    float4x4 InverseWorldView;
    float4x4 View;
    float4x4 Proj;
    float4x4 MVP;
    float4 AmbientColor;
    };

    struct TMATERIAL
    {
    float4 Diffuse;
    float4 Ambient;
    float4 Specular;
    float4 Emissive;
    float Power;
    float Opacity;
    bool IsShaded;
    bool AlphaIsTransparency;
    };

    const int LightType_Directional = 3;
    const int LightType_Spot = 2;
    const int LightType_Point = 1;

    struct TLIGHT
    {
    bool IsEnabled;
    int Type;
    float3 Position;
    float3 Direction;
    float4 Diffuse;
    float4 Ambient;
    float4 Specular;

    float Falloff;
    float Range;
    bool ForceRange;

    float ConstantAtten;
    float LinearAtten;
    float QuadraticAtten;
    };

    TNEON NEON;
    TMATERIAL MAT;

    /* Vertex data */
    struct VS_INPUT
    {
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR0;
    float2 texCoord : TEXCOORD0;
    float2 texCoord2 : TEXCOORD1;
    float3 tangent : TANGENT;
    float3 bitangent : BINORMAL;
    };

    struct VS_INPUT_2D {
    float4 position : POSITION;
    float4 color : COLOR0;
    float2 texCoord : TEXCOORD0;
    };

    /* Textures */
    texture diffuseTex;
    bool hasDiffuseTex;

    texture specularTex;
    bool hasSpecularTex;

    texture normalTex;
    bool hasNormalTex;

    texture dispTex;
    bool hasDispTex;


    /* VS proxy */
    struct VS_PROXY {
    float4 position : POSITION;
    float2 texCoord : TEXCOORD0;
    };

    VS_PROXY VS_ProxyPass(VS_INPUT IN)
    {
    VS_PROXY OUT;

    OUT.position = float4(IN.position, 1.0f);
    OUT.texCoord = IN.texCoord;

    return OUT;
    }

    /* passthrough 3D vertex */
    struct VS_OUTPUT_3D {
        float4 position : POSITION;
        float4 worldPos : TEXCOORD2;
        float4 color : COLOR0;
        float2 texCoord : TEXCOORD0;
        float3 normal : NORMAL;
    };

    VS_OUTPUT_3D VS_Main3D(VS_INPUT IN)
    {
        VS_OUTPUT_3D OUT;
        OUT.position = mul(float4(IN.position, 1.0f), NEON.MVP);
        OUT.worldPos = mul(float4(IN.position, 1.0f), NEON.World);
        OUT.normal = mul(IN.normal, NEON.World);
        OUT.color = IN.color;
        OUT.texCoord = IN.texCoord;

        return OUT;
    }

    /* Light attenuation functions */

    // https://gamedev.stackexchange.com/a/56934
    float CalcLightAttenD3D(float distance, TLIGHT light)
    {
        float result = light.ConstantAtten;
        float range = light.Range;
        float b = light.LinearAtten;

        if (!light.ForceRange)
        {
            range = sqrt(1.0 / (light.LinearAtten * 0.01));
        }
        else
        {
            b = 1.0 / (light.Range * light.Range * 0.01);
        }

        if (distance < range)
        {
            result += b * distance + light.QuadraticAtten * (distance * distance);
        }
        else return 0.0;

        if (result == 0.0)
            return 0.0;

        return 1.0 / result;
    }

    float CalcLightAttenLinear(float distance, TLIGHT light)
    {
        float att = clamp(1.0 - distance / light.Range, 0.0, 1.0);
        return att * att * light.LinearAtten;
    }

    float CalcLightAttenQuadratic(float distance, TLIGHT light)
    {
        float att = clamp(1.0 - (distance * distance) / (light.Range * light.Range), 0.0, 1.0);
        return att * att * light.QuadraticAtten;
    }

    /* shadowmap write pass */

    void VS_ShadowPass(VS_INPUT IN, out float4 pos : POSITION, out float2 depth : TEXCOORD0)
    {
        pos = mul(float4(IN.position, 1.0f), NEON.MVP);
        depth.xy = pos.zw;
    }

    float4 PS_ShadowPass(float2 depth : TEXCOORD0) : COLOR
    {
        float dp = depth.x / depth.y;

        float dx = ddx(dp);
        float dy = ddy(dp);
        float m2 = dp * dp + 0.25 * (dx * dx + dy * dy);
        return float4(dp, m2, 0, 0);
    }

    technique ShadowGather {
        pass {
            VertexShader = compile vs_3_0 VS_ShadowPass();
            PixelShader = compile ps_3_0 PS_ShadowPass();
        }
    };

    /* shadow setup */

    texture2D shadowTex;

    sampler2D shadowMap = sampler_state
    {
        Texture = <shadowTex>;
        MipFilter = Point;
        MagFilter = Point;
        MinFilter = Point;
        AddressU = Border;
        AddressV = Border;
        BorderColor = float4(0,0,0,0);
    };

    struct SHADOW
    {
        float4x4 view;
        float4x4 proj;
    };

    float shadowMapSize = 512.0;
    SHADOW shadow;

    float SHADOW_EPSILON = 0.005f;

    float4 CalcShadowPos(float4 pos)
    {
        pos = mul(pos, shadow.view);
        pos = mul(pos, shadow.proj);
        return pos;
    }

    float CalcShadowDepth(float4 pos)
    {
        return pos.z / pos.w;
    }

    float CalcShadowBias(float diffuse)
    {
        return max(0.05 * (1.0 - diffuse), SHADOW_EPSILON);
    }

    float2 CalcShadowCoord(float4 vPosLight)
    {
        float2 shadowCoord = 0.5f * vPosLight.xy / vPosLight.w + float2(0.5f, 0.5f);
        shadowCoord.y = 1.0 - shadowCoord.y;
        return shadowCoord;
    }

    float CalcShadowSimple(sampler2D shadowMap, float bias, float depth, float2 shadowCoord)
    {
        return step(tex2D(shadowMap, shadowCoord).x, depth + bias);
    }

    float CalcShadowPCF2x2(sampler2D shadowMap, float bias, float depth, float2 shadowCoord, float shadowMapSize)
    {
        float2 tpos = shadowMapSize * shadowCoord;
        float2 lerps = frac(tpos);

        float srcvals[4];
        srcvals[0] = CalcShadowSimple(shadowMap, bias, depth, shadowCoord);
        srcvals[1] = CalcShadowSimple(shadowMap, bias, depth, shadowCoord + float2(1.0 / shadowMapSize, 0.0));
        srcvals[2] = CalcShadowSimple(shadowMap, bias, depth, shadowCoord + float2(0.0, 1.0 / shadowMapSize));
        srcvals[3] = CalcShadowSimple(shadowMap, bias, depth, shadowCoord + float2(1.0 / shadowMapSize, 1.0 / shadowMapSize));

        return lerp(
            lerp(srcvals[0], srcvals[2], lerps.y),
            lerp(srcvals[1], srcvals[3], lerps.y),
            lerps.x
        );
    }

    float CalcShadowPCF(sampler2D shadowMap, float numSamples, float bias, float depth, float2 shadowCoord, float shadowMapSize)
    {
        float start = (numSamples - 1) / 2.0;
        float2 tpos = shadowCoord / shadowMapSize;
        float result = 0.0;

        for (float y = -start; y <= start; y += 1.0)
        {
            for (float x = -start; x <= start; x += 1.0)
            {
                float2 uv = float2(x, y) * tpos;
                result += CalcShadowPCF2x2(shadowMap, bias, depth, shadowCoord + uv, shadowMapSize);
            }
        }

        return result / (numSamples * numSamples);
    }

    float linstep(float low, float high, float v)
    {
        return clamp((v - low) / (high - low), 0.0, 1.0);
    }

    float CalcShadowVariance(sampler2D shadowMap, float bias, float depth, float2 shadowCoord, float minVariance, float lightReduction, float shadowMapSize)
    {
        float2 m = tex2D(shadowMap, shadowCoord).xy;

        float p = CalcShadowPCF2x2(shadowMap, bias, depth, shadowCoord, shadowMapSize);
        float variance = max(m.y - m.x * m.x, minVariance);

        float d = depth + bias - m.x;
        float pMax = linstep(lightReduction, 1.0, variance / (variance + d * d));

        return min(max(p / (d * d), 1 - pMax), 1.0);
    }

    float2 poissonDisk[4] = {
      float2(-0.94201624, -0.39906216),
      float2(0.94558609, -0.76890725),
      float2(-0.094184101, -0.92938870),
      float2(0.34495938, 0.29387760)
    };

    float ApplyPoissonSampling(float4 seed, float r, float amt, float spread, float2 shadowCoord, sampler2D shadowMap, float bias, float depth)
    {
        for (int i = 0; i < 4; i++)
        {
            seed.w = i;
            float dd = dot(seed, float4(12.9898, 78.233, 45.164, 94.673));
            float rnds = frac(sin(dd) * 43758.5453);
            int idx = int(16.0 * rnds) % 16;
            if (tex2D(shadowMap, shadowCoord + poissonDisk[idx] / spread).r < depth + bias)
                r -= amt;
        }

        return r;
    }

    float CalcShadowOcclusion(float4 worldPos, float diffuse, int shadowMethod)
    {
        float lamt = 1.0;

        float depth = CalcShadowDepth(worldPos);
        float2 shadowCoord = CalcShadowCoord(worldPos);
        float bias = CalcShadowBias(diffuse);

        if (depth > 0.0)
        {
            if (shadowMethod == 0) lamt *= CalcShadowPCF(shadowMap, 6, bias, depth, shadowCoord, shadowMapSize);
            if (shadowMethod == 1) lamt *= CalcShadowVariance(shadowMap, bias, depth, shadowCoord, 0.0002, 0.94, shadowMapSize);
            if (shadowMethod == 2) lamt *= CalcShadowSimple(shadowMap, bias, depth, shadowCoord);
        }
        
        return lamt;
    }
);

#undef M
