#include "stdafx.h"
#include "Network.h"
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"
#include "05.Objects\03.AnimatedObject\AnimatedObject.h"
#include "05.Objects\08.Player\Player.h"
#include "05.Objects\06.Minion\Minion.h"
#include "05.Objects\09.NexusTower\NexusTower.h"
#include "03.Scenes/00.BaseScene/Scene.h"
#include "04.Shaders/04.AniShader/MinionShader.h"

CNetwork::CNetwork()
{
}


CNetwork::~CNetwork()
{
}

void CNetwork::Initialize(HWND hWnd)
{
	m_hWnd = hWnd;

	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	m_mysocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(MY_SERVER_PORT);
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int Result = WSAConnect(m_mysocket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);
	if (Result)WSAGetLastError();
	WSAAsyncSelect(m_mysocket, m_hWnd, WM_SOCKET, FD_READ);

	m_send_wsabuf.buf = m_send_buffer;
	m_send_wsabuf.len = MAX_BUFF_SIZE;
	m_recv_wsabuf.buf = m_recv_buffer;
	m_recv_wsabuf.len = MAX_BUFF_SIZE;

}

void CNetwork::Finalize()
{
	closesocket(m_mysocket);
}

void CNetwork::ProcessPacket(char *ptr)
{
	static bool first_time = true;

 	if (ptr[1] != 0) {
		switch (ptr[1])
		{
		case SC_PUT_PLAYER:
		{
			SC_Msg_Put_Character *my_packet = reinterpret_cast<SC_Msg_Put_Character *>(ptr);
			int id = my_packet->Character_id;
			if (first_time) {
				first_time = false;
				m_myid = id;
			}
			if (id == m_myid) {
				//자기 아이디 처리
				m_ppPlayer[id]->CBaseObject::SetPosition(my_packet->x, my_packet->y);
			}
			else if (id < NPC_START) { //다른플레이어에게 알려줄때 쓰는거
				//딴 아이디 처리
				m_ppPlayer[id]->CBaseObject::SetPosition(my_packet->x, my_packet->y);
			}
			break;
		}
		/*case SC_PACKET:
		{
			SC_Msg_What_Is_Packet *my_packet = reinterpret_cast<SC_Msg_What_Is_Packet *>(ptr);
			if (my_packet->what == 0) break;
			if (my_packet->what == 1) {
				ReadPacket();
				break;
			}
		}*/
		case SC_POS:
		{
			SC_Msg_Pos_Character *my_packet = reinterpret_cast<SC_Msg_Pos_Character *>(ptr);
			//printf("%d\n", my_packet->Character_id);
			int id = my_packet->Character_id;
			if (first_time) {
				first_time = false;
				m_myid = id;
			}
			if (id == m_myid) {
				if (m_ppPlayer[id]->GetUpdateTime() <= my_packet->updatetime)
				{
					m_ppPlayer[id]->SetPosition(my_packet->x, my_packet->y);
					//m_ppPlayer[id]->SyncAnimation((AnimationsType)my_packet->state, my_packet->frameTime);
					m_ppPlayer[id]->SetUpdateTime(my_packet->updatetime);
					m_ppPlayer[id]->SetLevel(my_packet->level, my_packet->maxexp, my_packet->exp);
				}
			}
			else if (id < NPC_START) { 
				if (m_ppPlayer[id]->GetUpdateTime() <= my_packet->updatetime)
				{
					m_ppPlayer[id]->SetPosition(my_packet->x, my_packet->y);
					//m_ppPlayer[id]->SyncAnimation((AnimationsType)my_packet->state, my_packet->frameTime);
					m_ppPlayer[id]->SetUpdateTime(my_packet->updatetime);
					m_ppPlayer[id]->SetLevel(my_packet->level, my_packet->maxexp, my_packet->exp);
				}
			}
			break;
		}
		case SC_CHANGE_WEAPON:
		{
			SC_Msg_BroadCast_Change_Weapon *my_packet = reinterpret_cast<SC_Msg_BroadCast_Change_Weapon *>(ptr);
			m_ppPlayer[my_packet->Character_id]->GetPlayerStatus()->Weapon = my_packet->WeaponNum;
			break;
		}
		case SC_PERMIT_USE_SKILL:
		{
			SC_Msg_Permit_Use_Skill *my_packet = reinterpret_cast<SC_Msg_Permit_Use_Skill *>(ptr);
			m_ppPlayer[my_packet->Character_id]->ActiveSkill((AnimationsType)my_packet->skilltype);
			break;
		}
		case SC_CHANGE_TARGET:
		{
			SC_Msg_Target_Location *my_packet = reinterpret_cast<SC_Msg_Target_Location *>(ptr);
			int id = my_packet->Character_id;
			m_ppPlayer[id]->SetPathToGo(m_pWayFinder->GetPathToPosition(
				m_ppPlayer[id]->GetPosition(),
				my_packet->location));
			break;
		}
		
		case SC_POS_MONSTER:
		{
			SC_Msg_Pos_Neutrality* my_packet = reinterpret_cast<SC_Msg_Pos_Neutrality*>(ptr);
			CCollisionObject* Monster{ m_pColManager->RequestNeutralByTag(my_packet->Monster_Tag) };
			if ( Monster && Monster->GetUpdateTime() <= my_packet->updatetime )
			{	
				Monster->SetPosition(my_packet->x, my_packet->y);
				Monster->SetUpdateTime(my_packet->updatetime);
			}
			break;
		}
		case SC_MONSTER_STATE:
		{
			SC_Msg_Set_Monster_State* my_packet = reinterpret_cast<SC_Msg_Set_Monster_State*>(ptr);

			CCollisionObject* target{ m_pColManager->RequestNeutralByTag(my_packet->Monster_Tag) };
			target->SetState((StatesType)my_packet->Monster_State, m_pWayFinder);
			break;
		}
		case SC_MONSTER_CHANGE_TEAM:
		{
			SC_Msg_Monster_Ready_to_Attak* my_packet = reinterpret_cast<SC_Msg_Monster_Ready_to_Attak*>(ptr);
			CCollisionObject* Monster{ m_pColManager->RequestNeutralByTag(my_packet->Monster_Tag) };
			Monster->NotifyDamageTeam((TeamType)my_packet->Team_Type);
			Monster->SetWayKind(my_packet->Way_Kind);
			Monster->ReadyToAtk(m_pWayFinder);
			break;
		}
		case SC_MONSTER_RESPAWN:
		{
			SC_Msg_Monster_Respawn* my_packet = reinterpret_cast<SC_Msg_Monster_Respawn*>(ptr);
			CCollisionObject* Roider{ m_pColManager->RequestNeutralByTag(my_packet->Monster_Tag) };
			Roider->Respawn();
			break;
		}
		case SC_POS_MINION:
		{
			SC_Msg_Pos_Minion* my_packet = reinterpret_cast<SC_Msg_Pos_Minion*>(ptr);
		
			CCollisionObject* Minion{ m_pColManager->RequestObjectByTag(my_packet->Minion_Tag) };
			if (Minion && Minion->GetUpdateTime() <= my_packet->updatetime)
			{
				Minion->SetPosition(my_packet->x, my_packet->y);
				Minion->SetUpdateTime(my_packet->updatetime);
			}
			break;
		}
		
		case SC_MINION_SPAWN:
		{
			SC_Msg_Spawn_Minion* my_packet = reinterpret_cast<SC_Msg_Spawn_Minion*>(ptr);
			m_pMinionShader->SpawnMinion((ObjectType)my_packet->Minion_Species, my_packet->Minion_Tag);
			break;
		}
		case SC_MINION_STATE:
		{
			SC_Msg_Set_Minion_State* my_packet = reinterpret_cast<SC_Msg_Set_Minion_State*>(ptr);
			
			CCollisionObject* target{ m_pColManager->RequestObjectByTag(my_packet->Minion_Tag) };
			target->SetState((StatesType)my_packet->Minion_State, m_pWayFinder);
			break;
		}
		case SC_SET_ENEMY:
		{
			SC_Msg_Enemy_Tag* my_packet = reinterpret_cast<SC_Msg_Enemy_Tag*>(ptr);
			
			CCollisionObject* target{ m_pColManager->RequestObjectByTag(my_packet->Minion_Tag) };
			target->SetEnemyByTag(my_packet->Enemy_Tag);
			break;
		}
		case SC_MONSTER_SET_ENEMY:
		{
			SC_Msg_Enemy_Tag_Neutral* my_packet = reinterpret_cast<SC_Msg_Enemy_Tag_Neutral*>(ptr);

			CCollisionObject* target{ m_pColManager->RequestNeutralByTag(my_packet->Monster_Tag) };
			target->SetEnemyByTag(my_packet->Enemy_Tag);
			
			break;
		}

		case SC_BOSS_SET_CURR_ANIM:
		{
			SC_Msg_Boss_Idle_Animation* my_packet = reinterpret_cast<SC_Msg_Boss_Idle_Animation*>(ptr);
			CCollisionObject* Golem{ m_pColManager->RequestNeutralByTag(my_packet->Monster_Tag) };
			Golem->SetAnimation((AnimationsType)my_packet->Animation);
			break;
		}
		case SC_BOSS_SET_NEXT_ANIM:
		{
			SC_Msg_Boss_Idle_Animation* my_packet = reinterpret_cast<SC_Msg_Boss_Idle_Animation*>(ptr);
			CCollisionObject* Golem{ m_pColManager->RequestNeutralByTag(my_packet->Monster_Tag) };

			Golem->SetNextAnimation((AnimationsType)my_packet->Animation);
			break;
		}

		case SC_HP_SYNC:
		{
			SC_Msg_Hp_Sync* my_packet = reinterpret_cast<SC_Msg_Hp_Sync*>(ptr);
			CCollisionObject* Target{ m_pColManager->RequestObjectByTag(my_packet->Target_Tag) };
			
			if (Target && Target->GetUpdateTime() <= my_packet->updatetime)
			{
				Target->SetUpdateTime(my_packet->updatetime);
				Target->SetHP(my_packet->maxhp, my_packet->curhp);
			}
			break;
		}
		case SC_NEXUS_STATE:
		{
			SC_Msg_Set_Nexus_State* my_packet = reinterpret_cast<SC_Msg_Set_Nexus_State*>(ptr);
			CCollisionObject* target{ m_pColManager->RequestObjectByTag(my_packet->Building_Tag) };
			if(target) target->SetState((StatesType)my_packet->Building_State, m_pWayFinder);
			break;
		}
		case SC_BUILDING_SET_ENEMY:
		{
			SC_Msg_Enemy_Tag_Nexus* my_packet = reinterpret_cast<SC_Msg_Enemy_Tag_Nexus*>(ptr);

			CCollisionObject* target{ m_pColManager->RequestObjectByTag(my_packet->Building_Tag) };
			target->SetEnemyByTag(my_packet->Enemy_Tag);
			break;
		}
		case SC_GAME_OVER:
		{
			SC_Msg_Game_Over* my_packet = reinterpret_cast<SC_Msg_Game_Over*>(ptr);
			m_pColManager->GameOver((TeamType)my_packet->Team_Type);

			break;
		}
		case SC_BUILDING_ATTACK:
		{
			SC_Msg_Building_Attack_Enemy* my_packet = reinterpret_cast<SC_Msg_Building_Attack_Enemy*>(ptr);
			CCollisionObject* Building{ m_pColManager->RequestObjectByTag(my_packet->Building_Tag) };
			Building->AttackEnemy();
			break;
		}
		case SC_EXP_UP:
		{
			SC_Msg_Exp_Up* my_packet = reinterpret_cast<SC_Msg_Exp_Up*>(ptr);
			CCollisionObject* Player{ m_pColManager->RequestObjectByTag(my_packet->Target_Tag) };
			PlayerInfo* status{ Player->GetPlayerStatus() };
			status->Exp += my_packet->exp;
			break;
		}
		case SC_LEVEL_UP:
		{
			SC_Msg_Level_Up* my_packet = reinterpret_cast<SC_Msg_Level_Up*>(ptr);
			CCollisionObject* Player{ m_pColManager->RequestObjectByTag(my_packet->Target_Tag) };
			PlayerInfo* status{ Player->GetPlayerStatus() };
			status->Exp -= status->Level * 110 + 170;
			Player->LevelUP(Player);
			break;
		}
		case SC_UPDATE_GOLEM_STAT:
		{
			SC_Msg_Update_Golem_Stat* my_packet = reinterpret_cast<SC_Msg_Update_Golem_Stat*>(ptr);
			CCollisionObject* Golem{ m_pColManager->RequestNeutralByTag(my_packet->Monster_Tag) };
			Golem->SetCommonStatus(my_packet->maxHP, my_packet->atk, my_packet->def);
			break;
		}
		default:
			printf("Unknown PACKET type [%d]\n", ptr[1]);
			break;
		}
	}
	
}


//
void CNetwork::ReadPacket()
{
	DWORD ioflag = 0;
	DWORD iobyte = 0;
	int errorcount = 0;

	int ret = WSARecv(m_mysocket, &m_recv_wsabuf, 1, &iobyte, &ioflag, NULL, NULL);
	//printf("Recv Error [%d]\n" , WSAGetLastError());
	if (ret) {
		int err_code = WSAGetLastError();
		printf("Recv Error [%d]\n", err_code);
		if (err_code == 10035) {
			errorcount += 1;
			//if (errorcount > 3) return; //10035계속 발생하면 걍 리턴 
			ProcessPacket(m_packet_buffer);
		}
	}
	BYTE *ptr = reinterpret_cast<BYTE *>(m_recv_buffer);

	while (0 != iobyte) {
		if (0 == m_in_packet_size) m_in_packet_size = ptr[0];
		if (iobyte + m_saved_packet_size >= m_in_packet_size) {
			memcpy(m_packet_buffer + m_saved_packet_size, ptr, m_in_packet_size - m_saved_packet_size);
			ProcessPacket(m_packet_buffer);
			ptr += m_in_packet_size - m_saved_packet_size;
			iobyte -= m_in_packet_size - m_saved_packet_size;
			m_in_packet_size = 0;
			m_saved_packet_size = 0;
		}
		else {
			memcpy(m_packet_buffer + m_saved_packet_size, ptr, iobyte);
			m_saved_packet_size += iobyte;
			iobyte = 0;
		}
		
	}
	
	
}

void CNetwork::SendPacket(void* ptr)
{
	char *packet = reinterpret_cast<char *>(ptr);
	EXOVER *s_over = new EXOVER;
	s_over->is_recv = false;
	memcpy(s_over->m_iobuf, packet, packet[0]);
	s_over->m_wsabuf.buf = s_over->m_iobuf;
	s_over->m_wsabuf.len = s_over->m_iobuf[0];
	ZeroMemory(&s_over->m_over, sizeof(WSAOVERLAPPED));
	int res = WSASend(m_mysocket, &s_over->m_wsabuf, 1, NULL, 0,
		&s_over->m_over, NULL);
	if (0 != res) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no) printf("Send Error![%d] ", err_no);
	}
}

void CNetwork::SetScene(shared_ptr<CScene> pScene)
{
	m_pScene = pScene;
}

void CNetwork::PrepareData()
{
	m_pMinionShader = (CMinionShader*)m_pScene->GetShader(2);
}
//
//void  CNetwork::err_display(char* msg)
//{
//	LPVOID lpMsgBuf;
//	FormatMessage(
//		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
//		NULL, WSAGetLastError(),
//		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//		(LPTSTR)&lpMsgBuf, 0, NULL);
//
//	DisplayText("[%s] %s\r\n", msg, (LPCTSTR)lpMsgBuf);
//	LocalFree(lpMsgBuf);
//}
//
//
//
//
//
