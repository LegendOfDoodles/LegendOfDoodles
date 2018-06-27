#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "05.Objects/96.Billboard/Billboard.h"

typedef std::vector<CCollisionObject*> CollisionObjectList;
typedef std::vector<CMinimapIconObjects*> MinionIconObjectList;

class CMaterial;
class CPlayer;
class CUIObjectManager;

class CMinimapIconShader : public CShader
{
public:
	CMinimapIconShader(CCreateMgr *pCreateMgr);
	virtual ~CMinimapIconShader();

public: // 공개 함수
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

	virtual void GetCamera(CCamera *pCamera);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);
	virtual bool OnProcessMouseInput(WPARAM pKeyBuffer);

	void SetUIObjectsManager(CUIObjectManager * pManger);

	virtual void SetPlayer(CBaseObject **pPlayer) { m_pPlayer = (CCollisionObject**)pPlayer; };
	virtual void SetPlayerCnt(int cnt) { m_nPlayer = cnt; };

	virtual void SetNexusAndTower(CBaseObject **ppObjects) { m_ppNexusAndTower = (CCollisionObject**)ppObjects; };
	virtual void SetNexusAndTowerCnt(int cnt) { m_nNexusAndTower = cnt; };

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false);
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers = 1);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

	void SpawnMinionIcon();

protected: // 변수
	// Players Icon & Static Objects
	CBaseObject **m_ppObjects{ NULL };
	int m_nObjects = 0;

	// Materials
	CMaterial	**m_ppMaterials{ NULL };
	int m_nMaterials = 0;

	// 카메라
	CCamera *m_pCamera;

	// Players
	CCollisionObject **m_pPlayer;
	int m_nPlayer = 0;

	// Tower And Nexus
	CCollisionObject **m_ppNexusAndTower{ NULL };
	int m_nNexusAndTower = 0;

	//동적생성
	CUIObjectManager *m_pIconManger{ NULL };

	CollisionObjectList *m_MinionObjectList;
	MinionIconObjectList m_MinionIconObjectList;

	int *m_nBlues{ NULL };
	int *m_nReds{ NULL };

	UINT8 *m_pMappedObjects{ NULL };

	CCreateMgr* m_pCreateMgr{ NULL };
};