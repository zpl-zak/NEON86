float SHADOW_EPSILON = 0.01f;

float2 CalcShadowCoord(float4 vPosLight)
{
    float2 shadowCoord = 0.5f * vPosLight.xy / vPosLight.w + float2(0.5f, 0.5f);
    shadowCoord.y = 1.0 - shadowCoord.y;
    return shadowCoord;
}

float CalcShadowSimple(sampler2D shadowMap, float bias, float depth, float2 shadowCoord)
{
    return tex2D(shadowMap, shadowCoord).r < depth + bias;
}

float CalcShadowPCF2x2(sampler2D shadowMap, float bias, float depth, float2 shadowCoord, float shadowMapSize)
{
    float2 tpos = shadowMapSize * shadowCoord;
    float2 lerps = frac(tpos); 

    float srcvals[4];
    srcvals[0] = (tex2D(shadowMap, shadowCoord) > depth + bias) ? 0.0f : 1.0f;
    srcvals[1] = (tex2D(shadowMap, shadowCoord + float2(1.0/shadowMapSize, 0.0)) > depth + bias) ? 0.0f : 1.0f;
    srcvals[2] = (tex2D(shadowMap, shadowCoord + float2(0.0, 1.0/shadowMapSize)) > depth + bias) ? 0.0f : 1.0f;
    srcvals[3] = (tex2D(shadowMap, shadowCoord + float2(1.0/shadowMapSize, 1.0/shadowMapSize)) > depth + bias) ? 0.0f : 1.0f;

    return lerp(
        lerp(srcvals[0], srcvals[1], lerps.x),
        lerp(srcvals[2], srcvals[3], lerps.x),
        lerps.y
    );
}

float2 poissonDisk[4] = {
  float2( -0.94201624, -0.39906216 ),
  float2( 0.94558609, -0.76890725 ),
  float2( -0.094184101, -0.92938870 ),
  float2( 0.34495938, 0.29387760 )
};

float ApplyPoissonSampling(float4 seed, float r, float amt, float spread, float2 shadowCoord, sampler2D shadowMap, float bias, float depth)
{
    for (int i=0; i<4; i++)
    {
        seed.w = i;
        float dd = dot(seed, float4(12.9898,78.233,45.164,94.673));
        float rnds = frac(sin(dd) * 43758.5453);
        int idx = int(16.0*rnds)%16;
        if (tex2D(shadowMap, shadowCoord + poissonDisk[idx]/spread).r < depth + bias)
            r -= amt;
    }

    return r;
}