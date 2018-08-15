#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "05.Objects/97.Skeleton/Skeleton.h"

class CPlayer;
class CEquipment : public CAnimatedObject
{
public:
	CEquipment(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes = 1);
	~CEquipment();
	virtual void Render(CCamera *pCamera, UINT instanceCnt = 1);
	void SetMaster(CPlayer *player) { m_pPlayer = player; }
	CPlayer* GetMaster() { return m_pPlayer; }

	void SetMaterialIndex(UINT index) { m_nMaterialIndex = index; }
	UINT GetMaterialIndex() { return m_nMaterialIndex; }
private:
	virtual void AdjustAnimationIndex();
	CPlayer * m_pPlayer;
	UINT m_nMaterialIndex;
};

