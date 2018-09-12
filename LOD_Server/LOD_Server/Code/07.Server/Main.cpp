#include "stdafx.h"

//클라쪽 헤더
#include "03.Scenes\01.GameScene\GameScene.h"
#include "04.Shaders\05.PlayerShader\PlayerShader.h"
#include "04.Shaders/04.AniShader/MinionShader.h"
#include "05.Objects\03.AnimatedObject\AnimatedObject.h"
#include "05.Objects\08.Player\Player.h"
#include "05.Objects/06.Minion/Minion.h"
#include "05.Objects\09.NexusTower\NexusTower.h"


HANDLE gh_iocp;

array <Client, MAX_USER> g_clients;
array <bool, MAX_USER> g_loaded;
array <NexusTower, 14> g_nexustowers;

//CScene* g_pScene{ NULL };



shared_ptr<CScene> g_pScene;
CAnimatedObject** g_ppPlayer{ NULL };
CollisionObjectList* g_blueSwordMinions;
CollisionObjectList* g_blueStaffMinions;
CollisionObjectList* g_blueBowMinions;
CollisionObjectList* g_redSwordMinions;
CollisionObjectList* g_redStaffMinions;
CollisionObjectList* g_redBowMinions;
CAnimatedObject** g_ppNeutrality{ NULL };
CCollisionObject** g_ppNexusTower{ NULL };


bool AcceptFinish = false;

int g_NeutralityCount = 0;
int g_NexusTowerCount = 0;

bool g_Clientsync = false;

float g_GameTime{ 0.0f };
CommonInfo g_SwordMinionStat;
CommonInfo g_StaffMinionStat;
CommonInfo g_BowMinionStat;

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

void initialize(shared_ptr<CScene> pScene)
{
	g_pScene = pScene;

	g_ppPlayer = g_pScene->GetPlayerObject();
	g_blueSwordMinions = g_pScene->GetShader(0)->GetMinionList(SwordMinion, Blue);
	g_blueBowMinions = g_pScene->GetShader(0)->GetMinionList(BowMinion, Blue);
	g_blueStaffMinions = g_pScene->GetShader(0)->GetMinionList(StaffMinion, Blue);
	g_redSwordMinions = g_pScene->GetShader(0)->GetMinionList(SwordMinion, Red);
	g_redBowMinions = g_pScene->GetShader(0)->GetMinionList(BowMinion, Red);
	g_redStaffMinions = g_pScene->GetShader(0)->GetMinionList(StaffMinion, Red);
	g_ppNeutrality = g_pScene->GetNeutralityObject();
	g_NeutralityCount = g_pScene->GetShader(2)->GetObjectCount();
	g_ppNexusTower = g_pScene->GetNexusTower();
	g_NexusTowerCount = g_pScene->GetShader(3)->GetObjectCount();

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
	p.Character_id = (BYTE)object;
	p.size = sizeof(p);
	p.type = SC_PUT_PLAYER;
	p.x = (short)g_clients[object].m_x;
	p.y = (short)g_clients[object].m_y;
	SendPacket(client, &p);
}
void SendRemovePacket(int client, int object)
{
	SC_Msg_Remove_Character p;
	p.Character_id = (BYTE)object;
	p.size = sizeof(p);
	p.type = SC_REMOVE_PLAYER;

	SendPacket(client, &p);
}
void ProcessPacket(int id, char *packet)
{
	//클라로부터 오는 패킷 종류들
	CS_MsgChMove* MovePacket = reinterpret_cast<CS_MsgChMove*>(packet);
	CS_Msg_Demand_Use_Skill* CSkillPacket = reinterpret_cast<CS_Msg_Demand_Use_Skill*>(packet);
	CS_Msg_Change_Weapon* WeaponPacket = reinterpret_cast<CS_Msg_Change_Weapon*>(packet);
	CS_Msg_Set_Speacial_Point* SpeacialPacket = reinterpret_cast<CS_Msg_Set_Speacial_Point*>(packet);
	CS_Msg_Prepare_Data* PreparePacket = reinterpret_cast<CS_Msg_Prepare_Data*>(packet);
	//서버에서 클라로 보내줘야할 패킷들
	switch (MovePacket->type)
	{
		//이동하는 부분
	case CS_MOVE_PLAYER:
	{
		XMFLOAT3 pickposition{MovePacket->x, 0 ,MovePacket->y };
		g_pScene->GenerateLayEndWorldPosition(pickposition, MovePacket->Character_id);

		g_clients[MovePacket->Character_id].m_targetlocation.x = MovePacket->x;
		g_clients[MovePacket->Character_id].m_targetlocation.y = MovePacket->y;

		for (int i = 0; i < MAX_USER; ++i)
		{
			if (g_clients[i].m_isconnected && i != MovePacket->Character_id) {
				SC_Msg_Target_Location p;
				p.Character_id = MovePacket->Character_id;
				p.location = g_clients[MovePacket->Character_id].m_targetlocation;
				p.size = sizeof(p);
				p.type = SC_CHANGE_TARGET;
				SendPacket(i, &p);
			}
		}
		break;
	}
	case CS_CHANGE_WEAPON:
	{
		g_ppPlayer[WeaponPacket->Character_id]->ChangeWeapon(WeaponPacket->WeaponNum, (ObjectType)WeaponPacket->ObjectType);
		g_pScene->GetShader(1)->SetPlayerAnimation((ObjectType)WeaponPacket->ObjectType, WeaponPacket->Character_id);

		for (int i = 0; i < MAX_USER; ++i)
		{
			if (g_clients[i].m_isconnected) {
				SC_Msg_BroadCast_Change_Weapon p;
				p.Character_id = WeaponPacket->Character_id;
				p.WeaponNum = WeaponPacket->WeaponNum;
				p.ObjectType = WeaponPacket->ObjectType;
				p.size = sizeof(p);
				p.type = SC_CHANGE_WEAPON;
				SendPacket(i, &p);
			}
		}
		break;
	}
	case CS_SET_ABILITY_POINT:
	{
		int idx{};
		for (int i = 0; i < 4; ++i) {
			if (g_ppPlayer[SpeacialPacket->Character_id]->GetPlayerStatus()->Special[i] == (SpecialType::NoSelected)) {
				g_ppPlayer[SpeacialPacket->Character_id]->GetPlayerStatus()->Special[i] = (SpecialType)SpeacialPacket->Ability_Type;
				g_ppPlayer[SpeacialPacket->Character_id]->ApplySpecialStat((SpecialType)SpeacialPacket->Ability_Type);
				idx = i;
				break;
			}
		}
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (g_clients[i].m_isconnected) {
				SC_Msg_Set_Speacial_Point p;
				p.Ability_Type = SpeacialPacket->Ability_Type;
				p.Character_id = SpeacialPacket->Character_id;
				p.idx = (BYTE)idx;
				p.size = sizeof(p);
				p.type = SC_SET_ABILITY_POINT;
				SendPacket(i, &p);
			}
		}
		
		break;
	}
	case CS_DEMAND_USE_SKILL:
	{
		g_ppPlayer[CSkillPacket->Character_id]->ActiveSkill((AnimationsType)CSkillPacket->skilltype);

		for (int i = 0; i < MAX_USER; ++i) {
			if (g_clients[i].m_isconnected) {
				SC_Msg_Permit_Use_Skill p;
				p.Character_id = CSkillPacket->Character_id;
				p.size = sizeof(p);
				p.skilltype = CSkillPacket->skilltype;
				p.type = SC_PERMIT_USE_SKILL;
				SendPacket(i, &p);
			}
		}
		break;
	}
	case CS_PREPARE_DATA:
	{
		g_clients[PreparePacket->Character_id].m_isconnected = true;
		
		SC_Msg_Put_Character p;
		p.Character_id = (BYTE)PreparePacket->Character_id;
		p.size = sizeof(p);
		p.type = SC_PUT_PLAYER;


		//지금 연결된 애한테 4명 어디있는지 
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (i == id) continue;
			p.Character_id = (BYTE)i;
			p.x = g_ppPlayer[i]->GetPosition().x;
			p.y = g_ppPlayer[i]->GetPosition().z;

			SendPacket(id, &p);

		}
		if (g_clients[(BYTE)PreparePacket->Character_id].m_isconnected)
		{
			SC_Msg_Sync_Time p2;
			p2.Game_Time = g_GameTime;
			p2.size = sizeof(p2);
			p2.type = SC_SYNC_TIME;
			SendPacket((BYTE)PreparePacket->Character_id, &p2);
		}

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
	p.Character_id = (BYTE)id;
	p.size = sizeof(p);
	p.type = SC_REMOVE_PLAYER;
	for (int i = 0; i < MAX_USER; ++i) {
		if (false == g_clients[i].m_isconnected) continue;
		if (i == id) continue;
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
	g_loaded[id] = false;
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
			if (false == g_loaded[i]) {
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
		g_loaded[id] = true;
		StartRecv(id);

		SC_Msg_Put_Character p;
		p.Character_id = (BYTE)id;
		p.size = sizeof(p);
		p.type = SC_PUT_PLAYER;
		p.x = g_ppPlayer[id]->GetPosition().x;
		p.y = g_ppPlayer[id]->GetPosition().z;
		SendPacket(id, &p);
	}
}

void timer_thread()
{
	static float PacketCoolTime{ 0.f };
	static float CoolTimeSync{ 0.f };
	static float GameTimeSync{ 0.f };
	static float StatusTimeChecker{ 0.f };

	//if (g_Clientsync)
	while (1)
	{
		Sleep(100);

		if (g_GameTime - StatusTimeChecker >= 60.f)
		{
			StatusTimeChecker = g_GameTime;

			// 미니언 스테이터스 증가 처리
			g_pScene->GetShader(0)->UpdateActiveMinionStatus();

			// 타워 스테이터스 증가 처리
			g_pScene->GetShader(3)->UpdateActiveTowerStatus();

			// 중립 몬스터 스테이터스 증가 처리
			g_pScene->GetShader(2)->UpdateActiveNeutralStatus();
		}
		if (g_GameTime - GameTimeSync >= 5.f) {
			GameTimeSync = g_GameTime;

			for (int i = 0; i < MAX_USER; ++i) {
				if (g_clients[i].m_isconnected) {
					SC_Msg_Sync_Time p2;
					p2.Game_Time = g_GameTime;
					p2.size = sizeof(p2);
					p2.type = SC_SYNC_TIME;
					SendPacket(i, &p2);
				}
			}
		}
		if (g_GameTime - PacketCoolTime >= 0.3f)
		{
			PacketCoolTime = g_GameTime;

			//Sqva User's Information
			for (int i = 0; i < MAX_USER; ++i) {
				XMFLOAT3 playerPos{ g_ppPlayer[i]->GetPosition() };
				g_clients[i].m_x = playerPos.x;
				g_clients[i].m_y = playerPos.z;
				//g_clients[i].m_anistate = g_ppPlayer[i]->GetAnimState();
				//g_clients[i].m_frameTime = g_ppPlayer[i]->GetAnimTimeRemain();
				PlayerInfo* playerStat{ g_ppPlayer[i]->GetPlayerStatus() };
				g_clients[i].m_maxhp = playerStat->maxHP;
				g_clients[i].m_curhp = playerStat->HP;
				g_clients[i].m_level = playerStat->Level;
				g_clients[i].m_maxexp = playerStat->MaxExp;
				g_clients[i].m_exp = playerStat->Exp;
			}

			//Send Every User's Position Packet
			for (int i = 0; i < MAX_USER; ++i) {
				if (g_clients[i].m_isconnected == true) {
					SC_Msg_Pos_Character p;
					p.Character_id = (BYTE)i;
					p.size = sizeof(p);
					p.type = SC_POS;
					p.level = (short)g_clients[i].m_level;
					p.maxexp = (short)g_clients[i].m_maxexp;
					p.exp = (short)g_clients[i].m_exp;
					p.updatetime = g_GameTime;
					p.x = g_clients[i].m_x;
					p.y = g_clients[i].m_y;
					//p.state = (short)g_clients[i].m_anistate;
					//p.frameTime = (short)g_clients[i].m_frameTime;
					for (int j = 0; j < MAX_USER; ++j) {
						if (g_clients[j].m_isconnected == true) {
							SendPacket(j, &p);

						}
					}
				}
			}

			//Send Every User Blue Sword Minion Packet
			for (auto iter = g_blueSwordMinions->begin(); iter != g_blueSwordMinions->end(); ++iter) {
				XMFLOAT3 MinionPos{ (*iter)->GetPosition() };
				CommonInfo* MinionStat{ (*iter)->GetCommonStatus() };
				if (MinionStat) {
					SC_Msg_Pos_Minion p;
					p.size = sizeof(p);
					p.x = MinionPos.x;
					p.y = MinionPos.z;
					p.Minion_Tag = (short)(*iter)->GetTag();
					p.updatetime = g_GameTime;
					p.type = SC_POS_MINION;
					for (int j = 0; j < MAX_USER; ++j) {
						if (g_clients[j].m_isconnected == true) {
							SendPacket(j, &p);
						}
					}
				}
			}
			//Send Every User Blue Bow Minion Packet
			for (auto iter = g_blueBowMinions->begin(); iter != g_blueBowMinions->end(); ++iter) {
				XMFLOAT3 MinionPos{ (*iter)->GetPosition() };
				CommonInfo* MinionStat{ (*iter)->GetCommonStatus() };
				if (MinionStat) {
					SC_Msg_Pos_Minion p;
					p.size = sizeof(p);
					p.x = MinionPos.x;
					p.y = MinionPos.z;
					p.Minion_Tag = (short)(*iter)->GetTag();
					p.updatetime = g_GameTime;
					p.type = SC_POS_MINION;
					for (int j = 0; j < MAX_USER; ++j) {
						if (g_clients[j].m_isconnected == true) {
							SendPacket(j, &p);
						}
					}
				}
			}
			//Send Every User Blue Staff Minion Packet
			for (auto iter = g_blueStaffMinions->begin(); iter != g_blueStaffMinions->end(); ++iter) {
				XMFLOAT3 MinionPos{ (*iter)->GetPosition() };
				CommonInfo* MinionStat{ (*iter)->GetCommonStatus() };
				if (MinionStat) {
					SC_Msg_Pos_Minion p;
					p.size = sizeof(p);
					p.x = MinionPos.x;
					p.y = MinionPos.z;
					p.Minion_Tag = (short)(*iter)->GetTag();
					p.updatetime = g_GameTime;
					p.type = SC_POS_MINION;
					for (int j = 0; j < MAX_USER; ++j) {
						if (g_clients[j].m_isconnected == true) {
							SendPacket(j, &p);
						}
					}
				}
			}
			//Send Every User Red Sword Minion Packet
			for (auto iter = g_redSwordMinions->begin(); iter != g_redSwordMinions->end(); ++iter) {
				XMFLOAT3 MinionPos{ (*iter)->GetPosition() };
				CommonInfo* MinionStat{ (*iter)->GetCommonStatus() };
				if (MinionStat) {
					SC_Msg_Pos_Minion p;
					p.size = sizeof(p);
					p.x = MinionPos.x;
					p.y = MinionPos.z;
					p.Minion_Tag = (short)(*iter)->GetTag();
					p.updatetime = g_GameTime;
					p.type = SC_POS_MINION;
					for (int j = 0; j < MAX_USER; ++j) {
						if (g_clients[j].m_isconnected == true) {
							SendPacket(j, &p);
						}
					}
				}
			}
			//Send Every User Red Bow Minion Packet
			for (auto iter = g_redBowMinions->begin(); iter != g_redBowMinions->end(); ++iter) {
				XMFLOAT3 MinionPos{ (*iter)->GetPosition() };
				CommonInfo* MinionStat{ (*iter)->GetCommonStatus() };
				if (MinionStat) {
					SC_Msg_Pos_Minion p;
					p.size = sizeof(p);
					p.x = MinionPos.x;
					p.y = MinionPos.z;
					p.Minion_Tag = (short)(*iter)->GetTag();
					p.updatetime = g_GameTime;
					p.type = SC_POS_MINION;
					for (int j = 0; j < MAX_USER; ++j) {
						if (g_clients[j].m_isconnected == true) {
							SendPacket(j, &p);
						}
					}
				}
			}
			//Send Every User Red Staff Minion Packet
			for (auto iter = g_redStaffMinions->begin(); iter != g_redStaffMinions->end(); ++iter) {
				XMFLOAT3 MinionPos{ (*iter)->GetPosition() };
				CommonInfo* MinionStat{ (*iter)->GetCommonStatus() };
				if (MinionStat) {
					SC_Msg_Pos_Minion p;
					p.size = sizeof(p);
					p.x = MinionPos.x;
					p.y = MinionPos.z;
					p.Minion_Tag = (short)(*iter)->GetTag();
					p.updatetime = g_GameTime;
					p.type = SC_POS_MINION;
					for (int j = 0; j < MAX_USER; ++j) {
						if (g_clients[j].m_isconnected == true) {
							SendPacket(j, &p);
						}
					}
				}
			}

			//Send Every Neutrality's Position Packet
			for (int i = 0; i < g_NeutralityCount; ++i) {
				XMFLOAT3 NeutralPos{ g_ppNeutrality[i]->GetPosition() };
				
				SC_Msg_Pos_Neutrality p;
				p.Monster_Tag = (short)g_ppNeutrality[i]->GetTag();
				p.x = NeutralPos.x;
				p.y = NeutralPos.z;
				p.size = sizeof(p);
				p.type = SC_POS_MONSTER;
				p.updatetime = g_GameTime;
				for (int j = 0; j < MAX_USER; ++j) {
					if (g_clients[j].m_isconnected == true) {
						SendPacket(j, &p);
					}
				}
				
			}
		}
		if (g_GameTime - CoolTimeSync >= 0.2f)
		{
			CoolTimeSync = g_GameTime;

			for (int i = 0; i < MAX_USER; ++i) {
				if (g_clients[i].m_isconnected == true) {
					g_ppPlayer[i]->SendCoolTime(i);
				}
			}
		}
	}
}

void condition_thread()
{

}