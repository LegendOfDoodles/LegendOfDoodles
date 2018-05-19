#pragma once
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"

class CMaterial;
class CHeightMapTerrain;
class CUIObjectManager;

class CAniShader : public CShader
{
public: // 생성자, 소멸자
	CAniShader(CCreateMgr *pCreateMgr, Network* pNetwork);
	virtual ~CAniShader();

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

	void SetGaugeManager(CUIObjectManager *pManger) { m_pGaugeManger = pManger; }

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr, UINT nRenderTargets = 1);
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers = 1);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // 변수
	CBaseObject * *m_ppBlues{ NULL };
	int m_nBlues{ 0 };

	CBaseObject * *m_ppReds{ NULL };
	int m_nReds{ 0 };

	ObjectType m_kind{ ObjectType::SwordMinion };

	CSkinnedMesh* m_pWeapons[3];
	int m_nWeaponState{ 0 };

#if USE_INSTANCING
	CB_GAMEOBJECT_INFO *m_pMappedObjects{ NULL };
	CMaterial						*m_pMaterial{ NULL };
#else
	UINT8 *m_pMappedObjects{ NULL };
#endif
	UINT8 *m_pMappedBoundingBoxes{ NULL };
	
	CUIObjectManager *m_pGaugeManger{ NULL };
	CHeightMapTerrain * m_pTerrain{ NULL };

	CCreateMgr* m_pCreateMgr{ NULL };

	Network* m_pNetwork{ NULL };

	bool m_buildFinished{ false };

	float m_spawnTime{ 10.1f };
	float m_preSpawnTime{ 0.0f };
};
