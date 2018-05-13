#pragma once
class CScene;

class CFramework
{
public:	// ������, �Ҹ���
	CFramework();
	~CFramework();

public: // ���� �Լ�
	// Initialize and Release
	bool Initialize();
	void Finalize();
	void FrameAdvance(float timeElapsed);

	// Message Process
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID,
		WPARAM wParam, LPARAM lParam);

private: // ���� �Լ�
	void BuildObjects();
	void ReleaseObjects();

private: // ����
	CScene *m_pScene{ NULL };
};

