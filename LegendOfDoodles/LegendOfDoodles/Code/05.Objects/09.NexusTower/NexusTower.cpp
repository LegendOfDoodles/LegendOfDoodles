#include "stdafx.h"
#include "NexusTower.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CNexusTower::CNexusTower(CCreateMgr *pCreateMgr, int nMeshes) : CCollisionObject(pCreateMgr, nMeshes)
{
	m_StatusInfo.maxHP = 1000;
	m_StatusInfo.HP = 1000;
	m_StatusInfo.Atk = 0;
	m_StatusInfo.Def= 0;
	m_StatusInfo.Exp = 0;

}


CNexusTower::~CNexusTower()
{
}

void CNexusTower::Animate(float timeElapsed)
{
	switch (m_curState)
	{
	case States::Idle:
		break;
	case States::Die:
		if (m_fEndTime <= 100) {

		m_fEndTime +=60* timeElapsed;
		Translate(new XMFLOAT3(0, -1, 0));
		Translate(new XMFLOAT3(0, sin(m_fEndTime * 2)*1.f, 0));
		Translate(new XMFLOAT3(sin(m_fEndTime * 2)*1.f, 0, 0));
		Translate(new XMFLOAT3(0,0,sin(m_fEndTime*2)*1.f));
		}
		break;
	case States::Remove:
		break;
	case States::Win:
		break;
	case States::Defeat:
		if (m_fEndTime <= 100) {

			m_fEndTime += 60 * timeElapsed;
			Translate(new XMFLOAT3(0, -1, 0));
			Translate(new XMFLOAT3(0, sin(m_fEndTime * 2)*1.f, 0));
			Translate(new XMFLOAT3(sin(m_fEndTime * 2)*1.f, 0, 0));
			Translate(new XMFLOAT3(0, 0, sin(m_fEndTime * 2)*1.f));
		}
		break;
	default:
		assert(!"Error:: There is No State");
	}
}

////////////////////////////////////////////////////////////////////////
// 공개 함수


////////////////////////////////////////////////////////////////////////
// 내부 함수