#pragma once

#include <chrono>

class CTimer
{
	using clock = std::chrono::system_clock;
public: // ������, �Ҹ���
	CTimer();
	virtual ~CTimer();

public:	// ���� �Լ�
	void SetUpdateCaptionHwnd(HWND hWnd);

	bool Update();

	float GetTimeElapsed() const { return m_timeElapsed; }

private:	// ���� �Լ�
	void UpdateCaption();

	float GetFPS() const { return { 0.5f + m_preCumulativeFPS / static_cast<float>(m_preCumulativeFPSCount) }; }

private:	// ����
	clock::time_point m_current_time{ clock::now() };

	float		m_timeElapsed{ 0.f };

	float		m_cumulativeFPS{ 0.f };
	int		m_cumulativeFPSCount{ 0 };

	float		m_preCumulativeFPS{ 0.f };
	int		m_preCumulativeFPSCount{ 0 };

	clock::time_point m_LastUpdate_time{ clock::now() };

	std::chrono::duration<float> m_updateElapsed{ 0.f };

	HWND		m_hWnd{ nullptr };
	TCHAR		m_CaptionTitle[TITLE_MAX_LENGTH];
	int			m_TitleLength{ 0 };

#if defined(SHOW_CAPTIONFPS)
	int			m_nLastFps{ 0 };
#endif

};

