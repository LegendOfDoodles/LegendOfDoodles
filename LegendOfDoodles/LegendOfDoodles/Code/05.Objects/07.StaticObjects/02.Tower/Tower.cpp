#include "stdafx.h"
#include "Tower.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"

/// <summary>
/// 목적: 타워 오브젝트 생성 및 동작관리
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-05-08
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CTower::CTower(CCreateMgr *pCreateMgr) : CCollisionObject(pCreateMgr)
{
}

CTower::~CTower()
{
}
