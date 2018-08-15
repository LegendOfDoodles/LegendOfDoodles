#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CMaterial;
class CPlayer;


class CHeightMapTerrain;
class CCollisionManager;
class CSkeleton;

class CEquipShader : public CShader
{
public: // 생성자, 소멸자
	CEquipShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CEquipShader();

public: // 공개 함수
	virtual void UpdateShaderVariables(int opt = 0);
	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);
	virtual void RenderShadow(CCamera *pCamera);

	virtual void SetPlayer(CCollisionObject **pPlayer) { m_pPlayer = (CPlayer**)pPlayer; };
	virtual void SetPlayerCnt(int cnt) { m_nPlayer = cnt; };


protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	D3D12_SHADER_BYTECODE CreateEmissivePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual D3D12_SHADER_BYTECODE CreateShadowVertexShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);
	virtual void ReleaseObjects();

	void SetEquipMesh(int playerindex, int equiptype, int equipindex,int weaponnum=0);


protected: // 변수

	UINT incrementSize;

	UINT m_nSword{ 0 };
	UINT m_nStaff{ 0 };
	UINT m_nBow{ 0 };
	UINT m_nArmor{ 0 };
	UINT m_nSpecial{ 0 };

	CSkinnedMesh* m_pStick{ NULL };
	CSkinnedMesh** m_pSword{ NULL };
	CSkinnedMesh** m_pStaff{ NULL };
	CSkinnedMesh** m_pBow{ NULL };
	CSkinnedMesh** m_pArmor{ NULL };
	CSkinnedMesh** m_pSpecial{ NULL };

	int* m_iEmissiveIndex;
	int* m_nMeshIndex;

	int m_nMeshCnt{ 0 };

	CPlayer** m_pPlayer;

	UINT m_nMaxEquip{ 5 };
	int m_nPlayer{4};

	UINT m_nPlayerWeaponType[4];
	int m_nPlayerWeaponNum[4];
	int m_nPlayerEquipments[4];
	int m_arrEquipIndex[4][5];
};