#include "MWOcean.h"
#include "MWEnvironment.h"
#include "MWProjectionGrid.h"
#include "MWWaterManager.h"
#include "Engine.h"

namespace Myway {

    Ocean::Ocean()
		: OnPreRender(RenderEvent::OnPreRenderEvent, this, &Ocean::_PreRender)
    {
        _init();

        mPosition = Vec3(0, 10, 0);
        mProjGrid = new ProjectedGrid();
    }

    Ocean::~Ocean()
    {
		delete mProjGrid;
    }

	void Ocean::Resize(int w, int h)
	{
		_initRT();
	}

    void Ocean::_PreRender(Event * sender)
    {
		if (!WaterManager::Instance()->IsUnderWater())
			_renderRelfection();
    }

    void Ocean::Render(Texture * depthTex, Texture * colorTex)
    {
        if (WaterManager::Instance()->IsUnderWater())
            _renderUnderWater(depthTex, colorTex);
        else
            _renderUpWater(depthTex, colorTex);
    }

	void Ocean::_init()
	{
		mTech = Environment::Instance()->GetShaderLib()->GetTechnique("Ocean");
		mTech_UnderWater = Environment::Instance()->GetShaderLib()->GetTechnique("UnderWater");
		mTex_Wave = VideoBufferManager::Instance()->Load2DTexture("Water\\waves2.dds", "Water\\waves2.dds");
		mTex_Fresnel = VideoBufferManager::Instance()->Load2DTexture("Water\\Fresnel.bmp", "Water\\Fresnel.bmp");
		mTex_Normal0 = VideoBufferManager::Instance()->Load2DTexture("Water\\WaterNormal1.tga", "Water\\WaterNormal1.tga");
		mTex_Normal1 = VideoBufferManager::Instance()->Load2DTexture("Water\\WaterNormal2.tga", "Water\\WaterNormal2.tga");

		d_assert (mTech != NULL && mTex_Normal0 != NULL && mTech_UnderWater != NULL);

		_initRT();
	}

	void Ocean::_initRT()
	{
		mTex_Refl = NULL;
		mDepthStencil = NULL;
		mRT_Refl = NULL;
		
		RenderScheme * scheme = Engine::Instance()->GetRenderScheme();

		int width = scheme->GetColorTexture()->GetWidth() / 2;
		int height = scheme->GetColorTexture()->GetHeight() / 2;

		mTex_Refl = VideoBufferManager::Instance()->CreateTextureRT("Core_Water_Tex", width, height);
		mDepthStencil = VideoBufferManager::Instance()->CreateDepthStencil("Core_Water_DS", width, height, FMT_D24S8, MSAA_NONE);
		mRT_Refl = VideoBufferManager::Instance()->CreateRenderTarget(mTex_Refl);
	}


    void Ocean::_renderUpWater(Texture * depthTex, Texture * colorTex)
    {
		float elapsedTime = Engine::Instance()->GetFrameTime();

		mProjGrid->SetHeight(mPosition.y);
		mProjGrid->Update(elapsedTime);

		RenderSystem * render = RenderSystem::Instance();
		Camera * cam = World::Instance()->MainCamera();
		float time = Engine::Instance()->GetTime();

		const Vec3 * worldCorner = cam->GetWorldCorner();
		const Vec3 & camPos = cam->GetPosition();

		Vec3 worldCornerLeftTop = worldCorner[4] - camPos;
		Vec3 worldCornerRightDir = worldCorner[5] - worldCorner[4];
		Vec3 worldCornerDownDir = worldCorner[6] - worldCorner[4];

		const Vec3 * viewCorner = cam->GetCorner();

		Vec3 viewCornerLeftTop = viewCorner[4];
		Vec3 viewCornerRightDir = viewCorner[5] - viewCorner[4];
		Vec3 viewCornerDownDir = viewCorner[6] - viewCorner[4];

		ShaderParam * uPosition = mTech->GetVertexShaderParamTable()->GetParam("gPosition");
		ShaderParam * uScale = mTech->GetVertexShaderParamTable()->GetParam("gScale");

		ShaderParam * uFresnelParam = mTech->GetPixelShaderParamTable()->GetParam("gFresnelParam");
		ShaderParam * uCamPos = mTech->GetPixelShaderParamTable()->GetParam("gCameraPos");
		ShaderParam * uWorldCornerLeftTop = mTech->GetPixelShaderParamTable()->GetParam("gWorldCornerLeftTop");
		ShaderParam * uWorldCornerRightDir = mTech->GetPixelShaderParamTable()->GetParam("gWorldCornerRightDir");
		ShaderParam * uWorldCornerDownDir = mTech->GetPixelShaderParamTable()->GetParam("gWorldCornerDownDir");
		ShaderParam * uViewCornerLeftTop = mTech->GetPixelShaderParamTable()->GetParam("gViewCornerLeftTop");
		ShaderParam * uViewCornerRightDir = mTech->GetPixelShaderParamTable()->GetParam("gViewCornerRightDir");
		ShaderParam * uViewCornerDownDir = mTech->GetPixelShaderParamTable()->GetParam("gViewCornerDownDir");
		ShaderParam * uDeepColor = mTech->GetPixelShaderParamTable()->GetParam("gDeepColor");
		ShaderParam * uRefractionDist = mTech->GetPixelShaderParamTable()->GetParam("gRefractionDist");

		ShaderParam * uSunDir = mTech->GetPixelShaderParamTable()->GetParam("gSunDir");
		ShaderParam * uSunColor = mTech->GetPixelShaderParamTable()->GetParam("gSunColor");
		ShaderParam * uSunLightParam = mTech->GetPixelShaderParamTable()->GetParam("gSunLightParam");

		ShaderParam * uNoiseScale = mTech->GetPixelShaderParamTable()->GetParam("gNoiseScale");
		ShaderParam * uNoiseUVParam = mTech->GetPixelShaderParamTable()->GetParam("gNoiseUVParam");

		ShaderParam * uNormalUVParam = mTech->GetPixelShaderParamTable()->GetParam("gNormalUVParam");
		ShaderParam * uNormalWeight = mTech->GetPixelShaderParamTable()->GetParam("gNormalWeight");

		ShaderParam * uInvScreenSize = mTech->GetPixelShaderParamTable()->GetParam("gInvScreenSize");
		ShaderParam * uFarClip = mTech->GetPixelShaderParamTable()->GetParam("gFarClip");

		Vec3 sunDir = Environment::Instance()->GetEvParam()->SunDir;
		Color4 sunColor = Environment::Instance()->GetEvParam()->SunColor;
		Color4 deepColor = Environment::Instance()->GetEvParam()->WaterParam.deepColor;

		if (mProjGrid)
		{
			uPosition->SetUnifom(0, 0, 0, 0);
			uScale->SetUnifom(1, 1, 1, 1);
		}

		uCamPos->SetUnifom(camPos.x, camPos.y, camPos.z, 0);

		uWorldCornerLeftTop->SetUnifom(worldCornerLeftTop.x, worldCornerLeftTop.y, worldCornerLeftTop.z, 0);
		uWorldCornerRightDir->SetUnifom(worldCornerRightDir.x, worldCornerRightDir.y, worldCornerRightDir.z, 0);
		uWorldCornerDownDir->SetUnifom(worldCornerDownDir.x, worldCornerDownDir.y, worldCornerDownDir.z, 0);

		uViewCornerLeftTop->SetUnifom(viewCornerLeftTop.x, viewCornerLeftTop.y, viewCornerLeftTop.z, 0);
		uViewCornerRightDir->SetUnifom(viewCornerRightDir.x, viewCornerRightDir.y, viewCornerRightDir.z, 0);
		uViewCornerDownDir->SetUnifom(viewCornerDownDir.x, viewCornerDownDir.y, viewCornerDownDir.z, 0);

		uDeepColor->SetUnifom(deepColor.r, deepColor.g, deepColor.b, 1);
		uRefractionDist->SetUnifom(40, 0, 0, 0);

		uSunColor->SetUnifom(sunColor.r, sunColor.g, sunColor.b, 0);
		uSunDir->SetUnifom(-sunDir.x, -sunDir.y, -sunDir.z, 0);
		uSunLightParam->SetUnifom(0.8f, 200, 0, 0);

		float uvNoiseScroll = time * 0.02f;

		uNoiseUVParam->SetUnifom(0.01f, 0.01f, uvNoiseScroll, uvNoiseScroll);
		uNoiseScale->SetUnifom(0.03f, 0, 0 , 0);

		Vec4 uvNormalParam[4] = {
			Vec4 (0.02f, 0.02f, time * 0.03f, time * 0.03f),
			Vec4 (0.01f, 0.01f, time * 0.03f, time * 0.03f),
			Vec4 (0.015f, 0.015f, time * 0.03f, time * 0.03f),
			Vec4 (0.005f, 0.005f, time * 0.03f, time * 0.03f)
		};

		uNormalUVParam->SetVector(uvNormalParam, 4);
		uNormalWeight->SetUnifom(0.08f, 0.1f, 0.06f, 0.1f);

		float screenW = (float)Engine::Instance()->GetDeviceProperty()->Width;
		float screenH = (float)Engine::Instance()->GetDeviceProperty()->Height;

		uInvScreenSize->SetUnifom(1 / screenW, 1 / screenW, 0, 0);
		uFarClip->SetUnifom(cam->GetFarClip(), 0, 0, 0);

		SamplerState state;
		render->SetTexture(0, state, mTex_Wave.c_ptr());
		state.Address = TEXA_CLAMP;
		state.Filter = TEXF_POINT;
		render->SetTexture(1, state, depthTex);
		render->SetTexture(2, state, colorTex);
		render->SetTexture(3, state, mTex_Refl.c_ptr());
		state.Filter = TEXF_LINEAR;
		render->SetTexture(4, state, mTex_Fresnel.c_ptr());

		SamplerState state1;
		render->SetTexture(5, state1, mTex_Wave.c_ptr());
		render->SetTexture(6, state1, mTex_Wave.c_ptr());

		render->Render(mTech, mProjGrid->GetRender());
    }

    void Ocean::_renderUnderWater(Texture * depthTex, Texture * colorTex)
    {
		float elapsedTime = Engine::Instance()->GetFrameTime();

		mProjGrid->Update(elapsedTime);

        Camera * cam = World::Instance()->MainCamera();
        RenderSystem * render = RenderSystem::Instance();
        float time = Engine::Instance()->GetTime();

        ShaderParam * uPosition = mTech_UnderWater->GetVertexShaderParamTable()->GetParam("gPosition");
        ShaderParam * uScale = mTech_UnderWater->GetVertexShaderParamTable()->GetParam("gScale");
        ShaderParam * uUVParam = mTech_UnderWater->GetVertexShaderParamTable()->GetParam("gUVParam");

        ShaderParam * uTintColor = mTech_UnderWater->GetPixelShaderParamTable()->GetParam("gTintColor");
        ShaderParam * uNoiseScale = mTech_UnderWater->GetPixelShaderParamTable()->GetParam("gNoiseScale");
        ShaderParam * uFogParam = mTech_UnderWater->GetPixelShaderParamTable()->GetParam("gFogParam");
        ShaderParam * uFogColor = mTech_UnderWater->GetPixelShaderParamTable()->GetParam("gFogColor");
        ShaderParam * uCameraPos = mTech_UnderWater->GetPixelShaderParamTable()->GetParam("gCameraPos");

        float uvScroll = time * 0.02f;
        float fogStart = Environment::Instance()->GetEvParam()->WaterParam.fogStart;
        float fogEnd = Environment::Instance()->GetEvParam()->WaterParam.fogEnd;
        Color4 fogColor = Environment::Instance()->GetEvParam()->WaterParam.fogColor;

        Vec3 camPos = cam->GetPosition();

        uPosition->SetUnifom(0, 0, 0, 0);
        uScale->SetUnifom(1, 1, 1, 1);
        uUVParam->SetUnifom(0.01f, 0.01f, uvScroll, uvScroll);

        uTintColor->SetUnifom(0.1f, 0.1f, 0.3f, 1);
        uNoiseScale->SetUnifom(0.08f, 0, 0 , 0);
		uFogParam->SetUnifom(fogStart, fogEnd, 1 / (fogEnd - fogStart), 0.3f);
		uFogColor->SetUnifom(fogColor.r, fogColor.g, fogColor.b, 0);
		uCameraPos->SetUnifom(camPos.x, camPos.y, camPos.z, 0);

		SamplerState state;
		render->SetTexture(0, state, mTex_Wave.c_ptr());
		state.Address = TEXA_CLAMP;
		render->SetTexture(1, state, colorTex);
		render->SetTexture(2, state, mTex_Fresnel.c_ptr());

		render->Render(mTech_UnderWater, mProjGrid->GetRender());
	}

    void Ocean::_renderRelfection()
    {
		RenderSystem * render = RenderSystem::Instance();

		Plane mirrorPlane = Plane(Vec3(0, 1, 0), mPosition);

		if (mProjGrid)
		{
			mirrorPlane = mProjGrid->GetUpperPlane();
		}

        RenderTarget * oldRT = render->GetRenderTarget(0);
        DepthStencil * oldDS = render->GetDepthStencil();

        // ---> Bind RenderTarget
        render->SetRenderTarget(0, mRT_Refl.c_ptr());
        render->SetRenderTarget(1, NULL);
        render->SetRenderTarget(2, NULL);
        render->SetRenderTarget(3, NULL);
        render->SetDepthStencil(mDepthStencil.c_ptr());

        // ---> clear buffer
        render->ClearBuffer(NULL, true, true, false, Color::Black, 1, 0);

		Color4 deepColor = Environment::Instance()->GetEvParam()->WaterParam.reflColor;

		RenderRegister::Instance()->SetMirrorColor(deepColor);

		Engine::Instance()->GetRenderScheme()->RenderInMirror(mirrorPlane);

        mRT_Refl->Stretch(mTex_Refl.c_ptr());

        render->SetRenderTarget(0, oldRT);
        render->SetDepthStencil(oldDS);
    }
}