#include <neon>

float3 inverseTexSize;
float fxaaReduceMul;
float fxaaReduceMin;
float fxaaSpanMax;

texture2D srcTex;

sampler2D srcMap = sampler_state
{
    Texture = <srcTex>;
    MagFilter = Linear;
    MinFilter = Anisotropic;
    MipFilter = Linear;
    MaxAnisotropy = 16;
};

struct VS_OUTPUT
{
    float4 position : POSITION;
    float2 texCoord : TEXCOORD0;
};

VS_OUTPUT VS_FXAA(VS_INPUT IN)
{
    VS_OUTPUT OUT;

    OUT.position = float4(IN.position, 1.0f);
    OUT.texCoord = IN.texCoord;
    
    return OUT;
}

float4 PS_FXAA(VS_OUTPUT IN) : COLOR
{
    float2 uvoffs = inverseTexSize.xy;
    float2 uv = IN.texCoord;

    float4 lumaTexTL = tex2D(srcMap, uv + (float2(-1.0f, -1.0f) * uvoffs));
    float4 lumaTexTR = tex2D(srcMap, uv + (float2(1.0f, -1.0f) * uvoffs));
    float4 lumaTexBL = tex2D(srcMap, uv + (float2(-1.0f, 1.0f) * uvoffs));
    float4 lumaTexBR = tex2D(srcMap, uv + (float2(1.0f, 1.0f) * uvoffs));
    float4 lumaTexM = tex2D(srcMap, uv);

    float3 luma = float3(0.209f, 0.587f, 0.114f);
    float lumaTL = dot(luma, lumaTexTL.xyz);
    float lumaTR = dot(luma, lumaTexTR.xyz);
    float lumaBL = dot(luma, lumaTexBL.xyz);
    float lumaBR = dot(luma, lumaTexBR.xyz);
    float lumaM = dot(luma, lumaTexM.xyz);

    float2 dir;
    dir.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));
    dir.y = ((lumaTL + lumaBL) - (lumaTR + lumaBR));

    float dirR = max((lumaTL + lumaTR + lumaBL + lumaBR) * (fxaaReduceMul * 0.25f), fxaaReduceMin);
    float invDirAdj = 1.0f/(min(abs(dir.x), abs(dir.y)) + dirR);

    dir = min(float2(fxaaSpanMax, fxaaSpanMax), max(float2(-fxaaSpanMax, -fxaaSpanMax), dir * invDirAdj));

    dir.x = dir.x * step(1.0f, abs(dir.x));
    dir.y = dir.y * step(1.0f, abs(dir.y));

    dir = dir * uvoffs;

    float4 r1 = 0.5f*(
        tex2D(srcMap, uv + (dir * float2(1.0f/3.0f - 0.5f, 1.0f/3.0f - 0.5f))) +
        tex2D(srcMap, uv + (dir * float2(2.0f/3.0f - 0.5f, 2.0f/3.0f - 0.5f)))
    );
    
    float4 r2 = r1 * 0.5f + (1.0f/4.0f) * (
        tex2D(srcMap, uv + (dir * float2(-0.5f, -0.5f))) +
        tex2D(srcMap, uv + (dir * float2(1.0f - 0.5f, 1.0f - 0.5f)))
    );

    float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
    float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));
    float lumaR2 = dot(luma, r2);

    if (lumaR2 < lumaMin || lumaR2 > lumaMax)
        return float4(r1.rgb, 1.0f);
    else
        return float4(r2.rgb, 1.0f);
}

technique FXAA
{
    pass
    {
        VertexShader = compile vs_3_0 VS_FXAA();
        PixelShader = compile ps_3_0 PS_FXAA();
    }
}