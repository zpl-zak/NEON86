#include "common.fx"

/* Uniforms */
float3 campos;
float4 globalAmbient;
float alphaValue;
float time;

float4 sunColor = float4(0.91f, 0.58f, 0.13f, 1.0f);
float3 sunDir = float3(4.0f, 3.0f, -5.0f);

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

/* Ambient */
VS_OUTPUT VS_AmbientLighting(VS_INPUT IN)
{
    VS_OUTPUT OUT;

    OUT.position = mul(float4(IN.position, 1.0f), NEON.MVP);
    OUT.worldPos = mul(float4(IN.position, 1.0f), NEON.World);
    OUT.viewDir = (campos - OUT.worldPos);
    OUT.texCoord = IN.texCoord;
    OUT.normal = IN.normal;
    OUT.tangent = IN.tangent;

    return OUT;
}

float4 PS_AmbientLighting(VS_OUTPUT IN) : COLOR
{
    float4 outc = tex2D(colorMap, IN.texCoord);
    outc.a = alphaValue;        
	return outc;
}

/* Point TEST */
VS_OUTPUT VS_PointLighting(VS_INPUT IN)
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

float4 CalcSunLight(VS_OUTPUT IN)
{
    float3 n = normalize(IN.normal);
    float3 l = sunDir;
    float3 v = normalize(IN.viewDir);
    float4 s = float4(0,0,0,0);
    
    l = normalize(l);    
    float diffuse = saturate(dot(n, l));
    float3 h = normalize(l+v);
    float4 ss = tex2D(specularMap, IN.texCoord);
    s = (hasSpecularTex == true) ? ss : s;
    float specular = saturate(dot(n, h));

    float power = (diffuse == 0.0f) ? 0.0f : pow(specular, MAT.Power);

	return (MAT.Diffuse * sunColor * 2.0f * diffuse)
            + (sunColor * 4.0f * specular * power * s);
}

float4 CalcPointLight(VS_OUTPUT IN)
{
    float3 n = normalize(IN.normal);
    float3 v = normalize(IN.viewDir);
    float4 s = float4(0.0f,0.0f,0.0f,0.0f);
    
    float3 l = (campos - IN.worldPos) / 10.0f;
    float atten = saturate(1.0f - dot(l, l));
    
    l = normalize(l);
    float3 h = normalize(l+v);
    s = (hasSpecularTex == true) ? tex2D(specularMap, IN.texCoord) : s;
    
    float diffuse = saturate(dot(n, l));
    float specular = saturate(dot(n, h));

    float power = (diffuse == 0.0f) ? 0.0f : pow(specular, MAT.Power);

	return (MAT.Diffuse * diffuse * atten) +
            (MAT.Specular * specular * power * atten * s);
}

float4 PS_PointLighting(VS_OUTPUT IN) : COLOR
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

    float4 OUT = globalAmbient + CalcPointLight(IN) + CalcSunLight(IN)/4;
    OUT.a = alphaValue;
    OUT += tex2D(colorMap, IN.texCoord);
	return OUT;
}

/* Techniques */
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
