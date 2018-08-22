#include "stdafx.h"
#include "Camera.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"

/// <summary>
/// 목적: 기본 카메라 코드, 인터 페이스 용
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-08-05
/// </summary>


////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CCamera::CCamera()
{
	m_xmf4x4View = Matrix4x4::Identity();
	m_xmf4x4Projection = Matrix4x4::Identity();
	m_viewport = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
	m_scissorRect = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT };
}

CCamera::~CCamera()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CCamera::Initialize(shared_ptr<CCreateMgr> pCreateMgr)
{
	int width = pCreateMgr->GetWindowWidth();
	int height = pCreateMgr->GetWindowHeight();

	m_hWnd = pCreateMgr->GetHwnd();
	m_pCommandList = pCreateMgr->GetCommandList();

	SetViewport(0, 0, width, height, 0.0f, 1.0f);
	SetScissorRect(0, 0, width, height);
	GeneratePerspectiveProjectionMatrix(1.0f, 50000.0f, float(width) / float(height), 90.0f);
	GenerateViewMatrix(
		XMFLOAT3(0.0f, 70.0f, 0.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(0.0f, 1.0f, 0.0f));

	CreateShaderVariables(pCreateMgr, ((sizeof(VS_CB_CAMERA_INFO) + 255) & ~255));
}

void CCamera::Finalize()
{
	ReleaseShaderVariables();
}

void CCamera::UpdateShaderVariables(int rootSignatureIndex)
{
	VS_CB_CAMERA_INFO *pMappedCamera = (VS_CB_CAMERA_INFO *)(m_pMappedCamera);

	XMStoreFloat4x4(&pMappedCamera->m_xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));
	XMStoreFloat4x4(&pMappedCamera->m_xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Projection)));

	memcpy(&pMappedCamera->m_xmf3Position, &m_xmf3Position, sizeof(XMFLOAT3));

	D3D12_GPU_VIRTUAL_ADDRESS gpuVirtualAddress = m_pConstBuffer->GetGPUVirtualAddress();
	m_pCommandList->SetGraphicsRootConstantBufferView(rootSignatureIndex, gpuVirtualAddress);
}

void CCamera::SetViewportsAndScissorRects()
{
	m_pCommandList->RSSetViewports(1, &m_viewport);
	m_pCommandList->RSSetScissorRects(1, &m_scissorRect);
}

void CCamera::Move(float fTimeElapsed, bool bVelocity)
{
	UNREFERENCED_PARAMETER(bVelocity);

	if (m_direction)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		float distance = m_speed * fTimeElapsed;

		if (m_direction & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, distance);
		else if (m_direction & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -distance);

		if (m_direction & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, distance);
		else if (m_direction & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -distance);

		if (m_direction & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, distance);
		else if (m_direction & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -distance);

		m_direction = NULL;

		Move(xmf3Shift);
	}
}

void CCamera::Move(XMFLOAT3& xmf3Shift)
{
	m_xmf3Position.x += xmf3Shift.x;
	m_xmf3Position.y += xmf3Shift.y;
	m_xmf3Position.z += xmf3Shift.z;
}

void CCamera::Rotate(float x, float y, float z)
{
	if (x != 0.0f)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right),
			XMConvertToRadians(x));
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
	}
	if (y != 0.0f)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up),
			XMConvertToRadians(y));
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
	}
	if (z != 0.0f)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look),
			XMConvertToRadians(z));
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
	}
}

void CCamera::Update(float fTimeElapsed)
{
	if (m_direction) { Move(fTimeElapsed, true); }
	if (!Vector3::IsZero(m_rotation))
	{
		Rotate(m_rotation.x, m_rotation.y, m_rotation.z);
		m_rotation.x = m_rotation.y = m_rotation.z = 0.0f;
	}

	RegenerateViewMatrix();
}

void CCamera::SetLookAt(XMFLOAT3& xmf3LookAt)
{
	UNREFERENCED_PARAMETER(xmf3LookAt);
}

void CCamera::SetOffset(XMFLOAT3 xmf3Offset) {
	m_xmf3Offset = xmf3Offset;

	m_xmf3Position.x += xmf3Offset.x;
	m_xmf3Position.y += xmf3Offset.y;
	m_xmf3Position.z += xmf3Offset.z;
}

void CCamera::RegenerateViewMatrix()
{
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);

	m_xmf4x4View._11 = m_xmf3Right.x; m_xmf4x4View._12 = m_xmf3Up.x; m_xmf4x4View._13 = m_xmf3Look.x;
	m_xmf4x4View._21 = m_xmf3Right.y; m_xmf4x4View._22 = m_xmf3Up.y; m_xmf4x4View._23 = m_xmf3Look.y;
	m_xmf4x4View._31 = m_xmf3Right.z; m_xmf4x4View._32 = m_xmf3Up.z; m_xmf4x4View._33 = m_xmf3Look.z;

	m_xmf4x4View._41 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Right);
	m_xmf4x4View._42 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Up);
	m_xmf4x4View._43 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Look);

	GenerateFrustum();
}

void CCamera::GeneratePerspectiveProjectionMatrix(
	float fNearPlaneDistance, float fFarPlaneDistance,
	float fAspectRatio, float fFOVAngle)
{
	m_xmf4x4Projection = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(fFOVAngle),
		fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
}

void CCamera::GenerateOrthographicProjectionMatrix(
	float fWidth, float fHeight,
	float fNearPlaneDistance, float fFarPlaneDistance)
{
	m_xmf4x4Projection = Matrix4x4::OrthographicLH(fWidth, fHeight, fNearPlaneDistance, fFarPlaneDistance);
}

void CCamera::GenerateOrthographicOffCenterProjectionMatrix(
	float left, float right,
	float top, float bottom,
	float fNearPlaneDistance, float fFarPlaneDistance)
{
	m_xmf4x4Projection = Matrix4x4::OrthographicOffCenterLH(left, right, top, bottom, fNearPlaneDistance, fFarPlaneDistance);
}

void CCamera::SavePickedPos()
{
	::GetCursorPos(&m_pickCursorPos);
}

void CCamera::SetMaster(CCollisionObject * master)
{
	m_pMaster = master;

	XMFLOAT3 masterPos{ master->GetPosition() };

	SetPosition(masterPos.x, masterPos.z);
}

bool CCamera::OnProcessMouseWheel(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	m_speed += GET_WHEEL_DELTA_WPARAM(wParam);

	if (m_speed < MIN_CAMERA_SPEED) m_speed = MIN_CAMERA_SPEED;
	else if (m_speed > MAX_CAMERA_SPEED) m_speed = MAX_CAMERA_SPEED;

	return true;
}

bool CCamera::OnProcessMouseInput(UCHAR * pKeyBuffer)
{
	POINT cursorPos;
	float cxDelta{ 0.0f }, cyDelta{ 0.0f };

	if (::GetCapture() == m_hWnd)
	{
		::GetCursorPos(&cursorPos);
		cxDelta = (float)(cursorPos.x - m_pickCursorPos.x) / 3.0f;
		cyDelta = (float)(cursorPos.y - m_pickCursorPos.y) / 3.0f;
		::SetCursorPos(m_pickCursorPos.x, m_pickCursorPos.y);
	}
	if ((cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				SetRotation(cyDelta, 0.0f, -cxDelta);
			else
				SetRotation(cyDelta, cxDelta, 0.0f);
		}
	}
	return true;
}

bool CCamera::OnProcessKeyInput(UCHAR * pKeyBuffer)
{
	DWORD direction{ NULL };

	if (pKeyBuffer['W'] & 0xF0)
	{
		direction |= DIR_FORWARD;
	}
	if (pKeyBuffer['S'] & 0xF0)
	{
		direction |= DIR_BACKWARD;
	}
	if (pKeyBuffer['A'] & 0xF0)
	{
		direction |= DIR_LEFT;
	}
	if (pKeyBuffer['D'] & 0xF0)
	{
		direction |= DIR_RIGHT;
	}
	if (pKeyBuffer['Q'] & 0xF0)
	{
		direction |= DIR_UP;
	}
	if (pKeyBuffer['E'] & 0xF0)
	{
		direction |= DIR_DOWN;
	}

	m_direction = direction;

	return false;
}

void CCamera::GenerateFrustum()
{
	m_xmFrustum.CreateFromMatrix(m_xmFrustum, XMLoadFloat4x4(&m_xmf4x4Projection));
	XMMATRIX xmmtxInversView = XMMatrixInverse(NULL, XMLoadFloat4x4(&m_xmf4x4View));
	m_xmFrustum.Transform(m_xmFrustum, xmmtxInversView);
}

bool CCamera::IsInFrustum(BoundingOrientedBox & xmBoundingBox)
{
	return(m_xmFrustum.Intersects(xmBoundingBox));
}

void CCamera::GoToMaster()
{
	XMFLOAT3 masterPos{ m_pMaster->GetPosition() };

	SetPosition(masterPos.x, masterPos.z - m_xmf3Position.y * 0.5f);
}

void CCamera::SetPosition(float x, float y, float z)
{
	m_xmf3Position.x = x;
	m_xmf3Position.y = y;
	m_xmf3Position.z = z;
}

void CCamera::SetPosition(float x, float z)
{
	m_xmf3Position.x = x;
	m_xmf3Position.z = z;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CCamera::GenerateViewMatrix()
{
	m_xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, m_xmf3LookAtWorld, m_xmf3Up);
}

void CCamera::GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up)
{
	m_xmf3Position = xmf3Position;
	m_xmf3LookAtWorld = xmf3LookAt;

	if (Vector3::IsZero(xmf3Up))
	{
		m_xmf3Look = Vector3::Normalize(Vector3::Subtract(xmf3LookAt, xmf3Position));
		m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
	}
	else
	{
		m_xmf3Up = xmf3Up;
	}

	GenerateViewMatrix();
}

void CCamera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
	m_viewport.TopLeftX = float(xTopLeft);
	m_viewport.TopLeftY = float(yTopLeft);
	m_viewport.Width = float(nWidth);
	m_viewport.Height = float(nHeight);
	m_viewport.MinDepth = fMinZ;
	m_viewport.MaxDepth = fMaxZ;
}

void CCamera::SetScissorRect(
	LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	m_scissorRect.left = xLeft;
	m_scissorRect.top = yTop;
	m_scissorRect.right = xRight;
	m_scissorRect.bottom = yBottom;
}

void CCamera::CreateShaderVariables(shared_ptr<CCreateMgr> pCreateMgr, UINT stride)
{
	m_pConstBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		stride,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	HRESULT hResult = m_pConstBuffer->Map(0, NULL, (void **)&m_pMappedCamera);
	ThrowIfFailed(hResult);
}

void CCamera::ReleaseShaderVariables()
{
	if (m_pConstBuffer.Get())
	{
		m_pConstBuffer->Unmap(0, NULL);
		m_pConstBuffer.Reset();
	}
}