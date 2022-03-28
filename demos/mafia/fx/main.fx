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
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
};

TLIGHT sun;

#define NUM_LIGHTS 8
TLIGHT lights[NUM_LIGHTS];

VS_OUTPUT VS_LitPass(VS_INPUT IN)
{
    VS_OUTPUT OUT;

    OUT.position = mul(float4(IN.position, 1.0f), NEON.MVP);
    OUT.worldPos = mul(IN.position, NEON.World);
    OUT.normal = mul(IN.normal, NEON.World);
    OUT.texCoord = IN.texCoord;

    return OUT;
}

float4 PS_LitPass(VS_OUTPUT IN) : COLOR
{
    float4 OUT = float4(0,0,0,1);
    float3 n = normalize(IN.normal);

    OUT = NEON.AmbientColor;

    // sun pass
    {
        float3 l = normalize(-sun.Direction);
        float d = saturate(dot(n,l));
        OUT += sun.Diffuse * MAT.Diffuse * d;
    }

    // points pass
    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        TLIGHT lit = lights[i];
        float3 l = lit.Position - IN.worldPos;
        float d = saturate(dot(n,normalize(l)));
        float dist = length(l);
        float atten = 1.0f / (1.0 + 0.005 * dist + 0.0075 * (dist*dist));
        OUT += lit.Diffuse * MAT.Diffuse * d * atten * 2;
    }

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