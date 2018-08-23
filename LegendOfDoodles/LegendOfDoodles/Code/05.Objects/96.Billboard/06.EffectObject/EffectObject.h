#pragma once
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"

//#include "05.Objects/96.Billboard/99.BillboardObject/Billboard.h"

class CCollisionManager;

class CEffectObject : public CCollisionObject
{
public:
	CEffectObject(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes = 1);
	virtual ~CEffectObject();

public:	// 외부 함수
	virtual void Animate(float timeElapsed = 0.0f);
	virtual void Render(CCamera *pCamera, UINT istanceCnt = 1);

	virtual void SetCollisionManager(shared_ptr<CCollisionManager> manager) { m_pColManager = manager; }
	virtual void SetDirection(const XMFLOAT3& direction);
	virtual void SetEffectObjectsType(EffectObjectType type);

	virtual void SetMasterObject(CCollisionObject *pObject) { m_pMasterObject = pObject; };
	virtual void SetAnimationLength(int time) { m_maxAnimaitonTime = static_cast<float>(time); }

	virtual EffectObjectType GetEffetObjectsType() { return m_EffectObjectType; }
	void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; }
	float GetAnimationTime() { return m_AnimaitonTime; }

protected: // 내부 함수
	void LookAt(XMFLOAT3 objPosition);
	void LookAt(XMFLOAT2 objPosition);

	void MoveToDirection(float dist);

protected: // 변수
	EffectObjectType m_EffectObjectType;
	XMFLOAT3 m_direction{};
	float m_AnimaitonTime{ 0.0f };
	float m_maxAnimaitonTime{ 10.f };

	float	m_speed{ 0.0f };

	CCollisionObject * m_pMasterObject;

	shared_ptr<CCollisionManager> m_pColManager{ NULL };
	CCamera * m_pCamera{ NULL };
};