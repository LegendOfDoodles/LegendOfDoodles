#pragma once
#include "05.Objects/02.CollisionObject/CollisionObject.h"

class CCollisionManager;
class CHeightMapTerrain;

class CFlyingObject : public CCollisionObject
{
public:
	CFlyingObject(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes = 1);
	virtual ~CFlyingObject();

public:	// 외부 함수
	virtual void Animate(float timeElapsed);

	virtual void SetFlyingObjectsType(FlyingObjectType type);
	virtual void SetDirection(const XMFLOAT3& direction);

	virtual void SetCollisionManager(shared_ptr<CCollisionManager> manager) { m_pColManager = manager; }
	virtual void SetEffectManager(shared_ptr<CEffectMgr> manager) { m_pEffectMgr = manager; }

	virtual FlyingObjectType GetFlyingObjectsType() { return m_flyingObjectType; }

	void SetTerrainImage(CHeightMapTerrain * pTerrain) { m_pTerrain = pTerrain; }

	virtual void SetDamage(float dmg) { m_damage = dmg; }

protected: // 내부 함수
	void LookAt(XMFLOAT3 objPosition);
	void LookAt(XMFLOAT2 objPosition);

	void MoveToDirection(float dist);

protected: // 변수
	FlyingObjectType m_flyingObjectType;
	float	m_damage{ 0.0f };
	float	m_speed{ 0.0f };
	float	m_distance{ 0.0f };
	float	m_maxDistance{ 0.0f };
	bool	m_EffectTriger{ true };
	bool	m_SoundTrigier{ true };
	XMFLOAT3 m_direction{};

	shared_ptr<CCollisionManager> m_pColManager{ NULL };

	CHeightMapTerrain * m_pTerrain{ NULL };
};

