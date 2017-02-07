#include "../math/fm_quaternion.h"
#include "../math/fm_fixedpoint.h"
#include <math.h>
#include <string.h>


/****************************************************************************
** Functions
****************************************************************************/

/*!***************************************************************************
@Function			FmMatrixQuaternionIdentityX
@Output			qOut	Identity quaternion
@Description		Sets the quaternion to (0, 0, 0, 1), the identity quaternion.
*****************************************************************************/
void FmMatrixQuaternionIdentityX(FmQUATERNIONx		*pqOut)
{
	(*pqOut).x = FMF2X(0.0f);
	(*pqOut).y = FMF2X(0.0f);
	(*pqOut).z = FMF2X(0.0f);
	(*pqOut).w = FMF2X(1.0f);
}

/*!***************************************************************************
@Function			FmMatrixQuaternionRotationAxisX
@Output			qOut	Rotation quaternion
@Input				vAxis	Axis to rotate around
@Input				fAngle	Angle to rotate
@Description		Create quaternion corresponding to a rotation of fAngle
radians around submitted vector.
*****************************************************************************/
void FmMatrixQuaternionRotationAxisX(
									 FmQUATERNIONx		*pqOut,
									 const FmVECTOR3x	*pvAxis,
									 const int			fAngle)
{
	Assert( (pqOut != NULL) && (pvAxis != NULL) );
	int	fSin, fCos;

	fSin = FMXSIN(fAngle>>1);
	fCos = FMXCOS(fAngle>>1);

	/* Create quaternion */
	(*pqOut).x = FMXMUL((*pvAxis).x, fSin);
	(*pqOut).y = FMXMUL((*pvAxis).y, fSin);
	(*pqOut).z = FMXMUL((*pvAxis).z, fSin);
	(*pqOut).w = fCos;

	/* Normalise it */
	FmMatrixQuaternionNormalizeX(pqOut);
}

/*!***************************************************************************
@Function			FmMatrixQuaternionToAxisAngleX
@Input				qIn		Quaternion to transform
@Output			vAxis	Axis of rotation
@Output			fAngle	Angle of rotation
@Description		Convert a quaternion to an axis and angle. Expects a unit
quaternion.
*****************************************************************************/
void FmMatrixQuaternionToAxisAngleX(
									const FmQUATERNIONx	*pqIn,
									FmVECTOR3x			*pvAxis,
									int						*pfAngle)
{
	Assert( (pqIn != NULL) && (pvAxis != NULL) && (pfAngle != NULL) );
	int		fCosAngle, fSinAngle;
	int		temp;

	/* Compute some values */
	fCosAngle	= (*pqIn).w;
	temp		= FMF2X(1.0f) - FMXMUL(fCosAngle, fCosAngle);
	*pfAngle		= FMXMUL(FMXACOS(fCosAngle), FMF2X(2.0f));
	fSinAngle	= FMF2X(((float)sqrt(FMX2F(temp))));

	/* This is to avoid a division by zero */
	if (FMABS(fSinAngle)<FMF2X(0.0005f))
	{
		fSinAngle = FMF2X(1.0f);
	}

	/* Get axis vector */
	(*pvAxis).x = FMXDIV((*pqIn).x, fSinAngle);
	(*pvAxis).y = FMXDIV((*pqIn).y, fSinAngle);
	(*pvAxis).z = FMXDIV((*pqIn).z, fSinAngle);
}

/*!***************************************************************************
@Function			FmMatrixQuaternionSlerpX
@Output			qOut	Result of the interpolation
@Input				qA		First quaternion to interpolate from
@Input				qB		Second quaternion to interpolate from
@Input				t		Coefficient of interpolation
@Description		Perform a Spherical Linear intERPolation between quaternion A
and quaternion B at time t. t must be between 0.0f and 1.0f
Requires input quaternions to be normalized
*****************************************************************************/
void FmMatrixQuaternionSlerpX(
							  FmQUATERNIONx			*pqOut,
							  const FmQUATERNIONx	*pqA,
							  const FmQUATERNIONx	*pqB,
							  const int				t)
{
	int		fCosine, fAngle, A, B;

	/* Parameter checking */
	if (t<FMF2X(0.0f) || t>FMF2X(1.0f))
	{
//		_RPT0(_CRT_WARN, "FmMatrixQuaternionSlerp : Bad parameters\n");
		(*pqOut).x = FMF2X(0.0f);
		(*pqOut).y = FMF2X(0.0f);
		(*pqOut).z = FMF2X(0.0f);
		(*pqOut).w = FMF2X(1.0f);
		return;
	}

	/* Find sine of Angle between Quaternion A and B (dot product between quaternion A and B) */
	fCosine = FMXMUL((*pqA).w, (*pqB).w) +
		FMXMUL((*pqA).x, (*pqB).x) + FMXMUL((*pqA).y, (*pqB).y) + FMXMUL((*pqA).z, (*pqB).z);

	if(fCosine < FMF2X(0.0f))
	{
		FmQUATERNIONx qi;

		/*
		<http://www.magic-software.com/Documentation/Quaternions.pdf>

		"It is important to note that the quaternions q and -q represent
		the same rotation... while either quaternion will do, the
		interpolation methods require choosing one over the other.

		"Although q1 and -q1 represent the same rotation, the values of
		Slerp(t; q0, q1) and Slerp(t; q0,-q1) are not the same. It is
		customary to choose the sign... on q1 so that... the angle
		between q0 and q1 is acute. This choice avoids extra
		spinning caused by the interpolated rotations."
		*/
		qi.x = -(*pqB).x;
		qi.y = -(*pqB).y;
		qi.z = -(*pqB).z;
		qi.w = -(*pqB).w;

		FmMatrixQuaternionSlerpX(pqOut, pqA, &qi, t);
		return;
	}

	fCosine = FM_MIN(fCosine, FMF2X(1.0f));
	fAngle = FMXACOS(fCosine);

	/* Avoid a division by zero */
	if (fAngle==FMF2X(0.0f))
	{
		*pqOut = *pqA;
		return;
	}

	/* Precompute some values */
	A = FMXDIV(FMXSIN(FMXMUL((FMF2X(1.0f)-t), fAngle)), FMXSIN(fAngle));
	B = FMXDIV(FMXSIN(FMXMUL(t, fAngle)), FMXSIN(fAngle));

	/* Compute resulting quaternion */
	(*pqOut).x = FMXMUL(A, (*pqA).x) + FMXMUL(B, (*pqB).x);
	(*pqOut).y = FMXMUL(A, (*pqA).y) + FMXMUL(B, (*pqB).y);
	(*pqOut).z = FMXMUL(A, (*pqA).z) + FMXMUL(B, (*pqB).z);
	(*pqOut).w = FMXMUL(A, (*pqA).w) + FMXMUL(B, (*pqB).w);

	/* Normalise result */
	FmMatrixQuaternionNormalizeX(pqOut);
}

/*!***************************************************************************
@Function			FmMatrixQuaternionNormalizeX
@Modified			quat	Vector to normalize
@Description		Normalize quaternion.
Original quaternion is scaled down prior to be normalized in
order to avoid overflow issues.
*****************************************************************************/
void FmMatrixQuaternionNormalizeX(FmQUATERNIONx *pquat)
{
	Assert( pquat != NULL );
	FmQUATERNIONx	qTemp;
	int				f, n;

	/* Scale vector by uniform value */
	n = FMABS((*pquat).w) + FMABS((*pquat).x) + FMABS((*pquat).y) + FMABS((*pquat).z);
	qTemp.w = FMXDIV((*pquat).w, n);
	qTemp.x = FMXDIV((*pquat).x, n);
	qTemp.y = FMXDIV((*pquat).y, n);
	qTemp.z = FMXDIV((*pquat).z, n);

	/* Compute quaternion magnitude */
	f = FMXMUL(qTemp.w, qTemp.w) + FMXMUL(qTemp.x, qTemp.x) + FMXMUL(qTemp.y, qTemp.y) + FMXMUL(qTemp.z, qTemp.z);
	f = FMXDIV(FMF2X(1.0f), FMF2X(sqrt(FMX2F(f))));

	/* Multiply vector components by f */
	(*pquat).x = FMXMUL(qTemp.x, f);
	(*pquat).y = FMXMUL(qTemp.y, f);
	(*pquat).z = FMXMUL(qTemp.z, f);
	(*pquat).w = FMXMUL(qTemp.w, f);
}

/*!***************************************************************************
@Function			FmMatrixRotationQuaternionX
@Output			mOut	Resulting rotation matrix
@Input				quat	Quaternion to transform
@Description		Create rotation matrix from submitted quaternion.
Assuming the quaternion is of the form [X Y Z W]:

|       2     2									|
| 1 - 2Y  - 2Z    2XY - 2ZW      2XZ + 2YW		 0	|
|													|
|                       2     2					|
M = | 2XY + 2ZW       1 - 2X  - 2Z   2YZ - 2XW		 0	|
|													|
|                                      2     2		|
| 2XZ - 2YW       2YZ + 2XW      1 - 2X  - 2Y	 0	|
|													|
|     0			   0			  0          1  |
*****************************************************************************/
void FmMatrixRotationQuaternionX(
								 FmMATRIXx				*pOut,
								 const FmQUATERNIONx	*pquat)
{
	Assert( (pOut != NULL ) && (pquat != NULL ) );
	const FmQUATERNIONx *pQ;
	pQ = pquat;

	/* Fill matrix members */
	(*pOut).f[0] = FMF2X(1.0f) - (FMXMUL(pQ->y, pQ->y)<<1) - (FMXMUL(pQ->z, pQ->z)<<1);
	(*pOut).f[1] = (FMXMUL(pQ->x, pQ->y)<<1) - (FMXMUL(pQ->z, pQ->w)<<1);
	(*pOut).f[2] = (FMXMUL(pQ->x, pQ->z)<<1) + (FMXMUL(pQ->y, pQ->w)<<1);
	(*pOut).f[3] = FMF2X(0.0f);

	(*pOut).f[4] = (FMXMUL(pQ->x, pQ->y)<<1) + (FMXMUL(pQ->z, pQ->w)<<1);
	(*pOut).f[5] = FMF2X(1.0f) - (FMXMUL(pQ->x, pQ->x)<<1) - (FMXMUL(pQ->z, pQ->z)<<1);
	(*pOut).f[6] = (FMXMUL(pQ->y, pQ->z)<<1) - (FMXMUL(pQ->x, pQ->w)<<1);
	(*pOut).f[7] = FMF2X(0.0f);

	(*pOut).f[8] = (FMXMUL(pQ->x, pQ->z)<<1) - (FMXMUL(pQ->y, pQ->w)<<1);
	(*pOut).f[9] = (FMXMUL(pQ->y, pQ->z)<<1) + (FMXMUL(pQ->x, pQ->w)<<1);
	(*pOut).f[10] = FMF2X(1.0f) - (FMXMUL(pQ->x, pQ->x)<<1) - (FMXMUL(pQ->y, pQ->y)<<1);
	(*pOut).f[11] = FMF2X(0.0f);

	(*pOut).f[12] = FMF2X(0.0f);
	(*pOut).f[13] = FMF2X(0.0f);
	(*pOut).f[14] = FMF2X(0.0f);
	(*pOut).f[15] = FMF2X(1.0f);
}

/*!***************************************************************************
@Function			FmMatrixQuaternionMultiplyX
@Output			qOut	Resulting quaternion
@Input				qA		First quaternion to multiply
@Input				qB		Second quaternion to multiply
@Description		Multiply quaternion A with quaternion B and return the
result in qOut.
Input quaternions must be normalized.
*****************************************************************************/
void FmMatrixQuaternionMultiplyX(
								 FmQUATERNIONx			*pqOut,
								 const FmQUATERNIONx	*pqA,
								 const FmQUATERNIONx	*pqB)
{
	Assert( (pqOut != NULL) && (pqA != NULL) && (pqB != NULL) );
	FmVECTOR3x	CrossProduct;

	/* Compute scalar component */
	(*pqOut).w = FMXMUL((*pqA).w, (*pqB).w) -
		(FMXMUL((*pqA).x, (*pqB).x) + FMXMUL((*pqA).y, (*pqB).y) + FMXMUL((*pqA).z, (*pqB).z));

	/* Compute cross product */
	CrossProduct.x = FMXMUL((*pqA).y, (*pqB).z) - FMXMUL((*pqA).z, (*pqB).y);
	CrossProduct.y = FMXMUL((*pqA).z, (*pqB).x) - FMXMUL((*pqA).x, (*pqB).z);
	CrossProduct.z = FMXMUL((*pqA).x, (*pqB).y) - FMXMUL((*pqA).y, (*pqB).x);

	/* Compute result vector */
	(*pqOut).x = FMXMUL((*pqA).w, (*pqB).x) + FMXMUL((*pqB).w, (*pqA).x) + CrossProduct.x;
	(*pqOut).y = FMXMUL((*pqA).w, (*pqB).y) + FMXMUL((*pqB).w, (*pqA).y) + CrossProduct.y;
	(*pqOut).z = FMXMUL((*pqA).w, (*pqB).z) + FMXMUL((*pqB).w, (*pqA).z) + CrossProduct.z;

	/* Normalize resulting quaternion */
	FmMatrixQuaternionNormalizeX(pqOut);
}

