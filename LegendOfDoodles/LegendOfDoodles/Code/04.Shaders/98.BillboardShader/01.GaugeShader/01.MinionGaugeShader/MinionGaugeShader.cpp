#include "stdafx.h"
#include "MinionGaugeShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/06.Minion/Minion.h"
#include "05.Objects/99.Material/Material.h"
#include "00.Global/01.Utility/06.HPGaugeManager/HPGaugeManager.h"

/// <summary>
/// 목적: Billboard 테스트 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  이용선, 김나단
/// 최종 수정 날짜: 2018-05-19
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CMinionHPGaugeShader::CMinionHPGaugeShader(CCreateMgr *pCreateMgr)
	: CShader(pCreateMgr)
{
	m_pCreateMgr = pCreateMgr;
}

CMinionHPGaugeShader::~CMinionHPGaugeShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CMinionHPGaugeShader::ReleaseUploadBuffers()
{
#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}
#endif
}

void CMinionHPGaugeShader::UpdateShaderVariables()
{
	static UINT elementBytes = ((sizeof(CB_GAUGE_INFO) + 255) & ~255);

	for (int i = 0; i < *m_nBlues; ++i)
	{
		CB_GAUGE_INFO *pMappedObject = (CB_GAUGE_INFO *)(m_pMappedObjects + (i * elementBytes));
		pMappedObject->m_fCurrentHP = m_HPGaugeObjectList[i]->GetCurrentHP();
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_HPGaugeObjectList[i]->GetWorldMatrix())));
	}

	for (int i = MAX_MINION; i < MAX_MINION + *m_nReds; ++i)
	{
		CB_GAUGE_INFO *pMappedObject = (CB_GAUGE_INFO *)(m_pMappedObjects + (i * elementBytes));
		pMappedObject->m_fCurrentHP = m_HPGaugeObjectList[i]->GetCurrentHP();
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_HPGaugeObjectList[i]->GetWorldMatrix())));
	}
}

void CMinionHPGaugeShader::AnimateObjects(float timeElapsed)
{
	if (m_pGaugeManger->SpawnStarted()) SpawnGauge();

	for (int i = 0; i < *m_nBlues; ++i)
	{
		m_HPGaugeObjectList[i]->Animate(timeElapsed);
	}

	for (int i = MAX_MINION; i < MAX_MINION + *m_nReds; ++i)
	{
		m_HPGaugeObjectList[i]->Animate(timeElapsed);
	}
}

void CMinionHPGaugeShader::Render(CCamera *pCamera)
{
	CShader::Render(pCamera);
	if (m_ppMaterials) m_ppMaterials[0]->UpdateShaderVariables();

	for (int i = 0; i < *m_nBlues; ++i)
	{
		if(!static_cast<CAnimatedObject*>((*m_MinionObjectList)[i])->IsDead())
			m_HPGaugeObjectList[i]->Render(pCamera);
	}

	for (int i = MAX_MINION; i < MAX_MINION + *m_nReds; ++i)
	{
		if (!static_cast<CAnimatedObject*>((*m_MinionObjectList)[i])->IsDead())
			m_HPGaugeObjectList[i]->Render(pCamera);
	}
}

void CMinionHPGaugeShader::GetCamera(CCamera * pCamera)
{
	m_pCamera = pCamera;

	for (int i = 0; i < MAX_MINION * 2; ++i)
	{
		m_HPGaugeObjectList[i]->SetCamera(m_pCamera);
	}
}

void CMinionHPGaugeShader::SetGaugeManager(CUIObjectManager * pManger)
{
	m_pGaugeManger = pManger;
	m_MinionObjectList = m_pGaugeManger->GetMinionObjectList();
}

bool CMinionHPGaugeShader::OnProcessKeyInput(UCHAR * pKeyBuffer)
{

	return true;
}

bool CMinionHPGaugeShader::OnProcessMouseInput(WPARAM pKeyBuffer)
{
	return false;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CMinionHPGaugeShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pd3dInputElementDescs[1] = {
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CMinionHPGaugeShader::CreateVertexShader(ID3DBlob **ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSTexturedGauge",
		"vs_5_1",
		ppShaderBlob));
}

D3D12_SHADER_BYTECODE CMinionHPGaugeShader::CreatePixelShader(ID3DBlob **ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"PSTexturedGauge",
		"ps_5_1",
		ppShaderBlob));
}

void CMinionHPGaugeShader::CreateShader(CCreateMgr *pCreateMgr, UINT nRenderTargets)
{
	m_nPipelineStates = 1;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets);
}

void CMinionHPGaugeShader::CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers)
{
	HRESULT hResult;

	UINT elementBytes = ((sizeof(CB_GAUGE_INFO) + 255) & ~255);

	m_pConstBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		elementBytes * nBuffers,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	hResult = m_pConstBuffer->Map(0, NULL, (void **)&m_pMappedObjects);
	assert(SUCCEEDED(hResult) && "m_pConstBuffer->Map Failed");
}

void CMinionHPGaugeShader::BuildObjects(CCreateMgr *pCreateMgr, void *pContext)
{
	m_pCamera = (CCamera*)pContext;

	UINT ncbElementBytes = ((sizeof(CB_GAUGE_INFO) + 255) & ~255);

	CreateShaderVariables(pCreateMgr, MAX_MINION * 2);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, MAX_MINION * 2, 1);

	CreateConstantBufferViews(pCreateMgr, MAX_MINION * 2, m_pConstBuffer, ncbElementBytes);

#if USE_BATCH_MATERIAL
	m_nMaterials = 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];

	m_ppMaterials[0] = Materials::CreateRedMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
#else
	CMaterial *pCubeMaterial = Materials::CreateRedMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle[0], &m_srvGPUDescriptorStartHandle[0]);
#endif

}

void CMinionHPGaugeShader::SpawnGauge()
{
	static UINT incrementSize{ m_pCreateMgr->GetCbvSrvDescriptorIncrementSize() };

	m_pCreateMgr->ResetCommandList();

	int cnt{ 0 };

	for (; cnt < m_pGaugeManger->GetCount(); ++cnt)
	{
		CHPGaugeObjects *pGaugeObject{ NULL };
		CCollisionObject *pMinionObjects{ NULL };

		pGaugeObject = new CHPGaugeObjects(m_pCreateMgr, GagueUIType::MinionGauge);
		pMinionObjects = (*m_MinionObjectList)[cnt];

		pGaugeObject->SetObject(pMinionObjects);
		pGaugeObject->SetCamera(m_pCamera);

		pGaugeObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * cnt));

		m_HPGaugeObjectList.emplace_back(pGaugeObject);
	}
	m_pCreateMgr->ExecuteCommandList();

	for (int i = 0; i < cnt; ++i)
	{
		m_HPGaugeObjectList[i]->ReleaseUploadBuffers();
	}

	m_nBlues = m_pGaugeManger->GetBlueMinionCnt();
	m_nReds = m_pGaugeManger->GetRedMinionCnt();

}

void CMinionHPGaugeShader::ReleaseObjects()
{
	for (auto& iter = m_HPGaugeObjectList.begin(); iter != m_HPGaugeObjectList.end();)
	{
		iter = m_HPGaugeObjectList.erase(iter);
	}
	m_HPGaugeObjectList.clear();

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