#pragma once

#define _M(...) #__VA_ARGS__

static LPCSTR _shader_common = _M(
    /* NEON structures */
    struct TNEON
    {
        float4x4 World;
        float4x4 WorldView;
        float4x4 InverseWorld;
        float4x4 InverseWorldView;
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
        bool IsShaded;
    };

    const int LightType_Directional = 3;
    const int LightType_Spot = 2;
    const int LightType_Point = 1;

    struct TLIGHT
    {
        bool IsEnabled;
        int Type;
        float3 Position;
        float3 Direction;
        float4 Diffuse;
        float4 Ambient;
        float4 Specular;

        float Falloff;
        float Range;

        float ConstantAtten;
        float LinearAtten;
        float QuadraticAtten;
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
        float2 texCoord2 : TEXCOORD1;
        float3 tangent : TANGENT;
        float3 bitangent : BINORMAL;
    };

    struct VS_INPUT_2D {
        float4 position : POSITION;
        float4 color : COLOR0;
        float2 texCoord : TEXCOORD0;
    };

    /* Textures */
    texture diffuseTex;
    bool hasDiffuseTex;

    texture specularTex;
    bool hasSpecularTex;

    texture normalTex;
    bool hasNormalTex;

    texture dispTex;
    bool hasDispTex;

);

#undef _M