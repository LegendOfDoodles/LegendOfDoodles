#include <windows.h>

#pragma once
#pragma pack(push,1)
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
	float x;
	float y;
};
struct SC_Msg_Target_Location : public Packet
{
	BYTE Character_id;
	XMFLOAT2 location;
};
struct CS_Msg_Demand_Sync : public Packet
{
	BYTE Sync_on;
};
struct SC_Msg_Pos_Character : public Packet
{
	BYTE Character_id;
	float x;
	float y;
	float maxhp;
	float curhp;
	short state;
	short frameTime;
	short weapon;
	short level;
	short maxexp;
	short exp;
	float updatetime;
	XMFLOAT3 vLook;
};
struct CS_MsgChMove :public Packet
{
	BYTE Character_id;
	float x;
	float y;
};
struct SC_Msg_Remove_Character : public Packet
{
	BYTE Character_id;
};
//struct SC_Msg_What_Is_Packet : public Packet
//{
//	BYTE what;
//};
struct CS_Msg_Demand_Pos_Character : public Packet
{
	BYTE Character_id;
	float x;
	float y;
};

struct CS_Msg_Demand_Use_Skill : public Packet
{
	BYTE Character_id;
	BYTE skilltype;
};
struct SC_Msg_Permit_Use_Skill : public Packet
{
	BYTE Character_id;
	BYTE skilltype;
};



struct SC_Msg_Pos_Minion : public Packet
{
	short Minion_Tag;
	float x;
	float y;
	float maxhp;
	float curhp;
	float updatetime;
	XMFLOAT3 vLook;
};
struct SC_Msg_Spawn_Minion : public Packet
{
	BYTE Minion_Species;
	short Minion_Tag;
};
struct SC_Msg_Set_Minion_State : public Packet
{
	short Minion_Tag;
	BYTE Minion_State;
};
struct SC_Msg_Enemy_Tag : public Packet
{
	short Minion_Tag;
	short Enemy_Tag;
};
struct SC_Msg_Pos_Nexus : public Packet
{
	BYTE Object_id;
	XMFLOAT3 vPos;
	float maxhp;
	float curhp;
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

//enum class Type : BYTE { Connect, Create, Delete, Move, Attack, Hit };
//Server-> Client
#define SC_CONNECT_PLAYER		  101
#define SC_PUT_PLAYER			  102
#define SC_REMOVE_PLAYER		  103
#define SC_POS					  104
#define SC_PERMIT_USE_SKILL		  105
#define SC_POS_MINION			  106
#define SC_POS_NEXUS			  107
//#define SC_PACKET				  115
#define SC_CHANGE_TARGET		  108
#define SC_MINION_SPAWN			  109
#define SC_MINION_STATE			  110
#define SC_SET_ENEMY			  111
//Client->Server
#define CS_ACTION				  201
#define CS_MOVE_PLAYER			  202
#define CS_DEMAND_USE_SKILL		  203






