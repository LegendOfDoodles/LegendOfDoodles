#pragma once
#
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

//클라쪽 헤더
#include "03.Scenes\00.BaseScene\Scene.h"
#include "04.Shaders\05.PlayerShader\PlayerShader.h"
#include "05.Objects\03.AnimatedObject\AnimatedObject.h"
#include "05.Objects\08.Player\Player.h"
#include "05.Objects\06.Minion\Minion.h"
#include "05.Objects\09.NexusTower\NexusTower.h"
using namespace std;
using namespace chrono;

typedef std::list<CCollisionObject*> CollisionObjectList;

HANDLE gh_iocp;
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
	int m_x;
	int m_y;
	int m_maxhp;
	int m_curhp;
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
		ZeroMemory(&m_rxover.m_over, sizeof(WSAOVERLAPPED));
		m_rxover.m_wsabuf.buf = m_rxover.m_iobuf;
		m_rxover.m_wsabuf.len = sizeof(m_rxover.m_wsabuf.buf);
		m_rxover.is_recv = true;
		m_prev_packet_size = 0;
	}
};

class Minion {
public:
	int m_x;
	int m_y;
	int m_maxhp;
	int m_curhp;
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

array <Minion, 300> g_blueminions;
array <Minion, 300> g_redminions;
array <Client, MAX_USER> g_clients;
array <NexusTower, 14> g_nexustowers;

//CScene* g_pScene{ NULL };

CollisionObjectList* g_pBlueMinions{ NULL };
CollisionObjectList* g_pRedMinions{ NULL };
CNexusTower** g_ppNexusTower{ NULL };

shared_ptr<CScene> g_pScene;
CAnimatedObject** g_ppPlayer{ NULL };

bool AcceptFinish = false;

int g_MinionCounts = 0;
int g_ReuseMinion = -1;
float g_PacketCoolTime = 0;
bool g_Clientsync = false;
void error_display(const char *msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::wcout << L"  에러" << lpMsgBuf << std::endl;
	LocalFree(lpMsgBuf);
	while (true);
}

void ErrorDisplay(const char * location)
{
	error_display(location, WSAGetLastError());
}
//굳이 서버에서 안할듯 싶음
bool CanSee(int a, int b)
{
	int dist_x = g_clients[a].m_x - g_clients[b].m_x;
	int dist_y = g_clients[a].m_y - g_clients[b].m_y;
	int dist = dist_x*dist_x + dist_y*dist_y;
	//don't use sqrt() in SERVER! 
	return (VIEW_RADIUS * VIEW_RADIUS >= dist);
}

bool IsNPC(int id)
{
	return ((id >= NPC_START) && id < (NUM_OF_NPC));
}

void initialize(shared_ptr<CScene> pScene)
{
	g_pScene = pScene;

	g_ppPlayer = g_pScene->GetPlayerObject();
	
	//g_pBlueMinions = g_pScene->GetBlueObjects();
	//g_pRedMinions = g_pScene->GetRedObjects();
	//g_ppNexusTower = g_pScene->GetNexusTower();

	gh_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0); // 의미없는 파라메터, 마지막은 알아서 쓰레드를 만들어준다.
	std::wcout.imbue(std::locale("korean"));

	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);
}

void StartRecv(int id)
{
	unsigned long r_flag = 0;
	ZeroMemory(&g_clients[id].m_rxover.m_over, sizeof(WSAOVERLAPPED));
	int ret = WSARecv(g_clients[id].m_s, &g_clients[id].m_rxover.m_wsabuf, 1,
		NULL, &r_flag, &g_clients[id].m_rxover.m_over, NULL);
	if (0 != ret) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no) error_display("Recv Error", err_no);
	}
}

void SendPacket(int id, void *ptr)
{
	char *packet = reinterpret_cast<char *>(ptr);
	EXOVER *s_over = new EXOVER;
	s_over->is_recv = false;
	memcpy(s_over->m_iobuf, packet, packet[0]);
	s_over->m_wsabuf.buf = s_over->m_iobuf;
	s_over->m_wsabuf.len = s_over->m_iobuf[0];
	ZeroMemory(&s_over->m_over, sizeof(WSAOVERLAPPED));
	int res = WSASend(g_clients[id].m_s, &s_over->m_wsabuf, 1, NULL, 0,
		&s_over->m_over, NULL);
	if (0 != res) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no) error_display("Send Error! ", err_no);
	}
}
void SendPutObjectPacket(int client, int object)
{
	SC_Msg_Put_Character p;
	p.Character_id = object;
	p.size = sizeof(p);
	p.type = SC_PUT_PLAYER;
	p.x = g_clients[object].m_x;
	p.y = g_clients[object].m_y;
	SendPacket(client, &p);
}
void SendRemovePacket(int client, int object)
{
	SC_Msg_Remove_Character p;
	p.Character_id = object;
	p.size = sizeof(p);
	p.type = SC_REMOVE_PLAYER;

	SendPacket(client, &p);
}
void ProcessPacket(int id, char *packet)
{
	//클라로부터 오는 패킷 종류들
	CS_MsgChMove* MovePacket = reinterpret_cast<CS_MsgChMove*>(packet);
	CS_MsgChCollision* CollisionPacket = reinterpret_cast<CS_MsgChCollision*>(packet);
	CS_MsgMoDelete* DeleteMinionPacket = reinterpret_cast<CS_MsgMoDelete*>(packet);
	CS_Msg_Demand_Use_Skill* CSkillPacket = reinterpret_cast<CS_Msg_Demand_Use_Skill*>(packet);
	CS_Msg_Demand_Change_Weapon* CChangeWeapon = reinterpret_cast<CS_Msg_Demand_Change_Weapon*>(packet);
	int x = 0;
	int y = 0;
	//서버에서 클라로 보내줘야할 패킷들
	SC_MsgMoCreate p;
	SC_Msg_Permit_Use_Skill SkillPacket;
	//cout << packet[1] << endl;
	switch (MovePacket->type)
	{
		//이동하는 부분
	case CS_MOVE_PLAYER:
	{
		XMFLOAT3 pickposition{ (float)MovePacket->x, 0 ,(float)MovePacket->y };
		g_pScene->GenerateLayEndWorldPosition(pickposition, MovePacket->Character_id);
		
		
		//g_clients[MovePacket->Character_id].m_x = MovePacket->x;
		//g_clients[MovePacket->Character_id].m_y = MovePacket->y;
		//x = MovePacket->x;
		//y = MovePacket->y;
		//cout << "Client[" << id << "] X is " << MovePacket->x << endl;
		//cout << "Client[" << id << "] Y is " << MovePacket->y << endl;
		break;
	}
	/*case CS_COLLISION:
	{
	if (g_clients[CollisionPacket->Character_id].m_x != CollisionPacket->x || g_clients[CollisionPacket->Character_id].m_y != CollisionPacket->y) {
	g_clients[CollisionPacket->Character_id].m_x = CollisionPacket->x;
	g_clients[CollisionPacket->Character_id].m_y = CollisionPacket->y;
	cout << "Client[" << id << "] Collied at X = " << g_clients[CollisionPacket->Character_id].m_x << " Y = " << g_clients[CollisionPacket->Character_id].m_y << endl;

	break;
	}
	else {
	break;
	}
	}*/

	case CS_DEMAND_SYNC:
	{
		g_Clientsync = true;
		break;
	}
	//재사용을 할 필요가 있을까? --> 적용했는데 잘 안될경우 그냥 필요한 수만큼 선언.
	case CS_PUT_MINION:
	{
		break;
	}
	case CS_DELETE_MINION:
	{
		break;
	}
	case CS_DAMAND_MAKE_ROOM:
	{
		cout << "fuck!\n" << endl;
		break;
	}

	case CS_DEMAND_USE_SKILL:
	{
		//dynamic_cast<CPlayer*>(g_ppPlayer[CSkillPacket->Character_id])->ActiveSkill((AnimationsType)CSkillPacket->skilltype);
		break;
	}
	case CS_DEMAND_CHANGE_WEAPON:
	{
		//dynamic_cast<CPlayer*>(g_ppPlayer[CChangeWeapon->Character_id])->ChangeWeapon();
		break;
	}
	default:
		cout << "Unkown Packet Type from Client [" << id << "]\n";
		return;
	}
}

void DisconnectPlayer(int id)
{
	SC_Msg_Remove_Character p;
	p.Character_id = id;
	p.size = sizeof(p);
	p.type = SC_REMOVE_PLAYER;
	for (int i = 0; i < MAX_USER; ++i) {
		if (false == g_clients[i].m_isconnected) continue;
		if (i == id) continue;
		if (true == IsNPC(i)) break;
		g_clients[i].m_mvl.lock();
		if (0 != g_clients[i].m_viewlist.count(id)) {
			g_clients[i].m_viewlist.erase(id);
			g_clients[i].m_mvl.unlock();
			SendPacket(i, &p);
		}
		else {
			g_clients[i].m_mvl.unlock();
		}
	}
	closesocket(g_clients[id].m_s);
	g_clients[id].m_mvl.lock();
	g_clients[id].m_viewlist.clear();
	g_clients[id].m_mvl.unlock();
	g_clients[id].m_isconnected = false;
}

void worker_thread()
{
	while (true)
	{
		unsigned long io_size;
		unsigned long long iocp_key; // 64 비트 integer , 우리가 64비트로 컴파일해서 64비트
		WSAOVERLAPPED *over;
		BOOL ret = GetQueuedCompletionStatus(gh_iocp, &io_size, &iocp_key, &over, INFINITE);
		int key = static_cast<int>(iocp_key);
		//cout << "WT::Network I/O with Client [" << key << "]\n";
		if (FALSE == ret) {
			cout << "Error in GQCS\n";
			DisconnectPlayer(key);
			continue;
		}
		if (0 == io_size) {
			DisconnectPlayer(key);
			continue;
		}

		EXOVER *p_over = reinterpret_cast<EXOVER *>(over);
		if (true == p_over->is_recv) {
			//cout << "WT:Packet From Client [" << key << "]\n";
			int work_size = io_size;
			char *wptr = p_over->m_iobuf;
			while (0 < work_size) {
				int p_size;
				if (0 != g_clients[key].m_packet_size)
					p_size = g_clients[key].m_packet_size;
				else {
					p_size = wptr[0];
					g_clients[key].m_packet_size = p_size;
				}
				int need_size = p_size - g_clients[key].m_prev_packet_size;
				if (need_size <= work_size) {
					memcpy(g_clients[key].m_packet
						+ g_clients[key].m_prev_packet_size, wptr, need_size);
					
					ProcessPacket(key, g_clients[key].m_packet);

					g_clients[key].m_prev_packet_size = 0;
					g_clients[key].m_packet_size = 0;
					work_size -= need_size;
					wptr += need_size;
				}
				else {
					memcpy(g_clients[key].m_packet + g_clients[key].m_prev_packet_size, wptr, work_size);
					g_clients[key].m_prev_packet_size += work_size;
					work_size = -work_size;
					wptr += work_size;
				}
			}
			StartRecv(key);
		}
		else {  // Send 후처리
				//cout << "WT:A packet was sent to Client[" << key << "]\n";
			delete p_over;
		}
	}
}

void accept_thread()	//새로 접속해 오는 클라이언트를 IOCP로 넘기는 역할
{
	SOCKET s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN bind_addr;
	ZeroMemory(&bind_addr, sizeof(SOCKADDR_IN));
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_port = htons(MY_SERVER_PORT);
	bind_addr.sin_addr.s_addr = INADDR_ANY;	// 0.0.0.0  아무대서나 오는 것을 다 받겠다.

	::bind(s, reinterpret_cast<sockaddr *>(&bind_addr), sizeof(bind_addr));
	listen(s, 1000);

	while (true)
	{
		SOCKADDR_IN c_addr;
		ZeroMemory(&c_addr, sizeof(SOCKADDR_IN));
		c_addr.sin_family = AF_INET;
		c_addr.sin_port = htons(MY_SERVER_PORT);
		c_addr.sin_addr.s_addr = INADDR_ANY;	// 0.0.0.0  아무대서나 오는 것을 다 받겠다.
		int addr_size = sizeof(sockaddr);

		SOCKET cs = WSAAccept(s, reinterpret_cast<sockaddr *>(&c_addr), &addr_size, NULL, NULL);
		if (INVALID_SOCKET == cs) {
			ErrorDisplay("In Accept Thread:WSAAccept()");
			continue;
		}
		int id = -1;
		for (int i = 0; i < MAX_USER; ++i)
			if (false == g_clients[i].m_isconnected) {
				id = i;
				break;
			}
		if (-1 == id) {
			cout << "MAX USER Exceeded\n";
			continue;
		}
		cout << "ID of new Client is [" << id << "]\n";
		g_clients[id].m_s = cs;
		//clear for reuse
		g_clients[id].m_packet_size = 0;
		g_clients[id].m_prev_packet_size = 0;
		g_clients[id].m_viewlist.clear();

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(cs), gh_iocp, id, 0);
		g_clients[id].m_isconnected = true;
		StartRecv(id);

		g_clients[id].m_login = system_clock::now();
		SC_Msg_Put_Character p;
		p.Character_id = id;
		p.size = sizeof(p);
		p.type = SC_PUT_PLAYER;
		p.x = g_ppPlayer[id]->GetPosition().x;
		p.y = g_ppPlayer[id]->GetPosition().z;
		SendPacket(id, &p);
		//for (int i = 0; i < MAX_USER; ++i)
		//{
		//	//연결된 애들한테 4개 플레이어 패킷 다 보냄
		//	if (g_clients[i].m_isconnected) {
		//		SendPacket(i, &p);
		//	}
		//}

		//지금 연결된 애한테 4명 어디있는지 
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (i == id) continue;
			p.Character_id = i;
			p.x = g_ppPlayer[i]->GetPosition().x;
			p.y = g_ppPlayer[i]->GetPosition().z;

			SendPacket(id, &p);
			
		}
	}
}

void timer_thread()
{
	if(g_Clientsync)
	while (1)
	{
		Sleep(10);
		g_PacketCoolTime += 10;
		if (g_PacketCoolTime >= 1000)
		{
			cout << "위치 동기화 패킷 보냈어요~" << endl;
			g_PacketCoolTime = 0;
			//wait for 0.3second
			//Sleep(10);
			// cool time += 10
			// Cool Time >= 1000 -> 작은 패킷 종류 적어서 보내고 원래 전송
			// 아니면 넘어가는데 작은 패킷 없는거로 보내고

			//system_clock::time_point duration = system_clock::now();
			for (int i = 0; i < MAX_USER; ++i) {
				//if (i == 0) cout << "x=" << g_ppPlayer[i]->GetPosition().x << "z =" << g_ppPlayer[i]->GetPosition().z << endl;
				g_clients[i].m_x = g_ppPlayer[i]->GetPosition().x;
				g_clients[i].m_y = g_ppPlayer[i]->GetPosition().z;

				//if(i==0) cout << "x=" << g_clients[i].m_x << "z =" << g_clients[i].m_y << endl;
				//g_clients[i].m_anistate = g_ppPlayer[i]->GetAnimState();
				//g_clients[i].m_frameTime = g_ppPlayer[i]->GetFrameTime();
				//g_clients[i].m_vLook = g_ppPlayer[i]->GetLook();
				/*g_clients[i].m_maxhp = ((CPlayer*)g_ppPlayer[i])->GetPlayerStatus()->maxHP;
				g_clients[i].m_curhp = ((CPlayer*)g_ppPlayer[i])->GetPlayerStatus()->HP;

				g_clients[i].m_level = ((CPlayer*)g_ppPlayer[i])->GetPlayerStatus()->Level;
				g_clients[i].m_maxexp = ((CPlayer*)g_ppPlayer[i])->GetPlayerStatus()->MaxExp;
				g_clients[i].m_exp = ((CPlayer*)g_ppPlayer[i])->GetPlayerStatus()->Exp;

				g_clients[i].m_weaponstate = ((CPlayer*)g_ppPlayer[i])->GetWeaponState();
				g_clients[i].m_duration = duration - g_clients[i].m_login;*/
			}

			//Send Every User's Position Packet

			for (int i = 0; i < MAX_USER; ++i) {
				if (g_clients[i].m_isconnected == true) {
					SC_Msg_Pos_Character p;
					p.Character_id = i;
					p.size = sizeof(p);
					p.type = SC_POS;
					//p.maxhp = g_clients[i].m_maxhp;
					//p.curhp = g_clients[i].m_curhp;
					//p.weapon = g_clients[i].m_weaponstate;
					p.x = g_clients[i].m_x;
					p.y = g_clients[i].m_y;
					//p.state = g_clients[i].m_anistate;
					//p.frameTime = g_clients[i].m_frameTime;
					//p.vLook = g_clients[i].m_vLook;

					for (int j = 0; j < MAX_USER; ++j) {
						if (g_clients[j].m_isconnected == true) {
							SendPacket(j, &p);
							
						}
					}
				}
			}

			//	int idx{ 0 };
			//	for (auto iter = g_pBlueMinions->begin(); iter != g_pBlueMinions->end(); ++iter, ++idx) {
			//		g_blueminions[idx].m_x = (*iter)->GetPosition().x;
			//		g_blueminions[idx].m_y = (*iter)->GetPosition().z;
			//		g_blueminions[idx].m_anistate = (*iter)->GetAnimState();
			//		g_blueminions[idx].m_frameTime = (*iter)->GetFrameTime();
			//		g_blueminions[idx].m_vLook = (*iter)->GetLook();
			//		g_blueminions[idx].m_maxhp = ((CMinion*)(*iter))->GetCommonStatus()->maxHP;
			//		g_blueminions[idx].m_curhp = ((CMinion*)(*iter))->GetCommonStatus()->HP;

			//	}

			//	for (int i = 0; i < MAX_USER; ++i) {
			//		SC_Msg_Minion_Count p;
			//		p.color = 1;
			//		p.count = idx;
			//		p.size = sizeof(p);
			//		p.type = SC_MINION_COUNT;
			//		if (g_clients[i].m_isconnected == true) {
			//			SendPacket(i, &p);
			//		}
			//	}

			//	//Send Every User Blue Minion Packet
			//	for (int i = 0; i < idx; ++i) {
			//		SC_Msg_Pos_Minion p;
			//		p.color = 1;
			//		p.size = sizeof(p);
			//		p.x = g_blueminions[i].m_x;
			//		p.y = g_blueminions[i].m_y;
			//		p.maxhp = g_blueminions[i].m_maxhp;
			//		p.curhp = g_blueminions[i].m_curhp;

			//		p.state = g_blueminions[i].m_anistate;
			//		p.type = SC_POS_MINION;
			//		p.frameTime = g_blueminions[i].m_frameTime;
			//		p.vLook = g_blueminions[i].m_vLook;
			//		for (int j = 0; j < MAX_USER; ++j) {
			//			if (g_clients[j].m_isconnected == true) {
			//				SendPacket(j, &p);
			//			}
			//		}
			//	}

			//	idx = 0;
			//	for (auto iter = g_pRedMinions->begin(); iter != g_pRedMinions->end(); ++iter, ++idx) {
			//		g_redminions[idx].m_x = (*iter)->GetPosition().x;
			//		g_redminions[idx].m_y = (*iter)->GetPosition().z;
			//		g_redminions[idx].m_anistate = (*iter)->GetAnimState();
			//		g_redminions[idx].m_frameTime = (*iter)->GetFrameTime();
			//		g_redminions[idx].m_vLook = (*iter)->GetLook();
			//		g_redminions[idx].m_maxhp = ((CMinion*)(*iter))->GetCommonStatus()->maxHP;
			//		g_redminions[idx].m_curhp = ((CMinion*)(*iter))->GetCommonStatus()->HP;
			//	}

			//	for (int i = 0; i < MAX_USER; ++i) {
			//		SC_Msg_Minion_Count p;
			//		p.color = 0;
			//		p.count = idx;
			//		p.size = sizeof(p);
			//		p.type = SC_MINION_COUNT;
			//		if (g_clients[i].m_isconnected == true) {
			//			SendPacket(i, &p);
			//		}
			//	}

			//	//Send Every User Red Minion Packet
			//	for (int i = 0; i < idx; ++i) {
			//		SC_Msg_Pos_Minion p;
			//		p.color = 0;
			//		p.size = sizeof(p);
			//		p.x = g_redminions[i].m_x;
			//		p.y = g_redminions[i].m_y;
			//		p.maxhp = g_redminions[i].m_maxhp;
			//		p.curhp = g_redminions[i].m_curhp;
			//		p.type = SC_POS_MINION;
			//		p.state = g_redminions[i].m_anistate;
			//		p.frameTime = g_redminions[i].m_frameTime;
			//		p.vLook = g_redminions[i].m_vLook;
			//		for (int j = 0; j < MAX_USER; ++j) {
			//			if (g_clients[j].m_isconnected == true) {
			//				SendPacket(j, &p);
			//			}
			//		}
			//	}



			//	for (int i = 0; i < 14; ++i){
			//		g_nexustowers[i].m_vPos = g_ppNexusTower[i]->GetPosition();
			//		g_nexustowers[i].m_curhp = g_ppNexusTower[i]->GetStatusInfo()->HP;
			//		g_nexustowers[i].m_maxhp = g_ppNexusTower[i]->GetStatusInfo()->maxHP;
			//	}

			//	for (int i = 0; i < 14; ++i) {
			//		SC_Msg_Pos_Nexus p;
			//		p.curhp = g_nexustowers[i].m_curhp;
			//		p.maxhp = g_nexustowers[i].m_maxhp;
			//		p.size = sizeof(p);
			//		p.type = SC_POS_NEXUS;
			//		p.Object_id = i;
			//		p.vPos = g_nexustowers[i].m_vPos;
			//		for (int j = 0; j < MAX_USER; ++j) {
			//			if (g_clients[j].m_isconnected == true) {
			//				SendPacket(j, &p);
			//			}
			//		}
			//	}
			//}
		}
	}
}

void condition_thread()
{

}