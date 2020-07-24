float SHADOW_EPSILON = 0.005f;

float2 CalcShadowCoord(float4 vPosLight)
{
    float2 shadowCoord = 0.5f * vPosLight.xy / vPosLight.w + float2(0.5f, 0.5f);
    shadowCoord.y = 1.0 - shadowCoord.y;
    return shadowCoord;
}

float CalcShadowSimple(sampler2D shadowMap, float bias, float depth, float2 shadowCoord)
{
    return step(tex2D(shadowMap, shadowCoord).x, depth+bias);
}

float CalcShadowPCF2x2(sampler2D shadowMap, float bias, float depth, float2 shadowCoord, float shadowMapSize)
{
    float2 tpos = shadowMapSize * shadowCoord;
    float2 lerps = frac(tpos);

    float srcvals[4];
    srcvals[0] = CalcShadowSimple(shadowMap, bias, depth, shadowCoord);
    srcvals[1] = CalcShadowSimple(shadowMap, bias, depth, shadowCoord + float2(1.0/shadowMapSize, 0.0));
    srcvals[2] = CalcShadowSimple(shadowMap, bias, depth, shadowCoord + float2(0.0, 1.0/shadowMapSize));
    srcvals[3] = CalcShadowSimple(shadowMap, bias, depth, shadowCoord + float2(1.0/shadowMapSize, 1.0/shadowMapSize));

    return lerp(
        lerp(srcvals[0], srcvals[2], lerps.y),
        lerp(srcvals[1], srcvals[3], lerps.y),
        lerps.x
    );
}

float CalcShadowPCF(sampler2D shadowMap, float numSamples, float bias, float depth, float2 shadowCoord, float shadowMapSize)
{
    float start = (numSamples-1)/2.0;
    float2 tpos = shadowCoord/shadowMapSize;
    float result = 0.0;

    for (float y = -start; y <= start; y += 1.0)
    {
        for (float x = -start; x <= start; x += 1.0)
        {
            float2 uv = float2(x,y)*tpos;
            result += CalcShadowPCF2x2(shadowMap, bias, depth, shadowCoord+uv, shadowMapSize);
        }
    }

    return result/(numSamples*numSamples);
}

float linstep(float low, float high, float v)
{
    return clamp((v-low)/(high-low), 0.0, 1.0);
}

float CalcShadowVariance(sampler2D shadowMap, float bias, float depth, float2 shadowCoord, float minVariance, float lightReduction)
{
    float2 m = tex2D(shadowMap, shadowCoord).xy;

    float p = step(m.x, depth + bias);
    float variance = max(m.y - m.x * m.x, minVariance);

    float d = depth + bias - m.x;
    float pMax = linstep(lightReduction, 1.0, variance / (variance + d*d));

    return min(max(p, 1-pMax), 1.0);
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