//////////////////////////////////////////////////////////////////////////
//
// EMotionFX Integration.
//   EMotionFX engine : http://www.mysticgd.com
//   
//	 author: Myway
//	 e-mail: Myway3D@Gmail.com
//
//   this soft is free, no license.
//
//
#pragma once

#include "MActorEntry.h"
#include "MWResource.h"

namespace EMotionFX {
	class Motion;
}

namespace Myway {

	class MActorRes;

	class MACTOR_ENTRY MMotion : public Resource
	{
	public:
		struct PlayInfo
		{
			float BlendInTime;
			float BlendOutTime;
			float Speed;
			int Loops;
			bool Mixed;
			bool Overwrite;
			bool UseScale;

			PlayInfo()
			{
				BlendInTime		 = 0.3f;
				BlendOutTime	 = 0.3f;
				Speed			 = 1.0f;
				Loops			 = -1;
				Overwrite		 = true;
				UseScale		 = false;
				Mixed			 = false;
			}
		};

	public:
		MMotion();
		virtual ~MMotion();

		const TString128 & GetName();
		float GetLength();

		void _Load(const TString128 & source);

		// override Resource
		virtual void Load();
		virtual void Reload();
		virtual void Unload();

		virtual void LoadImp(DataStreamPtr stream);

		EMotionFX::Motion * GetEmotionMotion() { return mMotion; }

	protected:
		MActorRes * mActor;
		EMotionFX::Motion * mMotion;
		TString128 mName;
	};

}