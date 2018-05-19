#include "stdafx.h"
#include "Texture.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"

/// <summary>
/// 목적: 텍스처에 필요한 처리 담당하는 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-05-19
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CTexture::CTexture(int nTextures, UINT nTextureType, int nSamplers)
{
	m_textureType = nTextureType;
	m_nTextures = nTextures;
	if (m_nTextures > 0)
	{
		m_pRootArgumentInfos = new SRVROOTARGUMENTINFO[m_nTextures];
		m_ppTextureUploadBuffers = new ID3D12Resource*[m_nTextures];
		m_ppTextures = new ID3D12Resource*[m_nTextures];
	}

	m_nSamplers = nSamplers;
	if (m_nSamplers > 0) m_pSamplerGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nSamplers];
}

CTexture::~CTexture()
{
	if (m_ppTextures)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppTextures[i]) m_ppTextures[i]->Release();
	}

	if (m_pRootArgumentInfos)
	{
		delete[] m_pRootArgumentInfos;
	}

	if (m_pSamplerGpuDescriptorHandles) delete[] m_pSamplerGpuDescriptorHandles;
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CTexture::Finalize()
{
	ReleaseShaderVariables();
}

void CTexture::ReleaseUploadBuffers()
{
	if (!m_ppTextureUploadBuffers) return;

	for (int i = 0; i < m_nTextures; i++)
	{
		if (m_ppTextureUploadBuffers[i])
		{
			m_ppTextureUploadBuffers[i]->Release();
		}
	}

	Safe_Delete_Array(m_ppTextureUploadBuffers);
}

void CTexture::SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE srvGpuDescriptorHandle)
{
	m_pRootArgumentInfos[nIndex].m_nRootParameterIndex = nRootParameterIndex;
	m_pRootArgumentInfos[nIndex].m_srvGpuDescriptorHandle = srvGpuDescriptorHandle;
}

void CTexture::SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE samplerGpuDescriptorHandle)
{
	m_pSamplerGpuDescriptorHandles[nIndex] = samplerGpuDescriptorHandle;
}

void CTexture::UpdateShaderVariables()
{
	if (m_textureType == RESOURCE_TEXTURE_2D_ARRAY)
	{
		m_pCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[0].m_nRootParameterIndex, m_pRootArgumentInfos[0].m_srvGpuDescriptorHandle);
	}
	else
	{
		for (int i = 0; i < m_nTextures; i++)
		{
			m_pCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[i].m_nRootParameterIndex, m_pRootArgumentInfos[i].m_srvGpuDescriptorHandle);
		}
	}
}

void CTexture::UpdateShaderVariable(int nIndex)
{
	m_pCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[nIndex].m_nRootParameterIndex, m_pRootArgumentInfos[nIndex].m_srvGpuDescriptorHandle);
}

void CTexture::LoadTextureFromFile(CCreateMgr *pCreateMgr, wchar_t *pszFileName, UINT nIndex)
{
	m_pCommandList = pCreateMgr->GetCommandList();
	m_ppTextures[nIndex] = pCreateMgr->CreateTextureResourceFromFile(pszFileName, &m_ppTextureUploadBuffers[nIndex], D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

ID3D12Resource * CTexture::CreateTexture(CCreateMgr *pCreateMgr, UINT nWidth, UINT nHeight, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS resourceFlags, D3D12_RESOURCE_STATES resourceStates, D3D12_CLEAR_VALUE * pClearValue, UINT nIndex)
{
	m_pCommandList = pCreateMgr->GetCommandList();
	m_ppTextures[nIndex] = pCreateMgr->CreateTexture2DResource(nWidth, nHeight, dxgiFormat, resourceFlags, resourceStates, pClearValue);
	return(m_ppTextures[nIndex]);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CTexture::ReleaseShaderVariables()
{
}
