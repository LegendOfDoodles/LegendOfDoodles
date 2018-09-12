#include "stdafx.h"
#include "Scene.h"

/// <summary>
/// 목적: 기본 씬, 인터페이스 용
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-09-12
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CScene::CScene()
{
}

CScene::~CScene()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
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
// 내부 함수
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
