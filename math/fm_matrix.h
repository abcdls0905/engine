#ifndef _FM_MATRIX_H
#define _FM_MATRIX_H

/****************************************************************************
** Defines
****************************************************************************/
#define MAT00 0
#define MAT01 1
#define MAT02 2
#define MAT03 3
#define MAT10 4
#define MAT11 5
#define MAT12 6
#define MAT13 7
#define MAT20 8
#define MAT21 9
#define MAT22 10
#define MAT23 11
#define MAT30 12
#define MAT31 13
#define MAT32 14
#define MAT33 15


/****************************************************************************
** Typedefs
****************************************************************************/
/*!***************************************************************************
2D floating point vector
*****************************************************************************/
typedef struct
{
	float x;	/*!< x coordinate */
	float y;	/*!< y coordinate */
} FmVECTOR2f;

/*!***************************************************************************
2D fixed point vector
*****************************************************************************/
typedef struct
{
	int x;	/*!< x coordinate */
	int y;	/*!< y coordinate */
} FmVECTOR2x;

/*!***************************************************************************
3D floating point vector
*****************************************************************************/
typedef struct
{
	float x;	/*!< x coordinate */
	float y;	/*!< y coordinate */
	float z;	/*!< z coordinate */
} FmVECTOR3f;

/*!***************************************************************************
3D fixed point vector
*****************************************************************************/
typedef struct
{
	int x;	/*!< x coordinate */
	int y;	/*!< y coordinate */
	int z;	/*!< z coordinate */
} FmVECTOR3x;

/*!***************************************************************************
4D floating point vector
*****************************************************************************/
typedef struct
{
	float x;	/*!< x coordinate */
	float y;	/*!< y coordinate */
	float z;	/*!< z coordinate */
	float w;	/*!< w coordinate */
} FmVECTOR4f;

/*!***************************************************************************
4D fixed point vector
*****************************************************************************/
typedef struct
{
	int x;	/*!< x coordinate */
	int y;	/*!< y coordinate */
	int z;	/*!< z coordinate */
	int w;	/*!< w coordinate */
} FmVECTOR4x;

/*!***************************************************************************
  floating point plane
*****************************************************************************/
typedef struct
{
	float a; /*!< The a coefficient of the clipping plane in the general plane equation.*/
	float b; /*!< The b coefficient of the clipping plane in the general plane equation.*/
	float c; /*!< The c coefficient of the clipping plane in the general plane equation.*/
	float d; /*!< The d coefficient of the clipping plane in the general plane equation.*/
} FmPLANEf;

/*!***************************************************************************
  floating point plane
*****************************************************************************/
typedef struct
{
	int a; /*!< The a coefficient of the clipping plane in the general plane equation.*/
	int b; /*!< The b coefficient of the clipping plane in the general plane equation.*/
	int c; /*!< The c coefficient of the clipping plane in the general plane equation.*/
	int d; /*!< The d coefficient of the clipping plane in the general plane equation.*/
} FmPLANEx;



/*!***************************************************************************
4x4 floating point matrix
*****************************************************************************/

typedef struct FmMATRIXf
{
#ifdef __cplusplus
public:
	float* operator [] ( const int Row )
	{
		return &f[Row<<2];
	}
#endif //__cplusplus
	union
	{
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;

		};
		float m[4][4];
		float f[16];	/*!< Array of float */
	};
}FmMATRIXf, *PFmMATRIXf;

/*!***************************************************************************
4x4 fixed point matrix
*****************************************************************************/
typedef struct FmMATRIXx
{
#ifdef __cplusplus
public:
	int* operator [] ( const int Row )
	{
		return &f[Row<<2];
	}
#endif //__cplusplus
	union 
	{
		struct {
			int        _11, _12, _13, _14;
			int        _21, _22, _23, _24;
			int        _31, _32, _33, _34;
			int        _41, _42, _43, _44;

		};
		int m[4][4];
		int f[16];
	};
}FmMATRIXx, *PFmMATRIXx;

/*!***************************************************************************
3x3 floating point matrix
*****************************************************************************/

typedef struct FmMATRIX3f
{
#ifdef __cplusplus
public:
	float* operator [] ( const int Row )
	{
		return &f[Row*3];
	}
#endif //__cplusplus
	float f[9];	/*!< Array of float */
}FmMATRIX3f, *PFmMATRIX3f;

/*!***************************************************************************
3x3 fixed point matrix
*****************************************************************************/
typedef struct FmMATRIX3x
{
#ifdef __cplusplus
public:
	int* operator [] ( const int Row )
	{
		return &f[Row*3];
	}
#endif //__cplusplus
	int f[9];
}FmMATRIX3x, *PFmMATRIX3x;


/****************************************************************************
** Float or fixed
****************************************************************************/
#ifdef FM_FIXED_POINT_ENABLE
typedef FmPLANEx        FmPLANE;
typedef FmVECTOR2x		FmVECTOR2;
typedef FmVECTOR3x		FmVECTOR3;
typedef FmVECTOR4x		FmVECTOR4;
typedef FmMATRIX3x		FmMATRIX3;
typedef FmMATRIXx		FmMATRIX;
#define FmMatrixIdentity				FmMatrixIdentityX
#define FmMatrixMultiply				FmMatrixMultiplyX
#define FmMatrixTranslation				FmMatrixTranslationX
#define FmMatrixScaling					FmMatrixScalingX
#define FmMatrixRotationX				FmMatrixRotationXX
#define FmMatrixRotationY				FmMatrixRotationYX
#define FmMatrixRotationZ				FmMatrixRotationZX
#define FmMatrixTranspose				FmMatrixTransposeX
#define FmMatrixInverse					FmMatrixInverseX
#define FmMatrixInverseEx				FmMatrixInverseExX
#define FmMatrixLookAtLH				FmMatrixLookAtLHX
#define FmMatrixLookAtRH				FmMatrixLookAtRHX
#define FmMatrixPerspectiveFovLH		FmMatrixPerspectiveFovLHX
#define FmMatrixPerspectiveFovRH		FmMatrixPerspectiveFovRHX
#define FmMatrixOrthoLH					FmMatrixOrthoLHX
#define FmMatrixOrthoRH					FmMatrixOrthoRHX
#define FmMatrixVec3Lerp				FmMatrixVec3LerpX
#define FmMatrixVec3DotProduct			FmMatrixVec3DotProductX
#define FmMatrixVec3CrossProduct		FmMatrixVec3CrossProductX
#define FmMatrixVec3Normalize			FmMatrixVec3NormalizeX
#define FmMatrixVec3Length				FmMatrixVec3LengthX
#define FmMatrixLinearEqSolve			FmMatrixLinearEqSolveX
#else
typedef FmPLANEf        FmPLANE;
typedef FmVECTOR2f		FmVECTOR2;
typedef FmVECTOR3f		FmVECTOR3;
typedef FmVECTOR4f		FmVECTOR4;
typedef FmMATRIX3f		FmMATRIX3;
typedef FmMATRIXf		FmMATRIX;
#define FmMatrixIdentity				FmMatrixIdentityF
#define FmMatrixMultiply				FmMatrixMultiplyF
#define FmMatrixTranslation				FmMatrixTranslationF
#define FmMatrixScaling					FmMatrixScalingF
#define FmMatrixRotationX				FmMatrixRotationXF
#define FmMatrixRotationY				FmMatrixRotationYF
#define FmMatrixRotationZ				FmMatrixRotationZF
#define FmMatrixTranspose				FmMatrixTransposeF
#define FmMatrixInverse					FmMatrixInverseF
#define FmMatrixInverseEx				FmMatrixInverseExF
#define FmMatrixLookAtLH				FmMatrixLookAtLHF
#define FmMatrixLookAtRH				FmMatrixLookAtRHF
#define FmMatrixPerspectiveFovLH		FmMatrixPerspectiveFovLHF
#define FmMatrixPerspectiveFovRH		FmMatrixPerspectiveFovRHF
#define FmMatrixOrthoLH					FmMatrixOrthoLHF
#define FmMatrixOrthoRH					FmMatrixOrthoRHF
#define FmMatrixVec3Lerp				FmMatrixVec3LerpF
#define FmMatrixVec3DotProduct			FmMatrixVec3DotProductF
#define FmMatrixVec3CrossProduct		FmMatrixVec3CrossProductF
#define FmMatrixVec3Normalize			FmMatrixVec3NormalizeF
#define FmMatrixVec3Length				FmMatrixVec3LengthF
#define FmMatrixLinearEqSolve			FmMatrixLinearEqSolveF
#endif

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
** Functions
****************************************************************************/

/*!***************************************************************************
@Function			FmMatrixIdentityF
@Output			mOut	Set to identity
@Description		Reset matrix to identity matrix.
*****************************************************************************/
extern void FmMatrixIdentityF(FmMATRIXf *pOut);

/*!***************************************************************************
@Function			FmMatrixIdentityX
@Output			mOut	Set to identity
@Description		Reset matrix to identity matrix.
*****************************************************************************/
extern void FmMatrixIdentityX(FmMATRIXx *pOut);

/*!***************************************************************************
@Function			FmMatrixMultiplyF
@Output			mOut	Result of mA x mB
@Input				mA		First operand
@Input				mB		Second operand
@Description		Multiply mA by mB and assign the result to mOut
(mOut = p1 * p2). A copy of the result matrix is done in
the function because mOut can be a parameter mA or mB.
*****************************************************************************/
extern void FmMatrixMultiplyF(
					   FmMATRIXf			*pOut,
					   const FmMATRIXf	*pA,
					   const FmMATRIXf	*pB);
/*!***************************************************************************
@Function			FmMatrixMultiplyX
@Output			mOut	Result of mA x mB
@Input				mA		First operand
@Input				mB		Second operand
@Description		Multiply mA by mB and assign the result to mOut
(mOut = p1 * p2). A copy of the result matrix is done in
the function because mOut can be a parameter mA or mB.
The fixed-point shift could be performed after adding
all four intermediate results together however this might
cause some overflow issues.
*****************************************************************************/
extern void FmMatrixMultiplyX(
					   FmMATRIXx			*pOut,
					   const FmMATRIXx	*pA,
					   const FmMATRIXx	*pB);

/*!***************************************************************************
@Function Name		FmMatrixTranslationF
@Output			mOut	Translation matrix
@Input				fX		X component of the translation
@Input				fY		Y component of the translation
@Input				fZ		Z component of the translation
@Description		Build a transaltion matrix mOut using fX, fY and fZ.
*****************************************************************************/
extern void FmMatrixTranslationF(
						  FmMATRIXf	*pOut,
						  const float	fX,
						  const float	fY,
						  const float	fZ);
/*!***************************************************************************
@Function Name		FmMatrixTranslationX
@Output			mOut	Translation matrix
@Input				fX		X component of the translation
@Input				fY		Y component of the translation
@Input				fZ		Z component of the translation
@Description		Build a transaltion matrix mOut using fX, fY and fZ.
*****************************************************************************/
extern void FmMatrixTranslationX(
						  FmMATRIXx	*pOut,
						  const int	fX,
						  const int	fY,
						  const int	fZ);

/*!***************************************************************************
@Function Name		FmMatrixScalingF
@Output			mOut	Scale matrix
@Input				fX		X component of the scaling
@Input				fY		Y component of the scaling
@Input				fZ		Z component of the scaling
@Description		Build a scale matrix mOut using fX, fY and fZ.
*****************************************************************************/
extern void FmMatrixScalingF(
					  FmMATRIXf	*pOut,
					  const float fX,
					  const float fY,
					  const float fZ);

/*!***************************************************************************
@Function Name		FmMatrixScalingX
@Output			mOut	Scale matrix
@Input				fX		X component of the scaling
@Input				fY		Y component of the scaling
@Input				fZ		Z component of the scaling
@Description		Build a scale matrix mOut using fX, fY and fZ.
*****************************************************************************/
extern void FmMatrixScalingX(
					  FmMATRIXx	*pOut,
					  const int	fX,
					  const int	fY,
					  const int	fZ);

/*!***************************************************************************
@Function Name		FmMatrixRotationXF
@Output			mOut	Rotation matrix
@Input				fAngle	Angle of the rotation
@Description		Create an X rotation matrix mOut.
*****************************************************************************/
extern void FmMatrixRotationXF(
						FmMATRIXf	*pOut,
						const float fAngle);

/*!***************************************************************************
@Function Name		FmMatrixRotationXX
@Output			mOut	Rotation matrix
@Input				fAngle	Angle of the rotation
@Description		Create an X rotation matrix mOut.
*****************************************************************************/
extern void FmMatrixRotationXX(
						FmMATRIXx	*pOut,
						const int	fAngle);

/*!***************************************************************************
@Function Name		FmMatrixRotationYF
@Output			mOut	Rotation matrix
@Input				fAngle	Angle of the rotation
@Description		Create an Y rotation matrix mOut.
*****************************************************************************/
extern void FmMatrixRotationYF(
						FmMATRIXf	*pOut,
						const float fAngle);

/*!***************************************************************************
@Function Name		FmMatrixRotationYX
@Output			mOut	Rotation matrix
@Input				fAngle	Angle of the rotation
@Description		Create an Y rotation matrix mOut.
*****************************************************************************/
extern void FmMatrixRotationYX(
						FmMATRIXx	*pOut,
						const int	fAngle);

/*!***************************************************************************
@Function Name		FmMatrixRotationZF
@Output			mOut	Rotation matrix
@Input				fAngle	Angle of the rotation
@Description		Create an Z rotation matrix mOut.
*****************************************************************************/
extern void FmMatrixRotationZF(
						FmMATRIXf	*pOut,
						const float fAngle);
/*!***************************************************************************
@Function Name		FmMatrixRotationZX
@Output			mOut	Rotation matrix
@Input				fAngle	Angle of the rotation
@Description		Create an Z rotation matrix mOut.
*****************************************************************************/
extern void FmMatrixRotationZX(
						FmMATRIXx	*pOut,
						const int	fAngle);

/*!***************************************************************************
@Function Name		FmMatrixTransposeF
@Output			mOut	Transposed matrix
@Input				mIn		Original matrix
@Description		Compute the transpose matrix of mIn.
*****************************************************************************/
extern void FmMatrixTransposeF(
						FmMATRIXf			*pOut,
						const FmMATRIXf	*pIn);
/*!***************************************************************************
@Function Name		FmMatrixTransposeX
@Output			mOut	Transposed matrix
@Input				mIn		Original matrix
@Description		Compute the transpose matrix of mIn.
*****************************************************************************/
extern void FmMatrixTransposeX(
						FmMATRIXx			*pOut,
						const FmMATRIXx	*pIn);

/*!***************************************************************************
@Function			FmMatrixInverseF
@Output			mOut	Inversed matrix
@Input				mIn		Original matrix
@Description		Compute the inverse matrix of mIn.
The matrix must be of the form :
A 0
C 1
Where A is a 3x3 matrix and C is a 1x3 matrix.
*****************************************************************************/
extern void FmMatrixInverseF(
					  FmMATRIXf			*pOut,
					  const FmMATRIXf	*pIn);
/*!***************************************************************************
@Function			FmMatrixInverseX
@Output			mOut	Inversed matrix
@Input				mIn		Original matrix
@Description		Compute the inverse matrix of mIn.
The matrix must be of the form :
A 0
C 1
Where A is a 3x3 matrix and C is a 1x3 matrix.
*****************************************************************************/
extern void FmMatrixInverseX(
					  FmMATRIXx			*pOut,
					  const FmMATRIXx	*pIn);

/*!***************************************************************************
@Function			FmMatrixInverseExF
@Output			mOut	Inversed matrix
@Input				mIn		Original matrix
@Description		Compute the inverse matrix of mIn.
Uses a linear equation solver and the knowledge that M.M^-1=I.
Use this fn to calculate the inverse of matrices that
FmMatrixInverse() cannot.
*****************************************************************************/
extern void FmMatrixInverseExF(
						FmMATRIXf			*pOut,
						const FmMATRIXf	*pIn);
/*!***************************************************************************
@Function			FmMatrixInverseExX
@Output			mOut	Inversed matrix
@Input				mIn		Original matrix
@Description		Compute the inverse matrix of mIn.
Uses a linear equation solver and the knowledge that M.M^-1=I.
Use this fn to calculate the inverse of matrices that
FmMatrixInverse() cannot.
*****************************************************************************/
extern void FmMatrixInverseExX(
						FmMATRIXx			*pOut,
						const FmMATRIXx	*pIn);

/*!***************************************************************************
@Function			FmMatrixLookAtLHF
@Output			mOut	Look-at view matrix
@Input				vEye	Position of the camera
@Input				vAt		Point the camera is looking at
@Input				vUp		Up direction for the camera
@Description		Create a look-at view matrix.
*****************************************************************************/
extern void FmMatrixLookAtLHF(
					   FmMATRIXf			*pOut,
					   const FmVECTOR3f	*pvEye,
					   const FmVECTOR3f	*pvAt,
					   const FmVECTOR3f	*pvUp);
/*!***************************************************************************
@Function			FmMatrixLookAtLHX
@Output			mOut	Look-at view matrix
@Input				vEye	Position of the camera
@Input				vAt		Point the camera is looking at
@Input				vUp		Up direction for the camera
@Description		Create a look-at view matrix.
*****************************************************************************/
extern void FmMatrixLookAtLHX(
					   FmMATRIXx			*pOut,
					   const FmVECTOR3x	*pvEye,
					   const FmVECTOR3x	*pvAt,
					   const FmVECTOR3x	*pvUp);

/*!***************************************************************************
@Function			FmMatrixLookAtRHF
@Output			mOut	Look-at view matrix
@Input				vEye	Position of the camera
@Input				vAt		Point the camera is looking at
@Input				vUp		Up direction for the camera
@Description		Create a look-at view matrix.
*****************************************************************************/
extern void FmMatrixLookAtRHF(
					   FmMATRIXf			*pOut,
					   const FmVECTOR3f	*pvEye,
					   const FmVECTOR3f	*pvAt,
					   const FmVECTOR3f	*pvUp);
/*!***************************************************************************
@Function			FmMatrixLookAtRHX
@Output			mOut	Look-at view matrix
@Input				vEye	Position of the camera
@Input				vAt		Point the camera is looking at
@Input				vUp		Up direction for the camera
@Description		Create a look-at view matrix.
*****************************************************************************/
extern void FmMatrixLookAtRHX(
					   FmMATRIXx			*pOut,
					   const FmVECTOR3x	*pvEye,
					   const FmVECTOR3x	*pvAt,
					   const FmVECTOR3x	*pvUp);

/*!***************************************************************************
@Function		FmMatrixPerspectiveFovLHF
@Output		mOut		Perspective matrix
@Input			fFOVy		Field of view
@Input			fAspect		Aspect ratio
@Input			fNear		Near clipping distance
@Input			fFar		Far clipping distance
@Input			bRotate		Should we rotate it ? (for upright screens)(defaut = false)
@
@Description	Create a perspective matrix.
*****************************************************************************/
extern void FmMatrixPerspectiveFovLHF(
							   FmMATRIXf	*pOut,
							   const float	fFOVy,
							   const float	fAspect,
							   const float	fNear,
							   const float	fFar,
							   const int  bRotate );
/*!***************************************************************************
@Function		FmMatrixPerspectiveFovLHX
@Output		mOut		Perspective matrix
@Input			fFOVy		Field of view
@Input			fAspect		Aspect ratio
@Input			fNear		Near clipping distance
@Input			fFar		Far clipping distance
@Input			bRotate		Should we rotate it ? (for upright screens) (defaut = false)
@Description	Create a perspective matrix.
*****************************************************************************/
extern void FmMatrixPerspectiveFovLHX(
							   FmMATRIXx	*pOut,
							   const int	fFOVy,
							   const int	fAspect,
							   const int	fNear,
							   const int	fFar,
							   const int  bRotate );

/*!***************************************************************************
@Function		FmMatrixPerspectiveFovRHF
@Output		mOut		Perspective matrix
@Input			fFOVy		Field of view
@Input			fAspect		Aspect ratio
@Input			fNear		Near clipping distance
@Input			fFar		Far clipping distance
@Input			bRotate		Should we rotate it ? (for upright screens)
@Description	Create a perspective matrix.
*****************************************************************************/
extern void FmMatrixPerspectiveFovRHF(
							   FmMATRIXf	*pOut,
							   const float	fFOVy,
							   const float	fAspect,
							   const float	fNear,
							   const float	fFar,
							   const int  bRotate );
/*!***************************************************************************
@Function		FmMatrixPerspectiveFovRHX
@Output		mOut		Perspective matrix
@Input			fFOVy		Field of view
@Input			fAspect		Aspect ratio
@Input			fNear		Near clipping distance
@Input			fFar		Far clipping distance
@Input			bRotate		Should we rotate it ? (for upright screens)
@Description	Create a perspective matrix.
*****************************************************************************/
extern void FmMatrixPerspectiveFovRHX(
							   FmMATRIXx	*pOut,
							   const int	fFOVy,
							   const int	fAspect,
							   const int	fNear,
							   const int	fFar,
							   const int  bRotate );

/*!***************************************************************************
@Function		FmMatrixOrthoLHF
@Output		mOut		Orthographic matrix
@Input			w			Width of the screen
@Input			h			Height of the screen
@Input			zn			Near clipping distance
@Input			zf			Far clipping distance
@Input			bRotate		Should we rotate it ? (for upright screens)
@Description	Create an orthographic matrix.
*****************************************************************************/
extern void FmMatrixOrthoLHF(
					  FmMATRIXf	*pOut,
					  const float w,
					  const float h,
					  const float zn,
					  const float zf,
					  const int  bRotate );
/*!***************************************************************************
@Function		FmMatrixOrthoLHX
@Output		mOut		Orthographic matrix
@Input			w			Width of the screen
@Input			h			Height of the screen
@Input			zn			Near clipping distance
@Input			zf			Far clipping distance
@Input			bRotate		Should we rotate it ? (for upright screens)
@Description	Create an orthographic matrix.
*****************************************************************************/
extern void FmMatrixOrthoLHX(
					  FmMATRIXx	*pOut,
					  const int	w,
					  const int	h,
					  const int	zn,
					  const int	zf,
					  const int  bRotate );

/*!***************************************************************************
@Function		FmMatrixOrthoRHF
@Output		mOut		Orthographic matrix
@Input			w			Width of the screen
@Input			h			Height of the screen
@Input			zn			Near clipping distance
@Input			zf			Far clipping distance
@Input			bRotate		Should we rotate it ? (for upright screens)
@Description	Create an orthographic matrix.
*****************************************************************************/
extern void FmMatrixOrthoRHF(
					  FmMATRIXf	*pOut,
					  const float w,
					  const float h,
					  const float zn,
					  const float zf,
					  const int  bRotate );
/*!***************************************************************************
@Function		FmMatrixOrthoRHX
@Output		mOut		Orthographic matrix
@Input			w			Width of the screen
@Input			h			Height of the screen
@Input			zn			Near clipping distance
@Input			zf			Far clipping distance
@Input			bRotate		Should we rotate it ? (for upright screens)
@Description	Create an orthographic matrix.
*****************************************************************************/
extern void FmMatrixOrthoRHX(
					  FmMATRIXx	*pOut,
					  const int	w,
					  const int	h,
					  const int	zn,
					  const int	zf,
					  const int  bRotate );

/*!***************************************************************************
@Function			FmMatrixVec3LerpF
@Output			vOut	Result of the interpolation
@Input				v1		First vector to interpolate from
@Input				v2		Second vector to interpolate form
@Input				s		Coefficient of interpolation
@Description		This function performs the linear interpolation based on
the following formula: V1 + s(V2-V1).
*****************************************************************************/
extern void FmMatrixVec3LerpF(
					   FmVECTOR3f		*pvOut,
					   const FmVECTOR3f	*pv1,
					   const FmVECTOR3f	*pv2,
					   const float			s);
/*!***************************************************************************
@Function			FmMatrixVec3LerpX
@Output			vOut	Result of the interpolation
@Input				v1		First vector to interpolate from
@Input				v2		Second vector to interpolate form
@Input				s		Coefficient of interpolation
@Description		This function performs the linear interpolation based on
the following formula: V1 + s(V2-V1).
*****************************************************************************/
extern void FmMatrixVec3LerpX(
					   FmVECTOR3x		*pvOut,
					   const FmVECTOR3x	*pv1,
					   const FmVECTOR3x	*pv2,
					   const int			s);

/*!***************************************************************************
@Function			FmMatrixVec3DotProductF
@Input				v1		First vector
@Input				v2		Second vector
@Return			Dot product of the two vectors.
@Description		This function performs the dot product of the two
supplied vectors.
*****************************************************************************/
extern float FmMatrixVec3DotProductF(
							  const FmVECTOR3f	*pv1,
							  const FmVECTOR3f	*pv2);
/*!***************************************************************************
@Function			FmMatrixVec3DotProductX
@Input				v1		First vector
@Input				v2		Second vector
@Return			Dot product of the two vectors.
@Description		This function performs the dot product of the two
supplied vectors.
A single >> 16 shift could be applied to the final accumulated
result however this runs the risk of overflow between the
results of the intermediate additions.
*****************************************************************************/
extern int FmMatrixVec3DotProductX(
							const FmVECTOR3x	*pv1,
							const FmVECTOR3x	*pv2);

/*!***************************************************************************
@Function			FmMatrixVec3CrossProductF
@Output			vOut	Cross product of the two vectors
@Input				v1		First vector
@Input				v2		Second vector
@Description		This function performs the cross product of the two
supplied vectors.
*****************************************************************************/
extern void FmMatrixVec3CrossProductF(
							   FmVECTOR3f		*pvOut,
							   const FmVECTOR3f	*pv1,
							   const FmVECTOR3f	*pv2);
/*!***************************************************************************
@Function			FmMatrixVec3CrossProductX
@Output			vOut	Cross product of the two vectors
@Input				v1		First vector
@Input				v2		Second vector
@Description		This function performs the cross product of the two
supplied vectors.
*****************************************************************************/
extern void FmMatrixVec3CrossProductX(
							   FmVECTOR3x		*pvOut,
							   const FmVECTOR3x	*pv1,
							   const FmVECTOR3x	*pv2);

/*!***************************************************************************
@Function			FmMatrixVec3NormalizeF
@Output			vOut	Normalized vector
@Input				vIn		Vector to normalize
@Description		Normalizes the supplied vector.
*****************************************************************************/
extern void FmMatrixVec3NormalizeF(
							FmVECTOR3f		*pvOut,
							const FmVECTOR3f	*pvIn);
/*!***************************************************************************
@Function			FmMatrixVec3NormalizeX
@Output			vOut	Normalized vector
@Input				vIn		Vector to normalize
@Description		Normalizes the supplied vector.
The square root function is currently still performed
in floating-point.
Original vector is scaled down prior to be normalized in
order to avoid overflow issues.
*****************************************************************************/
extern void FmMatrixVec3NormalizeX(
							FmVECTOR3x		*pvOut,
							const FmVECTOR3x	*pvIn);
/*!***************************************************************************
@Function			FmMatrixVec3LengthF
@Input				vIn		Vector to get the length of
@Return			The length of the vector
@Description		Gets the length of the supplied vector.
*****************************************************************************/
extern float FmMatrixVec3LengthF(
						  const FmVECTOR3f	*pvIn);
/*!***************************************************************************
@Function			FmMatrixVec3LengthX
@Input				vIn		Vector to get the length of
@Return			The length of the vector
@Description		Gets the length of the supplied vector
*****************************************************************************/
extern int FmMatrixVec3LengthX(
						const FmVECTOR3x	*pvIn);
/*!***************************************************************************
@Function			FmMatrixLinearEqSolveF
@Input				pSrc	2D array of floats. 4 Eq linear problem is 5x4
matrix, constants in first column
@Input				nCnt	Number of equations to solve
@Output			pRes	Result
@Description		Solves 'nCnt' simultaneous equations of 'nCnt' variables.
pRes should be an array large enough to contain the
results: the values of the 'nCnt' variables.
This fn recursively uses Gaussian Elimination.
*****************************************************************************/

extern void FmMatrixLinearEqSolveF(
							float		* const pRes,
							float		** const pSrc,
							const int	nCnt);
/*!***************************************************************************
@Function			FmMatrixLinearEqSolveX
@Input				pSrc	2D array of floats. 4 Eq linear problem is 5x4
matrix, constants in first column
@Input				nCnt	Number of equations to solve
@Output			pRes	Result
@Description		Solves 'nCnt' simultaneous equations of 'nCnt' variables.
pRes should be an array large enough to contain the
results: the values of the 'nCnt' variables.
This fn recursively uses Gaussian Elimination.
*****************************************************************************/
extern void FmMatrixLinearEqSolveX(
							int			* const pRes,
							int			** const pSrc,
							const int	nCnt);

#ifdef __cplusplus
}
#endif

#endif
