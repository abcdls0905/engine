#include "../math/fm_matrix.h"
#include <math.h>
#include <string.h>
#include "../math/fm_fixedpoint.h"		// Only needed for trig function float lookups
#include "../math/fm_matrix.h"


/****************************************************************************
** Constants
****************************************************************************/
static const FmMATRIXf	c_mIdentity = {
	{
		1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
	}
};

/****************************************************************************
** Functions
****************************************************************************/

/*!***************************************************************************
@Function			FmMatrixIdentityF
@Output			mOut	Set to identity
@Description		Reset matrix to identity matrix.
*****************************************************************************/
void FmMatrixIdentityF(FmMATRIXf *pOut)
{
	Assert(pOut != NULL);
	(*pOut).f[ 0]=1.0f;	(*pOut).f[ 4]=0.0f;	(*pOut).f[ 8]=0.0f;	(*pOut).f[12]=0.0f;
	(*pOut).f[ 1]=0.0f;	(*pOut).f[ 5]=1.0f;	(*pOut).f[ 9]=0.0f;	(*pOut).f[13]=0.0f;
	(*pOut).f[ 2]=0.0f;	(*pOut).f[ 6]=0.0f;	(*pOut).f[10]=1.0f;	(*pOut).f[14]=0.0f;
	(*pOut).f[ 3]=0.0f;	(*pOut).f[ 7]=0.0f;	(*pOut).f[11]=0.0f;	(*pOut).f[15]=1.0f;
}


/*!***************************************************************************
@Function			FmMatrixMultiplyF
@Output			mOut	Result of mA x mB
@Input				mA		First operand
@Input				mB		Second operand
@Description		Multiply mA by mB and assign the result to mOut
(mOut = p1 * p2). A copy of the result matrix is done in
the function because mOut can be a parameter mA or mB.
*****************************************************************************/
void FmMatrixMultiplyF(
					   FmMATRIXf			*pOut,
					   const FmMATRIXf	*pA,
					   const FmMATRIXf	*pB)
{
	Assert((pOut != NULL) && (pA != NULL ) && ( pB != NULL) );
	FmMATRIXf mRet;
	FmMATRIXf mA = *pA;
    FmMATRIXf mB = *pB;

	/* Perform calculation on a dummy matrix (mRet) */
	mRet.f[ 0] = mA.f[ 0]*mB.f[ 0] + mA.f[ 1]*mB.f[ 4] + mA.f[ 2]*mB.f[ 8] + mA.f[ 3]*mB.f[12];
	mRet.f[ 1] = mA.f[ 0]*mB.f[ 1] + mA.f[ 1]*mB.f[ 5] + mA.f[ 2]*mB.f[ 9] + mA.f[ 3]*mB.f[13];
	mRet.f[ 2] = mA.f[ 0]*mB.f[ 2] + mA.f[ 1]*mB.f[ 6] + mA.f[ 2]*mB.f[10] + mA.f[ 3]*mB.f[14];
	mRet.f[ 3] = mA.f[ 0]*mB.f[ 3] + mA.f[ 1]*mB.f[ 7] + mA.f[ 2]*mB.f[11] + mA.f[ 3]*mB.f[15];

	mRet.f[ 4] = mA.f[ 4]*mB.f[ 0] + mA.f[ 5]*mB.f[ 4] + mA.f[ 6]*mB.f[ 8] + mA.f[ 7]*mB.f[12];
	mRet.f[ 5] = mA.f[ 4]*mB.f[ 1] + mA.f[ 5]*mB.f[ 5] + mA.f[ 6]*mB.f[ 9] + mA.f[ 7]*mB.f[13];
	mRet.f[ 6] = mA.f[ 4]*mB.f[ 2] + mA.f[ 5]*mB.f[ 6] + mA.f[ 6]*mB.f[10] + mA.f[ 7]*mB.f[14];
	mRet.f[ 7] = mA.f[ 4]*mB.f[ 3] + mA.f[ 5]*mB.f[ 7] + mA.f[ 6]*mB.f[11] + mA.f[ 7]*mB.f[15];

	mRet.f[ 8] = mA.f[ 8]*mB.f[ 0] + mA.f[ 9]*mB.f[ 4] + mA.f[10]*mB.f[ 8] + mA.f[11]*mB.f[12];
	mRet.f[ 9] = mA.f[ 8]*mB.f[ 1] + mA.f[ 9]*mB.f[ 5] + mA.f[10]*mB.f[ 9] + mA.f[11]*mB.f[13];
	mRet.f[10] = mA.f[ 8]*mB.f[ 2] + mA.f[ 9]*mB.f[ 6] + mA.f[10]*mB.f[10] + mA.f[11]*mB.f[14];
	mRet.f[11] = mA.f[ 8]*mB.f[ 3] + mA.f[ 9]*mB.f[ 7] + mA.f[10]*mB.f[11] + mA.f[11]*mB.f[15];

	mRet.f[12] = mA.f[12]*mB.f[ 0] + mA.f[13]*mB.f[ 4] + mA.f[14]*mB.f[ 8] + mA.f[15]*mB.f[12];
	mRet.f[13] = mA.f[12]*mB.f[ 1] + mA.f[13]*mB.f[ 5] + mA.f[14]*mB.f[ 9] + mA.f[15]*mB.f[13];
	mRet.f[14] = mA.f[12]*mB.f[ 2] + mA.f[13]*mB.f[ 6] + mA.f[14]*mB.f[10] + mA.f[15]*mB.f[14];
	mRet.f[15] = mA.f[12]*mB.f[ 3] + mA.f[13]*mB.f[ 7] + mA.f[14]*mB.f[11] + mA.f[15]*mB.f[15];

	/* Copy result to mOut */
	*pOut = mRet;
}


/*!***************************************************************************
@Function Name		FmMatrixTranslationF
@Output			mOut	Translation matrix
@Input				fX		X component of the translation
@Input				fY		Y component of the translation
@Input				fZ		Z component of the translation
@Description		Build a transaltion matrix mOut using fX, fY and fZ.
*****************************************************************************/
void FmMatrixTranslationF(
						  FmMATRIXf	*pOut,
						  const float fX,
						  const float fY,
						  const float fZ)
{
	Assert( pOut != NULL );
	(*pOut).f[ 0]=1.0f;	(*pOut).f[ 4]=0.0f;	(*pOut).f[ 8]=0.0f;	(*pOut).f[12]=fX;
	(*pOut).f[ 1]=0.0f;	(*pOut).f[ 5]=1.0f;	(*pOut).f[ 9]=0.0f;	(*pOut).f[13]=fY;
	(*pOut).f[ 2]=0.0f;	(*pOut).f[ 6]=0.0f;	(*pOut).f[10]=1.0f;	(*pOut).f[14]=fZ;
	(*pOut).f[ 3]=0.0f;	(*pOut).f[ 7]=0.0f;	(*pOut).f[11]=0.0f;	(*pOut).f[15]=1.0f;
}

/*!***************************************************************************
@Function Name		FmMatrixScalingF
@Output			mOut	Scale matrix
@Input				fX		X component of the scaling
@Input				fY		Y component of the scaling
@Input				fZ		Z component of the scaling
@Description		Build a scale matrix mOut using fX, fY and fZ.
*****************************************************************************/
void FmMatrixScalingF(
					  FmMATRIXf	*pOut,
					  const float fX,
					  const float fY,
					  const float fZ)
{
	Assert( pOut != NULL );
	(*pOut).f[ 0]=fX;	(*pOut).f[ 4]=0.0f;	(*pOut).f[ 8]=0.0f;	(*pOut).f[12]=0.0f;
	(*pOut).f[ 1]=0.0f;	(*pOut).f[ 5]=fY;	(*pOut).f[ 9]=0.0f;	(*pOut).f[13]=0.0f;
	(*pOut).f[ 2]=0.0f;	(*pOut).f[ 6]=0.0f;	(*pOut).f[10]=fZ;	(*pOut).f[14]=0.0f;
	(*pOut).f[ 3]=0.0f;	(*pOut).f[ 7]=0.0f;	(*pOut).f[11]=0.0f;	(*pOut).f[15]=1.0f;
}

/*!***************************************************************************
@Function Name		FmMatrixRotationXF
@Output			mOut	Rotation matrix
@Input				fAngle	Angle of the rotation
@Description		Create an X rotation matrix mOut.
*****************************************************************************/
void FmMatrixRotationXF(
						FmMATRIXf	*pOut,
						const float fAngle)
{
	Assert( pOut != NULL );
	float		fCosine, fSine;

	fCosine	= (float)FMFCOS(fAngle);
	fSine	= (float)FMFSIN(fAngle);

	/* Create the trigonometric matrix corresponding to X Rotation */
	(*pOut).f[ 0]=1.0f;	(*pOut).f[ 4]=0.0f;	(*pOut).f[ 8]=0.0f;	(*pOut).f[12]=0.0f;
	(*pOut).f[ 1]=0.0f;	(*pOut).f[ 5]=fCosine;	(*pOut).f[ 9]=fSine;	(*pOut).f[13]=0.0f;
	(*pOut).f[ 2]=0.0f;	(*pOut).f[ 6]=-fSine;	(*pOut).f[10]=fCosine;	(*pOut).f[14]=0.0f;
	(*pOut).f[ 3]=0.0f;	(*pOut).f[ 7]=0.0f;	(*pOut).f[11]=0.0f;	(*pOut).f[15]=1.0f;
}

/*!***************************************************************************
@Function Name		FmMatrixRotationYF
@Output			mOut	Rotation matrix
@Input				fAngle	Angle of the rotation
@Description		Create an Y rotation matrix mOut.
*****************************************************************************/
void FmMatrixRotationYF(
						FmMATRIXf	*pOut,
						const float fAngle)
{
	Assert( pOut != NULL );
	float		fCosine, fSine;

	fCosine	= (float)FMFCOS(fAngle);
	fSine	= (float)FMFSIN(fAngle);

	/* Create the trigonometric matrix corresponding to Y Rotation */
	(*pOut).f[ 0]=fCosine;	(*pOut).f[ 4]=0.0f;	(*pOut).f[ 8]=-fSine;	(*pOut).f[12]=0.0f;
	(*pOut).f[ 1]=0.0f;		(*pOut).f[ 5]=1.0f;	(*pOut).f[ 9]=0.0f;		(*pOut).f[13]=0.0f;
	(*pOut).f[ 2]=fSine;	(*pOut).f[ 6]=0.0f;	(*pOut).f[10]=fCosine;	(*pOut).f[14]=0.0f;
	(*pOut).f[ 3]=0.0f;		(*pOut).f[ 7]=0.0f;	(*pOut).f[11]=0.0f;		(*pOut).f[15]=1.0f;
}

/*!***************************************************************************
@Function Name		FmMatrixRotationZF
@Output			mOut	Rotation matrix
@Input				fAngle	Angle of the rotation
@Description		Create an Z rotation matrix mOut.
*****************************************************************************/
void FmMatrixRotationZF(
						FmMATRIXf	*pOut,
						const float fAngle)
{
	Assert( pOut != NULL );
	float		fCosine, fSine;

	fCosine =	(float)FMFCOS(fAngle);
	fSine =		(float)FMFSIN(fAngle);

	/* Create the trigonometric matrix corresponding to Z Rotation */
	(*pOut).f[ 0]=fCosine;	(*pOut).f[ 4]=fSine;	(*pOut).f[ 8]=0.0f;	(*pOut).f[12]=0.0f;
	(*pOut).f[ 1]=-fSine;	(*pOut).f[ 5]=fCosine;	(*pOut).f[ 9]=0.0f;	(*pOut).f[13]=0.0f;
	(*pOut).f[ 2]=0.0f;		(*pOut).f[ 6]=0.0f;	    (*pOut).f[10]=1.0f;	(*pOut).f[14]=0.0f;
	(*pOut).f[ 3]=0.0f;		(*pOut).f[ 7]=0.0f;	    (*pOut).f[11]=0.0f;	(*pOut).f[15]=1.0f;
}

/*!***************************************************************************
@Function Name		FmMatrixTransposeF
@Output			mOut	Transposed matrix
@Input				mIn		Original matrix
@Description		Compute the transpose matrix of mIn.
*****************************************************************************/
void FmMatrixTransposeF(
						FmMATRIXf			*pOut,
						const FmMATRIXf	*pIn)
{
	Assert( (pOut != NULL ) && ( pIn != NULL ) );
    FmMATRIXf	mIn = *pIn;
	FmMATRIXf	mTmp;

	mTmp.f[ 0]=mIn.f[ 0];	mTmp.f[ 4]=mIn.f[ 1];	mTmp.f[ 8]=mIn.f[ 2];	mTmp.f[12]=mIn.f[ 3];
	mTmp.f[ 1]=mIn.f[ 4];	mTmp.f[ 5]=mIn.f[ 5];	mTmp.f[ 9]=mIn.f[ 6];	mTmp.f[13]=mIn.f[ 7];
	mTmp.f[ 2]=mIn.f[ 8];	mTmp.f[ 6]=mIn.f[ 9];	mTmp.f[10]=mIn.f[10];	mTmp.f[14]=mIn.f[11];
	mTmp.f[ 3]=mIn.f[12];	mTmp.f[ 7]=mIn.f[13];	mTmp.f[11]=mIn.f[14];	mTmp.f[15]=mIn.f[15];

	*pOut = mTmp;
}

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
void FmMatrixInverseF(
					  FmMATRIXf			*pOut,
					  const FmMATRIXf	*pIn)
{
	Assert( (pOut != NULL) && (pIn != NULL) );
    FmMATRIXf mIn = *pIn;
	FmMATRIXf	mDummyMatrix;
	double		det_1;
	double		pos, neg, temp;

	/* Calculate the determinant of submatrix A and determine if the
	the matrix is singular as limited by the double precision
	floating-point data representation. */
	pos = neg = 0.0;
	temp =  mIn.f[ 0] * mIn.f[ 5] * mIn.f[10];
	if (temp >= 0.0) pos += temp; else neg += temp;
	temp =  mIn.f[ 4] * mIn.f[ 9] * mIn.f[ 2];
	if (temp >= 0.0) pos += temp; else neg += temp;
	temp =  mIn.f[ 8] * mIn.f[ 1] * mIn.f[ 6];
	if (temp >= 0.0) pos += temp; else neg += temp;
	temp = -mIn.f[ 8] * mIn.f[ 5] * mIn.f[ 2];
	if (temp >= 0.0) pos += temp; else neg += temp;
	temp = -mIn.f[ 4] * mIn.f[ 1] * mIn.f[10];
	if (temp >= 0.0) pos += temp; else neg += temp;
	temp = -mIn.f[ 0] * mIn.f[ 9] * mIn.f[ 6];
	if (temp >= 0.0) pos += temp; else neg += temp;
	det_1 = pos + neg;

	/* Is the submatrix A singular? */
	if ((det_1 == 0.0) || (FMABS(det_1 / (pos - neg)) < 1.0e-15))
	{
		/* Matrix M has no inverse */
		//_RPT0(_CRT_WARN, "Matrix has no inverse : singular matrix\n");
		return;
	}
	else
	{
		/* Calculate inverse(A) = adj(A) / det(A) */
		det_1 = 1.0 / det_1;
		mDummyMatrix.f[ 0] =   ( mIn.f[ 5] * mIn.f[10] - mIn.f[ 9] * mIn.f[ 6] ) * (float)det_1;
		mDummyMatrix.f[ 1] = - ( mIn.f[ 1] * mIn.f[10] - mIn.f[ 9] * mIn.f[ 2] ) * (float)det_1;
		mDummyMatrix.f[ 2] =   ( mIn.f[ 1] * mIn.f[ 6] - mIn.f[ 5] * mIn.f[ 2] ) * (float)det_1;
		mDummyMatrix.f[ 4] = - ( mIn.f[ 4] * mIn.f[10] - mIn.f[ 8] * mIn.f[ 6] ) * (float)det_1;
		mDummyMatrix.f[ 5] =   ( mIn.f[ 0] * mIn.f[10] - mIn.f[ 8] * mIn.f[ 2] ) * (float)det_1;
		mDummyMatrix.f[ 6] = - ( mIn.f[ 0] * mIn.f[ 6] - mIn.f[ 4] * mIn.f[ 2] ) * (float)det_1;
		mDummyMatrix.f[ 8] =   ( mIn.f[ 4] * mIn.f[ 9] - mIn.f[ 8] * mIn.f[ 5] ) * (float)det_1;
		mDummyMatrix.f[ 9] = - ( mIn.f[ 0] * mIn.f[ 9] - mIn.f[ 8] * mIn.f[ 1] ) * (float)det_1;
		mDummyMatrix.f[10] =   ( mIn.f[ 0] * mIn.f[ 5] - mIn.f[ 4] * mIn.f[ 1] ) * (float)det_1;

		/* Calculate -C * inverse(A) */
		mDummyMatrix.f[12] = - ( mIn.f[12] * mDummyMatrix.f[ 0] + mIn.f[13] * mDummyMatrix.f[ 4] + mIn.f[14] * mDummyMatrix.f[ 8] );
		mDummyMatrix.f[13] = - ( mIn.f[12] * mDummyMatrix.f[ 1] + mIn.f[13] * mDummyMatrix.f[ 5] + mIn.f[14] * mDummyMatrix.f[ 9] );
		mDummyMatrix.f[14] = - ( mIn.f[12] * mDummyMatrix.f[ 2] + mIn.f[13] * mDummyMatrix.f[ 6] + mIn.f[14] * mDummyMatrix.f[10] );

		/* Fill in last row */
		mDummyMatrix.f[ 3] = 0.0f;
		mDummyMatrix.f[ 7] = 0.0f;
		mDummyMatrix.f[11] = 0.0f;
		mDummyMatrix.f[15] = 1.0f;
	}

	/* Copy contents of dummy matrix in pfMatrix */
	*pOut = mDummyMatrix;
}

/*!***************************************************************************
@Function			FmMatrixInverseExF
@Output			mOut	Inversed matrix
@Input				mIn		Original matrix
@Description		Compute the inverse matrix of mIn.
Uses a linear equation solver and the knowledge that M.M^-1=I.
Use this fn to calculate the inverse of matrices that
FmMatrixInverse() cannot.
*****************************************************************************/
void FmMatrixInverseExF(
						FmMATRIXf			*pOut,
						const FmMATRIXf	*pIn)
{
	Assert( (pOut != NULL) && (pIn != NULL) );
	FmMATRIXf mIn = *pIn;
	FmMATRIXf		mTmp;
	float 			*ppfRows[4];
	float 			pfRes[4];
	float 			pfIn[20];
	int				i, j;

	for(i = 0; i < 4; ++i)
		ppfRows[i] = &pfIn[i * 5];

	/* Solve 4 sets of 4 linear equations */
	for(i = 0; i < 4; ++i)
	{
		for(j = 0; j < 4; ++j)
		{
			ppfRows[j][0] = c_mIdentity.f[i + 4 * j];
			memcpy(&ppfRows[j][1], &mIn.f[j * 4], 4 * sizeof(float));
		}

		FmMatrixLinearEqSolveF(pfRes, (float**)ppfRows, 4);

		for(j = 0; j < 4; ++j)
		{
			mTmp.f[i + 4 * j] = pfRes[j];
		}
	}

	*pOut = mTmp;
}

/*!***************************************************************************
@Function			FmMatrixLookAtLHF
@Output			mOut	Look-at view matrix
@Input				vEye	Position of the camera
@Input				vAt		Point the camera is looking at
@Input				vUp		Up direction for the camera
@Description		Create a look-at view matrix.
*****************************************************************************/
void FmMatrixLookAtLHF(
					   FmMATRIXf			*pOut,
					   const FmVECTOR3f	*pvEye,
					   const FmVECTOR3f	*pvAt,
					   const FmVECTOR3f	*pvUp)
{
	Assert( (pOut != NULL) && ( pvEye != NULL) && (pvUp != NULL ) );
    FmVECTOR3f vEye = *pvEye;
	FmVECTOR3f vAt = *pvAt;
    FmVECTOR3f vUp = *pvUp;

	FmVECTOR3f f, vUpActual, s, u;
	FmMATRIXf	t;

	f.x = vEye.x - vAt.x;
	f.y = vEye.y - vAt.y;
	f.z = vEye.z - vAt.z;

	FmMatrixVec3NormalizeF(&f, &f);
	FmMatrixVec3NormalizeF(&vUpActual, &vUp);
	FmMatrixVec3CrossProductF(&s, &f, &vUpActual);
	FmMatrixVec3CrossProductF(&u, &s, &f);

	(*pOut).f[ 0] = s.x;
	(*pOut).f[ 1] = u.x;
	(*pOut).f[ 2] = -f.x;
	(*pOut).f[ 3] = 0;

	(*pOut).f[ 4] = s.y;
	(*pOut).f[ 5] = u.y;
	(*pOut).f[ 6] = -f.y;
	(*pOut).f[ 7] = 0;

	(*pOut).f[ 8] = s.z;
	(*pOut).f[ 9] = u.z;
	(*pOut).f[10] = -f.z;
	(*pOut).f[11] = 0;

	(*pOut).f[12] = 0;
	(*pOut).f[13] = 0;
	(*pOut).f[14] = 0;
	(*pOut).f[15] = 1;

	FmMatrixTranslationF(&t, -vEye.x, -vEye.y, -vEye.z);
	FmMatrixMultiplyF(pOut, &t, pOut);
}

/*!***************************************************************************
@Function			FmMatrixLookAtRHF
@Output			mOut	Look-at view matrix
@Input				vEye	Position of the camera
@Input				vAt		Point the camera is looking at
@Input				vUp		Up direction for the camera
@Description		Create a look-at view matrix.
*****************************************************************************/
void FmMatrixLookAtRHF(
					   FmMATRIXf			*pOut,
					   const FmVECTOR3f	*pvEye,
					   const FmVECTOR3f	*pvAt,
					   const FmVECTOR3f	*pvUp)
{
	Assert( (pOut != NULL) && (pvEye != NULL) && (pvAt != NULL) && (pvUp != NULL) );
    FmVECTOR3f vEye = *pvEye;
	FmVECTOR3f vAt = *pvAt;
	FmVECTOR3f vUp = *pvUp;
	FmVECTOR3f f, vUpActual, s, u;
	FmMATRIXf	t;

	f.x = vAt.x - vEye.x;
	f.y = vAt.y - vEye.y;
	f.z = vAt.z - vEye.z;

	FmMatrixVec3NormalizeF(&f, &f);
	FmMatrixVec3NormalizeF(&vUpActual, &vUp);
	FmMatrixVec3CrossProductF(&s, &f, &vUpActual);
	FmMatrixVec3CrossProductF(&u, &s, &f);

	(*pOut).f[ 0] = s.x;
	(*pOut).f[ 1] = u.x;
	(*pOut).f[ 2] = -f.x;
	(*pOut).f[ 3] = 0;

	(*pOut).f[ 4] = s.y;
	(*pOut).f[ 5] = u.y;
	(*pOut).f[ 6] = -f.y;
	(*pOut).f[ 7] = 0;

	(*pOut).f[ 8] = s.z;
	(*pOut).f[ 9] = u.z;
	(*pOut).f[10] = -f.z;
	(*pOut).f[11] = 0;

	(*pOut).f[12] = 0;
	(*pOut).f[13] = 0;
	(*pOut).f[14] = 0;
	(*pOut).f[15] = 1;

	FmMatrixTranslationF(&t, -vEye.x, -vEye.y, -vEye.z);
	FmMatrixMultiplyF(pOut, &t, pOut);
}

/*!***************************************************************************
@Function		FmMatrixPerspectiveFovLHF
@Output		mOut		Perspective matrix
@Input			fFOVy		Field of view
@Input			fAspect		Aspect ratio
@Input			fNear		Near clipping distance
@Input			fFar		Far clipping distance
@Input			bRotate		Should we rotate it ? (for upright screens)
@Description	Create a perspective matrix.
*****************************************************************************/
void FmMatrixPerspectiveFovLHF(
							   FmMATRIXf	*pOut,
							   const float	fFOVy,
							   const float	fAspect,
							   const float	fNear,
							   const float	fFar,
							   const int  bRotate)
{
	Assert( pOut != NULL );
	float f, n, fRealAspect;

	if (bRotate)
		fRealAspect = 1.0f / fAspect;
	else
		fRealAspect = fAspect;

	// cotangent(a) == 1.0f / tan(a);
	f = 1.0f / (float)FMFTAN(fFOVy * 0.5f);
	n = 1.0f / (fFar - fNear);

	(*pOut).f[ 0] = f / fRealAspect;
	(*pOut).f[ 1] = 0;
	(*pOut).f[ 2] = 0;
	(*pOut).f[ 3] = 0;

	(*pOut).f[ 4] = 0;
	(*pOut).f[ 5] = f;
	(*pOut).f[ 6] = 0;
	(*pOut).f[ 7] = 0;

	(*pOut).f[ 8] = 0;
	(*pOut).f[ 9] = 0;
	(*pOut).f[10] = fFar * n;
	(*pOut).f[11] = 1;

	(*pOut).f[12] = 0;
	(*pOut).f[13] = 0;
	(*pOut).f[14] = -fFar * fNear * n;
	(*pOut).f[15] = 0;

	if (bRotate)
	{
		FmMATRIXf mRotation, mTemp = *pOut;
		FmMatrixRotationZF(&mRotation, 90.0f*FM_PIf/180.0f);
		FmMatrixMultiplyF(pOut, &mTemp, &mRotation);
	}
}

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
void FmMatrixPerspectiveFovRHF(
							   FmMATRIXf	*pOut,
							   const float	fFOVy,
							   const float	fAspect,
							   const float	fNear,
							   const float	fFar,
							   const int  bRotate)
{
	Assert( pOut != NULL );
	float f, n, fRealAspect;

	if (bRotate)
		fRealAspect = 1.0f / fAspect;
	else
		fRealAspect = fAspect;

	// cotangent(a) == 1.0f / tan(a);
	f = 1.0f / (float)FMFTAN(fFOVy * 0.5f);
	n = 1.0f / (fNear - fFar);

	(*pOut).f[ 0] = f / fRealAspect;
	(*pOut).f[ 1] = 0;
	(*pOut).f[ 2] = 0;
	(*pOut).f[ 3] = 0;

	(*pOut).f[ 4] = 0;
	(*pOut).f[ 5] = f;
	(*pOut).f[ 6] = 0;
	(*pOut).f[ 7] = 0;

	(*pOut).f[ 8] = 0;
	(*pOut).f[ 9] = 0;
	(*pOut).f[10] = (fFar + fNear) * n;
	(*pOut).f[11] = -1;

	(*pOut).f[12] = 0;
	(*pOut).f[13] = 0;
	(*pOut).f[14] = (2 * fFar * fNear) * n;
	(*pOut).f[15] = 0;

	if (bRotate)
	{
		FmMATRIXf mRotation, mTemp = *pOut;
		FmMatrixRotationZF(&mRotation, -90.0f*FM_PIf/180.0f);
		FmMatrixMultiplyF(pOut, &mTemp, &mRotation);
	}
}

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
void FmMatrixOrthoLHF(
					  FmMATRIXf	*pOut,
					  const float w,
					  const float h,
					  const float zn,
					  const float zf,
					  const int  bRotate)
{
	Assert( pOut != NULL );
	(*pOut).f[ 0] = 2 / w;
	(*pOut).f[ 1] = 0;
	(*pOut).f[ 2] = 0;
	(*pOut).f[ 3] = 0;

	(*pOut).f[ 4] = 0;
	(*pOut).f[ 5] = 2 / h;
	(*pOut).f[ 6] = 0;
	(*pOut).f[ 7] = 0;

	(*pOut).f[ 8] = 0;
	(*pOut).f[ 9] = 0;
	(*pOut).f[10] = 1 / (zf - zn);
	(*pOut).f[11] = zn / (zn - zf);

	(*pOut).f[12] = 0;
	(*pOut).f[13] = 0;
	(*pOut).f[14] = 0;
	(*pOut).f[15] = 1;

	if (bRotate)
	{
		FmMATRIXf mRotation, mTemp = *pOut;
		FmMatrixRotationZF(&mRotation, -90.0f*FM_PIf/180.0f);
		FmMatrixMultiplyF(pOut, &mRotation, &mTemp);
	}
}

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
void FmMatrixOrthoRHF(
					  FmMATRIXf	*pOut,
					  const float w,
					  const float h,
					  const float zn,
					  const float zf,
					  const int  bRotate)
{
	Assert( pOut != NULL );
	(*pOut).f[ 0] = 2 / w;
	(*pOut).f[ 1] = 0;
	(*pOut).f[ 2] = 0;
	(*pOut).f[ 3] = 0;

	(*pOut).f[ 4] = 0;
	(*pOut).f[ 5] = 2 / h;
	(*pOut).f[ 6] = 0;
	(*pOut).f[ 7] = 0;

	(*pOut).f[ 8] = 0;
	(*pOut).f[ 9] = 0;
	(*pOut).f[10] = 1 / (zn - zf);
	(*pOut).f[11] = zn / (zn - zf);

	(*pOut).f[12] = 0;
	(*pOut).f[13] = 0;
	(*pOut).f[14] = 0;
	(*pOut).f[15] = 1;

	if (bRotate)
	{
		FmMATRIXf mRotation, mTemp = *pOut;
		FmMatrixRotationZF(&mRotation, -90.0f*FM_PIf/180.0f);
		FmMatrixMultiplyF(pOut, &mRotation, &mTemp);
	}
}

/*!***************************************************************************
@Function			FmMatrixVec3LerpF
@Output			vOut	Result of the interpolation
@Input				v1		First vector to interpolate from
@Input				v2		Second vector to interpolate form
@Input				s		Coefficient of interpolation
@Description		This function performs the linear interpolation based on
the following formula: V1 + s(V2-V1).
*****************************************************************************/
void FmMatrixVec3LerpF(
					   FmVECTOR3f		*pvOut,
					   const FmVECTOR3f	*pv1,
					   const FmVECTOR3f	*pv2,
					   const float	s)
{
	Assert( (pvOut != NULL ) && (pv1 != NULL) && (pv2 != NULL) );
	(*pvOut).x = (*pv1).x + s * ((*pv2).x - (*pv1).x);
	(*pvOut).y = (*pv1).y + s * ((*pv2).y - (*pv1).y);
	(*pvOut).z = (*pv1).z + s * ((*pv2).z - (*pv1).z);
}

/*!***************************************************************************
@Function			FmMatrixVec3DotProductF
@Input				v1		First vector
@Input				v2		Second vector
@Return			Dot product of the two vectors.
@Description		This function performs the dot product of the two
supplied vectors.
*****************************************************************************/
float FmMatrixVec3DotProductF(
							  const FmVECTOR3f	*pv1,
							  const FmVECTOR3f	*pv2)
{
	Assert( (pv1 != NULL) && (pv2 != NULL) );
	return ((*pv1).x*(*pv2).x + (*pv1).y*(*pv2).y + (*pv1).z*(*pv2).z);
}

/*!***************************************************************************
@Function			FmMatrixVec3CrossProductF
@Output			vOut	Cross product of the two vectors
@Input				v1		First vector
@Input				v2		Second vector
@Description		This function performs the cross product of the two
supplied vectors.
*****************************************************************************/
void FmMatrixVec3CrossProductF(
							   FmVECTOR3f		*pvOut,
							   const FmVECTOR3f	*pv1,
							   const FmVECTOR3f	*pv2)
{
	Assert( (pvOut != NULL) && (pv1 != NULL) && (pv2 != NULL) );
	FmVECTOR3f result;
    FmVECTOR3f v1 = *pv1;
	FmVECTOR3f v2 = *pv2;

	/* Perform calculation on a dummy VECTOR (result) */
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;

	/* Copy result in pOut */
	*pvOut = result;
}

/*!***************************************************************************
@Function			FmMatrixVec3NormalizeF
@Output			vOut	Normalized vector
@Input				vIn		Vector to normalize
@Description		Normalizes the supplied vector.
*****************************************************************************/
void FmMatrixVec3NormalizeF(
							FmVECTOR3f		*pvOut,
							const FmVECTOR3f	*pvIn)
{
    Assert( (pvOut != NULL) && (pvIn != NULL ) );
	FmVECTOR3f vIn = *pvIn;
	float	f;
	double temp;

	temp = (double)(vIn.x * vIn.x + vIn.y * vIn.y + vIn.z * vIn.z);
	temp = 1.0 / sqrt(temp);
	f = (float)temp;

	(*pvOut).x = vIn.x * f;
	(*pvOut).y = vIn.y * f;
	(*pvOut).z = vIn.z * f;
}

/*!***************************************************************************
@Function			FmMatrixVec3LengthF
@Input				vIn		Vector to get the length of
@Return			The length of the vector
@Description		Gets the length of the supplied vector.
*****************************************************************************/
float FmMatrixVec3LengthF(
						  const FmVECTOR3f	*pvIn)
{
	Assert( pvIn != NULL );
	double temp;

	temp = (double)((*pvIn).x * (*pvIn).x + (*pvIn).y * (*pvIn).y + (*pvIn).z * (*pvIn).z);
	return (float) sqrt(temp);
}

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
void FmMatrixLinearEqSolveF(
							float		* const pRes,
							float		** const pSrc,	// 2D array of floats. 4 Eq linear problem is 5x4 matrix, constants in first column.
							const int	nCnt)
{
	int		i, j, k;
	float	f;

#if 0
	/*
	Show the matrix in debug output
	*/
	_RPT1(_CRT_WARN, "LinearEqSolve(%d)\n", nCnt);
	for(i = 0; i < nCnt; ++i)
	{
		_RPT1(_CRT_WARN, "%.8f |", pSrc[i][0]);
		for(j = 1; j <= nCnt; ++j)
			_RPT1(_CRT_WARN, " %.8f", pSrc[i][j]);
		//_RPT0(_CRT_WARN, "\n");
	}
#endif

	if(nCnt == 1)
	{
		Assert(pSrc[0][1] != 0);
		pRes[0] = pSrc[0][0] / pSrc[0][1];
		return;
	}

	// Loop backwards in an attempt avoid the need to swap rows
	i = nCnt;
	while(i)
	{
		--i;

		if(pSrc[i][nCnt] != 0)
		{
			// Row i can be used to zero the other rows; let's move it to the bottom
			if(i != (nCnt-1))
			{
				for(j = 0; j <= nCnt; ++j)
				{
					// Swap the two values
					f = pSrc[nCnt-1][j];
					pSrc[nCnt-1][j] = pSrc[i][j];
					pSrc[i][j] = f;
				}
			}

			// Now zero the last columns of the top rows
			for(j = 0; j < (nCnt-1); ++j)
			{
				Assert(pSrc[nCnt-1][nCnt] != 0);
				f = pSrc[j][nCnt] / pSrc[nCnt-1][nCnt];

				// No need to actually calculate a zero for the final column
				for(k = 0; k < nCnt; ++k)
				{
					pSrc[j][k] -= f * pSrc[nCnt-1][k];
				}
			}

			break;
		}
	}

	// Solve the top-left sub matrix
	FmMatrixLinearEqSolveF(pRes, pSrc, nCnt - 1);

	// Now calc the solution for the bottom row
	f = pSrc[nCnt-1][0];
	for(k = 1; k < nCnt; ++k)
	{
		f -= pSrc[nCnt-1][k] * pRes[k-1];
	}
	Assert(pSrc[nCnt-1][nCnt] != 0);
	f /= pSrc[nCnt-1][nCnt];
	pRes[nCnt-1] = f;

#if 0
	{
		float fCnt;

		/*
		Verify that the result is correct
		*/
		fCnt = 0;
		for(i = 1; i <= nCnt; ++i)
			fCnt += pSrc[nCnt-1][i] * pRes[i-1];

		Assert(abs(fCnt - pSrc[nCnt-1][0]) < 1e-3);
	}
#endif
}

