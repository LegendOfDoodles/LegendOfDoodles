#include "stdafx.h"
#include "NexusAndTowerGaugeShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "05.Objects/96.Billboard/02.GaugeObject/GaugeObject.h"

/// <summary>
/// 목적: Nexus Tower HP Gauge 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  이용선, 김나단
/// 최종 수정 날짜: 2018-08-22
/// </summary>

CNexusAndTowerHPGaugeShader::CNexusAndTowerHPGaugeShader(shared_ptr<CCreateMgr> pCreateMgr)
	:CBillboardShader(pCreateMgr)
{
	m_pCreateMgr = pCreateMgr;
}

CNexusAndTowerHPGaugeShader::~CNexusAndTowerHPGaugeShader()
{
}

////////////////////////////////////////////////////////////////////////
//
void CNexusAndTowerHPGaugeShader::UpdateShaderVariables(int opt)
{
	UNREFERENCED_PARAMETER(opt);
	static UINT elementBytes = ((sizeof(CB_GAUGE_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAUGE_INFO *pMappedObject = (CB_GAUGE_INFO *)(m_pMappedObjects + (i * elementBytes));
		pMappedObject->m_fCurrentHP = ((CGaugeObject*)m_ppObjects[i])->GetCurrentHP();
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CNexusAndTowerHPGaugeShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CNexusAndTowerHPGaugeShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera, 0);

	for (int j = 0; j < m_nObjects; j++)
	{
		CCollisionObject* master = ((CGaugeObject*)m_ppObjects[j])->GetMasterObject();

		if (master->GetTeam() == TeamType::Blue) 		m_ppMaterials[0]->UpdateShaderVariable(1);
		else if (master->GetTeam() == TeamType::Red)	m_ppMaterials[0]->UpdateShaderVariable(0);

		m_ppObjects[j]->Render(pCamera);
	}
}

bool CNexusAndTowerHPGaugeShader::OnProcessKeyInput(UCHAR * pKeyBuffer)
{
	UNREFERENCED_PARAMETER(pKeyBuffer);

	return true;
}

bool CNexusAndTowerHPGaugeShader::OnProcessMouseInput(WPARAM pKeyBuffer)
{
	UNREFERENCED_PARAMETER(pKeyBuffer);

	return true;
}

////////////////////////////////////////////////////////////////////////
//
D3D12_INPUT_LAYOUT_DESC CNexusAndTowerHPGaugeShader::CreateInputLayout()
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

D3D12_BLEND_DESC CNexusAndTowerHPGaugeShader::CreateBlendState()
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

D3D12_SHADER_BYTECODE CNexusAndTowerHPGaugeShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/UIShader.hlsl",
		"VSTexturedGauge",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CNexusAndTowerHPGaugeShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/UIShader.hlsl",
		"PSTexturedGauge",
		"ps_5_1",
		pShaderBlob));
}

void CNexusAndTowerHPGaugeShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 1;

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CNexusAndTowerHPGaugeShader::BuildObjects(shared_ptr<CCreateMgr>  pCreateMgr, void * pContext)
{
	m_pCamera = (CCamera*)pContext;

	m_nObjects = m_nNexusAndTower;
	m_ppObjects = new CBaseObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAUGE_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 2);
	CreateShaderVariables(pCreateMgr, ncbElementBytes, m_nObjects);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pConstBuffer.Get(), ncbElementBytes);

	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CGaugeObject *pGaugeObject = NULL;

	m_nMaterials = m_nHeaps;
	m_ppMaterials = new CMaterial*[m_nMaterials];

	m_ppMaterials[0] = Materials::CreateHPGaugeMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);

	for (int i = 0; i < m_nObjects; ++i) {
		pGaugeObject = new CGaugeObject(pCreateMgr, GaugeUIType::NexusAndTower);
		pGaugeObject->SetCamera(m_pCamera);
		pGaugeObject->SetObject(m_ppNexusAndTower[i]);

		pGaugeObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * i));

		m_ppObjects[i] = pGaugeObject;
	}
}

void CNexusAndTowerHPGaugeShader::ReleaseObjects()
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

