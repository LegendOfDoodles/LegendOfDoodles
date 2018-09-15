#include "stdafx.h"
#include "StaticCamera.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"

/// <summary>
/// ����: �Է� ���� �ʴ� ���� ī�޶�
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-09-15
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CStaticCamera::CStaticCamera()
{
	m_viewport = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
	m_scissorRect = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT };
}

CStaticCamera::~CStaticCamera()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CStaticCamera::Initialize(shared_ptr<CCreateMgr> pCreateMgr)
{
	int width = pCreateMgr->GetWindowWidth();
	int height = pCreateMgr->GetWindowHeight();

	m_hWnd = pCreateMgr->GetHwnd();
	m_pCommandList = pCreateMgr->GetCommandList();

	SetViewport(0, 0, width, height, 0.0f, 1.0f);
	SetScissorRect(0, 0, width, height);

	GenerateOrthographicProjectionMatrix(width * 1.f, height * 1.f, 1, 5000.0f);
	GenerateViewMatrix(
		XMFLOAT3(0.0f, 0.f, 0.0f),
		XMFLOAT3(0.f, 0.f, 5000.0f));

	CreateShaderVariables(pCreateMgr, ((sizeof(VS_CB_CAMERA_INFO) + 255) & ~255));
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
