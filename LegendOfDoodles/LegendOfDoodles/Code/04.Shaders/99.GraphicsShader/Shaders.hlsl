#define TERRAIN_SIZE_WIDTH 10000
#define TERRAIN_SIZE_HEIGHT 5000
#define TERRAIN_SIZE_BORDER 1000

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

cbuffer cbGameObjectInfo : register(b2)
{
    matrix gmtxGameObject : packoffset(c0);
};

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
PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSDiffused(VS_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
    output.color = input.color;
    output.normal = float4(0, 0, 0, 0);

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

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTextured(VS_TEXTURED_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
    output.color = gtxtTexture.Sample(wrapSampler, input.uv);
    output.normal = float4(0, 0, 0, 0);

    return (output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTexturedRepeat(VS_TEXTURED_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
    float2 newUV;
    newUV.x = input.uv.x * 6;
    newUV.y = input.uv.y * 5;

    output.color = gtxtTexture.Sample(mirrorSampler, newUV);
    output.normal = float4(0, 0, 0, 0);

    return (output);
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

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTexturedUI(VS_UI_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
    output.color = gtxtTextures[NonUniformResourceIndex(input.texIndex)].Sample(wrapSampler, input.uv);
    output.normal = float4(0, 0, 0, 0);

    return (output);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Gauge Object 
cbuffer cbGaugeObjectInfo : register(b7)
{
    matrix gmtxGaugeObject : packoffset(c0);
    float CurrentHP : packoffset(c4);
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

    return (output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTexturedGauge(VS_GAUGE_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

    if (input.uv.x <= CurrentHP)
    {
        output.color = gtxtTexture.Sample(wrapSampler, input.uv);
        output.normal = float4(0, 0, 0, 0);
    }
    else
        discard;

    return (output);
}

// Icon Gague
PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTexturedIconGauge(VS_GAUGE_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

    if (1 - input.uv.y <= CurrentHP)
    {
        output.color = gtxtTexture.Sample(wrapSampler, input.uv);
        output.normal = float4(0, 0, 0, 0);
    }
    else
        discard;

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
    output.position.x /= TERRAIN_SIZE_WIDTH;
    output.position.y /= TERRAIN_SIZE_BORDER;
    output.position.z /= TERRAIN_SIZE_HEIGHT;

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
};

VS_TEXTURED_LIGHTING_OUTPUT VSTexturedLighting(VS_TEXTURED_LIGHTING_INPUT input)
{
    VS_TEXTURED_LIGHTING_OUTPUT output;

    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;
    output.tangentW = mul(input.tangent, (float3x3) gmtxGameObject);

    return (output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTexturedLighting(VS_TEXTURED_LIGHTING_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

    float3 N = normalize(input.normalW);
    if (input.tangentW.x != 0 || input.tangentW.y != 0 || input.tangentW.z != 0)
    {
        float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
        float3 B = cross(N, T); // 노말과 탄젠트를 외적해서 바이 탄젠트(바이 노말)생성
        float3x3 TBN = float3x3(T, B, N); // 이를 바탕으로 TBN행렬 생성
        float3 normal = gtxtNormal.Sample(wrapSampler, input.uv); // 노말 맵에서 해당하는 uv에 해당하는 노말 읽기
        normal = 2.0f * normal - 1.0f; // 노말을 -1에서 1사이의 값으로 변환
        N = mul(normal, TBN); // 노말을 TBN행렬로 변환
        output.normal = float4(N, 1);
    }

    output.color = gtxtTexture.Sample(wrapSampler, input.uv);
    output.roughMetalFresnel = float4(gMaterials.m_cRoughness, gMaterials.m_cMetalic, 1, 0);
    output.albedo = gMaterials.m_cAlbedo;
    output.position = float4(input.positionW, 0);
    output.position.x /= TERRAIN_SIZE_WIDTH;
    output.position.y /= TERRAIN_SIZE_BORDER;
    output.position.z /= TERRAIN_SIZE_HEIGHT;

    return output;
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTexturedLightingDetail(VS_TEXTURED_LIGHTING_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
    float3 N = normalize(input.normalW);
    if (input.tangentW.x != 0 || input.tangentW.y != 0 || input.tangentW.z != 0)
    {
        float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
        float3 B = cross(N, T); // 노말과 탄젠트를 외적해서 바이 탄젠트(바이 노말)생성
        float3x3 TBN = float3x3(T, B, N); // 이를 바탕으로 TBN행렬 생성
        float3 normal = gtxtNormal.Sample(wrapSampler, input.uv); // 노말 맵에서 해당하는 uv에 해당하는 노말 읽기
        normal = 2.0f * normal - 1.0f; // 노말을 -1에서 1사이의 값으로 변환
        N = mul(normal, TBN); // 노말을 TBN행렬로 변환
        output.normal = float4(N, 1);
    }

    output.color = gtxtTexture.Sample(wrapSampler, input.uv) + gtxtSpecular.Sample(wrapSampler, input.uv);
    output.roughMetalFresnel = gtxtRoughnessMetalFresnel.Sample(wrapSampler, input.uv);
    output.albedo = gMaterials.m_cAlbedo;
    output.position = float4(input.positionW, 0);
    output.position.x /= TERRAIN_SIZE_WIDTH;
    output.position.y /= TERRAIN_SIZE_BORDER;
    output.position.z /= TERRAIN_SIZE_HEIGHT;

    return output;
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

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSDiffuseTextured(VS_DIFFUSE_TEXTURED_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
    output.color = lerp(input.color, gtxtTexture.Sample(wrapSampler, input.uv), 0.7f);
    output.normal = float4(0, 0, 0, 0);
    output.roughMetalFresnel = float4(gMaterials.m_cRoughness, gMaterials.m_cMetalic, 1, 0);
    output.albedo = gMaterials.m_cAlbedo;
    output.position = float4(input.positionW, 0);
    output.position.x /= TERRAIN_SIZE_WIDTH;
    output.position.y /= TERRAIN_SIZE_BORDER;
    output.position.z /= TERRAIN_SIZE_HEIGHT;
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

cbuffer cbSkinnedInfo : register(b5)
{
    float4x4 gmtxBoneWorld : packoffset(c0);
    float4x4 gmtxBoneTransforms[128] : packoffset(c4);
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

    return (output);
}

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

Texture2D<float4> gtxtSceneBaseColor : register(t5);
Texture2D<float4> gtxtSceneNormal : register(t6);
Texture2D<float4> gtxtSceneRoughMetalFresnel : register(t7);
Texture2D<float4> gtxtSceneAlbedo : register(t8);
Texture2D<float4> gtxtScenePosition : register(t9);

static int2 gnOffsets[9] = { { -1, -1 }, { 0, -1 }, { 1, -1 }, { -1, 0 }, { 0, 0 }, { 1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 } };

float4 PSTextureToFullScreen(float4 position : SV_POSITION) : SV_Target
{
    if (gtxtSceneNormal[int2(position.xy)].w == 0)
        return gtxtSceneBaseColor[int2(position.xy)];

    float4 pos = gtxtScenePosition[int2(position.xy)];
    pos.x *= TERRAIN_SIZE_WIDTH;
    pos.y *= TERRAIN_SIZE_BORDER;
    pos.z *= TERRAIN_SIZE_HEIGHT;

    return Lighting(float3(pos.xyz), float3(gtxtSceneNormal[int2(position.xy)].xyz), gtxtSceneAlbedo[int2(position.xy)],
								gtxtSceneBaseColor[int2(position.xy)], gtxtSceneRoughMetalFresnel[int2(position.xy)]);
}
