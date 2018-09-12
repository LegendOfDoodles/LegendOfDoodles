#include "stdafx.h"
#include "Scene.h"

/// <summary>
/// ����: �⺻ ��, �������̽� ��
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-09-12
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CScene::CScene()
{
}

CScene::~CScene()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CScene::Initialize()
{
	BuildObjects();
}

void CScene::Finalize()
{
	ReleaseObjects();
}

void CScene::AnimateObjects(float timeElapsed)
{
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->AnimateObjects(timeElapsed);
	}
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CScene::ReleaseObjects()
{
	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			if (m_ppShaders[i]) m_ppShaders[i]->Finalize();
		}
		Safe_Delete_Array(m_ppShaders);
	}
}
