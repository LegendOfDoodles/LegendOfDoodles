#include "stdafx.h"
#include "BillboardShader.h"
#include "05.Objects/95.Billboard/Billboard.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"

/// <summary>
/// 목적: Billboard 테스트 쉐이더
/// 최종 수정자:  이용선
/// 수정자 목록:  이용선
/// 최종 수정 날짜: 2018-05-09
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CBillboardShader::CBillboardShader(CCreateMgr *pCreateMgr)
	: CShader(pCreateMgr)
{
}

CBillboardShader::~CBillboardShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CBillboardShader::ReleaseUploadBuffers()
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

void CBillboardShader::UpdateShaderVariables()
{
#if USE_INSTANCING
	m_pCommandList->SetGraphicsRootShaderResourceView(2,
		m_pInstanceBuffer->GetGPUVirtualAddress());

	for (int i = 0; i < m_nObjects; i++)
	{
		XMStoreFloat4x4(&m_pMappedObjects[i].m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
#else
	static UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedObjects + (i * elementBytes));
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
#endif
}

void CBillboardShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CBillboardShader::Render(CCamera *pCamera)
{
	CShader::Render(pCamera);
#if USE_BATCH_MATERIAL
	if (m_ppMaterials) m_ppMaterials[0]->UpdateShaderVariables();
#endif

#if USE_INSTANCING
	m_ppObjects[0]->Render(pCamera, m_nObjects);
#else
	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j]) m_ppObjects[j]->Render(pCamera);
	}
#endif
}

void CBillboardShader::OnProcessKeyUp(WPARAM wParam, LPARAM lParam, float timeElapsed)
{

}

void CBillboardShader::OnProcessKeyDown(WPARAM wParam, LPARAM lParam, float timeElapsed)
{

}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CBillboardShader::CreateInputLayout()
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

D3D12_SHADER_BYTECODE CBillboardShader::CreateVertexShader(ID3DBlob **ppShaderBlob)
{
	//./Code/04.Shaders/99.GraphicsShader/
#if USE_INSTANCING
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSTexturedLightingInstancing",
		"vs_5_1",
		ppShaderBlob));
#else
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSTextured",
		"vs_5_1",
		ppShaderBlob));
#endif
}

D3D12_SHADER_BYTECODE CBillboardShader::CreatePixelShader(ID3DBlob **ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"PSTextured",
		"ps_5_1",
		ppShaderBlob));
}

void CBillboardShader::CreateShader(CCreateMgr *pCreateMgr)
{
	m_nPipelineStates = 1;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr);
}

void CBillboardShader::CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers)
{
	HRESULT hResult;

#if USE_INSTANCING
	m_pInstanceBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		sizeof(CB_GAMEOBJECT_INFO) * nBuffers,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL);

	hResult = m_pInstanceBuffer->Map(0, NULL, (void **)&m_pMappedObjects);
	assert(SUCCEEDED(hResult) && "m_pInstanceBuffer->Map Failed");
#else
	UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	m_pConstBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		elementBytes * nBuffers,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	hResult = m_pConstBuffer->Map(0, NULL, (void **)&m_pMappedObjects);
	assert(SUCCEEDED(hResult) && "m_pConstBuffer->Map Failed");
#endif


}

void CBillboardShader::BuildObjects(CCreateMgr *pCreateMgr, void *pContext)
{
	CCamera *pCamera = (CCamera*)pContext;

	int xObjects = 10, yObjects = 0, zObjects = 10, i = 0;

	m_nObjects = (xObjects * 2 + 1) * (yObjects * 2 + 1) * (zObjects * 2 + 1);
	m_ppObjects = new CBaseObject*[m_nObjects];

#if USE_INSTANCING
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, 0, 2);
	CreateShaderVariables(pCreateMgr);
#else

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 1);
	CreateShaderVariables(pCreateMgr, m_nObjects);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pConstBuffer, ncbElementBytes);

#endif

#if USE_BATCH_MATERIAL
	m_nMaterials = 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = Materials::CreateGreyMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
#else
	CMaterial *pCubeMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
#endif

	float fxPitch = 12.0f * 5.f;
	float fyPitch = 12.0f * 5.f;
	float fzPitch = 12.0f * 5.f;

	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CBillboardObject *pBillboardObject = NULL;
	for (int y = -yObjects; y <= yObjects; y++)
	{
		for (int z = -zObjects; z <= zObjects; z++)
		{
			for (int x = -xObjects; x <= xObjects; x++)
			{

				pBillboardObject = new CBillboardObject(pCreateMgr);
#if !USE_INSTANCING
#endif
#if !USE_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				pBillboardObject->SetPosition(x * 30 + 200, y * 100 + 100, z * 100 + 1000);
				pBillboardObject->SetCamera(pCamera);
#if !USE_INSTANCING
				pBillboardObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * i));
#endif
				m_ppObjects[i++] = pBillboardObject;
			}
		}
	}

#if USE_INSTANCING
	m_ppObjects[0]->SetMesh(0, pCubeMesh);
#endif
}

void CBillboardShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			Safe_Delete(m_ppObjects[j]);
		}
		Safe_Delete_Array(m_ppObjects);
	}

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