
/****************************************************************

  File name   :  CollisionShape.cpp
  Author      :  Ò¶·å
  Version     :  1.0a
  Create Date :  2013/10/23  
  Description :  Åö×²ÐÎ

*****************************************************************/

// --------------------------------------------------------------------------------------

// Collision Shapes
#include "bullet_collision/collision_shapes/bt_collision_shape.h"
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
#include "bullet_collision/collision_shapes/bt_triangle_index_vertex_array.h"

#include "common.h"
#include "physics_bullet.h"
#include "collision_shape.h"

#include <assert.h>

// --------------------------------------------------------------------------------------

CollisionShape::CollisionShape(btCollisionShape* pBtShape, YeCollisionShapeType type, long heightAxis)
{
    assert(pBtShape);
    m_pBtCollisionShape = pBtShape;
    m_type = type;
    m_heightAxis = heightAxis;
    memset(m_params, 0, sizeof(m_params));
    m_next = NULL;
    m_prev = NULL;
    m_ref = 1;
}

CollisionShape::~CollisionShape()
{

}

void    CollisionShape::Release()
{
    m_ref--;
    if (m_ref == 0)
    {
        if (m_type == YE_CST_COMPOUND)
        {
            btCompoundShape* pCompoundShape = (btCompoundShape*)m_pBtCollisionShape;
            int count = pCompoundShape->getNumChildShapes();
            for (int i = 0; i < count; i++)
            {
                btCollisionShape* pChildShape = pCompoundShape->getChildShape(i);
                g_physicsBulletPtr->ReleaseCollisionShape(pChildShape);
            }
        }
        else if (m_type == YE_CST_STATIC_SCALED_MESH)
        {
            btScaledBvhTriangleMeshShape* pScaledBvhTriangleMeshShape = (btScaledBvhTriangleMeshShape*)m_pBtCollisionShape;
            btBvhTriangleMeshShape*	pBvhTriangleMeshShape = pScaledBvhTriangleMeshShape->getChildShape();
            g_physicsBulletPtr->ReleaseCollisionShape(pBvhTriangleMeshShape);
        }
        else if (m_type == YE_CST_DYNAMIC_MESH || m_type == YE_CST_STATIC_MESH)
        {
            btTriangleIndexVertexArray* ptr;
            memcpy(&ptr, &m_params[0], sizeof(void*));
            delete(ptr);
        }
        g_physicsBulletPtr->DestoryCollisionShape(this);
    }
}

void    CollisionShape::AddRef()
{
    m_ref++;
}

void    CollisionShape::GetAabb(const FmMat4& mtx, FmVec3& aabbMin, FmVec3& aabbMax) const
{
    btTransform tm;
    tm.setFromOpenGLMatrix((float*)&mtx);
    btVector3 vMin;
    btVector3 vMax;
    m_pBtCollisionShape->getAabb(tm, vMin, vMax);
    aabbMin = BtVector3ToFmVec3(vMin);
    aabbMax = BtVector3ToFmVec3(vMax);
}

void    CollisionShape::GetBoundingSphere(FmVec3& center, float& radius) const
{
    btVector3 v;
    m_pBtCollisionShape->getBoundingSphere(v, radius);
    center = BtVector3ToFmVec3(v);
}

void    CollisionShape::SetMargin(float margin)
{
    m_pBtCollisionShape->setMargin(margin);
}

float   CollisionShape::GetMargin() const
{
    return m_pBtCollisionShape->getMargin();
}

void    CollisionShape::SetScaling(const FmVec3& vScale)
{
    btVector3 v = FmVec3ToBtVector3(vScale);
    m_pBtCollisionShape->setLocalScaling(v);
}

void    CollisionShape::GetScaling(FmVec3& vScale)
{
    btVector3 v = m_pBtCollisionShape->getLocalScaling();
    vScale = BtVector3ToFmVec3(v);
}

void    CollisionShape::SetUserPointer(void* userPtr)
{
    m_pBtCollisionShape->setUserPointer(userPtr);
}

void*   CollisionShape::GetUserPointer() const
{
    return m_pBtCollisionShape->getUserPointer();
}
