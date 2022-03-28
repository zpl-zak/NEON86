#include <neon>

int shadowMethod;
int noShadows;

float3 campos;

sampler2D diffuseMap = sampler_state
{
    Texture = <diffuseTex>;
    MipFilter = Linear;
    MagFilter = Linear;
    MinFilter = Linear;
};

struct VS_OUTPUT
{
    float4 position : POSITION;
    float4 viewPos : TEXCOORD1;
    float4 worldPos : TEXCOORD2;
    float2 texCoord : TEXCOORD;
    float3 viewDir  : TEXCOORD3;
    float4 normal   : NORMAL;
};

TLIGHT sun;

VS_OUTPUT VS_ScenePass(VS_INPUT IN)
{
    VS_OUTPUT OUT;
    OUT.position = mul(float4(IN.position, 1.0f), NEON.MVP);
    OUT.viewPos = mul(float4(IN.position, 1.0f), NEON.WorldView);
    OUT.viewDir = (campos - mul(float4(IN.position, 1.0f), NEON.World));
    OUT.normal = mul(IN.normal, NEON.World);
    OUT.texCoord = IN.texCoord;
    OUT.worldPos = CalcShadowPos(mul(float4(IN.position, 1.0f), NEON.World));
    return OUT;
}

float4 PS_ScenePass(VS_OUTPUT IN) : COLOR
{
    float4 OUT;
    float4 n = normalize(IN.normal);
    float3 l = normalize(-sun.Direction);
    float3 v = normalize(IN.viewDir);
    float3 h = normalize(l+v);

    /* lighting calcs */
    float diffuse = saturate(dot(n,l));
    float specular = saturate(dot(n,h));
    float powerFactor = 0.12;
    float power = (diffuse == 0.0f) ? 0.0f : pow(specular, MAT.Power * powerFactor);

    float lamt = CalcShadowOcclusion(IN.worldPos, diffuse, shadowMethod);

    OUT = NEON.AmbientColor
        + (sun.Diffuse * diffuse * lamt)
        + (sun.Specular * specular * power * lamt);

    if (hasDiffuseTex)
        OUT *= tex2D(diffuseMap, IN.texCoord);

    return OUT;
}

technique Scene {
    pass {
        VertexShader = compile vs_3_0 VS_ScenePass();
        PixelShader = compile ps_3_0 PS_ScenePass();
    }
};
