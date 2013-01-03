

/* ����: FastCast
------------------------------------------------------------------------------
    @Remark:
        ��������ת��.
------------------------------------------------------------------------------
*/
inline int FastFloatToInt(float x)
{
#ifdef MW_PLATFORM_WIN32

    int y;

    __asm
    {
        fld     x;
        fistp   y;
    }

    return y;

#else if 0

    int shift = 23 - IEEE_FLOAT_EXPONENT(x);
    int y = (IEEE_FLOAT_MANTISSA(x) | 0x00800000) >> shift;
    int sign = IEEE_FLOAT_SIGN(x);
    y = (y ^ sign) - sign;
    sign = IEEE_FLOAT_EXPONENT_SIGN(x);
    y &= ~sign;
    return y;

else

    return (int)x;

#endif
}



/* ����: Maximum
------------------------------------------------------------------------------
    @Remark:
        ȡ����֮������ֵ.
------------------------------------------------------------------------------
*/
template<class T>
inline const T & Math::Maximum(const T & a, const T & b)
{
    return a > b ? a : b;
}


/* ����: Minimum
------------------------------------------------------------------------------
    @Remark:
        ȡ����֮�����Сֵ.
------------------------------------------------------------------------------
*/
template<class T>
inline const T & Math::Minimum(const T & a, const T & b)
{
    return a < b ? a : b;
}


/* ����: Swap
------------------------------------------------------------------------------
    @Remark:
        ��������.
------------------------------------------------------------------------------
*/
template<class T>
inline void Math::Swap(T & a, T & b)
{
    T temp = a;
    a = b;
    b = temp;
}


/* ����: HighestBitSet
------------------------------------------------------------------------------
    @Remark:
        ȡ�����λΪ1��λ��.
------------------------------------------------------------------------------
*/
inline int Math::HighestBitSet(int x)
{
#ifdef MW_PLATFORM_WIN32 

    int y;

    __asm
    {
        bsr     eax,    x;
        mov     y,      eax;
    }

    return y;

#else

    int tmp = 0x80000000;
    int num = 31;
    while (num > 0 && !(T & tmp))
    {
        --num;
        tmp >>= 1;
    }

    return 0;

#endif

}

/* ����: LowestBitSet
------------------------------------------------------------------------------
    @Remark:
        ȡ�����λΪ1��λ��.
------------------------------------------------------------------------------
*/
inline int Math::LowestBitSet(int x)
{
#ifdef MW_PLATFORM_WIN32

    int y;

    __asm
    {
        bsf     eax,    x;
        mov     y,      eax;
    }

    return y;

#else

    int tmp = 0x00000001;
    int num = 0;

    while (num < 32 && !(x & tmp))
    {
        tmp <<= 1;
        ++num;
    }

    return num;

#endif
}

/* ����: IsPowerOfTwo
------------------------------------------------------------------------------
    @Remark:
        �Ƿ�2�������η�.
------------------------------------------------------------------------------
*/
template<class T>
inline bool Math::IsPowerOfTwo(T x)
{
    d_assert (0);
    return !(x & (x - 1)) && x > 0;
}

template<>
inline bool Math::IsPowerOfTwo(int x)
{
    return !(x & (x - 1)) && x > 0;
}

template<>
inline bool Math::IsPowerOfTwo(float x)
{
    return !(IEEE_FLOAT_MANTISSA(x));
}


/* ����: RoundPowerOfTwo
------------------------------------------------------------------------------
    @Remark:
        ������ӽ�2�������η�(��������)
------------------------------------------------------------------------------
*/
template<class T>
inline T Math::RoundPowerOfTwo(T x)
{
    if (x < 1) 
        return 1;

    int iHighestSet = HighestBitSet(x);
    int iNextBit = x & (1 << (iHighestSet - 1));

    if (iNextBit)
        ++iHighestSet;

    return 1 << iHighestSet;
}

template<>
inline float Math::RoundPowerOfTwo(float x)
{
    int n = INT_BIT(x);

    if ((n & 0xC0000000) != 0x40000000)
        return 1.0f;

    n += (n & 0x00400000) << 1;
    n &= IEEE_FLOAT_EXPONENT_BIT;

    return FLOAT_BIT(n);
}


/* ����: FloorPowerOfTwo
------------------------------------------------------------------------------
    @Remark:
        ������ӽ�2�������η�(����ȥȡ).
------------------------------------------------------------------------------
*/
template<class T>
inline T Math::FloorPowerOfTwo(T x)
{
    if (x < 1) 
        return 1;

    int iHighestSet = HighestBitSet(x);

    return 1 << iHighestSet;
}

template<>
inline float Math::FloorPowerOfTwo(float x)
{
    int n = INT_BIT(x);

    if ((n & 0xC0000000) != 0x40000000)
        return 1.0f;

    n &= IEEE_FLOAT_EXPONENT_BIT;

    return FLOAT_BIT(n);
}



/* ����: CeilPowerOfTwo
------------------------------------------------------------------------------
    @Remark:
        ������ӽ�2�������η�(����ȥȡ).
------------------------------------------------------------------------------
*/
template<class T>
inline T Math::CeilPowerOfTwo(T x)
{
/*
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x++;

    return x;
*/

    if (x < 1) 
        return 1;

    int iHighestSet = HighestBitSet(x);
    int iMask = x & ((1 << iHighestSet) - 1);

    if (iMask)
        ++iHighestSet;

    return 1 << iHighestSet;
}

template<>
inline float Math::CeilPowerOfTwo(float x)
{
    int n = INT_BIT(x);

    if ((n & 0xC0000000) != 0x40000000)
        return 1.0f;

    n += IEEE_FLOAT_MANTISSA_BIT;
    n &= IEEE_FLOAT_EXPONENT_BIT;

    return FLOAT_BIT(n);
}


/* ����: Abs
------------------------------------------------------------------------------
    @Remark:
        ȡ����ֵ.
------------------------------------------------------------------------------
*/
template<class T>
inline T Math::Abs(T x)
{
    return ::abs(x);
}

template<>
inline int Math::Abs(int x)
{
    int Sign = x >> 31;
    return (x ^ Sign) - Sign;
}

template<>
inline float Math::Abs(float x)
{
    int y = INT_BIT(x);
    y &= IEEE_FLOAT_SIGN_MARK;
    return  FLOAT_BIT(y);
}



/* ����: Sign
------------------------------------------------------------------------------
    @Remark:
        ȡ����, С���㷵��-1, ���򷵻�1.
------------------------------------------------------------------------------
*/
template<class T>
inline T Math::Sign(T x)
{
    if (x < 0)
        return (T)-1;
    else
        return (T)1;
}



/* ����: Compare
------------------------------------------------------------------------------
    @Remark:
        �Ƚ�2��ֵ, ���ڷ���true, ���򷵻�false.
------------------------------------------------------------------------------
*/
template<class T>
inline bool Math::Compare(const T & a, const T & b)
{
    return a == b;
}

template<>
inline bool Math::Compare(const float & a, const float & b)
{
    float x = a - b;
    int y = INT_BIT(x);
    y &= IEEE_FLOAT_SIGN_MARK;
    return FLOAT_BIT(y) < DEFAULT_EPSILON;
}


/* ����: Clamp
------------------------------------------------------------------------------
    @Remark:
        ��ȡ, ���ô������ֵ, ����С�����ֵ.
------------------------------------------------------------------------------
*/
template<class T>
inline T Math::Clamp(T x, T low, T hight)
{
    if (x < low)
        return low;

    if (x > hight)
        return hight;

    return x;
}


/* ����: ClampPositive
------------------------------------------------------------------------------
    @Remark:
        ������ȡ, ����С��0.
------------------------------------------------------------------------------
*/
template<class T> 
inline T Math::ClampPositive(T x)
{
    return x > 0 ? x : 0;
}

template<>
inline float Math::ClampPositive(float x)
{
    int n = INT_BIT(x);
    int sign_mask = ~IEEE_FLOAT_SIGN(x);
    n &= sign_mask;
    return FLOAT_BIT(n);
}


/* ����: ClampNegative
------------------------------------------------------------------------------
    @Remark:
        �򸺼�ȡ, ���ô���0.
------------------------------------------------------------------------------
*/
template<class T>
inline T Math::ClampNegative(T x)
{
    return x < 0 ? x : 0;
}

template<>
inline float Math::ClampNegative(float x)
{
    int n = INT_BIT(x);
    int sign_mask = IEEE_FLOAT_SIGN(x);
    n &= sign_mask;
    return FLOAT_BIT(n);
}


/* ����: ClampUnit
------------------------------------------------------------------------------
    @Remark:
        ��λ��ȡ, ���ô���-1, ���ô���1.
------------------------------------------------------------------------------
*/
template<class T>
inline T Math::ClampUnit(T x)
{
    if (x < -1)
        return -1;
    
    if (x > 1)
        return 1;

    return x;
}

template<>
inline float Math::ClampUnit(float x)
{
    int n = INT_BIT(x);
    int exp_sign = IEEE_FLOAT_EXPONENT_SIGN(x);
    int one = (127 << 23) & (~exp_sign);
    n = (n & exp_sign) + one;
    return FLOAT_BIT(n);
}


/* ����: Sqrt
------------------------------------------------------------------------------
    @Remark:
        ��ƽ��.
------------------------------------------------------------------------------
*/
template<class T>
inline T Math::Lerp(const T & a, const T & b, float k)
{
	if (k < 0)
		return a;
	else if (k > 1)
		return b;
	else
		return a + (b - a) * k;
}



/* ����: Sqrt
------------------------------------------------------------------------------
    @Remark:
        ��ƽ��.
------------------------------------------------------------------------------
*/
inline float Math::Sqrt(float x)
{
#ifdef MW_PLATFORM_WIN32

    __asm
    {
        fld     x;
        fsqrt;
        fstp    x;
    }

    return x;

#else
    return ::sqrtf(x);

#endif
}

/* ����: Sin
------------------------------------------------------------------------------
    @Remark:
        ȡ����ֵ.
------------------------------------------------------------------------------
*/
inline float Math::Sin(float x)
{
#ifdef MW_PLATFORM_WIN32

    __asm
    {
        fld     x;
        fsin;
        fstp    x;
    }

    return x;

#else

    return ::sin(x);

#endif
}


/* ����: Cos
------------------------------------------------------------------------------
    @Remark:
        ȡ����ֵ.
------------------------------------------------------------------------------
*/
inline float Math::Cos(float x)
{
#ifdef MW_PLATFORM_WIN32

    __asm
    {
        fld     x;
        fcos;
        fstp    x;
    }

    return x;

#else 

    return ::cos(x);

#endif 

}


/* ����: SinCos
------------------------------------------------------------------------------
    @Remark:
        ȡ����ֵ������ֵ.
------------------------------------------------------------------------------
*/
inline void Math::SinCos(float x, float & s, float & c)
{
#ifdef MW_PLATFORM_WIN32

    __asm
    {
        mov     ecx, c;
        mov     edx, s;
        fld     x;
        fsincos;
        fstp    [ecx];
        fstp    [edx];
    }

#else

    s = ::sinf(x);
    c = ::cosf(x);

#endif
}



/* ����: Tan
------------------------------------------------------------------------------
    @Remark:
        ȡ����ֵ.
------------------------------------------------------------------------------
*/
inline float Math::Tan(float x)
{
#ifdef MW_PLATFORM_WIN32

    __asm
    {
        fld     x;
        fsincos;
        fdiv;
        fstp    x;
    }

    return x;

#else

    ::tanf(x);

#endif
}



/* ����: Tan
------------------------------------------------------------------------------
    @Remark:
        ȡ����ֵ.
------------------------------------------------------------------------------
*/
inline float Math::Cot(float x)
{
#ifdef MW_PLATFORM_WIN32

    __asm
    {
        fld     x;
        fsincos;
        fxch;
        fdiv;
        fstp    x;
    }

    return x;

#else

    return 1.0f / ::tanf(x);

#endif
}


/* ����: ASin
------------------------------------------------------------------------------
    @Remark:
        ȡ������ֵ.
------------------------------------------------------------------------------
*/
inline float Math::ASin(float x)
{
    return ::asinf(x);
}


/* ����: ACos
------------------------------------------------------------------------------
    @Remark:
        ȡ������ֵ.
------------------------------------------------------------------------------
*/
inline float Math::ACos(float x)
{
    return ::acosf(x);
}


/* ����: ATan
------------------------------------------------------------------------------
    @Remark:
        ȡ������ֵ.
------------------------------------------------------------------------------
*/
inline float Math::ATan(float x)
{
#ifdef MW_PLATFORM_WIN32

    __asm
    {
        fld     x;
        fpatan;
        fstp    x;
    }

    return x;

#else

    return ::atanf(x);

#endif
}


/* ����: Sinh
------------------------------------------------------------------------------
    @Remark:
        ����˫���ߺ���.
------------------------------------------------------------------------------
*/
inline float Math::Sinh(float x)
{
    return ::sinhf(x);
}


/* ����: Cosh
------------------------------------------------------------------------------
    @Remark:
        ����˫���ߺ���.
------------------------------------------------------------------------------
*/
inline float Math::Cosh(float x)
{
    return ::coshf(x);
}


/* ����: Tanh
------------------------------------------------------------------------------
    @Remark:
        ����˫���ߺ���.
------------------------------------------------------------------------------
*/
inline float Math::Tanh(float x)
{
    return ::tanhf(x);
}


/* ����: Round
------------------------------------------------------------------------------
    @Remark:
        ��������.
------------------------------------------------------------------------------
*/
inline float Math::Round(float x)
{
#ifdef MW_PLATFORM_WIN32

    __asm
    {
        fld     x;
        frndint;
    }

#else

    int n = INT_BIT(x);
    int sign = n & IEEE_FLOAT_EXPONENT_SIGN;
    n ~= sign;
    x = FLOAT_BIT(n);
    x = ::floorf(x + 0.4999999f);
    n = INT_BIT(x) | sign;

    return FLOAT_BIT(n);

#endif 
}

/* ����: Fmod
------------------------------------------------------------------------------
    @Remark:
        ȡ����.
------------------------------------------------------------------------------
*/
inline float Math::Fmod(float x, float y)
{
    return ::fmod(x, y);
}


/* ����: Floor
------------------------------------------------------------------------------
    @Remark:
        ����ȡ��.
------------------------------------------------------------------------------
*/
inline float Math::Floor(float x)
{
#if 0

    int n = IEEE_FLOAT_MANTISSA(x);

    int shift = 23 - IEEE_FLOAT_EXPONENT(x);
    int y = (IEEE_FLOAT_MANTISSA(x) | 0x00800000) >> shift;
    int sign = IEEE_FLOAT_SIGN(x);
    y = (y ^ sign) - sign;
    sign = IEEE_FLOAT_EXPONENT_SIGN(x);
    y &= ~sign;
    y -= IEEE_FLOAT_SIGN(x) && (IEEE_FLOAT_EXPONENT_SIGN(x) || (IEEE_FLOAT_MANTISSA(x) & ((1 << shift) - 1)));
    return (float)y;

#else

    return (float)::floorf(x);

#endif
}

/* ����: Ceil
------------------------------------------------------------------------------
    @Remark:
        ����ȡ��.
------------------------------------------------------------------------------
*/
inline float Math::Ceil(float x)
{
#if 0

    int shift = 23 - IEEE_FLOAT_EXPONENT(x);
    int y = (IEEE_FLOAT_MANTISSA(x) | 0x00800000) >> shift;
    int sign = IEEE_FLOAT_SIGN(x);
    y = (y ^ sign) - sign;
    sign = IEEE_FLOAT_EXPONENT_SIGN(x);
    y &= ~sign;
    y += !IEEE_FLOAT_SIGN(x) && (IEEE_FLOAT_EXPONENT_SIGN(x) || (IEEE_FLOAT_MANTISSA(x) & ((1 << shift) - 1)));
    return (float)y;

#else

    return ::ceilf(x);

#endif
}


/* ����: Exp
------------------------------------------------------------------------------
    @Remark:
        e��x�η�.
------------------------------------------------------------------------------
*/
inline float Math::Exp(float x)
{
    return ::expf(x);
}


/* ����: Log
------------------------------------------------------------------------------
    @Remark:
        x��2�Ķ���.
------------------------------------------------------------------------------
*/
inline float Math::Log(float x)
{
    return ::logf(x);
}


/* ����: Pow
------------------------------------------------------------------------------
    @Remark:
        x��y�η�.
------------------------------------------------------------------------------
*/
inline float Math::Pow(float x, float y)
{
    return ::powf(x, y);
}


/* ����: SRandom
------------------------------------------------------------------------------
    @Remark:
        �������������.
------------------------------------------------------------------------------
*/
inline void Math::SRandom(int seed)
{
    RAND_SEED = seed;
}


/* ����: Random
------------------------------------------------------------------------------
    @Remark:
        ���������.
------------------------------------------------------------------------------
*/
inline int Math::Random()
{
    RAND_SEED = (RAND_SEED * 123 + 59) & RANDOM_MAX; 
    return RAND_SEED;
}


inline float Math::RandomFloat()
{
    return  (float)Random() / (float)RANDOM_MAX;
}


/* ����: RandRange
------------------------------------------------------------------------------
    @Remark:
        ���������.
------------------------------------------------------------------------------
*/
template<class T>
inline T Math::RandRange(T a, T b)
{
    return a + (T)(Random() % (b - a + 1)); 
}

template<>
inline float Math::RandRange(float a, float b)
{
    return a + (b - a) * RandomFloat();
}


/* ����: DegreeToRadian
------------------------------------------------------------------------------
    @Remark:
        �Ƕ�ת����.
------------------------------------------------------------------------------
*/
inline float Math::DegreeToRadian(float degree)
{
    static const float DEG_TO_RAD_FACTORY = PI_1 / 180.0f;
    return degree * DEG_TO_RAD_FACTORY;
}


/* ����: RadianToDegree
------------------------------------------------------------------------------
    @Remark:
        ����ת�Ƕ�.
------------------------------------------------------------------------------
*/
inline float Math::RadianToDegree(float rad)
{
    static const float RAD_TO_DEG_FACTORY = 180.0f / PI_1;
    return rad * RAD_TO_DEG_FACTORY;
}


/* ����: DegreeNormalize
------------------------------------------------------------------------------
    @Remark:
        normalize degree in [0, 360).
------------------------------------------------------------------------------
*/
inline float Math::DegreeNormalize(float x)
{
    if (x >= 360.0f || x < 0)
    {
        x -= 360.0f * Floor(x / 360.0f);
    }

    return x;
}


/* ����: RadianNormalize
------------------------------------------------------------------------------
    @Remark:
        normalize radian in [0, 2pi).
------------------------------------------------------------------------------
*/
inline float Math::RadianNormalize(float x)
{
    if (x >= PI_2 || x < 0)
    {
        x -= PI_2 * Floor(x / PI_2);
    }

    return x;
}


/* :) ComputeVariance
------------------------------------------------------------------------------
    @Remark:
        ���׼��̬��.
                        n
        var = sqrt ( (  E(xi - avg)^2 / (n - 1)  ) )     
                        1
------------------------------------------------------------------------------
*/
inline float Math:: ComputeVariance(float * Array, int iCount)
{
    float sum = 0.0f, avg = 0.0f, var = 0.0f;
    for (int i = 0; i < iCount; ++i)
    {
        sum += Array[i];
    }

    avg = sum / (float)iCount;

    for (int i = 0; i < iCount; ++i)
    {
        var += (Array[i] - avg) * (Array[i] - avg);
    }

    var /= iCount - 1;
    var = Sqrt(var);

    return var;
}

/* :) ComputeGaussian
------------------------------------------------------------------------------
    @Remark:
        ��˹�ֲ�.
                                             -(x - miu)^2 / (2 * rou^2)
        G(x) = (1 / (sqrt(2PI) * rou)) * e ^ 
                                            -((x - miu)^2 + (y - miu)^2)) / (2 * rou^2)
        G(x, y) = (1 / (2PI * rou^2)) * e ^ 
------------------------------------------------------------------------------
*/
inline float Math::ComputeGaussian(float x, float mid, float var)
{
    static const float InvSqrt2PI = 1.0f / Sqrt(PI_2);
    float dx = x - mid;

    var = 1.0f / var;
    float exp_param = -dx * dx * 2.0f * var * var;

    return InvSqrt2PI * var * Exp(exp_param);
}

inline float Math::ComputeGaussian(float x, float y, float mid_x, float mid_y, float var)
{
    static const float Inv2PI = PI_INV * 0.5f;
    float dx = x - mid_x;
    float dy = y - mid_y;

    var = 1.0f / var;
    var *= var;

    float exp_param = -(dx * dx + dy * dy) * 2.0f * var;

    return Inv2PI * var * Exp(exp_param);
}




/* ����: FastInverse
------------------------------------------------------------------------------
    @Remark:
        �����󸡵㵹��(Ҳ�첻�˶���).
------------------------------------------------------------------------------
*/
inline float Math::FastInverse(float x)
{
    debug_assert(!Compare(x, 0.0f), "0 can't inverse");
    //�����ֵx0
    int n = 2 * 0x03F800000 - INT_BIT(x);
    float x0 = FLOAT_BIT(n);

    //ţ�ٵ����� f(x) = 1 - ax = 0
    x0 = x0 * (2.0f - x * x0);
    return x0;
}


/* : FastClamp0_1
------------------------------------------------------------------------------
        Fast Clamp 0 - 1
------------------------------------------------------------------------------
*/
inline float Math::FastClamp0_1(float x)
{
    float_int fi;
    int sign;

    fi.f = x;
    sign = fi.i >> 31;
    fi.i &= ~sign;

    fi.f = 1.0f - fi.f;
    sign = fi.i >> 31;
    fi.i &= ~sign;
    fi.f = 1.0f - fi.f;

    return fi.f;
}

inline float Math::FastClamp(float x, float a, float b)
{
    float_int fi;
    int sign;

    fi.f = x - a;
    sign = fi.i >> 31;
    fi.i &= ~sign;
    fi.f = fi.f + a;

    fi.f = b - fi.f;
    sign = fi.i >> 31;
    fi.i &= ~sign;
    fi.f = b - fi.f;

    return fi.f;
}

inline float Math::FastClampNegative(float x)
{
    float_int fi;
    int sign;

    fi.f = x;
    sign = fi.i >> 31;
    fi.i &= sign;

    return fi.f;
}

inline float Math::FastClampPositive(float x)
{
    float_int fi;
    int sign;

    fi.f = x;
    sign = fi.i >> 31;
    fi.i &= ~sign;

    return fi.f;
}

inline float Math::FastIntToFloat_23(int x)
{
    float_int y1, y2;

    y1.i = x;
    y2.i = ((23 + 127) << 23) + (1 << 22);
    y1.i += y2.i;
    y1.f -= y2.f;

    return y1.f;
}

inline int Math::FastFloatToInt_23(float x)
{
    float_int fi;

    fi.f = x;
    fi.f += (1 << 23) + (1 << 22);
    fi.i &= (1 << 23) - 1;
    fi.i -= 1 << 22;

    return fi.i;
}


/* ����: FastSinAnlge
------------------------------------------------------------------------------
    @Remark:
        �������Һ���(���).
------------------------------------------------------------------------------
*/
inline float Math::FastSinAngle(float x)
{
    int Sign = INT_BIT(x) & IEEE_FLOAT_SIGN_BIT;
    x = Math::Abs(x * TRI_TABLE_ANGLE_MUL);
    int index = (int)x;
    x = x - index;
    index = index & TRI_TABLE_MASK;
    x = SIN_TABLE[index] + x * (SIN_TABLE[index + 1] - SIN_TABLE[index]);
    index = INT_BIT(x) ^ Sign;
    return FLOAT_BIT(index);
}

/* ����: FastCosAnlge
------------------------------------------------------------------------------
    @Remark:
        �������Һ���(���).
------------------------------------------------------------------------------
*/
inline float Math::FastCosAngle(float x)
{
    x = Math::Abs(x * TRI_TABLE_ANGLE_MUL);
    int index = (int)x;
    x -= index;
    index = index & TRI_TABLE_MASK;
    return COS_TABLE[index] + x * (COS_TABLE[index + 1] - COS_TABLE[index]);
}

/* ����: FastTanAnlge
------------------------------------------------------------------------------
    @Remark:
        �������к���(���).
------------------------------------------------------------------------------
*/
inline float Math::FastTanAnlge(float x)
{
    int Sign = INT_BIT(x) & IEEE_FLOAT_SIGN_BIT;
    x = Math::Abs(x * TRI_TABLE_ANGLE_MUL);
    int index = (int)x;
    x = x - index;
    index = index & TRI_TABLE_MASK;
    x = TAN_TABLE[index] + x * (TAN_TABLE[index + 1] - TAN_TABLE[index]);
    index = INT_BIT(x) ^ Sign;
    return FLOAT_BIT(index);
}


/* ����: FastSin
------------------------------------------------------------------------------
    @Remark:
        �������Һ���(���).
------------------------------------------------------------------------------
*/
inline float Math::FastSin(float x)
{
    int Sign = INT_BIT(x) & IEEE_FLOAT_SIGN_BIT;
    x = Math::Abs(x * TRI_TABLE_RADIAN_MUL);
    int index = (int)x;
    x = x - index;
    index = index & TRI_TABLE_MASK;
    x = SIN_TABLE[index] + x * (SIN_TABLE[index + 1] - SIN_TABLE[index]);
    index = INT_BIT(x) ^ Sign;
    return FLOAT_BIT(index);
}


/* ����: FastCos
------------------------------------------------------------------------------
    @Remark:
        ���������Һ���(���).
------------------------------------------------------------------------------
*/
inline float Math::FastCos(float x)
{
    x = Math::Abs(x * TRI_TABLE_RADIAN_MUL);
    int index = (int)x;
    x -= index;
    index = index & TRI_TABLE_MASK;
    return COS_TABLE[index] + x * (COS_TABLE[index + 1] - COS_TABLE[index]);
}


/* ����: FastSin
------------------------------------------------------------------------------
    @Remark:
        �������Һ���(���).
------------------------------------------------------------------------------
*/
inline float Math::FastTan(float x)
{
    int Sign = INT_BIT(x) & IEEE_FLOAT_SIGN_BIT;
    x = Math::Abs(x * TRI_TABLE_RADIAN_MUL);
    int index = (int)x;
    x = x - index;
    index = index & TRI_TABLE_MASK;
    x = TAN_TABLE[index] + x * (TAN_TABLE[index + 1] - TAN_TABLE[index]);
    index = INT_BIT(x) ^ Sign;
    return FLOAT_BIT(index);
}


/* ����: FastSqrt
------------------------------------------------------------------------------
    @Remark:
        ������ƽ����(���).
------------------------------------------------------------------------------
*/
inline float Math::FastSqrt(float x)
{
    debug_assert(x >= 0.0f, "x is negative!");

    int n = INT_BIT(x);

    n = (((n + IEEE_FLOAT_STEP_BIT) >> 1) & IEEE_FLOAT_EXPONENT_BIT) | SQRT_TABLE[(n >> 8) & 0xFFFF];
    n *= (x != 0);

    return FLOAT_BIT(n);
}


/* ����: FastInvSqrt
------------------------------------------------------------------------------
    @Remark:
        ������ƽ��������(���).
------------------------------------------------------------------------------
*/
#pragma warning(push)
#pragma warning(disable : 4307)

inline float Math::FastInvSqrt(float x)
{
    debug_assert(x >= DEFAULT_EPSILON, "x is negative or zero!");

    int n = INT_BIT(x);

    n = ((( (IEEE_FLOAT_STEP_BIT * 3) - (n & IEEE_FLOAT_EXPONENT_BIT) ) >> 1) 
           & IEEE_FLOAT_EXPONENT_BIT) | INV_SQRT_TABLE[(n >> 8) & 0xFFFF];

    float r = FLOAT_BIT(n);
    r = r * (1.5f - 0.5f * x * r * r);

    return r;
}

#pragma warning(pop)

/* :) Log2le 
------------------------------------------------------------------------------
    @Remark:
        lower equal log2.
------------------------------------------------------------------------------
*/
inline int Math::Log2le(int x)
{
    int log2x = 0;
    int t = 0x10000;

    //t = 0x10000
    if (x >= t)
    {
        log2x += 16;
        t <<= 8;
    }
    else
    {
        t >>= 9;
    }

    //t = 0x1000000 or 0x100
    if (x >= t)
    {
        log2x += 8;
        t <<= 4;
    }
    else
    {
        t >>= 4;
    }

    //t = 0x10000000 or 0x10
    if (x >= t)
    {
        log2x += 4;
        t <<= 2;
    }
    else
    {
        t >>= 2;
    }

    //t = 0x80000000 or 0x04
    if (x >= t)
    {
        log2x += 2;
        t <<= 1;
    }
    else
    {
        t >>= 1;
    }

    // t = 0x00 or 0x02
    if (x > t)
    {
        log2x += 1;
    }

    return log2x;
}


/* :) Log2le 
------------------------------------------------------------------------------
    @Remark:
        greater equal log2.
------------------------------------------------------------------------------
*/
inline int Math::Log2ge(int x)
{
    int log2x = 0;
    int t = 0x8000;

    if (x > 0x80000000)
    {
        return 32;
    }

    if (x > t)
    {
        log2x += 16;
        t <<= 8;
    }
    else
    {
        t >>= 8;
    }

    if (x > t)
    {
        log2x += 8;
        t <<= 4;
    }
    else
    {
        t >>= 4;
    }

    if (x >= t)
    {
        log2x += 4;
        t <<= 2;
    }
    else
    {
        t >>= 2;
    }

    if (x >= t)
    {
        log2x += 2;
        t <<= 1;
    }
    else
    {
        t >>= 1;
    }

    if (x > t)
    {
        log2x += 1;
    }

    return log2x;
}























