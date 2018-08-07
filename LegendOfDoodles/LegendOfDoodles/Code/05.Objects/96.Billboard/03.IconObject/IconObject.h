#pragma once
#include "05.Objects/96.Billboard/01.FrameObject/UIFrameObject.h"


class CIconObject :public CUIFrameObject
{
public:
	CIconObject(shared_ptr<CCreateMgr> pCreateMgr);
	CIconObject(shared_ptr<CCreateMgr> pCreateMgr, IconUIType type);
	virtual ~CIconObject();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(CCamera *pCamera, UINT istanceCnt = 1);

	virtual float GetCurrentHP();

	void WorldToMinimap();

	virtual void SetObject(CCollisionObject *pObject) { m_pMasterObject = pObject; };
	void GetmasterObjectType(ObjectType type) { m_MasterObjectType = type; };

	StatesType GetState() { return m_pMasterObject->GetState(); }

protected:
	XMFLOAT3		  m_MinimapPosition;
	ObjectType		  m_MasterObjectType;
	IconUIType		  m_type;
};