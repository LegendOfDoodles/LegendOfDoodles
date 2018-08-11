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
	short x;
	short y;
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
	float x;
	float y;
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

struct SC_Msg_Pos_Character : public Packet
{
	BYTE Character_id;
	short x;
	short y;
	short maxhp;
	short curhp;
	short state;
	short frameTime;
	short weapon;
	short level;
	short maxexp;
	short exp;
	XMFLOAT3 vLook;
};
struct SC_Msg_What_Is_Packet : public Packet
{
	BYTE what;
};
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

struct SC_Msg_Minion_Count : public Packet
{
	BYTE color;
	short count;
};

struct SC_Msg_Pos_Minion : public Packet
{
	BYTE color;
	short x;
	short y;
	short maxhp;
	short curhp;
	short state;
	short frameTime;
	XMFLOAT3 vLook;
};
struct SC_Msg_Spawn_Minion : public Packet
{
	BYTE Minion_Species;
	short Minion_Tag;
};
struct SC_Msg_Set_Minion_State : public Packet
{
	BYTE Team_Type;
	int Minion_Species;
	int Minion_Tag;
	BYTE Minion_State;
};
struct SC_Msg_Pos_Nexus : public Packet
{
	BYTE Object_id;
	XMFLOAT3 vPos;
	short maxhp;
	short curhp;
};

struct CS_Msg_Demand_Change_Weapon : public Packet
{
	BYTE Character_id;
};

struct SC_Msg_Login_Time : public Packet
{
	BYTE Character_id;
	//system_clock::time_point Duration_time;
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
#define SC_MOVE_PLAYER			  104
#define SC_ATTACK				  105
#define SC_HIT					  106
#define SC_PUT_MONSTER			  107
#define SC_PUT_MINION			  108
#define SC_PERMIT_MAKE_ROOM		  109
#define SC_POS					  110
#define SC_PERMIT_USE_SKILL		  111
#define SC_POS_MINION			  112
#define SC_MINION_COUNT			  113
#define SC_POS_NEXUS			  114
#define SC_PACKET				  115
#define SC_CHANGE_TARGET		  116
#define SC_MINION_SPAWN			  117
#define SC_MINION_STATE			  118
//Client->Server
#define CS_ACTION				  201
#define CS_MOVE_PLAYER			  202
#define CS_COLLISION			  203
#define CS_DAMAND_MAKE_ROOM		  204
#define CS_PUT_MINION			  205
#define CS_DELETE_MINION		  206
#define CS_DEMAND_USE_SKILL		  207
#define CS_DEMAND_CHANGE_WEAPON   208
#define CS_DEMAND_SYNC			  209
//In Client Move Object
#define CS_UP					  1
#define CS_DOWN					  2
#define CS_LEFT					  3
#define CS_RIGHT				  4




//enum 추가할 예정


