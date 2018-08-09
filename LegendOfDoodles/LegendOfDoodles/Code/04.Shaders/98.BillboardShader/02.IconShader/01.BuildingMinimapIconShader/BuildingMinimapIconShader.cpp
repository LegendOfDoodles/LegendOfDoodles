#include "stdafx.h"
#include "BuildingMinimapIconShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/09.NexusTower/NexusTower.h"
#include "05.Objects/99.Material/Material.h"
#include "05.Objects/96.Billboard/03.IconObject/IconObject.h"

/// <summary>
/// 목적: 미니맵 건물 Icon 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  이용선, 김나단
/// 최종 수정 날짜: 2018-07-03
/// </summary>

CBuildingMinimapIconShader::CBuildingMinimapIconShader(shared_ptr<CCreateMgr> pCreateMgr)
	:CBillboardShader(pCreateMgr)
{
}

CBuildingMinimapIconShader::~CBuildingMinimapIconShader()
{
}

void CBuildingMinimapIconShader::UpdateShaderVariables(int opt)
{
	static UINT elementBytes = ((sizeof(CB_GAUGE_INFO) + 255) & ~255);
	int cnt{ 0 };

	if (opt == 0)
	{
		for (int i = 0; i < m_nObjects; i++)
		{
			if (m_ppNexusAndTower[i]->GetType() == ObjectType::FirstTower)
			{
				CB_GAUGE_INFO *pMappedObject = (CB_GAUGE_INFO *)(m_pMappedObjects + (cnt++ * elementBytes));
				pMappedObject->m_fCurrentHP = ((CIconObject*)m_ppObjects[i])->GetCurrentHP();
				XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
					XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
			}
		}
	}
	else if (opt == 1)
	{
		for (int i = 0; i < m_nObjects; i++)
		{
			if (m_ppNexusAndTower[i]->GetType() == ObjectType::Nexus)
			{
				CB_GAUGE_INFO *pMappedObject = (CB_GAUGE_INFO *)(m_pMappedObjects + ((cnt++ +  m_nTower) * elementBytes));
				pMappedObject->m_fCurrentHP = ((CIconObject*)m_ppObjects[i])->GetCurrentHP();
				XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
					XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
			}
		}
	}
}

void CBuildingMinimapIconShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CBuildingMinimapIconShader::Render(CCamera * pCamera)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		CCollisionObject* master = ((CIconObject*)m_ppObjects[j])->GetMasterObject();

		switch (master->GetType())
		{
		case ObjectType::FirstTower:
			CShader::Render(pCamera, 0);

			if (master->GetTeam() == TeamType::Blue) {
				m_ppMaterials[0]->UpdateShaderVariable(0);
			}
			else {
				m_ppMaterials[0]->UpdateShaderVariable(1);
			}
			break;
		case ObjectType::Nexus:
			CShader::Render(pCamera, 1);

			if (master->GetTeam() == TeamType::Blue) {
				m_ppMaterials[1]->UpdateShaderVariable(0);
			}
			else {
				m_ppMaterials[1]->UpdateShaderVariable(1);
			}
			break;
		}

		if (m_ppObjects[j]) m_ppObjects[j]->Render(pCamera);
	}
}

void CBuildingMinimapIconShader::GetCamera(CCamera * pCamera)
{
	m_pCamera = pCamera;

	for (int i = 0; i < m_nObjects; ++i)
	{
		static_cast<CIconObject*>(m_ppObjects[i])->SetCamera(m_pCamera);
	}
}

bool CBuildingMinimapIconShader::OnProcessKeyInput(UCHAR * pKeyBuffer)
{
	UNREFERENCED_PARAMETER(pKeyBuffer);

	return false;
}

bool CBuildingMinimapIconShader::OnProcessMouseInput(WPARAM pKeyBuffer)
{
	UNREFERENCED_PARAMETER(pKeyBuffer);

	return false;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CBuildingMinimapIconShader::CreateInputLayout()
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

D3D12_BLEND_DESC CBuildingMinimapIconShader::CreateBlendState()
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

D3D12_SHADER_BYTECODE CBuildingMinimapIconShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSTexturedGauge",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CBuildingMinimapIconShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"PSTexturedIconGauge",
		"ps_5_1",
		pShaderBlob));
}

void CBuildingMinimapIconShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 1;

	m_nHeaps = 2;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CBuildingMinimapIconShader::BuildObjects(shared_ptr<CCreateMgr>  pCreateMgr, void * pContext)
{
	m_pCamera = (CCamera*)pContext;

	m_nObjects = m_nNexus + m_nTower;
	m_ppObjects = new CBaseObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAUGE_INFO) + 255) & ~255);

	CreateShaderVariables(pCreateMgr, ncbElementBytes, m_nObjects);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nTower, 2, 0);
	CreateConstantBufferViews(pCreateMgr, m_nTower, m_pConstBuffer.Get(), ncbElementBytes, 0, 0);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nNexus, 2, 1);
	CreateConstantBufferViews(pCreateMgr, m_nNexus, m_pConstBuffer.Get(), ncbElementBytes, m_nTower, 1);

#if USE_BATCH_MATERIAL
	m_nMaterials = m_nHeaps;
	m_ppMaterials = new CMaterial*[m_nMaterials];

	// Tower
	/* 0. Blue 1. Red */
	m_ppMaterials[0] = Materials::CreateTowerIconMaterial(pCreateMgr,	&m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);/*
	m_ppMaterials[1] = Materials::CreateRedTowerIconMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[1], &m_psrvGPUDescriptorStartHandle[1]);*/
	
	// Nexus
	/* 0. Blue 1. Red */
	m_ppMaterials[1] = Materials::CreateNexusIconMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[1], &m_psrvGPUDescriptorStartHandle[1]);
	/*
	m_ppMaterials[2] = Materials::CreateBoxNexusIconMaterial(pCreateMgr,	&m_psrvCPUDescriptorStartHandle[2], &m_psrvGPUDescriptorStartHandle[2]);
	m_ppMaterials[3] = Materials::CreateShellNexusIconMaterial(pCreateMgr,	&m_psrvCPUDescriptorStartHandle[3], &m_psrvGPUDescriptorStartHandle[3]);*/

#else
	CMaterial *pCubeMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
#endif

	// Player Icon 생성
	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CIconObject *pIconObject = NULL;

	int nexusCnt{ 0 }, towerCnt{ 0 };
	for (int i = 0; i < m_nObjects; ++i) {
		pIconObject = new CIconObject(pCreateMgr, IconUIType::NexusAndTowerIcon);

		pIconObject->SetCamera(m_pCamera);
		pIconObject->SetDistance((FRAME_BUFFER_WIDTH / 128.f) - 0.04f);	// distance 9
		pIconObject->SetObject(m_ppNexusAndTower[i]);
		pIconObject->WorldToMinimap();
		if (m_ppNexusAndTower[i]->GetType() == ObjectType::FirstTower)
		{
			pIconObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * towerCnt++));
		}
		else if (m_ppNexusAndTower[i]->GetType() == ObjectType::Nexus)
		{
			pIconObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[1].ptr + (incrementSize * nexusCnt++));
		}

		m_ppObjects[i] = pIconObject;
	}
}

void CBuildingMinimapIconShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			delete m_ppObjects[j];
		}
		Safe_Delete_Array(m_ppObjects);
	}

#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i < m_nMaterials; ++i)
		{
			Safe_Delete(m_ppMaterials[i]);
		}
		Safe_Delete_Array(m_ppMaterials);
	}
#endif
}
