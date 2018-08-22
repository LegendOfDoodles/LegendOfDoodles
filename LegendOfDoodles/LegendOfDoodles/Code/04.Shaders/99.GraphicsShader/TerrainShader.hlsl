#include "./Common.hlsl"
#include "./Light.hlsl"

///////////////////////////////////////////////////////////////////////////////////////////
//  다중 랜더 타겟 용  PS출력
struct PS_MULTIPLE_RENDER_TARGETS_OUTPUT
{
    float4 color : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 roughMetalFresnel : SV_TARGET2;
    float4 albedo : SV_TARGET3;
    float4 position : SV_TARGET4;
    float4 toonPower : SV_TARGET6;
};

///////////////////////////////////////////////////////////////////////////////////////////
//
struct FOWINFO
{
    int m_bFoW[256];
};
StructuredBuffer<FOWINFO> gFogOfWar : register(t12);

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

	int indexI = clamp((int)((output.position.x) / 41.4), 0, 243);//243
	int indexJ = clamp((int)((output.position.z) / 41.4), 0, 122);//122

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
    static float3x3 NormalRotationMat = 
	float3x3(
		1.f, 0.f, 0.f,
		0.f, 0.f, -1.f,
		0.f, 1.f, 0.f
	);

    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
    float3 normal = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnNormal)).xyz; // 노말 맵에서 해당하는 uv에 해당하는 노말 읽기
    normal = 2.0f * normal - 1.0f; // 노말을 -1에서 1사이의 값으로 변환

    output.normal = float4(mul(normal, NormalRotationMat), 1);
    output.color = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnDiffuse)) + gtxtTextures.Sample(wrapSampler, float3(input.uv, gnSpecular));
	
	int indexI = clamp((int)((input.positionW.x) / 41.4), 0, 243);//243
	int indexJ = clamp((int)((input.positionW.z) / 41.4), 0, 122);//122

	output.color += float4(-0.9f* (1-input.fow), -0.9f*(1-input.fow), -0.9f*(1-input.fow), 0);

    output.roughMetalFresnel = float4(gtxtTextures.Sample(wrapSampler, float3(input.uv, gnMix3Data)).rgb, 0);
    output.albedo = gMaterials.m_cAlbedo;
    output.position = input.positionW;
    output.toonPower = float4(1, 1, 1, 1);

    return output;
}
