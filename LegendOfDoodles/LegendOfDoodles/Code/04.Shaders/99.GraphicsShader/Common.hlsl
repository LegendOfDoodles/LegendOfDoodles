#define TERRAIN_SIZE_WIDTH 10000
#define TERRAIN_SIZE_HEIGHT 5000
#define TERRAIN_SIZE_BORDER 1000

#define	FRAME_BUFFER_WIDTH		1280
#define	FRAME_BUFFER_HEIGHT		720

#define gnDiffuse 0
#define gnNormal 1
#define gnMix3Data 2
#define gnSpecular 3
#define gnEmissive 4

#define EMISSIVE_POWER 6
#define REFLECTION_POWER 0.25
#define OUTLINE_POWER 1

Texture2D gtxtTexture : register(t0);
Texture2DArray gtxtTextures : register(t1);
TextureCube gtxtTextureCube : register(t2);

SamplerState wrapSampler : register(s0);
SamplerState mirrorSampler : register(s1);
SamplerComparisonState shadowSampler : register(s2);

//카메라의 정보를 위한 상수 버퍼를 선언한다.
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

cbuffer cbSkinnedInfo : register(b4)
{
    float4x4 gmtxBoneWorld : packoffset(c0);
    float4x4 gmtxBoneTransforms[128] : packoffset(c4);
};

cbuffer cbGaugeObjectInfo : register(b5)
{
    matrix gmtxGaugeObject : packoffset(c0);
    float CurrentHP : packoffset(c4);
};

cbuffer cbCameraInfo : register(b6)
{
    matrix gmtxLightView : packoffset(c0);
    matrix gmtxLightProjection : packoffset(c4);
    matrix gmtxShadowViewProjTex : packoffset(c8);
};

cbuffer cbEffectObjectInfo : register(b7)
{
	matrix gmtxEffectObject : packoffset(c0);
	float animationTime : packoffset(c4);
}