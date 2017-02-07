
#ifndef _MATH_3D_H
#define _MATH_3D_H

 


#include "../visual/fm_header.h"

// 球体与三角形碰撞
bool SphereTriangleCollision(const FmVec3& center, float radius,
	const FmVec3& v1, const FmVec3& v2, const FmVec3& v3);
// 球体与三角形包围盒碰撞
bool SphereTriangleAABBCollision(const FmVec3& center, float radius,
	const FmVec3& v1, const FmVec3& v2, const FmVec3& v3);
 
#endif // _MATH_3D_H
