#include "stdafx.h"
#include "ThrowingMgr.h"
#include "04.Shaders/08.FlyingShader/FlyingShader.h"

/// <summary>
/// ����: ���ư��� ������Ʈ(ȭ��, �Ʒ� ��)�� ���� ��û ������ ���� Ŭ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-07-27
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CThrowingMgr::CThrowingMgr()
{
}

CThrowingMgr::~CThrowingMgr()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CThrowingMgr::RequestSpawn(const XMFLOAT3 & position, const float positionOffset, const XMFLOAT3 & direction, TeamType teamType, FlyingObjectType objectType)
{
	m_pFlyingShader->SpawnFlyingObject(position, positionOffset, direction, teamType, objectType);
}

void CThrowingMgr::RequestPause()
{
	m_pFlyingShader->Pause();
}

void CThrowingMgr::RequestResume()
{
	m_pFlyingShader->Resume();
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
