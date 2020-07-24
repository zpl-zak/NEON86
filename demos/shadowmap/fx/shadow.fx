float shadowEpsilon = 0.01f;

float2 CalcShadowCoord(float4 vPosLight)
{
    float2 shadowCoord = 0.5f * vPosLight.xy / vPosLight.w + float2(0.5f, 0.5f);
    shadowCoord.y = 1.0 - shadowCoord.y;
    return shadowCoord;
}

float CalcShadowSimple(sampler2D shadowMap, float depth, float2 shadowCoord)
{
    return tex2D(shadowMap, shadowCoord).r < depth + shadowEpsilon;
}

float CalcShadowPCF2x2(sampler2D shadowMap, float depth, float2 shadowCoord, float shadowMapSize)
{
    float2 tpos = shadowMapSize * shadowCoord;
    float2 lerps = frac(tpos); 

    float srcvals[4];
    srcvals[0] = (tex2D(shadowMap, shadowCoord) > depth + shadowEpsilon) ? 0.0f : 1.0f;
    srcvals[1] = (tex2D(shadowMap, shadowCoord + float2(1.0/shadowMapSize, 0.0)) > depth + shadowEpsilon) ? 0.0f : 1.0f;
    srcvals[2] = (tex2D(shadowMap, shadowCoord + float2(0.0, 1.0/shadowMapSize)) > depth + shadowEpsilon) ? 0.0f : 1.0f;
    srcvals[3] = (tex2D(shadowMap, shadowCoord + float2(1.0/shadowMapSize, 1.0/shadowMapSize)) > depth + shadowEpsilon) ? 0.0f : 1.0f;

    return lerp(
        lerp(srcvals[0], srcvals[1], lerps.x),
        lerp(srcvals[2], srcvals[3], lerps.x),
        lerps.y
    );
}
