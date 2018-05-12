#pragma once

#define RESOURCE_TEXTURE_2D			0x01
#define RESOURCE_TEXTURE_2D_ARRAY	0x02
#define RESOURCE_TEXTURE_CUBE		0x03
#define RESOURCE_BUFFER				0x04

class CCreateMgr;

struct SRVROOTARGUMENTINFO
{
	UINT							m_nRootParameterIndex{ 0 };
	D3D12_GPU_DESCRIPTOR_HANDLE		m_srvGpuDescriptorHandle;
};

class CTexture
{
public:	// ������, �Ҹ���
	CTexture(int nTextureResources = 1, UINT nResourceType = RESOURCE_TEXTURE_2D, int nSamplers = 0);
	virtual ~CTexture();

public: // ���� �Լ�
	void Finalize();

	void ReleaseUploadBuffers();

	void SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE srvGpuDescriptorHandle);
	void SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE samplerGpuDescriptorHandle);

	void UpdateShaderVariables();
	void UpdateShaderVariable(int nIndex);

	void LoadTextureFromFile(CCreateMgr *pCreateMgr, wchar_t *pszFileName, UINT nIndex);

	int GetTextureCount() { return(m_nTextures); }
	ID3D12Resource *GetTexture(int nIndex) { return(m_ppTextures[nIndex]); }
	UINT GetTextureType() { return(m_textureType); }

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

protected: // ���� �Լ�
	void ReleaseShaderVariables();

protected: // ����
	int	m_nReferences{ 0 };

	UINT							m_textureType{ RESOURCE_TEXTURE_2D };
	int								m_nTextures{ 0 };
	ID3D12Resource					**m_ppTextures{ NULL };
	ID3D12Resource					**m_ppTextureUploadBuffers{ NULL };
	SRVROOTARGUMENTINFO				*m_pRootArgumentInfos{ NULL };

	int								m_nSamplers{ 0 };
	D3D12_GPU_DESCRIPTOR_HANDLE		*m_pSamplerGpuDescriptorHandles{ NULL };

	ID3D12GraphicsCommandList *m_pCommandList{ NULL };
};

