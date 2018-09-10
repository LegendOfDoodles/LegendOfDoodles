#pragma once


#define WIN32_LEAN_AND_MEAN  
#define INITGUID
#pragma comment(lib, "ws2_32.lib")
// include important windows stuff
#include <windows.h> 
#include <Winsock2.h>

//참조 헤더
#include <thread>
#include <vector>
#include <array>
#include <iostream>
#include <unordered_set>
#include <mutex>
#include <chrono>

class CCollisionObject;
class CNexusTower;
class CScene;
class CAnimatedObject;

using namespace std;
using namespace chrono;

typedef std::list<CCollisionObject*> CollisionObjectList;

struct EXOVER {
	WSAOVERLAPPED m_over;
	char m_iobuf[MAX_BUFF_SIZE];
	WSABUF m_wsabuf;
	bool is_recv;
};

class Client {
public:
	SOCKET m_s;
	bool m_isconnected;
	float m_x;
	float m_y;
	float m_maxhp;
	float m_curhp;
	int m_anistate;
	int m_weaponstate;
	
	//아직 정보 설정 안한 얘들
	int m_kill;
	int m_death;
	int m_maxexp;
	int m_exp;
	int m_level;
	system_clock::time_point m_login;
	std::chrono::duration<double> m_duration;

	XMFLOAT2 m_targetlocation;
	bool m_changetarget;

	float m_frameTime;
	XMFLOAT3 m_vLook;
	EXOVER m_rxover;
	int m_packet_size;  // 지금 조립하고 있는 패킷의 크기
	int	m_prev_packet_size; // 지난번 recv에서 완성되지 않아서 저장해 놓은 패킷의 앞부분의 크기
	char m_packet[MAX_PACKET_SIZE];
	unordered_set <int> m_viewlist;
	mutex m_mvl; 

	Client()
	{
		m_isconnected = false;
		m_x = 0;
		m_y = 0;
		m_login = {};
		m_changetarget = false;
		ZeroMemory(&m_rxover.m_over, sizeof(WSAOVERLAPPED));
		m_rxover.m_wsabuf.buf = m_rxover.m_iobuf;
		m_rxover.m_wsabuf.len = sizeof(m_rxover.m_wsabuf.buf);
		m_rxover.is_recv = true;
		m_prev_packet_size = 0;
	}
};

class Minion {
public:
	float m_x;
	float m_y;
	float m_maxhp;
	float m_curhp;
	int m_anistate;
	float m_frameTime;
	XMFLOAT3 m_vLook;

public:
	Minion()
	{
	}
};

class Royder {
public:
	int m_x;
	int m_y;
	int m_maxhp;
	int m_curhp;
	int m_anistate;
	float m_frameTime;
	XMFLOAT3 m_vLook;

public:
	Royder()
	{
	}
};

class NexusTower {
public:
	XMFLOAT3 m_vPos;
	int m_maxhp;
	int m_curhp;
};

//////////////////////////////////////////////////////////////
// 전역 변수
extern HANDLE gh_iocp;

extern array <Client, MAX_USER> g_clients;
extern array <bool, MAX_USER> g_loaded;
extern array <NexusTower, 14> g_nexustowers;

//CScene* g_pScene{ NULL };



extern shared_ptr<CScene> g_pScene;
extern CAnimatedObject** g_ppPlayer;
extern CollisionObjectList* g_blueSwordMinions;
extern CollisionObjectList* g_blueStaffMinions;
extern CollisionObjectList* g_blueBowMinions;
extern CollisionObjectList* g_redSwordMinions;
extern CollisionObjectList* g_redStaffMinions;
extern CollisionObjectList* g_redBowMinions;
extern CAnimatedObject** g_ppNeutrality;
extern CCollisionObject** g_ppNexusTower;

extern bool AcceptFinish;

extern int g_MinionCounts;
extern int g_ReuseMinion;

extern bool g_Clientsync;

extern float g_GameTime;

extern CommonInfo g_SwordMinionStat;
extern CommonInfo g_StaffMinionStat;
extern CommonInfo g_BowMinionStat;

//////////////////////////////////////////////////////////////
// 네크워크 함수
void error_display(const char *msg, int err_no);


void ErrorDisplay(const char * location);

void initialize(shared_ptr<CScene> pScene);


void StartRecv(int id);


void SendPacket(int id, void *ptr);

void SendPutObjectPacket(int client, int object);

void SendRemovePacket(int client, int object);

void ProcessPacket(int id, char *packet);

void DisconnectPlayer(int id);


void worker_thread();


void accept_thread();	//새로 접속해 오는 클라이언트를 IOCP로 넘기는 역할

void timer_thread();

void condition_thread();
