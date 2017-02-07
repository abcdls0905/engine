#ifndef _FM_QUATERNION_H
#define _FM_QUATERNION_H


#include "fm_matrix.h"

/****************************************************************************
** Typedefs
****************************************************************************/
/*!***************************************************************************
Floating point Quaternion
*****************************************************************************/
typedef struct
{
	float x;	/*!< x coordinate */
	float y;	/*!< y coordinate */
	float z;	/*!< z coordinate */
	float w;	/*!< w coordinate */
} FmQUATERNIONf;
/*!***************************************************************************
Fixed point Quaternion
*****************************************************************************/
typedef struct
{
	int x;	/*!< x coordinate */
	int y;	/*!< y coordinate */
	int z;	/*!< z coordinate */
	int w;	/*!< w coordinate */
} FmQUATERNIONx;

/****************************************************************************
** Float or fixed
****************************************************************************/
#ifdef FM_FIXED_POINT_ENABLE
typedef FmQUATERNIONx		FmQUATERNION;
#define FmMatrixQuaternionIdentity		FmMatrixQuaternionIdentityX
#define FmMatrixQuaternionRotationAxis	FmMatrixQuaternionRotationAxisX
#define FmMatrixQuaternionToAxisAngle	FmMatrixQuaternionToAxisAngleX
#define FmMatrixQuaternionSlerp			FmMatrixQuaternionSlerpX
#define FmMatrixQuaternionNormalize		FmMatrixQuaternionNormalizeX
#define FmMatrixRotationQuaternion		FmMatrixRotationQuaternionX
#define FmMatrixQuaternionMultiply		FmMatrixQuaternionMultiplyX
#else
typedef FmQUATERNIONf		FmQUATERNION;
#define FmMatrixQuaternionIdentity		FmMatrixQuaternionIdentityF
#define FmMatrixQuaternionRotationAxis	FmMatrixQuaternionRotationAxisF
#define FmMatrixQuaternionToAxisAngle	FmMatrixQuaternionToAxisAngleF
#define FmMatrixQuaternionSlerp			FmMatrixQuaternionSlerpF
#define FmMatrixQuaternionNormalize		FmMatrixQuaternionNormalizeF
#define FmMatrixRotationQuaternion		FmMatrixRotationQuaternionF
#define FmMatrixQuaternionMultiply		FmMatrixQuaternionMultiplyF
#endif



/****************************************************************************
** Functions
****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif


/*!***************************************************************************
@Function			FmMatrixQuaternionIdentityF
@Output			qOut	Identity quaternion
@Description		Sets the quaternion to (0, 0, 0, 1), the identity quaternion.
*****************************************************************************/
extern void FmMatrixQuaternionIdentityF(
								 FmQUATERNIONf		*pqOut);

/*!***************************************************************************
@Function			FmMatrixQuaternionIdentityX
@Output			qOut	Identity quaternion
@Description		Sets the quaternion to (0, 0, 0, 1), the identity quaternion.
*****************************************************************************/
extern void FmMatrixQuaternionIdentityX(
								 FmQUATERNIONx		*pqOut);

/*!***************************************************************************
@Function			FmMatrixQuaternionRotationAxisF
@Output			qOut	Rotation quaternion
@Input				vAxis	Axis to rotate around
@Input				fAngle	Angle to rotate
@Description		Create quaternion corresponding to a rotation of fAngle
radians around submitted vector.
*****************************************************************************/
extern void FmMatrixQuaternionRotationAxisF(
									 FmQUATERNIONf		*pqOut,
									 const FmVECTOR3f	*pvAxis,
									 const float			fAngle);

/*!***************************************************************************
@Function			FmMatrixQuaternionRotationAxisX
@Output			qOut	Rotation quaternion
@Input				vAxis	Axis to rotate around
@Input				fAngle	Angle to rotate
@Description		Create quaternion corresponding to a rotation of fAngle
radians around submitted vector.
*****************************************************************************/
extern void FmMatrixQuaternionRotationAxisX(
									 FmQUATERNIONx		*pqOut,
									 const FmVECTOR3x	*pvAxis,
									 const int			fAngle);


/*!***************************************************************************
@Function			FmMatrixQuaternionToAxisAngleF
@Input				qIn		Quaternion to transform
@Output			vAxis	Axis of rotation
@Output			fAngle	Angle of rotation
@Description		Convert a quaternion to an axis and angle. Expects a unit
quaternion.
*****************************************************************************/
extern void FmMatrixQuaternionToAxisAngleF(
									const FmQUATERNIONf	*pqIn,
									FmVECTOR3f			*pvAxis,
									float					*pfAngle);

/*!***************************************************************************
@Function			FmMatrixQuaternionToAxisAngleX
@Input				qIn		Quaternion to transform
@Output			vAxis	Axis of rotation
@Output			fAngle	Angle of rotation
@Description		Convert a quaternion to an axis and angle. Expects a unit
quaternion.
*****************************************************************************/
extern void FmMatrixQuaternionToAxisAngleX(
									const FmQUATERNIONx	*pqIn,
									FmVECTOR3x			*pvAxis,
									int						*pfAngle);

/*!***************************************************************************
@Function			FmMatrixQuaternionSlerpF
@Output			qOut	Result of the interpolation
@Input				qA		First quaternion to interpolate from
@Input				qB		Second quaternion to interpolate from
@Input				t		Coefficient of interpolation
@Description		Perform a Spherical Linear intERPolation between quaternion A
and quaternion B at time t. t must be between 0.0f and 1.0f
*****************************************************************************/
extern void FmMatrixQuaternionSlerpF(
							  FmQUATERNIONf			*pqOut,
							  const FmQUATERNIONf	*pqA,
							  const FmQUATERNIONf	*pqB,
							  const float				t);

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
extern void FmMatrixQuaternionSlerpX(
							  FmQUATERNIONx			*pqOut,
							  const FmQUATERNIONx	*pqA,
							  const FmQUATERNIONx	*pqB,
							  const int				t);

/*!***************************************************************************
@Function			FmMatrixQuaternionNormalizeF
@Modified			quat	Vector to normalize
@Description		Normalize quaternion.
*****************************************************************************/
extern void FmMatrixQuaternionNormalizeF(FmQUATERNIONf *pquat);

/*!***************************************************************************
@Function			FmMatrixQuaternionNormalizeX
@Modified			quat	Vector to normalize
@Description		Normalize quaternion.
Original quaternion is scaled down prior to be normalized in
order to avoid overflow issues.
*****************************************************************************/
extern void FmMatrixQuaternionNormalizeX(FmQUATERNIONx *pquat);

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
extern void FmMatrixRotationQuaternionF(
								 FmMATRIXf				*pOut,
								 const FmQUATERNIONf	*pquat);

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
extern void FmMatrixRotationQuaternionX(
								 FmMATRIXx				*pOut,
								 const FmQUATERNIONx	*pquat);

/*!***************************************************************************
@Function			FmMatrixQuaternionMultiplyF
@Output			qOut	Resulting quaternion
@Input				qA		First quaternion to multiply
@Input				qB		Second quaternion to multiply
@Description		Multiply quaternion A with quaternion B and return the
result in qOut.
*****************************************************************************/
extern void FmMatrixQuaternionMultiplyF(
								 FmQUATERNIONf			*pqOut,
								 const FmQUATERNIONf	*pqA,
								 const FmQUATERNIONf	*pqB);

/*!***************************************************************************
@Function			FmMatrixQuaternionMultiplyX
@Output			qOut	Resulting quaternion
@Input				qA		First quaternion to multiply
@Input				qB		Second quaternion to multiply
@Description		Multiply quaternion A with quaternion B and return the
result in qOut.
Input quaternions must be normalized.
*****************************************************************************/
extern void FmMatrixQuaternionMultiplyX(
								 FmQUATERNIONx			*pqOut,
								 const FmQUATERNIONx	*pqA,
								 const FmQUATERNIONx	*pqB);


#ifdef __cplusplus
}
#endif


#endif