#pragma once
#include "05.Objects/02.CollisionObject/CollisionObject.h"

typedef std::list<CCollisionObject*> CollisionObjectList;

class CHPGaugeManager
{
public:
	CHPGaugeManager();
	~CHPGaugeManager();

	void AddMinionObject(CCollisionObject *pNewMinion);
	int GetCount() { return m_naddCount; };
	void ResetCount() { m_naddCount = 0; };

	CollisionObjectList *GetMinionObjectList() { return &m_MinionObjectList; };

private:
	CollisionObjectList m_MinionObjectList;
	int m_naddCount{ 0 };
};
