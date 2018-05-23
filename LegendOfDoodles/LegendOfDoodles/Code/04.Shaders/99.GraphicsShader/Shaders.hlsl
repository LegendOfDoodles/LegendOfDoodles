#define TERRAIN_SIZE_WIDTH 10000
#define TERRAIN_SIZE_HEIGHT 5000
#define TERRAIN_SIZE_BORDER 1000

#define gnDiffuse 0
#define gnNormal 1
#define gnMix3Data 2
#define gnSpecular 3
#define gnEmissive 4

#define EMISSIVE_POWER 20
#define REFLECTION_POWER 0.25
#define OUTLINE_POWER 1

Texture2D gtxtTexture : register(t0);
Texture2DArray gtxtTextures : register(t1);
TextureCube gtxtTextureCube : register(t2);

SamplerState wrapSampler : register(s0);
SamplerState mirrorSampler : register(s1);

//ī�޶��� ������ ���� ��� ���۸� �����Ѵ�.
cbuffer cbCameraInfo : register(b0)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    float3 gvCameraPosition : packoffset(c8);
};

cbuffer cbGameObjectInfo : register(b1)
{
    matrix gmtxGameObject : packoffset(c0);
};

#include "./Light.hlsl"

///////////////////////////////////////////////////////////////////////////////////////////
//  ���� ���� Ÿ�� ��  PS���
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
};

struct PS_MULTIPLE_RENDER_TARGETS_OUTPUT_EMISSIVE
{
    float4 color : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 roughMetalFresnel : SV_TARGET2;
    float4 albedo : SV_TARGET3;
    float4 position : SV_TARGET4;
    float4 emissive : SV_TARGET5;
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
//���� ���̴��� �Է��� ���� ����ü�� �����Ѵ�.
struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};


//���� ���̴��� ���(�ȼ� ���̴��� �Է�)�� ���� ����ü�� �����Ѵ�.
struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

//���� ���̴��� �����Ѵ�.
VS_OUTPUT VSDiffused(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.color = input.color;
    return (output);
}


//�ȼ� ���̴��� �����Ѵ�.
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
cbuffer cbGaugeObjectInfo : register(b5)
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

PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT PSTexturedGauge(VS_GAUGE_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT output;

    if (input.uv.x <= CurrentHP)
    {
        output.color = gtxtTexture.Sample(wrapSampler, input.uv);
        output.normal = float4(0, 0, 0, 0);
        output.roughMetalFresnel = float4(0, 0, 0, 0);
    }
    else
        discard;

    return (output);
}

// Icon Gague
PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT PSTexturedIconGauge(VS_GAUGE_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT output;

    if (1 - input.uv.y <= CurrentHP)
    {
        output.color = gtxtTexture.Sample(wrapSampler, input.uv);
        output.normal = float4(0, 0, 0, 0);
        output.roughMetalFresnel = float4(0, 0, 0, 0);
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

// ������ ������Ʈ �׸����
PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTexturedLightingDetail(VS_TEXTURED_LIGHTING_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
    float3 N = normalize(input.normalW);
    if (input.tangentW.x != 0 || input.tangentW.y != 0 || input.tangentW.z != 0)
    {
        float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
        float3 B = cross(N, T); // �븻�� ź��Ʈ�� �����ؼ� ���� ź��Ʈ(���� �븻)����
        float3x3 TBN = float3x3(T, B, N); // �̸� �������� TBN��� ����
        float3 normal = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnNormal)); // �븻 �ʿ��� �ش��ϴ� uv�� �ش��ϴ� �븻 �б�
        normal = 2.0f * normal - 1.0f; // �븻�� -1���� 1������ ������ ��ȯ
        N = mul(normal, TBN); // �븻�� TBN��ķ� ��ȯ
        output.normal = float4(N, 1);
    }

    output.color = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnDiffuse)) + gtxtTextures.Sample(wrapSampler, float3(input.uv, gnSpecular));
    output.roughMetalFresnel = float4(gtxtTextures.Sample(wrapSampler, float3(input.uv, gnMix3Data)).rgb, 0);
    output.albedo = gMaterials.m_cAlbedo;
    output.position = float4(input.positionW, 0);
    output.position.x /= TERRAIN_SIZE_WIDTH;
    output.position.y /= TERRAIN_SIZE_BORDER;
    output.position.z /= TERRAIN_SIZE_HEIGHT;

    return output;
}

// �̹̽ú� ������Ʈ ó���� -> �ǹ� ó��
PS_MULTIPLE_RENDER_TARGETS_OUTPUT_EMISSIVE PSTexturedLightingEmissive(VS_TEXTURED_LIGHTING_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT_EMISSIVE output;
    float3 N = normalize(input.normalW);
    if (input.tangentW.x != 0 || input.tangentW.y != 0 || input.tangentW.z != 0)
    {
        float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
        float3 B = cross(N, T); // �븻�� ź��Ʈ�� �����ؼ� ���� ź��Ʈ(���� �븻)����
        float3x3 TBN = float3x3(T, B, N); // �̸� �������� TBN��� ����
        float3 normal = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnNormal)); // �븻 �ʿ��� �ش��ϴ� uv�� �ش��ϴ� �븻 �б�
        normal = 2.0f * normal - 1.0f; // �븻�� -1���� 1������ ������ ��ȯ
        N = mul(normal, TBN); // �븻�� TBN��ķ� ��ȯ
        output.normal = float4(N, 1);
    }

    output.color = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnDiffuse)) + gtxtTextures.Sample(wrapSampler, float3(input.uv, gnSpecular));
    output.roughMetalFresnel = float4(gtxtTextures.Sample(wrapSampler, float3(input.uv, gnMix3Data)).rgb, 0);
    output.albedo = gMaterials.m_cAlbedo;
    output.position = float4(input.positionW, 0);
    output.position.x /= TERRAIN_SIZE_WIDTH;
    output.position.y /= TERRAIN_SIZE_BORDER;
    output.position.z /= TERRAIN_SIZE_HEIGHT;
    output.emissive = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnEmissive));

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

cbuffer cbSkinnedInfo : register(b4)
{
    float4x4 gmtxBoneWorld : packoffset(c0);
    float4x4 gmtxBoneTransforms[128] : packoffset(c4);
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

// �����̴� ������Ʈ �׸����
PS_MULTIPLE_RENDER_TARGETS_OUTPUT_TOON PSBone(VS_TEXTURED_LIGHTING_TOON_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT_TOON output;

    float3 N = normalize(input.normalW);
    if (input.tangentW.x != 0 || input.tangentW.y != 0 || input.tangentW.z != 0)
    {
        float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
        float3 B = cross(N, T); // �븻�� ź��Ʈ�� �����ؼ� ���� ź��Ʈ(���� �븻)����
        float3x3 TBN = float3x3(T, B, N); // �̸� �������� TBN��� ����
        float3 normal = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnNormal)); // �븻 �ʿ��� �ش��ϴ� uv�� �ش��ϴ� �븻 �б�
        normal = 2.0f * normal - 1.0f; // �븻�� -1���� 1������ ������ ��ȯ
        N = mul(normal, TBN); // �븻�� TBN��ķ� ��ȯ
        output.normal = float4(N, 1);
    }

    output.color = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnDiffuse));
    output.roughMetalFresnel = float4(gMaterials.m_cRoughness, gMaterials.m_cMetalic, 1, 1);
    output.albedo = gMaterials.m_cAlbedo;
    output.position = float4(input.positionW, 0);
    output.position.x /= TERRAIN_SIZE_WIDTH;
    output.position.y /= TERRAIN_SIZE_BORDER;
    output.position.z /= TERRAIN_SIZE_HEIGHT;

    output.toonPower = float4(floor(input.toonPower * 3) / 3.0f, 1);
	output.uv.xy = input.uv;

    return output;
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

Texture2D<float4> gtxtSceneBaseColor : register(t3);
Texture2D<float4> gtxtSceneNormal : register(t4);
Texture2D<float4> gtxtSceneRoughMetalFresnel : register(t5);
Texture2D<float4> gtxtSceneAlbedo : register(t6);
Texture2D<float4> gtxtScenePosition : register(t7);
Texture2D<float4> gtxtSceneEmissive : register(t8);
Texture2D<float4> gtxtSceneToonPower : register(t9);
Texture2D<float4> gtxtUVBuffer : register(t10);

float4 CalculateOutlineColor(int2 pos)
{
    float outlineCnt = 0;

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
        return float4(1, 1, 1, 0);

    return float4(0, 0, 0, 0);
}

float4 CalculateEmissiveColor(int2 pos)
{
    float4 emissiveColor = gtxtSceneEmissive[pos] * 0.1;

    for (float i = -EMISSIVE_POWER; i <= EMISSIVE_POWER; ++i)
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
    for (float i = -EMISSIVE_POWER * 0.5; i <= EMISSIVE_POWER * 0.5; ++i)
    {
        emissiveColor += gtxtSceneEmissive[int2(pos.x - i, pos.y)] * 0.03;
        emissiveColor += gtxtSceneEmissive[int2(pos.x + i, pos.y)] * 0.03;
        emissiveColor += gtxtSceneEmissive[int2(pos.x, pos.y - i)] * 0.03;
        emissiveColor += gtxtSceneEmissive[int2(pos.x, pos.y + i)] * 0.03;
        emissiveColor += gtxtSceneEmissive[int2(pos.x - i, pos.y - i)] * 0.015;
        emissiveColor += gtxtSceneEmissive[int2(pos.x - i, pos.y + i)] * 0.015;
        emissiveColor += gtxtSceneEmissive[int2(pos.x + i, pos.y - i)] * 0.015;
        emissiveColor += gtxtSceneEmissive[int2(pos.x + i, pos.y + i)] * 0.015;
    }
    for (float i = -EMISSIVE_POWER * 0.25; i <= EMISSIVE_POWER * 0.25; ++i)
    {
        emissiveColor += gtxtSceneEmissive[int2(pos.x - i, pos.y)] * 0.05;
        emissiveColor += gtxtSceneEmissive[int2(pos.x + i, pos.y)] * 0.05;
        emissiveColor += gtxtSceneEmissive[int2(pos.x, pos.y - i)] * 0.05;
        emissiveColor += gtxtSceneEmissive[int2(pos.x, pos.y + i)] * 0.05;
        emissiveColor += gtxtSceneEmissive[int2(pos.x - i, pos.y - i)] * 0.025;
        emissiveColor += gtxtSceneEmissive[int2(pos.x - i, pos.y + i)] * 0.025;
        emissiveColor += gtxtSceneEmissive[int2(pos.x + i, pos.y - i)] * 0.025;
        emissiveColor += gtxtSceneEmissive[int2(pos.x + i, pos.y + i)] * 0.025;
    }

    return emissiveColor;
}

float4 CalculateSketchEffect(float2 uv, float intensity)
{
    float3 sketch0 = gtxtTextures.Sample(wrapSampler, float3(uv, 0)).rgb;
    float3 sketch1 = gtxtTextures.Sample(wrapSampler, float3(uv, 1)).rgb;

    float3 overbright = max(0, intensity - 1.0);

    float3 weightsA = saturate((intensity * 6.0) + float3(-5, -4, -3));
    float3 weightsB = saturate((intensity * 6.0) + float3(-2, -1, -0));

    weightsB.xy -= weightsB.yz;
    weightsB.z -= weightsA.x;
    weightsA.xy -= weightsA.zy;

    sketch0 = sketch0 * weightsA;
    sketch1 = sketch1 * weightsB;

    float4 finalColor = float4(overbright + sketch0.r + sketch0.g + sketch0.b + sketch1.r + sketch1.g + sketch1.b, 1);

    return float4(0.9 - finalColor.r, 0.9 - finalColor.g, 0.9 - finalColor.b, 1);
}

float4 PSTextureToFullScreen(float4 position : SV_POSITION) : SV_Target
{
    float2 uv = position.xy;
    uv.x /= gtxtSceneBaseColor.Length.x;
    uv.y /= gtxtSceneBaseColor.Length.y;

	// ���� ���
    float4 normal = gtxtSceneNormal.Sample(wrapSampler, uv);
    float4 albedo = gtxtSceneAlbedo.Sample(wrapSampler, uv);
    float4 baseColor = gtxtSceneBaseColor.Sample(wrapSampler, uv);
    float4 roughMetalFresnel = gtxtSceneRoughMetalFresnel.Sample(wrapSampler, uv);

	// �ܰ��� ó��
    float4 outlineColor = CalculateOutlineColor(position.xy);

	// ������ ���ϴ� ������Ʈ
    if (normal.w == 0)
        return baseColor + outlineColor;

    float4 pos = gtxtScenePosition.Sample(wrapSampler, uv);
    pos.x *= TERRAIN_SIZE_WIDTH;
    pos.y *= TERRAIN_SIZE_BORDER;
    pos.z *= TERRAIN_SIZE_HEIGHT;

	// Emissive ó��
    float4 emissiveColor = CalculateEmissiveColor(position.xy);

	// ȯ�� ���� ó��
    float3 viewDir = normalize(pos.xyz - gvCameraPosition);
    float3 viewReflectDir = reflect(viewDir, normal.xyz);
    float4 cubeMapColor = gtxtTextureCube.Sample(wrapSampler, viewReflectDir);

    float4 reflectColor = roughMetalFresnel.g * cubeMapColor + (1 - roughMetalFresnel.g) * baseColor;
    float4 totalReflectColor = (roughMetalFresnel.r * albedo + (1 - roughMetalFresnel.r) * reflectColor) * REFLECTION_POWER;

	// ����Ʈ ó��
    float4 lightColor = Lighting(pos.xyz, normal.xyz, albedo, baseColor, roughMetalFresnel);

	// ����ġ ����Ʈ ó��
    float4 finalColor = (lightColor + totalReflectColor) * gtxtSceneToonPower.Sample(wrapSampler, uv);

    if (roughMetalFresnel.w == 1)
    {
        float intensity = dot(finalColor.rgb, float3(0.2326, 0.7152, 0.0722));
        finalColor = finalColor * CalculateSketchEffect(gtxtUVBuffer.Sample(wrapSampler, uv).xy * 2, intensity);
    }

	// ���� ���� ���
    return finalColor + emissiveColor + outlineColor;
}
