#pragma once

#include "stdafx.h"
#include "Billboard.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"
#include "05.Objects/01.Camera/01.AOSCamera/AOSCamera.h"
#include "06.Meshes/00.Vertex/Vertex.h"

struct CB_GAUGE_INFO
{
	XMFLOAT4X4	m_xmf4x4World;
	float		m_fCurrentHP;
};

enum UIType {
	Minimap = 0,
	KDA = 1,
	Skill = 2,
	Status = 3
};

enum GagueUIType {
	PlayerGauge,
	MinionGauge,
	NexusAndTower
};

enum IconUIType {
	PlayerIcon,
	MinionIcon,
	NexusAndTowerIcon
};

class CBillboardObject : public CBaseObject
{
public:	// 생성자, 소멸자
	CBillboardObject(CCreateMgr *pCreateMgr);
	virtual ~CBillboardObject();

public: // 공개함수
	virtual void Animate(float fTimeElapsed);

	void SetLookAt();
	void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; };

protected: // 내부 함수

protected: // 변수
	CCamera * m_pCamera{ NULL };
};

class CUIObject : public CBillboardObject
{
public:
	CUIObject(CCreateMgr *pCreateMgr);
	CUIObject(CCreateMgr *pCreateMgr, UIType type);
	virtual ~CUIObject();

public: // 공개함수
	virtual void Animate(float fTimeElapsed);

	virtual void SetDistance(float x) {
		m_xmf4x4World = Matrix4x4::Identity();

		m_fDistance = x;
	}

protected: // 변수
	XMFLOAT3	m_xmf3Position;
	float		m_fDistance;
	UIType		m_type;
};

class CHPGaugeObjects : public CBillboardObject
{
public:	// 생성자, 소멸자
	CHPGaugeObjects(CCreateMgr *pCreateMgr);
	CHPGaugeObjects(CCreateMgr *pCreateMgr, GagueUIType type);
	virtual ~CHPGaugeObjects();

public: // 공개함수
	virtual void Animate(float fTimeElapsed);
	virtual void Render(CCamera *pCamera, UINT istanceCnt = 1);

	float GetCurrentHP();

	void GetmasterObjectType(ObjectType type) { m_MasterObjectType = type; };

	virtual void SetObject(CCollisionObject *pObject) { m_pMasterObject = pObject; };


protected: // 변수
	CCollisionObject * m_pMasterObject;
	GagueUIType		  m_Type;
	ObjectType		  m_MasterObjectType;

};

class CMinimapIconObjects :public CUIObject
{
public:
	CMinimapIconObjects(CCreateMgr *pCreateMgr);
	CMinimapIconObjects(CCreateMgr *pCreateMgr, IconUIType type);
	virtual ~CMinimapIconObjects();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(CCamera *pCamera, UINT istanceCnt = 1);

	void WorldToMinimap();

	virtual void SetObject(CCollisionObject *pObject) { m_pMasterObject = pObject; };
	void GetmasterObjectType(ObjectType type) { m_MasterObjectType = type; };

	CCollisionObject* GetMasterObject() { return m_pMasterObject; };

protected:
	CCollisionObject * m_pMasterObject;
	XMFLOAT3 m_MinimapPosition;

	ObjectType		  m_MasterObjectType;
};