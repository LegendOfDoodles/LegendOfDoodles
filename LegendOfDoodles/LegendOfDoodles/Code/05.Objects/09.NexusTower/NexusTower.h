#pragma once
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "00.Global/01.Utility/Enumerations.h"

class CCollisionManager;

class CNexusTower : public CCollisionObject
{
public:
	CNexusTower(CCreateMgr *pCreateMgr, int nMeshes = 1);
	virtual ~CNexusTower();

	virtual StaticInfo* GetNexusAndTowerStatus() { return &m_StatusInfo; }

public:	// �ܺ� �Լ�
	virtual void Animate(float timeElapsed);
	void SetMaxHP(int maxHP, int HP) { m_StatusInfo.maxHP = maxHP; m_StatusInfo.HP = HP; }

protected: // ���� �Լ�
	/*
	0. Idle		1.StartWalk		2.Walking	3.Smash		4.Slash		5.Dispute
	*/

protected: // ����
	float m_fEndTime{ 0 };

	StaticInfo m_StatusInfo;
};

