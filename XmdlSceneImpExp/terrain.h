#pragma once

#define TERRAIN_BLOCK		*(DWORD*)"TBLK"

#define TERRAIN_BLOCKINFO	*(DWORD*)"TBKI"
#define TERRAIN_WATERINFO	*(DWORD*)"TWAT"
#define TERRAIN_LAYER		*(DWORD*)"TLYR"
#define TERRAIN_SHADOW		*(DWORD*)"TSHW"
#define TERRAIN_SHADOWT		*(DWORD*)"TSWT"
#define TERRAIN_HEIGHT		*(DWORD*)"THIT"
#define TERRAIN_NORMAL		*(DWORD*)"TNRM"
#define TERRAIN_ECOTYPE		*(DWORD*)"TECO"

#define TERRAIN_LAYERINFO	*(DWORD*)"TLIF"
#define TERRAIN_LAYERALPHA	*(DWORD*)"TLAL"

#define TERRAIN_GRID_SIZE 25.6f
#define TERRAIN_BLOCK_SIZE 16
#define TERRAIN_MAX_LAYERS 4
#define TERRAIN_LAYER_SIZE 64

namespace xmdl {

	struct t_blockInfo
	{
		BYTE bZoneX;
		BYTE bZoneZ;
		BYTE bBlockX;
		BYTE bBlockZ;
		BYTE bLayers;		// �������
	};

	struct t_layerInfo
	{
		BYTE bTexture;		// ���������
		BYTE bProject;		// �����ͶӰ������
		BYTE bRepeatU;		// ����U�����ظ�����
		BYTE bRepeatV;		// ����V�����ظ�����
	};

	struct t_layer
	{
		int nLayerInfos;
		t_layerInfo * pLayerInfos;

		BYTE* pOpacities;
		UINT nOpacities;

		t_layer()
		{
			pOpacities = NULL;
			nOpacities = 0;

			nLayerInfos = 0;
			pLayerInfos = NULL;

		}

		~t_layer()
		{
			safe_delete_array (pOpacities);
			safe_delete_array (pLayerInfos);
			nOpacities = 0;
		}
	};

	struct t_block
	{
		t_blockInfo info;

		int nLayers;
		t_layer * pLayers[TERRAIN_MAX_LAYERS];

		WORD* pHeights;
		UINT nHeights;

		int xIndex;
		int zIndex;

		int xWeightIndex;
		int zWeightIndex;

		t_block()
		{
			pHeights = NULL;
			nHeights = 0;
			nLayers = 0;

			xIndex = 0;
			zIndex = 0;

			xWeightIndex = 0;
			zWeightIndex = 0;

			for (int i = 0; i < TERRAIN_MAX_LAYERS; ++i)
				pLayers[i] = NULL;
		}

		~t_block()
		{
			safe_delete_array (pHeights);

			for (int i = 0; i < TERRAIN_MAX_LAYERS; ++i)
				safe_delete (pLayers[i]);
		}
	};

	class t_scene;

	class t_terrain
	{
	public:
		t_terrain(t_scene * scene);
		~t_terrain();

		void loadTile(File & file, int size);

		void import();

	protected:
		void loadBlock(DataStreamPtr stream, int size);
		t_layer * loadLayer(DataStreamPtr strea, int size);

		float getHeight(float x, float z);

		void addLayer(const t_layerInfo & layers);
		const t_layerInfo * getLayer(int id);

	protected:
		Array<t_block *> mBlocks;
		t_scene * mScene;
		float * mHeights;
		int mXVertSize;
		int mZVertSize;

		Color * mWeights;
		Array<t_layerInfo> mLayers;
	};
}