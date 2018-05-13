#include "stdafx.h"
#include "CollisionObject.h"

/// <summary>
/// 목적: 충돌체가 있는 오브젝트 구분용 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-05-12
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CCollisionObject::CCollisionObject(CCreateMgr * pCreateMgr, int nMeshes) : CBaseObject(pCreateMgr, nMeshes)
{
}

CCollisionObject::~CCollisionObject()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
