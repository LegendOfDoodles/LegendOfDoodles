#pragma once

/// <summary>
/// 목적: 여러 지점에서 사용할 나열형 데이터를 통합 관리한다.
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-08-03
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
		SkillW,
		SkillE,
		SkillR,
		Win,
		Defeat,
		Defeat2,
		SpecialAttack1,
		SpecialAttack2,
		IdleToSit,
		SitToIdle,
		Sit
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

namespace Way_Species
{
	enum Way_Species {
		Blue_Up,
		Blue_Down,
		Red_Up,
		Red_Down,
		Roider_Exit,
		Golem_Exit
	};
}
typedef Way_Species::Way_Species WayType;

enum ObjectType {
	StickPlayer,
	SwordPlayer,
	StaffPlayer,
	BowPlayer,
	SwordMinion,
	StaffMinion,
	BowMinion,
	Roider,
	CAM,
	GOLEM,
	FirstTower,
	SecnondTower,
	Nexus,
	FlyingObject,
	EffectObject
};

enum FlyingObjectType {
	Roider_Dumbel,
	Minion_Arrow,
	Minion_Magic,
	BlueTower_Attack,
	RedTower_Attack,
	Player_Arrow,
	Player_MagicSkill_Q,
	Player_ArrowSkill_Q,
	Player_ArrowSkill_W,
	Player_ArrowSkill_E,
	Player_ArrowSkill_R,
	Player_MagicSkill_R,
	Player_Magic
};

enum EffectObjectType {
	Player_SwordSkill_Q,
	Player_SwordSkill_W,
	Player_SwordSkill_E,
	Player_SwordSkill_R,
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


enum UIFrameType {
	MinimapFrame,
	KDAFrame,
	TimerFrame,
	SkillFrameQ,
	SkillFrameW,
	SkillFrameE,
	SkillFrameR,
	StatusFrame,
	CharacterFrame,
	SpecialFrame,
	SelectSpecial_7,
	SelectSpecial_12,
	SelectSpecial_17,
	SelectSpecial_21,
	CharacterFrameHP,
	CharacterFrameMP,
	Minimap,
};

enum SpecialType {
	AttackSpecial,
	DefenceSpecial,
	TechnicSpecial,
	NoSelected
};

enum SpeedType {
	WalkSpeed,
	AttackSpeed
};

struct PlayerInfo
{
	float maxHP{ 1500 };
	float HP{ maxHP };

	float maxMP{ 500 };
	float MP{ 150 };

	float Atk{ 150 };
	float Def{ 60 };

	float QSkillCoolTime{ COOLTIME_SKILL_Q };
	float WSkillCoolTime{ COOLTIME_SKILL_W };
	float ESkillCoolTime{ COOLTIME_SKILL_E };
	float RSkillCoolTime{ COOLTIME_SKILL_R };

	float QSkillPower{ 1.0f };
	float WSkillPower{ 1.0f };
	float ESkillPower{ 1.0f };
	float RSkillPower{ 1.0f };

	float AtkSpeed{ 1.0f };
	float WalkSpeed{ 1.0f };

	float AtkRange{ 1.0f };
	float QSkillRange{ 1.0f };
	float WSkillRange{ 1.0f };
	float ESkillRange{ 1.0f };
	float RSkillRange{ 1.0f };

	//0. 몽둥이	1. 검	2. 마법	3.	활
	UINT Weapon{ 0 };

	UINT WeaponNum{ 0 };

	UINT Kill{ 0 };
	UINT Death{ 0 };
	UINT Assist{ 0 };

	UINT Level{ 0 };

	UINT MaxExp{ 170 };
	UINT Exp{ 0 };

	UINT SpecialPoint{ 0 };

	// 특성 7 ~ 21 4개
	SpecialType Special[4] = { SpecialType::NoSelected, SpecialType::NoSelected, SpecialType::NoSelected, SpecialType::NoSelected };
};


enum NumberType {
	BlueTeamKill,
	RedTeamKill,
	TimeMinute,
	TimeSec,
	PersonalKill,
	PersonalDeath,
	PersonalAssist,
};

enum SkillUIType {
	GrayQSkill,
	GrayWSkill,
	GrayESkill,
	GrayRSkill,
	QSkill,
	WSkill,
	ESkill,
	RSkill
};

enum GaugeUIType {
	PlayerGauge,
	MinionGauge,
	NexusAndTower,
	RoiderGauge,
};

enum IconUIType {
	PlayerIcon,
	MinionIcon,
	NexusAndTowerIcon,
	RoiderIcon
};

enum class PathType {
	Main,
	Sub
};