#pragma once
#include "05.Objects/96.Billboard/99.BillboardObject/Billboard.h"

class CGaugeObject : public CBillboardObject
{
public:	// 생성자, 소멸자
	CGaugeObject(shared_ptr<CCreateMgr> pCreateMgr);
	CGaugeObject(shared_ptr<CCreateMgr> pCreateMgr, GaugeUIType type);
	virtual ~CGaugeObject();

public: // 공개함수
	virtual void Animate(float fTimeElapsed);
	virtual void Render(CCamera *pCamera, UINT istanceCnt = 1);

	virtual float GetCurrentHP();

	void GetmasterObjectType(ObjectType type) { m_MasterObjectType = type; };

	StatesType GetState() { return m_pMasterObject->GetState(); }
	virtual void SetObject(CCollisionObject *pObject) { m_pMasterObject = pObject; };

	CCollisionObject* GetMasterObject() { return m_pMasterObject; }


protected: // 변수
	CCollisionObject * m_pMasterObject;
	ObjectType		  m_MasterObjectType;
	GaugeUIType		  m_Type;
};