#include "stdafx.h"
#include "Framework.h"
#include "03.Scenes/01.GameScene/GameScene.h"

/// <summary>
/// ����: �����ӿ�ũ Ŭ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-09-18
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
	if (g_currentScene == SceneType::GameScene)
	{
		g_GameTime += timeElapsed;
		m_pScene->AnimateObjects(timeElapsed);

		if (g_GameFinished)
		{
			FinishGame();
		}
	}
}

void CFramework::StartGame()
{
	if (m_pScene) m_pScene->Finalize();
	m_pScene = shared_ptr<CGameScene>(new CGameScene());
	m_pScene->Initialize();
	ReadyForScene(m_pScene);
	g_currentScene = SceneType::GameScene;
}

void CFramework::FinishGame()
{
	if (m_pScene) m_pScene->Finalize();
	m_pScene = shared_ptr<CScene>(new CScene());
	m_pScene->Initialize();
	g_currentScene = SceneType::RoomScene;
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CFramework::BuildObjects()
{
	m_pScene = shared_ptr<CScene>(new CScene());
	m_pScene->Initialize();
}

void CFramework::ReleaseObjects()
{
	if (m_pScene)
	{
		m_pScene->Finalize();
	}
}