#include "stdafx.h"
#include "BaseObject.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "04.Shaders/00.BaseShader/Shader.h"
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"
#include "05.Objects/99.Material/Material.h"

/// <summary>
/// 목적: 기본 오브젝트 클래스, 인터페이스 용
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-05-11
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CBaseObject::CBaseObject(CCreateMgr *pCreateMgr, int nMeshes)
{
	m_pCommandList = pCreateMgr->GetCommandList();

	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());

	m_nMeshes = nMeshes;
	if (m_nMeshes > 0)
	{
		m_ppMeshes = new CMesh*[m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;
	}
}

CBaseObject::~CBaseObject()
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) Safe_Release(m_ppMeshes[i]);
		}
		Safe_Delete_Array(m_ppMeshes);
	}
	if (m_pBoundingMesh) Safe_Release(m_pBoundingMesh);

	if (m_pShader){ m_pShader->Finalize(); }
	if (m_pMaterial) m_pMaterial->Finalize();
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CBaseObject::Initialize(CCreateMgr *pCreateMgr)
{
	CreateShaderVariables(pCreateMgr);
}

void CBaseObject::Finalize()
{
	ReleaseShaderVariables();
}

void CBaseObject::ReleaseUploadBuffers()
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->ReleaseUploadBuffers();
		}
	}

	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
}

void CBaseObject::SetMesh(int nIndex, CMesh *pMesh)
{

	if (!m_ppMeshes) return;
	if (nIndex >= m_nMeshes) return;


	if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex]->Release();
	m_ppMeshes[nIndex] = pMesh;

	if (pMesh) pMesh->AddRef();

}

void CBaseObject::SetBoundingMesh(CCreateMgr *pCreateMgr, float width, float height, float depth, float xOffset, float yOffSet, float zOffSet)
{
	m_pBoundingMesh = new CCubeMesh(pCreateMgr, width, height / 2.f, depth, xOffset, yOffSet, zOffSet);
}

void CBaseObject::SetShader(CShader *pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
	//if (!m_pMaterial)
	//{
	//	CMaterial *pMaterial = new CMaterial();
	//	SetMaterial(pMaterial);
	//}
	//if (m_pMaterial) m_pMaterial->SetShader(pShader);
}

void CBaseObject::SetMaterial(CMaterial *pMaterial)
{
	if (m_pMaterial) m_pMaterial->Release();
	m_pMaterial = pMaterial;
	if (m_pMaterial) m_pMaterial->AddRef();
}

void CBaseObject::Animate(float timeElapsed)
{
}

void CBaseObject::Render(CCamera *pCamera, UINT istanceCnt)
{
	OnPrepareRender();

	if (m_pMaterial)
	{
		m_pMaterial->Render(pCamera);
		m_pMaterial->UpdateShaderVariables();
	}

	if (m_cbvGPUDescriptorHandle.ptr)
		m_pCommandList->SetGraphicsRootDescriptorTable(2, m_cbvGPUDescriptorHandle);

	if (m_pShader)
	{
		UpdateShaderVariables();
		m_pShader->Render(pCamera);
	}

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(istanceCnt);
		}
	}
}

void CBaseObject::RenderBoundingBox(CCamera * pCamera, UINT istanceCnt)
{
	OnPrepareRender();

	if (m_cbvGPUDescriptorHandleForBB.ptr)
		m_pCommandList->SetGraphicsRootDescriptorTable(2, m_cbvGPUDescriptorHandleForBB);

	if (m_pBoundingMesh) m_pBoundingMesh->Render(istanceCnt);
}

void CBaseObject::GenerateRayForPicking(
	XMFLOAT3& pickPosition, XMFLOAT4X4&	 xmf4x4View, 
	XMFLOAT3 &pickRayOrigin, XMFLOAT3 &pickRayDirection)
{
	XMFLOAT4X4 xmf4x4WorldView{ Matrix4x4::Multiply(m_xmf4x4World, xmf4x4View) };
	XMFLOAT4X4 xmf4x4Inverse{ Matrix4x4::Inverse(xmf4x4WorldView) };
	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f);

	//카메라 좌표계의 원점을 모델 좌표계로 변환한다.
	pickRayOrigin = Vector3::TransformCoord(xmf3CameraOrigin, xmf4x4Inverse);
	//카메라 좌표계의 점(마우스 좌표를 역변환하여 구한 점)을 모델 좌표계로 변환한다.
	pickRayDirection = Vector3::TransformCoord(pickPosition, xmf4x4Inverse);
	//광선의 방향 벡터를 구한다.
	pickRayDirection = Vector3::Normalize(Vector3::Subtract(pickRayDirection, pickRayOrigin));
}

bool CBaseObject::PickObjectByRayIntersection(
	XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float &hitDistance)
{
	if (!m_ppMeshes) return false;
	if (!m_ppMeshes[0]) return false;
	
	bool intersected{ false };
	XMFLOAT3 pickRayOrigin, pickRayDirection;

	GenerateRayForPicking(xmf3PickPosition, xmf4x4View, pickRayOrigin, pickRayDirection);

	intersected = m_ppMeshes[0]->CheckRayIntersection(pickRayOrigin,	pickRayDirection, hitDistance);

	return(intersected);
}

void CBaseObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CBaseObject::SetPosition(xmf3Position);
}

void CBaseObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CBaseObject::SetPosition(xmf3Position);
}

void CBaseObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CBaseObject::SetPosition(xmf3Position);
}

void CBaseObject::Rotate(XMFLOAT3 *pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(
		XMLoadFloat3(pxmf3Axis), 
		XMConvertToRadians(fAngle));

	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CBaseObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate =XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(fPitch),
		XMConvertToRadians(fYaw),
		XMConvertToRadians(fRoll));

	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CBaseObject::Translate(XMFLOAT3 * pxmf3Axis)
{
	XMFLOAT3 xmf3Position = GetPosition();
	xmf3Position = Vector3::Add(xmf3Position, *pxmf3Axis);
	CBaseObject::SetPosition(xmf3Position);
}

void CBaseObject::Translate(float x, float y, float z)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Translate = XMFLOAT3(x,y,z);
	xmf3Position = Vector3::Add(xmf3Position, xmf3Translate);
	CBaseObject::SetPosition(xmf3Position);
}

void CBaseObject::Scale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);

	m_xmf4x4World = Matrix4x4::Multiply(mtxScale, m_xmf4x4World);
}

XMFLOAT3 CBaseObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 CBaseObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

XMFLOAT3 CBaseObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23)));
}

XMFLOAT3 CBaseObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3( m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13)));
}

void CBaseObject::SetPosition(float x, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._43 = z;
}

void CBaseObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void CBaseObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CBaseObject::CreateShaderVariables(CCreateMgr *pCreateMgr)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pcbGameObject = pCreateMgr->CreateBufferResource(
		NULL,
		ncbElementBytes, 
		D3D12_HEAP_TYPE_UPLOAD, 
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	m_pcbGameObject->Map(0, NULL, (void **)&m_pMappedObject);
}

void CBaseObject::ReleaseShaderVariables()
{
}

void CBaseObject::UpdateShaderVariables()
{
	CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedObject);

	XMStoreFloat4x4(&pMappedObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
}

void CBaseObject::OnPrepareRender()
{
}