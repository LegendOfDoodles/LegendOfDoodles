#pragma once
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "03.Scenes/00.BaseScene/Scene.h"
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"
#include "07.Network/Network.h"
#include "07.Network/protocol.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

class CFramework
{
public:	// ������, �Ҹ���
	CFramework();
	~CFramework();

public: // ���� �Լ�
	// Initialize and Release
	bool Initialize(HINSTANCE hInstance, HWND hWnd, Network pNetwork);
	void Finalize();
	void FrameAdvance(float timeElapsed);

	// Message Process
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID,
		WPARAM wParam, LPARAM lParam);

private: // ���� �Լ�
	void BuildObjects();
	void ReleaseObjects();

private: // ����
	HWND m_hWnd{ NULL };

	CCreateMgr m_createMgr;
	CRenderMgr *m_pRenderMgr{ NULL };

	CScene *m_pScene{ NULL };
	
	Network m_pNetwork;

	int m_FrameCheck = 0;
};

