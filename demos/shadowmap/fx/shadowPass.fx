#include <neon>

void VS_ShadowPass(VS_INPUT IN, out float4 pos : POSITION, out float2 depth : TEXCOORD0)
{
    pos = mul(float4(IN.position, 1.0f), NEON.MVP);
    depth.xy = pos.zw;
}

float4 PS_ShadowPass(float2 depth : TEXCOORD0) : COLOR
{
    return depth.x / depth.y;
}

technique Shadow {
    pass {
        VertexShader = compile vs_3_0 VS_ShadowPass();
        PixelShader = compile ps_3_0 PS_ShadowPass();
    }
};
