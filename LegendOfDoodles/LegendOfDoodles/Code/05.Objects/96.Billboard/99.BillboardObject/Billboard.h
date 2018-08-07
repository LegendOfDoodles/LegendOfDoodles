#pragma once

#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"
#include "05.Objects/01.Camera/01.AOSCamera/AOSCamera.h"

struct CB_GAUGE_INFO
{
	XMFLOAT4X4	m_xmf4x4World;
	float		m_fCurrentHP;
};


class CBillboardObject : public CBaseObject
{
public:	// 생성자, 소멸자
	CBillboardObject(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CBillboardObject();

public: // 공개함수
	virtual void Animate(float fTimeElapsed);

	void SetLookAt();
	void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; };

protected: // 내부 함수

protected: // 변수
	CCamera * m_pCamera{ NULL };
};