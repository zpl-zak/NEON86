#include <neon>

float4x4 shadowView;
float4x4 shadowProj;

float shadowMapSize = 512.0;
int shadowMethod;
int noShadows;

float3 campos;

#include "fx/shadow.fx"

sampler2D diffuseMap = sampler_state
{
    Texture = <diffuseTex>;
    MipFilter = Linear;
    MagFilter = Linear;
    MinFilter = Linear;
};

texture2D shadowTex;

sampler2D shadowMap = sampler_state
{
    Texture = <shadowTex>;
    MipFilter = Point;
    MagFilter = Point;
    MinFilter = Point;
    AddressU = Border;
    AddressV = Border;
};

struct VS_OUTPUT
{
    float4 position : POSITION;
    float4 viewPos : TEXCOORD1;
    float4 lightViewPos : TEXCOORD2;
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

    OUT.lightViewPos = mul(float4(IN.position, 1.0f), NEON.World);
    OUT.lightViewPos = mul(OUT.lightViewPos, shadowView);
    OUT.lightViewPos = mul(OUT.lightViewPos, shadowProj);

    return OUT;
}

float4 PS_ScenePass(VS_OUTPUT IN) : COLOR
{
    float4 OUT;
    float4 n = normalize(IN.normal);
    float3 l = normalize(-sun.Direction);
    float3 v = normalize(IN.viewDir);
    float3 h = normalize(l+v);
    float4 vpl = IN.lightViewPos;
    float lamt = 1.0f;

    float2 shadowCoord = CalcShadowCoord(vpl);
    float diffuse = saturate(dot(n,l));
    float specular = saturate(dot(n,h));
    float power = (diffuse == 0.0f) ? 0.0f : pow(specular, MAT.Power);

    float bias = SHADOW_EPSILON*tan(acos(diffuse));
    bias = clamp(bias, 0.01, 0.3);

    if (!noShadows)
    {
        if (shadowMethod == 0) lamt = CalcShadowPCF(shadowMap, 6, bias, vpl.z/vpl.w, shadowCoord, shadowMapSize);
        if (shadowMethod == 1) lamt = CalcShadowVariance(shadowMap, bias, vpl.z/vpl.w, shadowCoord, 0.0002, 0.94);
        if (shadowMethod == 2) lamt = CalcShadowSimple(shadowMap, bias, vpl.z/vpl.w, shadowCoord);
    }

    OUT = NEON.AmbientColor
        + (sun.Diffuse * diffuse * lamt)
        + (sun.Diffuse * specular * power * lamt);

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