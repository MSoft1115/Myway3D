#pragma once

#include "terrain.h"

namespace xmdl {

#define ZONE_FLAG			*(DWORD*)"ZONE"
#define ZONE_VER			1
#define ZONE_TERRAIN_TILE	*(DWORD*)"TERT"
#define ZONE_TERRAIN_BIG	*(DWORD*)"TERB"
#define ZONE_SCENE			*(DWORD*)"SCNE"
#define ZONE_INDOOR			*(DWORD*)"INDR"
#define ZONE_COLLOSION		*(DWORD*)"CLSN"
#define ZONE_FLORA			*(DWORD*)"FLRA"

#define SCENE_STRING		*(DWORD*)"TSTR"
#define SCENE_MDL_TEMPLATE	*(DWORD*)"MDLT"
#define SCENE_SND_TEMPLATE	*(DWORD*)"SNDT"
#define SCENE_MDL_INSTANCE	*(DWORD*)"INST"
#define SCENE_LIT_INSTANCE	*(DWORD*)"LINS"
#define SCENE_SND_INSTANCE	*(DWORD*)"SINS"
#define SCENE_MDL_VCOLOR	*(DWORD*)"VCLR"
#define SCENE_DECALS		*(DWORD*)"DCLS"

	enum ScnObjType
	{
		ScnObj_Unknown= -1,				// δ֪����
		ScnObj_Model,					// ����ģ��
		ScnObj_Light,					// �����ƹ�
		ScnObj_Sound,					// ������Ч
		ScnObj_Decal,					// ��������
		ScnObj_Flora,					// ����ֲ��
	};

	struct ScnObj_F
	{
		ScnObjType m_eType;				// �������������
		UINT	m_nTemplate;			// ģ��ID
		DWORD	m_dwGameID;				// �߼�����ID
		DWORD	m_dwUID;				// ʵ��ID
		Mat4	m_tmWorld;				// �ռ����
		UINT	m_nVCSize;				// ����ɫ������
		UINT	m_nVCOffset;			// �ڶ���ɫ�����е�ƫ����
		Vec3	m_vBoxMin;				// �߽��
		Vec3	m_vBoxMax;				// 
	};


	class t_zone
	{
	public:
		t_zone(t_scene * scene);
		~t_zone();

		void load(const char * filename, t_terrain * terrain);

		Aabb import();

	protected:
		void loadScene(File & file, int size);

	protected:
		t_scene * mScene;

		int mModelNameSize;
		char * mModelNames;

		int mObjectCount;
		ScnObj_F* mObjectsF;
	};


#define STAGE_FLAG		*(DWORD*)"STAG"
#define STAGE_VER		3
#define STAGE_VER4      4
#define STAGE_INFO		*(DWORD*)"INFO"	
#define STAGE_ENV		*(DWORD*)"ENVM"
#define STAGE_GLOW		*(DWORD*)"GLOW"
#define STAGE_SKYINFO	*(DWORD*)"SKY "
#define STAGE_TEXTURE	*(DWORD*)"STEX"
#define STAGE_TEX_REP	*(DWORD*)"STRP"
#define STAGE_TEX_PROJ	*(DWORD*)"STPJ"
#define STAGE_LIGHT		*(DWORD*)"SLIT"
#define STAGE_WATER		*(DWORD*)"SWAT"
#define STAGE_WATER_EX  *(DWORD*)"WAEX"
#define STAGE_VOL_FOG   *(DWORD*)"VFOG"

	struct t_scene_info
	{
		RECT rtZones;			// ��ZoneΪ��λ��ȫ�ֵ�ͼ��С.
		float fTileSize;		// ��Ԫ���ӳ���	
		float fLevelSize;		// �߶ȼ���
		float fHeight;			// �߶�
		USHORT nUnitsPreZone;	// ÿ��Zone�ĸ���ά��
		USHORT nLogicsPreUnit;	// ÿ�����ӵ�Logicά��
		BOOL bVCFullTime;		// �Ƿ���������ĸ�����ɫʱ��
	};


	class t_scene
	{
	public:
		t_scene();
		~t_scene();

		void load(const char * filename);

		const t_scene_info * getInfo() { return &mInfo; }

	protected:
		t_scene_info mInfo;
		t_terrain * mTerrain;

	};

	//class MyPlugin : public Infinite::iPlugin
	//{
	//public:
	//	MyPlugin();
	//	virtual ~MyPlugin();

	//	const char * GetImpExtern() { return  ".stg"; }

	//	void Import(const char * filename);
	//	void Export(const char * filename);

	//	int GetUsages();
	//};
}