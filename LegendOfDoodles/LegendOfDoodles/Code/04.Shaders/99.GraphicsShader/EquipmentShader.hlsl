#include "./Common.hlsl"
#include "./Light.hlsl"

///////////////////////////////////////////////////////////////////////////////////////////
//  다중 랜더 타겟 용  PS출력
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
struct VS_EQUIPMENTINPUT {
	float3 position : POSITION;
	float3 normal: NORMAL;
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

VS_TEXTURED_LIGHTING_TOON_OUTPUT VSEquipment(VS_EQUIPMENTINPUT input)
{
    VS_TEXTURED_LIGHTING_TOON_OUTPUT output;

	float fWeights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	fWeights[0] = input.boneWeights.x;
	fWeights[1] = input.boneWeights.y;
	fWeights[2] = input.boneWeights.z;
	fWeights[3] = 1 - fWeights[0] - fWeights[1] - fWeights[2];
	float3 position = float3(0.0f, 0.0f, 0.0f);
	float3 normal = float3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 4; ++i) {
		position += fWeights[i] * mul(float4(input.position, 1.0f), gmtxBoneTransforms[input.boneIndices[i]]).xyz;
		normal += fWeights[i] * mul(input.normal, (float3x3)gmtxBoneTransforms[input.boneIndices[i]]);
	}
	/*
	output.texcoord0 = input.texcoord0;
	*/

	output.normalW = mul(normal, (float3x3)gmtxBoneWorld);
	output.uv = input.texcoord0;
	output.tangentW = mul(input.tangent, (float3x3)gmtxBoneWorld);

	output.positionW = mul(float4(position, 1.0f), gmtxBoneWorld).xyz;

	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);

    output.toonPower = dot(-gLights[0].m_vDirection, normalize(output.normalW));

	return(output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT_TOON PSEquipment(VS_TEXTURED_LIGHTING_TOON_OUTPUT input)
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
	output.color = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnDiffuse)) + gtxtTextures.Sample(wrapSampler, float3(input.uv, gnSpecular));
    output.roughMetalFresnel = float4(gtxtTextures.Sample(wrapSampler, float3(input.uv, gnMix3Data)).rgb, 1);
	output.albedo = gMaterials.m_cAlbedo;
	output.position = float4(input.positionW, 0);
	output.toonPower = float4(floor(input.toonPower * 3) / 3.0f, 1);
	output.uv = float4(frac(input.uv.x * TERRAIN_SIZE_WIDTH / FRAME_BUFFER_WIDTH), frac(input.uv.y * TERRAIN_SIZE_HEIGHT / FRAME_BUFFER_HEIGHT), 0, 0);

	return output;
}


PS_MULTIPLE_RENDER_TARGETS_OUTPUT_EMISSIVE PSEquipmentEmissive(VS_TEXTURED_LIGHTING_TOON_OUTPUT input)
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
	output.toonPower = float4(floor(input.toonPower * 3) / 3.0f, 1);
	output.emissive = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnEmissive));
	output.uv = float4(frac(input.uv.x * TERRAIN_SIZE_WIDTH / FRAME_BUFFER_WIDTH), frac(input.uv.y * TERRAIN_SIZE_HEIGHT / FRAME_BUFFER_HEIGHT), 0, 0);

	return output;
}