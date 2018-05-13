#include "stdafx.h"
#include "Timer.h"

using namespace std::chrono;

/// <summary>
/// 목적: 타이머, 프레임 고정, 프레임 워크에 표시 용
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-04-14
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CTimer::CTimer()

{
	
}

CTimer::~CTimer()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CTimer::SetUpdateCaptionHwnd(HWND hWnd)
{
	m_hWnd = hWnd;

	// 캡션 변경
#if defined(TITLESTRING)
	lstrcpy(m_CaptionTitle, TITLESTRING);
#else
	GetWindowText(m_hWnd, m_CaptionTitle, TITLE_MAX_LENGTH);
#endif

#if defined(SHOW_CAPTIONFPS)
	lstrcat(m_CaptionTitle, TEXT(" ("));
#endif

	m_TitleLength = lstrlen(m_CaptionTitle);
	SetWindowText(m_hWnd, m_CaptionTitle);

}

bool CTimer::Update()
{
	auto now = clock::now();

	duration<float> timeElapsed = now - m_current_time;
	auto fElapsed = timeElapsed.count();
	
	/////////////////////////////////////////////////////////////////////////
	if (fElapsed <= MAX_FPS)									return false;

	m_current_time = now;
	m_timeElapsed = fElapsed;
	float fps = 1.f / m_timeElapsed;

	m_cumulativeFPS += fps;
	m_cumulativeFPSCount++;

	m_updateElapsed = now - m_LastUpdate_time;

	/////////////////////////////////////////////////////////////////////////
#if defined(SHOW_CAPTIONFPS)
	if (m_updateElapsed.count() <= MAX_UPDATE_FPS)				return true;

	m_LastUpdate_time = now;

	m_preCumulativeFPS = m_cumulativeFPS;
	m_preCumulativeFPSCount = m_cumulativeFPSCount;

	m_cumulativeFPS = 0.f;
	m_cumulativeFPSCount = 0;

	if (m_hWnd) UpdateCaption();
#endif

	
	return true;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CTimer::UpdateCaption()
{
	// 프레임레이트를 출력합니다.
#if defined(SHOW_CAPTIONFPS)

	int nCurrentFps = static_cast<int>(GetFPS());

	if (m_nLastFps == nCurrentFps) return;

	m_nLastFps = nCurrentFps;

	_itow_s(m_nLastFps, m_CaptionTitle + m_TitleLength, TITLE_MAX_LENGTH - m_TitleLength, 10);
	wcscat_s(m_CaptionTitle + m_TitleLength, TITLE_MAX_LENGTH - m_TitleLength, TEXT(" FPS)"));
	SetWindowText(m_hWnd, m_CaptionTitle);
#endif
}