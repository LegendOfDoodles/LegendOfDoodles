#pragma once
#include "05.Objects/02.CollisionObject/CollisionObject.h"

typedef std::vector<CCollisionObject*> CollisionObjectList;

class CHPGaugeManager
{
public:
	CHPGaugeManager();
	~CHPGaugeManager();

	void AddMinionObject(CCollisionObject *pNewMinion);
	int GetCount() { return m_naddCount; };
	void ResetCount() { m_naddCount = 0; };

	void StartSpawn() { m_spawnStart = true; }
	void EndSpawn() { m_spawnStart = false; }

	bool SpawnStarted() { return m_spawnStart; }

	CollisionObjectList *GetMinionObjectList() { return &m_MinionObjectList; };

	void SetBlueMinionCnt(int* cnt) { m_npBlueMinions = cnt; }
	void SetRedMinionCnt(int* cnt) { m_npRedMinions = cnt; }

	int* GetBlueMinionCnt() { return m_npBlueMinions; }
	int* GetRedMinionCnt() { return m_npRedMinions; }

private:
	CollisionObjectList m_MinionObjectList;
	int m_naddCount{ 0 };

	bool m_spawnStart{ false };

	int *m_npBlueMinions{ NULL };
	int *m_npRedMinions{ NULL };
};
