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

#ifndef BULLET_DYNAMICS_COMMON_H
#define BULLET_DYNAMICS_COMMON_H

///Common headerfile includes for Bullet Dynamics, including Collision Detection
#include "bt_bullet_collision_common.h"

#include "bullet_dynamics/dynamics/bt_discrete_dynamics_world.h"

#include "bullet_dynamics/dynamics/bt_rigid_body.h"

#include "bullet_dynamics/constraint_solver/bt_point_2_point_constraint.h"
#include "bullet_dynamics/constraint_solver/bt_hinge_constraint.h"
#include "bullet_dynamics/constraint_solver/bt_cone_twist_constraint.h"
#include "bullet_dynamics/constraint_solver/bt_generic_6_dof_constraint.h"
#include "bullet_dynamics/constraint_solver/bt_slider_constraint.h"
#include "bullet_dynamics/constraint_solver/bt_generic_6_dof_spring_constraint.h"
#include "bullet_dynamics/constraint_solver/bt_universal_constraint.h"
#include "bullet_dynamics/constraint_solver/bt_hinge_2_constraint.h"
#include "bullet_dynamics/constraint_solver/bt_gear_constraint.h"
#include "bullet_dynamics/constraint_solver/bt_fixed_constraint.h"


#include "bullet_dynamics/constraint_solver/bt_sequential_impulse_constraint_solver.h"


///Vehicle simulation, with wheel contact simulated by raycasts
#include "bullet_dynamics/vehicle/bt_raycast_vehicle.h"






#endif //BULLET_DYNAMICS_COMMON_H

