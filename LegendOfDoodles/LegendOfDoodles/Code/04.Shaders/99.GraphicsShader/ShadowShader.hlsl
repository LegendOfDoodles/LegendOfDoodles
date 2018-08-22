#include "./Common.hlsl"

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_TEXTURED_LIGHTING_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
};

VS_OUTPUT VSTexturedLighting(VS_TEXTURED_LIGHTING_INPUT input)
{
    VS_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxLightView), gmtxLightProjection);
    return (output);
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
};

VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
    VS_TERRAIN_OUTPUT output;

	output.position = mul(float4(input.position, 1.0f), gmtxGameObject);
    output.uv = input.uv;

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

    return output;
}

struct DS_TERRAIN_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
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

// Displacement Mapping
    output.position = mul(mul(p, gmtxLightView), gmtxLightProjection);;
    output.uv = uvResult;

    return output;
}

void PSTerrain(DS_TERRAIN_OUTPUT input)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// For Characters
struct VS_BONEINPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord0 : TEXCOORD;
    float3 boneWeights : WEIGHTS;
    uint4 boneIndices : BONEINDICES;
    float3 tangent : TANGENT;
};

VS_OUTPUT VSBone(VS_BONEINPUT input)
{
    VS_OUTPUT output;

    float fWeights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    fWeights[0] = input.boneWeights.x;
    fWeights[1] = input.boneWeights.y;
    fWeights[2] = input.boneWeights.z;
    fWeights[3] = 1 - fWeights[0] - fWeights[1] - fWeights[2];
    float3 position = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 4; ++i)
    {
        position += fWeights[i] * mul(float4(input.position, 1.0f), gmtxBoneTransforms[input.boneIndices[i]]).xyz;
    }

    output.position = mul(mul(mul(float4(position, 1.0f), gmtxBoneWorld), gmtxLightView), gmtxLightProjection);

    return (output);
}

void PSMain(VS_OUTPUT input)
{
}