#include "stdafx.h"
#include "Scene.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "04.Shaders/04.AniShader/MinionShader.h"
#include "04.Shaders/05.PlayerShader/PlayerShader.h"
#include "04.Shaders/06.NexusTowerShader/NexusTowerShader.h"
#include "04.Shaders/07.NeutralityShader/NeutralityShader.h"
#include "04.Shaders/08.FlyingShader/FlyingShader.h"
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"
#include "00.Global/01.Utility/07.ThrowingManager/ThrowingMgr.h"
#include "00.Global/02.AI/00.FSMMgr/FSMMgr.h"

/// <summary>
/// 목적: 기본 씬, 인터페이스 용
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-08-31
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
	g_GameTime += timeElapsed;

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->AnimateObjects(timeElapsed);
	}

	if (m_pCollisionManager)
	{
		m_pCollisionManager->Update(m_pWayFinder);
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CScene::BuildObjects()
{
	m_pTerrain = new CHeightMapTerrain(_T("Resource/Terrain/HeightMap.raw"), TERRAIN_IMAGE_SCALE);

	m_nShaders = 5;
	m_ppShaders = new CShader*[m_nShaders];
	m_ppShaders[0] = new CMinionShader();
	m_ppShaders[1] = new CPlayerShader();
	m_ppShaders[2] = new CNeutralityShader();
	m_ppShaders[3] = new CNexusTowerShader();
	m_ppShaders[4] = new CFlyingShader();

	for (int i = 0; i < m_nShaders; ++i)
	{
		m_ppShaders[i]->Initialize(m_pTerrain);
	}

	//Managere Initialize
	m_pWayFinder = shared_ptr<CWayFinder>(new CWayFinder());
	m_pCollisionManager = shared_ptr<CCollisionManager>(new CCollisionManager());
	m_pThrowingMgr = shared_ptr<CThrowingMgr>(new CThrowingMgr());
	m_pThrowingMgr->SetFlyingShader(static_cast<CFlyingShader*>(m_ppShaders[4]));
	m_pFSMMgr = shared_ptr<CFSMMgr>(new CFSMMgr(m_pWayFinder));

	//Manager Shaders Setting
	CMinionShader* pMinionS = (CMinionShader *)m_ppShaders[0];

	pMinionS->SetCollisionManager(m_pCollisionManager);
	pMinionS->SetFSMManager(m_pFSMMgr);
	pMinionS->SetThrowingManager(m_pThrowingMgr);

	m_pCollisionManager->SetNodeMap(m_pWayFinder->GetNodeMap(), m_pWayFinder->GetNodeSize(), m_pWayFinder->GetNodeWH());

	CPlayerShader* pPlayerS = (CPlayerShader *)m_ppShaders[1];
	int nColliderObject = pPlayerS->GetObjectCount();
	for (int i = 0; i < nColliderObject; ++i)
	{
		m_pCollisionManager->AddCollider((pPlayerS->GetCollisionObjects())[i]);
	}
	pPlayerS->SetColManagerToObject(m_pCollisionManager);
	pPlayerS->SetThrowingManagerToObject(m_pThrowingMgr);

	// 중립 몬스터에 충돌체 부여
	CNeutralityShader* pNetral = (CNeutralityShader *)m_ppShaders[2];
	nColliderObject = pNetral->GetObjectCount();
	for (int i = 0; i < nColliderObject; ++i)
	{
		m_pCollisionManager->AddCollider((pNetral->GetCollisionObjects())[i]);
	}
	pNetral->SetColManagerToObject(m_pCollisionManager);
	pNetral->SetFSMManager(m_pFSMMgr);
	pNetral->SetThrowingManagerToObject(m_pThrowingMgr);

	CNexusTowerShader* pNTS = (CNexusTowerShader *)m_ppShaders[3];
	nColliderObject = pNTS->GetObjectCount();
	for (int i = 0; i < nColliderObject; ++i)
	{
		m_pCollisionManager->AddCollider((pNTS->GetCollisionObjects())[i]);
	}
	pNTS->SetColManagerToObject(m_pCollisionManager);
	pNTS->SetFSMManager(m_pFSMMgr);
	pNTS->SetThrowingManagerToObject(m_pThrowingMgr);

	CFlyingShader* pFS = (CFlyingShader*)m_ppShaders[4];
	pFS->SetColManagerToObject(m_pCollisionManager);
}

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


// 플레이어 이동 시 사용 -> 입력 값 월드 포지션 패킷으로 받아서 적용
void CScene::GenerateLayEndWorldPosition(XMFLOAT3& pickPosition, int id)
{
	XMFLOAT3 pickWorldPosition = pickPosition;
	CAnimatedObject* pPlayer = ((CAnimatedObject * *)m_ppShaders[1]->GetCollisionObjects())[id];
	pPlayer->LookAt(pickWorldPosition);
	pPlayer->SetPathToGo(m_pWayFinder->GetPathToPosition(
		pPlayer->GetPosition(),
		pickWorldPosition));
}