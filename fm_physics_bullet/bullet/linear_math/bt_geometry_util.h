/*
Copyright (c) 2003-2006 Gino van den Bergen / Erwin Coumans  http://continuousphysics.com/Bullet/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/


#ifndef BT_GEOMETRY_UTIL_H
#define BT_GEOMETRY_UTIL_H

#include "bt_vector_3.h"
#include "bt_aligned_object_array.h"

///The btGeometryUtil helper class provides a few methods to convert between plane equations and vertices.
class btGeometryUtil
{
	public:
	
	    // 求点集的包围面
		static void	getPlaneEquationsFromVertices(btAlignedObjectArray<btVector3>& vertices, btAlignedObjectArray<btVector3>& planeEquationsOut );

        // 平面集合的交点(三个平面可以有一个交点)
		static void	getVerticesFromPlaneEquations(const btAlignedObjectArray<btVector3>& planeEquations , btAlignedObjectArray<btVector3>& verticesOut );
	
        // 这它妈的没实现
		static bool	isInside(const btAlignedObjectArray<btVector3>& vertices, const btVector3& planeNormal, btScalar	margin);
		
        // 点在所有的面一侧
		static bool	isPointInsidePlanes(const btAlignedObjectArray<btVector3>& planeEquations, const btVector3& point, btScalar	margin);

        // 所有的点都在面的一侧
		static bool	areVerticesBehindPlane(const btVector3& planeNormal, const btAlignedObjectArray<btVector3>& vertices, btScalar	margin);

};


#endif //BT_GEOMETRY_UTIL_H

