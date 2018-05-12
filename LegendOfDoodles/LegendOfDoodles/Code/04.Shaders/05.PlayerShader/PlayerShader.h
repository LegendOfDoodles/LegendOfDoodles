#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CMaterial;
class CHeightMapTerrain;
class CCollisionManager;
class CPlayerShader : public CShader
{
public:
	CPlayerShader(CCreateMgr *pCreateMgr, Network* network);
	~CPlayerShader();

public: // 공개 함수
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables();
	virtual void UpdateBoundingBoxShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);
	virtual void RenderBoundingBox(CCamera *pCamera);

	virtual CBaseObject *PickObjectByRayIntersection(
		XMFLOAT3& pickPosition, XMFLOAT4X4& xmf4x4View, float &nearHitDistance);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);


	void SetColManagerToObject(CCollisionManager* manager);
	
	
	virtual CBaseObject **GetCollisionObjects() {	return m_ppObjects; }
	int GetObjectCount() { return m_nObjects;	}

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr);
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers = 1);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // 변수
	CBaseObject * *m_ppObjects{ NULL };
	int m_nObjects = 0;

	int m_nSword{ 0 };
	int m_nSteff{ 0 };
	int m_nBow{ 0 };

	CSkinnedMesh* m_pStick{ NULL };
	CSkinnedMesh** m_pSword{ NULL };
	CSkinnedMesh** m_pSteff{ NULL };
	CSkinnedMesh** m_pBow{ NULL };
	int m_nWeaponState{ 0 };

#if USE_INSTANCING
	CB_GAMEOBJECT_INFO *m_pMappedObjects{ NULL };
	CMaterial						*m_pMaterial{ NULL };
#else
	UINT8 *m_pMappedObjects{ NULL };
#endif
	UINT8 *m_pMappedBoundingBoxes{ NULL };

	CHeightMapTerrain * m_pTerrain{ NULL };

	Network* m_pNetwork;

	int m_FrameCheck = 0;
};
