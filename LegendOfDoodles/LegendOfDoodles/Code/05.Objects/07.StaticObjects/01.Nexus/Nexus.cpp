#include "stdafx.h"
#include "Nexus.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"

/// <summary>
/// 목적: 넥서스 오브젝트 생성 및 동작관리
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-05-08
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CNexus::CNexus(CCreateMgr *pCreateMgr) : CCollisionObject(pCreateMgr)
{
}

CNexus::~CNexus()
{
}
