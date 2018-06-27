
#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CMaterial;
class CHeightMapTerrain;
class CCollisionManager;

class CNexusTowerShader : public CShader
{
public: // 생성자, 소멸자
	CNexusTowerShader(CCreateMgr *pCreateMgr, Network* network);
	virtual ~CNexusTowerShader();

public: // 공개 함수
	virtual void Initialize(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables();
	virtual void UpdateBoundingBoxShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);
	virtual void RenderBoundingBox(CCamera *pCamera);

	virtual CBaseObject *PickObjectByRayIntersection(
		XMFLOAT3& pickPosition, XMFLOAT4X4& xmf4x4View, float &nearHitDistance);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);

	int GetObjectCount() { return m_nObjects; }
	virtual CBaseObject **GetCollisionObjects() { return m_ppObjects; }

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false);
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers = 1);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

	void SetBoundingBoxMeshByIndex(CCreateMgr *pCreateMgr, CBaseObject* target, int index);

protected: // 변수
	CBaseObject * *m_ppObjects{ NULL };
	int m_nObjects{ 0 };
	int m_meshCounts[4];

	ID3D12Resource *m_myConstBuffer{ NULL };

	UINT8 *m_pMappedObjects{ NULL };
	UINT8 *m_pMappedBoundingBoxes{ NULL };

	CHeightMapTerrain * m_pTerrain{ NULL };

	Network* m_pNetwork;
};