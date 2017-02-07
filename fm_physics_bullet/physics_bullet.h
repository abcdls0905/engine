
/****************************************************************

  File name   :  PhysicsBullet.h
  Author      :  Ҷ��
  Version     :  1.0a
  Create Date :  2013/10/22   
  Description :  Bullet����ϵͳ

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

	// ��ʼִ��
	virtual bool Init();

	virtual bool Shut();

	// �Ƿ���Ҫÿ֡���У�����ExecFrameBegin��ExecFrameEnd��
	virtual bool NeedExecPerFrame() { return true; }
	// ÿ֡��ʼʱ����
	virtual void ExecFrameBegin();
	// ÿ֡����ʱ����
	virtual void ExecFrameEnd();

    // ��������ѧ����
    bool StartSimulator();

    // �رն���ѧ����
    void StopSimulator();

    // �Ƿ����
    bool IsReady() const;

    // ��������
    void SetGravity(const FmVec3& vGravity);

    // �����Ƿ�ʹ����������
    void SetUseContinuous(bool continuous);

    // ������ײ��
    ICollisionShape* CreateBoxCollisionShape(const FmVec3& vHalfExtend);

    ICollisionShape* CreateCubeCollisionShape(float fHalfExtend);

    ICollisionShape* CreateSphereCollisionShape(float fRadius);

    ICollisionShape* CreateCylinderCollisionShape(float fRadius, float fHeight, HeightAxis ha = HEIGHT_AXIS_Y);

    ICollisionShape* CreateCapsuleCollisionShape(float fRadius, float fHeight, HeightAxis ha = HEIGHT_AXIS_Y);

    ICollisionShape* CreateConeCollisionShape(float fRadius, float fHeight, HeightAxis ha = HEIGHT_AXIS_Y);

    // ͹������
    ICollisionShape* CreateConvexCollisionShape(const void* vb, unsigned int verticesNum, unsigned int stride);

    // ��ʼ����һ�������
    void BeginCompoundCollisionShape(unsigned int count = 0);

    // Ϊ������������״
    void AddChildCollisionShape(ICollisionShape* pShape, const FmMat4& mtx);

    // �������εĴ���
    ICollisionShape* FinishCompoundCollisionShape();

    // ��̬��Mesh
    ICollisionShape* CreateDynamicMeshCollisionShape(const void* vb, unsigned int verticesNum, unsigned int vertexStride,
        const void* ib, unsigned int trianglesNum, unsigned int indexStride = 6, IndexType it = INDEX_16_BIT);

    // ƽ��(ֻ�����ɾ�̬����)
    ICollisionShape* CreatePlaneCollisionShape(const FmVec3& vNormal, float distance);

    // ��̬��Mesh(ֻ�����ɾ�̬����)
    ICollisionShape* CreateStaticMeshCollisionShape(const void* vb, unsigned int verticesNum, unsigned int vertexStride,
        const void* ib, unsigned int trianglesNum, unsigned int indexStride = 6, IndexType it = INDEX_16_BIT);

    // �����ŵľ�̬Mesh(ֻ�����ɾ�̬����)
    ICollisionShape* CreateStaticScaledMeshCollisionShape(ICollisionShape* pStaticMeshCollisionShape, const FmVec3& vScale);

    // ����(ֻ�����ɾ�̬����)
    ICollisionShape* CreateTerrainCollisionShape(
        unsigned int row, unsigned int col, 
        const void* heightfieldData, float heightScale,
        float minHeight, float maxHeight,
        TerrainDataType heightDataType,
        bool flipQuadEdges = true, HeightAxis ha = HEIGHT_AXIS_Y);

    CollisionShape* FindCollisionShape(YeCollisionShapeType shapeType, HeightAxis ha, float params[4]);

    void DestoryCollisionShape(CollisionShape* ptr);

    void ReleaseCollisionShape(btCollisionShape* ptr);

    // ��������,fMassС�ڵ���0��ʾΪ��̬����
    IRigidBody* CreateRigidBody(ICollisionShape* pShape, float fMass, const FmMat4& mtx, const FmVec3& vVelocity);

    void DestoryRigidBody(RigidBody* ptr);

    // ����Լ��
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

    // ����һ��Լ��,��������ռ���һ�����
    IConstraint* CreatePositionConstraintP2P(IRigidBody* pB, const FmVec3& vPosition);

    // ����һ��Լ��,��������ռ���һ�����
    IConstraint* CreatePositionConstraintD6(IRigidBody* pB, const FmVec3& vPosition);

    // ɾ��Լ��
    void DestoryConstraint(Constraint* pConstraint);

    // ����RAGDOLL
    IRagdoll* CreateRagdoll(const FmVec3& vPos, float scale);

    // ɾ��RAGDOLL
    void DestoryRagdoll(IRagdoll* pRagdoll);

    // ���ߵ�ѡ
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

    // ������Ϣ����Ⱦ
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

    // ������ײ�巢����ײʱ�Ļص�,�䱾���Ǵ۸�btManifoldPoint,������ײ����ײʱ�ķ�����Ħ�����Ե�.
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
