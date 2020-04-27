struct VS_OUTPUT_2D {
    float4 position : POSITION;
    float4 color : COLOR0;
    float2 texCoord : TEXCOORD0;
};

texture sceneTex;
sampler2D sceneMap = sampler_state
{
    Texture = <sceneTex>;
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = MIRROR;
    AddressV = MIRROR;
    MaxAnisotropy = 16;
};

// Vertex shader, called per each vertex, here we perform transformations
// and calculate data that doesn't need per-pixel precision
VS_OUTPUT_2D VS_Copy(VS_INPUT_2D IN)
{
    VS_OUTPUT_2D OUT;
    OUT.position = IN.position;
    OUT.color = IN.color;
    OUT.texCoord = IN.texCoord;

    return OUT;
}

// Pixel shader, called per each fragment, here we calculate resulting color
// from data calculated in the Vertex shader and/or uniforms
float4 PS_Copy(VS_OUTPUT_2D IN) : COLOR
{
    IN.texCoord.y = 1 - IN.texCoord.y;
    float4 OUT = tex2D(sceneMap, IN.texCoord);
    
    return OUT;
}
