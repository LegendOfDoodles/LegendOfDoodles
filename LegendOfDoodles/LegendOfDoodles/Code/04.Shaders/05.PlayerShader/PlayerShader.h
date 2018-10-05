#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CMaterial;
class CHeightMapTerrain;
class CCollisionManager;
class CThrowingMgr;
class CSkeleton;

class CPlayerShader : public CShader
{
public:
	CPlayerShader(shared_ptr<CCreateMgr> pCreateMgr);
	~CPlayerShader();

public: // 공개 함수
	virtual void Initialize(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);

	virtual void UpdateShaderVariables(int opt = 0);
	virtual void UpdateBoundingBoxShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);
	virtual void RenderBoundingBox(CCamera *pCamera);
	virtual void RenderShadow(CCamera *pCamera);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);

	void SetColManagerToObject(shared_ptr<CCollisionManager> manager);
	void SetThrowingManagerToObject(shared_ptr<CThrowingMgr> manager);
	void SetEffectManagerToObject(shared_ptr<CEffectMgr> manager);
	void SetSoundManagerToObject(shared_ptr<CSoundManager> manager);

	virtual void SetChangeWeapon(int id);

	bool* GetChangeWeapon() { return &m_ChangeWeapon; }

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual D3D12_SHADER_BYTECODE CreateShadowVertexShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);
	virtual void ReleaseObjects();

protected: // 변수

		   //애니메이션 정보
	CSkeleton * * m_ppSwordAni{ NULL };
	CSkeleton** m_ppStaffAni{ NULL };

	CSkeleton** m_ppBowAni{ NULL };

	int m_nWeaponState{ 0 };

	CHeightMapTerrain * m_pTerrain{ NULL };

	bool m_ChangeWeapon{ false };
	float m_fRecvTime{ 0.f };
};
