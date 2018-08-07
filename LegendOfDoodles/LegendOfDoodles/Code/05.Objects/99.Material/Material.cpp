#include "stdafx.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "Material.h"

/// <summary>
/// 목적: 텍스처, 알베도, 재질 등 정리용 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-08-05
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CMaterial::CMaterial(shared_ptr<CCreateMgr> pCreateMgr)
{
	m_pCommandList = pCreateMgr->GetCommandList();
}

CMaterial::~CMaterial()
{
	Safe_Release(m_pTexture);
	Safe_Release(m_pShader);
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CMaterial::Initialize(shared_ptr<CCreateMgr> pCreateMgr)
{
	CreateShaderVariables(pCreateMgr);
}

void CMaterial::Finalize()
{
	if (m_pShader) m_pShader->Finalize();
	if (m_pTexture) m_pTexture->Finalize();

	ReleaseShaderVariables();
	Release();
}

void CMaterial::ReleaseUploadBuffers()
{
	if (m_pTexture) m_pTexture->ReleaseUploadBuffers();
}

void CMaterial::UpdateShaderVariables()
{
	if (m_pShader) m_pShader->UpdateShaderVariables();
	if (m_pTexture) m_pTexture->UpdateShaderVariables();
	UpdateShaderVariable();
}

void CMaterial::UpdateShaderVariable(int nIndex)
{
	if (m_pTexture) m_pTexture->UpdateShaderVariable(nIndex);
}

void CMaterial::Render(CCamera *pCamera)
{
	if (m_pShader) m_pShader->Render(pCamera);
}

void CMaterial::SetTexture(CTexture *pTexture)
{
	if (m_pTexture) m_pTexture->Release();
	m_pTexture = pTexture;
	if (m_pTexture) m_pTexture->AddRef();
}

void CMaterial::SetShader(CShader *pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CMaterial::CreateShaderVariables(shared_ptr<CCreateMgr> pCreateMgr)
{
	static UINT ncbMaterialBytes = ((sizeof(COLOR) + 255) & ~255); //256의 배수
	m_pcbColor = pCreateMgr->CreateBufferResource(
		NULL,
		ncbMaterialBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	m_pcbColor->Map(0, NULL, (void **)&m_pcbMappedColor);
}

void CMaterial::ReleaseShaderVariables()
{
	if (m_pcbColor.Get())
	{
		m_pcbColor->Unmap(0, NULL);
		m_pcbColor.Reset();
	}
}

void CMaterial::UpdateShaderVariable()
{
	::memcpy(m_pcbMappedColor, &m_color, sizeof(COLOR));

	D3D12_GPU_VIRTUAL_ADDRESS cbColorGpuVirtualAddress = m_pcbColor->GetGPUVirtualAddress();
	m_pCommandList->SetGraphicsRootConstantBufferView(5, cbColorGpuVirtualAddress);
}