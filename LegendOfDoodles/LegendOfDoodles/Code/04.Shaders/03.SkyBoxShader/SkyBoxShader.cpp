#include "stdafx.h"
#include "SkyBoxShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"

/// <summary>
/// 목적: 스카이 박스 처리용 Shader
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-10-01
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CSkyBoxShader::CSkyBoxShader(shared_ptr<CCreateMgr> pCreateMgr) : CShader(pCreateMgr)
{
}

CSkyBoxShader::~CSkyBoxShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CSkyBoxShader::ReleaseUploadBuffers()
{
	if (m_pSkyBox)  m_pSkyBox->ReleaseUploadBuffers();
	if (m_pFloor) m_pFloor->ReleaseUploadBuffers();
	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}

	m_bBulidFinished = true;
}

void CSkyBoxShader::UpdateShaderVariables(int opt)
{
	UNREFERENCED_PARAMETER(opt);
	CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedObjects);

	if (m_pSkyBox)
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_pSkyBox->GetWorldMatrix())));

	if (m_pFloor)
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_pFloor->GetWorldMatrix())));
}

void CSkyBoxShader::Render(CCamera * pCamera)
{
	if (m_pSkyBox)
	{
		XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
		m_pSkyBox->SetPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);
	}

	CShader::Render(pCamera);

	if (m_ppMaterials) m_ppMaterials[0]->UpdateShaderVariables();
	if (m_pSkyBox) m_pSkyBox->Render(pCamera);
	if (m_pFloor) m_pFloor->Render(pCamera);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CSkyBoxShader::CreateInputLayout()
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

D3D12_DEPTH_STENCIL_DESC CSkyBoxShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc;
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_NEVER;
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = 0xff;
	depthStencilDesc.StencilWriteMask = 0xff;
	depthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	return(depthStencilDesc);
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/DefaultShader.hlsl",
		"VSTextured",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/DefaultShader.hlsl",
		"PSTexturedRepeat",
		"ps_5_1",
		pShaderBlob));
}

void CSkyBoxShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 1;

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CSkyBoxShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void * pContext)
{
	UNREFERENCED_PARAMETER(pContext);

	//m_pSkyBox = new CSkyBox(pCreateMgr);
	m_pFloor = new CFloor(pCreateMgr);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	if (m_pSkyBox) CreateCbvAndSrvDescriptorHeaps(pCreateMgr, 1, 6);
	if (m_pFloor) CreateCbvAndSrvDescriptorHeaps(pCreateMgr, 1, 1);

	CreateShaderVariables(pCreateMgr, ncbElementBytes);
	CreateConstantBufferViews(pCreateMgr, 1, m_pConstBuffer.Get(), ncbElementBytes);

	if (m_pSkyBox)
	{
		m_pSkyBox->SetMaterial(Materials::CreateSkyBoxMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]));
		m_pSkyBox->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr);
	}

	if (m_pFloor)
	{
		m_pFloor->SetMaterial(Materials::CreateFloorMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]));
		m_pFloor->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr);
	}
}

void CSkyBoxShader::ReleaseObjects()
{
	Safe_Delete(m_pSkyBox);
	Safe_Delete(m_pFloor);
	if (m_ppMaterials)
	{
		for (int i = 0; i < m_nMaterials; ++i)
		{
			Safe_Delete(m_ppMaterials[i]);
		}
		Safe_Delete_Array(m_ppMaterials);
	}
}
