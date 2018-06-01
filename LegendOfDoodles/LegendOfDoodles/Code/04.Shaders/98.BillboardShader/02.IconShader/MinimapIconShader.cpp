#include "stdafx.h"
#include "MinimapIconShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/08.Player/Player.h"
#include "05.Objects/99.Material/Material.h"
#include "00.Global/01.Utility/06.HPGaugeManager/HPGaugeManager.h"

/// <summary>
/// 목적: 미니맵 Icon 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  이용선, 김나단
/// 최종 수정 날짜: 2018-06-01
/// </summary>

CMinimapIconShader::CMinimapIconShader(CCreateMgr *pCreateMgr)
	: CShader(pCreateMgr)
{
	m_pCreateMgr = pCreateMgr;
}

CMinimapIconShader::~CMinimapIconShader()
{
}

void CMinimapIconShader::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			m_ppObjects[j]->ReleaseUploadBuffers();
		}
	}

#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}
#endif
}

void CMinimapIconShader::UpdateShaderVariables()
{
	static UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedObjects + (i * elementBytes));
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}

	for (int i = m_nObjects; i < (*m_nBlues) + m_nObjects; ++i)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedObjects + (i * elementBytes));
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_MinionIconObjectList[i]->GetWorldMatrix())));
	}

	for (int i = MAX_MINION + m_nObjects; i < MAX_MINION + (*m_nReds + m_nObjects); ++i)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedObjects + (i * elementBytes));
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_MinionIconObjectList[i]->GetWorldMatrix())));
	}

}

void CMinimapIconShader::AnimateObjects(float timeElapsed)
{
	if (m_pIconManger->SpawnStarted()) SpawnMinionIcon();

	for (int i = 0; i < *m_nBlues; ++i)
	{
		m_MinionIconObjectList[i]->Animate(timeElapsed);
	}

	for (int i = MAX_MINION; i < MAX_MINION + *m_nReds; ++i)
	{
		m_MinionIconObjectList[i]->Animate(timeElapsed);
	}

	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CMinimapIconShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera);

	for (int j = 0; j < m_nObjects; j++)
	{
		CCollisionObject* master = ((CMinimapIconObjects*)m_ppObjects[j])->GetMasterObject();

		switch (master->GetType())
		{
		case ObjectType::StickPlayer:
			CShader::Render(pCamera, 0);
			m_ppMaterials[0]->UpdateShaderVariables();
			break;
		case ObjectType::SwordPlayer:
			CShader::Render(pCamera, 1);
			m_ppMaterials[1]->UpdateShaderVariables();
			break;
		case ObjectType::FirstTower:
			if (master->GetTeam() == TeamType::Blue) {
				// Blue Tower
				CShader::Render(pCamera, 3);
				m_ppMaterials[3]->UpdateShaderVariables();
			}
			else {
				// Red Tower
				CShader::Render(pCamera, 4);
				m_ppMaterials[4]->UpdateShaderVariables();
			}
			break;
		case ObjectType::Nexus:
			if (master->GetTeam() == TeamType::Blue) {
				// Box
				CShader::Render(pCamera, 5);
				m_ppMaterials[5]->UpdateShaderVariables();
			}
			else {
				// Shell
				CShader::Render(pCamera, 6);
				m_ppMaterials[6]->UpdateShaderVariables();
			}
			break;
		}

		if (m_ppObjects[j]) m_ppObjects[j]->Render(pCamera);
	}

	// 미니언
	CShader::Render(pCamera, 2);
	m_ppMaterials[2]->UpdateShaderVariables();

	for (int i = 0; i < *m_nBlues; ++i)
	{
		m_MinionIconObjectList[i]->Render(pCamera);
	}

	for (int i = MAX_MINION; i < MAX_MINION + *m_nReds; ++i)
	{
		m_MinionIconObjectList[i]->Render(pCamera);
	}
}

void CMinimapIconShader::GetCamera(CCamera * pCamera)
{
	m_pCamera = pCamera;

	for (int i = 0; i < MAX_MINION * 2; ++i)
	{
		m_MinionIconObjectList[i]->SetCamera(m_pCamera);
	}

	for (int i = 0; i < m_nObjects; ++i)
	{
		static_cast<CMinimapIconObjects*>(m_ppObjects[i])->SetCamera(m_pCamera);
	}
}

void CMinimapIconShader::SetUIObjectsManager(CUIObjectManager * pManger)
{
	m_pIconManger = pManger;
	m_MinionObjectList = m_pIconManger->GetMinionObjectList();
}

bool CMinimapIconShader::OnProcessKeyInput(UCHAR * pKeyBuffer)
{
	return true;
}

bool CMinimapIconShader::OnProcessMouseInput(WPARAM pKeyBuffer)
{
	return false;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CMinimapIconShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pInputElementDescs[0] = {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pInputElementDescs[1] = {
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_BLEND_DESC CMinimapIconShader::CreateBlendState()
{
	D3D12_BLEND_DESC blendDesc;
	::ZeroMemory(&blendDesc, sizeof(D3D12_BLEND_DESC));

	blendDesc.AlphaToCoverageEnable = TRUE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	blendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(blendDesc);
}

D3D12_SHADER_BYTECODE CMinimapIconShader::CreateVertexShader(ID3DBlob ** ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSTextured",
		"vs_5_1",
		ppShaderBlob));
}

D3D12_SHADER_BYTECODE CMinimapIconShader::CreatePixelShader(ID3DBlob ** ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"PSTextured",
		"ps_5_1",
		ppShaderBlob));
}

void CMinimapIconShader::CreateShader(CCreateMgr * pCreateMgr, UINT nRenderTargets)
{
	m_nPipelineStates = 1;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	m_nHeaps = 7;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets);
}

void CMinimapIconShader::CreateShaderVariables(CCreateMgr * pCreateMgr, int nBuffers)
{
	HRESULT hResult;

	UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	m_pConstBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		elementBytes * nBuffers,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	hResult = m_pConstBuffer->Map(0, NULL, (void **)&m_pMappedObjects);
	assert(SUCCEEDED(hResult) && "m_pConstBuffer->Map Failed");
}

void CMinimapIconShader::BuildObjects(CCreateMgr * pCreateMgr, void * pContext)
{
	m_pCamera = (CCamera*)pContext;

	m_nObjects = m_nPlayer + m_nNexusAndTower;
	m_ppObjects = new CBaseObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateShaderVariables(pCreateMgr, m_nObjects + (MAX_MINION*2));

	for (int i = 0; i < m_nHeaps; ++i) {
		CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects + (MAX_MINION*2), 1, i);
		CreateConstantBufferViews(pCreateMgr, m_nObjects + (MAX_MINION*2), m_pConstBuffer, ncbElementBytes, i);
	}
#if USE_BATCH_MATERIAL
	m_nMaterials = m_nHeaps;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = Materials::CreateStickIconMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[1] = Materials::CreateSwordIconMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[1], &m_psrvGPUDescriptorStartHandle[1]);
	m_ppMaterials[2] = Materials::CreateRedMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[2], &m_psrvGPUDescriptorStartHandle[2]);
	m_ppMaterials[3] = Materials::CreateBlueTowerIconMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[3], &m_psrvGPUDescriptorStartHandle[3]);
	m_ppMaterials[4] = Materials::CreateRedTowerIconMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[4], &m_psrvGPUDescriptorStartHandle[4]);
	m_ppMaterials[5] = Materials::CreateBoxNexusIconMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[5], &m_psrvGPUDescriptorStartHandle[5]);
	m_ppMaterials[6] = Materials::CreateShellNexusIconMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[6], &m_psrvGPUDescriptorStartHandle[6]);
#else
	CMaterial *pCubeMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
#endif

	// Player Icon 생성
	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CMinimapIconObjects *pIconObject = NULL;

	for (int i = 0; i < m_nObjects; ++i) {
		if (i < m_nPlayer) {
			pIconObject = new CMinimapIconObjects(pCreateMgr, IconUIType::PlayerIcon);

			pIconObject->SetCamera(m_pCamera);
			pIconObject->SetDistance((FRAME_BUFFER_WIDTH / 128.f) - 0.04f);	// distance 9
			pIconObject->SetObject(m_pPlayer[i]);
			pIconObject->GetmasterObjectType((ObjectType)m_pPlayer[i]->GetType());

			pIconObject->WorldToMinimap();

			pIconObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * i));
		}
		else {
			pIconObject = new CMinimapIconObjects(pCreateMgr, IconUIType::NexusAndTowerIcon);

			pIconObject->SetCamera(m_pCamera);
			pIconObject->SetDistance((FRAME_BUFFER_WIDTH / 128.f) - 0.02f);	// distance 9
			pIconObject->SetObject(m_ppNexusAndTower[i - m_nPlayer]);
			pIconObject->GetmasterObjectType((ObjectType)m_ppNexusAndTower[i - m_nPlayer]->GetType());

			pIconObject->WorldToMinimap();

			pIconObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * i));
		}

		m_ppObjects[i] = pIconObject;
	}
}

void CMinimapIconShader::SpawnMinionIcon()
{
	static UINT incrementSize{ m_pCreateMgr->GetCbvSrvDescriptorIncrementSize() };

	m_pCreateMgr->ResetCommandList();

	int cnt{ 0 };

	for (int i = m_nObjects; cnt < m_pIconManger->GetCount(); ++cnt, ++i)
	{
		CMinimapIconObjects *pMinionIcon{ NULL };
		CCollisionObject *pMinionObjects{ NULL };

		pMinionIcon = new CMinimapIconObjects(m_pCreateMgr, IconUIType::MinionIcon);
		pMinionObjects = (*m_MinionObjectList)[cnt];

		pMinionIcon->SetObject(pMinionObjects);
		pMinionIcon->SetDistance((FRAME_BUFFER_WIDTH / 128.f) - 0.01f);
		pMinionIcon->SetCamera(m_pCamera);
		pMinionIcon->WorldToMinimap();

		pMinionIcon->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * i));

		m_MinionIconObjectList.emplace_back(pMinionIcon);
	}
	m_pCreateMgr->ExecuteCommandList();

	for (int i = 0; i < cnt; ++i)
	{
		m_MinionIconObjectList[i]->ReleaseUploadBuffers();
	}
	m_nBlues = m_pIconManger->GetBlueMinionCnt();
	m_nReds = m_pIconManger->GetRedMinionCnt();

	m_pIconManger->EndSpawn();
	m_pIconManger->ResetCount();
}


void CMinimapIconShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			delete m_ppObjects[j];
		}
		Safe_Delete_Array(m_ppObjects);
	}

	for (auto& iter = m_MinionIconObjectList.begin(); iter != m_MinionIconObjectList.end();)
	{
		iter = m_MinionIconObjectList.erase(iter);
	}
	m_MinionIconObjectList.clear();

#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i < m_nMaterials; ++i)
		{
			if (m_ppMaterials[i]) delete m_ppMaterials[i];
		}
		Safe_Delete(m_ppMaterials);
	}
#endif
}
