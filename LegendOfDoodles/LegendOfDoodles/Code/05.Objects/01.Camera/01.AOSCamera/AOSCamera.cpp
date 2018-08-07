#include "stdafx.h"
#include "AOSCamera.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"

/// <summary>
/// 목적: In Game 에서 사용할 카메라
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-07-04
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CAOSCamera::CAOSCamera() : CCamera()
{
}

CAOSCamera::~CAOSCamera()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CAOSCamera::Initialize(shared_ptr<CCreateMgr> pCreateMgr)
{
	int width = pCreateMgr->GetWindowWidth();
	int height = pCreateMgr->GetWindowHeight();

	m_edgeSize.left = Window_Edge_Size;
	m_edgeSize.top = Window_Edge_Size;
	m_edgeSize.right = width - Window_Edge_Size;
	m_edgeSize.bottom = height - Window_Edge_Size;

	m_hWnd = pCreateMgr->GetHwnd();
	m_pCommandList = pCreateMgr->GetCommandList();

	SetViewport(0, 0, width, height, 0.0f, 1.0f);
	SetScissorRect(0, 0, width, height);
	GeneratePerspectiveProjectionMatrix(1.0f, 50000.0f, float(width) / float(height), 90.0f);
	GenerateViewMatrix(
		XMFLOAT3(0.0f, 500.0f, -200.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f));

	CreateShaderVariables(pCreateMgr, ((sizeof(VS_CB_CAMERA_INFO) + 255) & ~255));
}

void CAOSCamera::Move(float fTimeElapsed, bool bVelocity)
{
	UNREFERENCED_PARAMETER(bVelocity);

	if (m_direction)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);

		float upDown = m_speedVector.y * m_speed * fTimeElapsed;
		if (m_direction & DIR_UP | m_direction & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, XMFLOAT3(0, 0, 1), upDown);

		float leftRight = m_speedVector.x * m_speed * fTimeElapsed;
		if (m_direction & DIR_LEFT | m_direction & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, XMFLOAT3(1, 0, 0), leftRight);

		m_direction = NULL;

		CCamera::Move(xmf3Shift);
	}
}

bool CAOSCamera::OnProcessMouseWheel(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

	XMFLOAT3 pos{ GetPosition() };

	if (zDelta < 0)	// 휠 다운
	{
		if (pos.y < Zoom_Out_Max)
		{
			SetPosition(pos.x, pos.y + Zoom_Change_Value_Y, pos.z + Zoom_Change_Value_Z);
		}
	}
	else    // 휠 업
	{
		if (pos.y > Zoom_In_Max)
		{
			SetPosition(pos.x, pos.y - Zoom_Change_Value_Y, pos.z - Zoom_Change_Value_Z);
		}
	}

	return true;
}

bool CAOSCamera::OnProcessMouseInput(UCHAR * pKeyBuffer)
{
	UNREFERENCED_PARAMETER(pKeyBuffer);

	POINT cursorPos;

	GetCursorPos(&cursorPos);
	ScreenToClient(m_hWnd, &cursorPos);

	// 마우스를 이용한 이동
	if (cursorPos.x < m_edgeSize.left && m_xmf3Position.x > 0)
	{
		m_speedVector.x = (float)(cursorPos.x - m_edgeSize.left) / Window_Edge_Power;
		m_direction |= DIR_LEFT;
	}
	if (cursorPos.x > m_edgeSize.right && m_xmf3Position.x < TERRAIN_SIZE_WIDTH)
	{
		m_speedVector.x = (float)(cursorPos.x - m_edgeSize.right) / Window_Edge_Power;
		m_direction |= DIR_RIGHT;
	}
	if (cursorPos.y < m_edgeSize.top && m_xmf3Position.z < TERRAIN_SIZE_HEIGHT)
	{
		m_speedVector.y = (float)(m_edgeSize.top - cursorPos.y) / Window_Edge_Power;
		m_direction |= DIR_UP;
	}
	if (cursorPos.y > m_edgeSize.bottom && m_xmf3Position.z > 0)
	{
		m_speedVector.y = (float)(m_edgeSize.bottom - cursorPos.y) / Window_Edge_Power;
		m_direction |= DIR_DOWN;
	}

	return true;
}

bool CAOSCamera::OnProcessKeyInput(UCHAR * pKeyBuffer)
{
	// 카메라 속도 조절
	if (pKeyBuffer[VK_OEM_4] & 0xF0) // '['
	{
		if (m_speed > MIN_CAMERA_SPEED) m_speed -= 10.0f;
	}
	if (pKeyBuffer[VK_OEM_6] & 0xF0) // ']'
	{
		if (m_speed < MAX_CAMERA_SPEED) m_speed += 10.0f;
	}

	// 키보드를 이용한 이동
	if (pKeyBuffer[VK_LEFT] & 0xF0 && m_xmf3Position.x > 0)
	{
		m_speedVector.x = -1;
		m_direction |= DIR_LEFT;
	}
	if (pKeyBuffer[VK_RIGHT] & 0xF0 && m_xmf3Position.x < TERRAIN_SIZE_WIDTH)
	{
		m_speedVector.x = 1;
		m_direction |= DIR_RIGHT;
	}
	if (pKeyBuffer[VK_UP] & 0xF0 && m_xmf3Position.z < TERRAIN_SIZE_HEIGHT)
	{
		m_speedVector.y = 1;
		m_direction |= DIR_UP;
	}
	if (pKeyBuffer[VK_DOWN] & 0xF0 && m_xmf3Position.z > 0)
	{
		m_speedVector.y = -1;
		m_direction |= DIR_DOWN;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수