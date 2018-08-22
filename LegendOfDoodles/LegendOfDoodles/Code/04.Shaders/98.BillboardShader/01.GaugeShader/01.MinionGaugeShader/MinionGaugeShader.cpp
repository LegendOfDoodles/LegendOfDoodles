#include "stdafx.h"
#include "MinionGaugeShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/06.Minion/Minion.h"
#include "05.Objects/99.Material/Material.h"
#include "00.Global/01.Utility/06.HPGaugeManager/HPGaugeManager.h"
#include "05.Objects/96.Billboard/02.GaugeObject/GaugeObject.h"

/// <summary>
/// 목적: 미니언 HP Gauge 쉐이더
/// 최종 수정자:  김나단(Shader Code 변경 hlsl)
/// 수정자 목록:  이용선, 김나단
/// 최종 수정 날짜: 2018-08-22
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CMinionHPGaugeShader::CMinionHPGaugeShader(shared_ptr<CCreateMgr> pCreateMgr)
	: CBillboardShader(pCreateMgr)
{
	m_pCreateMgr = pCreateMgr;
}

CMinionHPGaugeShader::~CMinionHPGaugeShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CMinionHPGaugeShader::UpdateShaderVariables(int opt)
{
	UNREFERENCED_PARAMETER(opt);
	static UINT elementBytes = ((sizeof(CB_GAUGE_INFO) + 255) & ~255);

	for (auto iter = m_HPGaugeObjectList.begin(); iter != m_HPGaugeObjectList.end(); ++iter) {
		CB_GAUGE_INFO *pMappedObject = (CB_GAUGE_INFO *)(m_pMappedObjects + ((*iter)->GetIndex() * elementBytes));
		pMappedObject->m_fCurrentHP = (*iter)->GetCurrentHP();
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4((*iter)->GetWorldMatrix())));
	}
}

void CMinionHPGaugeShader::AnimateObjects(float timeElapsed)
{
	m_HPGaugeObjectList.remove_if([this](CGaugeObject* obj)
	{
		if (obj->GetState() == States::Die)
		{
			ResetPossibleIndex(obj->GetIndex());
			return true;
		}
		return false;
	});

	for (auto iter = m_HPGaugeObjectList.begin(); iter != m_HPGaugeObjectList.end(); ++iter) {
		(*iter)->Animate(timeElapsed);
	}

	if (m_pGaugeManger->GetCount() > 0) SpawnGauge();
}

void CMinionHPGaugeShader::Render(CCamera *pCamera)
{
	CShader::Render(pCamera);
	//	if (m_ppMaterials) m_ppMaterials[0]->UpdateShaderVariables();

	for (auto iter = m_HPGaugeObjectList.begin(); iter != m_HPGaugeObjectList.end(); ++iter) {

		CCollisionObject* master = ((CGaugeObject*)*iter)->GetMasterObject();

		if (master->GetTeam() == TeamType::Blue)
			m_ppMaterials[0]->UpdateShaderVariable(1);
		else if (master->GetTeam() == TeamType::Red)
			m_ppMaterials[0]->UpdateShaderVariable(0);

		(*iter)->Render(pCamera);
	}
}

void CMinionHPGaugeShader::SetCamera(CCamera * pCamera)
{
	m_pCamera = pCamera;

	for (auto iter = m_HPGaugeObjectList.begin(); iter != m_HPGaugeObjectList.end(); ++iter) {


		static_cast<CGaugeObject*>(*iter)->SetCamera(m_pCamera);
	}
}

void CMinionHPGaugeShader::SetUIObjectsManager(shared_ptr<CUIObjectManager> pManger)
{
	m_pGaugeManger = pManger;
	m_MinionObjectList = m_pGaugeManger->GetMinionObjectList();
}

bool CMinionHPGaugeShader::OnProcessKeyInput(UCHAR * pKeyBuffer)
{
	UNREFERENCED_PARAMETER(pKeyBuffer);

	return true;
}

bool CMinionHPGaugeShader::OnProcessMouseInput(WPARAM pKeyBuffer)
{
	UNREFERENCED_PARAMETER(pKeyBuffer);

	return true;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CMinionHPGaugeShader::CreateInputLayout()
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

D3D12_SHADER_BYTECODE CMinionHPGaugeShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/UIShader.hlsl",
		"VSTexturedGauge",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CMinionHPGaugeShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/UIShader.hlsl",
		"PSTexturedGauge",
		"ps_5_1",
		pShaderBlob));
}

void CMinionHPGaugeShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 1;

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CMinionHPGaugeShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext)
{
	m_pCamera = (CCamera*)pContext;

	UINT ncbElementBytes = ((sizeof(CB_GAUGE_INFO) + 255) & ~255);

	CreateShaderVariables(pCreateMgr, ncbElementBytes, MAX_MINION);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, MAX_MINION, 2);

	CreateConstantBufferViews(pCreateMgr, MAX_MINION, m_pConstBuffer.Get(), ncbElementBytes);

#if USE_BATCH_MATERIAL
	m_nMaterials = 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];

	m_ppMaterials[0] = Materials::CreateHPGaugeMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
#else
	CMaterial *pCubeMaterial = Materials::CreateRedMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle[0], &m_srvGPUDescriptorStartHandle[0]);
#endif

}

int CMinionHPGaugeShader::GetPossibleIndex()
{
	for (int idx = 0; idx < MAX_MINION; ++idx)
	{
		if (!m_indexArr[idx])
		{
			m_indexArr[idx] = true;
			return idx;
		}
	}
	return NONE;
}

void CMinionHPGaugeShader::SpawnGauge()
{
	static UINT incrementSize{ m_pCreateMgr->GetCbvSrvDescriptorIncrementSize() };

	m_pCreateMgr->ResetCommandList();

	int cnt{ 0 };
	CollisionObjectList::reverse_iterator minion{ m_MinionObjectList->rbegin() };

	for (; cnt < m_pGaugeManger->GetCount(); ++cnt)
	{
		int index = GetPossibleIndex();
		if (index == NONE) break;

		CGaugeObject *pGaugeObject{ NULL };
		CCollisionObject *pMinionObjects{ NULL };

		pGaugeObject = new CGaugeObject(m_pCreateMgr, GaugeUIType::MinionGauge);
		pMinionObjects = (*minion);

		m_ppMaterials[0]->AddRef();
		pGaugeObject->SetObject(pMinionObjects);
		pGaugeObject->SetCamera(m_pCamera);

		pGaugeObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * index));

		pGaugeObject->SaveIndex(index);

		XMFLOAT3 xmfGaugePosition;
		xmfGaugePosition = pMinionObjects->GetPosition();
		xmfGaugePosition.y += 70.f;
		pGaugeObject->SetPosition(xmfGaugePosition);
		m_HPGaugeObjectList.emplace_back(pGaugeObject);

		if (minion != m_MinionObjectList->rend()) ++minion;
	}
	m_pCreateMgr->ExecuteCommandList();

	if (!cnt) return;

	HPGaugeObjectList::reverse_iterator gaugeBegin{ m_HPGaugeObjectList.rbegin() };
	HPGaugeObjectList::reverse_iterator gaugeEnd{ m_HPGaugeObjectList.rbegin() };

	for (int i = 0; i < cnt - 1; ++i) ++gaugeBegin;

	for (int i = 0; i < cnt; ++i)
	{
		(*gaugeBegin)->ReleaseUploadBuffers();
		if (gaugeBegin != gaugeEnd) --gaugeBegin;
	}

	//m_pGaugeManger->ResetCount();
}

void CMinionHPGaugeShader::ReleaseObjects()
{
	for (auto iter = m_HPGaugeObjectList.begin(); iter != m_HPGaugeObjectList.end();)
	{
		delete(*iter);
		iter = m_HPGaugeObjectList.erase(iter);
	}
	m_HPGaugeObjectList.clear();

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