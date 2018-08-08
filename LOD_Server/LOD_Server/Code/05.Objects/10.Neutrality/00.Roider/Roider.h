#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "05.Objects/97.Skeleton/Skeleton.h"

typedef std::list<CPathEdge> Path;

class CRoider : public CAnimatedObject
{
public: // ������, �Ҹ���
	CRoider();
	virtual ~CRoider();

public:	// �ܺ� �Լ�
	virtual void Animate(float timeElapsed);

	virtual 	void SetState(StatesType newState);

	virtual void PlayIdle(float timeElapsed);
	virtual void PlayWalk(float timeElapsed, shared_ptr<CWayFinder> pWayFinder);
	virtual void PlayChase(float timeElapsed, shared_ptr<CWayFinder> pWayFinder);
	virtual void PlayAttack(float timeElapsed, shared_ptr<CWayFinder> pWayFinder);
	virtual void PlayRemove(float timeElapsed, shared_ptr<CWayFinder> pWayFinder);

	void SaveCurrentState();

	void SetNexusPoses(const XMFLOAT3& bluePos, const XMFLOAT3& redPos)
	{
		m_blueNexusLoc = bluePos;
		m_redNexusLoc = redPos;
	}

	virtual void ReceiveDamage(float damage)
	{
		// �̹� ����� ������ ��� ����� ó���� ���� �ʴ´�.
		if (m_curState == States::Die || m_curState == States::Remove) { return; }

		m_StatusInfo.HP -= damage * Compute_Defence(m_StatusInfo.Def);
		if (m_StatusInfo.HP <= 0) {
			SetState(States::Die);
		}
		m_activated = true;
	}

	virtual void NotifyDamager(CCollisionObject* other) { m_pDamager = other; }
	virtual void NotifyDamageTeam(TeamType type) { m_lastDamageTeam = type; }

	virtual CommonInfo* GetCommonStatus() { return &m_StatusInfo; };

	void SetPathes(Path* pathes) { m_pathes = pathes; }

protected:	// ���� �Լ�
	virtual void AdjustAnimationIndex();
	void AnimateByCurState();
	void ReadyToAtk(shared_ptr<CWayFinder> pWayFinder);
	void Respawn();
	void GenerateSubPathToSpawnLocation(shared_ptr<CWayFinder> pWayFinder);
	bool FarFromSpawnLocation();

protected:	// ����
	bool m_activated{ false };
	bool m_returning{ false };
	/*
	0. Idle		1.Attack	2.Attack2	3.StartWalk		4.Walking	5.Die
	*/

	/*
	0. SwordPlayer, 1. StaffPlayer,   2. BowPlayer,
	3. SwordMinion,	4. StaffMinion,   5. BowMinion,
	6. Roider,   	7. CAM,     	  8. GOLEM,
	9. FirstTower,  10. SecnondTower, 11. Nexus
	*/

	CommonInfo m_StatusInfo;

	XMFLOAT4X4 m_xmf4x4SpawnWorld;	// ������ ���� ��ȯ ���
	XMFLOAT3 m_spawnLocation;	// ���� ��ġ
	XMFLOAT3 m_blueNexusLoc;	// ��� �ؼ��� ��ġ
	XMFLOAT3 m_redNexusLoc;		// ���� �ؼ��� ��ġ

	float m_spawnCoolTime{ 0 };	// ���� ���� �ٽ� ������ �� ���� �ð�
	float m_deactiveTime{ 0 };	// ��� �ð����� ������ �ð�

	CCollisionObject* m_pDamager{ NULL };

	TeamType m_lastDamageTeam;

	Path* m_pathes;
};
