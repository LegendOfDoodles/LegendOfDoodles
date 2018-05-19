#include "stdafx.h"
#include "TerrainShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"

/// <summary>
/// 목적: 지형 출력용 Shader
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-05-09
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CTerrainShader::CTerrainShader(CCreateMgr *pCreateMgr) : CShader(pCreateMgr)
{
}

CTerrainShader::~CTerrainShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CTerrainShader::ReleaseUploadBuffers()
{
	if (m_pTerrain)  m_pTerrain->ReleaseUploadBuffers();
	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}
}

void CTerrainShader::UpdateShaderVariables()
{
	if (m_pMappedTerrain)
	{
		XMStoreFloat4x4(&m_pMappedTerrain->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_pTerrain->GetWorldMatrix())));
	}
}

void CTerrainShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera);

	if (m_ppMaterials) m_ppMaterials[0]->UpdateShaderVariables();
	if (m_pTerrain) m_pTerrain->Render(pCamera);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CTerrainShader::CreateInputLayout()
{
	UINT nInputElementDescs = 3;
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
	pInputElementDescs[2] = {
		"BCOLOR",
		0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0,
		20,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
	inputLayoutDesc.pInputElementDescs = pInputElementDescs;
	inputLayoutDesc.NumElements = nInputElementDescs;
	return(inputLayoutDesc);
}

D3D12_SHADER_BYTECODE CTerrainShader::CreateVertexShader(ID3DBlob **ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", 
		"VSDiffuseTextured", 
		"vs_5_1",
		ppShaderBlob));
}

D3D12_SHADER_BYTECODE CTerrainShader::CreatePixelShader(ID3DBlob **ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", 
		"PSDiffuseTextured", 
		"ps_5_1",
		ppShaderBlob));
}

void CTerrainShader::CreateShader(CCreateMgr *pCreateMgr, UINT nRenderTargets)
{
	m_nPipelineStates = 1;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets);
}

void CTerrainShader::CreateShaderVariables(CCreateMgr * pCreateMgr, int nBuffers)
{
	UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	m_pConstBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		elementBytes,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	HRESULT hResult = m_pConstBuffer->Map(0, NULL, (void **)&m_pMappedTerrain);
	assert(SUCCEEDED(hResult) && "m_pConstBuffer->Map Failed");
}

void CTerrainShader::BuildObjects(CCreateMgr * pCreateMgr, void * pContext)
{
	XMFLOAT3 xmf3Scale(14.0f, 0.7f, 14.0f);
	XMFLOAT4 xmf4Color(0.6f, 0.6f, 0.6f, 0.0f);

#ifdef _WITH_TERRAIN_PARTITION
	m_pTerrain = new CHeightMapTerrain(pCreateMgr, _T("Resource/Terrain/HeightMap.raw"),
		TERRAIN_IMAGE_WIDTH, TERRAIN_IMAGE_HEIGHT,
		TERRAIN_IMAGE_WIDTH / 10, TERRAIN_IMAGE_HEIGHT / 10,
		TERRAIN_IMAGE_SCALE, XMFLOAT4(0.6f, 0.6f, 0.6f, 0.0f));
#else
	m_pTerrain = new CHeightMapTerrain(pCreateMgr, _T("Resource/Terrain/HeightMap.raw"),
		TERRAIN_IMAGE_WIDTH, TERRAIN_IMAGE_HEIGHT,
		TERRAIN_IMAGE_WIDTH, TERRAIN_IMAGE_HEIGHT,
		TERRAIN_IMAGE_SCALE, XMFLOAT4(0.6f, 0.6f, 0.6f, 0.0f));
#endif
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, 1, 1);
	CreateShaderVariables(pCreateMgr);
	CreateConstantBufferViews(pCreateMgr, 1, m_pConstBuffer, ncbElementBytes);

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
			if (m_ppMaterials[i]) delete m_ppMaterials[i];
		}
		Safe_Delete(m_ppMaterials);
	}
}
