#include <math.h>
#include <string.h>

#include "../math/fm_fixedpoint.h"
#include "../math/fm_matrix.h"



/****************************************************************************
** Constants
****************************************************************************/
static const FmMATRIXx	c_mIdentity = {
	{
		FMF2X(1.0f), FMF2X(0.0f), FMF2X(0.0f), FMF2X(0.0f),
			FMF2X(0.0f), FMF2X(1.0f), FMF2X(0.0f), FMF2X(0.0f),
			FMF2X(0.0f), FMF2X(0.0f), FMF2X(1.0f), FMF2X(0.0f),
			FMF2X(0.0f), FMF2X(0.0f), FMF2X(0.0f), FMF2X(1.0f)
	}
};


/****************************************************************************
** Functions
****************************************************************************/

/*!***************************************************************************
@Function			FmMatrixIdentityX
@Output			mOut	Set to identity
@Description		Reset matrix to identity matrix.
*****************************************************************************/
void FmMatrixIdentityX(FmMATRIXx *pOut)
{
	Assert( pOut!= NULL );
	(*pOut).f[ 0]=FMF2X(1.0f);	(*pOut).f[ 4]=FMF2X(0.0f);	(*pOut).f[ 8]=FMF2X(0.0f);	(*pOut).f[12]=FMF2X(0.0f);
	(*pOut).f[ 1]=FMF2X(0.0f);	(*pOut).f[ 5]=FMF2X(1.0f);	(*pOut).f[ 9]=FMF2X(0.0f);	(*pOut).f[13]=FMF2X(0.0f);
	(*pOut).f[ 2]=FMF2X(0.0f);	(*pOut).f[ 6]=FMF2X(0.0f);	(*pOut).f[10]=FMF2X(1.0f);	(*pOut).f[14]=FMF2X(0.0f);
	(*pOut).f[ 3]=FMF2X(0.0f);	(*pOut).f[ 7]=FMF2X(0.0f);	(*pOut).f[11]=FMF2X(0.0f);	(*pOut).f[15]=FMF2X(1.0f);
}

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
****************************************************************************/
void FmMatrixMultiplyX(
					   FmMATRIXx			*pOut,
					   const FmMATRIXx	*pA,
					   const FmMATRIXx	*pB)
{
	Assert( (pOut != NULL) && (pA != NULL) && (pB != NULL) );
	const FmMATRIXx*  mA = pA;
	const FmMATRIXx*  mB = pB;

	FmMATRIXx mRet;

	/* Perform calculation on a dummy matrix (mRet) */
	mRet.f[ 0] = FMXMUL((*mA).f[ 0], (*mB).f[ 0]) + FMXMUL((*mA).f[ 1], (*mB).f[ 4]) + FMXMUL((*mA).f[ 2], (*mB).f[ 8]) + FMXMUL((*mA).f[ 3], (*mB).f[12]);
	mRet.f[ 1] = FMXMUL((*mA).f[ 0], (*mB).f[ 1]) + FMXMUL((*mA).f[ 1], (*mB).f[ 5]) + FMXMUL((*mA).f[ 2], (*mB).f[ 9]) + FMXMUL((*mA).f[ 3], (*mB).f[13]);
	mRet.f[ 2] = FMXMUL((*mA).f[ 0], (*mB).f[ 2]) + FMXMUL((*mA).f[ 1], (*mB).f[ 6]) + FMXMUL((*mA).f[ 2], (*mB).f[10]) + FMXMUL((*mA).f[ 3], (*mB).f[14]);
	mRet.f[ 3] = FMXMUL((*mA).f[ 0], (*mB).f[ 3]) + FMXMUL((*mA).f[ 1], (*mB).f[ 7]) + FMXMUL((*mA).f[ 2], (*mB).f[11]) + FMXMUL((*mA).f[ 3], (*mB).f[15]);

	mRet.f[ 4] = FMXMUL((*mA).f[ 4], (*mB).f[ 0]) + FMXMUL((*mA).f[ 5], (*mB).f[ 4]) + FMXMUL((*mA).f[ 6], (*mB).f[ 8]) + FMXMUL((*mA).f[ 7], (*mB).f[12]);
	mRet.f[ 5] = FMXMUL((*mA).f[ 4], (*mB).f[ 1]) + FMXMUL((*mA).f[ 5], (*mB).f[ 5]) + FMXMUL((*mA).f[ 6], (*mB).f[ 9]) + FMXMUL((*mA).f[ 7], (*mB).f[13]);
	mRet.f[ 6] = FMXMUL((*mA).f[ 4], (*mB).f[ 2]) + FMXMUL((*mA).f[ 5], (*mB).f[ 6]) + FMXMUL((*mA).f[ 6], (*mB).f[10]) + FMXMUL((*mA).f[ 7], (*mB).f[14]);
	mRet.f[ 7] = FMXMUL((*mA).f[ 4], (*mB).f[ 3]) + FMXMUL((*mA).f[ 5], (*mB).f[ 7]) + FMXMUL((*mA).f[ 6], (*mB).f[11]) + FMXMUL((*mA).f[ 7], (*mB).f[15]);

	mRet.f[ 8] = FMXMUL((*mA).f[ 8], (*mB).f[ 0]) + FMXMUL((*mA).f[ 9], (*mB).f[ 4]) + FMXMUL((*mA).f[10], (*mB).f[ 8]) + FMXMUL((*mA).f[11], (*mB).f[12]);
	mRet.f[ 9] = FMXMUL((*mA).f[ 8], (*mB).f[ 1]) + FMXMUL((*mA).f[ 9], (*mB).f[ 5]) + FMXMUL((*mA).f[10], (*mB).f[ 9]) + FMXMUL((*mA).f[11], (*mB).f[13]);
	mRet.f[10] = FMXMUL((*mA).f[ 8], (*mB).f[ 2]) + FMXMUL((*mA).f[ 9], (*mB).f[ 6]) + FMXMUL((*mA).f[10], (*mB).f[10]) + FMXMUL((*mA).f[11], (*mB).f[14]);
	mRet.f[11] = FMXMUL((*mA).f[ 8], (*mB).f[ 3]) + FMXMUL((*mA).f[ 9], (*mB).f[ 7]) + FMXMUL((*mA).f[10], (*mB).f[11]) + FMXMUL((*mA).f[11], (*mB).f[15]);

	mRet.f[12] = FMXMUL((*mA).f[12], (*mB).f[ 0]) + FMXMUL((*mA).f[13], (*mB).f[ 4]) + FMXMUL((*mA).f[14], (*mB).f[ 8]) + FMXMUL((*mA).f[15], (*mB).f[12]);
	mRet.f[13] = FMXMUL((*mA).f[12], (*mB).f[ 1]) + FMXMUL((*mA).f[13], (*mB).f[ 5]) + FMXMUL((*mA).f[14], (*mB).f[ 9]) + FMXMUL((*mA).f[15], (*mB).f[13]);
	mRet.f[14] = FMXMUL((*mA).f[12], (*mB).f[ 2]) + FMXMUL((*mA).f[13], (*mB).f[ 6]) + FMXMUL((*mA).f[14], (*mB).f[10]) + FMXMUL((*mA).f[15], (*mB).f[14]);
	mRet.f[15] = FMXMUL((*mA).f[12], (*mB).f[ 3]) + FMXMUL((*mA).f[13], (*mB).f[ 7]) + FMXMUL((*mA).f[14], (*mB).f[11]) + FMXMUL((*mA).f[15], (*mB).f[15]);

	/* Copy result in pResultMatrix */
	*pOut = mRet;
}

/*!***************************************************************************
@Function Name		FmMatrixTranslationX
@Output			mOut	Translation matrix
@Input				fX		X component of the translation
@Input				fY		Y component of the translation
@Input				fZ		Z component of the translation
@Description		Build a transaltion matrix mOut using fX, fY and fZ.
*****************************************************************************/
void FmMatrixTranslationX(
						  FmMATRIXx	*pOut,
						  const int	fX,
						  const int	fY,
						  const int	fZ)
{
	(*pOut).f[ 0]=FMF2X(1.0f);	(*pOut).f[ 4]=FMF2X(0.0f);	(*pOut).f[ 8]=FMF2X(0.0f);	(*pOut).f[12]=fX;
	(*pOut).f[ 1]=FMF2X(0.0f);	(*pOut).f[ 5]=FMF2X(1.0f);	(*pOut).f[ 9]=FMF2X(0.0f);	(*pOut).f[13]=fY;
	(*pOut).f[ 2]=FMF2X(0.0f);	(*pOut).f[ 6]=FMF2X(0.0f);	(*pOut).f[10]=FMF2X(1.0f);	(*pOut).f[14]=fZ;
	(*pOut).f[ 3]=FMF2X(0.0f);	(*pOut).f[ 7]=FMF2X(0.0f);	(*pOut).f[11]=FMF2X(0.0f);	(*pOut).f[15]=FMF2X(1.0f);
}

/*!***************************************************************************
@Function Name		FmMatrixScalingX
@Output			mOut	Scale matrix
@Input				fX		X component of the scaling
@Input				fY		Y component of the scaling
@Input				fZ		Z component of the scaling
@Description		Build a scale matrix mOut using fX, fY and fZ.
*****************************************************************************/
void FmMatrixScalingX(
					  FmMATRIXx	*pOut,
					  const int	fX,
					  const int	fY,
					  const int	fZ)
{
	(*pOut).f[ 0]=fX;			(*pOut).f[ 4]=FMF2X(0.0f);	(*pOut).f[ 8]=FMF2X(0.0f);	(*pOut).f[12]=FMF2X(0.0f);
	(*pOut).f[ 1]=FMF2X(0.0f);	(*pOut).f[ 5]=fY;			(*pOut).f[ 9]=FMF2X(0.0f);	(*pOut).f[13]=FMF2X(0.0f);
	(*pOut).f[ 2]=FMF2X(0.0f);	(*pOut).f[ 6]=FMF2X(0.0f);	(*pOut).f[10]=fZ;			(*pOut).f[14]=FMF2X(0.0f);
	(*pOut).f[ 3]=FMF2X(0.0f);	(*pOut).f[ 7]=FMF2X(0.0f);	(*pOut).f[11]=FMF2X(0.0f);	(*pOut).f[15]=FMF2X(1.0f);
}

/*!***************************************************************************
@Function Name		FmMatrixRotationXX
@Output			mOut	Rotation matrix
@Input				fAngle	Angle of the rotation
@Description		Create an X rotation matrix mOut.
*****************************************************************************/
void FmMatrixRotationXX(
						FmMATRIXx	*pOut,
						const int	fAngle)
{
	int		fCosine, fSine;

	fCosine	= FMXCOS(fAngle);
	fSine	= FMXSIN(fAngle);

	/* Create the trigonometric matrix corresponding to X Rotation */
	(*pOut).f[ 0]=FMF2X(1.0f);	(*pOut).f[ 4]=FMF2X(0.0f);		(*pOut).f[ 8]=FMF2X(0.0f);		(*pOut).f[12]=FMF2X(0.0f);
	(*pOut).f[ 1]=FMF2X(0.0f);	(*pOut).f[ 5]=fCosine;			(*pOut).f[ 9]=fSine;			(*pOut).f[13]=FMF2X(0.0f);
	(*pOut).f[ 2]=FMF2X(0.0f);	(*pOut).f[ 6]=-fSine;			(*pOut).f[10]=fCosine;			(*pOut).f[14]=FMF2X(0.0f);
	(*pOut).f[ 3]=FMF2X(0.0f);	(*pOut).f[ 7]=FMF2X(0.0f);		(*pOut).f[11]=FMF2X(0.0f);		(*pOut).f[15]=FMF2X(1.0f);
}

/*!***************************************************************************
@Function Name		FmMatrixRotationYX
@Output			mOut	Rotation matrix
@Input				fAngle	Angle of the rotation
@Description		Create an Y rotation matrix mOut.
*****************************************************************************/
void FmMatrixRotationYX(
						FmMATRIXx	*pOut,
						const int	fAngle)
{
	int		fCosine, fSine;

	fCosine	= FMXCOS(fAngle);
	fSine	= FMXSIN(fAngle);

	/* Create the trigonometric matrix corresponding to Y Rotation */
	(*pOut).f[ 0]=fCosine;			(*pOut).f[ 4]=FMF2X(0.0f);	(*pOut).f[ 8]=-fSine;			(*pOut).f[12]=FMF2X(0.0f);
	(*pOut).f[ 1]=FMF2X(0.0f);		(*pOut).f[ 5]=FMF2X(1.0f);	(*pOut).f[ 9]=FMF2X(0.0f);		(*pOut).f[13]=FMF2X(0.0f);
	(*pOut).f[ 2]=fSine;			(*pOut).f[ 6]=FMF2X(0.0f);	(*pOut).f[10]=fCosine;			(*pOut).f[14]=FMF2X(0.0f);
	(*pOut).f[ 3]=FMF2X(0.0f);		(*pOut).f[ 7]=FMF2X(0.0f);	(*pOut).f[11]=FMF2X(0.0f);		(*pOut).f[15]=FMF2X(1.0f);
}

/*!***************************************************************************
@Function Name		FmMatrixRotationZX
@Output			mOut	Rotation matrix
@Input				fAngle	Angle of the rotation
@Description		Create an Z rotation matrix mOut.
*****************************************************************************/
void FmMatrixRotationZX(
						FmMATRIXx	*pOut,
						const int	fAngle)
{
	Assert( pOut != NULL );
	int		fCosine, fSine;

	fCosine = FMXCOS(fAngle);
	fSine   = FMXSIN(fAngle);

	/* Create the trigonometric matrix corresponding to Z Rotation */
	(*pOut).f[ 0]=fCosine;			(*pOut).f[ 4]=fSine;			(*pOut).f[ 8]=FMF2X(0.0f);	(*pOut).f[12]=FMF2X(0.0f);
	(*pOut).f[ 1]=-fSine;			(*pOut).f[ 5]=fCosine;			(*pOut).f[ 9]=FMF2X(0.0f);	(*pOut).f[13]=FMF2X(0.0f);
	(*pOut).f[ 2]=FMF2X(0.0f);		(*pOut).f[ 6]=FMF2X(0.0f);		(*pOut).f[10]=FMF2X(1.0f);	(*pOut).f[14]=FMF2X(0.0f);
	(*pOut).f[ 3]=FMF2X(0.0f);		(*pOut).f[ 7]=FMF2X(0.0f);		(*pOut).f[11]=FMF2X(0.0f);	(*pOut).f[15]=FMF2X(1.0f);
}

/*!***************************************************************************
@Function Name		FmMatrixTransposeX
@Output			mOut	Transposed matrix
@Input				mIn		Original matrix
@Description		Compute the transpose matrix of mIn.
*****************************************************************************/
void FmMatrixTransposeX(
						FmMATRIXx			*pOut,
						const FmMATRIXx	*pIn)
{
	Assert( (pOut!= NULL ) && (pIn != NULL ) );
    FmMATRIXx mIn = *pIn;
	FmMATRIXx	mTmp;

	mTmp.f[ 0]=mIn.f[ 0];	mTmp.f[ 4]=mIn.f[ 1];	mTmp.f[ 8]=mIn.f[ 2];	mTmp.f[12]=mIn.f[ 3];
	mTmp.f[ 1]=mIn.f[ 4];	mTmp.f[ 5]=mIn.f[ 5];	mTmp.f[ 9]=mIn.f[ 6];	mTmp.f[13]=mIn.f[ 7];
	mTmp.f[ 2]=mIn.f[ 8];	mTmp.f[ 6]=mIn.f[ 9];	mTmp.f[10]=mIn.f[10];	mTmp.f[14]=mIn.f[11];
	mTmp.f[ 3]=mIn.f[12];	mTmp.f[ 7]=mIn.f[13];	mTmp.f[11]=mIn.f[14];	mTmp.f[15]=mIn.f[15];

	*pOut = mTmp;
}

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
void FmMatrixInverseX(
					  FmMATRIXx			*pOut,
					  const FmMATRIXx	*pIn)
{
	Assert( (pOut != NULL) && (pIn != NULL) );
	FmMATRIXx mIn = *pIn;
	FmMATRIXx	mDummyMatrix;
	int			det_1;
	int			pos, neg, temp;

	/* Calculate the determinant of submatrix A and determine if the
	the matrix is singular as limited by the double precision
	floating-point data representation. */
	pos = neg = 0;
	temp =  FMXMUL(FMXMUL(mIn.f[ 0], mIn.f[ 5]), mIn.f[10]);
	if (temp >= 0) pos += temp; else neg += temp;
	temp =  FMXMUL(FMXMUL(mIn.f[ 4], mIn.f[ 9]), mIn.f[ 2]);
	if (temp >= 0) pos += temp; else neg += temp;
	temp =  FMXMUL(FMXMUL(mIn.f[ 8], mIn.f[ 1]), mIn.f[ 6]);
	if (temp >= 0) pos += temp; else neg += temp;
	temp =  FMXMUL(FMXMUL(-mIn.f[ 8], mIn.f[ 5]), mIn.f[ 2]);
	if (temp >= 0) pos += temp; else neg += temp;
	temp =  FMXMUL(FMXMUL(-mIn.f[ 4], mIn.f[ 1]), mIn.f[10]);
	if (temp >= 0) pos += temp; else neg += temp;
	temp =  FMXMUL(FMXMUL(-mIn.f[ 0], mIn.f[ 9]), mIn.f[ 6]);
	if (temp >= 0) pos += temp; else neg += temp;
	det_1 = pos + neg;

	/* Is the submatrix A singular? */
	if (det_1 == 0)
	{
		/* Matrix M has no inverse */
		//_RPT0(_CRT_WARN, "Matrix has no inverse : singular matrix\n");
		return;
	}
	else
	{
		/* Calculate inverse(A) = adj(A) / det(A) */
		//det_1 = 1.0 / det_1;
		det_1 = FMXDIV(FMF2X(1.0f), det_1);
		mDummyMatrix.f[ 0] =   FMXMUL(( FMXMUL(mIn.f[ 5], mIn.f[10]) - FMXMUL(mIn.f[ 9], mIn.f[ 6]) ), det_1);
		mDummyMatrix.f[ 1] = - FMXMUL(( FMXMUL(mIn.f[ 1], mIn.f[10]) - FMXMUL(mIn.f[ 9], mIn.f[ 2]) ), det_1);
		mDummyMatrix.f[ 2] =   FMXMUL(( FMXMUL(mIn.f[ 1], mIn.f[ 6]) - FMXMUL(mIn.f[ 5], mIn.f[ 2]) ), det_1);
		mDummyMatrix.f[ 4] = - FMXMUL(( FMXMUL(mIn.f[ 4], mIn.f[10]) - FMXMUL(mIn.f[ 8], mIn.f[ 6]) ), det_1);
		mDummyMatrix.f[ 5] =   FMXMUL(( FMXMUL(mIn.f[ 0], mIn.f[10]) - FMXMUL(mIn.f[ 8], mIn.f[ 2]) ), det_1);
		mDummyMatrix.f[ 6] = - FMXMUL(( FMXMUL(mIn.f[ 0], mIn.f[ 6]) - FMXMUL(mIn.f[ 4], mIn.f[ 2]) ), det_1);
		mDummyMatrix.f[ 8] =   FMXMUL(( FMXMUL(mIn.f[ 4], mIn.f[ 9]) - FMXMUL(mIn.f[ 8], mIn.f[ 5]) ), det_1);
		mDummyMatrix.f[ 9] = - FMXMUL(( FMXMUL(mIn.f[ 0], mIn.f[ 9]) - FMXMUL(mIn.f[ 8], mIn.f[ 1]) ), det_1);
		mDummyMatrix.f[10] =   FMXMUL(( FMXMUL(mIn.f[ 0], mIn.f[ 5]) - FMXMUL(mIn.f[ 4], mIn.f[ 1]) ), det_1);

		/* Calculate -C * inverse(A) */
		mDummyMatrix.f[12] = - ( FMXMUL(mIn.f[12], mDummyMatrix.f[ 0]) + FMXMUL(mIn.f[13], mDummyMatrix.f[ 4]) + FMXMUL(mIn.f[14], mDummyMatrix.f[ 8]) );
		mDummyMatrix.f[13] = - ( FMXMUL(mIn.f[12], mDummyMatrix.f[ 1]) + FMXMUL(mIn.f[13], mDummyMatrix.f[ 5]) + FMXMUL(mIn.f[14], mDummyMatrix.f[ 9]) );
		mDummyMatrix.f[14] = - ( FMXMUL(mIn.f[12], mDummyMatrix.f[ 2]) + FMXMUL(mIn.f[13], mDummyMatrix.f[ 6]) + FMXMUL(mIn.f[14], mDummyMatrix.f[10]) );

		/* Fill in last row */
		mDummyMatrix.f[ 3] = FMF2X(0.0f);
		mDummyMatrix.f[ 7] = FMF2X(0.0f);
		mDummyMatrix.f[11] = FMF2X(0.0f);
		mDummyMatrix.f[15] = FMF2X(1.0f);
	}

	/* Copy contents of dummy matrix in pfMatrix */
	*pOut = mDummyMatrix;
}

/*!***************************************************************************
@Function			FMMatrixInverseExX
@Output			mOut	Inversed matrix
@Input				mIn		Original matrix
@Description		Compute the inverse matrix of mIn.
Uses a linear equation solver and the knowledge that M.M^-1=I.
Use this fn to calculate the inverse of matrices that
FmMatrixInverse() cannot.
*****************************************************************************/
void FmMatrixInverseExX(
						FmMATRIXx			*pOut,
						const FmMATRIXx	*pIn)
{
	Assert( (pOut != NULL) && (pIn != NULL) );
	FmMATRIXx mIn = *pIn;
	FmMATRIXx		mTmp;
	int				*ppfRows[4], pfRes[4], pfIn[20];
	int				i, j;

	for (i = 0; i < 4; ++i)
	{
		ppfRows[i] = &pfIn[i * 5];
	}

	/* Solve 4 sets of 4 linear equations */
	for (i = 0; i < 4; ++i)
	{
		for (j = 0; j < 4; ++j)
		{
			ppfRows[j][0] = c_mIdentity.f[i + 4 * j];
			memcpy(&ppfRows[j][1], &mIn.f[j * 4], 4 * sizeof(float));
		}

		FmMatrixLinearEqSolveX(pfRes, (int**)ppfRows, 4);

		for(j = 0; j < 4; ++j)
		{
			mTmp.f[i + 4 * j] = pfRes[j];
		}
	}

	*pOut = mTmp;
}

/*!***************************************************************************
@Function			FmMatrixLookAtLHX
@Output			mOut	Look-at view matrix
@Input				vEye	Position of the camera
@Input				vAt		Point the camera is looking at
@Input				vUp		Up direction for the camera
@Description		Create a look-at view matrix.
*****************************************************************************/
void FmMatrixLookAtLHX(
					   FmMATRIXx			*pOut,
					   const FmVECTOR3x	*pvEye,
					   const FmVECTOR3x	*pvAt,
					   const FmVECTOR3x	*pvUp)
{
	Assert( (pOut != NULL) && (pvEye != NULL) && (pvAt != NULL) && (pvUp != NULL) );
	FmVECTOR3x vEye = *pvEye;
	FmVECTOR3x vAt = *pvAt;
	FmVECTOR3x vUp = *pvUp;

	FmVECTOR3x	f, vUpActual, s, u;
	FmMATRIXx		t;

	f.x = vEye.x - vAt.x;
	f.y = vEye.y - vAt.y;
	f.z = vEye.z - vAt.z;

	FmMatrixVec3NormalizeX(&f, &f);
	FmMatrixVec3NormalizeX(&vUpActual, &vUp);
	FmMatrixVec3CrossProductX(&s, &f, &vUpActual);
	FmMatrixVec3CrossProductX(&u, &s, &f);

	(*pOut).f[ 0] = s.x;
	(*pOut).f[ 1] = u.x;
	(*pOut).f[ 2] = -f.x;
	(*pOut).f[ 3] = FMF2X(0.0f);

	(*pOut).f[ 4] = s.y;
	(*pOut).f[ 5] = u.y;
	(*pOut).f[ 6] = -f.y;
	(*pOut).f[ 7] = FMF2X(0.0f);

	(*pOut).f[ 8] = s.z;
	(*pOut).f[ 9] = u.z;
	(*pOut).f[10] = -f.z;
	(*pOut).f[11] = FMF2X(0.0f);

	(*pOut).f[12] = FMF2X(0.0f);
	(*pOut).f[13] = FMF2X(0.0f);
	(*pOut).f[14] = FMF2X(0.0f);
	(*pOut).f[15] = FMF2X(1.0f);

	FmMatrixTranslationX(&t, -vEye.x, -vEye.y, -vEye.z);
	FmMatrixMultiplyX(pOut, &t, pOut);
}

/*!***************************************************************************
@Function			FmMatrixLookAtRHX
@Output			mOut	Look-at view matrix
@Input				vEye	Position of the camera
@Input				vAt		Point the camera is looking at
@Input				vUp		Up direction for the camera
@Description		Create a look-at view matrix.
*****************************************************************************/
void FmMatrixLookAtRHX(
					   FmMATRIXx			*pOut,
					   const FmVECTOR3x	*pvEye,
					   const FmVECTOR3x	*pvAt,
					   const FmVECTOR3x	*pvUp)
{
	Assert( (pOut != NULL) && (pvEye != NULL) && (pvAt != NULL) && (pvUp != NULL) );
	FmVECTOR3x vEye = *pvEye;
	FmVECTOR3x vAt = *pvAt;
	FmVECTOR3x vUp = *pvUp;

	FmVECTOR3x	f, vUpActual, s, u;
	FmMATRIXx		t;

	f.x = vAt.x - vEye.x;
	f.y = vAt.y - vEye.y;
	f.z = vAt.z - vEye.z;

	FmMatrixVec3NormalizeX(&f, &f);
	FmMatrixVec3NormalizeX(&vUpActual, &vUp);
	FmMatrixVec3CrossProductX(&s, &f, &vUpActual);
	FmMatrixVec3CrossProductX(&u, &s, &f);

	(*pOut).f[ 0] = s.x;
	(*pOut).f[ 1] = u.x;
	(*pOut).f[ 2] = -f.x;
	(*pOut).f[ 3] = FMF2X(0.0f);

	(*pOut).f[ 4] = s.y;
	(*pOut).f[ 5] = u.y;
	(*pOut).f[ 6] = -f.y;
	(*pOut).f[ 7] = FMF2X(0.0f);

	(*pOut).f[ 8] = s.z;
	(*pOut).f[ 9] = u.z;
	(*pOut).f[10] = -f.z;
	(*pOut).f[11] = FMF2X(0.0f);

	(*pOut).f[12] = FMF2X(0.0f);
	(*pOut).f[13] = FMF2X(0.0f);
	(*pOut).f[14] = FMF2X(0.0f);
	(*pOut).f[15] = FMF2X(1.0f);

	FmMatrixTranslationX(&t, -vEye.x, -vEye.y, -vEye.z);
	FmMatrixMultiplyX(pOut, &t, pOut);
}

/*!***************************************************************************
@Function		FmMatrixPerspectiveFovLHX
@Output		mOut		Perspective matrix
@Input			fFOVy		Field of view
@Input			fAspect		Aspect ratio
@Input			fNear		Near clipping distance
@Input			fFar		Far clipping distance
@Input			bRotate		Should we rotate it ? (for upright screens)
@Description	Create a perspective matrix.
*****************************************************************************/
void FmMatrixPerspectiveFovLHX(
							   FmMATRIXx	*pOut,
							   const int	fFOVy,
							   const int	fAspect,
							   const int	fNear,
							   const int	fFar,
							   const int  bRotate)
{
	Assert( pOut != NULL );
	int		f, fRealAspect;

	if (bRotate)
		fRealAspect = FMXDIV(FMF2X(1.0f), fAspect);
	else
		fRealAspect = fAspect;

	f = FMXDIV(FMF2X(1.0f), FMXTAN(FMXMUL(fFOVy, FMF2X(0.5f))));

	(*pOut).f[ 0] = FMXDIV(f, fRealAspect);
	(*pOut).f[ 1] = FMF2X(0.0f);
	(*pOut).f[ 2] = FMF2X(0.0f);
	(*pOut).f[ 3] = FMF2X(0.0f);

	(*pOut).f[ 4] = FMF2X(0.0f);
	(*pOut).f[ 5] = f;
	(*pOut).f[ 6] = FMF2X(0.0f);
	(*pOut).f[ 7] = FMF2X(0.0f);

	(*pOut).f[ 8] = FMF2X(0.0f);
	(*pOut).f[ 9] = FMF2X(0.0f);
	(*pOut).f[10] = FMXDIV(fFar, fFar - fNear);
	(*pOut).f[11] = FMF2X(1.0f);

	(*pOut).f[12] = FMF2X(0.0f);
	(*pOut).f[13] = FMF2X(0.0f);
	(*pOut).f[14] = -FMXMUL(FMXDIV(fFar, fFar - fNear), fNear);
	(*pOut).f[15] = FMF2X(0.0f);

	if (bRotate)
	{
		FmMATRIXx mRotation, mTemp = *pOut;
		FmMatrixRotationZX(&mRotation, FMF2X(90.0f*FM_PIf/180.0f));
		FmMatrixMultiplyX(pOut, &mTemp, &mRotation);
	}
}

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
void FmMatrixPerspectiveFovRHX(
							   FmMATRIXx	*pOut,
							   const int	fFOVy,
							   const int	fAspect,
							   const int	fNear,
							   const int	fFar,
							   const int  bRotate)
{
	Assert( pOut != NULL );
	int		f;

	int fCorrectAspect = fAspect;
	if (bRotate)
	{
		fCorrectAspect = FMXDIV(FMF2X(1.0f), fAspect);
	}
	f = FMXDIV(FMF2X(1.0f), FMXTAN(FMXMUL(fFOVy, FMF2X(0.5f))));

	(*pOut).f[ 0] = FMXDIV(f, fCorrectAspect);
	(*pOut).f[ 1] = FMF2X(0.0f);
	(*pOut).f[ 2] = FMF2X(0.0f);
	(*pOut).f[ 3] = FMF2X(0.0f);

	(*pOut).f[ 4] = FMF2X(0.0f);
	(*pOut).f[ 5] = f;
	(*pOut).f[ 6] = FMF2X(0.0f);
	(*pOut).f[ 7] = FMF2X(0.0f);

	(*pOut).f[ 8] = FMF2X(0.0f);
	(*pOut).f[ 9] = FMF2X(0.0f);
	(*pOut).f[10] = FMXDIV(fFar + fNear, fNear - fFar);
	(*pOut).f[11] = FMF2X(-1.0f);

	(*pOut).f[12] = FMF2X(0.0f);
	(*pOut).f[13] = FMF2X(0.0f);
	(*pOut).f[14] = FMXMUL(FMXDIV(fFar, fNear - fFar), fNear) << 1;	// Cheap 2x
	(*pOut).f[15] = FMF2X(0.0f);

	if (bRotate)
	{
		FmMATRIXx mRotation, mTemp = *pOut;
		FmMatrixRotationZX(&mRotation, FMF2X(-90.0f*FM_PIf/180.0f));
		FmMatrixMultiplyX(pOut, &mTemp, &mRotation);
	}
}

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
void FmMatrixOrthoLHX(
					  FmMATRIXx	*pOut,
					  const int	w,
					  const int	h,
					  const int	zn,
					  const int	zf,
					  const int  bRotate)
{
	Assert( pOut != NULL );
	int fCorrectW = w;
	int fCorrectH = h;
	if (bRotate)
	{
		fCorrectW = h;
		fCorrectH = w;
	}
	(*pOut).f[ 0] = FMXDIV(FMF2X(2.0f), fCorrectW);
	(*pOut).f[ 1] = FMF2X(0.0f);
	(*pOut).f[ 2] = FMF2X(0.0f);
	(*pOut).f[ 3] = FMF2X(0.0f);

	(*pOut).f[ 4] = FMF2X(0.0f);
	(*pOut).f[ 5] = FMXDIV(FMF2X(2.0f), fCorrectH);
	(*pOut).f[ 6] = FMF2X(0.0f);
	(*pOut).f[ 7] = FMF2X(0.0f);

	(*pOut).f[ 8] = FMF2X(0.0f);
	(*pOut).f[ 9] = FMF2X(0.0f);
	(*pOut).f[10] = FMXDIV(FMF2X(1.0f), zf - zn);
	(*pOut).f[11] = FMXDIV(zn, zn - zf);

	(*pOut).f[12] = FMF2X(0.0f);
	(*pOut).f[13] = FMF2X(0.0f);
	(*pOut).f[14] = FMF2X(0.0f);
	(*pOut).f[15] = FMF2X(1.0f);

	if (bRotate)
	{
		FmMATRIXx mRotation, mTemp = *pOut;
		FmMatrixRotationZX(&mRotation, FMF2X(-90.0f*FM_PIf/180.0f));
		FmMatrixMultiplyX(pOut, &mRotation, &mTemp);
	}
}

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
void FmMatrixOrthoRHX(
					  FmMATRIXx	*pOut,
					  const int	w,
					  const int	h,
					  const int	zn,
					  const int	zf,
					  const int  bRotate)
{
	Assert( pOut != NULL );
	int fCorrectW = w;
	int fCorrectH = h;
	if (bRotate)
	{
		fCorrectW = h;
		fCorrectH = w;
	}
	(*pOut).f[ 0] = FMXDIV(FMF2X(2.0f), fCorrectW);
	(*pOut).f[ 1] = FMF2X(0.0f);
	(*pOut).f[ 2] = FMF2X(0.0f);
	(*pOut).f[ 3] = FMF2X(0.0f);

	(*pOut).f[ 4] = FMF2X(0.0f);
	(*pOut).f[ 5] = FMXDIV(FMF2X(2.0f), fCorrectH);
	(*pOut).f[ 6] = FMF2X(0.0f);
	(*pOut).f[ 7] = FMF2X(0.0f);

	(*pOut).f[ 8] = FMF2X(0.0f);
	(*pOut).f[ 9] = FMF2X(0.0f);
	(*pOut).f[10] = FMXDIV(FMF2X(1.0f), zn - zf);
	(*pOut).f[11] = FMXDIV(zn, zn - zf);

	(*pOut).f[12] = FMF2X(0.0f);
	(*pOut).f[13] = FMF2X(0.0f);
	(*pOut).f[14] = FMF2X(0.0f);
	(*pOut).f[15] = FMF2X(1.0f);

	if (bRotate)
	{
		FmMATRIXx mRotation, mTemp = *pOut;
		FmMatrixRotationZX(&mRotation, FMF2X(-90.0f*FM_PIf/180.0f));
		FmMatrixMultiplyX(pOut, &mRotation, &mTemp);
	}
}

/*!***************************************************************************
@Function			FmMatrixVec3LerpX
@Output			vOut	Result of the interpolation
@Input				v1		First vector to interpolate from
@Input				v2		Second vector to interpolate form
@Input				s		Coefficient of interpolation
@Description		This function performs the linear interpolation based on
the following formula: V1 + s(V2-V1).
*****************************************************************************/
void FmMatrixVec3LerpX(
					   FmVECTOR3x		*pvOut,
					   const FmVECTOR3x	*pv1,
					   const FmVECTOR3x	*pv2,
					   const int			s)
{
	Assert( (pvOut != NULL) && (pv1 != NULL) && (pv2 != NULL) );
	(*pvOut).x = (*pv1).x + FMXMUL(s, (*pv2).x - (*pv1).x);
	(*pvOut).y = (*pv1).y + FMXMUL(s, (*pv2).y - (*pv1).y);
	(*pvOut).z = (*pv1).z + FMXMUL(s, (*pv2).z - (*pv1).z);
}

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
int FmMatrixVec3DotProductX(
							const FmVECTOR3x	*pv1,
							const FmVECTOR3x	*pv2)
{
	Assert( (pv1 != NULL) && (pv2 != NULL ));
	return (FMXMUL((*pv1).x, (*pv2).x) + FMXMUL((*pv1).y, (*pv2).y) + FMXMUL((*pv1).z, (*pv2).z));
}

/*!***************************************************************************
@Function			FmMatrixVec3CrossProductX
@Output			vOut	Cross product of the two vectors
@Input				v1		First vector
@Input				v2		Second vector
@Description		This function performs the cross product of the two
supplied vectors.
*****************************************************************************/
void FmMatrixVec3CrossProductX(
							   FmVECTOR3x		*pvOut,
							   const FmVECTOR3x	*pv1,
							   const FmVECTOR3x	*pv2)
{
	Assert( (pvOut != NULL) && (pv1 != NULL) && (pv2 != NULL) );
	FmVECTOR3x v1 = *pv1;
	FmVECTOR3x v2 = *pv2;

	FmVECTOR3x result;

	/* Perform calculation on a dummy VECTOR (result) */
	result.x = FMXMUL(v1.y, v2.z) - FMXMUL(v1.z, v2.y);
	result.y = FMXMUL(v1.z, v2.x) - FMXMUL(v1.x, v2.z);
	result.z = FMXMUL(v1.x, v2.y) - FMXMUL(v1.y, v2.x);

	/* Copy result in pOut */
	*pvOut = result;
}

/*!***************************************************************************
@Function			FmMatrixVec3NormalizeX
@Output			vOut	Normalized vector
@Input				vIn		Vector to normalize
@Description		Normalizes the supplied vector.
The square root function is currently still performed
in floating-point.
Original vector is scaled down prior to be normalized in
order to avoid overflow issues.
****************************************************************************/
void FmMatrixVec3NormalizeX(
							FmVECTOR3x		*pvOut,
							const FmVECTOR3x	*pvIn)
{
	Assert( (pvOut != NULL) && (pvIn != NULL) );
    FmVECTOR3x vIn = *pvIn;
	int				f, n;
	FmVECTOR3x	vTemp;

	/* Scale vector by uniform value */
	n = FMABS(vIn.x) + FMABS(vIn.y) + FMABS(vIn.z);
	vTemp.x = FMXDIV(vIn.x, n);
	vTemp.y = FMXDIV(vIn.y, n);
	vTemp.z = FMXDIV(vIn.z, n);

	/* Calculate x2+y2+z2/sqrt(x2+y2+z2) */
	f = FmMatrixVec3DotProductX(&vTemp, &vTemp);
	f = FMXDIV(FMF2X(1.0f), FMF2X(sqrt(FMX2F(f))));

	/* Multiply vector components by f */
	(*pvOut).x = FMXMUL(vTemp.x, f);
	(*pvOut).y = FMXMUL(vTemp.y, f);
	(*pvOut).z = FMXMUL(vTemp.z, f);
}

/*!***************************************************************************
@Function			FmMatrixVec3LengthX
@Input				vIn		Vector to get the length of
@Return			The length of the vector
@Description		Gets the length of the supplied vector
*****************************************************************************/
int FmMatrixVec3LengthX(
						const FmVECTOR3x	*pvIn)
{
	Assert( pvIn != NULL );
	int temp;

	temp = FMXMUL((*pvIn).x,(*pvIn).x) + FMXMUL((*pvIn).y,(*pvIn).y) + FMXMUL((*pvIn).z,(*pvIn).z);
	return FMF2X(sqrt(FMX2F(temp)));
}

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
void FmMatrixLinearEqSolveX(
							int			* const pRes,
							int			** const pSrc,
							const int	nCnt)
{
	Assert( pRes != NULL );
	int		i, j, k;
	int		f;

	if (nCnt == 1)
	{
		Assert(pSrc[0][1] != 0);
		pRes[0] = FMXDIV(pSrc[0][0], pSrc[0][1]);
		return;
	}

	// Loop backwards in an attempt avoid the need to swap rows
	i = nCnt;
	while(i)
	{
		--i;

		if(pSrc[i][nCnt] != FMF2X(0.0f))
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
				Assert(pSrc[nCnt-1][nCnt] != FMF2X(0.0f));
				f = FMXDIV(pSrc[j][nCnt], pSrc[nCnt-1][nCnt]);

				// No need to actually calculate a zero for the final column
				for(k = 0; k < nCnt; ++k)
				{
					pSrc[j][k] -= FMXMUL(f, pSrc[nCnt-1][k]);
				}
			}

			break;
		}
	}

	// Solve the top-left sub matrix
	FmMatrixLinearEqSolveX(pRes, pSrc, nCnt - 1);

	// Now calc the solution for the bottom row
	f = pSrc[nCnt-1][0];
	for(k = 1; k < nCnt; ++k)
	{
		f -= FMXMUL(pSrc[nCnt-1][k], pRes[k-1]);
	}
	Assert(pSrc[nCnt-1][nCnt] != FMF2X(0));
	f = FMXDIV(f, pSrc[nCnt-1][nCnt]);
	pRes[nCnt-1] = f;
}

