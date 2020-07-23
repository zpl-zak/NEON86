#include <neon>

/* Uniforms */
float3 campos;
float alphaValue;
float time;

TLIGHT light;

sampler2D colorMap = sampler_state
{
	Texture = <diffuseTex>;
    MagFilter = Linear;
    MinFilter = Anisotropic;
    MipFilter = Linear;
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

sampler2D waterMap = sampler_state
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

/* Vertex data */
struct VS_OUTPUT
{
    float4 position : POSITION;
    float2 texCoord : TEXCOORD0;
    float4 worldPos : POSITION2;
    float3 viewDir : POSITION3;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

/* Point TEST */
VS_OUTPUT VS_TerrainRender(VS_INPUT IN)
{
    VS_OUTPUT OUT;

    OUT.position = mul(float4(IN.position, 1.0f), NEON.MVP);
    OUT.worldPos = mul(float4(IN.position, 1.0f), NEON.World);
    OUT.viewDir = (campos - OUT.worldPos);
    OUT.texCoord = IN.texCoord;
    OUT.normal = mul(IN.normal, NEON.InverseWorld);
    OUT.tangent = mul(IN.tangent, NEON.InverseWorld);

    return OUT;
}

float CalcShininess(float3 n, float3 viewDir)
{
    float3 v = normalize(viewDir);
    n = normalize(n);
    float3 h = normalize(-light.Direction+v);

    return saturate(dot(n,h));
}

float4 CalcSunLight(VS_OUTPUT IN)
{
    float3 n = normalize(IN.normal);
    float3 l = -light.Direction;
    float3 v = normalize(IN.viewDir);
    float4 s = float4(0,0,0,0);

    l = normalize(l);
    float diffuse = saturate(dot(n, l));
    float3 h = normalize(l+v);
    float4 ss = tex2D(specularMap, IN.texCoord);
    s = (hasSpecularTex == true) ? ss : s;
    float specular = saturate(dot(n, h));

    if (ss.r>0)
    {
        specular = saturate(dot(float4(0,1,0,1), h))*0.99;
        specular = lerp(specular, 0, 1-ss.r);
    }

    float power = (diffuse == 0.0f) ? 0.0f : pow(specular, MAT.Power);

	return (MAT.Diffuse * light.Diffuse * 2.0f * diffuse)
            + (light.Diffuse * 4.0f * specular * power * s);
}

float4 PS_TerrainRender(VS_OUTPUT IN) : COLOR
{
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float3 n = normalize(IN.normal);

    if (hasNormalTex == true)
    {
        float4 nm = tex2D(normalMap, IN.texCoord);
        nm = (2.0f*nm) - 1.0f;
        IN.tangent = normalize(IN.tangent - dot(IN.tangent, n)*n);
        float3 bt = cross(n, IN.tangent);
        float3x3 tx = float3x3(IN.tangent, bt, n);
        IN.normal = normalize(mul(nm, tx));
    }

    float4 OUT = NEON.AmbientColor + CalcSunLight(IN);

    float4 b = tex2D(colorMap, IN.texCoord);
    float4 wa = tex2D(waterMap, IN.texCoord);
    float4 bl = float4(0.847, 0.909, 0.913,1);

    float p = lerp(bl, 0, 1-wa.r);
    bl *= p;

    OUT *= b + bl;
    OUT.a = alphaValue;
	return OUT;
}

/* Techniques */
technique TerrainRender
{
    pass
    {
        VertexShader = compile vs_3_0 VS_TerrainRender();
        PixelShader = compile ps_3_0 PS_TerrainRender();
    }
}
