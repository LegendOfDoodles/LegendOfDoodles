#pragma once
#include "05.Objects/02.CollisionObject/CollisionObject.h"

class CCollisionManager;

class CNexusTower : public CCollisionObject
{
public:
	CNexusTower();
	virtual ~CNexusTower();

public:	// �ܺ� �Լ�
	virtual void Animate(float timeElapsed);

	virtual void PlayIdle(float timeElapsed);
	virtual void PlayAttack(float timeElapsed, shared_ptr<CWayFinder> pWayFinder);
	virtual void PlayDie(float timeElapsed);

	virtual void ReceiveDamage(float damage) {
		// �̹� ����� ������ ��� ����� ó���� ���� �ʴ´�.
		if (m_curState == States::Die || m_curState == States::Remove) { return; }

		m_StatusInfo.HP -= damage * Compute_Defence(m_StatusInfo.Def);

		if (m_StatusInfo.HP <= 0 && m_curState != States::Die) {
			SetState(States::Die);
			if (m_ObjectType == ObjectType::Nexus)
				m_pColManager->GameOver(m_TeamType);
		}
	}

	virtual StaticInfo* GetNexusAndTowerStatus() { return &m_StatusInfo; }
	

protected: // ���� �Լ�
		   /*
		   0. Idle		1.StartWalk		2.Walking	3.Smash		4.Slash		5.Dispute
		   */

protected: // ����
	float m_fEndTime{ 0 };

	StaticInfo m_StatusInfo;

	float m_atkCoolTime{ 0.0f };
};

