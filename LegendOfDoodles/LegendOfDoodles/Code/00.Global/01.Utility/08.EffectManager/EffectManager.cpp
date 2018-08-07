#include "stdafx.h"
#include "EffectManager.h"
#include "04.Shaders\09.EffectShader\EffectShader.h"

/// <summary>
/// ����: Effect ������ ��û�� ���� Ŭ����
/// ���� ������:  �̿뼱
/// ������ ���:  �̿뼱
/// ���� ���� ��¥: 2018-08-06
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CEffectMgr::CEffectMgr()
{
}

CEffectMgr::~CEffectMgr()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CEffectMgr::RequestSpawn(const XMFLOAT3 & position, const float positionOffset, const XMFLOAT3 & direction, TeamType teamType, EffectObjectType objectType)
{
	m_pEffectShader->SpawnEffectObject(position, positionOffset, direction, teamType, objectType);
}

void CEffectMgr::RequestPause()
{
	m_pEffectShader->Pause();
}

void CEffectMgr::RequestResume()
{
	m_pEffectShader->Resume();
}