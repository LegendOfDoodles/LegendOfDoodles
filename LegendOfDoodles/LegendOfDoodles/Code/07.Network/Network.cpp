#include "stdafx.h"
#include "Network.h"
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"
#include "05.Objects\03.AnimatedObject\AnimatedObject.h"
#include "05.Objects\08.Player\Player.h"
#include "05.Objects\06.Minion\Minion.h"
#include "05.Objects\09.NexusTower\NexusTower.h"
#include "03.Scenes/00.BaseScene/Scene.h"

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
	WSAGetLastError();
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

void CNetwork::ProcessPacket(int myid, char *ptr)
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
				m_pScene->SetPlayer();
			}
			if (id == m_myid) {
				//자기 아이디 처리
				m_ppPlayer[id]->CBaseObject::SetPosition(my_packet->x, my_packet->y);
			}
			else if (id < NPC_START) { //다른플레이어에게 알려줄때 쓰는거
				//딴 아이디 처리
				m_ppPlayer[id]->CBaseObject::SetPosition(my_packet->x, my_packet->y);
			}
			
			//else { //미니언, 몬스터 관리할때 쓰는거
			//npc[id - NPC_START].x = my_packet->x;
			//npc[id - NPC_START].y = my_packet->y;
			//npc[id - NPC_START].attr |= BOB_ATTR_VISIBLE;
			//}
			break;
		}
		case SC_PACKET:
		{
			SC_Msg_What_Is_Packet *my_packet = reinterpret_cast<SC_Msg_What_Is_Packet *>(ptr);
			if (my_packet->what == 0) break;
			if (my_packet->what == 1) {
				ReadPacket();
				break;
			}
		}
		case SC_POS:
		{
			SC_Msg_Pos_Character *my_packet = reinterpret_cast<SC_Msg_Pos_Character *>(ptr);
			printf("%d\n", my_packet->Character_id);
			int id = my_packet->Character_id;
			if (first_time) {
				first_time = false;
				m_myid = id;
			}
			if (id == m_myid) {
				//자기 아이디 처리
				//printf("CHECK");
				m_ppPlayer[id]->CBaseObject::SetPosition(my_packet->x, my_packet->y);
				m_ppPlayer[id]->SyncAnimation((AnimationsType)my_packet->state, my_packet->frameTime);
				m_ppPlayer[id]->SetHP(my_packet->maxhp, my_packet->curhp);
				m_ppPlayer[id]->SetLevel(my_packet->level, my_packet->maxexp, my_packet->exp);
				//dynamic_cast<CPlayer*>(m_ppPlayer[id])->SetWeapon(my_packet->weapon);
			}
			else if (id < NPC_START) { 
				m_ppPlayer[id]->CBaseObject::SetPosition(my_packet->x, my_packet->y);
				m_ppPlayer[id]->SyncAnimation((AnimationsType)my_packet->state, my_packet->frameTime);
				m_ppPlayer[id]->SetHP(my_packet->maxhp, my_packet->curhp);
				m_ppPlayer[id]->SetLevel(my_packet->level, my_packet->maxexp, my_packet->exp);
				//dynamic_cast<CPlayer*>(m_ppPlayer[id])->SetWeapon(my_packet->weapon);
			}
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
			printf("%d\n", my_packet->Character_id);
			int id = my_packet->Character_id;
			m_ppPlayer[id]->SetPathToGo(m_pWayFinder->GetPathToPosition(
				m_ppPlayer[id]->GetPosition(),
				my_packet->location));
		}
		case SC_PUT_MINION:
		{
			/*SC_MsgMoCreate *my_packet = reinterpret_cast<SC_MsgMoCreate *>(ptr);
			int id = my_packet->Monster_id;
			if (first_time) {
				first_time = false;
			}
			else {
				
			}*/

			break;
		}

		case SC_MOVE_PLAYER:
		{
			//SC_MsgChMove *my_packet = reinterpret_cast<SC_MsgChMove *>(ptr);
			//int other_id = my_packet->Character_id;
			//if (other_id == m_myid) {	
			///*	m_ppObject[m_myid] = object;
			//	m_ppObject[m_myid]->SetPosition(my_packet->x, my_packet->y);
			//	printf("Client[%d] Set Position\n", other_id);*/
			//	//player.x = my_packet->x;
			//	//player.y = my_packet->y;
			//	
			//}
			//else if (other_id < NPC_START) {
			//	m_ppObject[other_id]->SetPosition(my_packet->x, my_packet->y);
			//}
			///*
			//else {
			//npc[other_id - NPC_START].x = my_packet->x;
			//npc[other_id - NPC_START].y = my_packet->y;
			//}*/
			break;
		}

		case SC_REMOVE_PLAYER:
		{
			//SC_Msg_Remove_Character *my_packet = reinterpret_cast<SC_Msg_Remove_Character *>(ptr);
			//int other_id = my_packet->Character_id;
			//if (other_id == myid) {

			//}
			//else if (other_id < NPC_START) {
			//	//skelaton[other_id].attr &= //안보이게 처리;
			//}
			//else {
			//	//npc[other_id - NPC_START].attr &= //안보이게 처리;
			//}
			break;
		}
		/*case SC_POS_MINION:
		{
			SC_Msg_Pos_Minion* my_packet = reinterpret_cast<SC_Msg_Pos_Minion*>(ptr);
			if (my_packet->color == 1) {
				m_ppBlueMinions[m_minon_index]->CBaseObject::SetPosition(my_packet->x, my_packet->y);
				dynamic_cast<CAnimatedObject*>(m_ppBlueMinions[m_minon_index])->SetAnimation((AnimationsType)my_packet->state, (float)my_packet->frameTime);
				dynamic_cast<CAnimatedObject*>(m_ppBlueMinions[m_minon_index])->RegenerateWorldMatrixWithLook(my_packet->vLook);
				dynamic_cast<CMinion*>(m_ppBlueMinions[m_minon_index++])->SetMaxHP(my_packet->maxhp, my_packet->curhp);

			}
			else {
				m_ppRedMinions[m_minon_index]->CBaseObject::SetPosition(my_packet->x, my_packet->y);
				dynamic_cast<CAnimatedObject*>(m_ppRedMinions[m_minon_index])->SetAnimation((AnimationsType)my_packet->state, (float)my_packet->frameTime);
				dynamic_cast<CAnimatedObject*>(m_ppRedMinions[m_minon_index])->RegenerateWorldMatrixWithLook(my_packet->vLook);
				dynamic_cast<CMinion*>(m_ppRedMinions[m_minon_index++])->SetMaxHP(my_packet->maxhp, my_packet->curhp);
			}
			break;
		}*/
		case SC_MINION_COUNT:
		{
			m_minon_index = 0;
			SC_Msg_Minion_Count* my_packet = reinterpret_cast<SC_Msg_Minion_Count*>(ptr);
			if (my_packet->color == 1) {
				*m_pnBlue = my_packet->count;
			}
			else {
				*m_pnRed = my_packet->count;
			}
			break;
		}
		case SC_POS_NEXUS:
		{
			SC_Msg_Pos_Nexus* my_packet = reinterpret_cast<SC_Msg_Pos_Nexus*>(ptr);
			m_ppNexusTower[my_packet->Object_id]->SetPosition(my_packet->vPos);
			//dynamic_cast<CNexusTower*>(m_ppNexusTower[my_packet->Object_id])->SetMaxHP(my_packet->maxhp, my_packet->curhp);
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
			ProcessPacket(m_myid, m_packet_buffer);
		}
	}
	BYTE *ptr = reinterpret_cast<BYTE *>(m_recv_buffer);

	while (0 != iobyte) {
		if (0 == m_in_packet_size) m_in_packet_size = ptr[0];
		if (iobyte + m_saved_packet_size >= m_in_packet_size) {
			memcpy(m_packet_buffer + m_saved_packet_size, ptr, m_in_packet_size - m_saved_packet_size);
			ProcessPacket(m_myid, m_packet_buffer);
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
//
////void Network::SendMovePacket(WPARAM wParam, CBaseObject* object)
////{
////	int x = 0, y = 0;
////	if (wParam == VK_RIGHT)	x += 1;
////	if (wParam == VK_LEFT)	x -= 1;
////	if (wParam == VK_UP)	y -= 1;
////	if (wParam == VK_DOWN)	y += 1;
////	CS_MsgChMove *my_packet = reinterpret_cast<CS_MsgChMove *>(m_send_buffer);
////	my_packet->size = sizeof(my_packet);
////	m_send_wsabuf.len = sizeof(my_packet);
////	DWORD iobyte;
////	if (0 != x) {
////		if (1 == x) my_packet->type = CS_RIGHT;
////		else my_packet->type = CS_LEFT;
////		int ret = WSASend(m_mysocket, &m_send_wsabuf, 1, &iobyte, 0, NULL, NULL);
////		if (ret) {
////			int error_code = WSAGetLastError();
////			printf("Error while sending packet [%d]", error_code);
////		}
////	}
////	if (0 != y) {
////		if (1 == y) my_packet->type = CS_DOWN;
////		else my_packet->type = CS_UP;
////		WSASend(m_mysocket, &m_send_wsabuf, 1, &iobyte, 0, NULL, NULL);
////	}
////	ReadPacket(m_mysocket,object);
////}
//
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
