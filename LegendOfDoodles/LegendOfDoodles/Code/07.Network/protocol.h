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
struct CS_Msg_Change_Weapon : public Packet
{
	BYTE Character_id;
	BYTE WeaponNum;
	short ObjectType;
};

struct CS_Msg_Set_Speacial_Point : public Packet
{
	BYTE Character_id;
	BYTE Ability_Type;
};
struct SC_Msg_Set_Speacial_Point : public Packet
{
	BYTE Character_id;
	BYTE Ability_Type;
	BYTE idx;
};

struct SC_Msg_BroadCast_Change_Weapon :public Packet
{
	BYTE Character_id;
	BYTE WeaponNum;
	short ObjectType;
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

struct CS_Msg_Prepare_Data : public Packet
{
	BYTE Character_id;
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
struct SC_Msg_Pos_Neutrality : public Packet
{
	short Monster_Tag;
	float x;
	float y;
	float updatetime;
};

struct SC_Msg_Set_Monster_State : public Packet
{
	BYTE Monster_State;
	short Monster_Tag;
};

struct SC_Msg_Monster_Ready_to_Attak : public Packet
{
	short Monster_Tag;
	BYTE Team_Type;
	BYTE Way_Kind;
};

struct SC_Msg_Monster_Respawn : public Packet
{
	short Monster_Tag;
};

struct SC_Msg_Boss_Idle_Animation : public Packet
{
	short Monster_Tag;
	BYTE Animation;
};

struct SC_Msg_Pos_Minion : public Packet
{
	short Minion_Tag;
	float x;
	float y;
	float atk;
	float def;
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

struct SC_Msg_Update_Golem_Stat : public Packet
{
	short Monster_Tag;
	float maxHP;
	float atk;
	float def;
};

struct SC_Msg_Update_Tower_Stat : public Packet
{
	short Tower_Tag;
	float atk;
	float def;
};


struct SC_Msg_Enemy_Tag_Neutral : public Packet
{
	short Monster_Tag;
	short Enemy_Tag;
};

struct SC_Msg_Hp_Sync : public Packet
{
	float maxhp;
	float curhp;
	float updatetime;
	short Target_Tag;
	short Object_Type;
	short Flying_Type;
};

struct SC_Msg_Set_Nexus_State : public Packet
{
	short Building_Tag;
	BYTE Building_State;
};
struct SC_Msg_Enemy_Tag_Nexus : public Packet
{
	short Building_Tag;
	short Enemy_Tag;
 };

struct SC_Msg_Game_Over :public Packet
{
	BYTE Team_Type;
};

struct SC_Msg_Building_Attack_Enemy : public Packet
{
	short Building_Tag;
};

struct SC_Msg_Exp_Up : public Packet
{
	short exp;
	short Target_Tag;
};

struct SC_Msg_Level_Up : public Packet
{
	short level;
	short Target_Tag;
};

struct SC_Msg_Player_Respawn : public Packet
{
	short Target_Tag;
};

struct SC_Msg_Player_Missile : public Packet
{
	short Target_Tag;
	short Missile_Type;
};

struct SC_Msg_Cooltime_Percent : public Packet
{
	short Target_Tag;
	float QPercentage;
	float WPercentage;
	float EPercentage;
	float RPercentage;
};

struct SC_Msg_Change_Speed : public Packet
{
	short Target_Tag;
	BYTE Speed_Type;
	float Changed_Speed;
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
#define SC_PLAYER_RESPAWN		  1
#define SC_PLAYER_MISSILE		  2
#define SC_UPDATE_TOWER_STAT	  3
#define SC_COOLTIME				  4
#define SC_CHANGE_SPEED			  5

#define SC_CONNECT_PLAYER		  101
#define SC_PUT_PLAYER			  102
#define SC_REMOVE_PLAYER		  103
#define SC_POS					  104
#define SC_PERMIT_USE_SKILL		  105
#define SC_POS_MINION			  106
#define SC_HP_SYNC				  107
//#define SC_PACKET				  115
#define SC_CHANGE_TARGET		  108
#define SC_MINION_SPAWN			  109
#define SC_MINION_STATE			  110
#define SC_SET_ENEMY			  111
#define SC_POS_MONSTER			  112
#define SC_MONSTER_STATE		  113
#define SC_MONSTER_CHANGE_TEAM	  114
#define SC_MONSTER_RESPAWN		  115
#define SC_MONSTER_SET_ENEMY	  116
#define SC_BOSS_SET_CURR_ANIM	  117
#define SC_BOSS_SET_NEXT_ANIM	  118
#define SC_NEXUS_STATE			  119
#define SC_BUILDING_SET_ENEMY	  120
#define SC_GAME_OVER			  121
#define SC_BUILDING_ATTACK		  122
#define SC_EXP_UP				  123
#define SC_LEVEL_UP				  124
#define SC_CHANGE_WEAPON		  125
#define SC_UPDATE_GOLEM_STAT	  126
#define SC_SET_ABILITY_POINT      127

//Client->Server
#define CS_ACTION				  201
#define CS_MOVE_PLAYER			  202
#define CS_DEMAND_USE_SKILL		  203
#define CS_CHANGE_WEAPON		  204
#define CS_SET_ABILITY_POINT      205
#define CS_PREPARE_DATA			  206





