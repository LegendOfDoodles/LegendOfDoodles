#include "./Common.hlsl"
#include "./Light.hlsl"

///////////////////////////////////////////////////////////////////////////////////////////
//  다중 랜더 타겟 용  PS출력
struct PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT
{
    float4 color : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 roughMetalFresnel : SV_TARGET2;
};

struct PS_MULTIPLE_RENDER_TARGETS_OUTPUT
{
    float4 color : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 roughMetalFresnel : SV_TARGET2;
    float4 albedo : SV_TARGET3;
    float4 position : SV_TARGET4;
    float4 toonPower : SV_TARGET6;
};

struct PS_MULTIPLE_RENDER_TARGETS_OUTPUT_UV
{
    float4 color : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 roughMetalFresnel : SV_TARGET2;
    float4 albedo : SV_TARGET3;
    float4 position : SV_TARGET4;
    float4 toonPower : SV_TARGET6;
    float4 uv : SV_TARGET7;
};

struct PS_MULTIPLE_RENDER_TARGETS_OUTPUT_EMISSIVE
{
    float4 color : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 roughMetalFresnel : SV_TARGET2;
    float4 albedo : SV_TARGET3;
    float4 position : SV_TARGET4;
    float4 emissive : SV_TARGET5;
    float4 toonPower : SV_TARGET6;
    float4 uv : SV_TARGET7;
};

struct PS_MULTIPLE_RENDER_TARGETS_OUTPUT_TOON
{
    float4 color : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 roughMetalFresnel : SV_TARGET2;
    float4 albedo : SV_TARGET3;
    float4 position : SV_TARGET4;
    float4 toonPower : SV_TARGET6;
    float4 uv : SV_TARGET7;
};

///////////////////////////////////////////////////////////////////////////////////////////
//
//정점 셰이더의 입력을 위한 구조체를 선언한다.
struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};


//정점 셰이더의 출력(픽셀 셰이더의 입력)을 위한 구조체를 선언한다.
struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

//정점 셰이더를 정의한다.
VS_OUTPUT VSDiffused(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.color = input.color;
    return (output);
}


//픽셀 셰이더를 정의한다.
PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT PSDiffused(VS_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT output;
    output.color = input.color;
    output.normal = float4(0, 0, 0, 0);
    output.roughMetalFresnel = float4(0, 0, 0, 0);

    return (output);
}

///////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_TEXTURED_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_TEXTURED_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input)
{
    VS_TEXTURED_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.uv = input.uv;

    return (output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT PSTextured(VS_TEXTURED_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT output;
    output.color = gtxtTexture.Sample(wrapSampler, input.uv);
    output.normal = float4(0, 0, 0, 0);
    output.roughMetalFresnel = float4(0, 0, 0, 0);

    return (output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT PSTexturedRepeat(VS_TEXTURED_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT output;
    float2 newUV;
    newUV.x = input.uv.x * 6;
    newUV.y = input.uv.y * 5;

    output.color = gtxtTexture.Sample(mirrorSampler, newUV);
    output.normal = float4(0, 0, 0, 0);
    output.roughMetalFresnel = float4(0, 0, 0, 0);

    return (output);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gauge Object 
struct VS_GAUGE_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_GAUGE_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VS_GAUGE_OUTPUT VSTexturedGauge(VS_GAUGE_INPUT input)
{
    VS_GAUGE_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGaugeObject), gmtxView), gmtxProjection);
    output.uv = input.uv;

    return (output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT PSTexturedNumber(VS_GAUGE_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT output;

    float2 newUV = input.uv;

    newUV.x = (input.uv.x / 10.f) + ((float) CurrentHP / 10.f);

    output.color = gtxtTexture.Sample(wrapSampler, newUV);
    output.normal = float4(0, 0, 0, 0);
    output.roughMetalFresnel = float4(0, 0, 0, 0);

    return (output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT PSTexturedGauge(VS_GAUGE_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT output;

    if (input.uv.x > CurrentHP)
        discard;

    output.color = gtxtTexture.Sample(wrapSampler, input.uv);
    output.normal = float4(0, 0, 0, 0);
    output.roughMetalFresnel = float4(0, 0, 0, 0);

    return (output);
}

// Icon Gague
PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT PSTexturedIconGauge(VS_GAUGE_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT output;

    if (1 - input.uv.y > CurrentHP)
        discard;

    output.color = gtxtTexture.Sample(wrapSampler, input.uv);
    output.normal = float4(0, 0, 0, 0);
    output.roughMetalFresnel = float4(0, 0, 0, 0);

    return (output);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_LIGHTING_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VS_LIGHTING_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
//	nointerpolation float3 normalW : NORMAL;
};

VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input)
{
    VS_LIGHTING_OUTPUT output;

    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);

    return (output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSLighting(VS_LIGHTING_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

    output.normal = float4(normalize(input.normalW), 1);
    output.color = gMaterials.m_cAlbedo;
    output.roughMetalFresnel = float4(1, 0, 1, 0);
    output.albedo = gMaterials.m_cAlbedo;
    output.position = float4(input.positionW, 0);
    output.toonPower = float4(1, 1, 1, 1);

    return (output);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_TEXTURED_LIGHTING_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
};

struct VS_TEXTURED_LIGHTING_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
	//	nointerpolation float3 normalW : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangentW : TANGENT;
    float fow : FOW;
};

struct FOWINFO
{
    int m_bFoW[256];
};
StructuredBuffer<FOWINFO> gFogOfWar : register(t12);


VS_TEXTURED_LIGHTING_OUTPUT VSTexturedLighting(VS_TEXTURED_LIGHTING_INPUT input)
{
    VS_TEXTURED_LIGHTING_OUTPUT output;

    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;
    output.tangentW = mul(input.tangent, (float3x3) gmtxGameObject);

    int indexI = clamp((int) ((output.positionW.x) / 41.4), 0, 243); //243
    int indexJ = clamp((int) ((output.positionW.z) / 41.4), 0, 122); //122
	
    output.fow = gFogOfWar[indexI].m_bFoW[indexJ];
    return (output);
}

// 나머지 오브젝트 그리기용
PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTexturedLightingDetail(VS_TEXTURED_LIGHTING_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
    float3 N = normalize(input.normalW);
    if (input.tangentW.x != 0 || input.tangentW.y != 0 || input.tangentW.z != 0)
    {
        float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
        float3 B = cross(N, T); // 노말과 탄젠트를 외적해서 바이 탄젠트(바이 노말)생성
        float3x3 TBN = float3x3(T, B, N); // 이를 바탕으로 TBN행렬 생성
        float3 normal = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnNormal)).xyz; // 노말 맵에서 해당하는 uv에 해당하는 노말 읽기
        normal = 2.0f * normal - 1.0f; // 노말을 -1에서 1사이의 값으로 변환
        N = mul(normal, TBN); // 노말을 TBN행렬로 변환
    }

    output.normal = float4(N, 1);
    output.color = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnDiffuse)) + gtxtTextures.Sample(wrapSampler, float3(input.uv, gnSpecular));
    output.color += float4(-0.9f * (1 - input.fow), -0.9f * (1 - input.fow), -0.9f * (1 - input.fow), 0);

	
    output.roughMetalFresnel = float4(gtxtTextures.Sample(wrapSampler, float3(input.uv, gnMix3Data)).rgb, 0);
    output.albedo = gMaterials.m_cAlbedo;
    output.position = float4(input.positionW, 0);
    output.toonPower = float4(1, 1, 1, 1);

    return output;
}

// 이미시브 오브젝트 처리용 -> 건물 처리
PS_MULTIPLE_RENDER_TARGETS_OUTPUT_EMISSIVE PSTexturedLightingEmissive(VS_TEXTURED_LIGHTING_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT_EMISSIVE output;
    float3 N = normalize(input.normalW);
    float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
    float3 B = cross(N, T); // 노말과 탄젠트를 외적해서 바이 탄젠트(바이 노말)생성
    float3x3 TBN = float3x3(T, B, N); // 이를 바탕으로 TBN행렬 생성
    float3 normal = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnNormal)).xyz; // 노말 맵에서 해당하는 uv에 해당하는 노말 읽기
    normal = 2.0f * normal - 1.0f; // 노말을 -1에서 1사이의 값으로 변환
    N = mul(normal, TBN); // 노말을 TBN행렬로 변환

    output.normal = float4(N, 1);
    output.color = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnDiffuse)) + gtxtTextures.Sample(wrapSampler, float3(input.uv, gnSpecular));
    output.roughMetalFresnel = float4(gtxtTextures.Sample(wrapSampler, float3(input.uv, gnMix3Data)).rgb, 0);
    output.albedo = gMaterials.m_cAlbedo;
    output.position = float4(input.positionW, 0);
    output.emissive = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnEmissive));
    output.uv = float4(frac(input.uv.x * TERRAIN_SIZE_WIDTH / FRAME_BUFFER_WIDTH), frac(input.uv.y * TERRAIN_SIZE_HEIGHT / FRAME_BUFFER_HEIGHT), 1, 0);
    output.toonPower = float4(1, 1, 1, 1);

    return output;
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT_UV PSThrowingObj(VS_TEXTURED_LIGHTING_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT_UV output;
    float3 N = normalize(input.normalW);
    float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
    float3 B = cross(N, T); // 노말과 탄젠트를 외적해서 바이 탄젠트(바이 노말)생성
    float3x3 TBN = float3x3(T, B, N); // 이를 바탕으로 TBN행렬 생성
    float3 normal = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnNormal)).xyz; // 노말 맵에서 해당하는 uv에 해당하는 노말 읽기
    normal = 2.0f * normal - 1.0f; // 노말을 -1에서 1사이의 값으로 변환
    N = mul(normal, TBN); // 노말을 TBN행렬로 변환

    output.normal = float4(N, 1);
    output.color = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnDiffuse)) + gtxtTextures.Sample(wrapSampler, float3(input.uv, gnSpecular));
    output.roughMetalFresnel = float4(gtxtTextures.Sample(wrapSampler, float3(input.uv, gnMix3Data)).rgb, 1);
    output.albedo = gMaterials.m_cAlbedo;
    output.position = float4(input.positionW, 0);
    output.uv = float4(frac(input.uv.x * TERRAIN_SIZE_WIDTH / FRAME_BUFFER_WIDTH), frac(input.uv.y * TERRAIN_SIZE_HEIGHT / FRAME_BUFFER_HEIGHT), 1, 0);
    output.toonPower = float4(1, 1, 1, 1);

    return output;
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT_EMISSIVE PSThrowingObjEmissive(VS_TEXTURED_LIGHTING_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT_EMISSIVE output;
    float3 N = normalize(input.normalW);
    float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
    float3 B = cross(N, T); // 노말과 탄젠트를 외적해서 바이 탄젠트(바이 노말)생성
    float3x3 TBN = float3x3(T, B, N); // 이를 바탕으로 TBN행렬 생성
    float3 normal = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnNormal)).xyz; // 노말 맵에서 해당하는 uv에 해당하는 노말 읽기
    normal = 2.0f * normal - 1.0f; // 노말을 -1에서 1사이의 값으로 변환
    N = mul(normal, TBN); // 노말을 TBN행렬로 변환

    output.normal = float4(N, 1);
    output.color = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnDiffuse)) + gtxtTextures.Sample(wrapSampler, float3(input.uv, gnSpecular));
    output.roughMetalFresnel = float4(gtxtTextures.Sample(wrapSampler, float3(input.uv, gnMix3Data)).rgb, 1);
    output.albedo = gMaterials.m_cAlbedo;
    output.position = float4(input.positionW, 0);
    output.emissive = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnEmissive));
    output.uv = float4(frac(input.uv.x * TERRAIN_SIZE_WIDTH / FRAME_BUFFER_WIDTH), frac(input.uv.y * TERRAIN_SIZE_HEIGHT / FRAME_BUFFER_HEIGHT), 1, 0);
    output.toonPower = float4(1, 1, 1, 1);

    return output;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// For Terrain
struct VS_TERRAIN_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_TERRAIN_OUTPUT
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    float fow : FOW;
};

VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
    VS_TERRAIN_OUTPUT output;

    output.position = mul(float4(input.position, 1.0f), gmtxGameObject);
    output.uv = input.uv;

    int indexI = clamp((int) ((output.position.x) / 41.4), 0, 243); //243
    int indexJ = clamp((int) ((output.position.z) / 41.4), 0, 122); //122

    output.fow = gFogOfWar[indexI].m_bFoW[indexJ];

    return (output);
}

struct PatchTess
{
    float EdgeTess[4] : SV_TessFactor;
    float InsideTess[2] : SV_InsideTessFactor;
};

PatchTess ConstantHSTerrain(InputPatch<VS_TERRAIN_OUTPUT, 4> patch, uint patchID : SV_PrimitiveID)
{
    PatchTess pt;

    pt.EdgeTess[0] = 32;
    pt.EdgeTess[1] = 32;
    pt.EdgeTess[2] = 32;
    pt.EdgeTess[3] = 32;

    pt.InsideTess[0] = 32;
    pt.InsideTess[1] = 32;

    return pt;
}

struct HS_TERRAIN_OUTPUT
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    float fow : FOW;
};

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHSTerrain")]
[maxtessfactor(32.0f)]
HS_TERRAIN_OUTPUT HSTerrain(InputPatch<VS_TERRAIN_OUTPUT, 4> P, uint i : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    HS_TERRAIN_OUTPUT output;
    output.position = P[i].position;
    output.uv = P[i].uv;
    output.fow = P[i].fow;
    return output;
}

struct DS_TERRAIN_OUTPUT
{
    float4 position : SV_POSITION;
    float4 positionW : POSITIONW;
    float2 uv : TEXCOORD;
    float fow : FOW;
};

[domain("quad")]
DS_TERRAIN_OUTPUT DSTerrain(PatchTess patchTess, float2 uv : SV_DomainLocation, const OutputPatch<HS_TERRAIN_OUTPUT, 4> quad)
{
    DS_TERRAIN_OUTPUT output;

    float4 v1 = lerp(quad[0].position, quad[1].position, 1 - uv.y);
    float4 v2 = lerp(quad[2].position, quad[3].position, 1 - uv.y);
    float4 p = lerp(v1, v2, 1 - uv.x);

    float2 uv1 = lerp(quad[0].uv, quad[1].uv, 1 - uv.y);
    float2 uv2 = lerp(quad[2].uv, quad[3].uv, 1 - uv.y);
    float2 uvResult = lerp(uv1, uv2, 1 - uv.x);

    float f1 = lerp(quad[0].fow, quad[1].fow, 1 - uv.y);
    float f2 = lerp(quad[2].fow, quad[3].fow, 1 - uv.y);
    float fResult = lerp(f1, f2, 1 - uv.x);

	// Displacement Mapping
    p.y += gtxtTextures.SampleLevel(wrapSampler, float3(uvResult, gnMix3Data), 0).a * 255 * 2;

    output.position = mul(mul(p, gmtxView), gmtxProjection);
    output.positionW = p;
    output.uv = uvResult;
    output.fow = fResult;

    return output;
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTerrain(DS_TERRAIN_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
    float3 normal = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnNormal)).xyz; // 노말 맵에서 해당하는 uv에 해당하는 노말 읽기
    normal = 2.0f * normal - 1.0f; // 노말을 -1에서 1사이의 값으로 변환

    output.normal = float4(normal, 1);
    output.color = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnDiffuse)) + gtxtTextures.Sample(wrapSampler, float3(input.uv, gnSpecular));
	
    int indexI = clamp((int) ((input.positionW.x) / 41.4), 0, 243); //243
    int indexJ = clamp((int) ((input.positionW.z) / 41.4), 0, 122); //122

    output.color += float4(-0.9f * (1 - input.fow), -0.9f * (1 - input.fow), -0.9f * (1 - input.fow), 0);

    output.roughMetalFresnel = float4(gtxtTextures.Sample(wrapSampler, float3(input.uv, gnMix3Data)).rgb, 0);
    output.albedo = gMaterials.m_cAlbedo;
    output.position = input.positionW;
    output.toonPower = float4(1, 1, 1, 1);

    return output;
}


struct VS_BONEINPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord0 : TEXCOORD;
    float3 boneWeights : WEIGHTS;
    uint4 boneIndices : BONEINDICES;
    float3 tangent : TANGENT;
};

struct VS_TEXTURED_LIGHTING_TOON_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
	//	nointerpolation float3 normalW : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangentW : TANGENT;
    float3 toonPower : TOONPOWER;
};

VS_TEXTURED_LIGHTING_TOON_OUTPUT VSBone(VS_BONEINPUT input)
{
    VS_TEXTURED_LIGHTING_TOON_OUTPUT output;

    float fWeights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    fWeights[0] = input.boneWeights.x;
    fWeights[1] = input.boneWeights.y;
    fWeights[2] = input.boneWeights.z;
    fWeights[3] = 1 - fWeights[0] - fWeights[1] - fWeights[2];
    float3 position = float3(0.0f, 0.0f, 0.0f);
    float3 normal = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 4; ++i)
    {
        position += fWeights[i] * mul(float4(input.position, 1.0f), gmtxBoneTransforms[input.boneIndices[i]]).xyz;
        normal += fWeights[i] * mul(input.normal, (float3x3) gmtxBoneTransforms[input.boneIndices[i]]);
    }
	/*
	output.texcoord0 = input.texcoord0;
	*/

    output.normalW = mul(normal, (float3x3) gmtxBoneWorld);
    output.uv = input.texcoord0;
    output.tangentW = mul(input.tangent, (float3x3) gmtxBoneWorld);

    output.positionW = mul(float4(position, 1.0f), gmtxBoneWorld).xyz;

    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);

    output.toonPower = dot(-gLights[0].m_vDirection, normalize(output.normalW));

    return (output);
}

// 움직이는 오브젝트 그리기용
PS_MULTIPLE_RENDER_TARGETS_OUTPUT_TOON PSBone(VS_TEXTURED_LIGHTING_TOON_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT_TOON output;

    float3 N = normalize(input.normalW);
    float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
    float3 B = cross(N, T); // 노말과 탄젠트를 외적해서 바이 탄젠트(바이 노말)생성
    float3x3 TBN = float3x3(T, B, N); // 이를 바탕으로 TBN행렬 생성
    float3 normal = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnNormal)).xyz; // 노말 맵에서 해당하는 uv에 해당하는 노말 읽기
    normal = 2.0f * normal - 1.0f; // 노말을 -1에서 1사이의 값으로 변환
    N = mul(normal, TBN); // 노말을 TBN행렬로 변환

    output.normal = float4(N, 1);
    output.color = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnDiffuse));
    output.roughMetalFresnel = float4(gMaterials.m_cRoughness, gMaterials.m_cMetalic, 1, 1);
    output.albedo = gMaterials.m_cAlbedo;
    output.position = float4(input.positionW, 0);
    output.toonPower = float4(floor(input.toonPower * 3) / 3.0f, 1);
    output.uv = float4(frac(input.uv.x * TERRAIN_SIZE_WIDTH / FRAME_BUFFER_WIDTH), frac(input.uv.y * TERRAIN_SIZE_HEIGHT / FRAME_BUFFER_HEIGHT), 1, 0);

    return output;
}