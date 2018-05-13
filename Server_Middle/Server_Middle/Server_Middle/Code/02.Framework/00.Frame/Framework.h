#pragma once
class CScene;

class CFramework
{
public:	// 생성자, 소멸자
	CFramework();
	~CFramework();

public: // 공개 함수
	// Initialize and Release
	bool Initialize();
	void Finalize();
	void FrameAdvance(float timeElapsed);

	// Message Process
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID,
		WPARAM wParam, LPARAM lParam);

private: // 내부 함수
	void BuildObjects();
	void ReleaseObjects();

private: // 변수
	CScene *m_pScene{ NULL };
};

