#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"

#define BOUNDING_BOX 1

class CCreateMgr;
class CCamera;
class CTexture;

class CShader	// Warning!  Ani Shader가 오브젝트 쉐이더 상속받으면 적당히 기능 나눌 필요 있음
{
public:	// 생성자, 소멸자
	CShader(CCreateMgr *pCreateMgr);
	virtual ~CShader();

public: // 공개 함수
	virtual void Initialize(CCreateMgr *pCreateMgr, void *pContext = NULL);
	virtual void Finalize();

	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables();
	virtual void UpdateShaderVariable(XMFLOAT4X4 *pxmf4x4World);

	virtual void UpdateBoundingBoxShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);
	void Render(CCamera *pCamera, int opt);
	virtual void RenderBoundingBox(CCamera *pCamera);

	virtual CBaseObject *PickObjectByRayIntersection(
		XMFLOAT3& pickPosition, XMFLOAT4X4& xmf4x4View, float &nearHitDistance);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);
	virtual bool OnProcessMouseInput(WPARAM pKeyBuffer);

	bool HasBoundingBox() { return m_nPipelineStates > 1; }

	virtual void OnStatus(int ObjectType);
	virtual void OffStatus();

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	virtual CBaseObject * * GetCollisionObjects() { return nullptr; }

	void SaveBoundingBoxHeapNumber(int n) { m_boundingBoxHeapNumber = n; }

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	D3D12_INPUT_LAYOUT_DESC CreateBoundingBoxInputLayout();
	D3D12_RASTERIZER_DESC CreateBoundingBoxRasterizerState();

	void CreateDescriptorHeaps();
	void CreateCbvAndSrvDescriptorHeaps(CCreateMgr *pCreateMgr, int nConstantBufferViews, int nShaderResourceViews, int index = 0);
	void CreateConstantBufferViews(CCreateMgr *pCreateMgr, int nConstantBufferViews, ID3D12Resource *pConstantBuffers, UINT nStride, int index = 0);
	void CreateShaderResourceViews(CCreateMgr *pCreateMgr, CTexture *pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement, int index = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppShaderBlob);

	D3D12_SHADER_BYTECODE CreateBoundingBoxVertexShader(ID3DBlob **ppShaderBlob);
	D3D12_SHADER_BYTECODE CreateBoundingBoxPixelShader(ID3DBlob **ppShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr);
	void CreateBoundingBoxShader(CCreateMgr *pCreateMgr);
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers = 1);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseShaderVariables();
	virtual void ReleaseObjects();

	virtual void OnPrepareRender(int opt = 0);
	void OnPrepareRenderForBB();

	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName,
		LPCSTR pszShaderProfile, ID3DBlob **ppShaderBlob);

protected: // 변수
	int m_nReferences{ 0 };

	ID3D12PipelineState **m_ppPipelineStates{ NULL };
	int m_nPipelineStates{ 0 };

	ID3D12Resource *m_pInstanceBuffer{ NULL };
	ID3D12Resource *m_pConstBuffer{ NULL };
	ID3D12Resource *m_pBoundingBoxBuffer{ NULL };

#if USE_BATCH_MATERIAL
	CMaterial			**m_ppMaterials{ NULL };
	int					m_nMaterials{ 0 };
#endif

	ID3D12DescriptorHeap			**m_ppCbvSrvDescriptorHeaps{ NULL };
	int m_nHeaps{ 0 };
	int m_boundingBoxHeapNumber{ 0 };

	D3D12_CPU_DESCRIPTOR_HANDLE		*m_pcbvCPUDescriptorStartHandle{ NULL };
	D3D12_GPU_DESCRIPTOR_HANDLE		*m_pcbvGPUDescriptorStartHandle{ NULL };
	D3D12_CPU_DESCRIPTOR_HANDLE		*m_psrvCPUDescriptorStartHandle{ NULL };
	D3D12_GPU_DESCRIPTOR_HANDLE		*m_psrvGPUDescriptorStartHandle{ NULL };

	ID3D12GraphicsCommandList *m_pCommandList{ NULL };
};

