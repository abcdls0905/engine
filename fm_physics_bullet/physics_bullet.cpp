
/****************************************************************

  File name   :  PhysicsBullet.cpp
  Author      :  叶峰
  Version     :  1.0a
  Create Date :  2013/10/22   
  Description :  Bullet物理系统

*****************************************************************/

// INCLUDES -----------------------------------------------------------------------------

#include "../public/macros.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../public/module.h"
#include "../visual/i_primitive_3d_drawer.h"
#include "../visual/i_render.h"
#include "../visual/i_painter.h"
#include "../public/portable.h"

#include "bt_bullet_dynamics_common.h"
#include "bullet_collision/gimpact/bt_g_impact_shape.h"
#include "bullet_collision/gimpact/bt_g_impact_collision_algorithm.h"
#include "bullet_collision/collision_shapes/bt_heightfield_terrain_shape.h"
#include "bullet_collision/collision_shapes/bt_convex_polyhedron.h"
#include "bullet_collision/collision_shapes/bt_shape_hull.h"
#include "bullet_collision/collision_dispatch/bt_collision_object_wrapper.h"
#include "bullet_collision/narrow_phase_collision/bt_manifold_point.h"

#include "collision_shape.h"
#include "rigid_body.h"
#include "constraint.h"
#include "ragdoll.h"
#include "my_motion_state.h"
#include "physics_bullet.h"
#include "common.h"

// --------------------------------------------------------------------------------------

#define performance_time        Port_GetPerformanceTime

#define SAFE_DELETE(p)          { if(p) { delete (p);         (p) = NULL; } }

// --------------------------------------------------------------------------------------

PhysicsBullet* g_physicsBulletPtr = NULL;

// --------------------------------------------------------------------------------------

IVisBase* GetVisBase(const PERSISTID& id)
{
    if (!g_physicsBulletPtr)
    {
        return NULL;
    }

    IEntity* pEntity = g_physicsBulletPtr->GetCore()->GetEntity(id);

    if (NULL == pEntity)
    {
        return NULL;
    }

    if (!pEntity->GetEntInfo()->IsKindOf("IVisBase"))
    {
        return NULL;
    }

    return (IVisBase*)pEntity;
}

// --------------------------------------------------------------------------------------

DECLARE_FUNCTION_INTERFACE(PhysicsBullet);

// --------------------------------------------------------------------------------------

///This is to allow MaterialCombiner/Custom Friction/Restitution values
extern ContactAddedCallback		gContactAddedCallback;

bool PhysicsBullet::ContactAddedCallback(btManifoldPoint& cp, 
    const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, 
    const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
{
    const btCollisionObject* ptr0 = colObj0Wrap->getCollisionObject();
    const btCollisionObject* ptr1 = colObj1Wrap->getCollisionObject();

    RigidBody* pRB0 = (RigidBody*)ptr0->getUserPointer();
    RigidBody* pRB1 = (RigidBody*)ptr1->getUserPointer();

    RigidBodyCollisionCallback func0 = pRB0->GetCollisionCallback();
    RigidBodyCollisionCallback func1 = pRB1->GetCollisionCallback();

    // 两个刚体的相对速度
    FmVec3 vel0;
    pRB0->GetLinearVelocity(vel0);
    FmVec3 vel1;
    pRB1->GetLinearVelocity(vel1);
    FmVec3 vel;
    vel = vel0 - vel1;
    float speed = vel.length();

    if (func0)
    {
        (*func0)(pRB0, speed);
    }

    if (func1)
    {
        (*func1)(pRB1, speed);
    }

    //this return value is currently ignored, but to be on the safe side: return false if you don't calculate friction
    return false;
}

// --------------------------------------------------------------------------------------

PhysicsBullet::PhysicsBullet()
{
    m_pBtDispatcher = NULL;
    m_pBtBroadphaseInterface = NULL;
    m_pBtConstraintSolver = NULL;
    m_pBtCollisionConfiguration = NULL;

    m_pDynamicsWorld = NULL;

    m_firstCollisionShape = NULL;
    m_lastCollisionShape = NULL;

    m_firstRigidBody = NULL;
    m_lastRigidBody = NULL;

    m_firstConstraint = NULL;
    m_lastConstraint = NULL;

    m_firstRagdoll = NULL;
    m_lastRagdoll = NULL;

    m_vGravity = FmVec3(0.0f, -10.0f, 0.0f);

    m_fSimulationTime = 0.0f;

    m_nCollisionShapesNum = 0;

	m_nRigidBodiesNum = 0;
	m_rigidBodyID = 0;

    m_nConstraintsNum = 0;
    m_nRagdollsNum = 0;

    m_bUseContinuous = false;
    m_bPause = false;

    m_pRender = NULL;
    m_pPrimitive3DDrawer = NULL;
    m_pPainter = NULL;

    gContactAddedCallback = ContactAddedCallback;
}

PhysicsBullet::~PhysicsBullet()
{

}

// 开始执行
bool PhysicsBullet::Init()
{
    m_pRender = (IRender*)GetCore()->GetInterface("Render");
    m_pPrimitive3DDrawer = (IPrimitive3DDrawer*)GetCore()->GetInterface("Primitive3DDrawer");
    m_pPainter = (IPainter*)GetCore()->GetInterface("Painter");

    m_debugDrawer.init(m_pRender, m_pPrimitive3DDrawer, m_pPainter);

    g_physicsBulletPtr = this;

	return true;
}

bool PhysicsBullet::Shut()
{
	StopSimulator();

	// 释放Ragdoll
	{
		Ragdoll* currentPtr = m_firstRagdoll;
		Ragdoll* nextPtr;

		while (currentPtr)
		{
			nextPtr = currentPtr->m_next;
			CORE_DELETE(currentPtr);
			currentPtr = nextPtr;
		}
    }
    m_nRagdollsNum = 0;

	// 释放约束
	{
		Constraint* currentPtr = m_firstConstraint;
		Constraint* nextPtr;

		while (currentPtr)
		{
			nextPtr = currentPtr->m_next;
			CORE_DELETE(currentPtr);
			currentPtr = nextPtr;
		}
    }
    m_nConstraintsNum = 0;

	// 释放刚体
	{
		RigidBody* currentPtr = m_firstRigidBody;
		RigidBody* nextPtr;

		while (currentPtr)
		{
			nextPtr = currentPtr->m_next;
			CORE_DELETE(currentPtr);
			currentPtr = nextPtr;
		}
	}
	m_nRigidBodiesNum = 0;

	// 释放碰撞形
	{
		CollisionShape* currentPtr = m_firstCollisionShape;
		CollisionShape* nextPtr;

		while (currentPtr)
		{
			nextPtr = currentPtr->m_next;
			CORE_DELETE(currentPtr);
			currentPtr = nextPtr;
		}
    }
    m_nCollisionShapesNum = 0;

	return true;
}

// 每帧开始时调用
void PhysicsBullet::ExecFrameBegin()
{
	if (!m_pDynamicsWorld || m_bPause)
	{
		return;
	}

	float delta_time = (float)GetCore()->GetFrameSeconds();

#ifdef PERFORMANCE_DEBUG
	double dPhysicsTime = performance_time();

    m_pDynamicsWorld->stepSimulation(delta_time, 1);

    m_pRender->GetPerformance()->nPhysicsTime += (performance_time() - dPhysicsTime);

    m_pRender->GetPerformance()->nCollisionShapeNum = m_nCollisionShapesNum;
    m_pRender->GetPerformance()->nRigidbodyNum = m_nRigidBodiesNum;
    m_pRender->GetPerformance()->nConstraintNum = m_nConstraintsNum;
    m_pRender->GetPerformance()->nRagdollNum = m_nRagdollsNum;
#else
    m_pDynamicsWorld->stepSimulation(delta_time, 1);
#endif

    RigidBody* currentPtr = m_firstRigidBody;
    while (currentPtr)
    {
        currentPtr->UpdateState();
        currentPtr = currentPtr->m_next;
    }
}

// 每帧结束时调用
void PhysicsBullet::ExecFrameEnd()
{

}

bool PhysicsBullet::StartSimulator()
{
    // collision configuration contains default setup for memory, collision setup
    m_pBtCollisionConfiguration = new btDefaultCollisionConfiguration();
    // m_pBtCollisionConfiguration->setConvexConvexMultipointIterations();

    // use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    m_pBtDispatcher = new btCollisionDispatcher(m_pBtCollisionConfiguration);

    // btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    m_pBtBroadphaseInterface = new btDbvtBroadphase();

    // the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    m_pBtConstraintSolver = new btSequentialImpulseConstraintSolver();

    // btDiscreteDynamicsWorld provides discrete rigid body simulation
    m_pDynamicsWorld = new btDiscreteDynamicsWorld(
        m_pBtDispatcher,
        m_pBtBroadphaseInterface,
        m_pBtConstraintSolver,
        m_pBtCollisionConfiguration);
    m_pDynamicsWorld->setDebugDrawer(&m_debugDrawer);

    m_pDynamicsWorld->setGravity(FmVec3ToBtVector3(m_vGravity));
    m_pDynamicsWorld->getDispatchInfo().m_useContinuous = m_bUseContinuous;

    // register gimpact-algorithm
    btCollisionDispatcher* dispatcher = static_cast<btCollisionDispatcher*>(m_pDynamicsWorld ->getDispatcher());
    btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);

    m_bPause = false;
    m_rigidBodyID = 0;

    return true;
}

void PhysicsBullet::StopSimulator()
{
    if (!m_pDynamicsWorld)
    {
        return;
    }

    // 释放Ragdoll
    {
        Ragdoll* currentPtr = m_firstRagdoll;
        while (currentPtr)
        {
            currentPtr->Destroy();
            currentPtr = currentPtr->m_next;
        }
    }

    // 释放约束
    {
        Constraint* currentPtr = m_firstConstraint;
        while (currentPtr)
        {
            btTypedConstraint* constraint = currentPtr->m_pBtConstraint;
            if (constraint)
            {
                m_pDynamicsWorld->removeConstraint( constraint );
                delete constraint;
                currentPtr->m_pBtConstraint = NULL;
            }
            currentPtr = currentPtr->m_next;
        }
    }

    // 释放刚体
    {
        RigidBody* currentPtr = m_firstRigidBody;
        while (currentPtr)
        {
            btRigidBody* body = currentPtr->m_pBtRigidBody;
            if (body)
            {
                MyMotionState* msPtr = (MyMotionState*)body->getMotionState();
                if (msPtr)
                {
                    CORE_DELETE(msPtr);
                }

                m_pDynamicsWorld->removeRigidBody( body );
                delete body;

                currentPtr->m_pBtRigidBody = NULL;
            }
            currentPtr = currentPtr->m_next;
        }
    }

    // 释放碰撞形
    {
        CollisionShape* currentPtr = m_firstCollisionShape;
        while (currentPtr)
        {
            if (currentPtr->m_pBtCollisionShape)
            {
                delete currentPtr->m_pBtCollisionShape;
                currentPtr->m_pBtCollisionShape = NULL;
            }
            currentPtr = currentPtr->m_next;
        }
    }

    delete m_pDynamicsWorld;
    m_pDynamicsWorld = NULL;

    SAFE_DELETE(m_pBtConstraintSolver);
    SAFE_DELETE(m_pBtBroadphaseInterface);
    SAFE_DELETE(m_pBtDispatcher);
    SAFE_DELETE(m_pBtCollisionConfiguration);
}

bool PhysicsBullet::IsReady() const
{
    return (m_pDynamicsWorld != NULL);
}

void PhysicsBullet::SetGravity(const FmVec3& vGravity)
{
    m_vGravity = vGravity;
    if (m_pDynamicsWorld)
    {
        m_pDynamicsWorld->setGravity(FmVec3ToBtVector3(m_vGravity));
    }
}

void PhysicsBullet::SetUseContinuous(bool continuous)
{
    m_bUseContinuous = continuous;
    if (m_pDynamicsWorld)
    {
        m_pDynamicsWorld->getDispatchInfo().m_useContinuous = m_bUseContinuous;
    }
}

ICollisionShape* PhysicsBullet::CreateBoxCollisionShape(const FmVec3& vHalfExtend)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btVector3 v = FmVec3ToBtVector3(vHalfExtend);
    btBoxShape* pBtShape = new btBoxShape(v);

    CollisionShape* pShape = (CollisionShape*)CORE_ALLOC(sizeof(CollisionShape));
    new (pShape) CollisionShape(pBtShape, YE_CST_BOX, HEIGHT_AXIS_Y);

    pShape->m_params[0] = vHalfExtend.x;
    pShape->m_params[1] = vHalfExtend.y;
    pShape->m_params[2] = vHalfExtend.z;

    LinkCollisionShape(pShape);
    
    return pShape;
}

ICollisionShape* PhysicsBullet::CreateCubeCollisionShape(float fHalfExtend)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btVector3 v(fHalfExtend, fHalfExtend, fHalfExtend);
    btBoxShape* pBtShape = new btBoxShape(v);

    CollisionShape* pShape = (CollisionShape*)CORE_ALLOC(sizeof(CollisionShape));
    new (pShape) CollisionShape(pBtShape, YE_CST_CUBE, HEIGHT_AXIS_Y);

    pShape->m_params[0] = fHalfExtend;

    LinkCollisionShape(pShape);

    return pShape;
}

ICollisionShape* PhysicsBullet::CreateSphereCollisionShape(float fRadius)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btSphereShape* pBtShape = new btSphereShape(fRadius);

    CollisionShape* pShape = (CollisionShape*)CORE_ALLOC(sizeof(CollisionShape));
    new (pShape) CollisionShape(pBtShape, YE_CST_SPHERE, HEIGHT_AXIS_Y);

    pShape->m_params[0] = fRadius;

    LinkCollisionShape(pShape);

    return pShape;
}

ICollisionShape* PhysicsBullet::CreateCylinderCollisionShape(float fRadius, float fHeight, HeightAxis ha)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btCylinderShape* pBtShape;
    if (ha == HEIGHT_AXIS_X)
    {
        btVector3 v(fHeight*0.5f, fRadius, fRadius);
        pBtShape = new btCylinderShapeX(v);
    }
    else if (ha == HEIGHT_AXIS_Z)
    {
        btVector3 v(fRadius, fRadius, fHeight*0.5f);
        pBtShape = new btCylinderShapeZ(v);
    }
    else
    {
        btVector3 v(fRadius, fHeight*0.5f, fRadius);
        pBtShape = new btCylinderShape(v);
    }

    CollisionShape* pShape = (CollisionShape*)CORE_ALLOC(sizeof(CollisionShape));
    new (pShape) CollisionShape(pBtShape, YE_CST_CYLINDER, ha);

    pShape->m_params[0] = fRadius;
    pShape->m_params[1] = fHeight;

    LinkCollisionShape(pShape);

    return pShape;
}

ICollisionShape* PhysicsBullet::CreateCapsuleCollisionShape(float fRadius, float fHeight, HeightAxis ha)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btCapsuleShape* pBtShape;
    if (ha == HEIGHT_AXIS_X)
    {
        pBtShape = new btCapsuleShapeX(fRadius, fHeight);
    }
    else if (ha == HEIGHT_AXIS_Z)
    {
        pBtShape = new btCapsuleShapeZ(fRadius, fHeight);
    }
    else
    {
        pBtShape = new btCapsuleShape(fRadius, fHeight);
    }

    CollisionShape* pShape = (CollisionShape*)CORE_ALLOC(sizeof(CollisionShape));
    new (pShape) CollisionShape(pBtShape, YE_CST_CAPSULE, ha);

    pShape->m_params[0] = fRadius;
    pShape->m_params[1] = fHeight;

    LinkCollisionShape(pShape);

    return pShape;
}

ICollisionShape* PhysicsBullet::CreateConeCollisionShape(float fRadius, float fHeight, HeightAxis ha)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btConeShape* pBtShape;
    if (ha == HEIGHT_AXIS_X)
    {
        pBtShape = new btConeShapeX(fRadius, fHeight);
    }
    else if (ha == HEIGHT_AXIS_Z)
    {
        pBtShape = new btConeShapeZ(fRadius, fHeight);
    }
    else
    {
        pBtShape = new btConeShape(fRadius, fHeight);
    }

    CollisionShape* pShape = (CollisionShape*)CORE_ALLOC(sizeof(CollisionShape));
    new (pShape) CollisionShape(pBtShape, YE_CST_CONE, ha);

    pShape->m_params[0] = fRadius;
    pShape->m_params[1] = fHeight;

    LinkCollisionShape(pShape);

    return pShape;
}

// 凸多面体
ICollisionShape* PhysicsBullet::CreateConvexCollisionShape(const void* vb, unsigned int verticesNum, unsigned int stride)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btConvexHullShape* pBtShape = new btConvexHullShape((const btScalar*)vb, verticesNum, stride);

    CollisionShape* pShape = (CollisionShape*)CORE_ALLOC(sizeof(CollisionShape));
    new (pShape) CollisionShape(pBtShape, YE_CST_CONVEX, HEIGHT_AXIS_Y);

    pShape->m_params[0] = (float)verticesNum;

    LinkCollisionShape(pShape);

    return pShape;
}

// 开始创建一个组合形
void PhysicsBullet::BeginCompoundCollisionShape(unsigned int count)
{
    m_arrayShapeTM.clear();
    if (count > 0)
    {
        m_arrayShapeTM.reserve(count);
    }
}

// 为组合形添加子形状
void PhysicsBullet::AddChildCollisionShape(ICollisionShape* pShape, const FmMat4& mtx)
{
    ShapeTM shapeTM;
    shapeTM.pShape = ((CollisionShape*)pShape)->GetBtCollisionShape();
    shapeTM.tm.setFromOpenGLMatrix((const btScalar*)&mtx);
    m_arrayShapeTM.push_back(shapeTM);
}

// 完成组合形的创建
ICollisionShape* PhysicsBullet::FinishCompoundCollisionShape()
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    int numShapes = m_arrayShapeTM.size();
    if (numShapes == 0)
    {
        return NULL;
    }

    btCompoundShape* pBtShape = new btCompoundShape;
    for (int i = 0; i < numShapes; i++)
    {
        pBtShape->addChildShape(m_arrayShapeTM[i].tm, m_arrayShapeTM[i].pShape);
    }

    CollisionShape* pShape = (CollisionShape*)CORE_ALLOC(sizeof(CollisionShape));
    new (pShape) CollisionShape(pBtShape, YE_CST_CONVEX, HEIGHT_AXIS_Y);

    pShape->m_params[0] = (float)numShapes;

    LinkCollisionShape(pShape);

    m_arrayShapeTM.clear();

    return pShape;
}

// 动态的Mesh
ICollisionShape* PhysicsBullet::CreateDynamicMeshCollisionShape(
    const void* vb, unsigned int verticesNum, unsigned int vertexStride,
    const void* ib, unsigned int trianglesNum, unsigned int indexStride, IndexType it)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btTriangleIndexVertexArray* pIndexVertexArray;
    if (it == INDEX_16_BIT)
    {
        pIndexVertexArray = new btTriangleIndexVertexArray
            (trianglesNum,
             (short*)ib,
             indexStride,
             verticesNum,
             (btScalar*)vb,
             vertexStride);
    }
    else
    {
        pIndexVertexArray = new btTriangleIndexVertexArray
            (trianglesNum,
             (int*)ib,
             indexStride,
             verticesNum,
             (btScalar*)vb,
             vertexStride);
    }

    btGImpactMeshShape* pBtShape = new btGImpactMeshShape(pIndexVertexArray);

    CollisionShape* pShape = (CollisionShape*)CORE_ALLOC(sizeof(CollisionShape));
    new (pShape) CollisionShape(pBtShape, YE_CST_DYNAMIC_MESH, HEIGHT_AXIS_Y);

    memcpy(&pShape->m_params[0], &pIndexVertexArray, sizeof(void*));

    LinkCollisionShape(pShape);

    return pShape;
}

ICollisionShape* PhysicsBullet::CreatePlaneCollisionShape(const FmVec3& vNormal, float distance)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btVector3 v = FmVec3ToBtVector3(vNormal);
    btStaticPlaneShape* pBtShape = new btStaticPlaneShape(v, distance);

    CollisionShape* pShape = (CollisionShape*)CORE_ALLOC(sizeof(CollisionShape));
    new (pShape) CollisionShape(pBtShape, YE_CST_STATIC_PLANE, HEIGHT_AXIS_Y);

    pShape->m_params[0] = vNormal.x;
    pShape->m_params[1] = vNormal.y;
    pShape->m_params[2] = vNormal.z;
    pShape->m_params[3] = distance;

    LinkCollisionShape(pShape);

    return pShape;
}

// 静态的Mesh(只能生成静态刚体)
ICollisionShape* PhysicsBullet::CreateStaticMeshCollisionShape(
    const void* vb, unsigned int verticesNum, unsigned int vertexStride,
    const void* ib, unsigned int trianglesNum, unsigned int indexStride, IndexType it)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btTriangleIndexVertexArray* pIndexVertexArray;
    if (it == INDEX_16_BIT)
    {
        pIndexVertexArray = new btTriangleIndexVertexArray
            (trianglesNum,
             (short*)ib,
             indexStride,
             verticesNum,
             (btScalar*)vb,
             vertexStride);
    }
    else
    {
        pIndexVertexArray = new btTriangleIndexVertexArray
            (trianglesNum,
             (int*)ib,
             indexStride,
             verticesNum,
             (btScalar*)vb,
             vertexStride);
    }

    btBvhTriangleMeshShape* pBtShape = new btBvhTriangleMeshShape(pIndexVertexArray, true);

    CollisionShape* pShape = (CollisionShape*)CORE_ALLOC(sizeof(CollisionShape));
    new (pShape) CollisionShape(pBtShape, YE_CST_STATIC_MESH, HEIGHT_AXIS_Y);

    memcpy(&pShape->m_params[0], &pIndexVertexArray, sizeof(void*));

    LinkCollisionShape(pShape);

    return pShape;
}

// 可缩放的静态Mesh(只能生成静态刚体)
ICollisionShape* PhysicsBullet::CreateStaticScaledMeshCollisionShape(ICollisionShape* pStaticMeshCollisionShape, const FmVec3& vScale)
{
    if (pStaticMeshCollisionShape->GetShapeType() != YE_CST_STATIC_MESH)
    {
        return NULL;
    }

    btBvhTriangleMeshShape* pBvhTriangleMeshShape = (btBvhTriangleMeshShape*)((CollisionShape*)pStaticMeshCollisionShape)->GetBtCollisionShape();
    
    btVector3 v = FmVec3ToBtVector3(vScale);
    btScaledBvhTriangleMeshShape* pBtShape = new btScaledBvhTriangleMeshShape(pBvhTriangleMeshShape, v);
    ((CollisionShape*)pStaticMeshCollisionShape)->AddRef();

    CollisionShape* pShape = (CollisionShape*)CORE_ALLOC(sizeof(CollisionShape));
    new (pShape) CollisionShape(pBtShape, YE_CST_STATIC_SCALED_MESH, HEIGHT_AXIS_Y);

    LinkCollisionShape(pShape);

    return pShape;
}

// 地形
ICollisionShape* PhysicsBullet::CreateTerrainCollisionShape(
    unsigned int row, unsigned int col, 
    const void* heightfieldData, float heightScale,
    float minHeight, float maxHeight,
    TerrainDataType heightDataType,
    bool flipQuadEdges, HeightAxis ha)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btHeightfieldTerrainShape * pBtShape = new btHeightfieldTerrainShape(
        row, col,
        heightfieldData,
        heightScale,
        minHeight, maxHeight,
        ha, (PHY_ScalarType)heightDataType, flipQuadEdges);

    CollisionShape* pShape = (CollisionShape*)CORE_ALLOC(sizeof(CollisionShape));
    new (pShape) CollisionShape(pBtShape, YE_CST_TERRAIN, HEIGHT_AXIS_Y);

    pShape->m_params[0] = (float)row;
    pShape->m_params[1] = (float)col;

    LinkCollisionShape(pShape);

    return pShape;
}

CollisionShape* PhysicsBullet::FindCollisionShape(YeCollisionShapeType shapeType, HeightAxis ha, float params[4])
{
    CollisionShape* currentPtr = m_firstCollisionShape;

    while (currentPtr)
    {
        if (shapeType == currentPtr->m_type &&
            ha == currentPtr->m_heightAxis &&
            fabs(params[0] - currentPtr->m_params[0]) < FLT_EPSILON &&
            fabs(params[1] - currentPtr->m_params[1]) < FLT_EPSILON &&
            fabs(params[2] - currentPtr->m_params[2]) < FLT_EPSILON &&
            fabs(params[3] - currentPtr->m_params[3]) < FLT_EPSILON)
        {
            return currentPtr;
        }
        
        currentPtr = currentPtr->m_next;
    }

    return NULL;
}

void PhysicsBullet::DestoryCollisionShape(CollisionShape* ptr)
{
    delete ptr->m_pBtCollisionShape;
    UnlinkCollisionShape(ptr);
    CORE_DELETE(ptr);
}

void PhysicsBullet::ReleaseCollisionShape(btCollisionShape* ptr)
{
    CollisionShape* currentPtr = m_firstCollisionShape;

    while (currentPtr)
    {
        if (currentPtr->m_pBtCollisionShape == ptr)
        {
            currentPtr->Release();
            return;
        }
        currentPtr = currentPtr->m_next;
    }
}

IRigidBody* PhysicsBullet::CreateRigidBody(
    ICollisionShape* ptr, float fMass, const FmMat4& mtx, const FmVec3& vVelocity)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    CollisionShape* pShape = (CollisionShape*)ptr;
    btCollisionShape* pBtShape = pShape->m_pBtCollisionShape;

    btVector3 localInertia(0,0,0);
    if (fMass > 0.0f)
    {
        pBtShape->calculateLocalInertia(fMass, localInertia);
    }

    btTransform tm;
    tm.setFromOpenGLMatrix((const float*)&mtx);

    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    MyMotionState* myMotionState = (MyMotionState*)CORE_ALLOC(sizeof(MyMotionState));
    new (myMotionState) MyMotionState(tm);

    btRigidBody::btRigidBodyConstructionInfo rbInfo(fMass, myMotionState, pBtShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    body->setLinearVelocity(FmVec3ToBtVector3(vVelocity));
    body->setAngularVelocity(btVector3(0,0,0));

    //add the body to the dynamics world
    m_pDynamicsWorld->addRigidBody(body);

    RigidBody* pRigidBody = (RigidBody*)CORE_ALLOC(sizeof(RigidBody));
    new (pRigidBody) RigidBody(body, pShape, m_rigidBodyID, NULL);

    m_rigidBodyID++;

    LinkRigidBody(pRigidBody);

    pShape->AddRef();

    return pRigidBody;
}

void PhysicsBullet::DestoryRigidBody(RigidBody* pRigidBody)
{
    CollisionShape* pShape = pRigidBody->m_pCollisionShape;

    btRigidBody* body = pRigidBody->m_pBtRigidBody;
    if (body)
    {
        MyMotionState* msPtr = (MyMotionState*)body->getMotionState();
        if (msPtr)
        {
            CORE_DELETE(msPtr);
        }

        m_pDynamicsWorld->removeRigidBody( body );
        delete body;
    }
    UnlinkRigidBody(pRigidBody);
    CORE_DELETE(pRigidBody);

    SAFE_RELEASE(pShape);
}

// 创建一个约束,将刚体与空间中一点相绑定
IConstraint* PhysicsBullet::CreatePositionConstraintP2P(IRigidBody* pB, const FmVec3& vPosition)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btRigidBody* body = ((RigidBody*)pB)->m_pBtRigidBody;
    if (!body)
    {
        return NULL;
    }

    btTypedConstraint* pBtConstraint = NULL;
    btVector3 localPivot = FmVec3ToBtVector3(vPosition);
    // 需要由世界坐标转化到本地坐标
    localPivot = body->getCenterOfMassTransform().inverse() * localPivot;

    {
		btPoint2PointConstraint* p2p = new btPoint2PointConstraint(*body,localPivot);
		m_pDynamicsWorld->addConstraint(p2p,true);

        btScalar mousePickClamping = 30.f;
		p2p->m_setting.m_impulseClamp = mousePickClamping;
		//very weak constraint for picking
		p2p->m_setting.m_tau = 0.001f;

        /*
		p2p->setParam(BT_CONSTRAINT_CFM,0.8,0);
		p2p->setParam(BT_CONSTRAINT_CFM,0.8,1);
		p2p->setParam(BT_CONSTRAINT_CFM,0.8,2);
		p2p->setParam(BT_CONSTRAINT_ERP,0.1,0);
		p2p->setParam(BT_CONSTRAINT_ERP,0.1,1);
		p2p->setParam(BT_CONSTRAINT_ERP,0.1,2);
        */

        pBtConstraint = p2p;
    }

    Constraint* pConstraint = (Constraint*)CORE_ALLOC(sizeof(Constraint));
    new (pConstraint) Constraint(pBtConstraint, YE_CONSTRAINT_TYPE_POINT_2_POINT);

    LinkConstraint(pConstraint);

    ((RigidBody*)pB)->AddRef();

    return pConstraint;
}

// 创建一个约束,将刚体与空间中一点相绑定
IConstraint* PhysicsBullet::CreatePositionConstraintD6(IRigidBody* pB, const FmVec3& vPosition)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btRigidBody* body = ((RigidBody*)pB)->m_pBtRigidBody;
    if (!body)
    {
        return NULL;
    }

    btTypedConstraint* pBtConstraint = NULL;
    btVector3 localPivot = FmVec3ToBtVector3(vPosition);
    // 需要由世界坐标转化到本地坐标
    localPivot = body->getCenterOfMassTransform().inverse() * localPivot;

    {
        btTransform tr;
        tr.setIdentity();
        tr.setOrigin(localPivot);
        btGeneric6DofConstraint* dof6 = new btGeneric6DofConstraint(*body, tr, false);
        dof6->setLinearLowerLimit(btVector3(0,0,0));
        dof6->setLinearUpperLimit(btVector3(0,0,0));
        dof6->setAngularLowerLimit(btVector3(0,0,0));
        dof6->setAngularUpperLimit(btVector3(0,0,0));

        m_pDynamicsWorld->addConstraint(dof6,true);

        dof6->setParam(BT_CONSTRAINT_STOP_CFM,0.8f,0);
        dof6->setParam(BT_CONSTRAINT_STOP_CFM,0.8f,1);
        dof6->setParam(BT_CONSTRAINT_STOP_CFM,0.8f,2);
        dof6->setParam(BT_CONSTRAINT_STOP_CFM,0.8f,3);
        dof6->setParam(BT_CONSTRAINT_STOP_CFM,0.8f,4);
        dof6->setParam(BT_CONSTRAINT_STOP_CFM,0.8f,5);

        dof6->setParam(BT_CONSTRAINT_STOP_ERP,0.1f,0);
        dof6->setParam(BT_CONSTRAINT_STOP_ERP,0.1f,1);
        dof6->setParam(BT_CONSTRAINT_STOP_ERP,0.1f,2);
        dof6->setParam(BT_CONSTRAINT_STOP_ERP,0.1f,3);
        dof6->setParam(BT_CONSTRAINT_STOP_ERP,0.1f,4);
        dof6->setParam(BT_CONSTRAINT_STOP_ERP,0.1f,5);

        pBtConstraint = dof6;
    }

    Constraint* pConstraint = (Constraint*)CORE_ALLOC(sizeof(Constraint));
    new (pConstraint) Constraint(pBtConstraint, YE_CONSTRAINT_TYPE_D6);

    LinkConstraint(pConstraint);

    ((RigidBody*)pB)->AddRef();

    return pConstraint;
}

IConstraint* PhysicsBullet::CreatePoint2PointConstraint(IRigidBody* rbA, IRigidBody* rbB, const Point2PointConstraintData& constraintData)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btRigidBody* bodyA = ((RigidBody*)rbA)->m_pBtRigidBody;
    btRigidBody* bodyB = ((RigidBody*)rbB)->m_pBtRigidBody;
    if (!bodyA || !bodyB)
    {
        return NULL;
    }

    btTypedConstraint* pBtConstraint = NULL;
    {
        btVector3 pivotInA = FmVec3ToBtVector3(constraintData.m_pivotInA);
        btVector3 pivotInB = FmVec3ToBtVector3(constraintData.m_pivotInB);
        btPoint2PointConstraint* p2p = new btPoint2PointConstraint(*bodyA,*bodyB,pivotInA,pivotInB);

        pBtConstraint = p2p;
    }
    m_pDynamicsWorld->addConstraint(pBtConstraint, constraintData.m_disableCollisionsBetweenLinkedBodies!=0);

    Constraint* pConstraint = (Constraint*)CORE_ALLOC(sizeof(Constraint));
    new (pConstraint) Constraint(pBtConstraint, YE_CONSTRAINT_TYPE_POINT_2_POINT);
    pConstraint->SetProperties(&constraintData);

    LinkConstraint(pConstraint);

    ((RigidBody*)rbA)->AddRef();
    ((RigidBody*)rbB)->AddRef();

    return pConstraint;
}

IConstraint* PhysicsBullet::CreatePoint2PointConstraint(IRigidBody* rbA, const Point2PointConstraintData& constraintData)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btRigidBody* bodyA = ((RigidBody*)rbA)->m_pBtRigidBody;
    if (!bodyA)
    {
        return NULL;
    }

    btTypedConstraint* pBtConstraint = NULL;
    {
        btVector3 pivotInA = FmVec3ToBtVector3(constraintData.m_pivotInA);
        btPoint2PointConstraint* p2p = new btPoint2PointConstraint(*bodyA,pivotInA);

        pBtConstraint = p2p;
    }
    m_pDynamicsWorld->addConstraint(pBtConstraint, constraintData.m_disableCollisionsBetweenLinkedBodies!=0);

    Constraint* pConstraint = (Constraint*)CORE_ALLOC(sizeof(Constraint));
    new (pConstraint) Constraint(pBtConstraint, YE_CONSTRAINT_TYPE_POINT_2_POINT);
    pConstraint->SetProperties(&constraintData);

    LinkConstraint(pConstraint);

    ((RigidBody*)rbA)->AddRef();

    return pConstraint;
}

IConstraint* PhysicsBullet::CreateHingeConstraint(IRigidBody* rbA, IRigidBody* rbB, const HingeConstraintData& constraintData)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btRigidBody* bodyA = ((RigidBody*)rbA)->m_pBtRigidBody;
    btRigidBody* bodyB = ((RigidBody*)rbB)->m_pBtRigidBody;
    if (!bodyA || !bodyB)
    {
        return NULL;
    }

    btTypedConstraint* pBtConstraint = NULL;
    {
        btTransform rbAFrame;
        btTransform rbBFrame;
        rbAFrame.setFromOpenGLMatrix((const btScalar*)&constraintData.m_rbAFrame);
        rbBFrame.setFromOpenGLMatrix((const btScalar*)&constraintData.m_rbBFrame);

        btHingeConstraint* hinge = new btHingeConstraint(*bodyA,*bodyB,rbAFrame,rbBFrame,constraintData.m_useReferenceFrameA != 0);

        pBtConstraint = hinge;
    }
    m_pDynamicsWorld->addConstraint(pBtConstraint, constraintData.m_disableCollisionsBetweenLinkedBodies!=0);

    Constraint* pConstraint = (Constraint*)CORE_ALLOC(sizeof(Constraint));
    new (pConstraint) Constraint(pBtConstraint, YE_CONSTRAINT_TYPE_HINGE);
    pConstraint->SetProperties(&constraintData);

    LinkConstraint(pConstraint);

    ((RigidBody*)rbA)->AddRef();
    ((RigidBody*)rbB)->AddRef();

    return pConstraint;
}

IConstraint* PhysicsBullet::CreateHingeConstraint(IRigidBody* rbA, const HingeConstraintData& constraintData)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btRigidBody* bodyA = ((RigidBody*)rbA)->m_pBtRigidBody;
    if (!bodyA)
    {
        return NULL;
    }

    btTypedConstraint* pBtConstraint = NULL;
    {
        btTransform rbAFrame;
        rbAFrame.setFromOpenGLMatrix((const btScalar*)&constraintData.m_rbAFrame);

        btHingeConstraint* hinge = new btHingeConstraint(*bodyA,rbAFrame,constraintData.m_useReferenceFrameA != 0);

        pBtConstraint = hinge;
    }
    m_pDynamicsWorld->addConstraint(pBtConstraint, constraintData.m_disableCollisionsBetweenLinkedBodies!=0);

    Constraint* pConstraint = (Constraint*)CORE_ALLOC(sizeof(Constraint));
    new (pConstraint) Constraint(pBtConstraint, YE_CONSTRAINT_TYPE_HINGE);
    pConstraint->SetProperties(&constraintData);

    LinkConstraint(pConstraint);

    ((RigidBody*)rbA)->AddRef();

    return pConstraint;
}

IConstraint* PhysicsBullet::CreateConeTwistConstraint(IRigidBody* rbA, IRigidBody* rbB, const ConeTwistConstraintData& constraintData)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btRigidBody* bodyA = ((RigidBody*)rbA)->m_pBtRigidBody;
    btRigidBody* bodyB = ((RigidBody*)rbB)->m_pBtRigidBody;
    if (!bodyA || !bodyB)
    {
        return NULL;
    }

    btTypedConstraint* pBtConstraint = NULL;
    {
        btTransform rbAFrame;
        btTransform rbBFrame;
        rbAFrame.setFromOpenGLMatrix((const btScalar*)&constraintData.m_rbAFrame);
        rbBFrame.setFromOpenGLMatrix((const btScalar*)&constraintData.m_rbBFrame);

        btConeTwistConstraint* cone = new btConeTwistConstraint(*bodyA,*bodyB,rbAFrame,rbBFrame);

        pBtConstraint = cone;
    }
    m_pDynamicsWorld->addConstraint(pBtConstraint, constraintData.m_disableCollisionsBetweenLinkedBodies!=0);

    Constraint* pConstraint = (Constraint*)CORE_ALLOC(sizeof(Constraint));
    new (pConstraint) Constraint(pBtConstraint, YE_CONSTRAINT_TYPE_CONE_TWIST);
    pConstraint->SetProperties(&constraintData);

    LinkConstraint(pConstraint);

    ((RigidBody*)rbA)->AddRef();
    ((RigidBody*)rbB)->AddRef();

    return pConstraint;
}

IConstraint* PhysicsBullet::CreateConeTwistConstraint(IRigidBody* rbA, const ConeTwistConstraintData& constraintData)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btRigidBody* bodyA = ((RigidBody*)rbA)->m_pBtRigidBody;
    if (!bodyA)
    {
        return NULL;
    }

    btTypedConstraint* pBtConstraint = NULL;
    {
        btTransform rbAFrame;
        rbAFrame.setFromOpenGLMatrix((const btScalar*)&constraintData.m_rbAFrame);

        btConeTwistConstraint* cone = new btConeTwistConstraint(*bodyA,rbAFrame);

        pBtConstraint = cone;
    }
    m_pDynamicsWorld->addConstraint(pBtConstraint, constraintData.m_disableCollisionsBetweenLinkedBodies!=0);

    Constraint* pConstraint = (Constraint*)CORE_ALLOC(sizeof(Constraint));
    new (pConstraint) Constraint(pBtConstraint, YE_CONSTRAINT_TYPE_CONE_TWIST);
    pConstraint->SetProperties(&constraintData);

    LinkConstraint(pConstraint);

    ((RigidBody*)rbA)->AddRef();

    return pConstraint;
}

IConstraint* PhysicsBullet::CreateGeneric6DofConstraint(IRigidBody* rbA, IRigidBody* rbB, const Generic6DofConstraintData& constraintData)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btRigidBody* bodyA = ((RigidBody*)rbA)->m_pBtRigidBody;
    btRigidBody* bodyB = ((RigidBody*)rbB)->m_pBtRigidBody;
    if (!bodyA || !bodyB)
    {
        return NULL;
    }

    btTypedConstraint* pBtConstraint = NULL;
    {
        btTransform rbAFrame;
        btTransform rbBFrame;
        rbAFrame.setFromOpenGLMatrix((const btScalar*)&constraintData.m_rbAFrame);
        rbBFrame.setFromOpenGLMatrix((const btScalar*)&constraintData.m_rbBFrame);

        btGeneric6DofConstraint* dof = new btGeneric6DofConstraint(*bodyA,*bodyB,rbAFrame,rbBFrame,constraintData.m_useLinearReferenceFrameA != 0);

        pBtConstraint = dof;
    }
    m_pDynamicsWorld->addConstraint(pBtConstraint, constraintData.m_disableCollisionsBetweenLinkedBodies!=0);

    Constraint* pConstraint = (Constraint*)CORE_ALLOC(sizeof(Constraint));
    new (pConstraint) Constraint(pBtConstraint, YE_CONSTRAINT_TYPE_D6);
    pConstraint->SetProperties(&constraintData);

    LinkConstraint(pConstraint);

    ((RigidBody*)rbA)->AddRef();
    ((RigidBody*)rbB)->AddRef();

    return pConstraint;
}

IConstraint* PhysicsBullet::CreateGeneric6DofConstraint(IRigidBody* rbB, const Generic6DofConstraintData& constraintData)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btRigidBody* bodyB = ((RigidBody*)rbB)->m_pBtRigidBody;
    if (!bodyB)
    {
        return NULL;
    }

    btTypedConstraint* pBtConstraint = NULL;
    {
        btTransform rbBFrame;
        rbBFrame.setFromOpenGLMatrix((const btScalar*)&constraintData.m_rbBFrame);

        btGeneric6DofConstraint* dof = new btGeneric6DofConstraint(*bodyB,rbBFrame,constraintData.m_useLinearReferenceFrameA != 0);

        pBtConstraint = dof;
    }
    m_pDynamicsWorld->addConstraint(pBtConstraint, constraintData.m_disableCollisionsBetweenLinkedBodies!=0);

    Constraint* pConstraint = (Constraint*)CORE_ALLOC(sizeof(Constraint));
    new (pConstraint) Constraint(pBtConstraint, YE_CONSTRAINT_TYPE_D6);
    pConstraint->SetProperties(&constraintData);

    LinkConstraint(pConstraint);

    ((RigidBody*)rbB)->AddRef();

    return pConstraint;
}

IConstraint* PhysicsBullet::CreateGeneric6DofSpringConstraint(IRigidBody* rbA, IRigidBody* rbB, const Generic6DofSpringConstraintData& constraintData)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btRigidBody* bodyA = ((RigidBody*)rbA)->m_pBtRigidBody;
    btRigidBody* bodyB = ((RigidBody*)rbB)->m_pBtRigidBody;
    if (!bodyA || !bodyB)
    {
        return NULL;
    }

    btTypedConstraint* pBtConstraint = NULL;
    {
        btTransform rbAFrame;
        btTransform rbBFrame;
        rbAFrame.setFromOpenGLMatrix((const btScalar*)&constraintData.m_rbAFrame);
        rbBFrame.setFromOpenGLMatrix((const btScalar*)&constraintData.m_rbBFrame);

        btGeneric6DofSpringConstraint* dof = new btGeneric6DofSpringConstraint(*bodyA,*bodyB,rbAFrame,rbBFrame,constraintData.m_useLinearReferenceFrameA != 0);

        pBtConstraint = dof;
    }
    m_pDynamicsWorld->addConstraint(pBtConstraint, constraintData.m_disableCollisionsBetweenLinkedBodies!=0);

    Constraint* pConstraint = (Constraint*)CORE_ALLOC(sizeof(Constraint));
    new (pConstraint) Constraint(pBtConstraint, YE_CONSTRAINT_TYPE_D6_SPRING);
    pConstraint->SetProperties(&constraintData);

    LinkConstraint(pConstraint);

    ((RigidBody*)rbA)->AddRef();
    ((RigidBody*)rbB)->AddRef();

    return pConstraint;
}

IConstraint* PhysicsBullet::CreateSliderConstraint(IRigidBody* rbA, IRigidBody* rbB, const SliderConstraintData& constraintData)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btRigidBody* bodyA = ((RigidBody*)rbA)->m_pBtRigidBody;
    btRigidBody* bodyB = ((RigidBody*)rbB)->m_pBtRigidBody;
    if (!bodyA || !bodyB)
    {
        return NULL;
    }

    btTypedConstraint* pBtConstraint = NULL;
    {
        btTransform rbAFrame;
        btTransform rbBFrame;
        rbAFrame.setFromOpenGLMatrix((const btScalar*)&constraintData.m_rbAFrame);
        rbBFrame.setFromOpenGLMatrix((const btScalar*)&constraintData.m_rbBFrame);

        btSliderConstraint* slider = new btSliderConstraint(*bodyA,*bodyB,rbAFrame,rbBFrame,constraintData.m_useLinearReferenceFrameA != 0);

        pBtConstraint = slider;
    }
    m_pDynamicsWorld->addConstraint(pBtConstraint, constraintData.m_disableCollisionsBetweenLinkedBodies!=0);

    Constraint* pConstraint = (Constraint*)CORE_ALLOC(sizeof(Constraint));
    new (pConstraint) Constraint(pBtConstraint, YE_CONSTRAINT_TYPE_SLIDER);
    pConstraint->SetProperties(&constraintData);

    LinkConstraint(pConstraint);

    ((RigidBody*)rbA)->AddRef();
    ((RigidBody*)rbB)->AddRef();

    return pConstraint;
}

IConstraint* PhysicsBullet::CreateSliderConstraint(IRigidBody* rbB, const SliderConstraintData& constraintData)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btRigidBody* bodyB = ((RigidBody*)rbB)->m_pBtRigidBody;
    if (!bodyB)
    {
        return NULL;
    }

    btTypedConstraint* pBtConstraint = NULL;
    {
        btTransform rbBFrame;
        rbBFrame.setFromOpenGLMatrix((const btScalar*)&constraintData.m_rbBFrame);

        btSliderConstraint* slider = new btSliderConstraint(*bodyB,rbBFrame,constraintData.m_useLinearReferenceFrameA != 0);

        pBtConstraint = slider;
    }
    m_pDynamicsWorld->addConstraint(pBtConstraint, constraintData.m_disableCollisionsBetweenLinkedBodies!=0);

    Constraint* pConstraint = (Constraint*)CORE_ALLOC(sizeof(Constraint));
    new (pConstraint) Constraint(pBtConstraint, YE_CONSTRAINT_TYPE_SLIDER);
    pConstraint->SetProperties(&constraintData);

    LinkConstraint(pConstraint);

    ((RigidBody*)rbB)->AddRef();

    return pConstraint;
}

IConstraint* PhysicsBullet::CreateGearConstraint(IRigidBody* rbA, IRigidBody* rbB, const GearConstraintData& constraintData)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btRigidBody* bodyA = ((RigidBody*)rbA)->m_pBtRigidBody;
    btRigidBody* bodyB = ((RigidBody*)rbB)->m_pBtRigidBody;
    if (!bodyA || !bodyB)
    {
        return NULL;
    }

    btTypedConstraint* pBtConstraint = NULL;
    {
        btVector3 axisInA = FmVec3ToBtVector3(constraintData.m_axisInA);
        btVector3 axisInB = FmVec3ToBtVector3(constraintData.m_axisInB);

        btGearConstraint* gear = new btGearConstraint(*bodyA,*bodyB,axisInA,axisInB,constraintData.m_ratio);
    }
    m_pDynamicsWorld->addConstraint(pBtConstraint, constraintData.m_disableCollisionsBetweenLinkedBodies!=0);

    Constraint* pConstraint = (Constraint*)CORE_ALLOC(sizeof(Constraint));
    new (pConstraint) Constraint(pBtConstraint, YE_CONSTRAINT_TYPE_GEAR);
    pConstraint->SetProperties(&constraintData);

    LinkConstraint(pConstraint);

    ((RigidBody*)rbA)->AddRef();
    ((RigidBody*)rbB)->AddRef();

    return pConstraint;
}

IConstraint* PhysicsBullet::CreateFixConstraint(IRigidBody* rbA, IRigidBody* rbB, const FixConstraintData& constraintData)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btRigidBody* bodyA = ((RigidBody*)rbA)->m_pBtRigidBody;
    btRigidBody* bodyB = ((RigidBody*)rbB)->m_pBtRigidBody;
    if (!bodyA || !bodyB)
    {
        return NULL;
    }

    btTypedConstraint* pBtConstraint = NULL;
    {
        btTransform rbAFrame;
        btTransform rbBFrame;
        rbAFrame.setFromOpenGLMatrix((const btScalar*)&constraintData.m_rbAFrame);
        rbBFrame.setFromOpenGLMatrix((const btScalar*)&constraintData.m_rbBFrame);

        btFixedConstraint* fix = new btFixedConstraint(*bodyA,*bodyB,rbAFrame,rbBFrame);

        pBtConstraint = fix;
    }
    m_pDynamicsWorld->addConstraint(pBtConstraint, constraintData.m_disableCollisionsBetweenLinkedBodies!=0);

    Constraint* pConstraint = (Constraint*)CORE_ALLOC(sizeof(Constraint));
    new (pConstraint) Constraint(pBtConstraint, YE_CONSTRAINT_TYPE_FIX);
    pConstraint->SetProperties(&constraintData);

    LinkConstraint(pConstraint);

    ((RigidBody*)rbA)->AddRef();
    ((RigidBody*)rbB)->AddRef();

    return pConstraint;
}

// 删除约束
void PhysicsBullet::DestoryConstraint(Constraint* pConstraint)
{
    IRigidBody* rbA = pConstraint->GetRigidBodyA();
    IRigidBody* rbB = pConstraint->GetRigidBodyB();
    SAFE_RELEASE(rbA);
    SAFE_RELEASE(rbB);

    btTypedConstraint* constraint = pConstraint->m_pBtConstraint;
    if (constraint)
    {
        btRigidBody& bodyA = constraint->getRigidBodyA();
        if (!bodyA.isStaticOrKinematicObject())
        {
            bodyA.forceActivationState(ACTIVE_TAG);
            bodyA.setDeactivationTime( 0.0f );
        }

        btRigidBody& bodyB = constraint->getRigidBodyB();
        if (!bodyB.isStaticOrKinematicObject())
        {
            bodyB.forceActivationState(ACTIVE_TAG);
            bodyB.setDeactivationTime( 0.0f );
        }

        m_pDynamicsWorld->removeConstraint( constraint );
        delete constraint;
    }

    UnlinkConstraint(pConstraint);
    CORE_DELETE(pConstraint);
}

// 创建RAGDOLL
IRagdoll* PhysicsBullet::CreateRagdoll(const FmVec3& vPos, float scale)
{
    Ragdoll* pRagdoll = CORE_NEW(Ragdoll);
    if (!pRagdoll->Build(vPos, scale))
    {
        CORE_DELETE(pRagdoll);
        return NULL;
    }

    LinkRagdoll(pRagdoll);
    return pRagdoll;
}

// 删除RAGDOLL
void PhysicsBullet::DestoryRagdoll(IRagdoll* pRagdoll)
{
    Ragdoll* ragdoll = (Ragdoll*)pRagdoll;
    UnlinkRagdoll(ragdoll);
    CORE_DELETE(ragdoll);
}

// 射线点选
IRigidBody* PhysicsBullet::RayTrace(const FmVec3& vRayFrom, const FmVec3& vRayTo, 
                                    FmVec3* vPickPos, FmVec3* vPickNor)
{
    if (!m_pDynamicsWorld)
    {
        return NULL;
    }

    btVector3 rayFrom = FmVec3ToBtVector3(vRayFrom);
    btVector3 rayTo = FmVec3ToBtVector3(vRayTo);

    btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom,rayTo);
    m_pDynamicsWorld->rayTest(rayFrom,rayTo,rayCallback);
    if (!rayCallback.hasHit())
    {
        return NULL;
    }

    btRigidBody* body = (btRigidBody*)btRigidBody::upcast(rayCallback.m_collisionObject);
    if (!body || body->isStaticOrKinematicObject())
    {
        return NULL;
    }

    body->setActivationState(DISABLE_DEACTIVATION);

    if (vPickPos)
    {
        vPickPos->x = rayCallback.m_hitPointWorld.m_floats[0];
        vPickPos->y = rayCallback.m_hitPointWorld.m_floats[1];
        vPickPos->z = rayCallback.m_hitPointWorld.m_floats[2];
    }

    if (vPickNor)
    {
        vPickNor->x = rayCallback.m_hitNormalWorld.m_floats[0];
        vPickNor->y = rayCallback.m_hitNormalWorld.m_floats[1];
        vPickNor->z = rayCallback.m_hitNormalWorld.m_floats[2];
    }

    return (IRigidBody*)body->getUserPointer();
}

void PhysicsBullet::LinkCollisionShape(CollisionShape* object)
{
    if (m_lastCollisionShape)
    {
        m_lastCollisionShape->m_next = object;
        object->m_prev = m_lastCollisionShape;
        object->m_next = NULL;
        m_lastCollisionShape = object;
    }
    else
    {
        object->m_prev = NULL;
        object->m_next = NULL;
        m_firstCollisionShape = m_lastCollisionShape = object;
    }

    m_nCollisionShapesNum++;
}

void PhysicsBullet::UnlinkCollisionShape(CollisionShape* object)
{
    if (object->m_prev)
    {
        object->m_prev->m_next = object->m_next;

        if (object->m_next)
        {
            object->m_next->m_prev = object->m_prev;
            object->m_next = NULL;
        }
        else
        {
            m_lastCollisionShape = object->m_prev;
        }

        object->m_prev = NULL;
    }
    else
    {
        m_firstCollisionShape = object->m_next;

        if (object->m_next)
        {
            object->m_next->m_prev = NULL;
            object->m_next = NULL;
        }
        else
        {
            m_lastCollisionShape = NULL;
        }
    }

    m_nCollisionShapesNum--;
}

void PhysicsBullet::LinkRigidBody(RigidBody* object)
{
    if (m_lastRigidBody)
    {
        m_lastRigidBody->m_next = object;
        object->m_prev = m_lastRigidBody;
        object->m_next = NULL;
        m_lastRigidBody = object;
    }
    else
    {
        object->m_prev = NULL;
        object->m_next = NULL;
        m_firstRigidBody = m_lastRigidBody = object;
    }

    m_nRigidBodiesNum++;
}

void PhysicsBullet::UnlinkRigidBody(RigidBody* object)
{
    if (object->m_prev)
    {
        object->m_prev->m_next = object->m_next;

        if (object->m_next)
        {
            object->m_next->m_prev = object->m_prev;
            object->m_next = NULL;
        }
        else
        {
            m_lastRigidBody = object->m_prev;
        }

        object->m_prev = NULL;
    }
    else
    {
        m_firstRigidBody = object->m_next;

        if (object->m_next)
        {
            object->m_next->m_prev = NULL;
            object->m_next = NULL;
        }
        else
        {
            m_lastRigidBody = NULL;
        }
    }

    m_nRigidBodiesNum--;
}

void PhysicsBullet::LinkConstraint(Constraint* object)
{
    if (m_lastConstraint)
    {
        m_lastConstraint->m_next = object;
        object->m_prev = m_lastConstraint;
        object->m_next = NULL;
        m_lastConstraint = object;
    }
    else
    {
        object->m_prev = NULL;
        object->m_next = NULL;
        m_firstConstraint = m_lastConstraint = object;
    }

    m_nConstraintsNum++;
}

void PhysicsBullet::UnlinkConstraint(Constraint* object)
{
    if (object->m_prev)
    {
        object->m_prev->m_next = object->m_next;

        if (object->m_next)
        {
            object->m_next->m_prev = object->m_prev;
            object->m_next = NULL;
        }
        else
        {
            m_lastConstraint = object->m_prev;
        }

        object->m_prev = NULL;
    }
    else
    {
        m_firstConstraint = object->m_next;

        if (object->m_next)
        {
            object->m_next->m_prev = NULL;
            object->m_next = NULL;
        }
        else
        {
            m_lastConstraint = NULL;
        }
    }

    m_nConstraintsNum--;
}

void PhysicsBullet::LinkRagdoll(Ragdoll* object)
{
    if (m_lastRagdoll)
    {
        m_lastRagdoll->m_next = object;
        object->m_prev = m_lastRagdoll;
        object->m_next = NULL;
        m_lastRagdoll = object;
    }
    else
    {
        object->m_prev = NULL;
        object->m_next = NULL;
        m_firstRagdoll = m_lastRagdoll = object;
    }

    m_nRagdollsNum++;
}

void PhysicsBullet::UnlinkRagdoll(Ragdoll* object)
{
    if (object->m_prev)
    {
        object->m_prev->m_next = object->m_next;

        if (object->m_next)
        {
            object->m_next->m_prev = object->m_prev;
            object->m_next = NULL;
        }
        else
        {
            m_lastRagdoll = object->m_prev;
        }

        object->m_prev = NULL;
    }
    else
    {
        m_firstRagdoll = object->m_next;

        if (object->m_next)
        {
            object->m_next->m_prev = NULL;
            object->m_next = NULL;
        }
        else
        {
            m_lastRagdoll = NULL;
        }
    }

    m_nRagdollsNum--;
}

void PhysicsBullet::SetDebugDrawModes(int modes)
{
    m_debugDrawer.setDebugMode(modes);
}

int PhysicsBullet::GetDebugDrawModes() const
{
    return m_debugDrawer.getDebugMode();
}

void PhysicsBullet::DrawRigidBody(IRigidBody* ptr, unsigned int color)
{
    RigidBody* pRigidBody = (RigidBody*)ptr;
    CollisionShape* pCollisionShape = (CollisionShape*)pRigidBody->GetCollisionShape();

    const btCollisionShape* pBtShape = pCollisionShape->GetBtCollisionShape();

    FmMat4 mtxRB;
    pRigidBody->GetTransformRB(mtxRB);

    int debugMode = m_debugDrawer.getDebugMode();

    DrawCollisionShape(pBtShape, mtxRB, color, debugMode);
}

class Drawcallback : public btTriangleCallback
{
private:
    IPrimitive3DDrawer* m_pPrimitive3DDrawer;
    unsigned int m_color;
    bool m_wireframe;

public:
    Drawcallback(IPrimitive3DDrawer* pDrawer, unsigned int color, bool wireframe)
        :m_pPrimitive3DDrawer(pDrawer), m_color(color), m_wireframe(wireframe)
    {
    }

    virtual void processTriangle(btVector3* triangle,int partId, int triangleIndex)
    {
        FmVec3 v0 = BtVector3ToFmVec3(triangle[0]);
        FmVec3 v1 = BtVector3ToFmVec3(triangle[1]);
        FmVec3 v2 = BtVector3ToFmVec3(triangle[2]);

        if (m_wireframe)
        {
            m_pPrimitive3DDrawer->DrawLine(v0, v1, m_color);
            m_pPrimitive3DDrawer->DrawLine(v1, v2, m_color);
            m_pPrimitive3DDrawer->DrawLine(v2, v0, m_color);
        } 
        else
        {
            m_pPrimitive3DDrawer->DrawTriangle(v0, v1, v2, m_color);
        }
    }
};

void PhysicsBullet::DrawCollisionShape(const btCollisionShape* pBtShape, const FmMat4& mtxRB, unsigned int color, int debugMode)
{
    int shapeType = pBtShape->getShapeType();

    bool wireframe = (debugMode & btIDebugDraw::DBG_DrawWireframe)!=0;

    m_pPrimitive3DDrawer->SetWireMode(wireframe);

    if (shapeType == CUSTOM_CONVEX_SHAPE_TYPE)
    {
		const btBoxShape* boxShape = static_cast<const btBoxShape*>(pBtShape);
		btVector3 halfExtent = boxShape->getHalfExtentsWithMargin();
        FmVec3 vMax = BtVector3ToFmVec3(halfExtent);
        FmVec3 vMin = -vMax;

        m_pPrimitive3DDrawer->SetMtxWorld(mtxRB);
        m_pPrimitive3DDrawer->DrawBox(vMin, vMax, color);

		return;
	} 
	
    if((shapeType == BOX_SHAPE_PROXYTYPE) && (debugMode & btIDebugDraw::DBG_FastWireframe))
    {
        const btBoxShape* boxShape = static_cast<const btBoxShape*>(pBtShape);
        btVector3 halfExtent = boxShape->getHalfExtentsWithMargin();
        FmVec3 vMax = BtVector3ToFmVec3(halfExtent);
        FmVec3 vMin = -vMax;

        m_pPrimitive3DDrawer->SetMtxWorld(mtxRB);
        m_pPrimitive3DDrawer->DrawBindingBox(vMin, vMax, color);

		return;
	}

	if (shapeType == UNIFORM_SCALING_SHAPE_PROXYTYPE)
	{
		const btUniformScalingShape* scalingShape = static_cast<const btUniformScalingShape*>(pBtShape);
		const btConvexShape* convexShape = scalingShape->getChildShape();
		float scalingFactor = (float)scalingShape->getUniformScalingFactor();
		{
            FmMat4 mtxScale;
            FmMat4Scaling(&mtxScale, scalingFactor, scalingFactor, scalingFactor);
            FmMat4 mtxShape;
            FmMat4Multiply(&mtxShape, &mtxScale, &mtxRB);

			DrawCollisionShape(convexShape, mtxShape, color, debugMode);
		}

		return;
	}

	if (shapeType == COMPOUND_SHAPE_PROXYTYPE)
	{
		const btCompoundShape* compoundShape = static_cast<const btCompoundShape*>(pBtShape);
        int numShapes = compoundShape->getNumChildShapes();
		for (int i = 0; i < numShapes; i--)
		{
			btTransform childTrans = compoundShape->getChildTransform(i);
			const btCollisionShape* childShape = compoundShape->getChildShape(i);

			FmMat4 childMat;
			childTrans.getOpenGLMatrix((btScalar*)&childMat);
            FmMat4 mtxShape;
            FmMat4Multiply(&mtxShape, &childMat, &mtxRB);

            DrawCollisionShape(childShape, mtxShape, color, debugMode);
		}

        return;
	} 

    // 凸多面体
	if (pBtShape->isConvex())
    {
        m_pPrimitive3DDrawer->SetMtxWorld(mtxRB);

		///you can comment out any of the specific cases, and use the default
		switch (shapeType)
		{
		case SPHERE_SHAPE_PROXYTYPE:
			{
				const btSphereShape* sphereShape = static_cast<const btSphereShape*>(pBtShape);
				float radius = sphereShape->getMargin();//radius doesn't include the margin, so draw with margin
				m_pPrimitive3DDrawer->DrawSphere(FmVec3(0.0f, 0.0f, 0.0f), radius, color);
            }
            break;

		case BOX_SHAPE_PROXYTYPE:
			{
				const btBoxShape* boxShape = static_cast<const btBoxShape*>(pBtShape);
				btVector3 halfExtent = boxShape->getHalfExtentsWithMargin();
                FmVec3 vMax = BtVector3ToFmVec3(halfExtent);
                FmVec3 vMin = -vMax;
                m_pPrimitive3DDrawer->DrawBox(vMin, vMax, color);
            }
            break;

		case CONE_SHAPE_PROXYTYPE:
			{
				const btConeShape* coneShape = static_cast<const btConeShape*>(pBtShape);
				float radius = coneShape->getRadius();//+coneShape->getMargin();
                float height = coneShape->getHeight();//+coneShape->getMargin();

                int upIndex = coneShape->getConeUpIndex();
                YeAxis3DType axis = YE_AXIS_3D_PLANE_XOZ;
                FmVec3 vPos;
                if (upIndex == 0)
                {
                    axis = YE_AXIS_3D_PLANE_YOZ;
                    vPos = FmVec3(-height*0.5f, 0.0f, 0.0f);
                }
                else if (upIndex == 2)
                {
                    axis = YE_AXIS_3D_PLANE_XOY;
                    vPos = FmVec3(0.0f, 0.0f, -height*0.5f);
                }
                else
                {
                    vPos = FmVec3(0.0f, -height*0.5f, 0.0f);
                }

                m_pPrimitive3DDrawer->DrawCone(vPos, height, radius, color, color, axis);
            }
            break;

		case CYLINDER_SHAPE_PROXYTYPE:
			{
                const btCylinderShape* cylinder = static_cast<const btCylinderShape*>(pBtShape);

                int upAxis = cylinder->getUpAxis();
				float radius = cylinder->getRadius();
                float halfHeight = cylinder->getHalfExtentsWithMargin()[upAxis];

                YeAxis3DType axis = YE_AXIS_3D_PLANE_XOZ;
                FmVec3 vPos;
                if (upAxis == 0)
                {
                    axis = YE_AXIS_3D_PLANE_YOZ;
                    vPos = FmVec3(-halfHeight, 0.0f, 0.0f);
                }
                else if (upAxis == 2)
                {
                    axis = YE_AXIS_3D_PLANE_XOY;
                    vPos = FmVec3(0.0f, 0.0f, -halfHeight);
                }
                else
                {
                    vPos = FmVec3(0.0f, -halfHeight, 0.0f);
                }

                m_pPrimitive3DDrawer->DrawCylinder(vPos, halfHeight*2.0f, radius, radius, color, color, axis);
            }
            break;

        case CAPSULE_SHAPE_PROXYTYPE:
            {
                const btCapsuleShape* capsule = static_cast<const btCapsuleShape*>(pBtShape);

                int upAxis = capsule->getUpAxis();
                float radius = capsule->getRadius();
                float halfHeight = capsule->getHalfHeight();

                YeAxis3DType axis = YE_AXIS_3D_PLANE_XOZ;
                if (upAxis == 0)
                {
                    axis = YE_AXIS_3D_PLANE_YOZ;
                }
                else if (upAxis == 2)
                {
                    axis = YE_AXIS_3D_PLANE_XOY;
                }

                m_pPrimitive3DDrawer->DrawCapsule(FmVec3(0.0f, 0.0f, 0.0f), radius, halfHeight*2.0f, color, axis);
            }
            break;

		case MULTI_SPHERE_SHAPE_PROXYTYPE:
		    {
			    const btMultiSphereShape* multiSphereShape = static_cast<const btMultiSphereShape*>(pBtShape);
                
                int numShapes = multiSphereShape->getSphereCount();
                FmVec3 vPos;
                for (int i = 0; i < numShapes; i--)
                {
                    const btVector3& v = multiSphereShape->getSpherePosition(i);
                    vPos = BtVector3ToFmVec3(v);

                    m_pPrimitive3DDrawer->DrawSphere(vPos, multiSphereShape->getSphereRadius(i), color);
			    }
            }
            break;

		default:
            {
                btVector3 b1;
                btVector3 b2;
                btVector3 b3;

                FmVec3 v1;
                FmVec3 v2;
                FmVec3 v3;

				const btConvexPolyhedron* poly = pBtShape->isPolyhedral() ? ((btPolyhedralConvexShape*) pBtShape)->getConvexPolyhedron() : 0;
				if (poly)
                {
					for (int i = 0; i < poly->m_faces.size(); i++)
					{
						int numVerts = poly->m_faces[i].m_indices.size();
						if (numVerts > 2)
						{
                            b1 = poly->m_vertices[poly->m_faces[i].m_indices[0]];
                            v1 = BtVector3ToFmVec3(b1);

							for (int v=0; v<poly->m_faces[i].m_indices.size()-2; v++)
							{
								b2 = poly->m_vertices[poly->m_faces[i].m_indices[v+1]];
								b3 = poly->m_vertices[poly->m_faces[i].m_indices[v+2]];

                                v2 = BtVector3ToFmVec3(b2);
                                v3 = BtVector3ToFmVec3(b3);

                                m_pPrimitive3DDrawer->DrawTriangle(v1, v2, v3, color);
							}
						}
					}
				} 
                else
                {
					btShapeHull* hull = (btShapeHull*)pBtShape->getUserPointer();
					if (hull)
                    {
                        int numTriangles = hull->numTriangles();
                        int numIndices = hull->numIndices();
                        int numVertices = hull->numVertices();

						int index = 0;
						const unsigned int* idx = hull->getIndexPointer();
						const btVector3* vtx = hull->getVertexPointer();

						for (int i = 0; i < numTriangles; i++)
						{
							int i1 = index++;
							int i2 = index++;
							int i3 = index++;
							btAssert(i1 < numIndices && i2 < numIndices && i3 < numIndices);

							int index1 = idx[i1];
							int index2 = idx[i2];
							int index3 = idx[i3];
							btAssert(index1 < numVertices && index2 < numVertices && index3 < numVertices);

							b1 = vtx[index1];
							b2 = vtx[index2];
							b3 = vtx[index3];

                            v1 = BtVector3ToFmVec3(b1);
                            v2 = BtVector3ToFmVec3(b2);
                            v3 = BtVector3ToFmVec3(b3);

                            m_pPrimitive3DDrawer->DrawTriangle(v1, v2, v3, color);
						}
                    }
				}
            }
            break;
		}

        return;
	}

    // 平面
    if (shapeType == STATIC_PLANE_PROXYTYPE)
    {
        m_pPrimitive3DDrawer->SetMtxWorld(mtxRB);

        const btStaticPlaneShape* staticPlaneShape = static_cast<const btStaticPlaneShape*>(pBtShape);
        btScalar planeConst = staticPlaneShape->getPlaneConstant();
        const btVector3& planeNormal = staticPlaneShape->getPlaneNormal();
        btVector3 planeOrigin = planeNormal * planeConst;
        btScalar vecLen = 100.0f;

#if 0
        btVector3 vec0, vec1;
        btPlaneSpace1(planeNormal,vec0, vec1);
        btVector3 pt0 = planeOrigin + vec0*vecLen;
        btVector3 pt1 = planeOrigin - vec0*vecLen;
        btVector3 pt2 = planeOrigin + vec1*vecLen;
        btVector3 pt3 = planeOrigin - vec1*vecLen;

        FmVec3 v0 = BtVector3ToFmVec3(pt0);
        FmVec3 v1 = BtVector3ToFmVec3(pt1);
        FmVec3 v2 = BtVector3ToFmVec3(pt2);
        FmVec3 v3 = BtVector3ToFmVec3(pt3);
        m_pPrimitive3DDrawer->DrawQuadrangle(v0, v2, v1, v3, color);
#else
        FmPlane plane;
        FmVec3 normal = BtVector3ToFmVec3(planeNormal);
        FmVec3 origin = BtVector3ToFmVec3(planeOrigin);
        FmPlaneFromPointNormal(&plane, &origin, &normal);
        FmVec3 vMin = FmVec3(-vecLen, -vecLen, -vecLen);
        FmVec3 vMax = FmVec3(vecLen, vecLen, vecLen);
        m_pPrimitive3DDrawer->DrawPlane(plane, vMin, vMax, color);
#endif

        return;
    }

    // 凹多面体
	if (pBtShape->isConcave() && !pBtShape->isInfinite())
    {
        m_pPrimitive3DDrawer->SetMtxWorld(mtxRB);

        //btVector3 aabbMin(0,0,0),aabbMax(0,0,0);
        //m_dynamicsWorld->getBroadphase()->getBroadphaseAabb(aabbMin,aabbMax);

        btVector3 aabbMin(-BT_LARGE_FLOAT,-BT_LARGE_FLOAT,-BT_LARGE_FLOAT);
        btVector3 aabbMax(BT_LARGE_FLOAT,BT_LARGE_FLOAT,BT_LARGE_FLOAT);

		btConcaveShape* concaveMesh = (btConcaveShape*) pBtShape;
		Drawcallback drawCallback(m_pPrimitive3DDrawer, color, wireframe);
		concaveMesh->processAllTriangles(&drawCallback,aabbMin,aabbMax);
        
        return;
	}
}

// 通过R,G,B生成一整数表示颜色
#define YD_RGB(r,g,b)           \
    (unsigned int) ( (b) | ( (g) << 8 ) | ( (r) << 16 ) | 0xff000000 )

const unsigned int g_colors[12] = 
{
    YD_RGB(128, 0, 168),
    YD_RGB(48, 224, 0),
    YD_RGB(0, 168, 168),
    YD_RGB(224, 0, 128),
    YD_RGB(0, 48, 224),
    YD_RGB(168, 128, 0),

    YD_RGB(84, 0, 112),
    YD_RGB(64, 128, 0),
    YD_RGB(0, 112, 112),
    YD_RGB(168, 0, 84),
    YD_RGB(0, 32, 128),
    YD_RGB(112, 84, 0),
};

void PhysicsBullet::DrawDebug()
{
    // 渲染可见的刚体
    if (0 == (m_debugDrawer.getDebugMode() & btIDebugDraw::DBG_DrawWireframe))
    {
        IRigidBody::ActivationState state;
        unsigned int colorIndex;

        RigidBody* currentPtr = m_firstRigidBody;
        while (currentPtr)
        {
            if (currentPtr->IsVisible())
            {
                state = IRigidBody::AS_INVALID;
                currentPtr->GetActivationState(state);
                colorIndex = state;
                if (currentPtr->GetID() % 2 == 1)
                {
                    colorIndex += 6;
                }

                DrawRigidBody(currentPtr, g_colors[colorIndex]);
            }
            currentPtr = currentPtr->m_next;
        }
    }

    // 渲染调试数据
    if (m_debugDrawer.isReady())
    {
        m_pPrimitive3DDrawer->SetMtxWorldIdentity();
        m_pDynamicsWorld->debugDrawWorld();
    }

    m_pPrimitive3DDrawer->Flush();
}

performance_info_t* PhysicsBullet::GetPerformance()
{
    return m_pRender->GetPerformance();
}
