#pragma once
#include "06.Meshes/01.Mesh/Mesh.h"

class CCreateMgr;
class CShader;
class CCamera;
class CMaterial;
class CCollisionObject;

struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
};

struct CB_EFFECTOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
	float m_fAnimationTime;
};

class CBaseObject
{
public:	// 생성자, 소멸자
	CBaseObject(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes = 1);
	virtual ~CBaseObject();

public: // 공개 함수
	virtual void Initialize(shared_ptr<CCreateMgr> pCreateMgr);
	virtual void Finalize();

	void ReleaseUploadBuffers();

	void SetMesh(int nIndex, CMesh *pMesh);
	void SetBoundingMesh(CMesh *pMesh);
	void SetShader(CShader *pShader);
	void SetMaterial(CMaterial *pMaterial);

	virtual void Animate(float timeElapsed = 0.0f);

	virtual void Render(CCamera *pCamera, UINT istanceCnt = 1);
	virtual void RenderBoundingBox(CCamera *pCamera, UINT istanceCnt = 1);

	void MoveStrafe(float fDistance = 1.0f);
	virtual void MoveUp(float fDistance = 1.0f);
	virtual void MoveForward(float fDistance = 1.0f);

	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

	void Translate(XMFLOAT3 *pxmf3Axis);
	void Translate(float x, float y, float z);

	void Scale(float x = 1.0f, float y = 1.0f, float z = 1.0f);

	XMFLOAT3 GetPosition();
	virtual XMFLOAT3 GetLook();
	virtual XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	virtual void SetPosition(float x, float z);
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);

	bool IsInRect(float width, float height, POINT cursorPos);

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void ResetWorldMatrix() { XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity()); }

	XMFLOAT4X4* GetWorldMatrix() { return &m_xmf4x4World; }
	XMFLOAT4X4* GetFrameMatrix() { return m_xmf4x4Frame; }

	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE cbvGPUDescriptorHandle) { m_cbvGPUDescriptorHandle = cbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 cbvGPUDescriptorHandlePtr) { m_cbvGPUDescriptorHandle.ptr = cbvGPUDescriptorHandlePtr; }

	void SetCbvGPUDescriptorHandleForBB(D3D12_GPU_DESCRIPTOR_HANDLE cbvGPUDescriptorHandle) { m_cbvGPUDescriptorHandleForBB = cbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtrForBB(UINT64 cbvGPUDescriptorHandlePtr) { m_cbvGPUDescriptorHandleForBB.ptr = cbvGPUDescriptorHandlePtr; }

	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_cbvGPUDescriptorHandle); }

	void SaveIndex(int idx) { m_index = idx; }
	int GetIndex() { return m_index; }

	int GetType() { return m_ObjectType; };
	void SetType(ObjectType newObjectType) { m_ObjectType = newObjectType; };

	virtual TeamType GetTeam() { return m_TeamType; }
	virtual void SetTeam(TeamType type) { m_TeamType = type; }
	virtual CCollisionObject* GetMasterObject() { return NULL; }

	virtual void SetState(StatesType newState) { newState; }
	virtual void ActiveSkill(AnimationsType act) { act; }

protected: // 내부 함수
	virtual void OnPrepareRender();

	bool IsVisible(CCamera *pCamera = NULL);

protected: // 변수
	int m_nReferences{ 0 };

	int m_index{ 0 };

	XMFLOAT4X4 m_xmf4x4World;
	XMFLOAT4X4 m_xmf4x4Frame[128];

	CMesh	**m_ppMeshes{ NULL };
	int m_nMeshes{ 0 };

	CMesh *m_pBoundingMesh{ NULL };

	CShader *m_pShader{ NULL };
	CMaterial	 *m_pMaterial{ NULL };

	D3D12_GPU_DESCRIPTOR_HANDLE m_cbvGPUDescriptorHandle{ NULL };
	D3D12_GPU_DESCRIPTOR_HANDLE m_cbvGPUDescriptorHandleForBB{ NULL };

	ObjectType m_ObjectType{ ObjectType::StickPlayer };
	TeamType m_TeamType{ None };

	ComPtr<ID3D12GraphicsCommandList> m_pCommandList;
};
