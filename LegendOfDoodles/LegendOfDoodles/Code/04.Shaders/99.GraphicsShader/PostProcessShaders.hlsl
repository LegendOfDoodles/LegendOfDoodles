#include "./Common.hlsl"
#include "./Light.hlsl"

Texture2D<float4> gtxtSceneBaseColor : register(t3); // Texture Diffuse + Specular
Texture2D<float4> gtxtSceneNormal : register(t4); // xyz: 노말, w: 라이팅 여부
Texture2D<float4> gtxtSceneRoughMetalFresnel : register(t5); // r:Roughness, g: Metallic, b:Fresnel, a: 외곽선 처리 여부
Texture2D<float4> gtxtSceneAlbedo : register(t6); // Material 기본 색상
Texture2D<float4> gtxtScenePosition : register(t7); // xyz: Position 정보
Texture2D<float4> gtxtSceneEmissive : register(t8); //Emissive 색상
Texture2D<float4> gtxtSceneToonPower : register(t9); // 툰 쉐이딩 적용 색상
Texture2D<float4> gtxtUVBuffer : register(t10); // xy: UV, z: 스케치 이펙트 처리 여부
Texture2D gtxtShadowBuffer : register(t11); // Shadow Map Position

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
float4 VSTextureToFullScreen(uint nVertexID : SV_VertexID) : SV_POSITION
{
    if (nVertexID == 0)
        return (float4(-1.0f, +1.0f, 0.0f, 1.0f));
    if (nVertexID == 1)
        return (float4(+1.0f, +1.0f, 0.0f, 1.0f));
    if (nVertexID == 2)
        return (float4(+1.0f, -1.0f, 0.0f, 1.0f));
    if (nVertexID == 3)
        return (float4(-1.0f, +1.0f, 0.0f, 1.0f));
    if (nVertexID == 4)
        return (float4(+1.0f, -1.0f, 0.0f, 1.0f));
    if (nVertexID == 5)
        return (float4(-1.0f, -1.0f, 0.0f, 1.0f));

    return (float4(0, 0, 0, 0));
}

float4 CalculateOutlineColor(int2 pos)
{
    float outlineCnt = 0;
    float4 color = float4(0, 0, 0, 1);

    for (float i = -OUTLINE_POWER; i <= OUTLINE_POWER; ++i)
    {
        outlineCnt += gtxtSceneRoughMetalFresnel[int2(pos.x - i, pos.y)].a;
        outlineCnt += gtxtSceneRoughMetalFresnel[int2(pos.x + i, pos.y)].a;
        outlineCnt += gtxtSceneRoughMetalFresnel[int2(pos.x, pos.y - i)].a;
        outlineCnt += gtxtSceneRoughMetalFresnel[int2(pos.x, pos.y + i)].a;
        outlineCnt += gtxtSceneRoughMetalFresnel[int2(pos.x - i, pos.y - i)].a * 0.5;
        outlineCnt += gtxtSceneRoughMetalFresnel[int2(pos.x - i, pos.y + i)].a * 0.5;
        outlineCnt += gtxtSceneRoughMetalFresnel[int2(pos.x + i, pos.y - i)].a * 0.5;
        outlineCnt += gtxtSceneRoughMetalFresnel[int2(pos.x + i, pos.y + i)].a * 0.5;
    }
    if (outlineCnt >= 2 && outlineCnt < 6)
    {
        color = 1;
    }

    return color;
}

float4 CalculateEmissiveColor(int2 pos)
{
    float4 emissiveColor = gtxtSceneEmissive[pos] * 0.1;
    float i;
    for (i = -EMISSIVE_POWER; i <= EMISSIVE_POWER; ++i)
    {
        emissiveColor += gtxtSceneEmissive[int2(pos.x - i, pos.y)] * 0.01;
        emissiveColor += gtxtSceneEmissive[int2(pos.x + i, pos.y)] * 0.01;
        emissiveColor += gtxtSceneEmissive[int2(pos.x, pos.y - i)] * 0.01;
        emissiveColor += gtxtSceneEmissive[int2(pos.x, pos.y + i)] * 0.01;
        emissiveColor += gtxtSceneEmissive[int2(pos.x - i, pos.y - i)] * 0.005;
        emissiveColor += gtxtSceneEmissive[int2(pos.x - i, pos.y + i)] * 0.005;
        emissiveColor += gtxtSceneEmissive[int2(pos.x + i, pos.y - i)] * 0.005;
        emissiveColor += gtxtSceneEmissive[int2(pos.x + i, pos.y + i)] * 0.005;
    }
    for (i = -EMISSIVE_POWER * 0.5; i <= EMISSIVE_POWER * 0.5; ++i)
    {
        emissiveColor += gtxtSceneEmissive[int2(pos.x - i, pos.y)] * 0.01;
        emissiveColor += gtxtSceneEmissive[int2(pos.x + i, pos.y)] * 0.01;
        emissiveColor += gtxtSceneEmissive[int2(pos.x, pos.y - i)] * 0.01;
        emissiveColor += gtxtSceneEmissive[int2(pos.x, pos.y + i)] * 0.01;
        emissiveColor += gtxtSceneEmissive[int2(pos.x - i, pos.y - i)] * 0.005;
        emissiveColor += gtxtSceneEmissive[int2(pos.x - i, pos.y + i)] * 0.005;
        emissiveColor += gtxtSceneEmissive[int2(pos.x + i, pos.y - i)] * 0.005;
        emissiveColor += gtxtSceneEmissive[int2(pos.x + i, pos.y + i)] * 0.005;
    }
    for (i = -EMISSIVE_POWER * 0.25; i <= EMISSIVE_POWER * 0.25; ++i)
    {
        emissiveColor += gtxtSceneEmissive[int2(pos.x - i, pos.y)] * 0.01;
        emissiveColor += gtxtSceneEmissive[int2(pos.x + i, pos.y)] * 0.01;
        emissiveColor += gtxtSceneEmissive[int2(pos.x, pos.y - i)] * 0.01;
        emissiveColor += gtxtSceneEmissive[int2(pos.x, pos.y + i)] * 0.01;
        emissiveColor += gtxtSceneEmissive[int2(pos.x - i, pos.y - i)] * 0.005;
        emissiveColor += gtxtSceneEmissive[int2(pos.x - i, pos.y + i)] * 0.005;
        emissiveColor += gtxtSceneEmissive[int2(pos.x + i, pos.y - i)] * 0.005;
        emissiveColor += gtxtSceneEmissive[int2(pos.x + i, pos.y + i)] * 0.005;
    }

    return clamp(emissiveColor, 0, 1);
}

float4 CalculateSketchEffect(float2 uv, float intensity)
{
    float3 sketch0 = gtxtTextures.Sample(wrapSampler, float3(uv, 0)).rgb;
    float3 sketch1 = gtxtTextures.Sample(wrapSampler, float3(uv, 1)).rgb;

    float3 overbright = max(0, intensity - 1.0);

    float3 weightsA = saturate((intensity * 6.0) + float3(-3, -4, -5));
    float3 weightsB = saturate((intensity * 6.0) + float3(-0, -1, -2));

    weightsA.xy -= weightsA.yz;
    weightsA.z -= weightsB.x;
    weightsB.xy -= weightsB.zy;

    sketch0 = sketch0 * weightsA;
    sketch1 = sketch1 * weightsB;

    float4 finalColor = float4(overbright + sketch0.r + sketch0.g + sketch0.b + sketch1.r + sketch1.g + sketch1.b, 1);

    return float4(0.9 - finalColor.r, 0.9 - finalColor.g, 0.9 - finalColor.b, 1);
}

float CalculateShadowFactor(float4 shadowPosH)
{
    // Complete projection by doing division by w.
	// 직각투영이라 할 필요 없음
    //shadowPosH.xyz /= shadowPosH.w;

    // Depth in NDC space.
    float depth = shadowPosH.z * 0.999;

    uint width, height, numMips;
    gtxtShadowBuffer.GetDimensions(0, width, height, numMips);

    // Texel size.
    float dx = 1.0f / (float) width;

    float percentLit = 0.0f;
    const float2 offsets[9] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
    };

    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += gtxtShadowBuffer.SampleCmpLevelZero(shadowSampler,
            shadowPosH.xy + offsets[i], depth).r;
    }
    
    return percentLit / 9.0;
}

float4 PSTextureToFullScreen(float4 position : SV_POSITION) : SV_Target
{
    float2 uv = position.xy;
    uv.x /= FRAME_BUFFER_WIDTH;
    uv.y /= FRAME_BUFFER_HEIGHT;

	// 색상 사용
    float4 normal = gtxtSceneNormal.Sample(wrapSampler, uv);
    float4 albedo = gtxtSceneAlbedo.Sample(wrapSampler, uv);
    float4 baseColor = gtxtSceneBaseColor.Sample(wrapSampler, uv);
    float4 roughMetalFresnel = gtxtSceneRoughMetalFresnel.Sample(wrapSampler, uv);

	// 외곽선 처리
    float4 outlineColor = CalculateOutlineColor(position.xy);

	// 라이팅 안하는 오브젝트
    if (normal.w == 0)
        return baseColor + outlineColor;

    float4 pos = gtxtScenePosition.Sample(wrapSampler, uv);

	// Emissive 처리
    float4 emissiveColor = CalculateEmissiveColor(position.xy);

	// 그림자
    float4 shadowPos = mul(float4(pos.xyz, 1), gmtxShadowViewProjTex);
    float shadowFactor = CalculateShadowFactor(shadowPos);
	
	// 환경 매핑 처리
    float3 viewDir = normalize(pos.xyz - gvCameraPosition);
    float3 viewReflectDir = reflect(viewDir, normal.xyz);
    float4 cubeMapColor = gtxtTextureCube.Sample(wrapSampler, viewReflectDir);
    float4 reflectColor = roughMetalFresnel.g * cubeMapColor + (1 - roughMetalFresnel.g) * baseColor;
    float4 totalReflectColor = (roughMetalFresnel.r * albedo + (1 - roughMetalFresnel.r) * reflectColor) * REFLECTION_POWER;

	// 라이트 처리
    float4 lightColor = Lighting(pos.xyz, normal.xyz, albedo, baseColor, roughMetalFresnel, shadowFactor);
	
	// 스케치 이펙트 처리
    float4 finalColor = (lightColor + totalReflectColor) * gtxtSceneToonPower.Sample(wrapSampler, uv);
    if (gtxtUVBuffer.Sample(wrapSampler, uv).z == 1)
    {
        float intensity = dot(finalColor.rgb, float3(0.2326, 0.7152, 0.0722));
        finalColor = finalColor * CalculateSketchEffect(gtxtUVBuffer.Sample(wrapSampler, uv).xy * 2, intensity);
    }

    return (finalColor + emissiveColor) + outlineColor;
}