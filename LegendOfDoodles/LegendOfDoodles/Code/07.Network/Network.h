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
	int		m_myid;
	int		m_minon_index;


	CBaseObject * m_pSelectedObject{ NULL };
protected:
	HWND m_hWnd;

	shared_ptr<CWayFinder> m_pWayFinder;
	shared_ptr<CCollisionManager> m_pColManager;
	shared_ptr<CEffectMgr> m_pEffectMgr;
	shared_ptr<CGameScene> m_pScene;

	CAnimatedObject ** m_ppPlayer{ NULL };
	CMinionShader *m_pMinionShader{ NULL };
	CNumberShader *m_pNumberShader{ NULL };
	/*CollisionObjectList* m_pBlueSwordMinions;
	CollisionObjectList* m_pBlueStaffMinions;
	CollisionObjectList* m_pBlueBowMinions;
	CollisionObjectList* m_pRedSwordMinions;
	CollisionObjectList* m_pRedStaffMinions;
	CollisionObjectList* m_pRedBowMinions;*/

	CBaseObject** m_ppNexusTower{ NULL };

public:
	CNetwork();
	~CNetwork();
	void Initialize(HWND hWnd);
	void ProcessPacket(char *ptr);
	void Finalize();
	void ReadPacket();
	void SendPacket(void *ptr);
	//void err_display(void* msg);

	void SetGameScene(shared_ptr<CGameScene> pScene);
	void PrepareData();

	void SetCollisionManager(shared_ptr<CCollisionManager> pColmanager) { m_pColManager = pColmanager; }
	void SetEffectManager(shared_ptr<CEffectMgr> pManager) { m_pEffectMgr = pManager; }
	void SetWayfinder(shared_ptr<CWayFinder> pWayFinder) { m_pWayFinder = pWayFinder; }
	void SetPlayers(CCollisionObject** player) { m_ppPlayer = (CAnimatedObject**)player; }
	void SetNexusTowers(CBaseObject** nexus) { m_ppNexusTower = nexus; }
};

