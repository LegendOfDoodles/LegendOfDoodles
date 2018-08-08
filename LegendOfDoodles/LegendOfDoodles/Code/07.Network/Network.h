#include "protocol.h"
#include "Winsock2.h"


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
	HWND m_hWnd{ NULL };
	
	SOCKET  m_mysocket;
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
	CBaseObject** m_ppPlayer{ NULL };
	CBaseObject** m_ppBlueMinions{ NULL };
	CBaseObject** m_ppRedMinions{ NULL };
	int*		  m_pnBlue{ 0 };
	int*		  m_pnRed{ 0 };
	CBaseObject** m_ppNexusTower{ NULL };
	

public:
	CNetwork();
	~CNetwork();
	void Initialize();
	void ProcessPacket(int myid, char *ptr);
	void Finalize();
	void ReadPacket(SOCKET sock);
	void SendPacket(int id, void *ptr);

	void SetPlayers(CBaseObject** player) { m_ppPlayer = player; }
	void SetBlueMinions(CBaseObject** blue) { m_ppBlueMinions = blue; }
	void SetRedMinions(CBaseObject** red) { m_ppRedMinions = red; }
	void SetBlueCount(int* cnt) { m_pnBlue = cnt; }
	void SetRedCount(int* cnt) { m_pnRed = cnt; }
	void SetNexusTowers(CBaseObject** nexus) { m_ppNexusTower = nexus; }
};

