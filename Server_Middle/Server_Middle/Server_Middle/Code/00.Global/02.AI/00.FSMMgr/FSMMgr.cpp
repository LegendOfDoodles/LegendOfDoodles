#include "stdafx.h"
#include "FSMMgr.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"

/// <summary>
/// 목적: FSM을 하나의 클래스로 관리하여 처리하기 위함
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-05-11
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CFSMMgr::CFSMMgr(CWayFinder* pWayFinder)
{
	m_pWayFinder = pWayFinder;
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
		obj->PlayWalk(timeElapsed);
		break;
	case States::Chase:
		obj->PlayChase(timeElapsed, m_pWayFinder);
		break;
	case States::Attack:
		obj->PlayAttack(timeElapsed);
		break;
	case States::Die:
		obj->PlayDie(timeElapsed);
		break;
	case States::Remove:
		obj->PlayRemove(timeElapsed);
		break;
	default:
		assert(!"Error:: There is No State");
	}
	obj->Animate(timeElapsed);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CFSMMgr::PlayIdle(float timeElapsed, CCollisionObject * obj)
{
}

void CFSMMgr::PlayWalk(float timeElapsed, CCollisionObject * obj)
{
}

void CFSMMgr::PlayChase(float timeElapsed, CCollisionObject * obj)
{
}

void CFSMMgr::PlayAttack(float timeElapsed, CCollisionObject * obj)
{
}

void CFSMMgr::PlayDie(float timeElapsed, CCollisionObject * obj)
{
}

void CFSMMgr::PlayRemove(float timeElapsed, CCollisionObject * obj)
{
}
