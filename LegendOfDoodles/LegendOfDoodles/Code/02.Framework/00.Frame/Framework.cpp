#include "stdafx.h"

#include "Framework.h"



/// <summary>
/// 목적: 테
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-04-14
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CFramework::CFramework()
{
}

CFramework::~CFramework()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
bool CFramework::Initialize(HINSTANCE hInstance, HWND hWnd, Network pNetwork)
{
	m_hWnd = hWnd;
	m_pNetwork = pNetwork;

	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	m_pNetwork.m_mysocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0); \
		if (m_pNetwork.m_mysocket == INVALID_SOCKET) printf("Socket Failed\n");

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(MY_SERVER_PORT);
	ServerAddr.sin_addr.s_addr = inet_addr("13.125.173.158");

	int Result = WSAConnect(m_pNetwork.m_mysocket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);

	WSAAsyncSelect(m_pNetwork.m_mysocket, m_hWnd, WM_SOCKET, FD_CLOSE | FD_READ);

	m_pNetwork.m_send_wsabuf.buf = m_pNetwork.m_send_buffer;
	m_pNetwork.m_send_wsabuf.len = MAX_BUFF_SIZE;
	m_pNetwork.m_recv_wsabuf.buf = m_pNetwork.m_recv_buffer;
	m_pNetwork.m_recv_wsabuf.len = MAX_BUFF_SIZE;

	m_createMgr.Initialize(hInstance, hWnd);
	m_pRenderMgr = m_createMgr.GetRenderMgr();

	BuildObjects();

	return(true);
}

void CFramework::Finalize()
{
	m_pNetwork.Finalize();
	ReleaseObjects();
	m_createMgr.Release();
	
}

void CFramework::FrameAdvance(float timeElapsed)
{
	
	m_pScene->ProcessInput();
	m_pScene->AnimateObjects(timeElapsed);
	m_pRenderMgr->Render(m_pScene);
}




LRESULT CALLBACK CFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
	{
		m_createMgr.Resize(LOWORD(lParam), HIWORD(lParam));
		//m_createMgr.ChangeScreenMode();
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_MOUSEWHEEL:
		m_pScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		
		break;
	}
	case WM_SOCKET:
		{
			if (WSAGETSELECTERROR(lParam)) {
				closesocket((SOCKET)wParam);
				exit(-1);
				break;
			}
			switch (WSAGETSELECTEVENT(lParam)) {
			case FD_READ:
				m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
				break;
			case FD_CLOSE:
				closesocket((SOCKET)wParam);
				exit(-1);
				break;
			}
		}
	}
	return(0);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CFramework::BuildObjects()
{
	m_pRenderMgr->ResetCommandList();

	m_pScene = new CScene();
	m_pScene->Initialize(&m_createMgr,&m_pNetwork);

	m_pRenderMgr->ExecuteCommandList();

	m_pScene->ReleaseUploadBuffers();
}

void CFramework::ReleaseObjects()
{
	if (!m_pScene) return;

	m_pScene->Finalize();
	Safe_Delete(m_pScene);
}