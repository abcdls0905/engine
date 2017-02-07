/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2006 Erwin Coumans  http://continuousphysics.com/Bullet/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef BULLET_COLLISION_COMMON_H
#define BULLET_COLLISION_COMMON_H

///Common headerfile includes for Bullet Collision Detection

///Bullet's btCollisionWorld and btCollisionObject definitions
#include "bullet_collision/collision_dispatch/bt_collision_world.h"
#include "bullet_collision/collision_dispatch/bt_collision_object.h"

///Collision Shapes
#include "bullet_collision/collision_shapes/bt_box_shape.h"
#include "bullet_collision/collision_shapes/bt_sphere_shape.h"
#include "bullet_collision/collision_shapes/bt_capsule_shape.h"
#include "bullet_collision/collision_shapes/bt_cylinder_shape.h"
#include "bullet_collision/collision_shapes/bt_cone_shape.h"
#include "bullet_collision/collision_shapes/bt_static_plane_shape.h"
#include "bullet_collision/collision_shapes/bt_convex_hull_shape.h"
#include "bullet_collision/collision_shapes/bt_triangle_mesh.h"
#include "bullet_collision/collision_shapes/bt_convex_triangle_mesh_shape.h"
#include "bullet_collision/collision_shapes/bt_bvh_triangle_mesh_shape.h"
#include "bullet_collision/collision_shapes/bt_scaled_bvh_triangle_mesh_shape.h"
#include "bullet_collision/collision_shapes/bt_triangle_mesh_shape.h"
#include "bullet_collision/collision_shapes/bt_triangle_index_vertex_array.h"
#include "bullet_collision/collision_shapes/bt_compound_shape.h"
#include "bullet_collision/collision_shapes/bt_tetrahedron_shape.h"
#include "bullet_collision/collision_shapes/bt_empty_shape.h"
#include "bullet_collision/collision_shapes/bt_multi_sphere_shape.h"
#include "bullet_collision/collision_shapes/bt_uniform_scaling_shape.h"

///Narrowphase Collision Detector
#include "bullet_collision/collision_dispatch/bt_sphere_sphere_collision_algorithm.h"

//#include "BulletCollision/CollisionDispatch/btSphereBoxCollisionAlgorithm.h"
#include "bullet_collision/collision_dispatch/bt_default_collision_configuration.h"

///Dispatching and generation of collision pairs (broadphase)
#include "bullet_collision/collision_dispatch/bt_collision_dispatcher.h"
#include "bullet_collision/broadphase_collision/bt_simple_broadphase.h"
#include "bullet_collision/broadphase_collision/bt_axis_sweep_3.h"
#include "bullet_collision/broadphase_collision/bt_multi_sap_broadphase.h"
#include "bullet_collision/broadphase_collision/bt_dbvt_broadphase.h"

///Math library & Utils
#include "linear_math/bt_quaternion.h"
#include "linear_math/bt_transform.h"
#include "linear_math/bt_default_motion_state.h"
#include "linear_math/bt_quickprof.h"
#include "linear_math/bt_i_debug_draw.h"
#include "linear_math/bt_serializer.h"


#endif //BULLET_COLLISION_COMMON_H

