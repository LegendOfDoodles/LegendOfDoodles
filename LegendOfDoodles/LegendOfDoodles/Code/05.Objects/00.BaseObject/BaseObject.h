#pragma once
#include "06.Meshes/01.Mesh/Mesh.h"
#include "00.Global/01.Utility/Enumerations.h"

class CCreateMgr;
class CShader;
class CCamera;
class CMaterial;

struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
};

class CBaseObject
{
public:	// 생성자, 소멸자
	CBaseObject(CCreateMgr *pCreateMgr, int nMeshes = 1);
	virtual ~CBaseObject();

public: // 공개 함수
	virtual void Initialize(CCreateMgr *pCreateMgr);
	virtual void Finalize();

	void ReleaseUploadBuffers();

	void SetMesh(int nIndex, CMesh *pMesh);
	void SetBoundingMesh(CCreateMgr *pCreateMgr, float width, float height, float depth, float xOffset = 0.0f, float yOffSet = 0.0f, float zOffSet = 0.0f);
	void SetShader(CShader *pShader);
	void SetMaterial(CMaterial *pMaterial);

	virtual void Animate(float timeElapsed);

	virtual void Render(CCamera *pCamera, UINT istanceCnt = 1);
	virtual void RenderBoundingBox(CCamera *pCamera, UINT istanceCnt = 1);

	void GenerateRayForPicking(XMFLOAT3& pickPosition, XMFLOAT4X4& xmf4x4View,
		XMFLOAT3 &pickRayOrigin, XMFLOAT3 &pickRayDirection);
	bool PickObjectByRayIntersection(XMFLOAT3& pickPosition, XMFLOAT4X4& xmf4x4View, float &hitDistance);

	void MoveStrafe(float fDistance = 1.0f);
	virtual void MoveUp(float fDistance = 1.0f);
	virtual void MoveForward(float fDistance = 1.0f);

	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

	void Translate(XMFLOAT3 *pxmf3Axis);
	void Translate(float x,float y, float z);

	void Scale(float x = 1.0f, float y = 1.0f, float z = 1.0f);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	virtual void SetPosition(float x, float z);
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetLook(XMFLOAT3 vLook);
	void SetUp(XMFLOAT3 vUp);
	void SetRight(XMFLOAT3 vRight);

	bool IsVisible(CCamera *pCamera = NULL);

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	XMFLOAT4X4* GetWorldMatrix() { return &m_xmf4x4World; }
	XMFLOAT4X4* GetFrameMatrix() { return m_xmf4x4Frame; }

	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE cbvGPUDescriptorHandle) { m_cbvGPUDescriptorHandle = cbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 cbvGPUDescriptorHandlePtr) { m_cbvGPUDescriptorHandle.ptr = cbvGPUDescriptorHandlePtr; }

	void SetCbvGPUDescriptorHandleForBB(D3D12_GPU_DESCRIPTOR_HANDLE cbvGPUDescriptorHandle) { m_cbvGPUDescriptorHandleForBB = cbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtrForBB(UINT64 cbvGPUDescriptorHandlePtr) { m_cbvGPUDescriptorHandleForBB.ptr = cbvGPUDescriptorHandlePtr; }

	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_cbvGPUDescriptorHandle); }

	void SaveIndex(int idx) { m_index = idx; }
	int GetIndex() { return m_index; }

	int GetType() {	return m_ObjectType; };
	void SetType(ObjectType newObjectType) {m_ObjectType = newObjectType;};

protected: // 내부 함수
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables();

	virtual void OnPrepareRender();

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

	ID3D12Resource					*m_pcbGameObject{ NULL };
	UINT8				*m_pMappedObject{ NULL };

	ObjectType m_ObjectType{ ObjectType::StickPlayer };

	ID3D12GraphicsCommandList *m_pCommandList{ NULL };
};
