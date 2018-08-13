#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"
#include "00.Global/01.Utility/07.ThrowingManager/ThrowingMgr.h"
#include "00.Global\01.Utility/08.EffectManager/EffectManager.h"

class CWayFinder;

class CCollisionObject : public CBaseObject
{
public:	// 생성자, 소멸자
	CCollisionObject(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes = 1);
	virtual ~CCollisionObject();

public: // 공개 함수
	bool CheckEnemyState(CCollisionObject* other);
	virtual bool Attackable(CCollisionObject* other);
	virtual bool AttackableFarRange(CCollisionObject* other);

	virtual void PlayIdle(float timeElapsed) { timeElapsed; }
	virtual void PlayWalk(float timeElapsed, shared_ptr<CWayFinder> pWayFinder) { timeElapsed; pWayFinder; }
	virtual void PlayChase(float timeElapsed, shared_ptr<CWayFinder> pWayFinder) { timeElapsed; pWayFinder; }
	virtual void PlayAttack(float timeElapsed, shared_ptr<CWayFinder> pWayFinder) { timeElapsed; pWayFinder; }
	virtual void PlayDie(float timeElapsed) { timeElapsed; }
	virtual void PlayRemove(float timeElapsed, shared_ptr<CWayFinder> pWayFinder) { timeElapsed; pWayFinder; }

	virtual void RegenerateLookAt() {}

	virtual void SetHP(float maxHP, float curHP) { maxHP; curHP; }
	virtual void SetLevel(int level, int maxexp, int exp) { level; maxexp; exp; }

	virtual void SetCollisionSize(float size) { m_fCollisionSize = size; }
	virtual float GetCollisionSize() const { return m_fCollisionSize; }
	virtual XMFLOAT2 GetCollisionLevel() { return m_xmf2CollisionLevel; }
	virtual void ReceiveDamage(float Damage) { Damage; }
	virtual void NotifyDamager(CCollisionObject* other) { other; }
	virtual void NotifyDamageTeam(TeamType type) { type; }

	StatesType GetState() { return m_curState; }
	virtual void SetState(StatesType newState, shared_ptr<CWayFinder> pWayFinder = nullptr) { m_curState = newState; }

	void SetNextState(StatesType newState) { m_nextState = newState; }
	void SetEnemy(CCollisionObject* enemy) { m_pEnemy = enemy; }
	void SetEnemyByTag(short tag) { m_pEnemy = m_pColManager->RequestObjectByTag(tag); }

	virtual void SetCollisionManager(shared_ptr<CCollisionManager> manager) { m_pColManager = manager; }
	void ResetCollisionLevel() {
		m_xmf2CollisionLevel.x = floor(GetPosition().x / (TERRAIN_SIZE_WIDTH / 5000));
		m_xmf2CollisionLevel.y = floor(GetPosition().z / (TERRAIN_SIZE_WIDTH / 5000));
	}

	void SetThrowingManager(shared_ptr<CThrowingMgr> manager) { m_pThrowingMgr = manager; }
	void SetEffectManager(shared_ptr<CEffectMgr> manager) { m_pEffectMgr = manager; }

	void SetStatic(StaticType type) { m_StaticType = type; }
	StaticType GetStaticType() { return m_StaticType; }

	virtual PlayerInfo* GetPlayerStatus() { return NULL; }
	virtual CommonInfo* GetCommonStatus() { return NULL; }
	virtual StaticInfo* GetNexusAndTowerStatus() { return NULL; }

	virtual void SetFlyingObjectsType(FlyingObjectType type) { type; }
	virtual void SetEffectObjectsType(EffectObjectType type) { type; }

	virtual void SetAnimationTime(float time) { time; }
	virtual void SetAnimationLength(int time) { time; }

	virtual void SetDirection(const XMFLOAT3& direction) { direction; }
	virtual FlyingObjectType GetFlyingObjectsType() { return FlyingObjectType::Roider_Dumbel; }
	virtual EffectObjectType GetEffetObjectsType() { return EffectObjectType::Player_SwordSkill_Q_Effect; }

	float GetDetectRange() { return m_detectRange; }
	float GetSightRange() { return m_sightRange; }

	void SetDetected(bool value) { m_Detected = value; }
	bool GetDetected() { return m_Detected; }

	void GameOver() { m_GameOver = true; }

	void Activate() { m_Activated = true; }
	void Deactivate() { m_Activated = false; }

	void SetTag(short id) { m_tag = id; }
	short GetTag() { return m_tag; }

	void SetUpdateTime(float updatetime) { m_updatetime = updatetime; }
	float GetUpdateTime() { return m_updatetime; }

	virtual void ReadyToAtk(shared_ptr<CWayFinder> pWayFinder) { pWayFinder; }
	virtual void Respawn() {}

protected: // 내부 함수

protected: // 변수
	short m_tag{ 0 };
	float m_updatetime{ 0 };

	float m_fCollisionSize{ 1 };
	XMFLOAT2 m_xmf2CollisionLevel{ 0,0 };

	StatesType m_curState{ States::Idle };
	StatesType m_nextState{ States::Idle };
	bool m_Detected{ false };

	float m_sightRange{ 10.0f };
	float m_detectRange{ 0.0f };
	float m_attackRange{ 0.0f };
	float m_farAttackRange{ 0.0f };

	CCollisionObject* m_pEnemy{ NULL };
	StaticType m_StaticType{ StaticType::Move };
	shared_ptr<CCollisionManager> m_pColManager;
	shared_ptr<CThrowingMgr> m_pThrowingMgr;
	shared_ptr<CEffectMgr> m_pEffectMgr;

	bool m_GameOver{ false };

	bool m_Activated{ false };
};
