#include <neon>

float4x4 shadowView;
float4x4 shadowProj;

float shadowMapSize = 512.0;
int shadowMethod;

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
    float4 normal   : NORMAL;
};

TLIGHT sun;

VS_OUTPUT VS_ScenePass(VS_INPUT IN)
{
    VS_OUTPUT OUT;
    OUT.position = mul(float4(IN.position, 1.0f), NEON.MVP);
    OUT.viewPos = mul(float4(IN.position, 1.0f), NEON.WorldView);
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
    float4 s = tex2D(diffuseMap, IN.texCoord);
    float4 n = normalize(IN.normal);
    float3 l = normalize(-sun.Direction);
    float4 vpl = IN.lightViewPos;
    float lamt = 1.0f;

    float2 shadowCoord = CalcShadowCoord(vpl);
    float diffuse = saturate(dot(n,l));

    float bias = SHADOW_EPSILON*tan(acos(diffuse));
    bias = clamp(bias, 0, 0.01);
    
    if (shadowMethod == 0) lamt = CalcShadowPCF2x2(shadowMap, bias, vpl.z/vpl.w, shadowCoord, shadowMapSize);
    if (shadowMethod == 1) lamt = CalcShadowSimple(shadowMap, bias, vpl.z/vpl.w, shadowCoord);

    // lamt = ApplyPoissonSampling(vpl, lamt, 0.2, 700, shadowCoord, shadowMap, vpl.z/vpl.w);

    OUT = NEON.AmbientColor + sun.Diffuse * diffuse * lamt;

    if (hasDiffuseTex)
        OUT *= s;

    return OUT;
}

technique Scene {
    pass {
        VertexShader = compile vs_3_0 VS_ScenePass();
        PixelShader = compile ps_3_0 PS_ScenePass();
    }
};