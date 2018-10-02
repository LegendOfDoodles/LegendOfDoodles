#include "stdafx.h"
#include "TerrainShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"

/// <summary>
/// 목적: 지형 출력용 Shader
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-10-01
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CTerrainShader::CTerrainShader(shared_ptr<CCreateMgr> pCreateMgr) : CShader(pCreateMgr)
{
}

CTerrainShader::~CTerrainShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CTerrainShader::Initialize(shared_ptr<CCreateMgr> pCreateMgr, void * pContext)
{
	pCreateMgr->ResetCommandList();
	CreateShader(pCreateMgr, RENDER_TARGET_BUFFER_CNT, false, true);
	BuildObjects(pCreateMgr, pContext);
	pCreateMgr->ExecuteCommandList();
}

void CTerrainShader::ReleaseUploadBuffers()
{
	if (m_pTerrain)  m_pTerrain->ReleaseUploadBuffers();
	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}

	m_bBulidFinished = true;
}

void CTerrainShader::UpdateShaderVariables(int opt)
{
	UNREFERENCED_PARAMETER(opt);
	CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedObjects);

	XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
		XMMatrixTranspose(XMLoadFloat4x4(m_pTerrain->GetWorldMatrix())));
}

void CTerrainShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera);

	if (m_ppMaterials) m_ppMaterials[0]->UpdateShaderVariables();
	if (m_pTerrain) m_pTerrain->Render(pCamera);
}

void CTerrainShader::RenderShadow(CCamera * pCamera)
{
	CShader::Render(pCamera, 0, 1);

	if (m_pTerrain) m_pTerrain->Render(pCamera);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CTerrainShader::CreateInputLayout()
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

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
	inputLayoutDesc.pInputElementDescs = pInputElementDescs;
	inputLayoutDesc.NumElements = nInputElementDescs;
	return(inputLayoutDesc);
}

D3D12_SHADER_BYTECODE CTerrainShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/TerrainShader.hlsl",
		"VSTerrain",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CTerrainShader::CreateHullShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/TerrainShader.hlsl",
		"HSTerrain",
		"hs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CTerrainShader::CreateDomainShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/TerrainShader.hlsl",
		"DSTerrain",
		"ds_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CTerrainShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/TerrainShader.hlsl",
		"PSTerrain",
		"ps_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CTerrainShader::CreateShadowVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/ShadowShader.hlsl",
		"VSTerrain",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CTerrainShader::CreateShadowHullShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/ShadowShader.hlsl",
		"HSTerrain",
		"hs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CTerrainShader::CreateShadowDomainShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/ShadowShader.hlsl",
		"DSTerrain",
		"ds_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CTerrainShader::CreateShadowPixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/ShadowShader.hlsl",
		"PSTerrain",
		"ps_5_1",
		pShaderBlob));
}

void CTerrainShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	UNREFERENCED_PARAMETER(isRenderBB);

	m_nPipelineStates = 2;

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShaderWithTess(pCreateMgr, nRenderTargets, isRenderShadow);
}

void CTerrainShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void * pContext)
{
	UNREFERENCED_PARAMETER(pContext);

	m_pTerrain = new CHeightMapTerrain(pCreateMgr, _T("Resource/Terrain/HeightMap.raw"), TERRAIN_IMAGE_SCALE);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, 1, 1);
	CreateShaderVariables(pCreateMgr, ncbElementBytes);
	CreateConstantBufferViews(pCreateMgr, 1, m_pConstBuffer.Get(), ncbElementBytes);

	m_nMaterials = 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = Materials::CreateTerrainMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);

	m_pTerrain->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr);
}

void CTerrainShader::ReleaseObjects()
{
	Safe_Delete(m_pTerrain);
	if (m_ppMaterials)
	{
		for (int i = 0; i < m_nMaterials; ++i)
		{
			Safe_Delete(m_ppMaterials[i]);
		}
		Safe_Delete_Array(m_ppMaterials);
	}
}
