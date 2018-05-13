#pragma once
#pragma pack(push,1)

#include <windows.h>

struct Packet {
	
	BYTE size;
	short type;
};
//규칙
//SC -> 서버에서 클라 CS-> 클라에서 서버
//Ch -> 캐릭터 Mo -> 중립몬스터, 미니언
struct SC_MsgConnect : public Packet
{
	BYTE client_id;
	BYTE Character_id;
};
struct SC_Msg_Put_Character : public Packet
{
	BYTE Character_id;
	short x;
	short y;
};
struct SC_MsgMoCreate : public Packet
{
	BYTE Monster_id;
	short hp;
};
struct CS_MsgMoCreate : public Packet
{

};

struct SC_Msg_Remove_Character : public Packet
{
	BYTE Character_id;
};
struct CS_MsgMoDelete : public Packet
{
	BYTE Monster_id;
};
struct SC_MsgChMove : public Packet
{
	BYTE Character_id;
	short x;
	short y;
};
struct SC_MsgMoMove : public Packet
{
	BYTE Monster_id;
	short x;
	short y;
};
struct SC_MsgChAttack : public Packet
{
	BYTE Character_id;
	//배열로 처리해야 할때도 있을듯?
	//다중스킬!
	BYTE Monster_id;
};
struct SC_MsgMoAttack : public Packet
{
	BYTE Monster_id;
	BYTE Character_id;
};
struct SC_MsgChHit : public Packet
{
	BYTE Character_id;
	short Ch_Health;
};
struct SC_MsgMoHit : public Packet
{
	BYTE Monster_id;
	short Mo_Health;
};

struct CS_MsgChMove : public Packet
{
	BYTE Character_id;
	short x;
	short y;
};

struct CS_MsgChCollision : public Packet
{
	BYTE Character_id;
	short x;
	short y;
};

struct CS_MsgDemandMakeRoom : public Packet
{
	BYTE Character_id;

};
#pragma pack(pop)

#define MAX_BUFF_SIZE 4000
#define MAX_PACKET_SIZE 255

#define MAP_WIDTH 10000
#define MAP_HEIGHT 10000

#define MAX_USER 4
#define NPC_START 1000
#define NUM_OF_NPC 10000
#define VIEW_RADIUS 500

#define MY_SERVER_PORT 4000
#define WM_SOCKET		WM_USER+1
//enum class Type : BYTE { Connect, Create, Delete, Move, Attack, Hit };
//Server-> Client
#define SC_CONNECT_PLAYER		  101
#define SC_PUT_PLAYER			  102
#define SC_REMOVE_PLAYER		  103
#define SC_MOVE_PLAYER			  104
#define SC_ATTACK				  105
#define SC_HIT					  106
#define SC_PUT_MONSTER			  107
#define SC_PUT_MINION			  108
#define SC_PERMIT_MAKE_ROOM		  109
//Client->Server
#define CS_ACTION				  201
#define CS_MOVE_PLAYER			  202
#define CS_COLLISION			  203
#define CS_DAMAND_MAKE_ROOM		  204
#define CS_PUT_MINION			  205
#define CS_DELETE_MINION		  206
//In Client Move Object
#define CS_UP					  1
#define CS_DOWN					  2
#define CS_LEFT					  3
#define CS_RIGHT				  4




//enum 추가할 예정


