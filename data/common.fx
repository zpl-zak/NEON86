/* NEON structures */
struct TNEON
{
    float4x4 World;
    float4x4 WorldView;
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

/* Vertex data */
struct VS_INPUT
{
	float3 position : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR0;
	float2 texCoord : TEXCOORD0;
    float3 tangent : TANGENT;
};

/* Textures */
texture diffuseTex;
bool hasDiffuseTex;

texture specularTex;
bool hasSpecularTex;

texture normalTex;
bool hasNormalTex;
