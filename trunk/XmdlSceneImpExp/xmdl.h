#pragma once

namespace xmdl {

#define XMDL_FLAG		*(DWORD*)"XMDL"
#define XMDL_VER		3

#define MODEL_FLAG		*(DWORD*)"MODL"
#define TEXS_FLAG		*(DWORD*)"TEXS"
#define FXS_FLAG		*(DWORD*)"FXS "
#define STR_FLAG		*(DWORD*)"STRS"
#define NODE_FLAG		*(DWORD*)"NODE"
#define HOOK_FLAG		*(DWORD*)"HOOK"
#define GEOS_FLAG		*(DWORD*)"GEOS"
#define EMIT_FLAG		*(DWORD*)"EMIT"
#define BOARD_FLAG		*(DWORD*)"BORD"
#define CAMERA_FLAG		*(DWORD*)"CAMR"
#define LIGHT_FLAG		*(DWORD*)"LIGT"
#define NTRACK_FLAG		*(DWORD*)"NTRK"
#define FIGURE_FLAG		*(DWORD*)"FIGR"
#define RIBBON_FLAG		*(DWORD*)"RIBN"
#define RIBBONCTL_FLAG	*(DWORD*)"RIBC"
#define SHACK_FLAG		*(DWORD*)"SHAK"
#define FLYOBJ_FLAG		*(DWORD*)"FLYO"
#define DECAL_FLAG		*(DWORD*)"DECL"



#define GEOSET_PARAM	*(DWORD*)"PARM"
#define GEOSET_VERTEX	*(DWORD*)"VPOS"
#define GEOSET_NORMAL	*(DWORD*)"VNRM"
#define GEOSET_BINORMAL	*(DWORD*)"BNRM"
#define GEOSET_TANGENT	*(DWORD*)"TANG"
#define GEOSET_TEXCOORD	*(DWORD*)"VUV0"
#define GEOSET_VCOLOR	*(DWORD*)"VCLR"
#define GEOSET_INDEX	*(DWORD*)"INDX"
#define GEOSET_SKINS	*(DWORD*)"SKIN"
#define GEOSET_BWEIGHT	*(DWORD*)"BWGT"
#define GEOSET_EXTENT	*(DWORD*)"EXTN"
#define GEOSET_UNWRAP	*(DWORD*)"WRAP"
#define GEOSET_UVWMAP	*(DWORD*)"MAPA"
#define GEOSET_MPOINT	*(DWORD*)"MPNT"


	// �Ƿ������
#define NODE_IS_ROOT		(1L<<7)
	// �� CS����.CS���������Ų������˶��ͱ�׼�����,ֻ������ʱ���ڼ�������
#define NODE_IS_CSBONE		(1L<<8)
	// �Ƿ񸺾������
#define NODE_IS_NEGATIVE	(1L<<9)
	// �Ƿ���Ҫ������ʼ�����ת������,�����������.
#define NODE_HOLD_FIRSTFRAME	(1L<<10)
	// �Ƿ� Dumy.Dumyֻ��λ��,û����ת������.
#define NODE_IS_DUMY		(1L<<13)
	// ���� 14-15 3bits

	struct t_chunk
	{
		DWORD dwFlag;
		UINT nVersion;
		DWORD dwChunkSize;
	};

	struct t_head
	{
		DWORD dwFlag;
		DWORD dwVersion;
	};

	struct t_bound
	{
		FLOAT fBoundsRadius;
		FLOAT fMinimumExtent[3];
		FLOAT fMaximumExtent[3];
	};

	struct t_model
	{
		WORD wFlags;
		USHORT nFrames;
		t_bound bound;
	};

	struct t_hook
	{
		USHORT nHookName;
		USHORT nBoneIndex;
	};

	struct t_geoset
	{
		DWORD dwFlags;				// ����
		BYTE byFxId;				// ����
		BYTE byFaceType;			// D3DPRIMITIVETYPE
		USHORT nFrames;				// ��������Ķ�������(�������,UV����)
		t_bound b;					// ����߽�
		DWORD dwNameStr;			// �����ַ���
	};

	struct t_blend_infl
	{
		WORD wWeightCount;
		WORD wWeightOffset;
	};

	struct t_skin
	{
		FLOAT fWeight;
		BYTE byBoneID;
	};

	struct t_texture
	{
		DWORD dwFileNameStr;
	};

	struct t_node
	{
		WORD wNameStr;
		WORD wFlags;
		BYTE byType;			// NodeType
		BYTE byParent;			// ������
		BYTE byChildBegin;
		BYTE byChildCount;

		FLOAT Pos[3];			// ��ʼ���ֲ�����1������޹ؼ��壬ʹ�ó�ʼ���ֲ�����2�������������ʱ����õ���
		FLOAT Rot[4];
		FLOAT Scl[3];
	};

	struct t_track
	{
		UINT nFrame;
		Vec3 p;
		Quat q;
		Vec3 s;
	};

	struct t_skin_anim
	{
		USHORT nNodeIndex;
		Array<t_track> vTracks;
	};

	struct t_fx
	{
		DWORD dwFileNameStr;				// FX �ļ���
		int nVersion;						// Fx �汾
	};

	struct t_skin_info
	{
		unsigned char bindex[4];
		float bweight[4];
	};

	struct t_mesh
	{
		t_geoset mGeoset;

		int mTextureId;
		int mVertexCount;
		int mIndexCount;

		Vec3 * mVertex;
		Vec3 * mNormal;
		Vec2 * mUV;
		WORD * mIndex;

		t_blend_infl * mBInfl;

		int mSkinCount;
		t_skin * mSkin;

		t_skin_info * mSkinInfo;

		Array<short> boneIds;

		t_mesh()
		{
			mVertexCount = 0;
			mIndexCount = 0;

			mTextureId = -1;

			mVertex = NULL;
			mNormal = NULL;
			mUV = NULL;
			mIndex = NULL;

			mSkinCount = 0;

			mBInfl = NULL;
			mSkin = NULL;

			mSkinInfo = NULL;
		}

		~t_mesh()
		{
			safe_delete_array (mVertex);
			safe_delete_array (mNormal);
			safe_delete_array (mUV);
			safe_delete_array (mIndex);

			mVertexCount = 0;
			mIndexCount = 0;

			safe_delete_array (mBInfl);
			safe_delete_array (mSkin);

			mSkinCount = 0;

			safe_delete_array (mSkinInfo);
		}

		short MapBoneId(short boneId)
		{
			for (int i = 0; i < boneIds.Size(); ++i)
			{
				if (boneIds[i] == boneId)
					return i;
			}

			boneIds.PushBack(boneId);

			return boneIds.Size() - 1;
		}
	};


	struct t_xmdl
	{

	public:
		t_xmdl();
		virtual ~t_xmdl();

		void load(const char * filename);
		void save(const char * filename);

	protected:
		void _saveMesh(const char * filename);
		void _saveAnim(const char * filename);

		bool _exsitBoneName(const TString128 & name);
		void _optimize();
		t_mesh * _mergeMesh(t_mesh * mesh0, t_mesh * mesh1);

	protected:
		void _loadHead(DataStreamPtr & file);
		void _loadGeoset(DataStreamPtr & file, int size);
		int _loadTexture(DataStreamPtr & file, int size);
		void _loadTrack(DataStreamPtr & file, int size);

	protected:
		TString128 mFilename;

		int mCount;
		t_model * mModel;

		int mStrTableSize;
		char * mStrTable;

		int mTextureCount;
		t_texture * mTextures;

		int mFxCount;
		t_fx * mFxs;

		Array<t_mesh *> mMeshes;

		int mNodeCount;
		t_node * mNodes;

		int mSkinAnimCount;
		t_skin_anim * mSkinAnims;

		Array<TString128> mBoneNames;
	};

}