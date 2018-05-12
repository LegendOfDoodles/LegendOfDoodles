#pragma once

/// <summary>
/// 목적: 여러 지점에서 사용할 나열형 데이터를 통합 관리한다.
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-05-11
/// </summary>

namespace States
{
	enum States {
		Idle,
		Walk,
		Chase,
		Attack,
		Die,
		Remove,
		Win,
		Defeat
	};

	enum ProcessStates
	{ 
		Processing, 
		Found, 
		Not_Found, 
		Done 
	};
}

namespace Animations
{
	enum Animations {
		Idle,
		Attack1,
		Attack2,
		StartWalk,
		Walking,
		Die,
		SkillQ,
		SkillE,
		SkillR,
		Win,
		Defeat,
		Defeat2
	};
}

typedef States::States StatesType;
typedef States::ProcessStates ProcessType;
typedef Animations::Animations AnimationsType;

enum StaticType {
	Static,
	Move
};

enum TeamType {
	Blue,
	Red,
	Neutral,
	None,
};

enum CollisionType {
	SPHERE,
	SECTERFORM,
};

enum Minion_Species {
	Blue_Up,
	Blue_Down,
	Red_Up,
	Red_Down,
	Data_Prepare
};

enum ObjectType {
	StickPlayer,
	SwordPlayer,
	StaffPlayer,
	BowPlayer,
	SwordMinion,
	StaffMinion,
	BowMinion,
	Loyde,
	CAM,
	GOLEM,
	FirstTower,
	SecnondTower,
	Nexus
};

struct PlayerInfo
{
	float maxHP{ 500 };
	float HP{ maxHP };

	float maxMP{ 500 };
	float MP{ 150 };

	float Atk{ 50 };
	float Def{ 20 };

	UINT Level{ 0 };

	UINT MaxExp{ 170 };
	UINT Exp{ 0 };
};

struct CommonInfo
{
	float maxHP;
	float HP;

	float Atk;
	float Def;

	UINT Exp;
};

struct StaticInfo
{
	float maxHP;
	float HP;

	float Atk;
	float Def;

	UINT Exp;
};