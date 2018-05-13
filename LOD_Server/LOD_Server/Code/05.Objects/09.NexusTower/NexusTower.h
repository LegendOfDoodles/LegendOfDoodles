#pragma once
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "00.Global/01.Utility/Enumerations.h"

class CCollisionManager;

class CNexusTower : public CCollisionObject
{
public:
	CNexusTower();
	virtual ~CNexusTower();

	ObjectType type;
public:	// �ܺ� �Լ�
	virtual void Animate(float timeElapsed);


	virtual void SetCollisionManager(CCollisionManager* manager) { m_pColManager = manager; }

	virtual void ReceiveDamage(float damage) { 
		m_StatusInfo.HP -= damage * Compute_Defence(m_StatusInfo.Def);
		
		if (m_StatusInfo.HP <= 0 && m_curState != States::Die) {
			SetState(States::Die);
			if(type== ObjectType::Nexus)
				m_pColManager->GameOver(m_TeamType);
		}
	}

protected: // ���� �Լ�
	/*
	0. Idle		1.StartWalk		2.Walking	3.Smash		4.Slash		5.Dispute
	*/

protected: // ����
	float m_fEndTime{ 0 };

	StaticInfo m_StatusInfo;
	CCollisionManager * m_pColManager{ NULL };
};

