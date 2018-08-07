#pragma once
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"

struct VS_CB_LIGHT_CAMERA_INFO
{
	XMFLOAT4X4 m_xmf4x4View;
	XMFLOAT4X4 m_xmf4x4Projection;
	XMFLOAT4X4 m_xmf4x4ShadowViewProjTex;
};

class CLightCamera : public CCamera
{
public:	// 생성자, 소멸자
	CLightCamera(XMFLOAT3 lightDir);
	virtual ~CLightCamera();

public:	// 공개 함수
	virtual void Initialize(shared_ptr<CCreateMgr> pCreateMgr) override;

	virtual void UpdateShaderVariables(int rootSignatureIndex = 0);

	virtual bool IsInFrustum(BoundingOrientedBox& xmBoundingBox) { xmBoundingBox; return true; }

protected: // 내부 함수

protected: // 변수
	XMFLOAT3 m_lightDirection{ 0, 0, 0 };
};

