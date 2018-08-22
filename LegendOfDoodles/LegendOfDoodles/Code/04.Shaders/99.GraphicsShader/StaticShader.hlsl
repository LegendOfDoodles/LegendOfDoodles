#include "./Common.hlsl"
#include "./Light.hlsl"

///////////////////////////////////////////////////////////////////////////////////////////
//  ���� ���� Ÿ�� ��  PS���
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct FOWINFO
{
    int m_bFoW[256];
};
StructuredBuffer<FOWINFO> gFogOfWar : register(t12);

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
        float3 normal = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnNormal)).xyz; // �븻 �ʿ��� �ش��ϴ� uv�� �ش��ϴ� �븻 �б�
        normal = 2.0f * normal - 1.0f; // �븻�� -1���� 1������ ������ ��ȯ
        N = mul(normal, TBN); // �븻�� TBN��ķ� ��ȯ
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

// �̹̽ú� ������Ʈ ó���� -> �ǹ� ó��
PS_MULTIPLE_RENDER_TARGETS_OUTPUT_EMISSIVE PSTexturedLightingEmissive(VS_TEXTURED_LIGHTING_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT_EMISSIVE output;
    float3 N = normalize(input.normalW);
    float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
    float3 B = cross(N, T); // �븻�� ź��Ʈ�� �����ؼ� ���� ź��Ʈ(���� �븻)����
    float3x3 TBN = float3x3(T, B, N); // �̸� �������� TBN��� ����
    float3 normal = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnNormal)).xyz; // �븻 �ʿ��� �ش��ϴ� uv�� �ش��ϴ� �븻 �б�
    normal = 2.0f * normal - 1.0f; // �븻�� -1���� 1������ ������ ��ȯ
    N = mul(normal, TBN); // �븻�� TBN��ķ� ��ȯ

    output.normal = float4(N, 1);
    output.color = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnDiffuse)) + gtxtTextures.Sample(wrapSampler, float3(input.uv, gnSpecular));
    output.color += float4(-0.9f * (1 - input.fow), -0.9f * (1 - input.fow), -0.9f * (1 - input.fow), 0);
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
    float3 B = cross(N, T); // �븻�� ź��Ʈ�� �����ؼ� ���� ź��Ʈ(���� �븻)����
    float3x3 TBN = float3x3(T, B, N); // �̸� �������� TBN��� ����
    float3 normal = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnNormal)).xyz; // �븻 �ʿ��� �ش��ϴ� uv�� �ش��ϴ� �븻 �б�
    normal = 2.0f * normal - 1.0f; // �븻�� -1���� 1������ ������ ��ȯ
    N = mul(normal, TBN); // �븻�� TBN��ķ� ��ȯ

    output.normal = float4(N, 1);
    output.color = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnDiffuse)) + gtxtTextures.Sample(wrapSampler, float3(input.uv, gnSpecular));
    output.color += float4(-0.9f * (1 - input.fow), -0.9f * (1 - input.fow), -0.9f * (1 - input.fow), 0);
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
    float3 B = cross(N, T); // �븻�� ź��Ʈ�� �����ؼ� ���� ź��Ʈ(���� �븻)����
    float3x3 TBN = float3x3(T, B, N); // �̸� �������� TBN��� ����
    float3 normal = gtxtTextures.Sample(wrapSampler, float3(input.uv, gnNormal)).xyz; // �븻 �ʿ��� �ش��ϴ� uv�� �ش��ϴ� �븻 �б�
    normal = 2.0f * normal - 1.0f; // �븻�� -1���� 1������ ������ ��ȯ
    N = mul(normal, TBN); // �븻�� TBN��ķ� ��ȯ

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
