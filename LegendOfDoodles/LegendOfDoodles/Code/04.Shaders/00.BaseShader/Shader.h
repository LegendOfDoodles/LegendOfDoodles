#pragma once
#include "05.Objects/02.CollisionObject/CollisionObject.h"

#define BOUNDING_BOX 1

class CCreateMgr;
class CCamera;
class CTexture;
class CNetwork;

class CShader
{
public:	// 생성자, 소멸자
	CShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CShader();

public: // 공개 함수
	virtual void Initialize(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);
	virtual void Finalize();

	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables(int opt = 0);
	virtual void UpdateShaderVariable(XMFLOAT4X4 *pxmf4x4World);

	virtual void UpdateBoundingBoxShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);
	void Render(CCamera *pCamera, int opt);
	void Render(CCamera *pCamera, int opt, int index);
	virtual void RenderBoundingBox(CCamera *pCamera);
	virtual void RenderShadow(CCamera *pCamera);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);
	virtual bool OnProcessMouseInput(WPARAM pKeyBuffer);

	bool HasBoundingBox() { return m_isRenderBB; }

	virtual void OnStatus(int ObjectType);
	virtual void OffStatus();

	virtual void CreateGraphicsRootSignature(shared_ptr<CCreateMgr> pCreateMgr) { pCreateMgr; }

	void SaveBoundingBoxHeapNumber(int n) { m_boundingBoxHeapNumber = n; }
	ComPtr<ID3D12RootSignature> GetGraphicsRootSignature() { return(m_pGraphicsRootSignature); }

	virtual void SetCamera(CCamera *pCamera) { pCamera; }

	CCollisionObject **GetCollisionObjects() { return m_ppObjects; }
	int GetObjectCount() { return m_nObjects; }

	void SetNetwork(shared_ptr<CNetwork> pNetwork) { m_pNetwork = pNetwork; }

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual void SetChangeWeapon(int id) { id; }

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_RASTERIZER_DESC CreateShadowRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	D3D12_INPUT_LAYOUT_DESC CreateBoundingBoxInputLayout();
	D3D12_RASTERIZER_DESC CreateBoundingBoxRasterizerState();

	void CreateDescriptorHeaps();
	void CreateCbvAndSrvDescriptorHeaps(shared_ptr<CCreateMgr> pCreateMgr, int nConstantBufferViews, int nShaderResourceViews, int index = 0);
	void CreateConstantBufferViews(shared_ptr<CCreateMgr> pCreateMgr, int nConstantBufferViews, ID3D12Resource *pConstantBuffers, UINT nStride, int startNum = 0, int heapIndex = 0);
	void GetShaderResourceViewDesc(D3D12_RESOURCE_DESC resourceDesc, UINT nTextureType, D3D12_SHADER_RESOURCE_VIEW_DESC *pShaderResourceViewDesc);
	void CreateShaderResourceViews(shared_ptr<CCreateMgr> pCreateMgr, CTexture *pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement, int index = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateHullShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateDomainShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	D3D12_SHADER_BYTECODE CreateBoundingBoxVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	D3D12_SHADER_BYTECODE CreateBoundingBoxPixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual D3D12_SHADER_BYTECODE CreateShadowVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateShadowHullShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateShadowDomainShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateShadowPixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShaderWithTess(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderShadow = false);
	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);
	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, ComPtr<ID3D12RootSignature> pGraphicsRootSignature, UINT nRenderTargets = 1);

	virtual void CreateShaderVariables(shared_ptr<CCreateMgr> pCreateMgr, UINT stride, int nBuffers = 1, bool isRenderBB = false, UINT BBStride = 0, int BBnBuffers = 0);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);

	virtual void ReleaseShaderVariables();
	virtual void ReleaseObjects();

	virtual void OnPrepareRender(int opt = 0, int index = 0);
	void OnPrepareRenderForBB();

	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName,
		LPCSTR pszShaderProfile, ComPtr<ID3DBlob>& pShaderBlob);

protected: // 변수
	int m_nReferences{ 0 };

	bool m_isRenderBB{ false };

	std::vector<ComPtr<ID3D12PipelineState>> m_ppPipelineStates;
	int m_nPipelineStates{ 0 };

	ComPtr<ID3D12Resource> m_pConstBuffer;
	ComPtr<ID3D12Resource> m_pBoundingBoxBuffer;

#if USE_BATCH_MATERIAL
	CMaterial			**m_ppMaterials{ NULL };
	int					m_nMaterials{ 0 };
#endif

	CCollisionObject **m_ppObjects{ NULL };
	int m_nObjects{ 0 };

	UINT8 *m_pMappedObjects{ NULL };
	UINT8 *m_pMappedBoundingBoxes{ NULL };

	std::vector<ComPtr<ID3D12DescriptorHeap>> m_ppCbvSrvDescriptorHeaps;
	int m_nHeaps{ 0 };
	int m_boundingBoxHeapNumber{ 0 };

	D3D12_CPU_DESCRIPTOR_HANDLE		*m_pcbvCPUDescriptorStartHandle{ NULL };
	D3D12_GPU_DESCRIPTOR_HANDLE		*m_pcbvGPUDescriptorStartHandle{ NULL };
	D3D12_CPU_DESCRIPTOR_HANDLE		*m_psrvCPUDescriptorStartHandle{ NULL };
	D3D12_GPU_DESCRIPTOR_HANDLE		*m_psrvGPUDescriptorStartHandle{ NULL };

	ComPtr<ID3D12GraphicsCommandList> m_pCommandList;

	ComPtr<ID3D12RootSignature>		m_pGraphicsRootSignature;

	shared_ptr<CNetwork> m_pNetwork;
	bool m_bBulidFinished{ false };
};

