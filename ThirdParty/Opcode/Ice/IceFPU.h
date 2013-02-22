///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains FPU related code.
 *	\file		IceFPU.h
 *	\author		Pierre Terdiman
 *	\date		April, 4, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __ICEFPU_H__
#define __ICEFPU_H__

	#define	SIGN_BITMASK			0x80000000

#define NOT_SO_FAST_BUT_DO_NOT_BREAK_ALIAS_RULE

#ifdef NOT_SO_FAST_BUT_DO_NOT_BREAK_ALIAS_RULE
	inline void swap( float & x, float & y ){ float tmp( y ); y = x; x = tmp; }

	inline void absVal( float & x ) { x = fabs( x ); }

	union udwordFloatUnion {
		udword i;
		float f;
	};
	union sdwordFloatUnion {
		sdword i;
		float f;
	};
	union intFloatUnion {
		int i;
		float f;
	};

	inline float  FR(  udword x ) { udwordFloatUnion u; u.i = x; return u.f; }
	inline sdword SIR( float x  ) { sdwordFloatUnion u; u.f = x; return u.i; }
	inline udword IR(  float x  ) { udwordFloatUnion u; u.f = x; return u.i; }
	inline int  IntR(  float x  ) { intFloatUnion u; u.f = x; return u.i; }
#else
	//! Integer representation of a floating-point value.
	#define IR(x)					((udword&)(x))

	//! Signed integer representation of a floating-point value.
	#define SIR(x)					((sdword&)(x))

	//! Floating-point representation of an integer value.
	#define FR(x)					((float&)(x))

#endif

	//! Absolute integer representation of a floating-point value
	#define AIR(x)					(IR(x)&0x7fffffff)

	//! Integer-based comparison of a floating point value.
	//! Don't use it blindly, it can be faster or slower than the FPU comparison, depends on the context.
	#define IS_NEGATIVE_FLOAT(x)	(IR(x)&0x80000000)

	//! Fast fabs for floating-point values. It just clears the sign bit.
	//! Don't use it blindy, it can be faster or slower than the FPU comparison, depends on the context.
	inline_ float FastFabs(float x)
	{
		udword FloatBits = IR(x)&0x7fffffff;
		return FR(FloatBits);
	}

	//! Fast square root for floating-point values.
	inline_ float FastSqrt(float square)
	{
#ifdef _MSC_VER
			float retval;

			__asm {
					mov             eax, square
					sub             eax, 0x3F800000
					sar             eax, 1
					add             eax, 0x3F800000
					mov             [retval], eax
			}
			return retval;
#else
			return sqrt(square);
#endif
	}

	//! Saturates positive to zero.
	inline_ float fsat(float f)
	{
#ifdef NOT_SO_FAST_BUT_DO_NOT_BREAK_ALIAS_RULE
		udword y = IR(f) & ~(SIR(f) >>31);
		return FR(y);
#else
		udword y = (udword&)f & ~((sdword&)f >>31);
		return (float&)y;
#endif
	}

	//! Computes 1.0f / sqrtf(x).
	inline_ float frsqrt(float f)
	{
#ifdef NOT_SO_FAST_BUT_DO_NOT_BREAK_ALIAS_RULE
		return 1.0 / sqrt( f );
#else
		float x = f * 0.5f;
		udword y = 0x5f3759df - ((udword&)f >> 1);
		// Iteration...
		(float&)y  = (float&)y * ( 1.5f - ( x * (float&)y * (float&)y ) );
		// Result
		return (float&)y;
#endif
	}

	//! Computes 1.0f / sqrtf(x). Comes from NVIDIA.
	inline_ float InvSqrt(const float& x)
	{
#ifdef NOT_SO_FAST_BUT_DO_NOT_BREAK_ALIAS_RULE
		return frsqrt( x );
#else
		udword tmp = (udword(IEEE_1_0 << 1) + IEEE_1_0 - *(udword*)&x) >> 1;   
		float y = *(float*)&tmp;                                             
		return y * (1.47f - 0.47f * x * y * y);
#endif
	}

	//! Computes 1.0f / sqrtf(x). Comes from Quake3. Looks like the first one I had above.
	//! See http://www.magic-software.com/3DGEDInvSqrt.html
	inline_ float RSqrt(float number)
	{
#ifdef NOT_SO_FAST_BUT_DO_NOT_BREAK_ALIAS_RULE
		return frsqrt( number );
#else
		long i;
		float x2, y;
		const float threehalfs = 1.5f;

		x2 = number * 0.5f;
		y  = number;
		i  = * (long *) &y;
		i  = 0x5f3759df - (i >> 1);
		y  = * (float *) &i;
		y  = y * (threehalfs - (x2 * y * y));

		return y;
#endif	       
	}

	//! TO BE DOCUMENTED
	inline_ float fsqrt(float f)
	{
#ifdef NOT_SO_FAST_BUT_DO_NOT_BREAK_ALIAS_RULE
	  return frsqrt( f );
#else
		udword y = ( ( (sdword&)f - 0x3f800000 ) >> 1 ) + 0x3f800000;
		// Iteration...?
		// (float&)y = (3.0f - ((float&)y * (float&)y) / f) * (float&)y * 0.5f;
		// Result
		return (float&)y;
#endif	       
	}

	//! Returns the float ranged espilon value.
	inline_ float fepsilon(float f)
	{
#ifdef NOT_SO_FAST_BUT_DO_NOT_BREAK_ALIAS_RULE
		udword b = IR(f) & 0xff800000;
		udword a = b | 0x00000001;
		float fa = FR( a );
		float fb = FR( b );
		fa -= fb;
		// Result
		return fa;
#else
		udword b = (udword&)f & 0xff800000;
		udword a = b | 0x00000001;
		(float&)a -= (float&)b;
		// Result
		return (float&)a;
#endif
	}

	//! Is the float valid ?
	inline_ bool IsNAN(float value)				{ return (IR(value)&0x7f800000) == 0x7f800000;	}
	inline_ bool IsIndeterminate(float value)	{ return IR(value) == 0xffc00000;				}
	inline_ bool IsPlusInf(float value)			{ return IR(value) == 0x7f800000;				}
	inline_ bool IsMinusInf(float value)		{ return IR(value) == 0xff800000;				}

	inline_	bool IsValidFloat(float value)
	{
		if(IsNAN(value))			return false;
		if(IsIndeterminate(value))	return false;
		if(IsPlusInf(value))		return false;
		if(IsMinusInf(value))		return false;
		return true;
	}

	#define CHECK_VALID_FLOAT(x)	ASSERT(IsValidFloat(x));

/*
	//! FPU precision setting function.
	inline_ void SetFPU()
	{
		// This function evaluates whether the floating-point
		// control word is set to single precision/round to nearest/
		// exceptions disabled. If these conditions don't hold, the
		// function changes the control word to set them and returns
		// TRUE, putting the old control word value in the passback
		// location pointed to by pwOldCW.
		{
			uword wTemp, wSave;
 
			__asm fstcw wSave
			if (wSave & 0x300 ||            // Not single mode
				0x3f != (wSave & 0x3f) ||   // Exceptions enabled
				wSave & 0xC00)              // Not round to nearest mode
			{
				__asm
				{
					mov ax, wSave
					and ax, not 300h    ;; single mode
					or  ax, 3fh         ;; disable all exceptions
					and ax, not 0xC00   ;; round to nearest mode
					mov wTemp, ax
					fldcw   wTemp
				}
			}
		}
	}
*/
	//! This function computes the slowest possible floating-point value (you can also directly use FLT_EPSILON)
	inline_ float ComputeFloatEpsilon()
	{
#ifdef NOT_SO_FAST_BUT_DO_NOT_BREAK_ALIAS_RULE
		return FLT_EPSILON;
#else
		float f = 1.0f;
		((udword&)f)^=1;
		return f - 1.0f;	// You can check it's the same as FLT_EPSILON
#endif
	}

	inline_ bool IsFloatZero(float x, float epsilon=1e-6f)
	{
		return x*x < epsilon;
	}

	#define FCOMI_ST0	_asm	_emit	0xdb	_asm	_emit	0xf0
	#define FCOMIP_ST0	_asm	_emit	0xdf	_asm	_emit	0xf0
	#define FCMOVB_ST0	_asm	_emit	0xda	_asm	_emit	0xc0
	#define FCMOVNB_ST0	_asm	_emit	0xdb	_asm	_emit	0xc0

	#define FCOMI_ST1	_asm	_emit	0xdb	_asm	_emit	0xf1
	#define FCOMIP_ST1	_asm	_emit	0xdf	_asm	_emit	0xf1
	#define FCMOVB_ST1	_asm	_emit	0xda	_asm	_emit	0xc1
	#define FCMOVNB_ST1	_asm	_emit	0xdb	_asm	_emit	0xc1

	#define FCOMI_ST2	_asm	_emit	0xdb	_asm	_emit	0xf2
	#define FCOMIP_ST2	_asm	_emit	0xdf	_asm	_emit	0xf2
	#define FCMOVB_ST2	_asm	_emit	0xda	_asm	_emit	0xc2
	#define FCMOVNB_ST2	_asm	_emit	0xdb	_asm	_emit	0xc2

	#define FCOMI_ST3	_asm	_emit	0xdb	_asm	_emit	0xf3
	#define FCOMIP_ST3	_asm	_emit	0xdf	_asm	_emit	0xf3
	#define FCMOVB_ST3	_asm	_emit	0xda	_asm	_emit	0xc3
	#define FCMOVNB_ST3	_asm	_emit	0xdb	_asm	_emit	0xc3

	#define FCOMI_ST4	_asm	_emit	0xdb	_asm	_emit	0xf4
	#define FCOMIP_ST4	_asm	_emit	0xdf	_asm	_emit	0xf4
	#define FCMOVB_ST4	_asm	_emit	0xda	_asm	_emit	0xc4
	#define FCMOVNB_ST4	_asm	_emit	0xdb	_asm	_emit	0xc4

	#define FCOMI_ST5	_asm	_emit	0xdb	_asm	_emit	0xf5
	#define FCOMIP_ST5	_asm	_emit	0xdf	_asm	_emit	0xf5
	#define FCMOVB_ST5	_asm	_emit	0xda	_asm	_emit	0xc5
	#define FCMOVNB_ST5	_asm	_emit	0xdb	_asm	_emit	0xc5

	#define FCOMI_ST6	_asm	_emit	0xdb	_asm	_emit	0xf6
	#define FCOMIP_ST6	_asm	_emit	0xdf	_asm	_emit	0xf6
	#define FCMOVB_ST6	_asm	_emit	0xda	_asm	_emit	0xc6
	#define FCMOVNB_ST6	_asm	_emit	0xdb	_asm	_emit	0xc6

	#define FCOMI_ST7	_asm	_emit	0xdb	_asm	_emit	0xf7
	#define FCOMIP_ST7	_asm	_emit	0xdf	_asm	_emit	0xf7
	#define FCMOVB_ST7	_asm	_emit	0xda	_asm	_emit	0xc7
	#define FCMOVNB_ST7	_asm	_emit	0xdb	_asm	_emit	0xc7

	//! A global function to find MAX(a,b) using FCOMI/FCMOV
	inline_ float FCMax2(float a, float b)
	{
#ifdef _MSC_VER
		float Res;
		_asm	fld		[a]
		_asm	fld		[b]
		FCOMI_ST1
		FCMOVB_ST1
		_asm	fstp	[Res]
		_asm	fcomp
		return Res;
#else
		return (a > b) ? a : b;
#endif
	}

	//! A global function to find MIN(a,b) using FCOMI/FCMOV
	inline_ float FCMin2(float a, float b)
	{
#ifdef _MSC_VER
		float Res;
		_asm	fld		[a]
		_asm	fld		[b]
		FCOMI_ST1
		FCMOVNB_ST1
		_asm	fstp	[Res]
		_asm	fcomp
		return Res;
#else
		return (a < b) ? a : b;
#endif
	}

	//! A global function to find MAX(a,b,c) using FCOMI/FCMOV
	inline_ float FCMax3(float a, float b, float c)
	{
#ifdef _MSC_VER
		float Res;
		_asm	fld		[a]
		_asm	fld		[b]
		_asm	fld		[c]
		FCOMI_ST1
		FCMOVB_ST1
		FCOMI_ST2
		FCMOVB_ST2
		_asm	fstp	[Res]
		_asm	fcompp
		return Res;
#else
		return (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
#endif
	}

	//! A global function to find MIN(a,b,c) using FCOMI/FCMOV
	inline_ float FCMin3(float a, float b, float c)
	{
#ifdef _MSC_VER
		float Res;
		_asm	fld		[a]
		_asm	fld		[b]
		_asm	fld		[c]
		FCOMI_ST1
		FCMOVNB_ST1
		FCOMI_ST2
		FCMOVNB_ST2
		_asm	fstp	[Res]
		_asm	fcompp
		return Res;
#else
		return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);
#endif
	}

	inline_ int ConvertToSortable(float f)
	{
#ifdef NOT_SO_FAST_BUT_DO_NOT_BREAK_ALIAS_RULE
		int Fi = IntR( f );
#else
		int& Fi = (int&)f;
#endif
		int Fmask = (Fi>>31);
		Fi ^= Fmask;
		Fmask &= ~(1<<31);
		Fi -= Fmask;
		return Fi;
	}

	enum FPUMode
	{
		FPU_FLOOR		= 0,
		FPU_CEIL		= 1,
		FPU_BEST		= 2,

		FPU_FORCE_DWORD	= 0x7fffffff
	};

	FUNCTION FPUMode	GetFPUMode();
	FUNCTION void		SaveFPU();
	FUNCTION void		RestoreFPU();
	FUNCTION void		SetFPUFloorMode();
	FUNCTION void		SetFPUCeilMode();
	FUNCTION void		SetFPUBestMode();

	FUNCTION void		SetFPUPrecision24();
	FUNCTION void		SetFPUPrecision53();
	FUNCTION void		SetFPUPrecision64();
	FUNCTION void		SetFPURoundingChop();
	FUNCTION void		SetFPURoundingUp();
	FUNCTION void		SetFPURoundingDown();
	FUNCTION void		SetFPURoundingNear();

	FUNCTION int		intChop(const float& f);
	FUNCTION int		intFloor(const float& f);
	FUNCTION int		intCeil(const float& f);

#endif // __ICEFPU_H__
