#include "../math/fm_vector.h"
#include <math.h>

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


/*!***************************************************************************
** FmVec2 2 component vector
****************************************************************************/

/*!***************************************************************************
@Function			FmVec2
@Input				v3Vec a Vec3
@Description		Constructor from a Vec3
*****************************************************************************/
FmVec2::FmVec2(const FmVec3& vec3)
{
	x = vec3.x; y = vec3.y;
}

/*!***************************************************************************
** FmVec3 3 component vector
****************************************************************************/

/*!***************************************************************************
@Function			FmVec3
@Input				v4Vec a FmVec4
@Description		Constructor from a FmVec4
*****************************************************************************/
FmVec3::FmVec3(const FmVec4& vec4)
{
	x = vec4.x; y = vec4.y; z = vec4.z;
}

/*!***************************************************************************
@Function			*
@Input				rhs a FmMat3
@Returns			result of multiplication
@Description		matrix multiplication operator FmVec3 and FmMat3
****************************************************************************/
FmVec3 FmVec3::operator*(const FmMat3& rhs) const
{
	FmVec3 out;

	out.x = VERTTYPEMUL(x,rhs.f[0])+VERTTYPEMUL(y,rhs.f[1])+VERTTYPEMUL(z,rhs.f[2]);
	out.y = VERTTYPEMUL(x,rhs.f[3])+VERTTYPEMUL(y,rhs.f[4])+VERTTYPEMUL(z,rhs.f[5]);
	out.z = VERTTYPEMUL(x,rhs.f[6])+VERTTYPEMUL(y,rhs.f[7])+VERTTYPEMUL(z,rhs.f[8]);

	return out;
}

/*!***************************************************************************
@Function			*=
@Input				rhs a FmMat3
@Returns			result of multiplication and assignment
@Description		matrix multiplication and assignment operator for FmVec3 and FmMat3
****************************************************************************/
FmVec3& FmVec3::operator*=(const FmMat3& rhs)
{
	VERTTYPE tx = VERTTYPEMUL(x,rhs.f[0])+VERTTYPEMUL(y,rhs.f[1])+VERTTYPEMUL(z,rhs.f[2]);
	VERTTYPE ty = VERTTYPEMUL(x,rhs.f[3])+VERTTYPEMUL(y,rhs.f[4])+VERTTYPEMUL(z,rhs.f[5]);
	z = VERTTYPEMUL(x,rhs.f[6])+VERTTYPEMUL(y,rhs.f[7])+VERTTYPEMUL(z,rhs.f[8]);
	x = tx;
	y = ty;

	return *this;
}

/*!***************************************************************************
** FmVec4 4 component vector
****************************************************************************/

/*!***************************************************************************
@Function			*
@Input				rhs a FmMat4
@Returns			result of multiplication
@Description		matrix multiplication operator FmVec4 and FmMat4
****************************************************************************/
FmVec4 FmVec4::operator*(const FmMat4& rhs) const
{
	FmVec4 out;
	out.x = VERTTYPEMUL(x,rhs.f[0])+VERTTYPEMUL(y,rhs.f[1])+VERTTYPEMUL(z,rhs.f[2])+VERTTYPEMUL(w,rhs.f[3]);
	out.y = VERTTYPEMUL(x,rhs.f[4])+VERTTYPEMUL(y,rhs.f[5])+VERTTYPEMUL(z,rhs.f[6])+VERTTYPEMUL(w,rhs.f[7]);
	out.z = VERTTYPEMUL(x,rhs.f[8])+VERTTYPEMUL(y,rhs.f[9])+VERTTYPEMUL(z,rhs.f[10])+VERTTYPEMUL(w,rhs.f[11]);
	out.w = VERTTYPEMUL(x,rhs.f[12])+VERTTYPEMUL(y,rhs.f[13])+VERTTYPEMUL(z,rhs.f[14])+VERTTYPEMUL(w,rhs.f[15]);
	return out;
}

/*!***************************************************************************
@Function			*=
@Input				rhs a FmMat4
@Returns			result of multiplication and assignment
@Description		matrix multiplication and assignment operator for FmVec4 and FmMat4
****************************************************************************/
FmVec4& FmVec4::operator*=(const FmMat4& rhs)
{
	VERTTYPE tx = VERTTYPEMUL(x,rhs.f[0])+VERTTYPEMUL(y,rhs.f[1])+VERTTYPEMUL(z,rhs.f[2])+VERTTYPEMUL(w,rhs.f[3]);
	VERTTYPE ty = VERTTYPEMUL(x,rhs.f[4])+VERTTYPEMUL(y,rhs.f[5])+VERTTYPEMUL(z,rhs.f[6])+VERTTYPEMUL(w,rhs.f[7]);
	VERTTYPE tz = VERTTYPEMUL(x,rhs.f[8])+VERTTYPEMUL(y,rhs.f[9])+VERTTYPEMUL(z,rhs.f[10])+VERTTYPEMUL(w,rhs.f[11]);
	w = VERTTYPEMUL(x,rhs.f[12])+VERTTYPEMUL(y,rhs.f[13])+VERTTYPEMUL(z,rhs.f[14])+VERTTYPEMUL(w,rhs.f[15]);
	x = tx;
	y = ty;
	z = tz;
	return *this;
}

/*!***************************************************************************
** FmMat3 3x3 matrix
****************************************************************************/
/*!***************************************************************************
@Function			FmMat3
@Input				mat a FmMat4
@Description		constructor to form a FmMat3 from a FmMat4
****************************************************************************/
FmMat3::FmMat3(const FmMat4& mat)
{
	VERTTYPE *dest = (VERTTYPE*)f, *src = (VERTTYPE*)mat.f;
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
		{
			(*dest++) = (*src++);
		}
		src++;
	}
}

/*!***************************************************************************
@Function			RotationX
@Input				angle the angle of rotation
@Returns			rotation matrix
@Description		generates a 3x3 rotation matrix about the X axis
****************************************************************************/
FmMat3 FmMat3::RotationX(VERTTYPE angle)
{
	FmMat4 out;
	FmMatrixRotationX(&out,angle);
	return FmMat3(out);
}
/*!***************************************************************************
@Function			RotationY
@Input				angle the angle of rotation
@Returns			rotation matrix
@Description		generates a 3x3 rotation matrix about the Y axis
****************************************************************************/
FmMat3 FmMat3::RotationY(VERTTYPE angle)
{
	FmMat4 out;
	FmMatrixRotationY(&out,angle);
	return FmMat3(out);
}
/*!***************************************************************************
@Function			RotationZ
@Input				angle the angle of rotation
@Returns			rotation matrix
@Description		generates a 3x3 rotation matrix about the Z axis
****************************************************************************/
FmMat3 FmMat3::RotationZ(VERTTYPE angle)
{
	FmMat4 out;
	FmMatrixRotationZ(&out,angle);
	return FmMat3(out);
}


/*!***************************************************************************
** FmMat4 4x4 matrix
****************************************************************************/
/*!***************************************************************************
@Function			RotationX
@Input				angle the angle of rotation
@Returns			rotation matrix
@Description		generates a 4x4 rotation matrix about the X axis
****************************************************************************/
FmMat4 FmMat4::RotationX(VERTTYPE angle)
{
	FmMat4 out;
	FmMatrixRotationX(&out,angle);
	return out;
}
/*!***************************************************************************
@Function			RotationY
@Input				angle the angle of rotation
@Returns			rotation matrix
@Description		generates a 4x4 rotation matrix about the Y axis
****************************************************************************/
FmMat4 FmMat4::RotationY(VERTTYPE angle)
{
	FmMat4 out;
	FmMatrixRotationY(&out,angle);
	return out;
}
/*!***************************************************************************
@Function			RotationZ
@Input				angle the angle of rotation
@Returns			rotation matrix
@Description		generates a 4x4 rotation matrix about the Z axis
****************************************************************************/
FmMat4 FmMat4::RotationZ(VERTTYPE angle)
{
	FmMat4 out;
	FmMatrixRotationZ(&out,angle);
	return out;
}

/*!***************************************************************************
@Function			*
@Input				rhs another FmMat4
@Returns			result of multiplication
@Description		Matrix multiplication of two 4x4 matrices.
*****************************************************************************/
FmMat4 FmMat4::operator*(const FmMat4& rhs) const
{
	FmMat4 out;
	FmMatrixMultiply(&out, this, &rhs);
	return out;
}


/*!***************************************************************************
@Function			inverse
@Returns			inverse mat4
@Description		Calculates multiplicative inverse of this matrix
The matrix must be of the form :
A 0
C 1
Where A is a 3x3 matrix and C is a 1x3 matrix.
*****************************************************************************/
FmMat4 FmMat4::inverse() const
{
	FmMat4 out;
	VERTTYPE	det_1;
	VERTTYPE	pos, neg, temp;

	/* Calculate the determinant of submatrix A and determine if the
	the matrix is singular as limited by the double precision
	floating-point data representation. */
	pos = neg = f2vt(0.0);
	temp =  VERTTYPEMUL(VERTTYPEMUL(f[ 0], f[ 5]), f[10]);
	if (temp >= 0) pos += temp; else neg += temp;
	temp =  VERTTYPEMUL(VERTTYPEMUL(f[ 4], f[ 9]), f[ 2]);
	if (temp >= 0) pos += temp; else neg += temp;
	temp =  VERTTYPEMUL(VERTTYPEMUL(f[ 8], f[ 1]), f[ 6]);
	if (temp >= 0) pos += temp; else neg += temp;
	temp =  VERTTYPEMUL(VERTTYPEMUL(-f[ 8], f[ 5]), f[ 2]);
	if (temp >= 0) pos += temp; else neg += temp;
	temp =  VERTTYPEMUL(VERTTYPEMUL(-f[ 4], f[ 1]), f[10]);
	if (temp >= 0) pos += temp; else neg += temp;
	temp =  VERTTYPEMUL(VERTTYPEMUL(-f[ 0], f[ 9]), f[ 6]);
	if (temp >= 0) pos += temp; else neg += temp;
	det_1 = pos + neg;

	/* Is the submatrix A singular? */
	if (det_1 == f2vt(0.0)) //|| (VERTTYPEABS(det_1 / (pos - neg)) < 1.0e-15)
	{
		/* Matrix M has no inverse */
		//_RPT0(_CRT_WARN, "Matrix has no inverse : singular matrix\n");
	}
	else
	{
		/* Calculate inverse(A) = adj(A) / det(A) */
		//det_1 = 1.0 / det_1;
		det_1 = VERTTYPEDIV(f2vt(1.0f), det_1);
		out.f[ 0] =   VERTTYPEMUL(( VERTTYPEMUL(f[ 5], f[10]) - VERTTYPEMUL(f[ 9], f[ 6]) ), det_1);
		out.f[ 1] = - VERTTYPEMUL(( VERTTYPEMUL(f[ 1], f[10]) - VERTTYPEMUL(f[ 9], f[ 2]) ), det_1);
		out.f[ 2] =   VERTTYPEMUL(( VERTTYPEMUL(f[ 1], f[ 6]) - VERTTYPEMUL(f[ 5], f[ 2]) ), det_1);
		out.f[ 4] = - VERTTYPEMUL(( VERTTYPEMUL(f[ 4], f[10]) - VERTTYPEMUL(f[ 8], f[ 6]) ), det_1);
		out.f[ 5] =   VERTTYPEMUL(( VERTTYPEMUL(f[ 0], f[10]) - VERTTYPEMUL(f[ 8], f[ 2]) ), det_1);
		out.f[ 6] = - VERTTYPEMUL(( VERTTYPEMUL(f[ 0], f[ 6]) - VERTTYPEMUL(f[ 4], f[ 2]) ), det_1);
		out.f[ 8] =   VERTTYPEMUL(( VERTTYPEMUL(f[ 4], f[ 9]) - VERTTYPEMUL(f[ 8], f[ 5]) ), det_1);
		out.f[ 9] = - VERTTYPEMUL(( VERTTYPEMUL(f[ 0], f[ 9]) - VERTTYPEMUL(f[ 8], f[ 1]) ), det_1);
		out.f[10] =   VERTTYPEMUL(( VERTTYPEMUL(f[ 0], f[ 5]) - VERTTYPEMUL(f[ 4], f[ 1]) ), det_1);

		/* Calculate -C * inverse(A) */
		out.f[12] = - ( VERTTYPEMUL(f[12], out.f[ 0]) + VERTTYPEMUL(f[13], out.f[ 4]) + VERTTYPEMUL(f[14], out.f[ 8]) );
		out.f[13] = - ( VERTTYPEMUL(f[12], out.f[ 1]) + VERTTYPEMUL(f[13], out.f[ 5]) + VERTTYPEMUL(f[14], out.f[ 9]) );
		out.f[14] = - ( VERTTYPEMUL(f[12], out.f[ 2]) + VERTTYPEMUL(f[13], out.f[ 6]) + VERTTYPEMUL(f[14], out.f[10]) );

		/* Fill in last row */
		out.f[ 3] = f2vt(0.0f);
		out.f[ 7] = f2vt(0.0f);
		out.f[11] = f2vt(0.0f);
		out.f[15] = f2vt(1.0f);
	}

	return out;
}

/*!***************************************************************************
@Function			FmLinearEqSolve
@Input				pSrc	2D array of floats. 4 Eq linear problem is 5x4
matrix, constants in first column
@Input				nCnt	Number of equations to solve
@Output			pRes	Result
@Description		Solves 'nCnt' simultaneous equations of 'nCnt' variables.
pRes should be an array large enough to contain the
results: the values of the 'nCnt' variables.
This fn recursively uses Gaussian Elimination.
*****************************************************************************/
void FmLinearEqSolve(VERTTYPE * const pRes, VERTTYPE ** const pSrc, const int nCnt)
{
	int			i, j, k;
	VERTTYPE	f;

	if (nCnt == 1)
	{
		Assert(pSrc[0][1] != 0);
		pRes[0] = VERTTYPEDIV(pSrc[0][0], pSrc[0][1]);
		return;
	}

	// Loop backwards in an attempt avoid the need to swap rows
	i = nCnt;
	while(i)
	{
		--i;

		if(pSrc[i][nCnt] != f2vt(0.0f))
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
				Assert(pSrc[nCnt-1][nCnt] != f2vt(0.0f));
				f = VERTTYPEDIV(pSrc[j][nCnt], pSrc[nCnt-1][nCnt]);

				// No need to actually calculate a zero for the final column
				for(k = 0; k < nCnt; ++k)
				{
					pSrc[j][k] -= VERTTYPEMUL(f, pSrc[nCnt-1][k]);
				}
			}

			break;
		}
	}

	// Solve the top-left sub matrix
	FmLinearEqSolve(pRes, pSrc, nCnt - 1);

	// Now calc the solution for the bottom row
	f = pSrc[nCnt-1][0];
	for(k = 1; k < nCnt; ++k)
	{
		f -= VERTTYPEMUL(pSrc[nCnt-1][k], pRes[k-1]);
	}
	Assert(pSrc[nCnt-1][nCnt] != f2vt(0));
	f = VERTTYPEDIV(f, pSrc[nCnt-1][nCnt]);
	pRes[nCnt-1] = f;
}

/*!***************************************************************************
@Function			normalize
@Returns			the normalized value of the plane
@Description		normalizes the plane
****************************************************************************/
FmPlane* FmPlaneNormalize(FmPlane* pOut,const FmPlane* pPlane)
{
	Assert( (pOut != NULL) && (pPlane != NULL) );
    VERTTYPE norm = sqrt( (*pPlane).a * (*pPlane).a + (*pPlane).b * (*pPlane).b + (*pPlane).c * (*pPlane).c );
    FmPlane p;
    if( norm )
	{
		p.a = VERTTYPEDIV( (*pPlane).a , norm);
		p.b = VERTTYPEDIV( (*pPlane).b , norm);
		p.c = VERTTYPEDIV( (*pPlane).c , norm);
		p.d = VERTTYPEDIV( (*pPlane).d , norm);
	}
	else
	{
		p.a = f2vt(0.0f);
		p.b = f2vt(0.0f);
		p.c = f2vt(0.0f);
		p.d = f2vt(0.0f);
	}

	*pOut = p;
	return pOut;
}

/*!***************************************************************************
@Function			FmPlaneTransform
@Input				pOut  contains the resulting transformed plane
@Input				pP    which contains the plane that will be transformed. The vector (a,b,c) that describes the plane must be normalized before this function is called. See example
@Input              pM    
@Returns			representing the transformed plane. This is the same value returned in the pOut parameter so that this function can be used as a parameter for another function
@Description		Transforms a plane by a matrix. The input matrix is the inverse transpose of the actual transformation
****************************************************************************/
FmPlane* FmPlaneTransform(FmPlane* pOut,const FmPlane* pP,const FmMat4* pM)
{
	Assert( (pOut != NULL) && (pP != NULL) && (pM != NULL) );

	const FmPlane plane = *pP;
    
	(*pOut).a = (*pM)(0,0) * plane.a + (*pM)(0,1)*plane.b + (*pM)(0,2) * plane.c + (*pM)(0,3) * plane.d;
	(*pOut).b = (*pM)(1,0) * plane.a + (*pM)(1,1)*plane.b + (*pM)(1,2) * plane.c + (*pM)(1,3) * plane.d;
	(*pOut).c = (*pM)(2,0) * plane.a + (*pM)(2,1)*plane.b + (*pM)(2,2) * plane.c + (*pM)(2,3) * plane.d;
	(*pOut).d = (*pM)(3,0) * plane.a + (*pM)(3,1)*plane.b + (*pM)(3,2) * plane.c + (*pM)(3,3) * plane.d;

	return pOut;
}

/*!***************************************************************************
@Function			FmPlaneFromPointNormal
@Input				pOut       FmPlane
@Input				pPoint     Pointer to the source FmVec3 structure
@Input				pNormal    Pointer to a FmVec3 structure, defining the normal used to construct the plane
@Returns			constructed from the point and the normal.
@Description		The return value for this function is the same value returned in the pOut parameter.
In this way, the D3DXPlaneFromPointNormal function can be used as a parameter for another function.
****************************************************************************/
FmPlane* FmPlaneFromPointNormal(FmPlane* pOut,const FmVec3* pPoint,const FmVec3* pNormal)
{
	Assert( (pOut != NULL) && (pPoint != NULL) && (pNormal != NULL) );

	(*pOut).a = (*pNormal).x;
	(*pOut).b = (*pNormal).y;
	(*pOut).c = (*pNormal).z;
	(*pOut).d = - FmVec3Dot(pPoint,pNormal);
	return pOut;
}


/*!***************************************************************************
@Function			FmPlaneFromPoints
@Input				pOut   FmPlane
@Input				pv1    Pointer to the source FmVec3 structure
@Input				pv2    Pointer to the source FmVec3 structure
@Input				pv3    Pointer to the source FmVec3 structure
@Returns			constructed from the given points
@Description		Constructs a plane from three points
****************************************************************************/
FmPlane* FmPlaneFromPoints(FmPlane* pOut,const FmVec3* pv1,const FmVec3* pv2,const FmVec3* pv3)
{
	Assert( (pOut != NULL) && (pv1 != NULL) && (pv2 != NULL) && (pv3 != NULL) );

	FmVec3 edge1;
	FmVec3 edge2;

	FmVec3Subtract(&edge1,pv2,pv1);
	FmVec3Subtract(&edge2,pv3,pv1);

	FmVec3 normal,Nnormal;
	FmVec3Cross(&normal,&edge1,&edge2);
	FmVec3Normalize(&Nnormal,&normal);
 
	FmPlaneFromPointNormal(pOut,pv1,&Nnormal);
	return pOut;
}

/*!***************************************************************************
@Function			FmPlaneIntersectLine
@Input				pOut   Pointer to the source FmVec3 structure
@Input				pPlane    Pointer to the source FmPlane structure
@Input				pv1    Pointer to the source FmVec3 structure
@Input				pv2    Pointer to the source FmVec3 structure
@Returns			Finds the intersection between a plane and a line
@Description		If the line is parallel to the plane, NULL is returned.
The return value for this function is the same value returned in the pOut parameter. 
In this way, the D3DXPlaneIntersectLine function can be used as a parameter for another function.
****************************************************************************/
FmVec3* FmPlaneIntersectLine(FmVec3* pOut,const FmPlane* pPlane,const FmVec3* pv1,const FmVec3* pv2)
{
	Assert( (pOut != NULL) && (pPlane != NULL) && (pv1 != NULL) && (pv2 != NULL) );
	FmVec3 direction,normal;
	VERTTYPE dot,temp;

	normal.x = (*pPlane).a;
	normal.y = (*pPlane).b;
	normal.z = (*pPlane).c;

	direction.x = (*pv2).x - (*pv1).x;
	direction.y = (*pv2).y - (*pv1).y;
	direction.z = (*pv2).z - (*pv1).z;
   
	dot = FmVec3Dot(&normal,&direction);

	if( !dot ) return NULL;

	temp = ( (*pPlane).d + FmVec3Dot(&normal,pv1) ) / dot;
	(*pOut).x = (*pv1).x - temp * direction.x;
	(*pOut).y = (*pv1).y - temp * direction.y;
	(*pOut).z = (*pv1).z - temp * direction.z;

	return pOut;
}

/*!***************************************************************************
@Function			FmPlaneDotCoord
@Input		        pPlane Pointer to a source FmPlane structure.
@Input              pV     Pointer to a source FmVec3 structure.
@Returns            The dot product of the plane and 3D vector
Given a plane (a, b, c, d) and a 3D vector (x, y, z) the return value of this function is a*x + b*y + c*z + d*1. 
The FmPlaneDotCoord function is useful for determining the plane's relationship with a coordinate in 3D space
****************************************************************************/
VERTTYPE FmPlaneDotCoord(const FmPlane* pPlane,const FmVec3* pV)
{
	Assert( (pPlane != NULL) && (pV != NULL) );
	return (*pPlane).a * (*pV).x + (*pPlane).b * (*pV).y + (*pPlane).c * (*pV).z + (*pPlane).d;
}


/*!***************************************************************************
@Function			normalize
@Input				pOut   FmQuat
@Input				pIn    Pointer to the source FmQuat structure
@Returns			the normal of the quaternion
@Description		Computes a unit length quaternion
****************************************************************************/
FmQuat* FmQuatNormalize(FmQuat* pOut,const FmQuat* pIn)
{
	Assert( (pOut != NULL) && (pIn != NULL) );
    
	VERTTYPE norm = FmQuatLength(pIn);

	FmQuat q;
    
	q.x = VERTTYPEDIV( pIn->x , norm );
	q.y = VERTTYPEDIV( pIn->y , norm );
	q.z = VERTTYPEDIV( pIn->z , norm );
	q.w = VERTTYPEDIV( pIn->w , norm );

	*pOut = q;

	return pOut;
}

/*!***************************************************************************
@Function			FmQuatLength
@Input				pOut   FmQuat
@Input				pIn    Pointer to the source FmQuat structure
@Returns			Returns the length of a quaternion
@Description		Use FmQuatNormalize for any quaternion input that is not already normalized.
****************************************************************************/
VERTTYPE FmQuatLength(const FmQuat* pIn)
{
	Assert( pIn != NULL ); 
	return (VERTTYPE) f2vt(sqrt(vt2f((*pIn).x)*vt2f((*pIn).x) + vt2f((*pIn).y)*vt2f((*pIn).y) + vt2f((*pIn).z)*vt2f((*pIn).z)+ vt2f((*pIn).w)*vt2f((*pIn).w) ));
}

/*!***************************************************************************
@Function			FmQuatIdentity
@Input				pOut   FmQuat
@Returns			Returns the identity quaternion
@Description		Given a quaternion (x, y, z, w), the FmQuatIdentity function will return the quaternion (0, 0, 0, 1).
The return value for this function is the same value returned in the pOut parameter.
In this way, the FmQuatIdentity function can be used as a parameter for another function.
Use FmQuatNormalize for any quaternion input that is not already normalized
****************************************************************************/
FmQuat* FmQuatIdentity(FmQuat* pOut)
{
	Assert(pOut != NULL);
	FmMatrixQuaternionIdentity(pOut);
	return pOut;
}

VERTTYPE FmQuatDot(const FmQuat* pQ1,const FmQuat* pQ2)
{
	Assert( (pQ1 != NULL ) && (pQ2 != NULL ) );
    if ( !pQ1 || !pQ2 ) return 0.0f;
    return (pQ1->x) * (pQ2->x) + (pQ1->y) * (pQ2->y) + (pQ1->z) * (pQ2->z) + (pQ1->w) * (pQ2->w);
}


/*!***************************************************************************
@Function			FmQuatRotationMat4
@Input				pOut FmQuat
@Input				pIn  matrix
@Returns			FmQuat
@Description		Builds a quaternion from a rotation matrix.
****************************************************************************/
FmQuat* FmQuatRotationMat4(FmQuat* pOut,const FmMat4* pIn)
{
	Assert( (pOut != NULL ) && (pIn != NULL ) );

	VERTTYPE trace = (*pIn)(0,0) + (*pIn)(1,1) + (*pIn)(2,2) + f2vt(1.0f);

	if( trace > 1.0f )
	{
		float fsqrt_trace = sqrt(trace);
		float frecp_sqrt_trace = 1.0F / fsqrt_trace * 0.5F;
        pOut->x = (pIn->m[1][2] - pIn->m[2][1]) * frecp_sqrt_trace;
		pOut->y = (pIn->m[2][0] - pIn->m[0][2]) * frecp_sqrt_trace;
        pOut->z = (pIn->m[0][1] - pIn->m[1][0]) * frecp_sqrt_trace;
		pOut->w = fsqrt_trace * 0.5F;
		return pOut;
	}

	int maxi = 0;
	float maxdiag = pIn->m[0][0];

	if (pIn->m[1][1] > maxdiag)
	{
		maxi = 1;
		maxdiag = pIn->m[1][1];
	}

	if (pIn->m[2][2] > maxdiag)
	{
		maxi = 2;
		maxdiag = pIn->m[2][2];
	}

	//for (int i = 1; i < 3; i++)
	//{
	//	if (pIn->m[i][i] > maxdiag)
	//	{
	//		maxi = i;
	//		maxdiag = pIn->m[i][i];
	//	}
	//}

	float S;

	switch (maxi)
	{
	case 0:
		S = 2.0F * sqrt(1.0f + pIn->m[0][0] - pIn->m[1][1] - pIn->m[2][2]);
		pOut->x = 0.25F * S;
		pOut->y = (pIn->m[0][1] + pIn->m[1][0]) / S;
		pOut->z = (pIn->m[0][2] + pIn->m[2][0]) / S;
		pOut->w = (pIn->m[1][2] - pIn->m[2][1]) / S;
		break;
	case 1:
		S = 2.0F * sqrt(1.0f + pIn->m[1][1] - pIn->m[0][0] - pIn->m[2][2]);
		pOut->x = (pIn->m[0][1] + pIn->m[1][0]) / S;
		pOut->y = 0.25F * S;
		pOut->z = (pIn->m[1][2] + pIn->m[2][1]) / S;
		pOut->w = (pIn->m[2][0] - pIn->m[0][2]) / S;
		break;
	case 2:
		S = 2.0F * sqrt(1.0f + pIn->m[2][2] - pIn->m[0][0] - pIn->m[1][1]);
		pOut->x = (pIn->m[0][2] + pIn->m[2][0]) / S;
		pOut->y = (pIn->m[1][2] + pIn->m[2][1]) / S;
		pOut->z = 0.25F * S;
		pOut->w = (pIn->m[0][1] - pIn->m[1][0]) / S;
		break;
	}

	return pOut;
}

/*!***************************************************************************
@Function			FmQuatMultiply
@Input				pOut FmQuat
@Input				pA   FmQuat
@Input				pB   FmQuat
@Returns			FmQuat
@Description		Multiplies two quaternions.
****************************************************************************/
FmQuat* FmQuatMultiply(FmQuat* pOut,const FmQuat* pA,const FmQuat* pB)
{
	Assert( (pOut != NULL) && (pA != NULL ) && (pB != NULL) );
	FmMatrixQuaternionMultiply(pOut,pA,pB);
	return pOut;
}

/*!***************************************************************************
@Function			FmQuatToAxisAngle
@Input				pqIn		Quaternion to transform
@Output			vAxis	Axis of rotation
@Output			fAngle	Angle of rotation
@Description		Convert a quaternion to an axis and angle. Expects a unit
quaternion.
*****************************************************************************/
void FmQuatToAxisAngle(const FmQuat *pqIn,FmVec3 *pvAxis,VERTTYPE *pfAngle)
{
	Assert( (pqIn != NULL) && (pvAxis != NULL) && (pfAngle != NULL) );
	FmMatrixQuaternionToAxisAngle(pqIn,pvAxis,pfAngle);
}


/*!***************************************************************************
@Function			FmQuatSlerp
@Input				pOut FmQuat
@Input				pA   FmQuat
@Input				pB   FmQuat
@Input				t    Parameter that indicates how far to interpolate between the quaternions
@Returns			FmQuat
@Description		Interpolates between two quaternions, using spherical linear interpolation.
****************************************************************************/
FmQuat* FmQuatSlerp(FmQuat* pout,const FmQuat* pq1,const FmQuat* pq2,VERTTYPE t)
{
	Assert( (pout != NULL) && (pq1 != NULL ) && (pq2 != NULL) );
    float dot, epsilon, temp, theta, u;

    epsilon = 1.0f;
    temp = 1.0f - t;
    u = t;
    dot = FmQuatDot(pq1, pq2);
    if ( dot < 0.0f )
    {
        epsilon = -1.0f;
        dot = -dot;
    }
    if( 1.0f - dot > 0.001f )
    {
        theta = acos(dot);
        temp  = sin(theta * temp) / sin(theta);
        u = sin(theta * u) / sin(theta);
    }
    pout->x = temp * pq1->x + epsilon * u * pq2->x;
    pout->y = temp * pq1->y + epsilon * u * pq2->y;
    pout->z = temp * pq1->z + epsilon * u * pq2->z;
    pout->w = temp * pq1->w + epsilon * u * pq2->w;
	return pout;
}

/*!***************************************************************************
@Function			FmQuatRotationYawPitchRoll
@Input				pOut   FmQuat
@Input				yaw    Yaw around the y-axis, in radians
@Input				pitch  Pitch around the x-axis, in radians
@Input				roll   Roll around the z-axis, in radians
@Returns			FmQuat
@Description		Builds a quaternion from a rotation matrix.
****************************************************************************/
FmQuat* FmQuatRotationYawPitchRoll(FmQuat* pOut,VERTTYPE yaw,VERTTYPE pitch,VERTTYPE roll)
{
	Assert( pOut != NULL );

	(*pOut).x = FMFSIN( VERTTYPEDIV( yaw,f2vt(2.0f) ) ) * FMFCOS( VERTTYPEDIV( pitch , f2vt(2.0f)) ) * FMFSIN( VERTTYPEDIV( roll , f2vt(2.0f)) ) 
		      + FMFCOS( VERTTYPEDIV( yaw,f2vt(2.0f) ) ) * FMFSIN( VERTTYPEDIV( pitch , f2vt(2.0f)) ) * FMFCOS( VERTTYPEDIV( roll , f2vt(2.0f)) );

	(*pOut).y = FMFSIN( VERTTYPEDIV( yaw,f2vt(2.0f) ) ) * FMFCOS( VERTTYPEDIV( pitch , f2vt(2.0f)) ) * FMFCOS( VERTTYPEDIV( roll , f2vt(2.0f)) ) 
		      - FMFCOS( VERTTYPEDIV( yaw,f2vt(2.0f) ) ) * FMFSIN( VERTTYPEDIV( pitch , f2vt(2.0f)) ) * FMFSIN( VERTTYPEDIV( roll , f2vt(2.0f)) );

	(*pOut).z = FMFCOS( VERTTYPEDIV( yaw,f2vt(2.0f) ) ) * FMFCOS( VERTTYPEDIV( pitch , f2vt(2.0f)) ) * FMFSIN( VERTTYPEDIV( roll , f2vt(2.0f)) ) 
		      - FMFSIN( VERTTYPEDIV( yaw,f2vt(2.0f) ) ) * FMFSIN( VERTTYPEDIV( pitch , f2vt(2.0f)) ) * FMFCOS( VERTTYPEDIV( roll , f2vt(2.0f)) );

	(*pOut).w = FMFCOS( VERTTYPEDIV( yaw,f2vt(2.0f) ) ) * FMFCOS( VERTTYPEDIV( pitch , f2vt(2.0f)) ) * FMFCOS( VERTTYPEDIV( roll , f2vt(2.0f)) ) 
		      + FMFSIN( VERTTYPEDIV( yaw,f2vt(2.0f) ) ) * FMFSIN( VERTTYPEDIV( pitch , f2vt(2.0f)) ) * FMFSIN( VERTTYPEDIV( roll , f2vt(2.0f)) );

	return pOut;
}

/*!***************************************************************************
@Function			FmQuatRotationToYawPitchRoll
@Input				pIn   FmQuat
@Input				yaw    Yaw around the y-axis, in radians
@Input				pitch  Pitch around the x-axis, in radians
@Input				roll   Roll around the z-axis, in radians
@Returns			 
@Description		Builds a quaternion from a rotation matrix.
****************************************************************************/
void FmQuatRotationToYawPitchRoll(FmQuat* pIn,VERTTYPE* yaw,VERTTYPE* pitch,VERTTYPE* roll)
{
    VERTTYPE sp = -2.0f * ( (*pIn).y * (*pIn).z - (*pIn).w * (*pIn).x );

	if( (sp > 0.9999F) || (sp < -0.9999F) )
	{
		*pitch = FM_PI * 0.5f * sp;
		*yaw = FMATAN2(-(*pIn).x * (*pIn).z + (*pIn).w * (*pIn).y, 0.5f - (*pIn).y * (*pIn).y - (*pIn).z *(*pIn).z );
		*roll = 0.0f;
	}
	else
	{
		*pitch = FMASIN(sp);
		*yaw = FMATAN2( (*pIn).x * (*pIn).z + (*pIn).w * (*pIn).y, 0.5f - (*pIn).x * (*pIn).x - (*pIn).y *(*pIn).y );
		*roll = FMATAN2( (*pIn).x * (*pIn).y + (*pIn).w * (*pIn).z, 0.5f - (*pIn).x * (*pIn).x - (*pIn).z *(*pIn).z );
	}
}

/*!***************************************************************************
@Function			FmQuatRotationAxis
@Input				pOut FmQuat
@Input				pv identifies the axis about which to rotate the quaternion.
@Input				angle Angle of rotation, in radians. Angles are measured clockwise when looking along the rotation axis toward the origin
@Returns			result of addition
@Description		componentwise addition operator for two Vec2s
****************************************************************************/
FmQuat* FmQuatRotationAxis(FmQuat* pOut,const FmVec3* pv,VERTTYPE angle)
{
	Assert( (pOut != NULL) && (pv != NULL) );
	FmMatrixQuaternionRotationAxis(pOut,pv,angle);
	return pOut;
}

/*!***************************************************************************
@Function			+
@Input				rhs another Vec2
@Returns			result of addition
@Description		componentwise addition operator for two Vec2s
****************************************************************************/
FmVec2* FmVec2Add(FmVec2* pOut,const FmVec2* pv1,const FmVec2* pv2)
{
	Assert( (pOut != NULL) && (pv1 != NULL) && (pv2 != NULL) );
	(*pOut).x = (*pv1).x + (*pv2).x;
	(*pOut).y = (*pv1).y + (*pv2).y;
	return pOut;
}

/*!***************************************************************************
@Function			dot
@Input				rhs a single value
@Returns			scalar product
@Description		calculate the scalar product of two Vec3s
****************************************************************************/
VERTTYPE FmVec2Dot(const FmVec2* pv1,const FmVec2* pv2)
{
	Assert( (pv1 != NULL) && (pv2 != NULL ) );
	return VERTTYPEMUL((*pv1).x, (*pv2).x) + VERTTYPEMUL((*pv1).y, (*pv2).y);
}

/*!***************************************************************************
@Function			length
@Returns			the of the magnitude of the vector
@Description		calculates the magnitude of the vector
****************************************************************************/
VERTTYPE FmVec2Length(const FmVec2* pv)
{
	Assert( pv != NULL );
	return (VERTTYPE) f2vt(sqrt(vt2f((*pv).x)*vt2f((*pv).x) + vt2f((*pv).y)*vt2f((*pv).y)));
}

/*!***************************************************************************
@Function			lenSqr
@Returns			the square of the magnitude of the vector
@Description		calculates the square of the magnitude of the vector
****************************************************************************/
VERTTYPE FmVec2LengthSq(const FmVec2* pv)
{
	Assert( pv != NULL );
	return VERTTYPEMUL((*pv).x,(*pv).x)+VERTTYPEMUL((*pv).y,(*pv).y);
}

/*!***************************************************************************
@Function			normalize
@Returns			the normalized value of the vector
@Description		normalizes the vector
****************************************************************************/
FmVec2* FmVec2Normalize(FmVec2* pOut,const FmVec2* pv)
{
	Assert( (pOut != NULL ) && ( pv != NULL ) );
	VERTTYPE len = FmVec2Length(pv);
	if(len <= FM_SMALL_NUMBER)
	{
		(*pOut).x = 0.0;
		(*pOut).y = 0.0;
		return pOut;
	}
	(*pOut).x =VERTTYPEDIV((*pv).x,len);
	(*pOut).y =VERTTYPEDIV((*pv).y,len);
	return pOut;
}

/*!***************************************************************************
@Function			-
@Input				rhs another Vec2
@Returns			result of negation
@Description		negation operator for a Vec2
****************************************************************************/
FmVec2* FmVec2Subtract(FmVec2* pOut,const FmVec2* pv1,const FmVec2* pv2)
{
	Assert( (pOut != NULL) && (pv1 != NULL) && (pv2 != NULL) );
	(*pOut).x = (*pv1).x - (*pv2).x;
	(*pOut).y = (*pv1).y - (*pv2).y;
	return pOut;
}

/*!***************************************************************************
@Function			*
@Input				lhs scalar
@Input				rhs a Vec2
@Returns			result of multiplication
@Description		multiplication operator for a Vec2
****************************************************************************/
FmVec2* FmVec2Scale(FmVec2* pOut,const FmVec2* pv,VERTTYPE s)
{
	Assert( (pOut != NULL) && (pv != NULL) );
	(*pOut).x = (*pv).x * s;
	(*pOut).y = (*pv).y * s;
	return pOut;
}

/*!***************************************************************************
@Function			lerp
@Input				lhs scalar
@Input				rhs a Vec2
@Returns			result of multiplication
@Description		Performs a linear interpolation between two 2D vectors.
****************************************************************************/
FmVec2* FmVec2Lerp(FmVec2* pOut,const FmVec2* pv1,const FmVec2* pv2,VERTTYPE s)
{
	Assert( (pOut != NULL) && (pv1 != NULL) && (pv2 != NULL) );
	(*pOut).x = (*pv1).x + s * ( (*pv1).x - (*pv2).x);
	(*pOut).y = (*pv1).y + s * ( (*pv1).y - (*pv2).y);
	return pOut;
}

/*!***************************************************************************
@Function			max
@Input				rhs another Vec2
@Returns			result of negation
@Description		Returns a 2D vector that is made up of the largest components of two 2D vectors.
****************************************************************************/
FmVec2* FmVec2Maximize(FmVec2* pOut,const FmVec2* pv1,const FmVec2* pv2)
{
	Assert( (pOut != NULL ) && (pv1 != NULL ) && (pv2 != NULL) );
	(*pOut).x = (*pv1).x > (*pv2).x ? (*pv1).x : (*pv2).x;
	(*pOut).y = (*pv1).y > (*pv2).y ? (*pv1).y : (*pv2).y;
	return pOut;
}


/*!***************************************************************************
@Function			min
@Input				rhs another Vec2
@Returns			result of negation
@Description		Returns a 2D vector that is made up of the smallest components of two 2D vectors.
****************************************************************************/
FmVec2* FmVec2Minimize(FmVec2* pOut,const FmVec2* pv1,const FmVec2* pv2)
{
	Assert( (pOut != NULL ) && (pv1 != NULL ) && (pv2 != NULL) );
	(*pOut).x = (*pv1).x < (*pv2).x ? (*pv1).x : (*pv2).x;
	(*pOut).y = (*pv1).y < (*pv2).y ? (*pv1).y : (*pv2).y;
	return pOut;
}


/*!***************************************************************************
@Function			min
@Input				rhs another Vec2
@Returns			result of negation
@Description		Transforms a 2D vector by a given matrix.
****************************************************************************/
FmVec4* FmVec2Transform(FmVec4* pOut,const FmVec2* pv,const FmMat4* pm)
{
	Assert( (pOut != NULL) && (pv != NULL) && (pm != NULL));

	(*pOut).x = (*pm)(0,0) * (*pv).x + (*pm)(0,1) * (*pv).y  + (*pm)(0,3);
	(*pOut).y = (*pm)(1,0) * (*pv).x + (*pm)(1,1) * (*pv).y  + (*pm)(1,3);
	(*pOut).z = (*pm)(2,0) * (*pv).x + (*pm)(2,1) * (*pv).y  + (*pm)(2,3);
	(*pOut).w = (*pm)(3,0) * (*pv).x + (*pm)(3,1) * (*pv).y  + (*pm)(3,3);

	return pOut;
}


/*!***************************************************************************
@Function			TransformCoord
@Input				rhs another Vec2
@Input				rhs another FmMat4
@Returns			result of negation
@Description		Transforms a 2D vector by a given matrix, projecting the result back into w = 1.
****************************************************************************/
FmVec2* FmVec2TransformCoord(FmVec2* pOut,const FmVec2* pv,const FmMat4* pm)
{
	Assert( (pOut != NULL) && (pv != NULL) && (pm != NULL) );
	FmVec2 v;
    VERTTYPE norm;
	v =*pv;

	norm = (*pm)(3,0) * (*pv).x + (*pm)(3,1) * (*pv).y + (*pm)(3,3);

	(*pOut).x = (*pm)(0,0) * v.x + (*pm)(0,1) * v.y + (*pm)(0,3) / norm;
	(*pOut).y = (*pm)(1,0) * v.x + (*pm)(1,1) * v.y + (*pm)(1,3) / norm;
	return pOut;
}

/*!***************************************************************************
@Function			TransformNormal
@Input				rhs another Vec2
@Input				rhs another FmMat4
@Returns			result of negation
@Description		Transforms the 2D vector normal by the given matrix.
****************************************************************************/
FmVec2* FmVec2TransformNormal(FmVec2* pOut,const FmVec2* pv,const FmMat4* pm)
{
	Assert( (pOut != NULL) && (pv != NULL) && (pm != NULL) );
	const FmVec2 v = *pv;
	(*pOut).x = (*pm)(0,0) * v.x + (*pm)(0,1) * v.y;
	(*pOut).y = (*pm)(1,0) * v.x + (*pm)(1,1) * v.y;
	return pOut;
}


/*!***************************************************************************
@Function			+
@Input				rhs another Vec3
@Returns			result of addition
@Description		componentwise addition operator for two Vec3s
****************************************************************************/
FmVec3* FmVec3Add(FmVec3* pOut,const FmVec3* pv1,const FmVec3* pv2)
{
	Assert( (pOut != NULL) && (pv1 != NULL) && (pv2 != NULL) );

	(*pOut).x = (*pv1).x + (*pv2).x;
	(*pOut).y = (*pv1).y + (*pv2).y;
	(*pOut).z = (*pv1).z + (*pv2).z;
	return pOut;
}


/*!***************************************************************************
@Function			dot
@Input				rhs a single value
@Returns			scalar product
@Description		calculate the scalar product of two Vec3s
****************************************************************************/
VERTTYPE FmVec3Dot(const FmVec3* pv1,const FmVec3* pv2)
{
	Assert( (pv1 != NULL) && (pv2 != NULL ) );
	return VERTTYPEMUL((*pv1).x, (*pv2).x) + VERTTYPEMUL((*pv1).y, (*pv2).y) + VERTTYPEMUL((*pv1).z, (*pv2).z);
}

/*!***************************************************************************
@Function			length
@Returns			the of the magnitude of the vector
@Description		calculates the magnitude of the vector
****************************************************************************/
VERTTYPE FmVec3Length(const FmVec3* pv)
{
	Assert( pv != NULL );
	return (VERTTYPE) f2vt(sqrt(vt2f((*pv).x)*vt2f((*pv).x) + vt2f((*pv).y)*vt2f((*pv).y) + vt2f((*pv).z)*vt2f((*pv).z) ));
}

/*!***************************************************************************
@Function			lenSqr
@Returns			the square of the magnitude of the vector
@Description		calculates the square of the magnitude of the vector
****************************************************************************/
VERTTYPE FmVec3LengthSq(const FmVec3* pv)
{
	Assert( pv != NULL );
	return VERTTYPEMUL((*pv).x,(*pv).x)+VERTTYPEMUL((*pv).y,(*pv).y)+VERTTYPEMUL((*pv).z,(*pv).z);
}


/*!***************************************************************************
@Function			normalize
@Returns			the normalized value of the vector
@Description		normalizes the vector
****************************************************************************/
FmVec3* FmVec3Normalize(FmVec3* pOut,const FmVec3* pv)
{
	Assert( (pOut != NULL ) && ( pv != NULL ) );
	FmVec3 v;
	VERTTYPE len = FmVec3Length(pv);
	if(len <= FM_SMALL_NUMBER)
	{
		v.x = 0.0;
		v.y = 0.0;
		v.z = 0.0;
		*pOut = v;
		return pOut;
	}


	v.x =VERTTYPEDIV((*pv).x,len);
	v.y =VERTTYPEDIV((*pv).y,len);
	v.z =VERTTYPEDIV((*pv).z,len);
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			-
@Input				rhs another Vec2
@Returns			result of negation
@Description		negation operator for a Vec3
****************************************************************************/
FmVec3* FmVec3Subtract(FmVec3* pOut,const FmVec3* pv1,const FmVec3* pv2)
{
	Assert( (pOut != NULL ) && (pv1 != NULL) && (pv2 != NULL) );
	FmVec3 v;
	v.x = (*pv1).x - (*pv2).x;
	v.y = (*pv1).y - (*pv2).y;
	v.z = (*pv1).z - (*pv2).z;
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			*
@Input				lhs scalar
@Input				rhs a Vec3
@Returns			result of multiplication
@Description		multiplication operator for a Vec3
****************************************************************************/
FmVec3* FmVec3Scale(FmVec3* pOut,const FmVec3* pv,VERTTYPE s)
{
	Assert( (pOut != NULL) && (pv != NULL) );
	FmVec3 v;
	v.x = (*pv).x * s;
	v.y = (*pv).y * s;
	v.z = (*pv).z * s;
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			lerp
@Input				lhs scalar
@Input				rhs a Vec3
@Returns			result of multiplication
@Description		Performs a linear interpolation between two 3D vectors.
****************************************************************************/
FmVec3* FmVec3Lerp(FmVec3* pOut,const FmVec3* pv1,const FmVec3* pv2,VERTTYPE s)
{
	Assert( (pOut != NULL) && (pv1 != NULL) && (pv2 != NULL) );
    FmVec3 v;
	v.x = (*pv1).x + s * ( (*pv2).x - (*pv1).x);
	v.y = (*pv1).y + s * ( (*pv2).y - (*pv1).y);
	v.z = (*pv1).z + s * ( (*pv2).z - (*pv1).z);
	*pOut = v;
	return pOut;
}


/*!***************************************************************************
@Function			max
@Input				rhs another Vec3
@Returns			result of negation
@Description		Returns a 3D vector that is made up of the largest components of two 3D vectors.
****************************************************************************/
FmVec3* FmVec3Maximize(FmVec3* pOut,const FmVec3* pv1,const FmVec3* pv2)
{
	Assert( (pOut != NULL ) && (pv1 != NULL ) && (pv2 != NULL) );
    FmVec3 v;
	v.x = (*pv1).x > (*pv2).x ? (*pv1).x : (*pv2).x;
	v.y = (*pv1).y > (*pv2).y ? (*pv1).y : (*pv2).y;
	v.z = (*pv1).z > (*pv2).z ? (*pv1).z : (*pv2).z;
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			min
@Input				rhs another Vec3
@Returns			result of negation
@Description		Returns a 3D vector that is made up of the smallest components of two 3D vectors.
****************************************************************************/
FmVec3* FmVec3Minimize(FmVec3* pOut,const FmVec3* pv1,const FmVec3* pv2)
{
	Assert( (pOut != NULL ) && (pv1 != NULL ) && (pv2 != NULL) );
	FmVec3 v;
	v.x = (*pv1).x < (*pv2).x ? (*pv1).x : (*pv2).x;
	v.y = (*pv1).y < (*pv2).y ? (*pv1).y : (*pv2).y;
	v.z = (*pv1).z < (*pv2).z ? (*pv1).z : (*pv2).z;
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			Transform
@Input				rhs another Vec3
@Input				rhs another FmMat4
@Returns			result of negation
@Description		Transforms a 3D vector by a given matrix.
****************************************************************************/
FmVec4* FmVec3Transform(FmVec4* pOut,const FmVec3* pv,const FmMat4* pm)
{
	Assert( (pOut != NULL ) && (pv != NULL ) && (pm != NULL ) );
    FmVec4 v;
	v.x = (*pm)(0,0) * (*pv).x + (*pm)(0,1) * (*pv).y  + (*pm)(0,2) * (*pv).z  + (*pm)(0,3);
	v.y = (*pm)(1,0) * (*pv).x + (*pm)(1,1) * (*pv).y  + (*pm)(1,2) * (*pv).z  + (*pm)(1,3);
	v.z = (*pm)(2,0) * (*pv).x + (*pm)(2,1) * (*pv).y  + (*pm)(2,2) * (*pv).z  + (*pm)(2,3);
	v.w = (*pm)(3,0) * (*pv).x + (*pm)(3,1) * (*pv).y  + (*pm)(3,2) * (*pv).z  + (*pm)(3,3);

	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			TransformCoord
@Input				rhs another Vec3
@Input				rhs another FmMat4
@Returns			result of negation
@Description		Transforms a 3D vector by a given matrix, projecting the result back into w = 1.
****************************************************************************/
FmVec3* FmVec3TransformCoord(FmVec3* pOut,const FmVec3* pv,const FmMat4* pm)
{
	Assert( (pOut != NULL) && (pv != NULL) && (pm != NULL) );
	FmVec3 out;

	VERTTYPE norm;
	norm = (*pm)(3,0)*(*pv).x + (*pm)(3,1)*(*pv).y + (*pm)(3,2)*(*pv).z + (*pm)(3,3);

	if(norm)
	{
		out.x = ((*pm)(0,0)*(*pv).x + (*pm)(0,1)*(*pv).y + (*pm)(0,2)*(*pv).z + (*pm)(0,3))/norm;
		out.y = ((*pm)(1,0)*(*pv).x + (*pm)(1,1)*(*pv).y + (*pm)(1,2)*(*pv).z + (*pm)(1,3))/norm;
		out.z = ((*pm)(2,0)*(*pv).x + (*pm)(2,1)*(*pv).y + (*pm)(2,2)*(*pv).z + (*pm)(2,3))/norm;
	}
	else
	{
		out.x = 0.0;
		out.y = 0.0;
		out.z = 0.0;
	}

	*pOut = out;
	return pOut;
}

/*!***************************************************************************
@Function			TransformCoordStream
@Input              rhs another int
@Input				rhs another Vec3
@Input              rhs another int
@Input              rhs another int
@Input				rhs another FmMat4
@Returns			result of negation
@Description		Transforms a 3D vector by a given matrix, projecting the result back into w = 1.
****************************************************************************/
FmVec3* FmVec3TransformCoordStream(FmVec3* pOut, int nOutputStride, const FmVec3* pIn, int nInputStride, int loopCount, const FmMat4* pm)
{
	for (int i = 0; i < loopCount; i++)
	{
		FmVec3TransformCoord(
			(FmVec3*)((char*)pOut + nOutputStride * i),
			(const FmVec3*)((const char*)pIn + nInputStride * i),
			pm);
	}

	return pOut;
}

/*!***************************************************************************
@Function			TransformNormal
@Input				rhs another Vec3
@Input				rhs another FmMat4
@Returns			result of negation
@Description		Transforms the 3D vector normal by the given matrix.
****************************************************************************/
FmVec3* FmVec3TransformNormal(FmVec3* pOut,const FmVec3* pv,const FmMat4* pm)
{
	Assert( (pOut != NULL) && (pv != NULL) && (pm != NULL) );
	const FmVec3 v = *pv;
	(*pOut).x = (*pm)(0,0)*v.x + (*pm)(0,1)*v.y + (*pm)(0,2)*v.z;
	(*pOut).y = (*pm)(1,0)*v.x + (*pm)(1,1)*v.y + (*pm)(1,2)*v.z;
	(*pOut).z = (*pm)(2,0)*v.x + (*pm)(2,1)*v.y + (*pm)(2,2)*v.z;

	return pOut;
}

/*!***************************************************************************
@Function			TransformNormal
@Input				rhs another Vec3
@Input				rhs another Vec3
@Returns			result of negation
@Description		Determines the cross-product of two 3D vectors.
****************************************************************************/
FmVec3* FmVec3Cross(FmVec3* pOut,const FmVec3* pv1,const FmVec3* pv2)
{
	Assert( (pOut != NULL) && (pv1 != NULL) && (pv2 != NULL ));
	FmVec3 v;
	v.x = (*pv1).y * (*pv2).z - (*pv1).z * (*pv2).y;
	v.y = (*pv1).z * (*pv2).x - (*pv1).x * (*pv2).z;
	v.z = (*pv1).x * (*pv2).y - (*pv1).y * (*pv2).x;
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			+
@Input				rhs another Vec4
@Returns			result of addition
@Description		componentwise addition operator for two Vec4s
****************************************************************************/
FmVec4* FmVec4Add(FmVec4* pOut,const FmVec4* pv1,const FmVec4* pv2)
{
	Assert( (pOut != NULL) && (pv1 != NULL) && (pv2 != NULL) );
	FmVec4 v;
	v.x = (*pv1).x + (*pv2).x;
	v.y = (*pv1).y + (*pv2).y;
	v.z = (*pv1).z + (*pv2).z;
	v.w = (*pv1).w + (*pv2).w;
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			-
@Input				rhs another Vec4
@Returns			result of negation
@Description		negation operator for a Vec4
****************************************************************************/
FmVec4* FmVec4Subtract(FmVec4* pOut,const FmVec4* pv1,const FmVec4* pv2)
{
	Assert( (pOut != NULL) && (pv1 != NULL) && (pv2 != NULL) );
	FmVec4 v;
	v.x = (*pv1).x - (*pv2).x;
	v.y = (*pv1).y - (*pv2).y;
	v.z = (*pv1).z - (*pv2).z;
	v.w = (*pv1).w - (*pv2).w;
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			Vec4Cross
@Input				rhs another Vec4
@Input				rhs another Vec4
@Returns			result of negation
@Description		Determines the cross-product of two 4D vectors.
****************************************************************************/
FmVec4* FmVec4Cross(FmVec4* pOut,const FmVec4* pv1,const FmVec4* pv2,const FmVec4* pv3)
{
	Assert( (pOut != NULL) && (pv1 != NULL) && (pv2 != NULL) &&(pv3 != NULL));
	FmVec4 v;
    v.x =       (*pv1).y*((*pv2).z*(*pv3).w - (*pv3).z*(*pv2).w) 
              - (*pv1).z*((*pv2).y*(*pv3).w - (*pv3).y*(*pv2).w)
			  + (*pv1).w*((*pv2).y*(*pv3).z - (*pv2).z*(*pv3).y);

	v.y =       -((*pv1).x*((*pv2).z*(*pv3).w - (*pv3).z*(*pv2).w)
		        - (*pv1).z*((*pv2).x*(*pv3).w - (*pv3).x*(*pv2).w)
				+ (*pv1).w*((*pv2).x*(*pv3).z - (*pv3).x*(*pv2).z));

	v.z =       (*pv1).x*((*pv2).y*(*pv3).w - (*pv3).y*(*pv2).w)
		       -(*pv1).y*((*pv2).x*(*pv3).w - (*pv3).x*(*pv2).w)
			   +(*pv1).w*((*pv2).x*(*pv3).y - (*pv3).x*(*pv2).y);

	v.w =       -((*pv1).x*((*pv2).y*(*pv3).z - (*pv3).y*(*pv2).z)
		        - (*pv1).y*((*pv2).x*(*pv3).z - (*pv3).x*(*pv2).z)
                + (*pv1).z*((*pv2).x*(*pv3).y - (*pv3).x*(*pv2).y));
	*pOut = v;

	return pOut;
}

/*!***************************************************************************
@Function			length
@Returns			the of the magnitude of the vector
@Description		calculates the magnitude of the vector
****************************************************************************/
VERTTYPE FmVec4Length(const FmVec4* pv)
{
	Assert( pv != NULL );
	return (VERTTYPE) f2vt(sqrt(vt2f((*pv).x)*vt2f((*pv).x) + vt2f((*pv).y)*vt2f((*pv).y) + vt2f((*pv).z)*vt2f((*pv).z)+ vt2f((*pv).w)*vt2f((*pv).w) ));
}

/*!***************************************************************************
@Function			normalize
@Returns			the normalized value of the vector
@Description		normalizes the vector
****************************************************************************/
FmVec4* FmVec4Normalize(FmVec4* pOut,const FmVec4* pv)
{
	Assert( (pOut != NULL) && (pv != NULL) );

	FmVec4 v;

	VERTTYPE len = FmVec4Length(pv);

	if(len <= FM_SMALL_NUMBER)
	{
		v.x = 0.0;
		v.y = 0.0;
		v.z = 0.0;
		v.w = 0.0;
		*pOut = v;
		return pOut;
	}


	v.x =VERTTYPEDIV((*pv).x,len);
	v.y =VERTTYPEDIV((*pv).y,len);
	v.z =VERTTYPEDIV((*pv).z,len);
	v.w =VERTTYPEDIV((*pv).w,len);
	*pOut = v;
	return pOut;
}


/*!***************************************************************************
@Function			max
@Input				rhs another Vec4
@Returns			result of negation
@Description		Returns a 4D vector that is made up of the largest components of two 4D vectors.
****************************************************************************/
FmVec4* FmVec4Maximize(FmVec4* pOut,const FmVec4* pv1,const FmVec4* pv2)
{
	Assert( (pOut != NULL ) && (pv1 != NULL ) && (pv2 != NULL) );
	FmVec4 v;
	v.x = (*pv1).x > (*pv2).x ? (*pv1).x : (*pv2).x;
	v.y = (*pv1).y > (*pv2).y ? (*pv1).y : (*pv2).y;
	v.z = (*pv1).z > (*pv2).z ? (*pv1).z : (*pv2).z;
	v.w = (*pv1).w > (*pv2).w ? (*pv1).w : (*pv2).w;
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			min
@Input				rhs another Vec4
@Returns			result of negation
@Description		Returns a 4D vector that is made up of the smallest components of two 4D vectors.
****************************************************************************/
FmVec4* FmVec3Minimize(FmVec4* pOut,const FmVec4* pv1,const FmVec4* pv2)
{
	Assert( (pOut != NULL ) && (pv1 != NULL ) && (pv2 != NULL) );
	FmVec4 v;
	v.x = (*pv1).x < (*pv2).x ? (*pv1).x : (*pv2).x;
	v.y = (*pv1).y < (*pv2).y ? (*pv1).y : (*pv2).y;
	v.z = (*pv1).z < (*pv2).z ? (*pv1).z : (*pv2).z;
	v.w = (*pv1).w < (*pv2).w ? (*pv1).w : (*pv2).w;
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			lerp
@Input				lhs scalar
@Input				rhs a Vec4
@Returns			result of multiplication
@Description		Performs a linear interpolation between two 4D vectors.
****************************************************************************/
FmVec4* FmVec4Lerp(FmVec4* pOut,const FmVec4* pv1,const FmVec4* pv2,VERTTYPE s)
{
	Assert( (pOut != NULL) && (pv1 != NULL) && (pv2 != NULL) );
    FmVec4 v;
	v.x = (*pv1).x + s * ( (*pv1).x - (*pv2).x);
	v.y = (*pv1).y + s * ( (*pv1).y - (*pv2).y);
	v.z = (*pv1).z + s * ( (*pv1).z - (*pv2).z);
	v.w = (*pv1).w + s * ( (*pv1).w - (*pv2).w);
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			*
@Input				lhs scalar
@Input				rhs a Vec4
@Returns			result of multiplication
@Description		multiplication operator for a Vec4
****************************************************************************/
FmVec4* FmVec4Scale(FmVec4* pOut,const FmVec4* pv,VERTTYPE s)
{
	Assert( (pOut != NULL) && (pv != NULL) );
    FmVec4 v;
	v.x = (*pv).x * s;
	v.y = (*pv).y * s;
	v.z = (*pv).z * s;
	v.w = (*pv).w * s;
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			Transform
@Input				rhs another Vec4
@Input				rhs another FmMat4
@Returns			result of negation
@Description		Transforms a 3D vector by a given matrix.
****************************************************************************/
FmVec4* FmVec4Transform(FmVec4* pOut,const FmVec4* pv,const FmMat4* pm)
{
	Assert( (pOut != NULL) && (pv != NULL) && (pm != NULL) );
	FmVec4 v;

	v.x = (*pm)(0,0) * (*pv).x + (*pm)(0,1) * (*pv).y + (*pm)(0,2) * (*pv).z + (*pm)(0,3) * (*pv).w;
	v.y = (*pm)(1,0) * (*pv).x + (*pm)(1,1) * (*pv).y + (*pm)(1,2) * (*pv).z + (*pm)(1,3) * (*pv).w;
	v.z = (*pm)(2,0) * (*pv).x + (*pm)(2,1) * (*pv).y + (*pm)(2,2) * (*pv).z + (*pm)(2,3) * (*pv).w;
	v.w = (*pm)(3,0) * (*pv).x + (*pm)(3,1) * (*pv).y + (*pm)(3,2) * (*pv).z + (*pm)(3,3) * (*pv).w;
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			FmMat4Identity
@Input				rhs another FmMat4
@Returns			result of negation
@Description		Creates an identity matrix.
****************************************************************************/
FmMat4* FmMat4Identity(FmMat4* pOut)
{
	Assert( pOut != NULL );
	(*pOut).f[ 0]=1.0f;	(*pOut).f[ 4]=0.0f;	(*pOut).f[ 8]=0.0f;	(*pOut).f[12]=0.0f;
	(*pOut).f[ 1]=0.0f;	(*pOut).f[ 5]=1.0f;	(*pOut).f[ 9]=0.0f;	(*pOut).f[13]=0.0f;
	(*pOut).f[ 2]=0.0f;	(*pOut).f[ 6]=0.0f;	(*pOut).f[10]=1.0f;	(*pOut).f[14]=0.0f;
	(*pOut).f[ 3]=0.0f;	(*pOut).f[ 7]=0.0f;	(*pOut).f[11]=0.0f;	(*pOut).f[15]=1.0f;
	return pOut;
}


/*!***************************************************************************
@Function			FmMat4Multiply
@Output			mOut	Result of mA x mB
@Input				mA		First operand
@Input				mB		Second operand
@Description		Multiply mA by mB and assign the result to mOut
(mOut = p1 * p2). A copy of the result matrix is done in
the function because mOut can be a parameter mA or mB.
*****************************************************************************/
FmMat4* FmMat4Multiply(FmMat4 *pOut,const FmMat4	*pA,const FmMat4 *pB)
{
	Assert((pOut != NULL) && (pA != NULL ) && ( pB != NULL) );
	FmMat4 mRet;
	FmMat4 mA = *pA;
    FmMat4 mB = *pB;

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
	return pOut;
}

/*!***************************************************************************
@Function			FmMat4MultiplyTranspose
@Output			    pOut	Result of transpose(mA x mB)
@Input				pA		First operand
@Input				pB		Second operand
@Description		Multiply mA by mB and transpose it and assign the result to mOut
(mOut = transpose(p1 * p2)). A copy of the result matrix is done in
the function because mOut can be a parameter mA or mB.
*****************************************************************************/
FmMat4* FmMat4MultiplyTranspose(FmMat4 *pOut,const FmMat4 *pA, const FmMat4 *pB)
{
    FmMat4Multiply(pOut, pA, pB);
    FmMat4Transpose(pOut, pOut);
    return pOut;
}

/*!***************************************************************************
@Function Name		FmMat4Translation
@Output			mOut	Translation matrix
@Input				fX		X component of the translation
@Input				fY		Y component of the translation
@Input				fZ		Z component of the translation
@Description		Build a transaltion matrix mOut using fX, fY and fZ.
*****************************************************************************/
FmMat4* FmMat4Translation(FmMat4	*pOut,const float fX,const float fY,const float	fZ)
{
	Assert( pOut != NULL );
	(*pOut).f[ 0]=1.0f;	(*pOut).f[ 4]=0.0f;	(*pOut).f[ 8]=0.0f;	(*pOut).f[12]=fX;
	(*pOut).f[ 1]=0.0f;	(*pOut).f[ 5]=1.0f;	(*pOut).f[ 9]=0.0f;	(*pOut).f[13]=fY;
	(*pOut).f[ 2]=0.0f;	(*pOut).f[ 6]=0.0f;	(*pOut).f[10]=1.0f;	(*pOut).f[14]=fZ;
	(*pOut).f[ 3]=0.0f;	(*pOut).f[ 7]=0.0f;	(*pOut).f[11]=0.0f;	(*pOut).f[15]=1.0f;
	return pOut;
}

/*!***************************************************************************
@Function Name		FmMat4Scaling
@Output			mOut	Scale matrix
@Input				fX		X component of the scaling
@Input				fY		Y component of the scaling
@Input				fZ		Z component of the scaling
@Description		Build a scale matrix mOut using fX, fY and fZ.
*****************************************************************************/
FmMat4* FmMat4Scaling(FmMat4 *pOut,const float fX,const float fY,const float fZ)
{
	Assert( pOut != NULL );
	(*pOut).f[ 0]=fX;	(*pOut).f[ 4]=0.0f;	(*pOut).f[ 8]=0.0f;	(*pOut).f[12]=0.0f;
	(*pOut).f[ 1]=0.0f;	(*pOut).f[ 5]=fY;	(*pOut).f[ 9]=0.0f;	(*pOut).f[13]=0.0f;
	(*pOut).f[ 2]=0.0f;	(*pOut).f[ 6]=0.0f;	(*pOut).f[10]=fZ;	(*pOut).f[14]=0.0f;
	(*pOut).f[ 3]=0.0f;	(*pOut).f[ 7]=0.0f;	(*pOut).f[11]=0.0f;	(*pOut).f[15]=1.0f;
	return pOut;
}

/*!***************************************************************************
@Function Name		FmMa4RotationX
@Output			mOut	Rotation matrix
@Input				fAngle	Angle of the rotation
@Description		Create an X rotation matrix mOut.
*****************************************************************************/
FmMat4* FmMat4RotationX(FmMat4 *pOut,const float fAngle)
{
	Assert( pOut != NULL );

    FmMat4Identity(pOut);
    pOut->m[1][1] = cos(fAngle);
    pOut->m[2][2] = cos(fAngle);
    pOut->m[1][2] = sin(fAngle);
    pOut->m[2][1] = -sin(fAngle);

	/*
	float		fCosine, fSine;

	fCosine	= (float)FMFCOS(fAngle);
	fSine	= (float)FMFSIN(fAngle);
 
	(*pOut).f[ 0]=1.0f;	(*pOut).f[ 4]=0.0f;	(*pOut).f[ 8]=0.0f;	(*pOut).f[12]=0.0f;
	(*pOut).f[ 1]=0.0f;	(*pOut).f[ 5]=fCosine;	(*pOut).f[ 9]=fSine;	(*pOut).f[13]=0.0f;
	(*pOut).f[ 2]=0.0f;	(*pOut).f[ 6]=-fSine;	(*pOut).f[10]=fCosine;	(*pOut).f[14]=0.0f;
	(*pOut).f[ 3]=0.0f;	(*pOut).f[ 7]=0.0f;	(*pOut).f[11]=0.0f;	(*pOut).f[15]=1.0f;
	*/
	return pOut;
}

/*!***************************************************************************
@Function Name		FmMat4RotationY
@Output			mOut	Rotation matrix
@Input				fAngle	Angle of the rotation
@Description		Create an Y rotation matrix mOut.
*****************************************************************************/
FmMat4* FmMat4RotationY(FmMat4	*pOut,const float fAngle)
{
	Assert( pOut != NULL );

    FmMat4Identity(pOut);
    pOut->m[0][0] = cos(fAngle);
    pOut->m[2][2] = cos(fAngle);
    pOut->m[0][2] = -sin(fAngle);
    pOut->m[2][0] = sin(fAngle);
	/*
	float		fCosine, fSine;

	fCosine	= (float)FMFCOS(fAngle);
	fSine	= (float)FMFSIN(fAngle);
	(*pOut).f[ 0]=fCosine;	(*pOut).f[ 4]=0.0f;	(*pOut).f[ 8]=-fSine;	(*pOut).f[12]=0.0f;
	(*pOut).f[ 1]=0.0f;		(*pOut).f[ 5]=1.0f;	(*pOut).f[ 9]=0.0f;		(*pOut).f[13]=0.0f;
	(*pOut).f[ 2]=fSine;	(*pOut).f[ 6]=0.0f;	(*pOut).f[10]=fCosine;	(*pOut).f[14]=0.0f;
	(*pOut).f[ 3]=0.0f;		(*pOut).f[ 7]=0.0f;	(*pOut).f[11]=0.0f;		(*pOut).f[15]=1.0f;
	*/
	return pOut;
}

/*!***************************************************************************
@Function Name		FmMat4RotationZ
@Output			mOut	Rotation matrix
@Input				fAngle	Angle of the rotation
@Description		Create an Z rotation matrix mOut.
*****************************************************************************/
FmMat4* FmMat4RotationZ(FmMat4 *pOut,const float fAngle)
{
	Assert( pOut != NULL );

	FmMat4Identity(pOut);
    pOut->m[0][0] = cos(fAngle);
    pOut->m[1][1] = cos(fAngle);
    pOut->m[0][1] = sin(fAngle);
    pOut->m[1][0] = -sin(fAngle);
	/*
	float		fCosine, fSine;

	fCosine =	(float)FMFCOS(fAngle);
	fSine =		(float)FMFSIN(fAngle);
 
	(*pOut).f[ 0]=fCosine;	(*pOut).f[ 4]=fSine;	(*pOut).f[ 8]=0.0f;	(*pOut).f[12]=0.0f;
	(*pOut).f[ 1]=-fSine;	(*pOut).f[ 5]=fCosine;	(*pOut).f[ 9]=0.0f;	(*pOut).f[13]=0.0f;
	(*pOut).f[ 2]=0.0f;		(*pOut).f[ 6]=0.0f;	    (*pOut).f[10]=1.0f;	(*pOut).f[14]=0.0f;
	(*pOut).f[ 3]=0.0f;		(*pOut).f[ 7]=0.0f;	    (*pOut).f[11]=0.0f;	(*pOut).f[15]=1.0f;
	*/

	return pOut;
}

/*!***************************************************************************
@Function Name		FmMat4Determinant
@Output			mOut	Returns the determinant of the matrix
@Input				mIn		Original matrix
@Description		Returns the determinant of a matrix.
*****************************************************************************/
VERTTYPE FmMat4Determinant(const FmMat4 *pIn)
{
	Assert( pIn != NULL );
	FmVec4 minor,v1,v2,v3;
	VERTTYPE det;
    v1.x = (*pIn)(0,0);  v1.y = (*pIn)(0,1); v1.z = (*pIn)(0,2) ; v1.w = (*pIn)(0,3);
    v2.x = (*pIn)(1,0);  v2.y = (*pIn)(1,1); v2.z = (*pIn)(1,2) ; v2.w = (*pIn)(1,3);
    v3.x = (*pIn)(2,0);  v3.y = (*pIn)(2,1); v3.z = (*pIn)(2,2) ; v3.w = (*pIn)(2,3);
    FmVec4Cross(&minor, &v1, &v2, &v3);
    det =  - ((*pIn)(3,0) * minor.x + (*pIn)(3,1) * minor.y + (*pIn)(3,2) * minor.z + (*pIn)(3,3) * minor.w);
    return det;
}

/*!***************************************************************************
@Function Name		FmMat4Determinant
@Output			    pOutScale   	Pointer to the output FmVec3 that contains scaling factors applied along the x, y, and z-axes
@Output			    pOutRotation	Returns the determinant of the matrix
@Output			    pOutTranslation	Returns the determinant of the matrix
@Input				pM       		Pointer to an input FmMat4 matrix to decompose.
@Return             If the function succeeds, the return value is S_OK(1). If the function fails, the return value can be the following: D3DERR_INVALIDCALL.(0)
@Description		Breaks down a general 3D transformation matrix into its scalar, rotational, and translational components
*****************************************************************************/
int FmMat4Decompose(FmVec3* pOutScale,FmQuat* pOutRotation,FmVec3* pOutTranslation,const FmMat4* pM)
{
	Assert( (pOutScale != NULL) && ( pOutRotation != NULL ) && (pOutTranslation != NULL) && ( pM != NULL) );

	FmMat4 normalized;
	FmVec3 vec;

    /*Compute the scaling part.*/
	vec.x = (*pM)(0,0);
	vec.y = (*pM)(1,0);
	vec.z = (*pM)(2,0);
	(*pOutScale).x = FmVec3Length(&vec);

	vec.x = (*pM)(0,1);
	vec.y = (*pM)(1,1);
	vec.z = (*pM)(2,1);
	(*pOutScale).y =  FmVec3Length(&vec);

	vec.x = (*pM)(0,2);
	vec.y = (*pM)(1,2);
	vec.z = (*pM)(2,2);
	(*pOutScale).z =  FmVec3Length(&vec);

    /*Compute the translation part.*/
	(*pOutTranslation).x = (*pM)(0,3);
	(*pOutTranslation).y = (*pM)(1,3);
	(*pOutTranslation).z = (*pM)(2,3);

    /*Let's calculate the rotation now*/
	if( ( (*pOutScale).x == 0.0f ) || ( (*pOutScale).y == 0.0f ) || ( (*pOutScale).z == 0.0f ) )
		return 0;

	normalized.f[ 0*4 + 0 ] = (*pM)(0,0) / (*pOutScale).x;
	normalized.f[ 0*4 + 1 ] = (*pM)(1,0) / (*pOutScale).x;
	normalized.f[ 0*4 + 2 ] = (*pM)(2,0) / (*pOutScale).x;
	
	normalized.f[ 1*4 + 0 ] = (*pM)(0,1) / (*pOutScale).y;
	normalized.f[ 1*4 + 1 ] = (*pM)(1,1) / (*pOutScale).y;
	normalized.f[ 1*4 + 2 ] = (*pM)(2,1) / (*pOutScale).y;

	normalized.f[ 2*4 + 0 ] = (*pM)(0,2) / (*pOutScale).z;
	normalized.f[ 2*4 + 1 ] = (*pM)(1,2) / (*pOutScale).z;
	normalized.f[ 2*4 + 2 ] = (*pM)(2,2) / (*pOutScale).z;

	FmQuatRotationMat4( pOutRotation , &normalized );
	return 1;
}


/*!***************************************************************************
@Function Name		FmMat4Transpose
@Output			mOut	Transposed matrix
@Input				mIn		Original matrix
@Description		Compute the transpose matrix of mIn.
*****************************************************************************/
FmMat4* FmMat4Transpose(FmMat4 *pOut,const FmMat4 *pIn)
{
	Assert( (pOut != NULL ) && ( pIn != NULL ) );
    FmMat4	mIn = *pIn;
	FmMat4	mTmp;

	mTmp.f[ 0]=mIn.f[ 0];	mTmp.f[ 4]=mIn.f[ 1];	mTmp.f[ 8]=mIn.f[ 2];	mTmp.f[12]=mIn.f[ 3];
	mTmp.f[ 1]=mIn.f[ 4];	mTmp.f[ 5]=mIn.f[ 5];	mTmp.f[ 9]=mIn.f[ 6];	mTmp.f[13]=mIn.f[ 7];
	mTmp.f[ 2]=mIn.f[ 8];	mTmp.f[ 6]=mIn.f[ 9];	mTmp.f[10]=mIn.f[10];	mTmp.f[14]=mIn.f[11];
	mTmp.f[ 3]=mIn.f[12];	mTmp.f[ 7]=mIn.f[13];	mTmp.f[11]=mIn.f[14];	mTmp.f[15]=mIn.f[15];

	*pOut = mTmp;
	return pOut;
}
 

/*!***************************************************************************
@Function			FmMat4Inverse
@Output			mOut	Inversed matrix
@Input				mIn		Original matrix
@Description		Compute the inverse matrix of mIn.
Uses a linear equation solver and the knowledge that M.M^-1=I.
Use this fn to calculate the inverse of matrices that
FmMatrixInverse() cannot.
*****************************************************************************/
FmMat4* FmMat4InverseDeterminant(FmMat4 *pOut,const FmMat4 *pIn)
{
	Assert( (pOut != NULL) && (pIn != NULL) );
	FmMat4 mIn = *pIn;
	FmMat4		mTmp;
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
	return pOut;
}

/*!***************************************************************************
@Function			FmMat4Inverse
@Output			    pOut	Inversed matrix
@Input				pDeterminant	Pointer to a FLOAT value containing the determinant of the matrix. If the determinant is not needed, set this parameter to NULL.
@Input				pIn		Original matrix
@Description		Calculates the inverse of a matrix.
The return value for this function is the same value returned in the pOut parameter. 
In this way, the FmMat4Inverse function can be used as a parameter for another function.
*****************************************************************************/
FmMat4* FmMat4Inverse(FmMat4 *pOut,VERTTYPE * pDeterminant,const FmMat4 *pIn)
{
	Assert( (pOut != NULL) && (pIn != NULL) );
	FmMat4 out;
	FmVec4 v,vec[3];
    VERTTYPE det;

	det = FmMat4Determinant(pIn);
	if( !det ) return NULL;
	if( pDeterminant ) *pDeterminant = det;

	for(int i = 0; i < 4; i++ )
	{
		for(int j = 0; j < 4; j++ )
		{
			if( j != i)
			{
				int a = j;
				if( j > i ) a = a - 1;
				vec[a].x = (*pIn)(0,j);
				vec[a].y = (*pIn)(1,j);
				vec[a].z = (*pIn)(2,j);
				vec[a].w = (*pIn)(3,j);
			}
		}

		FmVec4Cross(&v,&vec[0],&vec[1],&vec[2]);
		out.f[0*4 + i] = pow( -1.0f,i) *v.x / det;
		out.f[1*4 + i] = pow( -1.0f,i) *v.y / det;
		out.f[2*4 + i] = pow( -1.0f,i) *v.z / det;
		out.f[3*4 + i] = pow( -1.0f,i) *v.w / det;
	}
	*pOut = out;
	return pOut;
}


/*!***************************************************************************
@Function			FmMat4LookAtLH
@Output			mOut	Look-at view matrix
@Input				vEye	Position of the camera
@Input				vAt		Point the camera is looking at
@Input				vUp		Up direction for the camera
@Description		Create a look-at view matrix.
*****************************************************************************/
FmMat4* FmMat4LookAtLH(FmMat4 *pOut,const FmVec3 *pvEye,const FmVec3 *pvAt,const FmVec3	*pvUp)
{
	Assert( (pOut != NULL) && ( pvEye != NULL) && (pvUp != NULL ) );
    FmVec3 vEye = *pvEye;
	FmVec3 vAt = *pvAt;
    FmVec3 vUp = *pvUp;

	FmVec3 f, vUpActual, s, u;
	FmMat4	t;

	f.x = vEye.x - vAt.x;
	f.y = vEye.y - vAt.y;
	f.z = vEye.z - vAt.z;

	FmVec3Normalize(&f, &f);
	FmVec3Normalize(&vUpActual, &vUp);
	FmVec3Cross(&s, &f, &vUpActual);
	FmVec3Cross(&u, &s, &f);

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

	FmMat4Translation(&t, -vEye.x, -vEye.y, -vEye.z);
	FmMat4Multiply(pOut, &t, pOut);
	return pOut;
}


/*!***************************************************************************
@Function			FmMat4LookAtRH
@Output			mOut	Look-at view matrix
@Input				vEye	Position of the camera
@Input				vAt		Point the camera is looking at
@Input				vUp		Up direction for the camera
@Description		Create a look-at view matrix.
*****************************************************************************/
FmMat4* FmMat4LookAtRH(FmMat4 *pOut,const FmVec3 *pvEye,const FmVec3 *pvAt,const FmVec3	*pvUp)
{
	Assert( (pOut != NULL) && (pvEye != NULL) && (pvAt != NULL) && (pvUp != NULL) );
    FmVec3 vEye = *pvEye;
	FmVec3 vAt = *pvAt;
	FmVec3 vUp = *pvUp;
	FmVec3 f, vUpActual, s, u;
	FmMat4	t;

	f.x = vAt.x - vEye.x;
	f.y = vAt.y - vEye.y;
	f.z = vAt.z - vEye.z;

	FmVec3Normalize(&f, &f);
	FmVec3Normalize(&vUpActual, &vUp);
	FmVec3Cross(&s, &f, &vUpActual);
	FmVec3Cross(&u, &s, &f);

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

	FmMat4Translation(&t, -vEye.x, -vEye.y, -vEye.z);
	FmMat4Multiply(pOut, &t, pOut);
	return pOut;
}

/*!***************************************************************************
@Function		FmMat4PerspectiveFovLH
@Output		mOut		Perspective matrix
@Input			fFOVy		Field of view
@Input			fAspect		Aspect ratio
@Input			fNear		Near clipping distance
@Input			fFar		Far clipping distance
@Input			bRotate		Should we rotate it ? (for upright screens)(defaut = false)
@
@Description	Create a perspective matrix.
*****************************************************************************/
FmMat4* FmMat4PerspectiveFovLH(FmMat4 *pOut,const float fFOVy,const float fAspect,const float fNear,const float fFar,const int bRotate)
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
		FmMat4 mRotation, mTemp = *pOut;
		FmMat4RotationZ(&mRotation, 90.0f*FM_PIf/180.0f);
		FmMat4Multiply(pOut, &mTemp, &mRotation);
	}
	return pOut;
}

/*!***************************************************************************
@Function		FmMat4PerspectiveFovRH
@Output		mOut		Perspective matrix
@Input			fFOVy		Field of view
@Input			fAspect		Aspect ratio
@Input			fNear		Near clipping distance
@Input			fFar		Far clipping distance
@Input			bRotate		Should we rotate it ? (for upright screens)
@Description	Create a perspective matrix.
*****************************************************************************/
FmMat4* FmMat4PerspectiveFovRH(FmMat4 *pOut,const float fFOVy,const float fAspect,const float fNear,const float fFar,const int bRotate)
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
		FmMat4 mRotation, mTemp = *pOut;
		FmMat4RotationZ(&mRotation, -90.0f*FM_PIf/180.0f);
		FmMat4Multiply(pOut, &mTemp, &mRotation);
	}
	return pOut;
}

/*!***************************************************************************
@Function		FmMat4OrthoLH
@Output		mOut		Orthographic matrix
@Input			w			Width of the screen
@Input			h			Height of the screen
@Input			zn			Near clipping distance
@Input			zf			Far clipping distance
@Input			bRotate		Should we rotate it ? (for upright screens)
@Description	Create an orthographic matrix.
*****************************************************************************/
FmMat4* FmMat4OrthoLH(FmMat4 *pOut,const float w,const float h,const float zn,const float zf,const int bRotate)
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
	(*pOut).f[11] = 0;

	(*pOut).f[12] = 0;
	(*pOut).f[13] = 0;
	(*pOut).f[14] = zn / (zn - zf);
	(*pOut).f[15] = 1;

	if (bRotate)
	{
		FmMat4 mRotation, mTemp = *pOut;
		FmMat4RotationZ(&mRotation, -90.0f*FM_PIf/180.0f);
		FmMat4Multiply(pOut, &mRotation, &mTemp);
	}
	return pOut;
}

/*!***************************************************************************
@Function		FmMat4OrthoRH
@Output		mOut		Orthographic matrix
@Input			w			Width of the screen
@Input			h			Height of the screen
@Input			zn			Near clipping distance
@Input			zf			Far clipping distance
@Input			bRotate		Should we rotate it ? (for upright screens)
@Description	Create an orthographic matrix.
*****************************************************************************/
FmMat4* FmMat4OrthoRH(FmMat4 *pOut,const float w,const float h,const float zn,const float zf,const int  bRotate)
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
		FmMat4 mRotation, mTemp = *pOut;
		FmMat4RotationZ(&mRotation, -90.0f*FM_PIf/180.0f);
		FmMat4Multiply(pOut, &mRotation, &mTemp);
	}
	return pOut;
}
 
/*!***************************************************************************
@Function		FmMat4OrthoOffCenterLH
@Output		    pOut		Orthographic matrix
@Input			l			Minimum x-value of view volume
@Input			r			Maximum x-value of view volume.
@Input          b           Minimum y-value of view volume
@Input          t           Maximum y-value of view volume
@Input			zn			Near clipping distance
@Input			zf			Far clipping distance
@Description	Builds a customized, left-handed orthographic projection matrix.
*****************************************************************************/
FmMat4* FmMat4OrthoOffCenterLH(FmMat4* pOut,float l,float r,float b,float t,float zn,float zf)
{
	Assert(pOut != NULL);
	FmMat4Identity(pOut);
	(*pOut)._11 = 2.0 / (r - l);
	(*pOut)._22 = 2.0 / (t - b);
	(*pOut)._33 = 1.0 / (zf - zn);
	(*pOut)._41 = (l + r) / ( l - r);
	(*pOut)._42 = (t + b) / ( b - t);
	(*pOut)._43 = zn / (zn - zf);
	
	return pOut;
}
 

/*!***************************************************************************
@Function		FmMat4OrthoOffCenterRH
@Output		    pOut		Orthographic matrix
@Input			l			Minimum x-value of view volume
@Input			r			Maximum x-value of view volume.
@Input          b           Minimum y-value of view volume
@Input          t           Maximum y-value of view volume
@Input			zn			Near clipping distance
@Input			zf			Far clipping distance
@Description	Create an orthographic matrix.
*****************************************************************************/
FmMat4* FmMat4OrthoOffCenterRH(FmMat4* pOut,float l,float r,float b,float t,float zn,float zf)
{
	Assert( pOut != NULL);
	FmMat4Identity(pOut);
	(*pOut)._11 = 2.0 / (r - l);
	(*pOut)._22 = 2.0 / (t - b);
	(*pOut)._33 = 1.0 / (zn - zf);
	(*pOut)._41 = (l + r) / ( l - r);
	(*pOut)._42 = (t + b) / ( b - t);
	(*pOut)._43 = zn / (zn - zf);
	return pOut;
}


/*!***************************************************************************
@Function		FmMat4Reflect
@Output		    pOut		Orthographic matrix
@Input			pPlane		Pointer to the source FmPlane structure.
@Description	Create an orthographic matrix.
*****************************************************************************/
FmMat4* FmMat4Reflect(FmMat4 *pOut,const FmPlane* pPlane)
{
	Assert( (pOut != NULL) && (pPlane != NULL) );

	FmPlane Nplane;
	FmPlaneNormalize(&Nplane,pPlane);
    FmMat4Identity(pOut);

	(*pOut).f[0*4 + 0] = 1.0f - 2.0f * Nplane.a * Nplane.a;
	(*pOut).f[0*4 + 1] = -2.0f * Nplane.a * Nplane.b;
	(*pOut).f[0*4 + 2] = -2.0f * Nplane.a * Nplane.c;

    (*pOut).f[1*4 + 0] = -2.0f * Nplane.a * Nplane.b;
	(*pOut).f[1*4 + 1] = 1.0f - 2.0f * Nplane.b*Nplane.b;
	(*pOut).f[1*4 + 2] = -2.0f * Nplane.b * Nplane.c;

	(*pOut).f[2*4 + 0] = -2.0f * Nplane.c * Nplane.a;
	(*pOut).f[2*4 + 1] = -2.0f * Nplane.c * Nplane.b;
	(*pOut).f[2*4 + 2] = 1.0f - 2.0f * Nplane.c * Nplane.c;

	(*pOut).f[3*4 + 0] = -2.0f * Nplane.d * Nplane.a;
	(*pOut).f[3*4 + 1] = -2.0f * Nplane.d * Nplane.b;
	(*pOut).f[3*4 + 2] = -2.0f * Nplane.d * Nplane.c;

	return pOut;
}

/*!***************************************************************************
@Function		FmMat4RotationAxis
@Output		    pOut		Orthographic matrix
@Input			pv	    	Pointer to the source FmVec3 structure.
@Input			angle	    Angle of rotation in radians. Angles are measured clockwise when looking along the rotation axis toward the origin..
@Description	Builds a matrix that rotates around an arbitrary axis.
*****************************************************************************/
FmMat4* FmMat4RotationAxis(FmMat4 *pOut, const FmVec3* pv,VERTTYPE angle)
{
	Assert( (pOut != NULL ) && (pv != NULL) );

	FmVec3 v;

	FmVec3Normalize(&v,pv);
	FmMat4Identity(pOut);

	(*pOut).f[0*4 + 0] = (1.0f - FMFCOS(angle) ) * v.x * v.x + FMFCOS(angle);
	(*pOut).f[1*4 + 0] = (1.0f - FMFCOS(angle) ) * v.x * v.y + FMFSIN(angle) * v.z;
	(*pOut).f[2*4 + 0] = (1.0f - FMFCOS(angle) ) * v.x * v.z + FMFSIN(angle) * v.y;

	(*pOut).f[0*4 + 1] = (1.0f - FMFCOS(angle) ) * v.y * v.x + FMFSIN(angle) * v.z;
	(*pOut).f[1*4 + 1] = (1.0f - FMFCOS(angle) ) * v.y * v.y + FMFCOS(angle);
	(*pOut).f[2*4 + 1] = (1.0f - FMFCOS(angle) ) * v.y * v.z - FMFSIN(angle) * v.x; 

	(*pOut).f[0*4 + 2] = (1.0f - FMFCOS(angle) ) * v.z * v.x - FMFSIN(angle) * v.y;
	(*pOut).f[1*4 + 2] = (1.0f - FMFCOS(angle) ) * v.z * v.y + FMFSIN(angle) * v.x;
	(*pOut).f[2*4 + 2] = (1.0f - FMFCOS(angle) ) * v.z * v.z + FMFCOS(angle);

	return pOut;
}


/*!***************************************************************************
@Function		FmMat4RotationQuat
@Output		    pOut		Orthographic matrix
@Input			pQuat	    Pointer to the source FmQuat structure.
@Input			angle	    Angle of rotation in radians. Angles are measured clockwise when looking along the rotation axis toward the origin..
@Description	Builds a rotation matrix from a quaternion.
*****************************************************************************/
FmMat4* FmMat4RotationQuat(FmMat4 *pOut, const FmQuat* pQuat)
{
	Assert( (pOut != NULL) && (pQuat != NULL ) );
	const FmQuat* pq = pQuat;
    FmMat4Identity(pOut);
    pOut->m[0][0] = 1.0f - 2.0f * (pq->y * pq->y + pq->z * pq->z);
    pOut->m[0][1] = 2.0f * (pq->x *pq->y + pq->z * pq->w);
    pOut->m[0][2] = 2.0f * (pq->x * pq->z - pq->y * pq->w);
    pOut->m[1][0] = 2.0f * (pq->x * pq->y - pq->z * pq->w);
    pOut->m[1][1] = 1.0f - 2.0f * (pq->x * pq->x + pq->z * pq->z);
    pOut->m[1][2] = 2.0f * (pq->y *pq->z + pq->x *pq->w);
    pOut->m[2][0] = 2.0f * (pq->x * pq->z + pq->y * pq->w);
    pOut->m[2][1] = 2.0f * (pq->y *pq->z - pq->x *pq->w);
    pOut->m[2][2] = 1.0f - 2.0f * (pq->x * pq->x + pq->y * pq->y);

    //FmMatrixRotationQuaternion(pOut,pQuat);
	return pOut;
}

/*!***************************************************************************
@Function		FmMat4RotationQuat
@Output		    pOut		Orthographic matrix
@Input			Yaw	        Yaw around the y-axis, in radians.
@Input			Pitch	    Pitch around the x-axis, in radians.
@Input          Roll        Roll around the z-axis, in radians
@Description	Builds a matrix with a specified yaw, pitch, and roll.
The return value for this function is the same value returned in the pOut parameter. 
In this way, the D3DXMatrixRotationYawPitchRoll function can be used as a parameter for another function.
The order of transformations is roll first, then pitch, then yaw. 
Relative to the object's local coordinate axis, this is equivalent to rotation around the z-axis,
followed by rotation around the x-axis, followed by rotation around the y-axis, as shown in the following illustration
*****************************************************************************/
FmMat4* FmMat4RotationYawPitchRoll(FmMat4* pOut,VERTTYPE Yaw,VERTTYPE Pitch,VERTTYPE Roll)
{
	Assert( pOut != NULL);

	FmMat4 m;
	FmMat4Identity(pOut);
	FmMat4RotationZ(&m,Roll);
	FmMat4Multiply(pOut,pOut,&m);
	FmMat4RotationX(&m,Pitch);
    FmMat4Multiply(pOut,pOut,&m);
    FmMat4RotationY(&m,Yaw);
    FmMat4Multiply(pOut,pOut,&m);
	return pOut;
}


/*!***************************************************************************
@Function		FmIntersectTri
@Input		    pv1		    Pointer to the source FmVec3 structure. describing the first triangle vertex position
@Input			pv2	        Pointer to the source FmVec3 structure. describing the second triangle vertex position.
@Input			pv3	        Pointer to the source FmVec3 structure. describing the third triangle vertex position
@Input			pRayPos     Pointer to a FmVec3 structure, specifying the point where the ray begins
@Input			pRayDir     Pointer to a FmVec3 structure, specifying the direction of the ray
@Output		    pU		    Barycentric hit coordinates, U
@Output 		pV          Barycentric hit coordinates, V
@Output 		pDist       Ray-intersection parameter distance
@Return         Returns TRUE(1) if the ray intersects the area of the triangle. Otherwise, returns FALSE(0)
@Description	The FmIntersectTri function provides a way to understand points in and around a triangle, independent of where the triangle is actually located. 
This function returns the resulting point by using the following equation: V1 + U(V2 - V1) + V(V3 - V1).
Any point in the plane V1V2V3 can be represented by the barycentric coordinate (U,V). 
The parameter U controls how much V2 gets weighted into the result, and the parameter V controls how much V3 gets weighted into the result.
Lastly, the value of [1 - (U + V)] controls how much V1 gets weighted into the result.
Barycentric coordinates are a form of general coordinates.
In this context, using barycentric coordinates represents a change in coordinate systems.
What holds true for Cartesian coordinates holds true for barycentric coordinates.
Barycentric coordinates define a point inside a triangle in terms of the triangle's vertices.
For a more in-depth description of barycentric coordinates, see Mathworld's Barycentric Coordinates Description.
*****************************************************************************/
int FmIntersectTri(const FmVec3* pv1,const FmVec3* pv2,const FmVec3* pv3,const FmVec3* pRayPos,const FmVec3* pRayDir,VERTTYPE* pU,VERTTYPE* pV,VERTTYPE* pDist)
{
   Assert( (pv1 != NULL) && (pv2 != NULL) && (pv3 != NULL) && (pRayPos != NULL) && (pRayDir != NULL) && (pU != NULL) && (pV != NULL) && (pDist != NULL));
   FmMat4 m;
   FmVec4 vec;

   m.f[ 0*4 + 0] = (*pv2).x - (*pv1).x;
   m.f[ 1*4 + 0] = (*pv3).x - (*pv1).x;
   m.f[ 2*4 + 0] = - (*pRayDir).x;
   m.f[ 3*4 + 0] = 0.0f;

   m.f[ 0*4 + 1] = (*pv2).y - (*pv1).z;
   m.f[ 1*4 + 1] = (*pv3).y - (*pv1).z;
   m.f[ 2*4 + 1] = - (*pRayDir).y;
   m.f[ 3*4 + 1] = 0.0f;

   m.f[ 0*4 + 2] = (*pv2).z - (*pv1).z;
   m.f[ 1*4 + 2] = (*pv3).z - (*pv1).z;
   m.f[ 2*4 + 2] = - (*pRayDir).z;
   m.f[ 3*4 + 2] = 0.0f;

   m.f[ 0*4 + 3] = 0.0f;
   m.f[ 1*4 + 3] = 0.0f;
   m.f[ 2*4 + 3] = 0.0f;
   m.f[ 3*4 + 3] = 1.0f;

   vec.x = (*pRayPos).x - (*pv1).x;
   vec.y = (*pRayPos).y - (*pv1).y;
   vec.z = (*pRayPos).z - (*pv1).z;
   vec.w = 0.0f;

   if( FmMat4Inverse(&m, NULL,&m) )
   {
	   FmVec4Transform(&vec,&vec,&m);
	   if( (vec.x >= 0.0f) && (vec.y >= 0.0f) && (vec.x + vec.y <= 1.0f ) && (vec.z >= 0.0f) )
	   {
		   *pU = vec.x;
		   *pV = vec.y;
		   *pDist = fabs( vec.z );
		   return 1;
	   }
   }
   return 0;
}


/*!***************************************************************************
@Function		FmBoxBoundProbe
@Input pMin Pointer to a FmVec3 structure, describing the lower-left corner of the bounding box. See Remarks.
@Input pMax Pointer to a FmVec3 structure, describing the upper-right corner of the bounding box. See Remarks.
@Input pRayPos Pointer to a FmVec3 structure, specifying the origin coordinate of the ray.
@Input pRayDir Pointer to a FmVec3 structure, specifying the direction of the ray. This vector should not be (0,0,0) but does not need to be normalized.
@Return     Returns TRUE if the ray intersects the volume of the box's bounding box. Otherwise, returns FALSE.
FmBoxBoundProbe determines if the ray intersects the volume of the box's bounding box, not just the surface of the box.
The values passed to D3DXboxBoundProbe are xmin, xmax, ymin, ymax, zmin, and zmax. Thus, the following defines the corners of the bounding box. 
xmax, ymax, zmax
xmax, ymax, zmin
xmax, ymin, zmax
xmax, ymin, zmin
xmin, ymax, zmax
xmin, ymax, zmin
xmin, ymin, zmax
xmin, ymin, zmin

The depth of the bounding box in the z direction is zmax - zmin, in the y direction is ymax - ymin, and in the x direction is xmax - xmin. 
For example, with the following minimum and maximum vectors, min (-1, -1, -1) and max (1, 1, 1), the bounding box is defined in the following manner. 

1,  1,  1
1,  1, -1
1, -1,  1
1, -1, -1
-1,  1,  1
-1,  1, -1
-1, -1,  1
-1, -1, -l
*****************************************************************************/
bool FmBoxBoundProbe(const FmVec3* pMin,const FmVec3* pMax,const FmVec3* pRayPos,const FmVec3* pRayDir)
{
    Assert( (pMin != NULL) && (pMax != NULL) && (pRayPos != NULL) && (pRayDir != NULL) );
	float div = 1.0f / (*pRayDir).x;
	float tmin,tmax,tymin,tymax,tzmin,tzmax;
	if( div >= 0.0f )
	{
		tmin = (pMin->x - pRayPos->x ) * div;
		tmax = (pMax->x - pRayPos->x ) * div;
	}
	else
	{
		tmin = (pMax->x - pRayPos->x ) * div;
		tmax = (pMin->x - pRayPos->x ) * div;
	}

	if( tmax < 0.0f )
		return false;

	div = 1.0f / pRayDir->y;

	if( div >= 0.0f)
	{
		tymin = (pMin->y - pRayPos->y ) * div;
		tymax = (pMax->y - pRayPos->y ) * div;
	}
	else
	{
		tymin = (pMax->y - pRayPos->y ) * div;
		tymax = (pMin->y - pRayPos->y ) * div;
	}

	if( ( tymax < 0.0f ) || ( tmin > tymax) || (tymin > tmax) )
		return false;

	if( tymin > tmin) tmin = tymin;
	if( tymax < tmax) tmax = tymax;

	div = 1.0f / pRayDir->z;
	if( div >= 0.0f )
	{
		tzmin = ( pMin->z - pRayPos->z ) * div;
		tzmax = ( pMax->z - pRayPos->z ) * div;
	}
	else
	{
		tzmin = ( pMax->z - pRayPos->z ) * div;
		tzmax = ( pMin->z - pRayPos->z ) * div;
	}

	if( (tzmax < 0.0f ) || ( tmin > tzmax) || ( tzmin > tmax) )
		return false;

	return true;

}

FmVec3* FmVec3Unproject(FmVec3* pOut,const FmVec3* pV,const FmMat4* pProjection,const FmMat4* pView,const FmMat4* pWorld)
{
	FmMat4 m;
	FmMat4Identity(&m);
	FmVec3 out;

	if(pWorld)
		FmMat4Multiply(&m,&m,pWorld);

	if(pView)
		FmMat4Multiply(&m,&m,pView);

	if(pProjection)
		FmMat4Multiply(&m, &m, pProjection);

	FmMat4Inverse(&m, NULL, &m);

    FmVec3TransformCoord(&out, pV, &m);
    *pOut = out;
    return pOut;
}


/*!***************************************************************************
@Function		FmVec3ProjectForD3D
@Input x		ViewportX
@Input y		ViewportY
@Input width	ViewportWidth
@Input height	ViewportHeight
@Input min_z	ViewportMinZ
@Input max_z	ViewportMaxZ
@Description	Project vector from object space into screen space
*****************************************************************************/
FmVec3* FmVec3ProjectForD3D(FmVec3* pOut,const FmVec3* pV,const FmMat4* pProjection,const FmMat4* pView,const FmMat4* pWorld,
					 unsigned int x,unsigned int y,unsigned int width,unsigned int height,float min_z,float max_z)
{
    Assert( (pOut != NULL) && (pV != NULL) && (pProjection != NULL) && (pView != NULL)  && (pWorld != NULL) );

	FmMat4 m;
	FmVec3 out;
	FmMat4Multiply(&m,pWorld,pView);
    FmMat4Multiply(&m,&m,pProjection);
	FmVec3TransformCoord(&out,pV,&m);

    out.x = x + (1.0f + out.x ) * width * 0.5f;
	out.y = y + (1.0f - out.y ) * height * 0.5f;
	out.z = min_z + out.z * ( max_z - min_z );

	*pOut = out;
	return pOut;
}

/*!***************************************************************************
@Function		FmVec3UnprojectForD3D
@Input x		ViewportX
@Input y		ViewportY
@Input width	ViewportWidth
@Input height	ViewportHeight
@Input min_z	ViewportMinZ
@Input max_z	ViewportMaxZ
@Description	Project vector from screen space into object space
*****************************************************************************/
FmVec3* FmVec3UnprojectForD3D(FmVec3* pOut,const FmVec3* pV,const FmMat4* pProjection,const FmMat4* pView,const FmMat4* pWorld,
							  unsigned int x,unsigned int y,unsigned int width,unsigned int height,float min_z,float max_z)
{
	Assert( (pOut != NULL) && (pV != NULL) && (pProjection != NULL) && (pView != NULL)  && (pWorld != NULL) );

	FmMat4 m;
	FmVec3 out;
	FmMat4Multiply(&m,pWorld,pView);
    FmMat4Multiply(&m,&m,pProjection);
	FmMat4Inverse(&m,NULL,&m);

	out.x = 2.0f * (pV->x - x ) / width - 1.0f;
	out.y = 1.0f - 2.0f * (pV->y - y ) / height;

	out.z = ( pV->z - min_z ) / (max_z - min_z);

	FmVec3TransformCoord(&out,&out,&m);
	*pOut = out;

	return pOut;
}
