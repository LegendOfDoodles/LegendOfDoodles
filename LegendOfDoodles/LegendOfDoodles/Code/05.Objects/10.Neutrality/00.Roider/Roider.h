#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "05.Objects/97.Skeleton/Skeleton.h"

typedef std::list<CPathEdge> Path;

class CRoider : public CAnimatedObject
{
public: // 생성자, 소멸자
	CRoider(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes = 1);
	virtual ~CRoider();

public:	// 외부 함수
	virtual void Animate(float timeElapsed = 0.0f);
	virtual void Render(CCamera *pCamera, UINT instanceCnt = 1);

	virtual 	void SetState(StatesType newState, shared_ptr<CWayFinder> pWayFinder = nullptr);

	virtual void PlayWalk(float timeElapsed, shared_ptr<CWayFinder> pWayFinder);
	virtual void PlayChase(float timeElapsed, shared_ptr<CWayFinder> pWayFinder);
	virtual void PlayAttack(float timeElapsed, shared_ptr<CWayFinder> pWayFinder);

	void SaveCurrentState();

	virtual void ReadyToAtk(shared_ptr<CWayFinder> pWayFinder);
	virtual void Respawn();

	virtual void SetCollisionManager(shared_ptr<CCollisionManager> manager);
	virtual void SetHP(float maxHP, float curHP);

	void SetNexusPoses(const XMFLOAT3& bluePos, const XMFLOAT3& redPos)
	{
		m_blueNexusLoc = bluePos;
		m_redNexusLoc = redPos;
	}

	virtual void NotifyDamager(CCollisionObject* other) { m_pDamager = other; }
	virtual void NotifyDamageTeam(TeamType type) { m_lastDamageTeam = type; }

	virtual CommonInfo* GetCommonStatus() { return &m_StatusInfo; };
	void SetPathes(Path* pathes) { m_pathes = pathes; }

protected:	// 내부 함수
	virtual void AdjustAnimationIndex();
	void AnimateByCurState();
	void GenerateSubPathToSpawnLocation(shared_ptr<CWayFinder> pWayFinder);
	bool FarFromSpawnLocation();

	virtual bool Heal(float timeElapsed);

protected:	// 변수
	bool m_activated{ false };
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

	XMFLOAT4X4 m_xmf4x4SpawnWorld;	// 생성시 월드 변환 행렬
	XMFLOAT3 m_spawnLocation;	// 생성 위치
	XMFLOAT3 m_blueNexusLoc;	// 블루 넥서스 위치
	XMFLOAT3 m_redNexusLoc;		// 레드 넥서스 위치

	float m_spawnCoolTime{ 0 };	// 죽은 이후 다시 생성할 때 까지 시간
	float m_deactiveTime{ 0 };	// 대기 시간으로 돌리는 시간

	CCollisionObject* m_pDamager{ NULL };

	TeamType m_lastDamageTeam;

	Path* m_pathes;
};
