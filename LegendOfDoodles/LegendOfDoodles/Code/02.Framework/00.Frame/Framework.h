#pragma once
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "03.Scenes/00.BaseScene/Scene.h"
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"
#include "07.Network/Network.h"

class CFramework
{
public:	// 생성자, 소멸자
	CFramework();
	~CFramework();

public: // 공개 함수
		// Initialize and Release
	bool Initialize(HINSTANCE hInstance, HWND hWnd, CNetwork* pNetwork);
	void Finalize();
	void FrameAdvance(float timeElapsed);

	// Message Process
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID,
		WPARAM wParam, LPARAM lParam);

	bool IsRunning() { return m_running; }

private: // 내부 함수
	void BuildObjects();
	void ReleaseObjects();

private: // 변수
	HWND m_hWnd{ NULL };

	shared_ptr<CCreateMgr> m_pCreateMgr;
	shared_ptr<CRenderMgr> m_pRenderMgr;

	shared_ptr<CScene> m_pScene;

	shared_ptr<CNetwork> m_pNetwork;

	bool m_running{ true };
};

