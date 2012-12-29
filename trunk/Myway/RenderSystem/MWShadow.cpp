#include "MWShadow.h"
#include "MWRenderHelper.h"
#include "MWEnvironment.h"

namespace Myway {

	Event Shadow::OnVisibleCull;
	Event Shadow::OnRenderDepth;

	Shadow::Shadow()
	{
		mLightCamera = World::Instance()->CreateCamera("Core_LightCamera");
		mLightCameraNode = World::Instance()->CreateSceneNode();

		mLightCameraNode->Attach(mLightCamera);

		mDist[0] = 0;
		mDist[1] = 50;
		mDist[2] = 150;
		mDist[3] = 500;
		mDist[4] = 1000;

		mOffset = 1000;

		_initRT();

		mTech_ShadowDepth = Environment::Instance()->GetShaderLib()->GetTechnique("ShadowDepth");
		mTech_Shadow = Environment::Instance()->GetShaderLib()->GetTechnique("Shadow");
	}

	Shadow::~Shadow()
	{
	}

	void Shadow::_initRT()
	{
		for (int i = 0; i < K_NumShadowLayers; ++i)
		{
			mTex_Depth[i] = VideoBufferManager::Instance()->CreateTextureRT(TString128("Core_Shadow_Tex") + i, 2048, 2048, FMT_R16F);
			mRT_Depth[i] = VideoBufferManager::Instance()->CreateRenderTarget(mTex_Depth[i]);
		}

		mDepthStencil = VideoBufferManager::Instance()->CreateDepthStencil("Core_Shadow_DepthStencil", 2048, 2048, FMT_D24S8, MSAA_NONE);

		mTex_Shadow = VideoBufferManager::Instance()->CreateTextureRT("Core_Shadow_Tex", -1, -1, FMT_X8R8G8B8);
		mRT_Shadow = VideoBufferManager::Instance()->CreateRenderTarget(mTex_Shadow);
	}

	void Shadow::Do(Texture * depthTex)
	{
		RenderSystem * render = RenderSystem::Instance();

		RenderTarget * oldRt = render->GetRenderTarget(0);
		DepthStencil * oldDs = render->GetDepthStencil();

		_updateCamera();

		for (int i = 0; i < 4; ++i)
		{
			//Mat4 matCrop = _calcuCropMatrix(i);
			Mat4 matCrop = Mat4::Identity;
			mCascadedViewProjMatrix[i] = mLightCamera->GetViewProjMatrix() * matCrop;
		}

		_impVisibleCull();

		RS_BeginEvent("ShadowMap_RenderDepth");

		_renderDepth();

		RS_EndEvent();

		RS_BeginEvent("ShadowMap_GenShadow");

		_genShadowMap(depthTex);

		RS_EndEvent();

		render->SetRenderTarget(0, oldRt);
		render->SetDepthStencil(oldDs);
	}

	Aabb Shadow::_calcuAabb(const Vec3 * v)
	{
		Aabb bound = Aabb::Invalid;

		const Mat4 & matViewProj = mLightCamera->GetViewProjMatrix();

		for (int i = 0; i < 8; ++i)
		{
			Vec3 pv = v[i] * matViewProj;

			bound.minimum = bound.minimum.Minimum(pv);
			bound.maximum = bound.maximum.Maximum(pv);
		}

		return bound;
	}

	Mat4 Shadow::_calcuCropMatrix(int layer)
	{
		float s0, s1;
		s0 = mDist[layer];
		s1 = mDist[layer + 1];

		Vec3 vs0(0, 0, s0), vs1(0, 0, s1);
		Math::VecTransform(vs0, vs0, mLightCamera->GetProjMatrix());
		Math::VecTransform(vs1, vs1, mLightCamera->GetProjMatrix());
		s0 = vs0.z, s1 = vs1.z;

		Vec3 frustum[8];

		frustum[0] = Vec3(-1, 1, s0);
		frustum[1] = Vec3(-1, 1, s1);
		frustum[2] = Vec3(1, 1, s0);
		frustum[3] = Vec3(1, 1, s1);
		frustum[4] = Vec3(-1, -1, s0);
		frustum[5] = Vec3(-1, -1, s1);
		frustum[6] = Vec3(1, -1, s0);
		frustum[7] = Vec3(1, -1, s1);

		Math::VecTransform(frustum, frustum, mInverseWorldCameraVP, 8);

		Aabb cropBB = _calcuAabb(frustum);

		cropBB.minimum.z = 0.0f; 

		float scaleX, scaleY, scaleZ;
		float offsetX, offsetY, offsetZ;
		scaleX = 2.0f / (cropBB.maximum.x - cropBB.minimum.x);
		scaleY = 2.0f / (cropBB.maximum.y - cropBB.minimum.y);
		offsetX = -0.5f * (cropBB.maximum.x + cropBB.minimum.x) * scaleX;
		offsetY = -0.5f * (cropBB.maximum.y + cropBB.minimum.y) * scaleY;
		scaleZ = 1.0f / (cropBB.maximum.z - cropBB.minimum.z);
		offsetZ = -cropBB.minimum.z * scaleZ; 

		return Mat4(scaleX,		0,			0,	0,
					0,			scaleY,     0,	0,
					0,			0,			0,	0,
					offsetX,	offsetY,	1,	1);
	}

	void Shadow::_updateCamera()
	{
		Camera * worldCam = World::Instance()->MainCamera();

		mInverseWorldCameraVP = World::Instance()->MainCamera()->GetViewProjMatrix().Inverse();

		float nearClip = worldCam->GetNearClip();
		float farClip = mDist[3];

		mDist[0] = nearClip;

		Vec3 xAixs = worldCam->GetDirection();
		Vec3 yAixs = worldCam->GetUp();
		Vec3 zAixs = Environment::Instance()->GetEvParam()->LightDir;

		if (Math::Abs(zAixs.Dot(yAixs)) > 0.99f)
		{
			yAixs = zAixs.Cross(xAixs);
			xAixs = yAixs.Cross(zAixs);
		}
		else
		{
			xAixs = yAixs.Cross(zAixs);
			yAixs = zAixs.Cross(zAixs);
		}

		Mat4 matView;
		Quat qOrient = Quat::S_FromAxis(xAixs, yAixs, zAixs);

		matView.MakeViewLH(worldCam->GetPosition(), qOrient);

		Vec3 corner[8];

		worldCam->GetWorldCorner(corner, nearClip, farClip);

		for (int i = 0; i < 8; ++i)
		{
			corner[i] *= matView;
		}

		Aabb aabb = Aabb::Invalid;

		for (int i = 0; i < 8; ++i)
		{
			aabb.minimum = Math::Minimum(aabb.minimum, corner[i]);
			aabb.maximum = Math::Maximum(aabb.maximum, corner[i]);
		}

		Vec3 center = aabb.GetCenter();
		float width = aabb.GetWidth();
		float height = aabb.GetHeight();
		float depth = aabb.GetDepth();

		center *= matView.Inverse();

		Vec3 lightPos = center - zAixs * mOffset;

		mLightCamera->SetPosition(lightPos);
		mLightCamera->SetOrientation(qOrient);
		mLightCamera->SetOrthoWidth(width);
		mLightCamera->SetOrthoHeight(height);
		mLightCamera->SetNearClip(nearClip);
		mLightCamera->SetFarClip(mOffset + depth / 2);
		mLightCamera->SetProjectionType(PROJTYPE_ORTHO);
	}

	void Shadow::_impVisibleCull()
	{
		mCullResult.nodes.Clear();
		mCullResult.lights.Clear();

		World::Instance()->ImpVisibleCull(mCullResult, mLightCamera, false);

		OnVisibleCull(mLightCamera, NULL);

		mRenderQueue.Clear();
		mRenderQueue.PushRenderer(mCullResult.nodes);
	}

	void Shadow::_renderDepth()
	{
		RenderSystem * render = RenderSystem::Instance();

		for (int i = 0; i < K_NumShadowLayers && i < 1; ++i)
		{
			render->SetRenderTarget(0, mRT_Depth[i].c_ptr());
			render->SetDepthStencil(mDepthStencil.c_ptr());

			render->ClearBuffer(NULL, true, true, false, Color::White);

			const Array<Renderer *> & objs = mRenderQueue.GetSolidRender();

			ShaderParam * uMatWVP = mTech_ShadowDepth->GetVertexShaderParamTable()->GetParam("matWVP");

			for (int j = 0; j < objs.Size(); ++j)
			{
				Renderer * rd = objs[j];

				bool skined = (rd->GetBlendMatrix(NULL) > 0);

				Mat4 form;

				rd->GetWorldTransform(&form);

				form *= mCascadedViewProjMatrix[i];

				uMatWVP->SetMatrix(form);
				
				render->Render(mTech_ShadowDepth, rd);
			}

			OnRenderDepth(this, &i);
		}
	}

	void Shadow::_genShadowMap(Texture * depthTex)
	{
		RenderSystem * render = RenderSystem::Instance();

		render->SetRenderTarget(0, mRT_Shadow.c_ptr());
		render->SetRenderTarget(1, NULL);
		render->SetRenderTarget(2, NULL);
		render->SetRenderTarget(3, NULL);
		render->SetDepthStencil(NULL);

		Camera * cam = World::Instance()->MainCamera();
		const Vec3 * corner = cam->GetCorner();

		Mat4 matInverseView = cam->GetViewMatrix().Inverse();

		Vec3 cornerLeftTop = corner[4];
		Vec3 cornerRightDir = corner[5] - corner[4];
		Vec3 cornerDownDir = corner[6] - corner[4];

		for (int i = 0; i < K_NumShadowLayers && i < 1; ++i)
		{
			ShaderParam * uShadowInfo = mTech_Shadow->GetPixelShaderParamTable()->GetParam("gShadowInfo");
			ShaderParam * uMatShadow = mTech_Shadow->GetPixelShaderParamTable()->GetParam("matShadow");
			ShaderParam * uCornerLeftTop = mTech_Shadow->GetPixelShaderParamTable()->GetParam("gCornerLeftTop");
			ShaderParam * uCornerRightDir = mTech_Shadow->GetPixelShaderParamTable()->GetParam("gCornerRightDir");
			ShaderParam * uCornerDownDir = mTech_Shadow->GetPixelShaderParamTable()->GetParam("gCornerDownDir");

			uCornerLeftTop->SetUnifom(cornerLeftTop.x, cornerLeftTop.y, cornerLeftTop.z, 0);
			uCornerRightDir->SetUnifom(cornerRightDir.x, cornerRightDir.y, cornerRightDir.z, 0);
			uCornerDownDir->SetUnifom(cornerDownDir.x, cornerDownDir.y, cornerDownDir.z, 0);

			uShadowInfo->SetUnifom(mDist[i], mDist[i + 1], 0, 0);
			uMatShadow->SetMatrix(matInverseView * mCascadedViewProjMatrix[i]);
			
			SamplerState state;
			state.Address = TEXA_CLAMP;
			state.Filter = TEXF_POINT;

			render->SetTexture(0, state, depthTex);
			render->SetTexture(1, state, mTex_Depth[i].c_ptr());

			RenderHelper::Instance()->DrawScreenQuad(BM_OPATICY, mTech_Shadow);
		}
	}
}