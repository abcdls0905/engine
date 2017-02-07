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

#include "bt_soft_soft_collision_algorithm.h"
#include "bullet_collision/collision_dispatch/bt_collision_dispatcher.h"
#include "bullet_collision/collision_shapes/bt_box_shape.h"
#include "bullet_collision/collision_dispatch/bt_collision_object.h"
#include "bullet_soft_body/bt_soft_body_solvers.h"
#include "bt_soft_body.h"
#include "bullet_collision/collision_dispatch/bt_collision_object_wrapper.h"

#define USE_PERSISTENT_CONTACTS 1

btSoftSoftCollisionAlgorithm::btSoftSoftCollisionAlgorithm(btPersistentManifold* /*mf*/,const btCollisionAlgorithmConstructionInfo& ci,const btCollisionObjectWrapper* /*obj0*/,const btCollisionObjectWrapper* /*obj1*/)
: btCollisionAlgorithm(ci)
//m_ownManifold(false),
//m_manifoldPtr(mf)
{
}

btSoftSoftCollisionAlgorithm::~btSoftSoftCollisionAlgorithm()
{
}

void btSoftSoftCollisionAlgorithm::processCollision (const btCollisionObjectWrapper* body0Wrap,const btCollisionObjectWrapper* body1Wrap,const btDispatcherInfo& /*dispatchInfo*/,btManifoldResult* /*resultOut*/)
{
	btSoftBody* soft0 =	(btSoftBody*)body0Wrap->getCollisionObject();
	btSoftBody* soft1 =	(btSoftBody*)body1Wrap->getCollisionObject();
	soft0->getSoftBodySolver()->processCollision(soft0, soft1);
}

btScalar btSoftSoftCollisionAlgorithm::calculateTimeOfImpact(btCollisionObject* /*body0*/,btCollisionObject* /*body1*/,const btDispatcherInfo& /*dispatchInfo*/,btManifoldResult* /*resultOut*/)
{
	//not yet
	return 1.f;
}
