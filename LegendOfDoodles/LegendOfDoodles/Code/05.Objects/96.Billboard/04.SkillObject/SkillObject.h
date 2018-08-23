#pragma once
#include "05.Objects/96.Billboard/99.BillboardObject/Billboard.h"

class CSkillObject : public CBillboardObject
{
public:
	CSkillObject(shared_ptr<CCreateMgr> pCreateMgr);
	CSkillObject(shared_ptr<CCreateMgr> pCreateMgr, SkillUIType type);
	virtual ~CSkillObject();

public: // 공개함수
	virtual void Animate(float fTimeElapsed = 0.0f);
	virtual void Render(CCamera *pCamera, UINT istanceCnt = 1);

	virtual int GetType() { return m_type; }
	virtual float GetCoolTime();

	virtual void SetDistance(float x) {
		m_xmf4x4World = Matrix4x4::Identity();

		m_fDistance = x;
	}

	virtual void SetObject(CCollisionObject *pObject) { m_pMasterObject = pObject; };
	CCollisionObject* GetMasterObject() { return m_pMasterObject; };

protected: // 변수
	float		m_fDistance;
	SkillUIType		m_type;

	CCollisionObject *m_pMasterObject{ NULL };
};