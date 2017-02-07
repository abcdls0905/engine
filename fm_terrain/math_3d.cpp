
#include "math_3d.h"
#include <math.h>
#include <float.h>
#include "../public/portable.h"

#define PI 3.1415926535897932

// 浮点数绝对值
static inline float Absolute(float num)
{
	if (num < 0)
	{
		return (0 - num);
	}

	return num;
}

// 三角面法线
static FmVec3 Normal(const FmVec3& v1, const FmVec3& v2, 
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
static inline float PlaneDistance(const FmVec3& Normal, 
	const FmVec3& Point)
{	
	return -((Normal.x * Point.x) + (Normal.y * Point.y) 
		+ (Normal.z * Point.z));
}

// 向量的夹角
static double AngleBetweenVectors(const FmVec3& Vector1, 
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

static bool InsidePolygon(const FmVec3& vIntersection, 
	const FmVec3& v1, const FmVec3& v2, const FmVec3& v3)
{
	const double MATCH_FACTOR = 0.999999;	
	
	double Angle = 0.0;
	
	FmVec3 vA;
	FmVec3 vB;
	
	vA = v1 - vIntersection;
	vB = v2 - vIntersection;
											
	Angle += AngleBetweenVectors(vA, vB);
											
	if (Angle >= (MATCH_FACTOR * (2.0 * PI)) )
	{
		return true;
	}
		
	vA = v2 - vIntersection;
	vB = v3 - vIntersection;
											
	Angle += AngleBetweenVectors(vA, vB);
											
	if (Angle >= (MATCH_FACTOR * (2.0 * PI)) )
	{
		return true;
	}
		
	vA = v3 - vIntersection;
	vB = v1 - vIntersection;
											
	Angle += AngleBetweenVectors(vA, vB);
											
	if (Angle >= (MATCH_FACTOR * (2.0 * PI)) )
	{
		return true;
	}
		
	return false;
}

static FmVec3 ClosestPointOnLine(const FmVec3& vA, 
	const FmVec3& vB, const FmVec3& vPoint)
{
	FmVec3 vVector1 = vPoint - vA;
    FmVec3 vVector2 = vB - vA;

	FmVec3Normalize(&vVector2, &vVector2);

	FmVec3 vecSub = vA - vB;
	float d = FmVec3Length(&vecSub);
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

static bool ClassifySphere(const FmVec3& vCenter, 
	const FmVec3& vNormal, const FmVec3& vPoint, float radius, 
	float& distance)
{
	// First we need to find the distance our polygon plane is from the origin.
	float d = (float)PlaneDistance(vNormal, vPoint);

	// Here we use the famous distance formula to find the distance the center point
	distance = vNormal.x * vCenter.x + vNormal.y * vCenter.y 
		+ vNormal.z * vCenter.z + d;

	if (Absolute(distance) < radius)
	{
		return true;
	}

	return false;
}

static bool EdgeSphereCollision(const FmVec3& vCenter, float radius, 
	const FmVec3& v1, const FmVec3& v2, const FmVec3& v3)
{
	FmVec3 vPoint = ClosestPointOnLine(v1, v2, vCenter);

	FmVec3 vec3Sub = vPoint - vCenter;
	float distance = FmVec3Length(&vec3Sub);

	if (distance < radius)
	{
		return true;
	}

	vPoint = ClosestPointOnLine(v2, v3, vCenter);

	vec3Sub =  vPoint - vCenter;
	distance = FmVec3Length(&vec3Sub);

	if (distance < radius)
	{
		return true;
	}

	vPoint = ClosestPointOnLine(v3, v1, vCenter);

	vec3Sub = vPoint - vCenter;
	distance = FmVec3Length(&vec3Sub);

	if (distance < radius)
	{
		return true;
	}

	return false;
}

bool SphereTriangleCollision(const FmVec3& vCenter, float radius,
	const FmVec3& v1, const FmVec3& v2, const FmVec3& v3)
{
	// 1) STEP ONE - Finding the sphere's classification
	FmVec3 vNormal = Normal(v1, v2, v3);

	float distance = 0.0F;

	if (ClassifySphere(vCenter, vNormal, v1, radius, distance))
	{
		// 2) STEP TWO - Finding the psuedo intersection point on the plane
		FmVec3 vOffset = vNormal * distance;
		FmVec3 vPosition = vCenter - vOffset;

		// 3) STEP THREE - Check if the intersection point is inside the polygons perimeter
		if (InsidePolygon(vPosition, v1, v2, v3))
		{
			return true;
		}
		else
		{
			// 4) STEP FOUR - Check the sphere to see if it intersects the polygon edges
			if (EdgeSphereCollision(vCenter, radius, v1, v2, v3))
			{
				return true;
			}
		}
	}

	return false;
}

/*
// 点是否包含在盒子内
static bool PointInBox(const FmVec4& v_min, const FmVec4& v_max, 
	const FmVec4& v_pos)
{
	return (v_pos.x >= v_min.x) && (v_pos.x <= v_max.x)
		&& (v_pos.z >= v_min.z) && (v_pos.z <= v_max.z)
		&& (v_pos.y >= v_min.y) && (v_pos.y <= v_max.y);
}

// 两个盒子是否相交
static bool BoxIntersect(const FmVec4& v_min_1, 
	const FmVec4& v_max_1, const FmVec4& v_min_2, 
	const FmVec4& v_max_2)
{
	return (v_max_1.x >= v_min_2.x) && (v_max_2.x >= v_min_1.x)
		&& (v_max_1.z >= v_min_2.z) && (v_max_2.z >= v_min_1.z)
		&& (v_max_1.y >= v_min_2.y) && (v_max_2.y >= v_min_1.y);
}

bool BoundBoxTriangleCollision(const FmVec4& v_min, 
	const FmVec4& v_max, const FmVec4& v1, const FmVec4& v2, 
	const FmVec4& v3)
{
	if (PointInBox(v_min, v_max, v1) || PointInBox(v_min, v_max, v2) 
		|| PointInBox(v_min, v_max, v3))
	{
		// 三角形的顶点在包围盒中
		return true;
	}

	// 获得三角形的包围盒
	FmVec4 triangle_min = v1;
	FmVec4 triangle_max = v1;

	FmVec3Minimize(&triangle_min, &triangle_min, &v2);
	FmVec3Minimize(&triangle_min, &triangle_min, &v3);
	FmVec3Maximize(&triangle_max, &triangle_max, &v2);
	FmVec3Maximize(&triangle_max, &triangle_max, &v3);

	if (!BoxIntersect(v_min, v_max, triangle_min, triangle_max))
	{
		// 包围盒不相交
		return false;
	}

	// 三角形的线段是否与包围盒相交
	if (FmBoxBoundProbe(&v_min, &v_max, &v1, &(v2 - v1))
		&& FmBoxBoundProbe(&v_min, &v_max, &v2, &(v1 - v2)))
	{
		return true;
	}
	
	if (FmBoxBoundProbe(&v_min, &v_max, &v2, &(v3 - v2))
		&& FmBoxBoundProbe(&v_min, &v_max, &v3, &(v2 - v3)))
	{
		return true;
	}

	if (FmBoxBoundProbe(&v_min, &v_max, &v3, &(v1 - v3))
		&& FmBoxBoundProbe(&v_min, &v_max, &v1, &(v3 - v1)))
	{
		return true;
	}

	return false;
}
*/

/*
#define in(a) ((unsigned int&) a)

static bool CheckPointInTriangle(const FmVec4& point,
	const FmVec4& pa, const FmVec4& pb, const FmVec4& pc)
{
	FmVec4 e10 = pb - pa;
	FmVec4 e20 = pc - pa;

	float a = FmVec3Dot(&e10, &e10);
	float b = FmVec3Dot(&e10, &e20);
	float c = FmVec3Dot(&e20, &e20);

	float ac_bb = (a * c) - (b * b);

	FmVec4 vp(point.x - pa.x, point.y - pa.y, point.z - pa.z);

	float d = FmVec3Dot(&vp, &e10);
	float e = FmVec3Dot(&vp, &e20);

	float x = (d * c) - (e * b);
	float y = (e * a)-(d * b);
	float z = x + y - ac_bb;

	return ((in(z) & ~(in(x) | in(y))) & 0x80000000) != 0;
}

static FmVec4 ClosestPointOnLineF(const FmVec4& vA, 
	const FmVec4& vB, const FmVec4& vPoint)
{
	FmVec4 vVector1 = vPoint - vA;
	FmVec4 vVector2 = vB - vA;

	FmVec3Normalize(&vVector2, &vVector2);

	float t = FmVec3Dot(&vVector2, &vVector1);

	if (t <= 0) 
	{
		return vA;
	}

	float dist_sq = FmVec3LengthSq(&(vA - vB));

	if ((t * t) >= dist_sq) 
	{
		return vB;
	}

	FmVec4 vVector3 = vVector2 * t;
	FmVec4 vClosestPoint = vA + vVector3;

	return vClosestPoint;
}

static bool EdgeSphereCollisionF(const FmVec4& vCenter, float radius, 
	const FmVec4& v1, const FmVec4& v2, const FmVec4& v3)
{
	FmVec4 vPoint = ClosestPointOnLineF(v1, v2, vCenter);

	float dist_sq = FmVec3LengthSq(&(vPoint - vCenter));

	if (dist_sq < (radius * radius))
	{
		return true;
	}

	vPoint = ClosestPointOnLineF(v2, v3, vCenter);

	dist_sq = FmVec3LengthSq(&(vPoint - vCenter));

	if (dist_sq < (radius * radius))
	{
		return true;
	}

	vPoint = ClosestPointOnLineF(v3, v1, vCenter);

	dist_sq = FmVec3LengthSq(&(vPoint - vCenter));

	if (dist_sq < (radius * radius))
	{
		return true;
	}

	return false;
}

bool SphereAABBTriangleCollision(const FmVec4& vCenter, float radius,
	const FmVec4& v1, const FmVec4& v2, const FmVec4& v3)
{
	// 1) STEP ONE - Finding the sphere's classification
	FmPlane plane;

	FmPlaneFromPoints(&plane, &v1, &v2, &v3);
	
	float distance = FmPlaneDotCoord(&plane, &vCenter);

	if (fabsf(distance) > radius)
	{
		return false;
	}

	// 2) STEP TWO - Finding the psuedo intersection point on the plane
	FmVec4 vOffset = FmVec4(plane.a, plane.b, plane.c) * distance;
	FmVec4 vPosition = vCenter - vOffset;

	// 3) STEP THREE - Check if the intersection point is inside the polygons perimeter
	if (CheckPointInTriangle(vPosition, v1, v2, v3))
	{
		return true;
	}

	// 4) STEP FOUR - Check the sphere to see if it intersects the polygon edges
	if (EdgeSphereCollisionF(vCenter, radius, v1, v2, v3))
	{
		return true;
	}

	return false;
}
*/

bool SphereTriangleAABBCollision(const FmVec3& center, float radius,
	const FmVec3& v1, const FmVec3& v2, const FmVec3& v3)
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
