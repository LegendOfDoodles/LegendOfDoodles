#include "stdafx.h"
#include "Scene.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "04.Shaders/01.ObjectShader/StaticObjectShader.h"
#include "04.Shaders/02.TerrainShader/TerrainShader.h"
#include "04.Shaders/03.SkyBoxShader/SkyBoxShader.h"
#include "04.Shaders/04.AniShader/AniShader.h"
#include "04.Shaders/05.PlayerShader/PlayerShader.h"
#include "04.Shaders/06.NexusTowerShader/NexusTowerShader.h"
#include "04.Shaders/98.BillboardShader/00.UIShader/UIShader.h"
#include "04.Shaders/98.BillboardShader/02.IconShader/MinimapIconShader.h"
#include "04.Shaders/98.BillboardShader/01.GaugeShader/00.PlayerGaugeShader/PlayerGaugeShader.h"
#include "04.Shaders/98.BillboardShader/01.GaugeShader/01.MinionGaugeShader/MinionGaugeShader.h"
#include "05.Objects/01.Camera/01.AOSCamera/AOSCamera.h"
#include "00.Global/01.Utility/06.HPGaugeManager/HPGaugeManager.h"

/// <summary>
/// 목적: 기본 씬, 인터페이스 용
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-05-22
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
void CScene::Initialize(CCreateMgr *pCreateMgr, Network* pNetwork)
{
	m_pNetwork = pNetwork;
	BuildObjects(pCreateMgr);
	CreateShaderVariables(pCreateMgr);
	//m_pNetwork->ReadPacket(m_pNetwork->m_mysocket, NULL );
}

void CScene::Finalize()
{
	ReleaseObjects();
	ReleaseShaderVariables();
}

void CScene::ReleaseUploadBuffers()
{
	if (m_ppShaders)
	{
		for (int j = 0; j < m_nShaders; j++)
		{
			if (m_ppShaders[j])
			{
				m_ppShaders[j]->ReleaseUploadBuffers();
			}
		}
	}
	if (m_pCubeMap) m_pCubeMap->ReleaseUploadBuffers();
}

void CScene::ProcessInput()
{
	static UCHAR pKeyBuffer[256];

	GetKeyboardState(pKeyBuffer);

	bool continual = m_pCamera->OnProcessMouseInput(pKeyBuffer);
	if(continual) continual = m_pCamera->OnProcessKeyInput(pKeyBuffer);
	for (int i = 0; i < m_nShaders; ++i) {
		if(continual)
			continual = m_ppShaders[i]->OnProcessKeyInput(pKeyBuffer);
	}
}

void CScene::AnimateObjects(float timeElapsed)
{
	m_pCamera->Update(timeElapsed);

	UpdateShaderVariables();

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->AnimateObjects(timeElapsed);
	}
}

void CScene::Render()
{
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	m_pCommandList->SetGraphicsRootConstantBufferView(4, d3dcbLightsGpuVirtualAddress); //Lights

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->Render(m_pCamera);
	}

	if (m_bRenderBoundingBox)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			if (m_ppShaders[i]->HasBoundingBox())
				m_ppShaders[i]->RenderBoundingBox(m_pCamera);
		}
	}
}

void CScene::RenderWithLights()
{
	UpdateShaderVariables();
	m_pCamera->UpdateShaderVariables();

	if (m_pCubeMap)
	{
		m_pCommandList->SetDescriptorHeaps(1, &m_pCbvSrvDescriptorHeap);
		m_pCubeMap->UpdateShaderVariables();
	}

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	m_pCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights
}

void CScene::SetViewportsAndScissorRects()
{
	m_pCamera->SetViewportsAndScissorRects();
}

void CScene::UpdateCamera()
{
	if (m_bCamChanged)
	{
		m_pCamera->Finalize();
		Safe_Delete(m_pCamera);

		if (m_bCurCamIsAOS)
		{
			m_pCamera = new  CAOSCamera();
		}
		else
		{
			m_pCamera = new CCamera();
		}

		m_pCamera->Initialize(m_pCreateMgr);

		static_cast<CUIObjectShader*>(m_ppShaders[6])->GetCamera(m_pCamera);
		static_cast<CPlayerHPGaugeShader*>(m_ppShaders[7])->GetCamera(m_pCamera);
		static_cast<CMinionHPGaugeShader*>(m_ppShaders[8])->GetCamera(m_pCamera);

		m_bCamChanged = false;
	}
	m_pCamera->UpdateShaderVariables();
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
		m_pCamera->SavePickedPos();
		PickObjectPointedByCursor(wParam, lParam);
		for (int i = 0; i < m_nShaders; ++i)
			m_ppShaders[i]->OnProcessMouseInput(wParam);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_MOUSEWHEEL:
		m_pCamera->OnProcessMouseWheel(wParam, lParam);
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
void CScene::CreateCbvAndSrvDescriptorHeap(CCreateMgr *pCreateMgr, int nConstantBufferViews, int nShaderResourceViews)
{
	UINT incrementSize = pCreateMgr->GetCbvSrvDescriptorIncrementSize();
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc;
	descriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descriptorHeapDesc.NodeMask = 0;
	HRESULT hResult = pCreateMgr->GetDevice()->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_pCbvSrvDescriptorHeap));
	assert(hResult == S_OK && "pCreateMgr->GetDevice()->CreateDescriptorHeap Failed");

	m_cbvCPUDescriptorStartHandle = m_pCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_cbvGPUDescriptorStartHandle = m_pCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_srvCPUDescriptorStartHandle.ptr = m_cbvCPUDescriptorStartHandle.ptr + (incrementSize * nConstantBufferViews);
	m_srvGPUDescriptorStartHandle.ptr = m_cbvGPUDescriptorStartHandle.ptr + (incrementSize * nConstantBufferViews);
}

void CScene::BuildLights()
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));

	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[0].m_color = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights->m_pLights[0].m_direction = Vector3::Normalize(XMFLOAT3(-1.0f, -0.3f, 1.0f));

	m_pLights->m_pLights[1].m_bEnable = true;
	m_pLights->m_pLights[1].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[1].m_color = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights->m_pLights[1].m_direction = Vector3::Normalize(XMFLOAT3(1.0f, -0.6f, 0.0f));

	m_pLights->m_pLights[2].m_bEnable = true;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_color = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights->m_pLights[2].m_direction = Vector3::Normalize(XMFLOAT3(0.0f, -0.6f, -1.0f));

	m_pLights->m_pLights[3].m_bEnable = true;
	m_pLights->m_pLights[3].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[3].m_color = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights->m_pLights[3].m_direction = Vector3::Normalize(XMFLOAT3(-1.0f, -0.6f, 0.0f));

	m_pLights->m_pLights[4].m_bEnable = true;
	m_pLights->m_pLights[4].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[4].m_color = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights->m_pLights[4].m_direction = Vector3::Normalize(XMFLOAT3(-1.0f, -0.3f, -1.0f));

	m_pLights->m_pLights[5].m_bEnable = true;
	m_pLights->m_pLights[5].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[5].m_color = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights->m_pLights[5].m_direction = Vector3::Normalize(XMFLOAT3(0.0f, -0.6f, 1.0f));

	m_pLights->m_pLights[6].m_bEnable = true;
	m_pLights->m_pLights[6].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[6].m_color = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights->m_pLights[6].m_direction = Vector3::Normalize(XMFLOAT3(1.0f, -0.3f, -1.0f));

	m_pLights->m_pLights[7].m_bEnable = true;
	m_pLights->m_pLights[7].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[7].m_color = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights->m_pLights[7].m_direction = Vector3::Normalize(XMFLOAT3(1.0f, -0.3f, 1.0f));

	CreateCbvAndSrvDescriptorHeap(m_pCreateMgr, 0, 1);
	m_pCubeMap = Materials::CreateCubeMapMaterial(m_pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
}

void CScene::BuildObjects(CCreateMgr *pCreateMgr)
{
	m_pCreateMgr = pCreateMgr;

	m_hWnd = pCreateMgr->GetHwnd();
	m_pCommandList = pCreateMgr->GetCommandList();

	m_pCamera = new  CAOSCamera();

	m_pCamera->Initialize(pCreateMgr);

	CTerrainShader* pTerrainShader = new CTerrainShader(pCreateMgr);
	
	m_nShaders = 10;
	m_ppShaders = new CShader*[m_nShaders];
	m_ppShaders[0] = new CSkyBoxShader(pCreateMgr);
	m_ppShaders[1] = pTerrainShader;
	m_ppShaders[2] = new CPlayerShader(pCreateMgr, m_pNetwork);
	m_ppShaders[3] = new CAniShader(pCreateMgr, m_pNetwork);
	m_ppShaders[4] = new CStaticObjectShader(pCreateMgr);
	m_ppShaders[5] = new CNexusTowerShader(pCreateMgr, m_pNetwork);
	
	// UI Shader
	m_ppShaders[6] = new CUIObjectShader(pCreateMgr);
	m_ppShaders[7] = new CPlayerHPGaugeShader(pCreateMgr);
	m_ppShaders[8] = new CMinionHPGaugeShader(pCreateMgr);
	m_ppShaders[9] = new CMinimapIconShader(pCreateMgr);

	// Manager Init
	m_pHPGaugeManager = new CUIObjectManager();
	static_cast<CAniShader*>(m_ppShaders[3])->SetGaugeManager(m_pHPGaugeManager);
	static_cast<CMinionHPGaugeShader*>(m_ppShaders[8])->SetGaugeManager(m_pHPGaugeManager);
	static_cast<CMinimapIconShader*>(m_ppShaders[9])->SetUIObjectsManager(m_pHPGaugeManager);

	// Object Shaders Init
	for (int i = 0; i < 2; ++i)
	{
		m_ppShaders[i]->Initialize(pCreateMgr);
	}

	for (int i = 2; i < m_nShaders - 4; ++i)
	{
		m_ppShaders[i]->Initialize(pCreateMgr, pTerrainShader->GetTerrain());
	}

	// UI Shaders Init
	((CPlayerHPGaugeShader*)m_ppShaders[7])->SetPlayerCnt(((CPlayerShader *)m_ppShaders[2])->GetObjectCount());
	((CPlayerHPGaugeShader*)m_ppShaders[7])->SetPlayer(((CPlayerShader *)m_ppShaders[2])->GetCollisionObjects());
	((CPlayerHPGaugeShader*)m_ppShaders[7])->SetNexusAndTowerCnt(((CNexusTowerShader *)m_ppShaders[5])->GetObjectCount());
	((CPlayerHPGaugeShader*)m_ppShaders[7])->SetNexusAndTower(((CNexusTowerShader *)m_ppShaders[5])->GetCollisionObjects());

	((CMinimapIconShader*)m_ppShaders[9])->SetPlayerCnt(((CPlayerShader *)m_ppShaders[2])->GetObjectCount());
	((CMinimapIconShader*)m_ppShaders[9])->SetPlayer(((CPlayerShader *)m_ppShaders[2])->GetCollisionObjects());
	((CMinimapIconShader*)m_ppShaders[9])->SetNexusAndTowerCnt(((CNexusTowerShader *)m_ppShaders[5])->GetObjectCount());
	((CMinimapIconShader*)m_ppShaders[9])->SetNexusAndTower(((CNexusTowerShader *)m_ppShaders[5])->GetCollisionObjects());

	m_ppShaders[6]->Initialize(pCreateMgr, m_pCamera);
	m_ppShaders[7]->Initialize(pCreateMgr, m_pCamera);
	m_ppShaders[8]->Initialize(pCreateMgr, m_pCamera);
	m_ppShaders[9]->Initialize(pCreateMgr, m_pCamera);

	BuildLights();
}

void CScene::ReleaseObjects()
{
	if (m_pCamera)
	{
		m_pCamera->Finalize();
		Safe_Delete(m_pCamera);
	}
	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			if (m_ppShaders[i]) m_ppShaders[i]->Finalize();
		}
		Safe_Delete_Array(m_ppShaders);
	}
	if (m_pHPGaugeManager) Safe_Delete(m_pHPGaugeManager);

	if (m_pCubeMap) Safe_Delete(m_pCubeMap);
}

void CScene::CreateShaderVariables(CCreateMgr *pCreateMgr)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = pCreateMgr->CreateBufferResource(NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);
}

void CScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		Safe_Release(m_pd3dcbLights);
	}
}

void CScene::UpdateShaderVariables()
{
	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
}

void CScene::PickObjectPointedByCursor(WPARAM wParam, LPARAM lParam)
{
	int xClient{ LOWORD(lParam) }, yClient{ HIWORD(lParam) };

	XMFLOAT4X4 xmf4x4View = m_pCamera->GetViewMatrix();
	XMFLOAT4X4 xmf4x4Projection = m_pCamera->GetProjectionMatrix();
	D3D12_VIEWPORT viewport = m_pCamera->GetViewport();

	XMFLOAT3 pickPosition;
	pickPosition.x = (((2.0f * xClient) / viewport.Width) - 1) / xmf4x4Projection._11;
	pickPosition.y = -(((2.0f * yClient) / viewport.Height) - 1) / xmf4x4Projection._22;
	pickPosition.z = 1.0f;

	int nIntersected{ 0 };
	float hitDistance{ FLT_MAX }, nearestHitDistance{ FLT_MAX };
	CBaseObject *pIntersectedObject{ NULL };
	//m_pSelectedObject = NULL;

	for (int i = 0; i < m_nShaders; i++)
	{
		pIntersectedObject = m_ppShaders[i]->PickObjectByRayIntersection(pickPosition, xmf4x4View, hitDistance);
		if (pIntersectedObject && (hitDistance < nearestHitDistance))
		{
			nearestHitDistance = hitDistance;
			m_pSelectedObject = reinterpret_cast<CAnimatedObject*>(pIntersectedObject);
			printf("selected!\n");

			// Status 창 띄우기 수도 코드
			// 현재 6번 쉐이더가 UI 이므로 상호작용하는 Object의 타입을 받아와서
			// 그 해당 오브젝트에 대한 정보를 출력
			m_ppShaders[6]->OnStatus(pIntersectedObject->GetType());

			//m_Network.StartRecv(m_pSelectedObject);
		}
	}

	if (wParam == MK_RBUTTON)
	{
		GenerateLayEndWorldPosition(pickPosition, xmf4x4View);
	}
	else if (wParam == MK_LBUTTON) {
		// 바닥 선택시 Status창 Off
		//m_ppShaders[7]->OffStatus();
	}
}

void CScene::GenerateLayEndWorldPosition(XMFLOAT3& pickPosition, XMFLOAT4X4&	 xmf4x4View)
{
	CS_MsgChMove my_packet; //= reinterpret_cast<CS_MsgChMove *>(m_Network.m_send_buffer);
	int ret = 0;
	XMFLOAT4X4  inverseArr = Matrix4x4::Inverse(xmf4x4View);
	XMFLOAT3 camPosition = m_pCamera->GetPosition();
	XMFLOAT3 layWorldPosition = Vector3::TransformCoord(pickPosition, inverseArr);
	XMFLOAT3 layDirection = Vector3::Subtract(layWorldPosition, camPosition);
	float yDiff = abs(camPosition.y / layDirection.y);

	m_pickWorldPosition = Vector3::Add(camPosition, Vector3::ScalarProduct(layDirection, yDiff, false));

	my_packet.Character_id = m_pNetwork->m_myid;
	my_packet.x = m_pickWorldPosition.x;
	my_packet.y = m_pickWorldPosition.z;
	my_packet.type = CS_MOVE_PLAYER;
	my_packet.size = sizeof(my_packet);
	m_pNetwork->SendPacket(m_pNetwork->m_myid, &my_packet);

	//if (m_pSelectedObject)
	//{
	//	m_pSelectedObject->LookAt(m_pickWorldPosition);
	//	m_pSelectedObject->SetPathToGo(m_pWayFinder->GetPathToPosition(
	//		XMFLOAT2(m_pSelectedObject->GetPosition().x, m_pSelectedObject->GetPosition().z),
	//		XMFLOAT2(m_pickWorldPosition.x, m_pickWorldPosition.z),
	//		m_pSelectedObject->GetCollisionSize()));
	//	//my_packet.Character_id = m_pNetwork->m_myid;

	//	//my_packet.size = sizeof(my_packet);
	//	//my_packet.x = m_pickWorldPosition.x;
	//	//my_packet.y = m_pickWorldPosition.z;
	//	//m_pNetwork->m_send_wsabuf.len = sizeof(my_packet);
	//	//DWORD iobyte;
	//	//my_packet.type = CS_MOVE_PLAYER;
	//	//memcpy(m_pNetwork->m_send_buffer, &my_packet, sizeof(my_packet));
	//	//m_pNetwork->SendPacket(m_pNetwork->m_myid, &my_packet);
	//	//m_pNetwork->ReadPacket(m_pNetwork->m_mysocket, (CBaseObject**)m_pSelectedObject);
	//}
}

// Process Keyboard Input
void CScene::OnProcessKeyUp(WPARAM wParam, LPARAM lParam)
{

	int x = 0, y = 0;
	if (wParam == VK_ESCAPE)
		::PostQuitMessage(0);
	else if (wParam == VK_F1)
	{
		if (!m_bCurCamIsAOS)
		{
			m_bCurCamIsAOS = true;
			m_bCamChanged = true;
		}
	}
	else if (wParam == VK_F2)
	{
		if (m_bCurCamIsAOS)
		{
			m_bCurCamIsAOS = false;
			m_bCamChanged = true;
		}
	}
	else if (wParam == VK_F3)
	{
		m_bRenderBoundingBox = !m_bRenderBoundingBox;
	}
}
