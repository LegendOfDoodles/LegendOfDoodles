#include "stdafx.h"
#include "Scene.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "04.Shaders/04.AniShader/AniShader.h"
#include "04.Shaders/05.PlayerShader/PlayerShader.h"
#include "04.Shaders/06.NexusTowerShader/NexusTowerShader.h"
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"
#include "00.Global/02.AI/00.FSMMgr/FSMMgr.h"

/// <summary>
/// 목적: 기본 씬, 인터페이스 용
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-05-11
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

void CScene::ProcessInput()
{
	static UCHAR pKeyBuffer[256];

	GetKeyboardState(pKeyBuffer);

	bool continual{ true };
	for (int i = 0; i < m_nShaders; ++i) {
		if(continual) continual = m_ppShaders[i]->OnProcessKeyInput(pKeyBuffer);
	}

	if (m_pSelectedObject && GetAsyncKeyState('K') & 0x0001)
	{
		if(m_pSelectedObject->GetState() != States::Die)
			m_pSelectedObject->SetState(States::Die);
	}
}

void CScene::AnimateObjects(float timeElapsed)
{
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->AnimateObjects(timeElapsed);
	}
	m_pCollisionManager->Update(m_pWayFinder);
}

void CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam)
{

	int ret = 0;
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		::SetCapture(hWnd);
		for (int i = 0; i < m_nShaders; ++i)
			m_ppShaders[i]->OnProcessMouseInput(wParam);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_MOUSEWHEEL:

		break;
	default:
		break;
	}
}

void CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam)
{
	if (nMessageID == WM_KEYUP)
	{
		OnProcessKeyUp(wParam, lParam);
		/*if(m_pSelectedObject != NULL && wParam >= 37 && wParam <=40)
			m_Network.ReadPacket(m_Network.m_mysocket, m_pSelectedObject);*/
	}

}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CScene::BuildObjects()
{
	m_pTerrain = new CHeightMapTerrain(_T("Resource/Terrain/HeightMap.raw"),
		TERRAIN_IMAGE_WIDTH, TERRAIN_IMAGE_HEIGHT,
		TERRAIN_IMAGE_WIDTH, TERRAIN_IMAGE_HEIGHT,
		TERRAIN_IMAGE_SCALE);

	m_nShaders = 3;
	m_ppShaders = new CShader*[m_nShaders];
	m_ppShaders[0] = new CNexusTowerShader();
	m_ppShaders[1] = new CAniShader();
	m_ppShaders[2] = new CPlayerShader();

	for (int i = 0; i < m_nShaders; ++i)
	{
		m_ppShaders[i]->Initialize(m_pTerrain);
	}

	m_pWayFinder = new CWayFinder(NODE_SIZE, NODE_SIZE);
	m_pCollisionManager = new CCollisionManager();
	m_pFSMMgr = new CFSMMgr(m_pWayFinder);

	CAniShader* pAniS = (CAniShader *)m_ppShaders[1];
	pAniS->SetCollisionManager(m_pCollisionManager);
	pAniS->SetFSMManager(m_pFSMMgr);

	CPlayerShader* pPlayerS = (CPlayerShader *)m_ppShaders[2];
	int nColliderObject = pPlayerS->GetObjectCount();
	for (int i = 0; i < nColliderObject; ++i)
	{
		m_pCollisionManager->AddCollider(((CCollisionObject * *)pPlayerS->GetCollisionObjects())[i]);
	}
	pPlayerS->SetColManagerToObject(m_pCollisionManager);

	CNexusTowerShader* pNTS = (CNexusTowerShader *)m_ppShaders[0];
	nColliderObject = pNTS->GetObjectCount();
	for (int i = 0; i < nColliderObject; ++i)
	{
		m_pCollisionManager->AddCollider(((CCollisionObject * *)pNTS->GetCollisionObjects())[i]);
	}
	pNTS->SetColManagerToObject(m_pCollisionManager);
}

void CScene::ReleaseObjects()
{
	if (m_pTerrain) Safe_Release(m_pTerrain);
	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			if (m_ppShaders[i]) m_ppShaders[i]->Finalize();
		}
		Safe_Delete_Array(m_ppShaders);
	}
	if (m_pWayFinder) Safe_Delete(m_pWayFinder);
	if (m_pCollisionManager) Safe_Delete(m_pCollisionManager);
	if (m_pFSMMgr) Safe_Delete(m_pFSMMgr);
}

// 플레이어 이동 시 사용 -> 입력 값 월드 포지션 패킷으로 받아서 적용
void CScene::GenerateLayEndWorldPosition(XMFLOAT3& pickPosition, XMFLOAT4X4& xmf4x4View)
{
	//CS_MsgChMove my_packet; //= reinterpret_cast<CS_MsgChMove *>(m_Network.m_send_buffer);
	//int ret = 0;
	//XMFLOAT4X4  inverseArr = Matrix4x4::Inverse(xmf4x4View);
	//XMFLOAT3 camPosition = m_pCamera->GetPosition();
	//XMFLOAT3 layWorldPosition = Vector3::TransformCoord(pickPosition, inverseArr);
	//XMFLOAT3 layDirection = Vector3::Subtract(layWorldPosition, camPosition);
	//float yDiff = abs(camPosition.y / layDirection.y);

	//m_pickWorldPosition = Vector3::Add(camPosition, Vector3::ScalarProduct(layDirection, yDiff, false));

	//if (m_pSelectedObject)
	//{
	//	m_pSelectedObject->LookAt(m_pickWorldPosition);
	//	m_pSelectedObject->SetPathToGo(m_pWayFinder->GetPathToPosition(
	//		XMFLOAT2(m_pSelectedObject->GetPosition().x, m_pSelectedObject->GetPosition().z),
	//		XMFLOAT2(m_pickWorldPosition.x, m_pickWorldPosition.z),
	//		m_pSelectedObject->GetCollisionSize()));
	//}
}

// Process Keyboard Input
void CScene::OnProcessKeyUp(WPARAM wParam, LPARAM lParam)
{
	if (wParam == VK_ESCAPE) ::PostQuitMessage(0);
}
