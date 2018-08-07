#pragma once

class CCreateMgr;

struct SRVROOTARGUMENTINFO
{
	UINT							m_nRootParameterIndex{ 0 };
	D3D12_GPU_DESCRIPTOR_HANDLE		m_srvGpuDescriptorHandle;
};

class CTexture
{
public:	// 생성자, 소멸자
	CTexture(int nTextureResources = 1, UINT nResourceType = RESOURCE_TEXTURE_2D, int nSamplers = 0);
	virtual ~CTexture();

public: // 공개 함수
	void Finalize();

	void ReleaseUploadBuffers();

	void SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE srvGpuDescriptorHandle);
	void SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE samplerGpuDescriptorHandle);

	void UpdateShaderVariables();
	void UpdateShaderVariable(int nIndex);

	void LoadTextureFromFile(shared_ptr<CCreateMgr> pCreateMgr, wchar_t *pszFileName, UINT nIndex);
	ComPtr<ID3D12Resource> CreateTexture(shared_ptr<CCreateMgr> pCreateMgr,
		UINT nWidth, UINT nHeight,
		DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS resourceFlags,
		D3D12_RESOURCE_STATES resourceStates, D3D12_CLEAR_VALUE *pClearValue, UINT nIndex);

	int GetTextureCount() { return(m_nTextures); }
	ComPtr<ID3D12Resource> GetTexture(int nIndex) { return(m_ppTextures[nIndex]); }
	UINT GetTextureType() { return(m_textureType); }

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

protected: // 내부 함수
	void ReleaseShaderVariables();

protected: // 변수
	int	m_nReferences{ 0 };

	UINT							m_textureType{ RESOURCE_TEXTURE_2D };
	int								m_nTextures{ 0 };
	ComPtr<ID3D12Resource>			*m_ppTextures{ NULL };
	ComPtr<ID3D12Resource>			*m_ppTextureUploadBuffers{ NULL };
	SRVROOTARGUMENTINFO				*m_pRootArgumentInfos{ NULL };

	int								m_nSamplers{ 0 };
	D3D12_GPU_DESCRIPTOR_HANDLE		*m_pSamplerGpuDescriptorHandles{ NULL };

	ComPtr<ID3D12GraphicsCommandList> m_pCommandList;
};

