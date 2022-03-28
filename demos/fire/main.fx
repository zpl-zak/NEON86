#include <neon>

TLIGHT bonfire;

sampler2D colorMap = sampler_state
{
	Texture = <diffuseTex>;
    Filter = MIN_MAG_MIP_LINEAR;
};

float4 PS_Main(VS_OUTPUT_3D IN) : COLOR
{
    float4 OUT;

    float3 n = normalize(IN.normal);
    float3 l = bonfire.Position - IN.worldPos;
    float dl = length(l);
    float diffuse = saturate(dot(n, normalize(l))) * CalcLightAttenD3D(dl, bonfire);

    OUT = NEON.AmbientColor + saturate(MAT.Diffuse * bonfire.Diffuse * diffuse);

    if (hasDiffuseTex)
        OUT *= tex2D(colorMap, IN.texCoord);

    return OUT;
}

technique Main
{
    pass Default {
        VertexShader = compile vs_3_0 VS_Main3D();
        PixelShader = compile ps_3_0 PS_Main();
    }
}
