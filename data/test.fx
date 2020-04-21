struct TNEON
{
    float4x4 World;
    float4x4 InverseWorld;
    float4x4 View;
    float4x4 Proj;
    float4x4 MVP;
};

struct TMATERIAL
{
    float4 Diffuse; 
    float4 Ambient; 
    float4 Specular;
    float4 Emissive;
    float Power;
    float Opacity;
};

TNEON NEON;
TMATERIAL MAT;

float time;
float3 campos;

float4 globalAmbient;

texture diffuseMap;

sampler2D colorMap = sampler_state
{
	Texture = <diffuseMap>;
    MagFilter = Linear;
    MinFilter = Anisotropic;
    MipFilter = Linear;
    MaxAnisotropy = 16;
};

struct VS_INPUT
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD0;
    float4 color : COLOR0;
	float3 normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 position : POSITION;
    float2 texCoord : TEXCOORD0;
    float4 worldPos : TEXCOORD1;
    float3 normal : TEXCOORD2;
};

VS_OUTPUT VS_AmbientLighting(VS_INPUT IN)
{
    VS_OUTPUT OUT;

    OUT.position = mul(float4(IN.position, 1.0f), NEON.MVP);
    OUT.worldPos = mul(float4(IN.position, 1.0f), NEON.World);
    OUT.texCoord = IN.texCoord;
    OUT.normal = IN.normal;

    return OUT;
}

float4 PS_AmbientLighting(VS_OUTPUT IN) : COLOR
{
    return tex2D(colorMap, IN.texCoord);
}

VS_OUTPUT VS_PointLighting(VS_INPUT IN)
{
    VS_OUTPUT OUT;

    OUT.position = mul(float4(IN.position, 1.0f), NEON.MVP);
    OUT.worldPos = mul(float4(IN.position, 1.0f), NEON.World);
    OUT.texCoord = IN.texCoord;
    OUT.normal = mul(IN.normal, NEON.InverseWorld);

    return OUT;
}

float4 PS_PointLighting(VS_OUTPUT IN) : COLOR
{
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 n = normalize(IN.normal);
    float3 l = float3(0.0f, 0.0f, 0.0f);
    
    float atten = 0.0f;
    float nDotL = 0.0f;
    
    l = (campos - IN.worldPos) / 10.0f;
    atten = saturate(1.0f - dot(l, l));
    
    l = normalize(l);
    
    nDotL = saturate(dot(n, l));
    
    color += globalAmbient +
            (MAT.Diffuse * nDotL * atten);
                   
	return color * tex2D(colorMap, IN.texCoord);
}

technique AmbientLighting
{
    pass
    {
        VertexShader = compile vs_3_0 VS_AmbientLighting();
        PixelShader = compile ps_3_0 PS_AmbientLighting();
    }
}

technique PointLighting
{
    pass
    {
        VertexShader = compile vs_3_0 VS_PointLighting();
        PixelShader = compile ps_3_0 PS_PointLighting();
    }
}
