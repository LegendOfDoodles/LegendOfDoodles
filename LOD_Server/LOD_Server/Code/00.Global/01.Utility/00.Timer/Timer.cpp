#include "stdafx.h"
#include "Timer.h"

using namespace std::chrono;

/// <summary>
/// ����: Ÿ�̸�, ������ ����, ������ ��ũ�� ǥ�� ��
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-04-14
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CTimer::CTimer()

{
	
}

CTimer::~CTimer()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CTimer::SetUpdateCaptionHwnd(HWND hWnd)
{
	m_hWnd = hWnd;

	// ĸ�� ����
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
// ���� �Լ�
void CTimer::UpdateCaption()
{
	// �����ӷ���Ʈ�� ����մϴ�.
#if defined(SHOW_CAPTIONFPS)

	int nCurrentFps = static_cast<int>(GetFPS());

	if (m_nLastFps == nCurrentFps) return;

	m_nLastFps = nCurrentFps;

	_itow_s(m_nLastFps, m_CaptionTitle + m_TitleLength, TITLE_MAX_LENGTH - m_TitleLength, 10);
	wcscat_s(m_CaptionTitle + m_TitleLength, TITLE_MAX_LENGTH - m_TitleLength, TEXT(" FPS)"));
	SetWindowText(m_hWnd, m_CaptionTitle);
#endif
}