#include "common.fx"

/* Uniforms */
float3 campos;
float4 globalAmbient;
float alphaValue;
float time;
bool enableFresnel;

bool isSun;

float4 sunColor = float4(0.91f, 0.58f, 0.13f, 1.0f);
float3 sunDir = float3(4.0f, 3.0f, -5.0f);
float3 sunPos = float3(0,0,0);

sampler2D colorMap = sampler_state
{
	Texture = <diffuseTex>;
    MagFilter = Anisotropic;
    MinFilter = Anisotropic;
    MipFilter = Anisotropic;
    MaxAnisotropy = 16;
};

sampler2D specularMap = sampler_state
{
	Texture = <specularTex>;
    MagFilter = Linear;
    MinFilter = Anisotropic;
    MipFilter = Linear;
    MaxAnisotropy = 16;
};

sampler2D normalMap = sampler_state
{
	Texture = <normalTex>;
    MagFilter = Linear;
    MinFilter = Anisotropic;
    MipFilter = Linear;
    MaxAnisotropy = 16;
};

sampler2D dispMap = sampler_state
{
	Texture = <dispTex>;
    MagFilter = Linear;
    MinFilter = Anisotropic;
    MipFilter = Linear;
    MaxAnisotropy = 16;
};

/* Vertex data */
struct VS_OUTPUT
{
    float4 position : POSITION;
    float2 texCoord : TEXCOORD0;
    float4 worldPos : COLOR2;
    float3 viewDir : TEXCOORD3;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BINORMAL;
    float3x3 tbn : TEXCOORD4;
};

/* Point TEST */
VS_OUTPUT VS_Main(VS_INPUT IN)
{
    VS_OUTPUT OUT;

    OUT.position = mul(float4(IN.position, 1.0f), NEON.MVP);
    OUT.worldPos = mul(float4(IN.position, 1.0f), NEON.World);
    OUT.viewDir = (campos - OUT.worldPos);
    OUT.texCoord = IN.texCoord;
    OUT.normal = mul(IN.normal, NEON.World);
    OUT.tangent = mul(IN.tangent, NEON.World);
    OUT.bitangent = mul(IN.bitangent, NEON.World);

    OUT.tbn = float3x3(OUT.tangent, OUT.bitangent, OUT.normal);

    return OUT;
}

float4 Sunlight(VS_OUTPUT IN)
{
    float3 n = normalize(IN.normal);
    float3 v = normalize(IN.viewDir);
    float4 s = float4(0.0f,0.0f,0.0f,0.0f);
    
    float3 l = normalize(sunPos - IN.worldPos);
    float3 h = normalize(l+v);
    
    s = (hasSpecularTex == true) ? tex2D(specularMap, IN.texCoord) : s;
    
    float diffuse = saturate(dot(n, l));
    float specular = saturate(dot(n, h));

    float power = (diffuse == 0.0f) ? 0.0f : pow(specular, MAT.Power);

	return (sunColor * MAT.Diffuse * diffuse) +
            (sunColor * MAT.Specular * specular * power * s);
}

float2 WarpUV(float2 uv)
{
    float2 uvc = 0.5*uv - 1;
    float2 l = length(uvc);
    float dp = 0.003;
    float ws = 100;
    float tm = 1;

    return uv + (uvc/l)*cos(l*ws-time*tm)*dp 
              + (float2(-uvc.y, uvc.x)/l)*sin(l*ws-time*tm)*dp;
}

float4 PS_Main(VS_OUTPUT IN) : COLOR
{
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);    

    if (isSun)
    {
        IN.texCoord = WarpUV(IN.texCoord);
    }

    if (hasNormalTex)
    {        
        float4 nm = tex2D(normalMap, IN.texCoord);
        nm = (2.0f*nm) - 1.0f;
        IN.normal = normalize(mul(nm, IN.tbn));
    }

    float4 OUT = float4(1,1,1,1);

    if (hasDiffuseTex)
    {
        OUT = tex2D(colorMap, IN.texCoord);
    }
    
    if (!isSun)
    {
        OUT *= globalAmbient + Sunlight(IN);
    }

    OUT.a = alphaValue;
	return OUT;
}

/* Techniques */
technique Main
{
    pass
    {
        VertexShader = compile vs_3_0 VS_Main();
        PixelShader = compile ps_3_0 PS_Main();
    }
}
