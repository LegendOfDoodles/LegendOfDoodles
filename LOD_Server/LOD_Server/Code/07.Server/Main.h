#pragma once
#
#define WIN32_LEAN_AND_MEAN  
#define INITGUID
#pragma comment(lib, "ws2_32.lib")
// include important windows stuff
#include <windows.h> 
#include <Winsock2.h>


//���� ���
#include <thread>
#include <vector>
#include <array>
#include <iostream>
#include <unordered_set>
#include <mutex>

//Ŭ���� ���
#include "03.Scenes\00.BaseScene\Scene.h"
#include "04.Shaders\05.PlayerShader\PlayerShader.h"
#include "05.Objects\03.AnimatedObject\AnimatedObject.h"
#include "05.Objects\08.Player\Player.h"
using namespace std;

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
	int m_anistate;
	float m_frameTime;
	XMFLOAT3 m_vLook;
	EXOVER m_rxover;
	int m_packet_size;  // ���� �����ϰ� �ִ� ��Ŷ�� ũ��
	int	m_prev_packet_size; // ������ recv���� �ϼ����� �ʾƼ� ������ ���� ��Ŷ�� �պκ��� ũ��
	char m_packet[MAX_PACKET_SIZE];
	unordered_set <int> m_viewlist;
	//set? vector? map? list?
	//view list has to insert, delete, search
	//set : logN, logN, logN  : more easier than map, map has body 
	//unordered_set : 1, 1, 1 : It's faster than set except look from first to end
	//vector : 1(push_back), N, N --> too big
	//map : logN, logN, logN
	//list: 1, 1, N : O(N) is too big to use....
	mutex m_mvl; //mutex for view list

	Client()
	{
		m_isconnected = false;
		m_x = 500;
		m_y = 2500;

		ZeroMemory(&m_rxover.m_over, sizeof(WSAOVERLAPPED));
		m_rxover.m_wsabuf.buf = m_rxover.m_iobuf;
		m_rxover.m_wsabuf.len = sizeof(m_rxover.m_wsabuf.buf);
		m_rxover.is_recv = true;
		m_prev_packet_size = 0;
	}
};

class Minion {
public:
	int m_hp;
	int m_id;
	Minion()
	{
		m_id = -1;
	}
};

array <Minion, NUM_OF_NPC> g_minions;
array <Client, MAX_USER> g_clients;
CScene* g_pScene{ NULL };
CAnimatedObject** g_ppPlayer{ NULL };

int g_MinionCounts = 0;
int g_ReuseMinion = -1;
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
	std::wcout << L"  ����" << lpMsgBuf << std::endl;
	LocalFree(lpMsgBuf);
	while (true);
}

void ErrorDisplay(const char * location)
{
	error_display(location, WSAGetLastError());
}
//���� �������� ���ҵ� ����
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

void initialize(CScene* pScene)
{
	g_pScene = pScene;
	g_ppPlayer = g_pScene->GetPlayerObject();

	gh_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0); // �ǹ̾��� �Ķ����, �������� �˾Ƽ� �����带 ������ش�.
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
	//Ŭ��κ��� ���� ��Ŷ ������
	CS_MsgChMove* MovePacket = reinterpret_cast<CS_MsgChMove*>(packet);
	CS_MsgChCollision* CollisionPacket = reinterpret_cast<CS_MsgChCollision*>(packet);
	CS_MsgMoDelete* DeleteMinionPacket = reinterpret_cast<CS_MsgMoDelete*>(packet);
	CS_Msg_Demand_Use_Skill* CSkillPacket = reinterpret_cast<CS_Msg_Demand_Use_Skill*>(packet);
	int x = 0;
	int y = 0;
	//�������� Ŭ��� ��������� ��Ŷ��
	SC_MsgMoCreate p;
	SC_Msg_Permit_Use_Skill SkillPacket;
	cout << packet[1] << endl;
	switch (MovePacket->type)
	{
		//�̵��ϴ� �κ�
	case CS_MOVE_PLAYER:
	{
		XMFLOAT3 pickposition{ (float)MovePacket->x, 0 ,(float)MovePacket->y };
		g_pScene->GenerateLayEndWorldPosition(pickposition, MovePacket->Character_id);
		
		
		//g_clients[MovePacket->Character_id].m_x = MovePacket->x;
		//g_clients[MovePacket->Character_id].m_y = MovePacket->y;
		//x = MovePacket->x;
		//y = MovePacket->y;
		//cout << "Client[" << id << "] X is " << g_clients[MovePacket->Character_id].m_x << endl;
		//cout << "Client[" << id << "] Y is " << g_clients[MovePacket->Character_id].m_y << endl;
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


	//������ �� �ʿ䰡 ������? --> �����ߴµ� �� �ȵɰ�� �׳� �ʿ��� ����ŭ ����.
	case CS_PUT_MINION:
	{
		for (int i = 0; i < g_MinionCounts; ++i) {
			if (g_minions[i].m_id == 0) {
				g_ReuseMinion = i;
			}
		}
		//����� �־ �����ؾ��Ѵ�.
		if (g_ReuseMinion != -1) {
			g_minions[g_ReuseMinion].m_id = NPC_START + g_MinionCounts;
			//Debugging
			cout << "Minion ID: [" << g_minions[g_ReuseMinion].m_id << "] Created\n";
			p.Monster_id = g_minions[g_ReuseMinion].m_id;
			g_MinionCounts++;
		}
		//����� ����.
		else {
			g_minions[g_MinionCounts].m_id = NPC_START + g_MinionCounts;
			//Debugging
			cout << "Minion ID: [" << g_minions[g_MinionCounts].m_id << "] Created\n";
			p.Monster_id = g_minions[g_MinionCounts].m_id;
			g_MinionCounts++;
		}
		p.size = sizeof(p);
		p.type = SC_PUT_MINION;
		for (int i = 0; i < MAX_USER; ++i) {
			if (g_clients[i].m_isconnected == true) SendPacket(i, &p);
		}
		break;
	}
	case CS_DELETE_MINION:
	{
		for (int i = 0; i < g_MinionCounts; ++i) {
			if (DeleteMinionPacket->Monster_id == g_minions[i].m_id) {
				g_minions[i].m_id = 0;
				//Debugging
				cout << "Minion ID: [" << g_minions[i].m_id << "] Deleted\n";
			}
		}
		break;
	}
	case CS_DAMAND_MAKE_ROOM:
	{
		cout << "fuck!\n" << endl;
		break;
	}

	case CS_DEMAND_USE_SKILL:
	{
		dynamic_cast<CPlayer*>(g_ppPlayer[id])->ActiveSkill((AnimationsType)CSkillPacket->skilltype);
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
		unsigned long long iocp_key; // 64 ��Ʈ integer , �츮�� 64��Ʈ�� �������ؼ� 64��Ʈ
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
			cout << "WT:Packet From Client [" << key << "]\n";
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
		else {  // Send ��ó��
				//cout << "WT:A packet was sent to Client[" << key << "]\n";
			delete p_over;
		}
	}
}

void accept_thread()	//���� ������ ���� Ŭ���̾�Ʈ�� IOCP�� �ѱ�� ����
{
	SOCKET s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN bind_addr;
	ZeroMemory(&bind_addr, sizeof(SOCKADDR_IN));
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_port = htons(MY_SERVER_PORT);
	bind_addr.sin_addr.s_addr = INADDR_ANY;	// 0.0.0.0  �ƹ��뼭�� ���� ���� �� �ްڴ�.

	::bind(s, reinterpret_cast<sockaddr *>(&bind_addr), sizeof(bind_addr));
	listen(s, 1000);

	while (true)
	{
		SOCKADDR_IN c_addr;
		ZeroMemory(&c_addr, sizeof(SOCKADDR_IN));
		c_addr.sin_family = AF_INET;
		c_addr.sin_port = htons(MY_SERVER_PORT);
		c_addr.sin_addr.s_addr = INADDR_ANY;	// 0.0.0.0  �ƹ��뼭�� ���� ���� �� �ްڴ�.
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

		SC_Msg_Put_Character p;
		p.Character_id = id;
		p.size = sizeof(p);
		p.type = SC_PUT_PLAYER;
		p.x = g_clients[id].m_x;
		p.y = g_clients[id].m_y;
		SendPacket(id, &p);
		//for (int i = 0; i < MAX_USER; ++i)
		//{
		//	//����� �ֵ����� 4�� �÷��̾� ��Ŷ �� ����
		//	if (g_clients[i].m_isconnected) {
		//		SendPacket(i, &p);
		//	}
		//}

		//���� ����� ������ 4�� ����ִ��� 
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (i == id) continue;
			p.Character_id = i;
			p.x = g_clients[i].m_x;
			p.y = g_clients[i].m_y;

			SendPacket(id, &p);

		}
	}
}

void timer_thread()
{
	while (1)
	{
		//wait for 0.3second
		Sleep(1);
		for (int i = 0; i < MAX_USER; ++i) {
			g_clients[i].m_x = g_ppPlayer[i]->GetPosition().x;
			g_clients[i].m_y = g_ppPlayer[i]->GetPosition().z;
			g_clients[i].m_anistate = g_ppPlayer[i]->GetAnimState();
			g_clients[i].m_frameTime = g_ppPlayer[i]->GetFrameTime();
			g_clients[i].m_vLook = g_ppPlayer[i]->GetLook();
		}
		
		//Send Every User's Position Packet
		for (int i = 0; i < MAX_USER; ++i) {
			if (g_clients[i].m_isconnected == true) {
				SC_Msg_Pos_Character p;
				p.Character_id = i;
				p.size = sizeof(p);
				p.type = SC_POS;
				p.x = g_clients[i].m_x;
				p.y = g_clients[i].m_y;
				p.state = g_clients[i].m_anistate;
				p.frameTime = g_clients[i].m_frameTime;
				p.vLook = g_clients[i].m_vLook;
				for (int j = 0; j < MAX_USER; ++j) {
					if (g_clients[j].m_isconnected == true) {
						SendPacket(j, &p);
					}
				}
			}
		}

	}
}

