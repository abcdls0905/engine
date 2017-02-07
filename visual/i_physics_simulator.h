
/****************************************************************

  File name   :  i_physics_simulator.h
  Author      :  叶峰
  Version     :  1.0a
  Create Date :  2013/11/27  
  Description :  物理模拟接口

*****************************************************************/

#ifndef __i_physics_simulator_H__
#define __i_physics_simulator_H__

// --------------------------------------------------------------------------------------

#include "../public/i_interface.h"
#include "../visual/fm_define.h"
#include "i_collision_shape.h"
#include "i_rigid_body.h"
#include "i_constraint.h"
#include "i_ragdoll.h"

// --------------------------------------------------------------------------------------

class IPhysicsSimulator : public IInterface
{
public:
    // 上下高度的轴向
    enum HeightAxis
    {
        HEIGHT_AXIS_X,
        HEIGHT_AXIS_Y,
        HEIGHT_AXIS_Z,
    };

    virtual ~IPhysicsSimulator() {};

    // 启动动力学世界
    virtual bool StartSimulator() = 0;

    // 关闭动力学世界
    virtual void StopSimulator() = 0;

    // 是否就绪
    virtual bool IsReady() const = 0;

    // 设置全局重力
    virtual void SetGravity(const FmVec3& vGravity) = 0;

    // 设置是否使用连续物理(默认不开启)
    virtual void SetUseContinuous(bool continuous) = 0;

    /* 创建碰撞形 */
	// 长方体
    virtual ICollisionShape* CreateBoxCollisionShape(const FmVec3& vHalfExtend) = 0;

	// 立方体
    virtual ICollisionShape* CreateCubeCollisionShape(float fHalfExtend) = 0;

	// 球体
    virtual ICollisionShape* CreateSphereCollisionShape(float fRadius) = 0;

	// 圆柱
    virtual ICollisionShape* CreateCylinderCollisionShape(float fRadius, float fHeight, HeightAxis ha = HEIGHT_AXIS_Y) = 0;

	// 胶囊体
    virtual ICollisionShape* CreateCapsuleCollisionShape(float fRadius, float fHeight, HeightAxis ha = HEIGHT_AXIS_Y) = 0;

	// 锥体
    virtual ICollisionShape* CreateConeCollisionShape(float fRadius, float fHeight, HeightAxis ha = HEIGHT_AXIS_Y) = 0;

	// 凸多面体
    virtual ICollisionShape* CreateConvexCollisionShape(const void* vb, unsigned int verticesNum, unsigned int stride) = 0;

    // 开始创建一个组合形
    virtual void BeginCompoundCollisionShape(unsigned int count = 0) = 0;

    // 为组合形添加子形状
    virtual void AddChildCollisionShape(ICollisionShape* pShape, const FmMat4& mtx) = 0;

    // 完成组合形的创建
    virtual ICollisionShape* FinishCompoundCollisionShape() = 0;

    // 索引类型(16位索引或32位索引)
    enum IndexType
    {
        INDEX_INVALID		= 0,        
        INDEX_16_BIT		= 16,
        INDEX_32_BIT		= 32,
    };

    // 动态的Mesh
    virtual ICollisionShape* CreateDynamicMeshCollisionShape(const void* vb, unsigned int verticesNum, unsigned int vertexStride,
        const void* ib, unsigned int trianglesNum, unsigned int indexStride = 6, IndexType it = INDEX_16_BIT) = 0;

	// 平面(只能生成静态刚体)
    virtual ICollisionShape* CreatePlaneCollisionShape(const FmVec3& vNormal, float distance) = 0;

    // 静态的Mesh(只能生成静态刚体)
    virtual ICollisionShape* CreateStaticMeshCollisionShape(const void* vb, unsigned int verticesNum, unsigned int vertexStride,
        const void* ib, unsigned int trianglesNum, unsigned int indexStride = 6, IndexType it = INDEX_16_BIT) = 0;

    // 可缩放的静态Mesh(只能生成静态刚体)
    virtual ICollisionShape* CreateStaticScaledMeshCollisionShape(ICollisionShape* pStaticMeshCollisionShape, const FmVec3& vScale) = 0;

    // 地形
    enum TerrainDataType
    {
        TDT_FLOAT,
        TDT_DOUBLE,
        TDT_INTEGER,
        TDT_SHORT,
        TDT_FIXEDPOINT88,
        TDT_UCHAR,
    };
    virtual ICollisionShape* CreateTerrainCollisionShape(
        unsigned int row, unsigned int col, 
        const void* heightfieldData, float heightScale,
        float minHeight, float maxHeight,
        TerrainDataType heightDataType,
        bool flipQuadEdges = true, HeightAxis ha = HEIGHT_AXIS_Y) = 0;

    // 创建刚体
	// fMass小于等于0表示为静态刚体,并不是所有的碰撞形都能生成动态刚体
    // mtx不能有缩放信息
    virtual IRigidBody* CreateRigidBody(ICollisionShape* pShape, float fMass, const FmMat4& mtx, const FmVec3& vVelocity) = 0;

    // 创建约束
    virtual IConstraint* CreatePoint2PointConstraint(IRigidBody* rbA, IRigidBody* rbB, const Point2PointConstraintData& constraintData) = 0;
    virtual IConstraint* CreatePoint2PointConstraint(IRigidBody* rbA, const Point2PointConstraintData& constraintData) = 0;
    virtual IConstraint* CreateHingeConstraint(IRigidBody* rbA, IRigidBody* rbB, const HingeConstraintData& constraintData) = 0;
    virtual IConstraint* CreateHingeConstraint(IRigidBody* rbA, const HingeConstraintData& constraintData) = 0;
    virtual IConstraint* CreateConeTwistConstraint(IRigidBody* rbA, IRigidBody* rbB, const ConeTwistConstraintData& constraintData) = 0;
    virtual IConstraint* CreateConeTwistConstraint(IRigidBody* rbA, const ConeTwistConstraintData& constraintData) = 0;
    virtual IConstraint* CreateGeneric6DofConstraint(IRigidBody* rbA, IRigidBody* rbB, const Generic6DofConstraintData& constraintData) = 0;
    virtual IConstraint* CreateGeneric6DofConstraint(IRigidBody* rbB, const Generic6DofConstraintData& constraintData) = 0;
    virtual IConstraint* CreateGeneric6DofSpringConstraint(IRigidBody* rbA, IRigidBody* rbB, const Generic6DofSpringConstraintData& constraintData) = 0;
    virtual IConstraint* CreateSliderConstraint(IRigidBody* rbA, IRigidBody* rbB, const SliderConstraintData& constraintData) = 0;
    virtual IConstraint* CreateSliderConstraint(IRigidBody* rbB, const SliderConstraintData& constraintData) = 0;
    virtual IConstraint* CreateGearConstraint(IRigidBody* rbA, IRigidBody* rbB, const GearConstraintData& constraintData) = 0;
    virtual IConstraint* CreateFixConstraint(IRigidBody* rbA, IRigidBody* rbB, const FixConstraintData& constraintData) = 0;

    // 创建RAGDOLL
    virtual IRagdoll* CreateRagdoll(const FmVec3& vPos, float scale) = 0;

    // 射线点选
    virtual IRigidBody* RayTrace(
        const FmVec3& vRayFrom, const FmVec3& vRayTo, 
        FmVec3* vPickPos = 0, FmVec3* vPickNor = 0) = 0;

	// 暂停恢复
    virtual void Pause(bool pause) = 0;

    virtual bool IsPaused() const = 0;

    // 调试信息的渲染
    enum DebugDrawModes
    {
        DDM_NoDebug                 = 0,
        DDM_DrawWireframe           = 1,
        DDM_DrawAabb                = 2,
        DDM_DrawFeaturesText        = 4,
        DDM_DrawContactPoints       = 8,
        DDM_NoDeactivation          = 16,
        DDM_NoHelpText              = 32,
        DDM_DrawText                = 64,
        DDM_ProfileTimings          = 128,
        DDM_EnableSatComparison     = 256,
        DDM_DisableBulletLCP        = 512,
        DDM_EnableCCD               = 1024,
        DDM_DrawConstraints         = (1 << 11),
        DDM_DrawConstraintLimits    = (1 << 12),
        DDM_FastWireframe           = (1 << 13),
        DDM_DrawNormals             = (1 << 14),
        DDM_MAX_DEBUG_DRAW_MODE
    };

    virtual void SetDebugDrawModes(int modes) = 0;

    virtual int GetDebugDrawModes() const = 0;

    virtual void DrawRigidBody(IRigidBody* pRigidBody, unsigned int color) = 0;

    virtual void DrawDebug() = 0;

    virtual performance_info_t* GetPerformance() = 0;
};

// --------------------------------------------------------------------------------------

#endif
