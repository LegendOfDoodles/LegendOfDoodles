// IDI_LEGENDOFDOODLES.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "LOD_Server.h"
#include "00.Global/01.Utility/00.Timer/Timer.h"

#define MAX_LOADSTRING 100

// 전역 변수:
CTimer timer;	// 타이머

				// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
BOOL                InitInstance();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(nCmdShow);


	// TODO: 여기에 코드를 입력합니다.

	MSG msg{ NULL };
	vector <thread> w_threads;



	try
	{
		// 응용 프로그램 초기화를 수행합니다.
		if (!InitInstance())
		{
			return FALSE;
		}
		for (int i = 0; i < 4; ++i) w_threads.push_back(thread{ worker_thread });
		thread a_thread{ accept_thread };
		thread time_thread{ timer_thread };
		printf("-----------------Server Online-----------------\n");

		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			else if (timer.Update())
			{
				g_Framework.FrameAdvance(timer.GetTimeElapsed());
			}
			if (!g_Framework.IsRunning()) break;
		}
		
		g_Framework.Finalize();
		for (auto& th : w_threads) th.join();
		a_thread.join();
		time_thread.join();
	}
	catch (DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"hResult Error", MB_OK);
	}

	

	return (int)msg.wParam;
}

BOOL InitInstance()
{
	// Framework Initialize
	g_Framework.Initialize();
	NetworkInitialize();

	return TRUE;
}
