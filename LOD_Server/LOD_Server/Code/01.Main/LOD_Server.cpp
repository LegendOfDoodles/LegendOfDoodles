// IDI_LEGENDOFDOODLES.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "LOD_Server.h"
#include "00.Global/01.Utility/00.Timer/Timer.h"
#include "02.Framework/00.Frame/Framework.h"
#include "07.Server/Main.h"

#define MAX_LOADSTRING 100

// 전역 변수:
CFramework framework;			// 프레임 워크
CTimer timer;	// 타이머

				// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
BOOL                InitInstance(HINSTANCE, int);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	MSG msg;
	HACCEL hAccelTable;
	vector <thread> w_threads;

	for (int i = 0; i < 4; ++i) w_threads.push_back(thread{ worker_thread });
	thread a_thread{ accept_thread };
	thread time_thread{ timer_thread };
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LODSERVER));

	while (1)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
			if (!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
		static float delta{ 0 };
		delta += timer.GetTimeElapsed();
		if (!timer.Update()) continue;
		framework.FrameAdvance(timer.GetTimeElapsed());
	}
	framework.Finalize();

	for (auto& th : w_threads) th.join();
	a_thread.join();
	time_thread.join();

	return (int)msg.wParam;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	// Framework Initialize
	framework.Initialize();
	initialize(framework.GetScene());

	return TRUE;
}
