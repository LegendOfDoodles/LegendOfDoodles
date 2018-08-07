#include "stdafx.h"
#include "FSMMgr.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"

/// <summary>
/// ����: FSM�� �ϳ��� Ŭ������ �����Ͽ� ó���ϱ� ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-08-05
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CFSMMgr::CFSMMgr(shared_ptr<CWayFinder> pWayFinder)
	:m_pWayFinder(pWayFinder)
{
}

CFSMMgr::~CFSMMgr()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
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
// ���� �Լ�