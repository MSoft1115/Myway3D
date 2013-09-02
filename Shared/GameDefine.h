#pragma once

enum eUserPower
{
	POWER_Player,
	POWER_GM,
};

enum eActorType
{
	ACTOR_Unknown,
	ACTOR_Npc,
	ACTOR_Player,
};

enum eSex
{
	SEX_Female,
	SEX_Male,
};

enum eRace
{
	RACE_Human,			// ����
	RACE_FoxEvil,		// ����
	RACE_Ghost,			// ����
	RACE_Spirit,		// ����
};

enum eEquipment
{
	EQUIP_Weapon,		// ����
	EQUIP_Helmet,		// ͷ��
	EQUIP_Shoulder,		// ���
	EQUIP_Clothes,		// �·�
	EQUIP_Shoes,		// Ь��

	EQUIP_Max,
};

#define PLAYER_ID_MAGIC 0x80000000

struct PlayerViewInfo
{
	TString32 Name;			// ����
	int Sex;				// �Ա�  --- eSex
	int Race;				// ����  --- eRace

	int ActorId;			// ��ɫId

	int HP;					// ����ֵ
	int MP;					// ħ��ֵ

	//int Equip[EQUIP_Max];	// ���ϵ�װ��

	Vec3 Position;			// λ��
	float Direction;		// ����
};


struct NpcViewInfo
{
	int ActorId;
	int TypeId;

	int HP;
	int MP;

	Vec3 Position;
	float Direction;

	NpcViewInfo()
		: ActorId(-1)
		, TypeId(-1)
		, HP(0)
		, MP(0)
		, Position(0, 0, 0)
		, Direction(0)
	{
	}
};

enum eActorState
{
	STATE_Idle,

	STATE_Fight,
};