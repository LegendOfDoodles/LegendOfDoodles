#include "stdafx.h"
#include "Framework.h"
#include "03.Scenes/01.GameScene/GameScene.h"

/// <summary>
/// ����: �����ӿ�ũ Ŭ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-08-05
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CFramework::CFramework()
{
}

CFramework::~CFramework()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
bool CFramework::Initialize()
{
	BuildObjects();

	return(true);
}



void CFramework::Finalize()
{
	ReleaseObjects();
}

void CFramework::FrameAdvance(float timeElapsed)
{
	g_GameTime += timeElapsed;
	m_pScene->AnimateObjects(timeElapsed);
	// Warning! �ӽ� ���� Ȯ�� -> ��� ���� �ʿ�
	//m_running = !(GetAsyncKeyState(VK_ESCAPE) & 0x8000);
}



////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CFramework::BuildObjects()
{
	m_pScene = shared_ptr<CGameScene>(new CGameScene());
	m_pScene->Initialize();
}

void CFramework::ReleaseObjects()
{
	if (m_pScene)
	{
		m_pScene->Finalize();
	}
}