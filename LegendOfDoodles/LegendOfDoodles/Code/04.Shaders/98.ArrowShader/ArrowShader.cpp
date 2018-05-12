#include "stdafx.h"
#include "ArrowShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"

/// <summary>
/// 목적: 작업 시 화살표 표시용 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-05-09
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CArrowShader::CArrowShader(CCreateMgr* pCreateMgr) : CShader(pCreateMgr)
{
}

CArrowShader::~CArrowShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CArrowShader::ReleaseUploadBuffers()
{
	if (m_pArrow)  m_pArrow->ReleaseUploadBuffers();
	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}
}

void CArrowShader::UpdateShaderVariables()
{
	if (m_pMappedArrow)
	{
		XMStoreFloat4x4(&m_pMappedArrow->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_pArrow->GetWorldMatrix())));
	}
}

void CArrowShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera);

	if (m_ppMaterials) m_ppMaterials[0]->UpdateShaderVariables();
	if (m_pArrow) m_pArrow->Render(pCamera);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CArrowShader::CreateInputLayout()
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
		"COLOR",
		0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0,
		12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
	inputLayoutDesc.pInputElementDescs = pInputElementDescs;
	inputLayoutDesc.NumElements = nInputElementDescs;
	return(inputLayoutDesc);
}

D3D12_SHADER_BYTECODE CArrowShader::CreateVertexShader(ID3DBlob **ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSDiffused",
		"vs_5_1",
		ppShaderBlob));
}

D3D12_SHADER_BYTECODE CArrowShader::CreatePixelShader(ID3DBlob **ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"PSDiffused",
		"ps_5_1",
		ppShaderBlob));
}

void CArrowShader::CreateShader(CCreateMgr *pCreateMgr)
{
	m_nPipelineStates = 1;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr);
}

void CArrowShader::CreateShaderVariables(CCreateMgr * pCreateMgr, int nBuffers)
{
	UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	m_pConstBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		elementBytes,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	HRESULT hResult = m_pConstBuffer->Map(0, NULL, (void **)&m_pMappedArrow);
	assert(SUCCEEDED(hResult) && "m_pConstBuffer->Map Failed");
}

void CArrowShader::BuildObjects(CCreateMgr * pCreateMgr, void * pContext)
{
	m_pArrow = new CArrow(pCreateMgr);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, 1, 0);
	CreateShaderVariables(pCreateMgr);
	CreateConstantBufferViews(pCreateMgr, 1, m_pConstBuffer, ncbElementBytes);

	m_pArrow->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr);
}

void CArrowShader::ReleaseObjects()
{
	Safe_Delete(m_pArrow);
	if (m_ppMaterials)
	{
		for (int i = 0; i < m_nMaterials; ++i)
		{
			if (m_ppMaterials[i]) delete m_ppMaterials[i];
		}
		Safe_Delete(m_ppMaterials);
	}
}
