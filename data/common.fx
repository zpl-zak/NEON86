/* NEON structures */
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
