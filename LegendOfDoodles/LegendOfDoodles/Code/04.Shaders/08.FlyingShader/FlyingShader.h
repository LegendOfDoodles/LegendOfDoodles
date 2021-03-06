#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

typedef std::list<CCollisionObject*> CollisionObjectList;

class CMaterial;
class CCollisionManager;
class CHeightMapTerrain;

struct FlyingObjectIndices
{
	int m_begIndex;
	int m_endIndex;
};

class CFlyingShader : public CShader
{
public: // 생성자, 소멸자
	CFlyingShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CFlyingShader();

public: // 공개 함수
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables(int opt = 0);

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

	void SpawnFlyingObject(const XMFLOAT3& position, const XMFLOAT3& direction, TeamType teamType, FlyingObjectType objectType, float damage, float rangeAdj = 1.f);

	void SetColManagerToObject(shared_ptr<CCollisionManager> manager);
	void SetEffectManagerToObject(shared_ptr<CEffectMgr> manager);
	void SetSoundManagerToObject(shared_ptr<CSoundManager> manager);

	void Pause() { m_Paused = true; }
	void Resume() { m_Paused = false; };

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	D3D12_SHADER_BYTECODE CreateNonEmissivePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

	int GetPossibleIndex(FlyingObjectType type);
	void ResetPossibleIndex(int idx) { m_objectsPossibleIndices[idx] = false; }

protected: // 변수
	static const int m_nMesh{ 8 };
	CStaticMesh * m_pMeshes[m_nMesh];

	UINT m_srvIncrementSize{ 0 };

	std::unordered_map<FlyingObjectType, UINT> m_objectsMaxCount;
	std::unordered_map<FlyingObjectType, FlyingObjectIndices> m_objectsIndices;
	std::unique_ptr<bool[]> m_objectsPossibleIndices;

	CollisionObjectList m_dumbelList;
	CollisionObjectList m_arrowList;
	CollisionObjectList m_magicList;
	CollisionObjectList m_blueTowerAtkList;
	CollisionObjectList m_redTowerAtkList;
	CollisionObjectList m_playerArrowList;
	CollisionObjectList m_playerMagicSkillQList;
	CollisionObjectList m_playerArrowSkillWList;
	CollisionObjectList m_playerMagicSkillRList;
	CollisionObjectList m_playerMagicList;

	CHeightMapTerrain * m_pTerrain{ NULL };

	bool m_Paused{ false };
};