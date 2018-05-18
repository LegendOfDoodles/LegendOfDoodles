#define USE_INSTANCING 0
Texture2D gtxtTexture : register(t0);
Texture2D gtxtNormal : register(t1);
Texture2D gtxtRoughnessMetalFresnel : register(t2);
Texture2D gtxtSpecular : register(t3);

Texture2D gtxtTextures[4] : register(t4);

SamplerState wrapSampler : register(s0);
SamplerState mirrorSampler : register(s1);

//게임 객체의 정보를 위한 상수 버퍼를 선언한다.
cbuffer cbPlayerInfo : register(b0)
{
	matrix gmtxWorld : packoffset(c0);
};

//카메라의 정보를 위한 상수 버퍼를 선언한다.
cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
    float3 gvCameraPosition : packoffset(c8);
};

#if USE_INSTANCING
struct INSTANCE_INFO
{
	matrix m_mtxGameObject;
};
StructuredBuffer<INSTANCE_INFO> gGameObjectInfos : register(t4);
#else
cbuffer cbGameObjectInfo : register(b2)
{
	matrix		gmtxGameObject : packoffset(c0);
};
#endif

#include "./Light.hlsl"

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
float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
    return (input.color);
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

#if USE_INSTANCING
VS_TEXTURED_OUTPUT VSTxtInstancing(VS_TEXTURED_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_TEXTURED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gGameObjectInfos[nInstanceID].m_mtxGameObject), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}
#else
VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input)
{
	VS_TEXTURED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}
#endif

float4 PSTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(wrapSampler, input.uv);

	return(cColor);
}

float4 PSTexturedRepeat(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
    float2 newUV;
    newUV.x = input.uv.x * 6;
    newUV.y = input.uv.y * 5;

    float4 cColor = gtxtTexture.Sample(mirrorSampler, newUV);

    return (cColor);
}

///////////////////////////////////////////////////////////////////////////////////////////
//
cbuffer cbGameObjectInfo : register(b6)
{
    matrix gmtxUI : packoffset(c0);
    uint texIndex : packoffset(c4);
};

struct VS_UI_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_UI_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    uint texIndex : TEXINDEX;
};

VS_UI_OUTPUT VSTexturedUI(VS_UI_INPUT input)
{
    VS_UI_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxUI), gmtxView), gmtxProjection);
    output.uv = input.uv;
    output.texIndex = texIndex;

    return (output);
}

float4 PSTexturedUI(VS_UI_OUTPUT input) : SV_TARGET
{
    float4 cColor = gtxtTextures[NonUniformResourceIndex(input.texIndex)].Sample(wrapSampler, input.uv);

    return (cColor);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Gauge Object 
cbuffer cbGaugeObjectInfo : register(b7)
{
	matrix		gmtxGaugeObject : packoffset(c0);
	float		CurrentHP : packoffset(c4);
};

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

	return(output);
}

float4 PSTexturedGauge(VS_GAUGE_OUTPUT input) : SV_TARGET
{
	float4 cColor;
if (input.uv.x <= CurrentHP) {
	cColor = gtxtTexture.Sample(wrapSampler, input.uv);
}
else
discard;

return (cColor);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//#define _WITH_VERTEX_LIGHTING

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
#ifdef _WITH_VERTEX_LIGHTING
    float4 color : COLOR;
#endif
};

#if USE_INSTANCING
VS_LIGHTING_OUTPUT VSLightingInstancing(VS_LIGHTING_INPUT input, uint nInstanceID : SV_InstanceID)
{
    VS_LIGHTING_OUTPUT output;

    output.normalW = mul(input.normal, (float3x3) gGameObjectInfos[nInstanceID].m_mtxGameObject);
    output.positionW = (float3) mul(float4(input.position, 1.0f), gGameObjectInfos[nInstanceID].m_mtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
#ifdef _WITH_VERTEX_LIGHTING
    output.normalW = normalize(output.normalW);
    output.color = Lighting(output.positionW, output.normalW);
#endif
    return (output);
}
#else
VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input)
{
    VS_LIGHTING_OUTPUT output;

    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
#ifdef _WITH_VERTEX_LIGHTING
    output.normalW = normalize(output.normalW);
    output.color = Lighting(output.positionW, output.normalW);
#endif
    return (output);
}
#endif

float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
#ifdef _WITH_VERTEX_LIGHTING
    return (input.color);
#else
	input.normalW = normalize(input.normalW);
	float4 color = Lighting(input.positionW, input.normalW);
	return(color);
#endif
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
#ifdef _WITH_VERTEX_LIGHTING
    float4 color : COLOR;
#endif
};

#if USE_INSTANCING
VS_TEXTURED_LIGHTING_OUTPUT VSTexturedLightingInstancing(VS_TEXTURED_LIGHTING_INPUT input, uint nInstanceID : SV_InstanceID)
{
    VS_TEXTURED_LIGHTING_OUTPUT output;

    output.normalW = mul(input.normal, (float3x3) gGameObjectInfos[nInstanceID].m_mtxGameObject);
    output.positionW = (float3) mul(float4(input.position, 1.0f), gGameObjectInfos[nInstanceID].m_mtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;
    output.tangentW = mul(input.tangent, (float3x3) gGameObjectInfos[nInstanceID].m_mtxGameObject);
#ifdef _WITH_VERTEX_LIGHTING
    output.normalW = normalize(output.normalW);
    output.color = Lighting(output.positionW, output.normalW);
#endif
    return (output);
}
#else
VS_TEXTURED_LIGHTING_OUTPUT VSTexturedLighting(VS_TEXTURED_LIGHTING_INPUT input)
{
    VS_TEXTURED_LIGHTING_OUTPUT output;

    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;
	output.tangentW = mul(input.tangent, (float3x3)gmtxGameObject);
#ifdef _WITH_VERTEX_LIGHTING
    output.normalW = normalize(output.normalW);
    output.color = Lighting(output.positionW, output.normalW);
#endif
    return (output);
}
#endif

float4 PSTexturedLighting(VS_TEXTURED_LIGHTING_OUTPUT input) : SV_TARGET
{
#ifdef _WITH_VERTEX_LIGHTING
    float4 cIllumination = input.color;
#else
    float3 N = normalize(input.normalW);
    if (input.tangentW.x != 0 || input.tangentW.y != 0 || input.tangentW.z != 0)
    {
        float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
        float3 B = cross(N, T); // 노말과 탄젠트를 외적해서 바이 탄젠트(바이 노말)생성
        float3x3 TBN = float3x3(T, B, N); // 이를 바탕으로 TBN행렬 생성
        float3 normal = gtxtNormal.Sample(wrapSampler, input.uv); // 노말 맵에서 해당하는 uv에 해당하는 노말 읽기
        normal = 2.0f * normal - 1.0f; // 노말을 -1에서 1사이의 값으로 변환
        N = mul(normal, TBN); // 노말을 TBN행렬로 변환
    }

    float4 cColor = Lighting(input.positionW, N, gtxtTexture.Sample(wrapSampler, input.uv));
#endif
    return gMaterials.m_cAlbedo * cColor;
}

float4 PSTexturedLightingDetail(VS_TEXTURED_LIGHTING_OUTPUT input) : SV_TARGET
{
#ifdef _WITH_VERTEX_LIGHTING
    float4 cIllumination = input.color;
#else
    float3 N = normalize(input.normalW);
    if (input.tangentW.x != 0 || input.tangentW.y != 0 || input.tangentW.z != 0)
    {
        float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
        float3 B = cross(N, T); // 노말과 탄젠트를 외적해서 바이 탄젠트(바이 노말)생성
        float3x3 TBN = float3x3(T, B, N); // 이를 바탕으로 TBN행렬 생성
        float3 normal = gtxtNormal.Sample(wrapSampler, input.uv); // 노말 맵에서 해당하는 uv에 해당하는 노말 읽기
        normal = 2.0f * normal - 1.0f; // 노말을 -1에서 1사이의 값으로 변환
        N = mul(normal, TBN); // 노말을 TBN행렬로 변환
    }

    float4 cColor = Lighting(input.positionW, N, 
	gtxtTexture.Sample(wrapSampler, input.uv),
	gtxtSpecular.Sample(wrapSampler, input.uv),
    gtxtRoughnessMetalFresnel.Sample(wrapSampler, input.uv));
#endif
    return gMaterials.m_cAlbedo * cColor;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_DIFFUSE_TEXTURED_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float4 color : BCOLOR;
};

struct VS_DIFFUSE_TEXTURED_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
	//	nointerpolation float3 normalW : NORMAL;
    float2 uv : TEXCOORD;
    float4 color : BCOLOR;
};

VS_DIFFUSE_TEXTURED_OUTPUT VSDiffuseTextured(VS_DIFFUSE_TEXTURED_INPUT input)
{
    VS_DIFFUSE_TEXTURED_OUTPUT output;

    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;
    output.color = input.color;
    return (output);
}

float4 PSDiffuseTextured(VS_DIFFUSE_TEXTURED_OUTPUT input) : SV_TARGET
{
    return lerp(input.color, gtxtTexture.Sample(wrapSampler, input.uv), 0.7f);
}


struct VS_BONEINPUT {
	float3 position : POSITION;
	float3 normal: NORMAL;
	float2 texcoord0 : TEXCOORD;
	float3 boneWeights : WEIGHTS;
	uint4 boneIndices : BONEINDICES;
	float3 tangent : TANGENT;
};

cbuffer cbSkinnedInfo: register(b5)
{
	float4x4 gmtxBoneWorld: packoffset(c0);
	float4x4 gmtxBoneTransforms[128]: packoffset(c4);
};

VS_TEXTURED_LIGHTING_OUTPUT VSBone(VS_BONEINPUT input)
{
    VS_TEXTURED_LIGHTING_OUTPUT output;

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
#ifdef _WITH_VERTEX_LIGHTING
	output.normalW = normalize(output.normalW);
	output.color = Lighting(output.positionW, output.normalW);
#endif


	output.positionW = mul(float4(position, 1.0f), gmtxBoneWorld).xyz;

	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);

	return(output);
}