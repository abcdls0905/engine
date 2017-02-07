#ifndef _FM_FIXEDPOINT_H
#define _FM_FIXEDPOINT_H

#include "../public/macros.h"


#define VERTTYPE float
 

/*!***************************************************************************
Macros
*****************************************************************************/
#define FM_MIN(a,b)            (((a) < (b)) ? (a) : (b))
#define FM_MAX(a,b)            (((a) > (b)) ? (a) : (b))
#define FM_CLAMP(x, l, h)      (FM_MIN((h), FM_MAX((x), (l))))


/* Fixed-point macros */
#define FMF2X(f)		( (int) ( (f)*(65536) ) )
#define FMX2F(x)		((float)(x)/65536.0f)
#define FMXMUL(a,b)	( (int)( ((int64_t)(a)*(b)) / 65536 ) )
#define FMXDIV(a,b)	( (int)( (((int64_t)(a))<<16)/(b) ) )
#define FMABS(a)		((a) <= 0 ? -(a) : (a) )

/* 16bit Fixed-point macros */
// -15 stored using a single precision bias of 127
#define FM_HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP  0x38000000
// max exponent value in single precision that will be converted
// to Inf or Nan when stored as a half-float
#define FM_HALF_FLOAT_MAX_BIASED_EXP_AS_SINGLE_FP_EXP  0x47800000

#define FM_FLOAT_MAX_BIASED_EXP  (0xFF << 23)
#define FM_HALF_FLOAT_MAX_BIASED_EXP (0x1F << 10)

//
#define FM_SMALL_NUMBER		(1.e-8)

/* Define trig table macros */
#include "fm_mathtable.h"

/* Useful values */
#define FM_PI_OVER_TWOf	(3.1415926535f / 2.0f)
#define FM_PIf			(3.1415926535f)
#define FM_TWO_PIf		(3.1415926535f * 2.0f)
#define FM_ONEf		(1.0f)

#define FM_PI_OVER_TWOx	FMF2X(FM_PI_OVER_TWOf)
#define FM_PIx			FMF2X(FM_PIf)
#define FM_TWO_PIx		FMF2X(FM_TWO_PIf)
#define FM_ONEx			FMF2X(FM_ONEf)


/* Fixed-point trig function lookups */
#define FMXCOS(x)		(cos_val[(FMXMUL(((FMXDIV((x)<0? -(x):(x), FM_TWO_PIx)) & 0x0000FFFF), (NUM_ENTRIES-1)))])
#define FMXSIN(x)		(sin_val[(FMXMUL(((FMXDIV((x)<0 ? FM_PIx-(x):(x), FM_TWO_PIx)) & 0x0000FFFF), (NUM_ENTRIES-1)))])
#define FMXTAN(x)		( (x)<0 ? -tan_val[(FMXMUL(((FMXDIV(-(x), FM_TWO_PIx)) & 0x0000FFFF), (NUM_ENTRIES-1)))] : tan_val[(FMXMUL(((FMXDIV(x, FM_TWO_PIx)) & 0x0000FFFF), (NUM_ENTRIES-1)))] )
#define FMXACOS(x)	    (acos_val[FMXMUL(((((x) + FMF2X(1.0f))>>1) & 0x0000FFFF), (NUM_ENTRIES-1))])

/* Floating-point trig functions lookups (needed by some tools chains that have problems with real math functions) */
#ifdef USE_TRIGONOMETRIC_LOOKUP_TABLES

/* If trig tables are forced ON in non-fixed-point builds then convert fixed-point trig tables results to float */
#define	FMFCOS(x)				FMX2F(FMXCOS(FMF2X(x)))
#define	FMFSIN(x)				FMX2F(FMXSIN(FMF2X(x)))
#define	FMFTAN(x)				FMX2F(FMXTAN(FMF2X(x)))
#define	FMFACOS(x)			    FMX2F(FMXACOS(FMF2X(x)))
#define FMFATAN2(a,b)           //Need implement
#define FMFASIN(x)              //Need implement 

#else

/* Trig abstraction macros default to normal math trig functions for full float mode */
#define	FMFCOS(x)				((float)cos(x))
#define	FMFSIN(x)				((float)sin(x))
#define	FMFTAN(x)				((float)tan(x))
#define	FMFACOS(x)			    ((float)acos(x))
#define FMFATAN2(a,b)           ((float)atan2(a,b))
#define FMFASIN(x)              ((float)asin(x))

#endif

/* Fixed/float macro abstraction */
#ifdef FM_FIXED_POINT_ENABLE

/* Fixed-point operations, including trig tables */
#define VERTTYPEMUL(a,b)			FMXMUL(b,a)
#define VERTTYPEDIV(a,b)			FMXDIV(a,b)
#define VERTTYPEABS(a)				FMABS(a)

#define f2vt(f) 					FMF2X(f)
#define vt2f(x) 					FMX2F(x)

#define FM_PI_OVER_TWO			    FM_PI_OVER_TWOx
#define FM_PI						FM_PIx
#define FM_TWO_PI					FM_TWO_PIx
#define FM_ONE					    FM_ONEx

#define	FMCOS(x)					FMXCOS(x)
#define	FMSIN(x)					FMXSIN(x)
#define	FMTAN(x)					FMXTAN(x)
#define	FMACOS(x)					FMXACOS(x)
#define FMATAN2(a,b)                FMFATAN2(a,b)
#define FMASIN(x)                   FMFASIN(x)

#else

/* Floating-point operations */
#define VERTTYPEMUL(a,b)			( (VERTTYPE)((b)*(a)) )//这里主要是解决矩阵乘法问题 去模拟D3D的乘法操作
#define VERTTYPEDIV(a,b)			( (VERTTYPE)((a)/(b)) )
#define VERTTYPEABS(a)				( (VERTTYPE)(fabs(a)) )

#define f2vt(x)						(x)
#define vt2f(x)						(x)

#define FM_PI_OVER_TWO			    FM_PI_OVER_TWOf
#define FM_PI						FM_PIf
#define FM_TWO_PI					FM_TWO_PIf
#define FM_ONE					    FM_ONEf

/* If trig tables are forced ON in non-fixed-point builds then convert fixed-point trig tables results to float */
#define	FMCOS(x)					FMFCOS(x)
#define	FMSIN(x)					FMFSIN(x)
#define	FMTAN(x)					FMFTAN(x)
#define	FMACOS(x)					FMFACOS(x)
#define FMATAN2(a,b)                FMFATAN2(a,b)
#define FMASIN(x)                   FMFASIN(x)

#endif

#define FM_TO_RADIAN(x)     ((x)*(FM_PI / 180.0f))



#endif