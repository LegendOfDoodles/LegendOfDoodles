#include "protocol.h"
#include "Winsock2.h"

class CScene;
class CGameScene;
class CAnimatedObject;
class CGameScene;
class CMinionShader;
class CNumberShader;
class CPlayer;
class CWayFinder;
class CCollisionManager;
class CEffectMgr;

#pragma once

struct EXOVER {
	WSAOVERLAPPED m_over;
	char m_iobuf[MAX_BUFF_SIZE];
	WSABUF m_wsabuf;
	bool is_recv;
};


class CNetwork
{
public:
	SOCKET  m_mysocket{ NULL };
	WSABUF	m_send_wsabuf;
	char 	m_send_buffer[MAX_BUFF_SIZE];
	WSABUF	m_recv_wsabuf;
	char	m_recv_buffer[MAX_BUFF_SIZE];
	char	m_packet_buffer[MAX_BUFF_SIZE];
	DWORD	m_in_packet_size = 0;
	int		m_saved_packet_size = 0;
	int		m_myid{ NONE };
	int		m_minon_index;

	bool m_gameFinished{ false };
	bool m_gameLoaded{ false };

	CardType m_EachCardType[MAX_USER];
	bool m_EachPlayerReadyState[MAX_USER];

protected:
	HWND m_hWnd;

	shared_ptr<CWayFinder> m_pWayFinder;
	shared_ptr<CCollisionManager> m_pColManager;
	shared_ptr<CEffectMgr> m_pEffectMgr;
	shared_ptr<CGameScene> m_pGameScene;
	shared_ptr<CScene> m_pRoomScene;

	CAnimatedObject ** m_ppPlayer{ NULL };
	CMinionShader *m_pMinionShader{ NULL };
	CNumberShader *m_pNumberShader{ NULL };
	CBaseObject** m_ppNexusTower{ NULL };

public:
	CNetwork();
	~CNetwork();
	bool Initialize(HWND hWnd);
	void ProcessPacket(char *ptr);
	void Finalize();
	void ReadPacket();
	void SendPacket(void *ptr);

	void ResetGameData();
	//void err_display(void* msg);

	void PrepareData();

	void SetRoomScene(shared_ptr<CScene> pScene) { m_pRoomScene = pScene; }
	void SetGameScene(shared_ptr<CGameScene> pScene) { m_pGameScene = pScene; }

	void SetCollisionManager(shared_ptr<CCollisionManager> pColmanager) { m_pColManager = pColmanager; }
	void SetEffectManager(shared_ptr<CEffectMgr> pManager) { m_pEffectMgr = pManager; }
	void SetWayfinder(shared_ptr<CWayFinder> pWayFinder) { m_pWayFinder = pWayFinder; }
	void SetPlayers(CCollisionObject** player) { m_ppPlayer = (CAnimatedObject**)player; }
	void SetNexusTowers(CBaseObject** nexus) { m_ppNexusTower = nexus; }
};

