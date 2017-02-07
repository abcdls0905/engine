
#include "../math/fm_quaternion.h"
#include "../math/fm_fixedpoint.h"

#include <math.h>
#include <string.h>


/****************************************************************************
** Functions
****************************************************************************/

/*!***************************************************************************
@Function			FmMatrixQuaternionIdentityF
@Output			qOut	Identity quaternion
@Description		Sets the quaternion to (0, 0, 0, 1), the identity quaternion.
*****************************************************************************/
void FmMatrixQuaternionIdentityF(FmQUATERNIONf *pqOut)
{
	Assert( pqOut != NULL );
	(*pqOut).x = 0;
	(*pqOut).y = 0;
	(*pqOut).z = 0;
	(*pqOut).w = 1;
}

/*!***************************************************************************
@Function			FmMatrixQuaternionRotationAxisF
@Output			qOut	Rotation quaternion
@Input				vAxis	Axis to rotate around
@Input				fAngle	Angle to rotate
@Description		Create quaternion corresponding to a rotation of fAngle
radians around submitted vector.
*****************************************************************************/
void FmMatrixQuaternionRotationAxisF(
									 FmQUATERNIONf		*pqOut,
									 const FmVECTOR3f	*pvAxis,
									 const float			fAngle)
{
	Assert( (pqOut != NULL ) && (pvAxis != NULL) );
	float	fSin, fCos;

	fSin = (float)FMFSIN(fAngle * 0.5f);
	fCos = (float)FMFCOS(fAngle * 0.5f);

	/* Create quaternion */
	(*pqOut).x = (*pvAxis).x * fSin;
	(*pqOut).y = (*pvAxis).y * fSin;
	(*pqOut).z = (*pvAxis).z * fSin;
	(*pqOut).w = fCos;

	/* Normalise it */
	FmMatrixQuaternionNormalizeF(pqOut);
}

/*!***************************************************************************
@Function			FmMatrixQuaternionToAxisAngleF
@Input				qIn		Quaternion to transform
@Output			vAxis	Axis of rotation
@Output			fAngle	Angle of rotation
@Description		Convert a quaternion to an axis and angle. Expects a unit
quaternion.
*****************************************************************************/
void FmMatrixQuaternionToAxisAngleF(
									const FmQUATERNIONf	*pqIn,
									FmVECTOR3f			*pvAxis,
									float					*pfAngle)
{
	Assert( (pqIn != NULL) && (pvAxis != NULL) && (pfAngle != NULL) );
	float	fCosAngle, fSinAngle;
	double	temp;

	/* Compute some values */
	fCosAngle	= (*pqIn).w;
	temp		= 1.0f - fCosAngle*fCosAngle;
	*pfAngle		= (float)FMFACOS(fCosAngle)*2.0f;
	fSinAngle	= (float)sqrt(temp);

	/* This is to avoid a division by zero */
	if ((float)fabs(fSinAngle)<0.0005f)
		fSinAngle = 1.0f;

	/* Get axis vector */
	(*pvAxis).x = (*pqIn).x / fSinAngle;
	(*pvAxis).y = (*pqIn).y / fSinAngle;
	(*pvAxis).z = (*pqIn).z / fSinAngle;
}

/*!***************************************************************************
@Function			FmMatrixQuaternionSlerpF
@Output			qOut	Result of the interpolation
@Input				qA		First quaternion to interpolate from
@Input				qB		Second quaternion to interpolate from
@Input				t		Coefficient of interpolation
@Description		Perform a Spherical Linear intERPolation between quaternion A
and quaternion B at time t. t must be between 0.0f and 1.0f
*****************************************************************************/
void FmMatrixQuaternionSlerpF(
							  FmQUATERNIONf			*pqOut,
							  const FmQUATERNIONf	*pqA,
							  const FmQUATERNIONf	*pqB,
							  const float				t)
{
	Assert( (pqOut != NULL ) && (pqA != NULL) && (pqB != NULL) );
	float		fCosine, fAngle, A, B;

	/* Parameter checking */
	if (t<0.0f || t>1.0f)
	{
		//_RPT0(_CRT_WARN, "FmMatrixQuaternionSlerp : Bad parameters\n");
		(*pqOut).x = 0;
		(*pqOut).y = 0;
		(*pqOut).z = 0;
		(*pqOut).w = 1;
		return;
	}

	/* Find sine of Angle between Quaternion A and B (dot product between quaternion A and B) */
	fCosine = (*pqA).w*(*pqB).w + (*pqA).x*(*pqB).x + (*pqA).y*(*pqB).y + (*pqA).z*(*pqB).z;

	if (fCosine < 0)
	{
		FmQUATERNIONf qi;

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

		FmMatrixQuaternionSlerpF(pqOut, pqA, &qi, t);
		return;
	}

	fCosine = FM_MIN(fCosine, 1.0f);
	fAngle = (float)FMFACOS(fCosine);

	/* Avoid a division by zero */
	if (fAngle==0.0f)
	{
		(*pqOut) = (*pqA);
		return;
	}

	/* Precompute some values */
	A = (float)(FMFSIN((1.0f-t)*fAngle) / FMFSIN(fAngle));
	B = (float)(FMFSIN(t*fAngle) / FMFSIN(fAngle));

	/* Compute resulting quaternion */
	(*pqOut).x = A * (*pqA).x + B * (*pqB).x;
	(*pqOut).y = A * (*pqA).y + B * (*pqB).y;
	(*pqOut).z = A * (*pqA).z + B * (*pqB).z;
	(*pqOut).w = A * (*pqA).w + B * (*pqB).w;

	/* Normalise result */
	FmMatrixQuaternionNormalizeF(pqOut);
}

/*!***************************************************************************
@Function			FmMatrixQuaternionNormalizeF
@Modified			quat	Vector to normalize
@Description		Normalize quaternion.
*****************************************************************************/
void FmMatrixQuaternionNormalizeF(FmQUATERNIONf *pquat)
{
	Assert( pquat != NULL );
	float	fMagnitude;
	double	temp;

	/* Compute quaternion magnitude */
	temp = (*pquat).w*(*pquat).w + (*pquat).x*(*pquat).x + (*pquat).y*(*pquat).y + (*pquat).z*(*pquat).z;
	fMagnitude = (float)sqrt(temp);

	/* Divide each quaternion component by this magnitude */
	if (fMagnitude!=0.0f)
	{
		fMagnitude = 1.0f / fMagnitude;
		(*pquat).x *= fMagnitude;
		(*pquat).y *= fMagnitude;
		(*pquat).z *= fMagnitude;
		(*pquat).w *= fMagnitude;
	}
}

/*!***************************************************************************
@Function			FmMatrixRotationQuaternionF
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
void FmMatrixRotationQuaternionF(
								 FmMATRIXf				*pOut,
								 const FmQUATERNIONf	*pquat)
{
	Assert( (pOut != NULL ) && (pquat != NULL ));
	const FmQUATERNIONf *pQ;

	pQ = pquat;

	/* Fill matrix members */
	(*pOut).f[0] = 1.0f - 2.0f*pQ->y*pQ->y - 2.0f*pQ->z*pQ->z;
	(*pOut).f[1] = 2.0f*pQ->x*pQ->y - 2.0f*pQ->z*pQ->w;
	(*pOut).f[2] = 2.0f*pQ->x*pQ->z + 2.0f*pQ->y*pQ->w;
	(*pOut).f[3] = 0.0f;

	(*pOut).f[4] = 2.0f*pQ->x*pQ->y + 2.0f*pQ->z*pQ->w;
	(*pOut).f[5] = 1.0f - 2.0f*pQ->x*pQ->x - 2.0f*pQ->z*pQ->z;
	(*pOut).f[6] = 2.0f*pQ->y*pQ->z - 2.0f*pQ->x*pQ->w;
	(*pOut).f[7] = 0.0f;

	(*pOut).f[8] = 2.0f*pQ->x*pQ->z - 2*pQ->y*pQ->w;
	(*pOut).f[9] = 2.0f*pQ->y*pQ->z + 2.0f*pQ->x*pQ->w;
	(*pOut).f[10] = 1.0f - 2.0f*pQ->x*pQ->x - 2*pQ->y*pQ->y;
	(*pOut).f[11] = 0.0f;

	(*pOut).f[12] = 0.0f;
	(*pOut).f[13] = 0.0f;
	(*pOut).f[14] = 0.0f;
	(*pOut).f[15] = 1.0f;
}

/*!***************************************************************************
@Function			FmMatrixQuaternionMultiplyF
@Output			qOut	Resulting quaternion
@Input				qA		First quaternion to multiply
@Input				qB		Second quaternion to multiply
@Description		Multiply quaternion A with quaternion B and return the
result in qOut.
*****************************************************************************/
void FmMatrixQuaternionMultiplyF(
								 FmQUATERNIONf			*pqOut,
								 const FmQUATERNIONf	*pqA,
								 const FmQUATERNIONf	*pqB)
{
	Assert( (pqOut != NULL ) && (pqA != NULL) && (pqB != NULL ) );

	FmVECTOR3f	CrossProduct;

	/* Compute scalar component */
	(*pqOut).w = ((*pqA).w*(*pqB).w) - ((*pqA).x*(*pqB).x + (*pqA).y*(*pqB).y + (*pqA).z*(*pqB).z);

	/* Compute cross product */
	CrossProduct.x = (*pqA).y*(*pqB).z - (*pqA).z*(*pqB).y;
	CrossProduct.y = (*pqA).z*(*pqB).x - (*pqA).x*(*pqB).z;
	CrossProduct.z = (*pqA).x*(*pqB).y - (*pqA).y*(*pqB).x;

	/* Compute result vector */
	(*pqOut).x = ((*pqA).w * (*pqB).x) + ((*pqB).w * (*pqA).x) + CrossProduct.x;
	(*pqOut).y = ((*pqA).w * (*pqB).y) + ((*pqB).w * (*pqA).y) + CrossProduct.y;
	(*pqOut).z = ((*pqA).w * (*pqB).z) + ((*pqB).w * (*pqA).z) + CrossProduct.z;

	/* Normalize resulting quaternion */
	FmMatrixQuaternionNormalizeF(pqOut);
}
