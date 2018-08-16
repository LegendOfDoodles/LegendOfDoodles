#include "stdafx.h"
#include "Scene.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "04.Shaders/01.ObjectShader/StaticObjectShader.h"
#include "04.Shaders/02.TerrainShader/TerrainShader.h"
#include "04.Shaders/03.SkyBoxShader/SkyBoxShader.h"
#include "04.Shaders/04.AniShader/MinionShader.h"
#include "04.Shaders/05.PlayerShader/PlayerShader.h"
#include "04.Shaders/06.NexusTowerShader/NexusTowerShader.h"
#include "04.Shaders/07.NeutralityShader/NeutralityShader.h"
#include "04.Shaders/08.FlyingShader/FlyingShader.h"
#include "04.Shaders/10.EquipShader/EquipShader.h"
#include "04.Shaders/98.BillboardShader/00.UIShader/UIShader.h"
#include "04.Shaders/98.BillboardShader/01.GaugeShader/00.PlayerGaugeShader/PlayerGaugeShader.h"
#include "04.Shaders/98.BillboardShader/01.GaugeShader/01.MinionGaugeShader/MinionGaugeShader.h"
#include "04.Shaders/98.BillboardShader/01.GaugeShader/02.NexusAndTowerGaugeShader/NexusAndTowerGaugeShader.h"
#include "04.Shaders/98.BillboardShader/01.GaugeShader/03.CharacterGaugeShader/CharacterFrameGaugeShader.h"
#include "04.Shaders/98.BillboardShader/01.GaugeShader/04.NeutralityGaugeShader/NeutralityGaugeShader.h"
#include "04.Shaders/98.BillboardShader/02.IconShader/00.MinimapIconShader/MinimapIconShader.h"
#include "04.Shaders/98.BillboardShader/02.IconShader/01.BuildingMinimapIconShader/BuildingMinimapIconShader.h"
#include "04.Shaders/98.BillboardShader/03.MinimapUIShader/MinimapShader.h"
#include "04.Shaders/98.BillboardShader/04.SkillShader/SkillShader.h"
#include "04.Shaders/98.BillboardShader/98.NumberShader/NumberShader.h"
#include "04.Shaders/98.BillboardShader/05.SpecialShader/01.SpecialSelectShader/SpecialSelectShader.h"
#include "04.Shaders/98.BillboardShader/05.SpecialShader/02.SelectedSpecialShader/SelectedSpecialShader.h"
#include "04.Shaders/09.EffectShader/EffectShader.h"
#include "05.Objects/01.Camera/01.AOSCamera/AOSCamera.h"
#include "05.Objects/01.Camera/02.LightCamera/LightCamera.h"
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"
#include "00.Global/01.Utility/06.HPGaugeManager/HPGaugeManager.h"
#include "00.Global/01.Utility/07.ThrowingManager/ThrowingMgr.h"
#include "00.Global/01.Utility/08.EffectManager/EffectManager.h"
#include "00.Global/02.AI/00.FSMMgr/FSMMgr.h"
#include "00.Global/01.Utility/09.SoundManager/SoundManager.h"

/// <summary>
/// 목적: 기본 씬, 인터페이스 용
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-08-05
/// </summary>

#define UI_Shader m_ppShaders[8]
#define PlayerHP_Shader m_ppShaders[9]
#define MinionHP_Shader m_ppShaders[10]
#define MinimapIco_Shader m_ppShaders[11]
#define BuildingMapIco_Shader m_ppShaders[12]
#define CharFrame_Shader m_ppShaders[13]
#define Minimap_Shader m_ppShaders[14]
#define Skill_Shader m_ppShaders[15]
#define Number_Shader m_ppShaders[16]
#define SelectedSpecial_Shader m_ppShaders[17]
#define NexusTowerHP_Shader m_ppShaders[18]
#define NeutralityHP_Shader m_ppShaders[19]
#define Eeffect_Shader m_ppShaders[20]
#define SpecialSelect_Shader m_ppShaders[21]

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
void CScene::Initialize(shared_ptr<CCreateMgr> pCreateMgr, shared_ptr<CNetwork> pNetwork)
{
	m_pNetwork = pNetwork;
	m_pNetwork->SetScene(shared_from_this());
	BuildObjects(pCreateMgr);
	m_pNetwork->PrepareData();
	CreateShaderVariables(pCreateMgr);
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
	if (m_pSketchEffect) m_pSketchEffect->ReleaseUploadBuffers();
}

void CScene::ProcessInput()
{
	if (m_pCollisionManager->IsGameOver()) return;

	UCHAR pKeyBuffer[256];

	GetKeyboardState(pKeyBuffer);

	bool continual = m_pCamera->OnProcessMouseInput(pKeyBuffer);
	if (continual) continual = m_pCamera->OnProcessKeyInput(pKeyBuffer);
	for (int i = 0; i < m_nShaders; ++i) {
		if (continual)
			continual = m_ppShaders[i]->OnProcessKeyInput(pKeyBuffer);
	}
	// Warning! 미니언 공격 테스트 용
	if (m_pMyPlayer && GetAsyncKeyState('K') & 0x0001)
	{
		m_pThrowingMgr->RequestSpawn(
			m_pMyPlayer->GetPosition(),
			m_pMyPlayer->GetLook(),
			m_pMyPlayer->GetTeam(),
			FlyingObjectType::Minion_Magic,
			1000.0f);
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

	if (m_pCollisionManager) {
		m_pCollisionManager->Update(m_pWayFinder);

		int(*fow)[NODE_HEIGHT] = m_pCollisionManager->GetFoW(m_pMyPlayer->GetTeam());
		for (int i = 0; i < NODE_WIDTH; ++i)
			for (int j = 0; j < NODE_HEIGHT; ++j)
			{
				m_pFoW[i].m_bFoW[j] = fow[i][j];
				//m_pFoW[i].m_bFoW[j] = 1;
			}
	}
}

void CScene::Render()
{
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pcbLights->GetGPUVirtualAddress();
	m_pCommandList->SetGraphicsRootConstantBufferView(4, d3dcbLightsGpuVirtualAddress); //Lights

	d3dcbLightsGpuVirtualAddress = m_pcbbFoW->GetGPUVirtualAddress();
	m_pCommandList->SetGraphicsRootShaderResourceView(9, d3dcbLightsGpuVirtualAddress);

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

void CScene::RenderShadow()
{
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->RenderShadow(m_pLightCamera);
	}
}

void CScene::RenderWithLights()
{
	UpdateShaderVariables();
	m_pCamera->UpdateShaderVariables(1);

	if (m_pCubeMap)
	{
		m_pCommandList->SetDescriptorHeaps(1, m_pCbvSrvDescriptorHeaps[0].GetAddressOf());
		m_pCubeMap->UpdateShaderVariable(0);
	}

	if (m_pSketchEffect)
	{
		m_pCommandList->SetDescriptorHeaps(1, m_pCbvSrvDescriptorHeaps[1].GetAddressOf());
		m_pSketchEffect->UpdateShaderVariable(0);
	}

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pcbLights->GetGPUVirtualAddress();
	m_pCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights
}

void CScene::SetViewportsAndScissorRects()
{
	m_pCamera->SetViewportsAndScissorRects();
}

void CScene::SetShadowViewportsAndScissorRects()
{
	m_pLightCamera->SetViewportsAndScissorRects();
}

void CScene::UpdateCamera()
{
	if (m_bCamChanged)
	{
		m_pCamera->Finalize();
		Safe_Delete(m_pCamera);

		if (m_bCurCamIsAOS)
		{
			m_pCamera = new CAOSCamera();
		}
		else
		{
			m_pCamera = new CCamera();
		}

		m_pCamera->Initialize(m_pCreateMgr);

		m_bCamChanged = false;
	}
	m_pCamera->UpdateShaderVariables();
}

void CScene::UpdateShadowCamera(int renderStage)
{
	if (renderStage == 0)
	{
		m_pLightCamera->UpdateShaderVariables(8);
	}
	else
	{
		m_pLightCamera->UpdateShaderVariables(6);
	}
}

void CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	{
		::SetCapture(hWnd);
		m_pCamera->SavePickedPos();
		bool continual{ true };
		for (int i = 0; i < m_nShaders; ++i)
			if (continual) continual = m_ppShaders[i]->OnProcessMouseInput(wParam);
		if (continual)
		{
			PickObjectPointedByCursor(wParam, lParam);
		}
		break;
	}
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
	UNREFERENCED_PARAMETER(hWnd);
	if (nMessageID == WM_KEYUP)
	{
		OnProcessKeyUp(wParam, lParam);
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CScene::CreateCbvAndSrvDescriptorHeap(shared_ptr<CCreateMgr> pCreateMgr, int nConstantBufferViews, int nShaderResourceViews, int index)
{
	m_pCbvSrvDescriptorHeaps.resize(m_nHeaps);

	UINT incrementSize = pCreateMgr->GetCbvSrvDescriptorIncrementSize();
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc;
	descriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descriptorHeapDesc.NodeMask = 0;
	HRESULT hResult = pCreateMgr->GetDevice()->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_pCbvSrvDescriptorHeaps[index]));
	ThrowIfFailed(hResult);

	m_cbvCPUDescriptorStartHandles[index] = m_pCbvSrvDescriptorHeaps[index]->GetCPUDescriptorHandleForHeapStart();
	m_cbvGPUDescriptorStartHandles[index] = m_pCbvSrvDescriptorHeaps[index]->GetGPUDescriptorHandleForHeapStart();
	m_srvCPUDescriptorStartHandles[index].ptr = m_cbvCPUDescriptorStartHandles[index].ptr + (incrementSize * nConstantBufferViews);
	m_srvGPUDescriptorStartHandles[index].ptr = m_cbvGPUDescriptorStartHandles[index].ptr + (incrementSize * nConstantBufferViews);
}

void CScene::BuildLights()
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));

	m_pFoW = new FOW[NODE_WIDTH];
	::ZeroMemory(m_pFoW, sizeof(FOW)*NODE_WIDTH);

	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[0].m_color = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	m_pLights->m_pLights[0].m_direction = Vector3::Normalize(XMFLOAT3(0.5f, -0.7f, 0.5f));

	m_pLights->m_pLights[1].m_bEnable = true;
	m_pLights->m_pLights[1].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[1].m_color = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[1].m_direction = Vector3::Normalize(XMFLOAT3(0.0f, -0.5f, -1.0f));

	m_pLights->m_pLights[2].m_bEnable = true;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_color = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[2].m_direction = Vector3::Normalize(XMFLOAT3(0.5f, -0.3f, 0.5f));

	CreateCbvAndSrvDescriptorHeap(m_pCreateMgr, 0, 1, 0);
	m_pCubeMap = Materials::CreateCubeMapMaterial(m_pCreateMgr, &m_srvCPUDescriptorStartHandles[0], &m_srvGPUDescriptorStartHandles[0]);

	CreateCbvAndSrvDescriptorHeap(m_pCreateMgr, 0, 1, 1);
	m_pSketchEffect = Materials::CreateSketchMaterial(m_pCreateMgr, &m_srvCPUDescriptorStartHandles[1], &m_srvGPUDescriptorStartHandles[1]);

	m_pLightCamera = new CLightCamera(m_pLights->m_pLights[0].m_direction);
	m_pLightCamera->Initialize(m_pCreateMgr);
}

void CScene::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr)
{
	m_pCreateMgr = pCreateMgr;

	m_hWnd = pCreateMgr->GetHwnd();
	m_pCommandList = pCreateMgr->GetCommandList();

	m_pCamera = new CAOSCamera();

	m_pCamera->Initialize(pCreateMgr);

	m_nShaders = 23;
	m_ppShaders = new CShader*[m_nShaders];
	m_ppShaders[0] = new CSkyBoxShader(pCreateMgr);
	CTerrainShader* pTerrainShader = new CTerrainShader(pCreateMgr);
	m_ppShaders[1] = pTerrainShader;
	m_ppShaders[2] = new CMinionShader(pCreateMgr);
	m_ppShaders[3] = new CPlayerShader(pCreateMgr);
	m_ppShaders[4] = new CNeutralityShader(pCreateMgr);
	m_ppShaders[5] = new CNexusTowerShader(pCreateMgr);
	m_ppShaders[6] = new CStaticObjectShader(pCreateMgr);
	m_ppShaders[7] = new CFlyingShader(pCreateMgr);

	for (int i = 2; i <= 5; ++i)
	{
		m_ppShaders[i]->SetNetwork(m_pNetwork);
	}

	//UI Shader
	UI_Shader = new CUIObjectShader(pCreateMgr);
	PlayerHP_Shader = new CPlayerHPGaugeShader(pCreateMgr);
	MinionHP_Shader = new CMinionHPGaugeShader(pCreateMgr);
	MinimapIco_Shader = new CMinimapIconShader(pCreateMgr);
	BuildingMapIco_Shader = new CBuildingMinimapIconShader(pCreateMgr);
	CharFrame_Shader = new CharacterFrameGaugeShader(pCreateMgr);
	Minimap_Shader = new CMinimapShader(pCreateMgr);
	Skill_Shader = new CSkillShader(pCreateMgr);
	Number_Shader = new CNumberShader(pCreateMgr);
	SelectedSpecial_Shader = new CSelectedSpecialShader(pCreateMgr);
	NexusTowerHP_Shader = new CNexusAndTowerHPGaugeShader(pCreateMgr);
	NeutralityHP_Shader = new CNeutralityGaugeShader(pCreateMgr);
	Eeffect_Shader = new CEffectShader(pCreateMgr);
	SpecialSelect_Shader = new CSpecialSelectShader(pCreateMgr);
	m_ppShaders[22] = new CEquipShader(pCreateMgr);

	m_pCollisionManager = shared_ptr<CCollisionManager>(new CCollisionManager());
	//m_pCollisionManager->SetMyTeam((TeamType)(m_pNetwork->m_myid % 2));

	for (int i = 0; i < 2; ++i)
	{
		m_ppShaders[i]->Initialize(pCreateMgr);
	}

	for (int i = 2; i < m_nShaders - 14; ++i)
	{
		m_ppShaders[i]->Initialize(pCreateMgr, pTerrainShader->GetTerrain());
	}

	((CEquipShader*)m_ppShaders[22])->SetPlayerCnt(((CPlayerShader *)m_ppShaders[3])->GetObjectCount());
	((CEquipShader*)m_ppShaders[22])->SetPlayer(((CPlayerShader *)m_ppShaders[3])->GetCollisionObjects());

	// UI Shaders Initialize
	((CPlayerHPGaugeShader*)PlayerHP_Shader)->SetPlayerCnt(((CPlayerShader *)m_ppShaders[3])->GetObjectCount());
	((CPlayerHPGaugeShader*)PlayerHP_Shader)->SetPlayer(((CPlayerShader *)m_ppShaders[3])->GetCollisionObjects());

	((CNexusAndTowerHPGaugeShader*)NexusTowerHP_Shader)->SetNexusAndTowerCnt(((CNexusTowerShader *)m_ppShaders[5])->GetObjectCount());
	((CNexusAndTowerHPGaugeShader*)NexusTowerHP_Shader)->SetNexusAndTower(((CNexusTowerShader *)m_ppShaders[5])->GetCollisionObjects());

	((CMinimapIconShader*)MinimapIco_Shader)->SetPlayerCnt(((CPlayerShader *)m_ppShaders[3])->GetObjectCount());
	((CMinimapIconShader*)MinimapIco_Shader)->SetPlayer(((CPlayerShader *)m_ppShaders[3])->GetCollisionObjects());
	((CMinimapIconShader*)MinimapIco_Shader)->SetRoiderCnt(((CNeutralityShader *)m_ppShaders[4])->GetObjectCount());
	((CMinimapIconShader*)MinimapIco_Shader)->SetRoider(((CNeutralityShader *)m_ppShaders[4])->GetCollisionObjects());

	((CBuildingMinimapIconShader*)BuildingMapIco_Shader)->SetNexusAndTowerCnt(
		((CNexusTowerShader *)m_ppShaders[5])->GetNexusCount(),
		((CNexusTowerShader *)m_ppShaders[5])->GetTowerCount()
	);
	((CBuildingMinimapIconShader*)BuildingMapIco_Shader)->SetNexusAndTower(((CNexusTowerShader *)m_ppShaders[5])->GetCollisionObjects());

	m_ppObjects = ((CPlayerShader *)m_ppShaders[3])->GetCollisionObjects();
	
	SetPlayer();

	((CSkillShader*)Skill_Shader)->SetChangeWeapon(((CPlayerShader*)m_ppShaders[3])->GetChangeWeapon());
	((CPlayerHPGaugeShader*)Number_Shader)->SetPlayerCnt(((CPlayerShader *)m_ppShaders[3])->GetObjectCount());
	((CPlayerHPGaugeShader*)Number_Shader)->SetPlayer(((CPlayerShader *)m_ppShaders[3])->GetCollisionObjects());

	((CNeutralityGaugeShader*)NeutralityHP_Shader)->SetRoiderCnt(((CNeutralityShader *)m_ppShaders[4])->GetObjectCount());
	((CNeutralityGaugeShader*)NeutralityHP_Shader)->SetRoider(((CNeutralityShader *)m_ppShaders[4])->GetCollisionObjects());

	for (int i = 8; i <= 21; ++i)
	{
		m_ppShaders[i]->SetNetwork(m_pNetwork);
	}

	UI_Shader->Initialize(pCreateMgr, m_pCamera);
	PlayerHP_Shader->Initialize(pCreateMgr, m_pCamera);
	MinionHP_Shader->Initialize(pCreateMgr, m_pCamera);
	MinimapIco_Shader->Initialize(pCreateMgr, m_pCamera);
	BuildingMapIco_Shader->Initialize(pCreateMgr, m_pCamera);
	CharFrame_Shader->Initialize(pCreateMgr, m_pCamera);
	Minimap_Shader->Initialize(pCreateMgr, m_pCamera);
	Skill_Shader->Initialize(pCreateMgr, m_pCamera);
	Number_Shader->Initialize(pCreateMgr, m_pCamera);
	SelectedSpecial_Shader->Initialize(pCreateMgr, m_pCamera);
	NexusTowerHP_Shader->Initialize(pCreateMgr, m_pCamera);
	NeutralityHP_Shader->Initialize(pCreateMgr, m_pCamera);
	Eeffect_Shader->Initialize(pCreateMgr, m_pCamera);
	SpecialSelect_Shader->Initialize(pCreateMgr, m_pCamera);
	m_ppShaders[22]->Initialize(pCreateMgr, m_pCamera);
	
	//Managere Initialize
	m_pWayFinder = shared_ptr<CWayFinder>(new CWayFinder());
	m_pUIObjectsManager = shared_ptr<CUIObjectManager>(new CUIObjectManager());
	m_pThrowingMgr = shared_ptr<CThrowingMgr>(new CThrowingMgr());
	m_pThrowingMgr->SetFlyingShader(static_cast<CFlyingShader*>(m_ppShaders[7]));
	m_pEffectMgr = shared_ptr<CEffectMgr>(new CEffectMgr());
	m_pEffectMgr->SetEffectShader(static_cast<CEffectShader*>(Eeffect_Shader));
	m_pFSMMgr = shared_ptr<CFSMMgr>(new CFSMMgr(m_pWayFinder));
	((CMinimapShader*)Minimap_Shader)->SetWayFinder(m_pWayFinder);
	
	m_pNetwork->SetEffectManager(m_pEffectMgr);

	m_pCollisionManager->SetEffectManager(m_pEffectMgr);

	m_pSoundManager = shared_ptr<CSoundManager>(new CSoundManager(m_pCamera));	// Fmod System Init
	m_pSoundManager->loading();													// Sound File Load

	//Manager Shaders Setting
	CMinionShader* pMinionS = (CMinionShader *)m_ppShaders[2];

	pMinionS->SetCollisionManager(m_pCollisionManager);
	pMinionS->SetGaugeManger(m_pUIObjectsManager);
	pMinionS->SetFSMManager(m_pFSMMgr);
	pMinionS->SetThrowingManager(m_pThrowingMgr);
	pMinionS->SetEffectManager(m_pEffectMgr);

	static_cast<CMinionHPGaugeShader*>(MinionHP_Shader)->SetUIObjectsManager(m_pUIObjectsManager);
	static_cast<CMinimapIconShader*>(MinimapIco_Shader)->SetUIObjectsManager(m_pUIObjectsManager);

	m_pCollisionManager->SetNodeMap(m_pWayFinder->GetNodeMap(), m_pWayFinder->GetNodeSize(), m_pWayFinder->GetNodeWH());

	CPlayerShader* pPlayerS = (CPlayerShader *)m_ppShaders[3];
	int nColliderObject = pPlayerS->GetObjectCount();
	for (int i = 0; i < nColliderObject; ++i)
	{
		m_pCollisionManager->AddCollider((pPlayerS->GetCollisionObjects())[i]);
	}
	pPlayerS->SetColManagerToObject(m_pCollisionManager);
	pPlayerS->SetThrowingManagerToObject(m_pThrowingMgr);
	pPlayerS->SetEffectManagerToObject(m_pEffectMgr);
	pPlayerS->SetSoundManagerToObject(m_pSoundManager);

	// 중립 몬스터에 충돌체 부여
	CNeutralityShader* pNetral = (CNeutralityShader *)m_ppShaders[4];
	nColliderObject = pNetral->GetObjectCount();
	for (int i = 0; i < nColliderObject; ++i)
	{
		m_pCollisionManager->AddCollider((pNetral->GetCollisionObjects())[i]);
	}
	pNetral->SetColManagerToObject(m_pCollisionManager);
	pNetral->SetFSMManager(m_pFSMMgr);
	pNetral->SetThrowingManagerToObject(m_pThrowingMgr);
	pNetral->SetEffectManagerToObject(m_pEffectMgr);
	pNetral->SetSoundManagerToObject(m_pSoundManager);

	CNexusTowerShader* pNTS = (CNexusTowerShader *)m_ppShaders[5];
	nColliderObject = pNTS->GetObjectCount();
	for (int i = 0; i < nColliderObject; ++i)
	{
		m_pCollisionManager->AddCollider((pNTS->GetCollisionObjects())[i]);
	}
	pNTS->SetColManagerToObject(m_pCollisionManager);
	pNTS->SetFSMManager(m_pFSMMgr);
	pNTS->SetThrowingManagerToObject(m_pThrowingMgr);

	CFlyingShader* pFS = (CFlyingShader*)m_ppShaders[7];
	pFS->SetColManagerToObject(m_pCollisionManager);
	pFS->SetEffectManagerToObject(m_pEffectMgr);
	pFS->SetSoundManagerToObject(m_pSoundManager);

	BuildLights();

	m_pNetwork->SetWayfinder(m_pWayFinder);
	m_pNetwork->SetCollisionManager(m_pCollisionManager);

	m_pSoundManager->play(SOUND::Back_Ground, XMFLOAT3(0, 0, 0));
}

void CScene::SetPlayer()
{
	m_pMyPlayer = (CAnimatedObject*)m_ppObjects[m_pNetwork->m_myid];
	((CSkillShader*)Skill_Shader)->SetPlayer(m_ppObjects[m_pNetwork->m_myid]);
	((CMinimapShader*)Minimap_Shader)->SetPlayer(m_ppObjects[m_pNetwork->m_myid]);
	((CharacterFrameGaugeShader*)CharFrame_Shader)->SetPlayer(m_ppObjects[m_pNetwork->m_myid]);
	((CSelectedSpecialShader*)SelectedSpecial_Shader)->SetPlayer(m_ppObjects[m_pNetwork->m_myid]);
	((CSelectedSpecialShader*)SpecialSelect_Shader)->SetPlayer(m_ppObjects[m_pNetwork->m_myid]);
	
	m_pCollisionManager->SetMyTeam(m_pMyPlayer->GetTeam());
}

void CScene::ReleaseObjects()
{
	if (m_pCamera)
	{
		m_pCamera->Finalize();
		Safe_Delete(m_pCamera);
	}
	if (m_pLightCamera)
	{
		m_pLightCamera->Finalize();
		Safe_Delete(m_pLightCamera);
	}
	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			if (m_ppShaders[i]) m_ppShaders[i]->Finalize();
		}
		Safe_Delete_Array(m_ppShaders);
	}
	if (m_pCubeMap) Safe_Delete(m_pCubeMap);
}

void CScene::CreateShaderVariables(shared_ptr<CCreateMgr> pCreateMgr)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pcbLights = pCreateMgr->CreateBufferResource(NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);


	ncbElementBytes = ((sizeof(FOW) + 255) & ~255); //256의 배수
	m_pcbbFoW = pCreateMgr->CreateBufferResource(NULL, ncbElementBytes*NODE_WIDTH, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, NULL);
	m_pcbbFoW->Map(0, NULL, (void **)&m_pcbMappedFoW);
}

void CScene::ReleaseShaderVariables()
{
	if (m_pcbLights.Get())
	{
		m_pcbLights->Unmap(0, NULL);
		m_pcbLights.Reset();
	}

	if (m_pcbbFoW.Get())
	{
		m_pcbbFoW->Unmap(0, NULL);
		m_pcbbFoW.Reset();
	}

	for (int i = 0; i < m_nHeaps; ++i)
	{
		m_pCbvSrvDescriptorHeaps[i].Reset();
	}
	m_pCbvSrvDescriptorHeaps.clear();
}

void CScene::UpdateShaderVariables()
{
	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));

	::memcpy(m_pcbMappedFoW, m_pFoW, sizeof(FOW)*NODE_WIDTH);
	m_pcbMappedFoW[0].m_bFoW[0] = 0;
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

	if (wParam == MK_RBUTTON)
	{
		GenerateLayEndWorldPosition(pickPosition, xmf4x4View);
	}
}

void CScene::GenerateLayEndWorldPosition(XMFLOAT3& pickPosition, XMFLOAT4X4&	 xmf4x4View)
{
	if (m_pCollisionManager->IsGameOver()) return;
	if (!m_pMyPlayer || m_pMyPlayer->GetState() == States::Die || m_pMyPlayer->GetState() == States::Remove) return;

	XMFLOAT4X4  inverseArr = Matrix4x4::Inverse(xmf4x4View);
	XMFLOAT3 camPosition = m_pCamera->GetPosition();
	XMFLOAT3 layWorldPosition = Vector3::TransformCoord(pickPosition, inverseArr);
	XMFLOAT3 layDirection = Vector3::Subtract(layWorldPosition, camPosition);
	float yDiff = abs(camPosition.y / layDirection.y);

	m_pickWorldPosition = Vector3::Add(camPosition, Vector3::ScalarProduct(layDirection, yDiff, false));

	if (m_pMyPlayer)
	{
		m_pMyPlayer->SetPathToGo(m_pWayFinder->GetPathToPosition(
			m_pMyPlayer->GetPosition(),
			m_pickWorldPosition));
	}

	CS_Msg_Demand_Pos_Character p;
	p.Character_id = (BYTE)m_pNetwork->m_myid;
	p.size = sizeof(p);
	p.type = CS_MOVE_PLAYER;
	p.x = m_pickWorldPosition.x;
	p.y = m_pickWorldPosition.z;
	m_pNetwork->SendPacket(&p);
}

// Process Keyboard Input
void CScene::OnProcessKeyUp(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

/*	if (wParam == VK_F1)
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
	else */
	if (wParam == VK_F3)
	{
		m_bRenderBoundingBox = !m_bRenderBoundingBox;
	}
}

