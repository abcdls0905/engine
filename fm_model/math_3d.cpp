//--------------------------------------------------------------------
// 文件名:		math_3d.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年5月4日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "math_3d.h"
#include <math.h>
#include <float.h>
#include "../public/portable.h"
//#include "../utils/system_utils.h"
#define PI 3.1415926535897932

// 浮点数绝对值
static inline float absolute(float num)
{
	if (num < 0)
	{
		return (0 - num);
	}

	return num;
}

// 三角面法线
static FmVec3 plane_normal(const FmVec3& v1, const FmVec3& v2, 
	const FmVec3& v3)
{
	FmVec3 vVector1 = v3 - v1;
	FmVec3 vVector2 = v2 - v1;
	FmVec3 vNormal;
	
	FmVec3Cross(&vNormal, &vVector1, &vVector2);
	FmVec3Normalize(&vNormal, &vNormal);

	return vNormal;
}

// 点到面的距离
static inline float plane_distance(const FmVec3& Normal, 
	const FmVec3& Point)
{	
	return -((Normal.x * Point.x) + (Normal.y * Point.y) 
		+ (Normal.z * Point.z));
}

// 向量的夹角
static double angle_between_vectors(const FmVec3& Vector1, 
	const FmVec3& Vector2)
{							
	double dotProduct = FmVec3Dot(&Vector1, &Vector2);				
	double vectorsMagnitude = FmVec3Length(&Vector1) 
		* FmVec3Length(&Vector2);
	double angle = acos(dotProduct / vectorsMagnitude);

	if (Port_IsNaN(angle))
	{
		return 0;
	}
	
	return angle;
}

static bool inside_polygon(const FmVec3& vIntersection, 
	const FmVec3& v1, const FmVec3& v2, const FmVec3& v3)
{
	const double MATCH_FACTOR = 0.999999;	
	double Angle = 0.0;
	FmVec3 vA;
	FmVec3 vB;
	
	vA = v1 - vIntersection;
	vB = v2 - vIntersection;
	Angle += angle_between_vectors(vA, vB);
											
	if (Angle >= (MATCH_FACTOR * (2.0 * PI)) )
	{
		return true;
	}
		
	vA = v2 - vIntersection;
	vB = v3 - vIntersection;
	Angle += angle_between_vectors(vA, vB);
											
	if (Angle >= (MATCH_FACTOR * (2.0 * PI)) )
	{
		return true;
	}
		
	vA = v3 - vIntersection;
	vB = v1 - vIntersection;
	Angle += angle_between_vectors(vA, vB);
											
	if (Angle >= (MATCH_FACTOR * (2.0 * PI)) )
	{
		return true;
	}
		
	return false;
}

static FmVec3 closest_point_on_line(const FmVec3& vA, 
	const FmVec3& vB, const FmVec3& vPoint)
{
	FmVec3 vVector1 = vPoint - vA;
    FmVec3 vVector2 = vB - vA;

	FmVec3Normalize(&vVector2, &vVector2);
	FmVec3 len = vA - vB;
	float d = FmVec3Length(&len);
    float t = FmVec3Dot(&vVector2, &vVector1);

    if (t <= 0) 
    {
		return vA;
	}

    if (t >= d) 
    {
		return vB;
	}
 
    FmVec3 vVector3 = vVector2 * t;
    FmVec3 vClosestPoint = vA + vVector3;

	return vClosestPoint;
}

static bool classify_sphere(const FmVec3& vCenter, 
	const FmVec3& vNormal, const FmVec3& vPoint, float radius, 
	float& distance)
{
	// First we need to find the distance our polygon plane is from the origin.
	float d = (float)plane_distance(vNormal, vPoint);

	// Here we use the famous distance formula to find the distance the center point
	distance = vNormal.x * vCenter.x + vNormal.y * vCenter.y 
		+ vNormal.z * vCenter.z + d;

	if (absolute(distance) < radius)
	{
		return true;
	}

	return false;
}

static bool edge_sphere_collision(const FmVec3& vCenter, float radius, 
	const FmVec3& v1, const FmVec3& v2, const FmVec3& v3)
{
	FmVec3 vPoint = closest_point_on_line(v1, v2, vCenter);
	FmVec3 len = vPoint - vCenter;
	float distance = FmVec3Length(&len);

	if (distance < radius)
	{
		return true;
	}

	vPoint = closest_point_on_line(v2, v3, vCenter);
	len = vPoint - vCenter;
	distance = FmVec3Length(&len);

	if (distance < radius)
	{
		return true;
	}

	vPoint = closest_point_on_line(v3, v1, vCenter);
	len = vPoint - vCenter;
	distance = FmVec3Length(&len);

	if (distance < radius)
	{
		return true;
	}

	return false;
}

bool math_sphere_triangle_collision(const FmVec3& vCenter, float radius,
	const FmVec3& v1, const FmVec3& v2, const FmVec3& v3)
{
	// 1) STEP ONE - Finding the sphere's classification
	FmVec3 vNormal = plane_normal(v1, v2, v3);

	float distance = 0.0F;

	if (classify_sphere(vCenter, vNormal, v1, radius, distance))
	{
		// 2) STEP TWO - Finding the psuedo intersection point on the plane
		FmVec3 vOffset = vNormal * distance;
		FmVec3 vPosition = vCenter - vOffset;

		// 3) STEP THREE - Check if the intersection point is inside the polygons perimeter
		if (inside_polygon(vPosition, v1, v2, v3))
		{
			return true;
		}
		else
		{
			// 4) STEP FOUR - Check the sphere to see if it intersects the polygon edges
			if (edge_sphere_collision(vCenter, radius, v1, v2, v3))
			{
				return true;
			}
		}
	}

	return false;
}

bool math_sphere_triangle_aabb_collision(const FmVec3& center, 
	float radius, const FmVec3& v1, const FmVec3& v2, 
	const FmVec3& v3)
{
	FmVec3 v_min = v1;
	FmVec3 v_max = v1;

	FmVec3Minimize(&v_min, &v_min, &v2);
	FmVec3Minimize(&v_min, &v_min, &v3);
	FmVec3Maximize(&v_max, &v_max, &v2);
	FmVec3Maximize(&v_max, &v_max, &v3);

	return (center.x >= v_min.x - radius) && (center.x <= v_max.x + radius)
		&& (center.z >= v_min.z - radius) && (center.z <= v_max.z + radius)
		&& (center.y >= v_min.y - radius) && (center.y <= v_max.y + radius);
}


//! if OPC_TRITRI_EPSILON_TEST is true then we do a check (if |dv|<EPSILON then dv=0.0;) else no check is done (which is less robust, but faster)
#define LOCAL_EPSILON 0.000001f

//! sort so that a<=b
#define SORT(a,b)			\
	if(a>b)					\
	{						\
		const float c=a;	\
		a=b;				\
		b=c;				\
	}

//! Edge to edge test based on Franlin Antonio's gem: "Faster Line Segment Intersection", in Graphics Gems III, pp. 199-202
#define EDGE_EDGE_TEST(V0, U0, U1)						\
    Bx = U0[i0] - U1[i0];							    \
    By = U0[i1] - U1[i1];							    \
    Cx = V0[i0] - U0[i0];							    \
    Cy = V0[i1] - U0[i1];							    \
	f  = Ay*Bx - Ax*By;									\
	d  = By*Cx - Bx*Cy;									\
	if((f>0.0f && d>=0.0f && d<=f) || (f<0.0f && d<=0.0f && d>=f))	\
	{													\
		const float e=Ax*Cy - Ay*Cx;					\
		if(f>0.0f)										\
		{												\
			if(e>=0.0f && e<=f) return true;			\
		}												\
		else											\
		{												\
			if(e<=0.0f && e>=f) return true;			\
		}												\
	}

//! TO BE DOCUMENTED
#define EDGE_AGAINST_TRI_EDGES(V0, V1, U0, U1, U2)		\
{														\
	float Bx,By,Cx,Cy,d,f;								\
	const float Ax = V1[i0] - V0[i0];				    \
	const float Ay = V1[i1] - V0[i1];				    \
	/* test edge U0,U1 against V0,V1 */					\
	EDGE_EDGE_TEST(V0, U0, U1);							\
	/* test edge U1,U2 against V0,V1 */					\
	EDGE_EDGE_TEST(V0, U1, U2);							\
	/* test edge U2,U1 against V0,V1 */					\
	EDGE_EDGE_TEST(V0, U2, U0);							\
}

//! TO BE DOCUMENTED
#define POINT_IN_TRI(V0, U0, U1, U2)					\
{														\
	/* is T1 completly inside T2? */					\
	/* check if V0 is inside tri(U0,U1,U2) */			\
	float a  = U1[i1] - U0[i1];						    \
	float b  = -(U1[i0] - U0[i0]);					    \
	float c  = -a*U0[i0] - b*U0[i1];				    \
	float d0 = a*V0[i0] + b*V0[i1] + c;				    \
														\
	a  = U2[i1] - U1[i1];							    \
	b  = -(U2[i0] - U1[i0]);						    \
	c  = -a*U1[i0] - b*U1[i1];						    \
	const float d1 = a*V0[i0] + b*V0[i1] + c;		    \
														\
	a  = U0[i1] - U2[i1];							    \
	b  = -(U0[i0] - U2[i0]);						    \
	c  = -a*U2[i0] - b*U2[i1];						    \
	const float d2 = a*V0[i0] + b*V0[i1] + c;		    \
	if(d0*d1>0.0f)										\
	{													\
		if(d0*d2>0.0f) return true;						\
	}													\
}

//! TO BE DOCUMENTED
bool CoplanarTriTri(const float* n, 
                    const float* v0, const float* v1, const float* v2, 
                    const float* u0, const float* u1, const float* u2)
{
	float A[3];
	short i0,i1;
	/* first project onto an axis-aligned plane, that maximizes the area */
	/* of the triangles, compute indices: i0,i1. */
	A[0] = fabsf(n[0]);
	A[1] = fabsf(n[1]);
	A[2] = fabsf(n[2]);
	if(A[0]>A[1])
	{
		if(A[0]>A[2])
		{
			i0=1;      /* A[0] is greatest */
			i1=2;
		}
		else
		{
			i0=0;      /* A[2] is greatest */
			i1=1;
		}
	}
	else   /* A[0]<=A[1] */
	{
		if(A[2]>A[1])
		{
			i0=0;      /* A[2] is greatest */
			i1=1;
		}
		else
		{
			i0=0;      /* A[1] is greatest */
			i1=2;
		}
	}

	/* test all edges of triangle 1 against the edges of triangle 2 */
	EDGE_AGAINST_TRI_EDGES(v0, v1, u0, u1, u2);
	EDGE_AGAINST_TRI_EDGES(v1, v2, u0, u1, u2);
	EDGE_AGAINST_TRI_EDGES(v2, v0, u0, u1, u2);

	/* finally, test if tri1 is totally contained in tri2 or vice versa */
	POINT_IN_TRI(v0, u0, u1, u2);
	POINT_IN_TRI(u0, v0, v1, v2);

	return false;
}

//! TO BE DOCUMENTED
#define NEWCOMPUTE_INTERVALS(VV0, VV1, VV2, D0, D1, D2, D0D1, D0D2, A, B, C, X0, X1)	\
{																						\
	if(D0D1>0.0f)																		\
	{																					\
		/* here we know that D0D2<=0.0 */												\
		/* that is D0, D1 are on the same side, D2 on the other or on the plane */		\
		A=VV2; B=(VV0 - VV2)*D2; C=(VV1 - VV2)*D2; X0=D2 - D0; X1=D2 - D1;				\
	}																					\
	else if(D0D2>0.0f)																	\
	{																					\
		/* here we know that d0d1<=0.0 */												\
		A=VV1; B=(VV0 - VV1)*D1; C=(VV2 - VV1)*D1; X0=D1 - D0; X1=D1 - D2;				\
	}																					\
	else if(D1*D2>0.0f || D0!=0.0f)														\
	{																					\
		/* here we know that d0d1<=0.0 or that D0!=0.0 */								\
		A=VV0; B=(VV1 - VV0)*D0; C=(VV2 - VV0)*D0; X0=D0 - D1; X1=D0 - D2;				\
	}																					\
	else if(D1!=0.0f)																	\
	{																					\
		A=VV1; B=(VV0 - VV1)*D1; C=(VV2 - VV1)*D1; X0=D1 - D0; X1=D1 - D2;				\
	}																					\
	else if(D2!=0.0f)																	\
	{																					\
		A=VV2; B=(VV0 - VV2)*D2; C=(VV1 - VV2)*D2; X0=D2 - D0; X1=D2 - D1;				\
	}																					\
	else																				\
	{																					\
		/* triangles are coplanar */													\
		return CoplanarTriTri(N1, V0, V1, V2, U0, U1, U2);								\
	}																					\
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Triangle/triangle intersection test routine,
 *	by Tomas Moller, 1997.
 *	See article "A Fast Triangle-Triangle Intersection Test",
 *	Journal of Graphics Tools, 2(2), 1997
 *
 *	Updated June 1999: removed the divisions -- a little faster now!
 *	Updated October 1999: added {} to CROSS and SUB macros 
 *
 *	int NoDivTriTriIsect(float V0[3],float V1[3],float V2[3],
 *                      float U0[3],float U1[3],float U2[3])
 *
 *	\param		V0		[in] triangle 0, vertex 0
 *	\param		V1		[in] triangle 0, vertex 1
 *	\param		V2		[in] triangle 0, vertex 2
 *	\param		U0		[in] triangle 1, vertex 0
 *	\param		U1		[in] triangle 1, vertex 1
 *	\param		U2		[in] triangle 1, vertex 2
 *	\return		true if triangles overlap
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 三角形与三角形碰撞
bool math_triangle_triangle_collision(const FmVec3& v0, const FmVec3& v1, const FmVec3& v2,
	const FmVec3& u0, const FmVec3& u1, const FmVec3& u2)
{
    const float* V0 = (const float*)&v0;
    const float* V1 = (const float*)&v1;
    const float* V2 = (const float*)&v2;
    const float* U0 = (const float*)&u0;
    const float* U1 = (const float*)&u1;
    const float* U2 = (const float*)&u2;

    FmVec3 e1;
    FmVec3 e2;
    float* E1 = (float*)&e1;
    float* E2 = (float*)&e2;

    // Compute plane equation of triangle(V0,V1,V2)
    e1 = v1 - v0;
    e2 = v2 - v0;
    FmVec3 n1;
    FmVec3Cross(&n1, &e1, &e2);
    const float d1 = -FmVec3Dot(&n1, &v0);
    // Plane equation 1: N1.X+d1=0

    // Put U0,U1,U2 into plane equation 1 to compute signed distances to the plane
    float du0 = FmVec3Dot(&n1, &u0) + d1;
    float du1 = FmVec3Dot(&n1, &u1) + d1;
    float du2 = FmVec3Dot(&n1, &u2) + d1;

    // Coplanarity robustness check
#ifdef OPC_TRITRI_EPSILON_TEST
    if(fabsf(du0)<LOCAL_EPSILON) du0 = 0.0f;
    if(fabsf(du1)<LOCAL_EPSILON) du1 = 0.0f;
    if(fabsf(du2)<LOCAL_EPSILON) du2 = 0.0f;
#endif
    const float du0du1 = du0 * du1;
    const float du0du2 = du0 * du2;

    if(du0du1>0.0f && du0du2>0.0f)	// same sign on all of them + not equal 0 ?
    {
        return false;				// no intersection occurs
    }

    // Compute plane of triangle (U0,U1,U2)
    e1 = u1 - u0;
    e2 = u2 - u0;
    FmVec3 n2;
    FmVec3Cross(&n2, &e1, &e2);
    const float d2 = -FmVec3Dot(&n2, &v0);
    // plane equation 2: N2.X+d2=0

    // put V0,V1,V2 into plane equation 2
    float dv0 = FmVec3Dot(&n2, &v0) + d2;
    float dv1 = FmVec3Dot(&n2, &v1) + d2;
    float dv2 = FmVec3Dot(&n2, &v2) + d2;

#ifdef OPC_TRITRI_EPSILON_TEST
    if(fabsf(dv0)<LOCAL_EPSILON) dv0 = 0.0f;
    if(fabsf(dv1)<LOCAL_EPSILON) dv1 = 0.0f;
    if(fabsf(dv2)<LOCAL_EPSILON) dv2 = 0.0f;
#endif

    float* N1 = (float*)&n1;
    float* N2 = (float*)&n2;

    const float dv0dv1 = dv0 * dv1;
    const float dv0dv2 = dv0 * dv2;

    if(dv0dv1>0.0f && dv0dv2>0.0f)	// same sign on all of them + not equal 0 ?
    {
        return false;				// no intersection occurs
    }

    // Compute direction of intersection line
    FmVec3 vd;
    FmVec3Cross(&vd, &n1, &n2);
    const float* D = (const float*)&vd;

    // Compute and index to the largest component of D
    float max=fabsf(D[0]);
    short index=0;
    float bb=fabsf(D[1]);
    float cc=fabsf(D[2]);
    if(bb>max) max=bb,index=1;
    if(cc>max) max=cc,index=2;

    // This is the simplified projection onto L
    const float vp0 = V0[index];
    const float vp1 = V1[index];
    const float vp2 = V2[index];

    const float up0 = U0[index];
    const float up1 = U1[index];
    const float up2 = U2[index];

    // Compute interval for triangle 1
    float a,b,c,x0,x1;
    NEWCOMPUTE_INTERVALS(vp0,vp1,vp2,dv0,dv1,dv2,dv0dv1,dv0dv2,a,b,c,x0,x1);

    // Compute interval for triangle 2
    float d,e,f,y0,y1;
    NEWCOMPUTE_INTERVALS(up0,up1,up2,du0,du1,du2,du0du1,du0du2,d,e,f,y0,y1);

    const float xx=x0*x1;
    const float yy=y0*y1;
    const float xxyy=xx*yy;

    float isect1[2], isect2[2];

    float tmp=a*xxyy;
    isect1[0]=tmp+b*x1*yy;
    isect1[1]=tmp+c*x0*yy;

    tmp=d*xxyy;
    isect2[0]=tmp+e*xx*y1;
    isect2[1]=tmp+f*xx*y0;

    SORT(isect1[0],isect1[1]);
    SORT(isect2[0],isect2[1]);

    if(isect1[1]<isect2[0] || isect2[1]<isect1[0]) 
    {
        return false;
    }

    return true;
}

void math_rotate_bounding_box(const FmVec3* srcMin, 
	const FmVec3* srcMax, FmVec3* dstMin, FmVec3* dstMax, 
	const FmMat4* mtxRotateMatrix)
{
	Assert(srcMin != NULL);
	Assert(srcMax != NULL);
	Assert(dstMin != NULL);
	Assert(dstMax != NULL);
	Assert(mtxRotateMatrix != NULL);

	FmMat4 mtxPureRotateMatrix = *mtxRotateMatrix;

	mtxPureRotateMatrix._41 = 0.0F;
	mtxPureRotateMatrix._42 = 0.0F;
	mtxPureRotateMatrix._43 = 0.0F;

	FmVec3 box[8] = 
	{
		FmVec3(srcMin->x, srcMin->y, srcMin->z),
		FmVec3(srcMax->x, srcMin->y, srcMin->z),
		FmVec3(srcMin->x, srcMax->y, srcMin->z),
		FmVec3(srcMin->x, srcMin->y, srcMax->z),
		FmVec3(srcMax->x, srcMax->y, srcMax->z),
		FmVec3(srcMin->x, srcMax->y, srcMax->z),
		FmVec3(srcMax->x, srcMin->y, srcMax->z),
		FmVec3(srcMax->x, srcMax->y, srcMin->z),
	};

	/*
	FmVec3TransformCoordArray(box, sizeof(FmVec3), box, 
		sizeof(FmVec3), &mtxPureRotateMatrix, 8);
		*/
	for(int i = 0; i < 8 ; i++ )
		FmVec3TransformCoord( &box[i],&box[i],&mtxPureRotateMatrix);


	FmVec3 min = box[0];
	FmVec3 max = min;

	FmVec3Minimize(&min, &box[1], &min);
	FmVec3Minimize(&min, &box[2], &min);
	FmVec3Minimize(&min, &box[3], &min);
	FmVec3Minimize(&min, &box[4], &min);
	FmVec3Minimize(&min, &box[5], &min);
	FmVec3Minimize(&min, &box[6], &min);
	FmVec3Minimize(&min, &box[7], &min);
	FmVec3Maximize(&max, &box[1], &max);
	FmVec3Maximize(&max, &box[2], &max);
	FmVec3Maximize(&max, &box[3], &max);
	FmVec3Maximize(&max, &box[4], &max);
	FmVec3Maximize(&max, &box[5], &max);
	FmVec3Maximize(&max, &box[6], &max);
	FmVec3Maximize(&max, &box[7], &max);

	*dstMin = min;
	*dstMax = max;
}

void math_create_texture_ani_matrix(FmMat4* pOut, 
	float fOffsetU, float fOffsetV, float fRotateU, float fRotateV, 
	float fRotateW, float fTilingU, float fTilingV)
{
	Assert(pOut != NULL);

	FmMat4 mtxTrans, mtxRotateU, mtxRotateV, mtxRotateW, mtxScale;

	FmMatrixIdentity(&mtxTrans);

	mtxRotateU = mtxRotateV = mtxRotateW = mtxScale = mtxTrans;

	// Translate.
	mtxTrans._41 = -fOffsetU;
	mtxTrans._42 = fOffsetV;

	// Rotate.
	float sinu = sinf(fRotateU);
	float cosu = cosf(fRotateU);
	float sinv = sinf(fRotateV);
	float cosv = cosf(fRotateV);
	float sinw = sinf(-fRotateW);
	float cosw = cosf(-fRotateW);

	mtxRotateU._11 = cosu;
	mtxRotateU._12 = sinu;
	mtxRotateU._21 = -sinu;
	mtxRotateU._22 = cosu;

	mtxRotateV._11 = cosv;
	mtxRotateV._12 = sinv;
	mtxRotateV._21 = -sinv;
	mtxRotateV._22 = cosv;

	mtxRotateW._11 = cosw;
	mtxRotateW._12 = sinw;
	mtxRotateW._21 = -sinw;
	mtxRotateW._22 = cosw;

	// Scale.
	mtxScale._11 = fTilingU;
	mtxScale._22 = fTilingV;

	// Final.
	FmMatrixMultiply(pOut, &mtxScale, &mtxRotateU);
	FmMatrixMultiply(pOut, &mtxRotateV, (FmMat4*)pOut);
	FmMatrixMultiply(pOut, &mtxRotateW, (FmMat4*)pOut);
	FmMatrixMultiply(pOut, &mtxTrans, (FmMat4*)pOut);

	// Final
	pOut->_31 = pOut->_41;
	pOut->_32 = pOut->_42;

	return;
}

bool math_intersect_bb_and_planes(const FmVec3* bbMin, 
	const FmVec3* bbMax, const FmVec4* planes, 
	unsigned int nPlaneCnt)
{
	Assert(bbMin != NULL);
	Assert(bbMax != NULL);
	Assert(planes != NULL);

	FmVec3 center;

	center.x = (bbMin->x + bbMax->x) * 0.5F;
	center.y = (bbMin->y + bbMax->y) * 0.5F;
	center.z = (bbMin->z + bbMax->z) * 0.5F;

	float sx = center.x - bbMin->x;
	float sy = center.y - bbMin->y;
	float sz = center.z - bbMin->z;
	float radius_square = sx * sx + sy * sy + sz * sz;

	for (unsigned int i = 0; i < nPlaneCnt; ++i)
	{
		float distance = center.x * planes[i].x + center.y * planes[i].y 
			+ center.z * planes[i].z + planes[i].w;

		if ((distance * distance) > radius_square)
		{
			return false;
		}
	}

	return true;
}

int math_intersect_bb_and_sphere(const FmVec3* srcMin, 
	const FmVec3* srcMax, const FmVec3* center, float radius)
{
	Assert(srcMin != NULL);
	Assert(srcMax != NULL);
	Assert(center != NULL);

	if ((center->x >= srcMin->x - radius) && (center->x <= srcMax->x + radius)
		&& (center->z >= srcMin->z - radius) && (center->z <= srcMax->z + radius)
		&& (center->y >= srcMin->y - radius) && (center->y <= srcMax->y + radius))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

///Swap numbers
#define YD_SWAP_NUMBERS(a,b){ \
    a = a+b; \
    b = a-b; \
    a = a-b; \
}\

#define TEST_CROSS_EDGE_BOX_MCR(edge, absolute_edge, pointa, pointb, _extend, i_dir_0, i_dir_1, i_comp_0, i_comp_1)\
{\
    const float dir0 = -edge[i_dir_0];\
    const float dir1 = edge[i_dir_1];\
    float pmin = pointa[i_comp_0]*dir0 + pointa[i_comp_1]*dir1;\
    float pmax = pointb[i_comp_0]*dir0 + pointb[i_comp_1]*dir1;\
    if(pmin>pmax)\
    {\
        YD_SWAP_NUMBERS(pmin,pmax); \
    }\
    const float abs_dir0 = absolute_edge[i_dir_0];\
    const float abs_dir1 = absolute_edge[i_dir_1];\
    const float rad = _extend[i_comp_0] * abs_dir0 + _extend[i_comp_1] * abs_dir1;\
    if(pmin > rad || -rad > pmax) return false;\
}\

#define TEST_CROSS_EDGE_BOX_X_AXIS_MCR(edge, absolute_edge, pointa, pointb, _extend)\
{\
    TEST_CROSS_EDGE_BOX_MCR(edge, absolute_edge ,pointa, pointb, _extend, 2, 1, 1, 2);\
}\

#define TEST_CROSS_EDGE_BOX_Y_AXIS_MCR(edge, absolute_edge, pointa, pointb ,_extend)\
{\
    TEST_CROSS_EDGE_BOX_MCR(edge, absolute_edge, pointa, pointb, _extend, 0, 2, 2, 0);\
}\

#define TEST_CROSS_EDGE_BOX_Z_AXIS_MCR(edge, absolute_edge, pointa, pointb, _extend)\
{\
    TEST_CROSS_EDGE_BOX_MCR(edge, absolute_edge, pointa, pointb, _extend, 1, 0, 0, 1);\
}\

int math_intersect_bb_and_triangle(const FmVec3* vMin, 
	const FmVec3* vMax, const FmVec3* vTriangle1, const FmVec3* vTriangle2, const FmVec3* vTriangle3)
{
    FmVec3 _vCenter = (*vMax + *vMin) * 0.5f;
    FmVec3 _vHalfExtents = (*vMax - *vMin) * 0.5f;

    const FmVec3 _v1(*vTriangle1 - _vCenter);
    const FmVec3 _v2(*vTriangle2 - _vCenter);
    const FmVec3 _v3(*vTriangle3 - _vCenter);

    float* vCenter = (float*)&_vCenter;
    float* vHalfExtents = (float*)&_vHalfExtents;
    float* v1 = (float*)&_v1;
    float* v2 = (float*)&_v2;
    float* v3 = (float*)&_v3;

    float diff[3];
    float abs_diff[3];

    diff[0] = (_v2.x - _v1.x);
    diff[1] = (_v2.y - _v1.y);
    diff[2] = (_v2.z - _v1.z);
    abs_diff[0] = fabsf(diff[0]);
    abs_diff[1] = fabsf(diff[1]);
    abs_diff[2] = fabsf(diff[2]);
    //Test With X axis
    TEST_CROSS_EDGE_BOX_X_AXIS_MCR(diff, abs_diff, v1, v3, vHalfExtents);
    //Test With Y axis                              
    TEST_CROSS_EDGE_BOX_Y_AXIS_MCR(diff, abs_diff, v1, v3, vHalfExtents);
    //Test With Z axis                              
    TEST_CROSS_EDGE_BOX_Z_AXIS_MCR(diff, abs_diff, v1, v3, vHalfExtents);
 
    diff[0] = (_v3.x - _v2.x);
    diff[1] = (_v3.y - _v2.y);
    diff[2] = (_v3.z - _v2.z);
    abs_diff[0] = fabsf(diff[0]);
    abs_diff[1] = fabsf(diff[1]);
    abs_diff[2] = fabsf(diff[2]);               
    //Test With X axis                              
    TEST_CROSS_EDGE_BOX_X_AXIS_MCR(diff, abs_diff, v2, v1, vHalfExtents);
    //Test With Y axis                              
    TEST_CROSS_EDGE_BOX_Y_AXIS_MCR(diff, abs_diff, v2, v1, vHalfExtents);
    //Test With Z axis                              
    TEST_CROSS_EDGE_BOX_Z_AXIS_MCR(diff, abs_diff, v2, v1, vHalfExtents);

    diff[0] = (_v1.x - _v3.x);
    diff[1] = (_v1.y - _v3.y);
    diff[2] = (_v1.z - _v3.z);
    abs_diff[0] = fabsf(diff[0]);
    abs_diff[1] = fabsf(diff[1]);
    abs_diff[2] = fabsf(diff[2]);                  
    //Test With X axis                              
    TEST_CROSS_EDGE_BOX_X_AXIS_MCR(diff, abs_diff, v3, v2, vHalfExtents);
    //Test With Y axis                              
    TEST_CROSS_EDGE_BOX_Y_AXIS_MCR(diff, abs_diff, v3, v2, vHalfExtents);
    //Test With Z axis                              
    TEST_CROSS_EDGE_BOX_Z_AXIS_MCR(diff, abs_diff, v3, v2, vHalfExtents);

    return true;
}

void CreateTextureAniMatrix(FmMat4* pOut, 
		float fOffsetU, float fOffsetV, float fRotateU, float fRotateV, 
	float fRotateW, float fTilingU, float fTilingV)
{
	Assert(pOut != NULL);

	FmMat4 mtxTrans, mtxRotateU, mtxRotateV, mtxRotateW, mtxScale;

	FmMat4Identity(&mtxTrans);

	mtxRotateU = mtxRotateV = mtxRotateW = mtxScale = mtxTrans;

	// Translate.
	mtxTrans._41 = -fOffsetU;
	mtxTrans._42 = fOffsetV;

	// Rotate.
	float sinu = sinf(fRotateU);
	float cosu = cosf(fRotateU);
	float sinv = sinf(fRotateV);
	float cosv = cosf(fRotateV);
	float sinw = sinf(-fRotateW);
	float cosw = cosf(-fRotateW);

	mtxRotateU._11 = cosu;
	mtxRotateU._12 = sinu;
	mtxRotateU._21 = -sinu;
	mtxRotateU._22 = cosu;

	mtxRotateV._11 = cosv;
	mtxRotateV._12 = sinv;
	mtxRotateV._21 = -sinv;
	mtxRotateV._22 = cosv;

	mtxRotateW._11 = cosw;
	mtxRotateW._12 = sinw;
	mtxRotateW._21 = -sinw;
	mtxRotateW._22 = cosw;

	// Scale.
	mtxScale._11 = fTilingU;
	mtxScale._22 = fTilingV;

	// Final.
	FmMat4Multiply(pOut, &mtxScale, &mtxRotateU);
	FmMat4Multiply(pOut, &mtxRotateV, (FmMat4*)pOut);
	FmMat4Multiply(pOut, &mtxRotateW, (FmMat4*)pOut);
	FmMat4Multiply(pOut, &mtxTrans, (FmMat4*)pOut);

	// Final
	pOut->_31 = pOut->_41;
	pOut->_32 = pOut->_42;

	return;
}