#include "stdafx.h"
#include "Obstacle.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"

/// <summary>
/// 목적: 장애물 오브젝트 생성 시 구분 용도
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-05-04
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CObstacle::CObstacle(CCreateMgr *pCreateMgr) : CBaseObject(pCreateMgr)
{
}

CObstacle::~CObstacle()
{
}
