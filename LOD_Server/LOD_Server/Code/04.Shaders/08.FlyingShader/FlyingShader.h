#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

typedef std::list<CCollisionObject*> CollisionObjectList;

class CMaterial;
class CCollisionManager;

struct FlyingObjectIndices
{
	int m_begIndex;
	int m_endIndex;
};

class CFlyingShader : public CShader
{
public: // ������, �Ҹ���
	CFlyingShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CFlyingShader();

public: // ���� �Լ�
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables(int opt = 0);

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

	void SpawnFlyingObject(const XMFLOAT3& position, const XMFLOAT3& direction, TeamType teamType, FlyingObjectType objectType);

	void SetColManagerToObject(shared_ptr<CCollisionManager> manager);

	void Pause() { m_Paused = true; }
	void Resume() { m_Paused = false; };

protected: // ���� �Լ�
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	D3D12_SHADER_BYTECODE CreateNonEmissivePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

	int GetPossibleIndex(FlyingObjectType type);
	void ResetPossibleIndex(int idx) { m_objectsPossibleIndices[idx] = false; }

protected: // ����
	static const int m_nMesh{ 3 };
	CStaticMesh * m_pMeshes[m_nMesh];

	UINT m_srvIncrementSize{ 0 };

	std::unordered_map<FlyingObjectType, UINT> m_objectsMaxCount;
	std::unordered_map<FlyingObjectType, FlyingObjectIndices> m_objectsIndices;
	std::unique_ptr<bool[]> m_objectsPossibleIndices;

	CollisionObjectList m_dumbelList;
	CollisionObjectList m_arrowList;
	CollisionObjectList m_magicList;

	bool m_Paused{ false };
};