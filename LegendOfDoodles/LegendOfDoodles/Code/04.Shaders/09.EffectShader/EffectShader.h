#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

typedef std::list<CCollisionObject*> CollisionObjectList;

class CMaterial;

struct EffectObjectIndices
{
	int m_begIndex;
	int m_endIndex;
};

class CEffectShader : public CShader
{
public: // 생성자, 소멸자
	CEffectShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CEffectShader();

public: // 공개 함수
	virtual void ReleaseUploadBuffers();
	virtual void UpdateShaderVariables(int opt = 0);

	virtual void AnimateObjects(float timeElapsed);
	virtual void Render(CCamera *pCamera);

	void SpawnEffectObject(const XMFLOAT3& position, const XMFLOAT3& direction, int aniLength, EffectObjectType objectType);

	void Pause() { m_Paused = true; }
	void Resume() { m_Paused = false; };

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);
	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);
	virtual void ReleaseObjects();

	int GetPossibleIndex(EffectObjectType type);
	void ResetPossibleIndex(int idx) { m_objectsPossibleIndices[idx] = false; }

protected: // 변수
	static const int m_nMesh{ 8 };
	CTexturedRectMesh* m_ppMesh[m_nMesh];
	int UseMatrialNumToObjectCnt[m_nMesh];

	UINT m_srvIncrementSize{ 0 };

	std::unordered_map<EffectObjectType, UINT> m_objectsMaxCount;
	std::unordered_map<EffectObjectType, EffectObjectIndices> m_objectsIndices;
	std::unique_ptr<bool[]> m_objectsPossibleIndices;

	CollisionObjectList m_PlayerSwordSkill_Q_EffectList;
	CollisionObjectList m_PlayerSwordSkill_W_EffectList;
	CollisionObjectList m_PlayerSwordSkill_E_EffectList;
	CollisionObjectList m_PlayerSwordSkill_R_EffectList;

	CollisionObjectList m_PlayerStaffSkill_W_EffectList;
	CollisionObjectList m_PlayerStaffSkill_E_EffectList;

	CollisionObjectList m_PlayerArrowAttack_EffectList;
	CollisionObjectList m_PlayerStaffAttack_EffectList;
	CollisionObjectList m_PlayerStaffQSkill_EffectList;
	CollisionObjectList m_PlayerStaffESkill_EffectList;

	CollisionObjectList m_MinionArrowAttack_EffectList;
	CollisionObjectList m_MinionStaffAttack_EffectList;

	CollisionObjectList m_MinionArrow_EffectList;
	CollisionObjectList m_PlayerArrow_EffectList;
	CollisionObjectList m_PlayerFireBall_EffectList;
	CollisionObjectList m_TowerAttack_EffectList;

	CollisionObjectList m_PlayerArrowAndFireBall_HitPosition_EffectList;
	CollisionObjectList m_NormalHit_Effect;

	CollisionObjectList m_GolemStandardAttack_EffectList;
	CollisionObjectList m_GolemStumpAttack_EffectList;
	CollisionObjectList m_GolemSpecialAttack_EffectList;

	CollisionObjectList m_CircleLevelUp_EffectList;
	CollisionObjectList m_ArrowLevelUp_EffectList;

	CollisionObjectList m_Kill_EffectList;
	CollisionObjectList m_Death_EffectList;
	CollisionObjectList m_Miss_EffectList;
	CollisionObjectList m_Recovery_EffectList;
	CollisionObjectList m_Absorption_EffectList;

	bool m_Paused{ false };

	CCamera *m_pCamera;
};