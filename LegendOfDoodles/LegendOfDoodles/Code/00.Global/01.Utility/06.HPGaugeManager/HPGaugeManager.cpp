#include "stdafx.h"
#include "HPGaugeManager.h"

CUIObjectManager::CUIObjectManager()
{
}

CUIObjectManager::~CUIObjectManager()
{
}

void CUIObjectManager::AddMinionObject(CCollisionObject * pNewMinion)
{
	if (pNewMinion) {
		m_MinionObjectList.push_back(pNewMinion);

		m_naddCount++;
	}
}