#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"

class CWayFinder;

class CCollisionObject : public CBaseObject
{
public:	// ������, �Ҹ���
	CCollisionObject(CCreateMgr *pCreateMgr, int nMeshes = 1);
	virtual ~CCollisionObject();

public: // ���� �Լ�
	virtual TeamType GetTeam() { return m_TeamType; }
	virtual void SetTeam(TeamType type) { m_TeamType = type; }

	void SetStatic(StaticType type) { m_StaticType = type; }
	StaticType GetStaticType() { return m_StaticType; }

	virtual PlayerInfo* GetPlayerStatus() { return NULL; }
	virtual CommonInfo* GetCommonStatus() { return NULL; }
	virtual StaticInfo* GetNexusAndTowerStatus() { return NULL; }

protected: // ���� �Լ�

protected: // ����
	TeamType m_TeamType{ None };

	StaticType m_StaticType {StaticType::Move};
};
