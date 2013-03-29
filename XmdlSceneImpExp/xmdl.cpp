#include "stdafx.h"

#include "xmdl.h"

namespace xmdl {

	// ����ģ����Ӱ����(2 Bits)
#define GEOSET_NOCASTSHADOW	(1L<<0)	// �Ƿ�Ͷ����Ӱ
#define GEOSET_NORCVSHADOW	(1L<<1)	// �Ƿ������Ӱ

	// ����ģ�͹�������(2 Bits)
#define GEOSET_RENDERMASK 	(3L<<2)	// ���շ�ʽ
#define GEOSET_RENDERSHIFT	2		//

	// ��������(1 bits)
#define GEOSET_BRIGE		(1L<<4)	// �Ƿ���

	// Billboard ����(2 bits)
#define GEOSET_BILLBOARD_MASK	(3L<<5)
#define GEOSET_BILLBOARD_SHIFT	5

	// ��ײ
#define GEOSET_COLLISION_MASK	(7L<<7)
#define GEOSET_COLLISION_SHIFT	7

#define GEOSET_COLLISIONPOLYGON		(1L<<15)	// todo:ȥ��.���ר�ŵ�Chunhk

	// �û�����ʱ��ɼ�����
#define GEOSET_TIMEHIDE_MASK		(0xff<<16)
#define GEOSET_TIMEHIDE_SHIFT	16



	t_xmdl::t_xmdl()
	{
		mStrTableSize = 0;
		mStrTable = NULL;

		mCount = 0;
		mModel = NULL;

		mTextureCount = 0;
		mTextures = NULL;
	}

	t_xmdl::~t_xmdl()
	{
		safe_delete_array (mStrTable);
		safe_delete_array (mModel);
		safe_delete_array (mTextures);

		for (int i = 0; i < mMeshes.Size(); ++i)
		{
			delete mMeshes[i];
		}
	}

	void t_xmdl::load(const char * filename)
	{
		mFilename = TString128(filename);

		DataStreamPtr file = ResourceManager::Instance()->OpenResource(mFilename.c_str(), true);

		if (file == NULL)
			return ;

		_loadHead(file);

		t_chunk ck;

		while ( file->Read(&ck, sizeof(t_chunk)) )
		{
			if (ck.dwFlag == MODEL_FLAG)
			{
				mCount = ck.dwChunkSize / sizeof(t_model);
				mModel = new t_model[mCount];

				d_assert (mCount * sizeof(t_model) == ck.dwChunkSize);

				file->Read(mModel, sizeof(t_model) * mCount);
			}

			else if (ck.dwFlag == TEXS_FLAG)
			{
				mTextureCount = ck.dwChunkSize / sizeof(t_texture);
				mTextures = new t_texture[mTextureCount];

				d_assert (mTextureCount * sizeof(t_texture) == ck.dwChunkSize);

				file->Read(mTextures, sizeof(t_texture) * mTextureCount);
			}

			else if (ck.dwFlag == FXS_FLAG)
			{
				mFxCount = ck.dwChunkSize / sizeof(t_fx);
				mFxs = new t_fx[mFxCount];

				d_assert (mFxCount * sizeof(t_fx) == ck.dwChunkSize);

				file->Read(mFxs, sizeof(t_fx) * mFxCount);
			}

			else if (ck.dwFlag == STR_FLAG)
			{
				mStrTableSize = ck.dwChunkSize;
				mStrTable= new char[mStrTableSize];

				file->Read(mStrTable, mStrTableSize);
			}

			else if (ck.dwFlag == GEOS_FLAG)
			{
				_loadGeoset(file, ck.dwChunkSize);
			}

			else
			{
				file->Skip(ck.dwChunkSize);
			}
		}
	}

	void t_xmdl::_loadHead(DataStreamPtr & file)
	{
		t_head head;

		file->Read(&head, sizeof (head));

		d_assert (head.dwFlag == XMDL_FLAG && head.dwVersion == XMDL_VER);
	}

	void t_xmdl::_loadGeoset(DataStreamPtr & file, int size)
	{
		t_geoset geoset;

		file->Read(&geoset, sizeof(t_geoset));

		size -= sizeof (t_geoset);

		if (size == 0 || (geoset.dwFlags & GEOSET_COLLISIONPOLYGON) || (geoset.dwFlags & GEOSET_BRIGE))
		{
			file->Skip(size);
			return ;
		}

		t_mesh * mesh = new t_mesh();

		mesh->mGeoset = geoset;

		while (size > 0)
		{
			t_chunk ck;
			file->Read(&ck, sizeof(t_chunk));

			if (ck.dwFlag == GEOSET_VERTEX)
			{
				mesh->mVertexCount = ck.dwChunkSize / sizeof (Vec3);
				mesh->mVertex = new Vec3[mesh->mVertexCount];

				d_assert (mesh->mVertexCount * sizeof (Vec3) == ck.dwChunkSize);

				file->Read(mesh->mVertex, mesh->mVertexCount * sizeof(Vec3));
			}
			else if (ck.dwFlag == GEOSET_NORMAL)
			{
				mesh->mVertexCount = ck.dwChunkSize / sizeof (Vec3);
				mesh->mNormal = new Vec3[mesh->mVertexCount];

				d_assert (mesh->mVertexCount * sizeof (Vec3) == ck.dwChunkSize);

				file->Read(mesh->mNormal, mesh->mVertexCount * sizeof(Vec3));
			}

			else if (ck.dwFlag == GEOSET_TEXCOORD)
			{
				mesh->mVertexCount = ck.dwChunkSize / sizeof (Vec2);
				mesh->mUV = new Vec2[mesh->mVertexCount];

				d_assert (mesh->mVertexCount * sizeof (Vec2) == ck.dwChunkSize);

				file->Read(mesh->mUV, mesh->mVertexCount * sizeof(Vec2));
			}

			else if (ck.dwFlag == GEOSET_INDEX)
			{
				mesh->mIndexCount = ck.dwChunkSize / sizeof (WORD);
				mesh->mIndex = new WORD[mesh->mIndexCount];

				d_assert (mesh->mIndexCount * sizeof (WORD) == ck.dwChunkSize);

				file->Read(mesh->mIndex, mesh->mIndexCount * sizeof(WORD));
			}

			else if (ck.dwFlag == GEOSET_PARAM)
			{
				mesh->mTextureId = _loadTexture(file, ck.dwChunkSize);
			}

			else
			{
				file->Skip(ck.dwChunkSize);
			}

			size -= sizeof(t_chunk) + ck.dwChunkSize;

			d_assert (size >= 0);
		}

		mMeshes.PushBack(mesh);
	}

	int t_xmdl::_loadTexture(DataStreamPtr & file, int size)
	{
		enum FxParamType
		{
			FXPARAM_UNKNOWN = -1,
			FXPARAM_TEXTURE = 0,
			FXPARAM_FLOAT = 1,	/*!<Property of Type float*/
			FXPARAM_POINT3 = 2,	/*!<Property of Type Point3*/
			FXPARAM_INT = 3,	/*!<Property of Type int*/
			FXPARAM_POINT4 = 4,	/*!<Property of Type Point4*/
			FXPARAM_LAST,
		};

		struct FxParamChunk2
		{
			BYTE byType;		// ��������
			BYTE bAnimated;		// �Ƿ񶯻�
			WORD wParam;		// ������
		};

		struct TrackChunk
		{
			BYTE byTrackType;			// TrackType
			BYTE byInterpType;			// InterpolationType
			WORD wTrackSubType;			// �켣���� NodeTrackType/UVWTrackType/����ID/FxParamType
			WORD wKeyCount;				// �ؼ�������
			WORD wKeySize;				// �ؼ���ṹ��С
		};

		size_t nFxParamSize[FXPARAM_LAST] = 
		{
			sizeof(void*),
			sizeof(float),
			sizeof(float)*3,
			sizeof(int),
			sizeof(float)*4,
		};

		BYTE buffer[1024 * 64];

		d_assert (size < 1024 * 64);

		file->Read(buffer, size);

		BYTE * p = (BYTE *)buffer;

		while ((int)(p - buffer) < size)
		{
			FxParamChunk2 * ck2 = (FxParamChunk2*)p;
			p += sizeof(FxParamChunk2);

			if (!ck2->bAnimated)
			{
				void * val = (void *)p;
				p += nFxParamSize[ck2->byType];

				if (ck2->byType == FXPARAM_TEXTURE)
				{
					return *(BYTE*)val;
				}
			}
			else
			{
				TrackChunk * pTrackChunk = (TrackChunk*)p;
				p += sizeof(TrackChunk);
				BYTE * pTrackData = (BYTE*)p;
				p += pTrackChunk->wKeyCount * pTrackChunk->wKeySize;
			}
		}

		return -1;
	}

	void t_xmdl::save(const char * filename)
	{
		if (mMeshes.Size() == 0)
			return ;

		File file;

		file.Open(filename, OM_WRITE_BINARY);

		d_assert (file.IsOpen());

		char magic[MODEL_FILE_MAGIC_LEN];
		int version;

		strcpy_s(magic, MODEL_FILE_MAGIC_LEN, MODEL_FILE_MAGIC);
		version = MeshLoader_v1::K_File_Verion;

		file.Write(magic, MODEL_FILE_MAGIC_LEN);
		file.Write(&version, sizeof(int));

		for (int i = 0; i < mMeshes.Size(); ++i)
		{
			t_mesh * mesh = mMeshes[i];

			int ckId = MC_SUBMESH;

			file.Write(&ckId, sizeof(int));

			file.Write(&MeshLoader_v1::K_SubMesh_Version, sizeof(int));

			int iVertexCount = mesh->mVertexCount;
			int iIndexCount = mesh->mIndexCount;
			int iVertexElem = MeshLoader_v1::VE_POSITION | MeshLoader_v1::VE_NORMAL | MeshLoader_v1::VE_TEXCOORD;

			file.Write(&iVertexCount, sizeof(int));
			file.Write(&iIndexCount, sizeof(int));
			file.Write(&iVertexElem, sizeof(int));

			for (int v = 0; v < iVertexCount; ++v)
			{
				file.Write(&mesh->mVertex[v], sizeof(Vec3));

				if (mesh->mNormal)
					file.Write(&mesh->mNormal[v], sizeof(Vec3));
				else
					file.Write(&Vec3::UnitY, sizeof(Vec3));

				if (mesh->mUV)
					file.Write(&mesh->mUV[v], sizeof(Vec2));
				else
					file.Write(&Vec2::Zero, sizeof(Vec2));
			}

			file.Write(mesh->mIndex, sizeof(WORD) * iIndexCount);

			// write material
			file.Write(&MeshLoader_v1::K_Material_Version, sizeof(int));

			int doubleSide = 0;
			int blendMode = BM_OPATICY;
			Color4 emissive = Color4::Black;
			Color4 ambient = Color4::Gray;
			Color4 diffuse = Color4::White;
			Color4 specular = Color4::Black;
			float specularPower = 80;
			TString128 emissiveMap = "";
			TString128 diffuseMap = "";
			TString128 normalMap = "";
			TString128 specularMap = "";

			if (mesh->mTextureId != -1)
			{
				const char * texture = mStrTable + mTextures[mesh->mTextureId].dwFileNameStr;

				diffuseMap = "Texture\\" + TString128(texture) + ".dds";
			}

			const char * strFxName = mStrTable + mFxs[mesh->mGeoset.byFxId].dwFileNameStr;

			if (strstr(strFxName, "AlphaTest") != NULL)
			{
				blendMode = BM_ALPHA_TEST;
				doubleSide = 1;
			}

			file.Write(&doubleSide, sizeof(int));
			file.Write(&blendMode, sizeof(int));

			file.Write(&emissive, sizeof(Color4));
			file.Write(&ambient, sizeof(Color4));
			file.Write(&diffuse, sizeof(Color4));
			file.Write(&specular, sizeof(Color4));
			file.Write(&specularPower, sizeof(float));

			file.Write(emissiveMap.c_str(), 128);
			file.Write(diffuseMap.c_str(), 128);
			file.Write(normalMap.c_str(), 128);
			file.Write(specularMap.c_str(), 128);
		}

		if (mModel)
		{
			Aabb bound;
			bound.minimum = Vec3(mModel->bound.fMinimumExtent[0],
				mModel->bound.fMinimumExtent[1],
				mModel->bound.fMinimumExtent[2]);
			bound.maximum = Vec3(mModel->bound.fMaximumExtent[0],
				mModel->bound.fMaximumExtent[1],
				mModel->bound.fMaximumExtent[2]);

			Sphere sph;
			sph.center = bound.GetCenter();
			sph.radius = mModel->bound.fBoundsRadius;

			file.Write(&MeshLoader_v1::K_Bound_Version, sizeof(int));

			file.Write(&bound.minimum, sizeof(Vec3));
			file.Write(&bound.maximum, sizeof(Vec3));

			file.Write(&sph.center, sizeof(Vec3));
			file.Write(&sph.radius, sizeof(float));
		}

		file.Close();
	}

}