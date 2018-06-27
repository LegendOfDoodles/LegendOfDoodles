#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "05.Objects/96.Billboard/Billboard.h"

typedef std::vector<CCollisionObject*> CollisionObjectList;
typedef std::vector<CHPGaugeObjects*> HPGaugeObjectList;

class CMaterial;
class CUIObjectManager;

class CMinionHPGaugeShader : public CShader
{
public: // 생성자, 소멸자
	CMinionHPGaugeShader(CCreateMgr *pCreateMgr);
	virtual ~CMinionHPGaugeShader();

public: // 공개 함수
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

	virtual void GetCamera(CCamera *pCamera);

	void SetGaugeManager(CUIObjectManager * pManger);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);
	virtual bool OnProcessMouseInput(WPARAM pKeyBuffer);

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false);
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers = 1);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

	void SpawnGauge();

protected: // 변수
	CUIObjectManager * m_pGaugeManger{ NULL };

	CCamera *m_pCamera;

	CollisionObjectList *m_MinionObjectList;
	HPGaugeObjectList m_HPGaugeObjectList;

	int *m_nBlues{ NULL };
	int *m_nReds{ NULL };

	UINT8 *m_pMappedObjects{ NULL };

	CCreateMgr* m_pCreateMgr{ NULL };
};

