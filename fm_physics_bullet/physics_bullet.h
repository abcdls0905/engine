
/****************************************************************

  File name   :  PhysicsBullet.h
  Author      :  叶峰
  Version     :  1.0a
  Create Date :  2013/10/22   
  Description :  Bullet物理系统

*****************************************************************/

#ifndef __PhysicsBullet_H__
#define __PhysicsBullet_H__

// INCLUDES -----------------------------------------------------------------------------

#include "../visual/i_physics_simulator.h"
#include "bullet/linear_math/bt_aligned_object_array.h"
#include "debug_drawer.h"

// --------------------------------------------------------------------------------------

class btDispatcher;
class btBroadphaseInterface;
class btConstraintSolver;
class btCollisionConfiguration;
class btDynamicsWorld;
class btTypedConstraint;
class btCollisionShape;
class btManifoldPoint;
struct btCollisionObjectWrapper;

class YicDX9Render;

class RigidBody;
class CollisionShape;
class Constraint;
class Ragdoll;

// --------------------------------------------------------------------------------------

class PhysicsBullet : public IPhysicsSimulator
{
public:
    PhysicsBullet();

    ~PhysicsBullet();

	// 开始执行
	virtual bool Init();

	virtual bool Shut();

	// 是否需要每帧运行（调用ExecFrameBegin和ExecFrameEnd）
	virtual bool NeedExecPerFrame() { return true; }
	// 每帧开始时调用
	virtual void ExecFrameBegin();
	// 每帧结束时调用
	virtual void ExecFrameEnd();

    // 启动动力学世界
    bool StartSimulator();

    // 关闭动力学世界
    void StopSimulator();

    // 是否就绪
    bool IsReady() const;

    // 设置重力
    void SetGravity(const FmVec3& vGravity);

    // 设置是否使用连续物理
    void SetUseContinuous(bool continuous);

    // 创建碰撞形
    ICollisionShape* CreateBoxCollisionShape(const FmVec3& vHalfExtend);

    ICollisionShape* CreateCubeCollisionShape(float fHalfExtend);

    ICollisionShape* CreateSphereCollisionShape(float fRadius);

    ICollisionShape* CreateCylinderCollisionShape(float fRadius, float fHeight, HeightAxis ha = HEIGHT_AXIS_Y);

    ICollisionShape* CreateCapsuleCollisionShape(float fRadius, float fHeight, HeightAxis ha = HEIGHT_AXIS_Y);

    ICollisionShape* CreateConeCollisionShape(float fRadius, float fHeight, HeightAxis ha = HEIGHT_AXIS_Y);

    // 凸多面体
    ICollisionShape* CreateConvexCollisionShape(const void* vb, unsigned int verticesNum, unsigned int stride);

    // 开始创建一个组合形
    void BeginCompoundCollisionShape(unsigned int count = 0);

    // 为组合形添加子形状
    void AddChildCollisionShape(ICollisionShape* pShape, const FmMat4& mtx);

    // 完成组合形的创建
    ICollisionShape* FinishCompoundCollisionShape();

    // 动态的Mesh
    ICollisionShape* CreateDynamicMeshCollisionShape(const void* vb, unsigned int verticesNum, unsigned int vertexStride,
        const void* ib, unsigned int trianglesNum, unsigned int indexStride = 6, IndexType it = INDEX_16_BIT);

    // 平面(只能生成静态刚体)
    ICollisionShape* CreatePlaneCollisionShape(const FmVec3& vNormal, float distance);

    // 静态的Mesh(只能生成静态刚体)
    ICollisionShape* CreateStaticMeshCollisionShape(const void* vb, unsigned int verticesNum, unsigned int vertexStride,
        const void* ib, unsigned int trianglesNum, unsigned int indexStride = 6, IndexType it = INDEX_16_BIT);

    // 可缩放的静态Mesh(只能生成静态刚体)
    ICollisionShape* CreateStaticScaledMeshCollisionShape(ICollisionShape* pStaticMeshCollisionShape, const FmVec3& vScale);

    // 地形(只能生成静态刚体)
    ICollisionShape* CreateTerrainCollisionShape(
        unsigned int row, unsigned int col, 
        const void* heightfieldData, float heightScale,
        float minHeight, float maxHeight,
        TerrainDataType heightDataType,
        bool flipQuadEdges = true, HeightAxis ha = HEIGHT_AXIS_Y);

    CollisionShape* FindCollisionShape(YeCollisionShapeType shapeType, HeightAxis ha, float params[4]);

    void DestoryCollisionShape(CollisionShape* ptr);

    void ReleaseCollisionShape(btCollisionShape* ptr);

    // 创建刚体,fMass小于等于0表示为静态刚体
    IRigidBody* CreateRigidBody(ICollisionShape* pShape, float fMass, const FmMat4& mtx, const FmVec3& vVelocity);

    void DestoryRigidBody(RigidBody* ptr);

    // 创建约束
    IConstraint* CreatePoint2PointConstraint(IRigidBody* rbA, IRigidBody* rbB, const Point2PointConstraintData& constraintData);
    IConstraint* CreatePoint2PointConstraint(IRigidBody* rbA, const Point2PointConstraintData& constraintData);
    IConstraint* CreateHingeConstraint(IRigidBody* rbA, IRigidBody* rbB, const HingeConstraintData& constraintData);
    IConstraint* CreateHingeConstraint(IRigidBody* rbA, const HingeConstraintData& constraintData);
    IConstraint* CreateConeTwistConstraint(IRigidBody* rbA, IRigidBody* rbB, const ConeTwistConstraintData& constraintData);
    IConstraint* CreateConeTwistConstraint(IRigidBody* rbA, const ConeTwistConstraintData& constraintData);
    IConstraint* CreateGeneric6DofConstraint(IRigidBody* rbA, IRigidBody* rbB, const Generic6DofConstraintData& constraintData);
    IConstraint* CreateGeneric6DofConstraint(IRigidBody* rbB, const Generic6DofConstraintData& constraintData);
    IConstraint* CreateGeneric6DofSpringConstraint(IRigidBody* rbA, IRigidBody* rbB, const Generic6DofSpringConstraintData& constraintData);
    IConstraint* CreateSliderConstraint(IRigidBody* rbA, IRigidBody* rbB, const SliderConstraintData& constraintData);
    IConstraint* CreateSliderConstraint(IRigidBody* rbB, const SliderConstraintData& constraintData);
    IConstraint* CreateGearConstraint(IRigidBody* rbA, IRigidBody* rbB, const GearConstraintData& constraintData);
    IConstraint* CreateFixConstraint(IRigidBody* rbA, IRigidBody* rbB, const FixConstraintData& constraintData);

    // 创建一个约束,将刚体与空间中一点相绑定
    IConstraint* CreatePositionConstraintP2P(IRigidBody* pB, const FmVec3& vPosition);

    // 创建一个约束,将刚体与空间中一点相绑定
    IConstraint* CreatePositionConstraintD6(IRigidBody* pB, const FmVec3& vPosition);

    // 删除约束
    void DestoryConstraint(Constraint* pConstraint);

    // 创建RAGDOLL
    IRagdoll* CreateRagdoll(const FmVec3& vPos, float scale);

    // 删除RAGDOLL
    void DestoryRagdoll(IRagdoll* pRagdoll);

    // 射线点选
    IRigidBody* RayTrace(const FmVec3& vRayFrom, const FmVec3& vRayTo, 
        FmVec3* vPickPos = 0, FmVec3* vPickNor = 0);

    void Pause(bool pause)
    {
        m_bPause = pause;
    }

    bool IsPaused() const
    {
        return m_bPause;
    }

    // 调试信息的渲染
    void SetDebugDrawModes(int modes);

    int GetDebugDrawModes() const;

    void DrawRigidBody(IRigidBody* pRigidBody, unsigned int color);

    void DrawDebug();

    performance_info_t* GetPerformance();

public:
    btDynamicsWorld* GetDynamicsWorld()
    {
        return m_pDynamicsWorld;
    }

    // 两个碰撞体发生碰撞时的回调,其本意是篡改btManifoldPoint,两个碰撞体碰撞时的反弹与摩擦属性的.
    static bool ContactAddedCallback(btManifoldPoint& cp, 
        const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, 
        const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1);

private:
    void LinkCollisionShape(CollisionShape* object);
    void UnlinkCollisionShape(CollisionShape* object);

    void LinkRigidBody(RigidBody* object);
    void UnlinkRigidBody(RigidBody* object);

    void LinkConstraint(Constraint* object);
    void UnlinkConstraint(Constraint* object);

    void LinkRagdoll(Ragdoll* object);
    void UnlinkRagdoll(Ragdoll* object);

    void DrawCollisionShape(const btCollisionShape* pBtShape, const FmMat4& mtxRB, unsigned int color, int debugMode);

private:
    btDispatcher* m_pBtDispatcher;
    btBroadphaseInterface* m_pBtBroadphaseInterface;
    btConstraintSolver* m_pBtConstraintSolver;
    btCollisionConfiguration* m_pBtCollisionConfiguration;

    btDynamicsWorld* m_pDynamicsWorld;

    CollisionShape* m_firstCollisionShape;
    CollisionShape* m_lastCollisionShape;
    long m_nCollisionShapesNum;

    RigidBody* m_firstRigidBody;
    RigidBody* m_lastRigidBody;
    long m_nRigidBodiesNum;
    long m_rigidBodyID;

    Constraint* m_firstConstraint;
    Constraint* m_lastConstraint;
    long m_nConstraintsNum;

    Ragdoll* m_firstRagdoll;
    Ragdoll* m_lastRagdoll;
    long m_nRagdollsNum;

    FmVec3 m_vGravity;

    float m_fSimulationTime;

    IRender* m_pRender;
    IPrimitive3DDrawer* m_pPrimitive3DDrawer;
    IPainter* m_pPainter;
    DebugDrawer m_debugDrawer;

    bool m_bPause;
    bool m_bUseContinuous;

    struct ShapeTM 
    {
        btCollisionShape*	pShape;
        btTransform			tm;
    };

    btAlignedObjectArray<ShapeTM> m_arrayShapeTM;
};

// --------------------------------------------------------------------------------------

extern PhysicsBullet* g_physicsBulletPtr;

// --------------------------------------------------------------------------------------

IVisBase* GetVisBase(const PERSISTID& id);

// --------------------------------------------------------------------------------------

#endif
