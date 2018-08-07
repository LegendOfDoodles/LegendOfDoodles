#include "stdafx.h"
#include "FSMMgr.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"

/// <summary>
/// 목적: FSM을 하나의 클래스로 관리하여 처리하기 위함
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-08-05
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CFSMMgr::CFSMMgr(shared_ptr<CWayFinder> pWayFinder)
	:m_pWayFinder(pWayFinder)
{
}

CFSMMgr::~CFSMMgr()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CFSMMgr::Update(float timeElapsed, CCollisionObject * obj)
{
	switch (obj->GetState())
	{
	case States::Idle:
		obj->PlayIdle(timeElapsed);
		break;
	case States::Walk:
		obj->PlayWalk(timeElapsed, m_pWayFinder);
		break;
	case States::Chase:
		obj->PlayChase(timeElapsed, m_pWayFinder);
		break;
	case States::Attack:
		obj->PlayAttack(timeElapsed, m_pWayFinder);
		break;
	case States::Die:
		obj->PlayDie(timeElapsed);
		break;
	case States::Remove:
		obj->PlayRemove(timeElapsed, m_pWayFinder);
		break;
	case States::Win:
		break;
	case States::Defeat:
		break;
	default:
		assert(!"Error:: There is No State");
	}
	obj->Animate(timeElapsed);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수