// This includes the system header file provided by the engine.
// It contains all the necessary data to perform shading.
#include <neon>

// Global variables / uniforms
// These can be set up from Lua as well
float time;
float4 ambience;

// Our global sampler state for the diffuse texture
sampler2D colorMap = sampler_state
{
	Texture = <diffuseTex>;
    MipFilter = ANISOTROPIC;
    MinFilter = ANISOTROPIC;
    MagFilter = ANISOTROPIC;
    MaxAnisotropy = 16;
};

sampler2D glowMap = sampler_state
{
	Texture = <specularTex>;
    Filter = MIN_MAG_MIP_POINT;
    MaxAnisotropy = 16;
};

TLIGHT lights[9];

// Pipeline structure used to pass calculated per-vertex
// data to the pixel shader and the rasterizer
struct VS_OUTPUT {
    float4 position : POSITION;
    float4 worldPos : TEXCOORD2;
    float4 color : COLOR0;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL;
};

// Vertex shader, called per each vertex, here we perform transformations
// and calculate data that doesn't need per-pixel precision
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

// Pixel shader, called per each fragment, here we calculate resulting color
// from data calculated in the Vertex shader and/or uniforms
float4 PS_Main(VS_OUTPUT IN) : COLOR
{
    float4 OUT = float4(0,0,0,1);
    float4 brightPart = tex2D(colorMap, IN.texCoord);

    if (brightPart.r > 0.1f) {
        OUT = float4(1, 0.4, 0.8,1);
    } else {
        float3 n = normalize(IN.normal);
        for (int i=1; i<9; i++) {
            if (!lights[i].IsEnabled)
                continue;

            
        }
    }

    return OUT;
}

// Our rendering technique, each technique consists of single/multiple passes
// These passes are emitted from the Lua side
technique Main
{
    // Our render pass, passes consist of various shaders and can be named.
    // Multiple passes can be used to produce various image effects, such as
    // shadowing, ambient occlusion or multi-pass lighting
    pass Default {
        // [ShaderType] = compile [ShaderModel] [EntryPoint]
        VertexShader = compile vs_3_0 VS_Main();
        PixelShader = compile ps_3_0 PS_Main();
    }
}
