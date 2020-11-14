#include <neon>

TLIGHT bonfire;

sampler2D colorMap = sampler_state
{
	Texture = <diffuseTex>;
    Filter = MIN_MAG_MIP_POINT;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float4 worldPos : TEXCOORD2;
    float4 color : COLOR0;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL;
};

VS_OUTPUT VS_Main(VS_INPUT IN)
{
    VS_OUTPUT OUT;
    OUT.position = mul(float4(IN.position, 1.0f), NEON.MVP);
    OUT.worldPos = mul(float4(IN.position, 1.0f), NEON.World);
    OUT.normal = mul(IN.normal, NEON.World);
    OUT.color = IN.color;
    OUT.texCoord = IN.texCoord;

    return OUT;
}

float4 PS_Main(VS_OUTPUT IN) : COLOR
{
    float4 OUT;

    float3 n = normalize(IN.normal);
    float3 l = IN.worldPos - bonfire.Position;
    float dl = length(l); if (dl == 0.0) dl = 0.00001;
    float diffuse = saturate(dot(n, normalize(-l))) / dl;

    OUT = NEON.AmbientColor + MAT.Diffuse * normalize(bonfire.Diffuse) * diffuse;

    if (hasDiffuseTex)
        OUT *= tex2D(colorMap, IN.texCoord);

    return OUT;
}

technique Main
{
    pass Default {
        VertexShader = compile vs_3_0 VS_Main();
        PixelShader = compile ps_3_0 PS_Main();
    }
}
