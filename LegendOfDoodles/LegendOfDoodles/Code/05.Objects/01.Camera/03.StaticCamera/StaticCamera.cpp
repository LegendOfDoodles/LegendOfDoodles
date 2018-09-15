#include "stdafx.h"
#include "StaticCamera.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"

/// <summary>
/// 목적: 입력 받지 않는 고정 카메라
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-09-15
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CStaticCamera::CStaticCamera()
{
	m_viewport = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
	m_scissorRect = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT };
}

CStaticCamera::~CStaticCamera()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
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
// 내부 함수
