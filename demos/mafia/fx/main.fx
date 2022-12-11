#include <neon>

sampler2D colorMap = sampler_state
{
    Texture = <diffuseTex>;
    MipFilter = ANISOTROPIC;
    MinFilter = ANISOTROPIC;
    MagFilter = ANISOTROPIC;
    MaxAnisotropy = 16;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float3 worldPos : TEXCOORD2;
    float3 viewDir : TEXCOORD3;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

TLIGHT sun;

#define NUM_LIGHTS 16
TLIGHT lights[NUM_LIGHTS];
int numLights;

float3 campos;

VS_OUTPUT VS_LitPass(VS_INPUT IN)
{
    VS_OUTPUT OUT;

    OUT.position = mul(float4(IN.position, 1.0f), NEON.MVP);
    OUT.worldPos = mul(IN.position, NEON.World);
    OUT.normal = mul(IN.normal, NEON.World);
    OUT.viewDir = (campos - mul(float4(IN.position, 1.0f), NEON.World));
    OUT.texCoord = IN.texCoord;
    OUT.color = NEON.AmbientColor;
    float3 n = normalize(OUT.normal);

    // sun pass
    {
        float3 l = normalize(-sun.Direction);
        float d = saturate(dot(n,l));
        OUT.color += sun.Diffuse * MAT.Diffuse * d;
    }

    // points pass
    for (int i = 0; i < numLights; i++)
    {
        TLIGHT lit = lights[i];
        float3 l = lit.Position - OUT.worldPos;
        float d = saturate(dot(n,normalize(l)));
        float dist = length(l);
        float atten = CalcLightAttenQuadratic(dist, lit);
        OUT.color += lit.Diffuse * MAT.Diffuse * d * atten;
    }

    return OUT;
}

float4 PS_LitPass(VS_OUTPUT IN) : COLOR
{
    float4 OUT = IN.color;

    float3 n = normalize(IN.normal);
    float3 l = normalize(-sun.Direction);
    float3 v = normalize(IN.viewDir);
    float3 h = normalize(l+v);

    /* lighting calcs */
    float diffuse = saturate(dot(n,l));
    float specular = saturate(dot(n,h));
    float powerFactor = 0.80;
    float power = diffuse == 0.0f ? 0.0f : pow(specular, MAT.Power * powerFactor);

    OUT += (sun.Specular * specular * power);

    if (hasDiffuseTex)
        OUT *= tex2D(colorMap, IN.texCoord);
    return OUT;
}

technique Lit {
    pass {
        VertexShader = compile vs_3_0 VS_LitPass();
        PixelShader = compile ps_3_0 PS_LitPass();
    }
};
