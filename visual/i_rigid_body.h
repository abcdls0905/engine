
/****************************************************************

  File name   :  i_rigid_body.h
  Author      :  叶峰
  Version     :  1.0a
  Create Date :  2013/11/27  
  Description :  物理刚体接口

*****************************************************************/

#ifndef __i_rigid_body_H__
#define __i_rigid_body_H__

// --------------------------------------------------------------------------------------

#include "../math/fm_vector.h"
#include "../math/fm_matrix.h"
#include "../math/fm_quaternion.h"
#include "../public/persistid.h"

// --------------------------------------------------------------------------------------

class ICollisionShape;
class IVisBase;
class IRigidBody;

// --------------------------------------------------------------------------------------

// 刚体与其他刚体发生碰撞后触发的回调
typedef void (*RigidBodyCollisionCallback)(IRigidBody* pRigidBody, float speed);

// 刚体激活属性变化时触发的回调
typedef void (*RigidBodyActivationStateCallback)(IRigidBody* pRigidBody, int stateOld, int stateNew);

// --------------------------------------------------------------------------------------

class IRigidBody
{
public:
    virtual ~IRigidBody() {}

    virtual void Release() = 0;

    virtual ICollisionShape* GetCollisionShape() = 0;

    virtual bool IsReady() const = 0;

    virtual long GetID() const = 0;

    // 设置刚体名
    virtual void SetName(const char* szName) = 0;

    // 返回刚体名
    virtual const char* GetName() const = 0;

    // 刚体位置与模型位置的偏移
    virtual bool SetOffset(FmVec3& offset) = 0;

    // 设置刚体位置
    virtual bool SetPosition(const FmVec3& pos) = 0;

    // 获得刚体位置
    virtual bool GetPosition(FmVec3& pos) const = 0;

    // 设置刚体旋转
    virtual bool SetRotation(const FmQuat& rot) = 0;

    // 获得刚体旋转
    virtual bool GetRotation(FmQuat& rot) const = 0;

    // 设置刚体变换矩阵(矩阵中不应该有缩放信息)
    virtual bool SetTransform(const FmMat4& mtx) = 0;

    // 获得刚体变换矩阵
    virtual bool GetTransform(FmMat4& mtx) const = 0;

    // 设置刚体变换
    virtual bool SetTransform(const FmVec3& position, const FmQuat& rot) = 0;

    // 获得刚体变换
    virtual bool GetTransform(FmVec3& position, FmQuat& rot) const = 0;

    // 设置刚体重量(修改MASS时会自动修改其惯性常量inertia tensor)
    // 当mass <= 0.0时会自动将其改变为静态刚体,同样当mass > 0.0时会将其改变为动态刚体
    virtual bool SetMass(float mass) = 0;

    // 获得刚体重量
    virtual bool GetMass(float& mass) const = 0;

    // 重设重力(创建刚体后，刚体受默认的重力影响，而调用该函数重设重力)
    virtual bool ResetGravity(const FmVec3& vGravity) = 0;  

    // 获取重力
    virtual bool GetGravity(FmVec3& vGravity) const = 0;

    // 设置位移的限制
    virtual bool SetLinearFactor(const FmVec3& linFac) = 0;

    // 设置旋转的限制
    virtual bool SetAngularFactor(const FmVec3& angFac) = 0;

    // 线速度阻尼.(bulet默认0.0)
    virtual bool GetLinearDamping(float& d) const = 0;

    virtual bool SetLinearDamping(float d) = 0;

    // 角速度阻尼.(bulet默认0.0)
    virtual bool GetAngularDamping(float& d) const = 0;

    virtual bool SetAngularDamping(float d) = 0;

    virtual bool SetDamping(float linearD, float angularD) = 0;

    // 线速度Sleep阈值.(bulet默认0.8)
    virtual bool GetLinearSleepingThreshold(float& d) const = 0;

    virtual bool SetLinearSleepingThreshold(float d) = 0;

    // 角速度Sleep阈值.(bulet默认1.0)
    virtual bool GetAngularSleepingThreshold(float& d) const = 0;

    virtual bool SetAngularSleepingThreshold(float d) = 0;

    virtual bool SetSleepingThresholds(float linearD, float angularD) = 0;

    // 刚体当前所受到的所有线速度力
    virtual bool GetTotalForce(FmVec3& vForce) const = 0;

    // 刚体当前所受到的所有角速度力
    virtual bool GetTotalTorque(FmVec3& vTorque) const = 0;

    // 给刚体上某一位置施加一个力(力会一直存在,并增加其线速度和角速度)
    virtual bool ApplyForce(const FmVec3& force, const FmVec3& rel_pos) = 0;

    // 给刚体中心位置施加一个力(力会一直存在,只增加其线速度)
    virtual bool ApplyForce(const FmVec3& force) = 0;

    // 给刚体施加一个角速度力(力会一直存在,只增加其角速度)
    virtual bool ApplyTorque(const FmVec3& torque) = 0;

    // 清空所有力(重力也会被清空)
    virtual bool ClearForceAndTorque() = 0;

    // 给刚体一个冲量,相当于给它一个动能,将改变其线速度
    virtual bool ApplyCentralImpulse(const FmVec3& impulse) = 0;

    // 给刚体一个冲量,相当于给它一个动能,将改变其角速度
    virtual bool ApplyTorqueImpulse(const FmVec3& torque) = 0;

    // 给刚体上某一位置施加一个冲量,将改变其线速度和角速度
    virtual bool ApplyImpulse(const FmVec3& impulse, const FmVec3& rel_pos) = 0;

    // 设置刚体线速度
    virtual bool SetLinearVelocity(const FmVec3& vel) = 0;

    // 获得刚体线速度
    virtual bool GetLinearVelocity(FmVec3& vel) const = 0;

    // 设置刚体角速度
    virtual bool SetAngularVelocity(const FmVec3& vel) = 0;

    // 获得刚体角速度
    virtual bool GetAngularVelocity(FmVec3& vel) const = 0;

    // 反弹系数
    virtual bool SetRestitution(float rest) = 0;

    virtual bool GetRestitution(float& rest) const = 0;

    // 静摩擦系数
    virtual bool SetFriction(float frict) = 0;

    virtual bool GetFriction(float& frict) const = 0;

    // 滚动摩擦系数
    virtual bool SetRollingFriction(float frict) = 0;

    virtual bool GetRollingFriction(float& frict) const = 0;

    // 碰撞属性标志
    enum CollisionFlag
    {
        CNF_STATIC_OBJECT                    = 1,
        CNF_KINEMATIC_OBJECT                 = 2,
        CNF_NO_CONTACT_RESPONSE              = 4,
        CNF_CUSTOM_MATERIAL_CALLBACK         = 8,   //this allows per-triangle material (friction/restitution)
        CNF_CHARACTER_OBJECT                 = 16,
        CNF_DISABLE_VISUALIZE_OBJECT         = 32,  //disable debug drawing
        CNF_DISABLE_SPU_COLLISION_PROCESSING = 64,  //disable parallel/SPU processing
    };
    virtual bool SetCollisionFlags(int flags) = 0;

    virtual bool GetCollisionFlags(int& flags) const = 0;

    // 连续碰撞处理的阈值(默认为0.0,表示不开启CCD)
    // 需要开启IPhysicsSimulator::SetUseContinuous(true)才会有效
    // threshold: Don't do continuous collision detection if the motion (in one step) is less then threshold
    // 其官方DEMO设置为HALF_EXTENTS
    virtual bool SetCcdMotionThreshold(float threshold) = 0;

    virtual bool GetCcdMotionThreshold(float& threshold) const = 0;

    // Swept sphere radius (0.0 by default), see btConvexConvexAlgorithm
    // 需要开启IPhysicsSimulator::SetUseContinuous(true)才会有效
    // 其官方DEMO设置为CcdMotionThreshold*0.9
    virtual bool SetCcdSweptSphereRadius(float radius) = 0;

    virtual bool GetCcdSweptSphereRadius(float& radius) const = 0;

    // 是否为静态刚体
    virtual bool IsStatic() const = 0;
   
    // 是否为帧驱动刚体
    virtual bool IsKinematic() const = 0;

    // 帧驱动刚体与动态刚体间的切换
    virtual bool EnableKinematic(bool kinematic) = 0;

    // 设置非激活时间
    virtual bool SetDeactivationTime(float time) = 0;

    virtual bool GetDeactivationTime(float& time) const = 0;

    // 刚体的激活状态
    enum ActivationState
    {
        AS_INVALID              = 0,
        AS_ACTIVE_TAG           = 1,
        AS_ISLAND_SLEEPING      = 2,
        AS_WANTS_DEACTIVATION   = 3,
        AS_DISABLE_DEACTIVATION = 4,
        AS_DISABLE_SIMULATION   = 5,
    };
    virtual bool SetActivationState(ActivationState state) = 0;

    virtual bool GetActivationState(ActivationState& state) const = 0;

    // 将世界坐标点转化为本地坐标点
    virtual bool ConvertPosFromWorldToLocal(const FmVec3& vWorldPos, FmVec3& vLocalPos) const = 0;

    // 将本地坐标点转化为世界坐标点
    virtual bool ConvertPosFromLocalToWorld(const FmVec3& vLocalPos, FmVec3& vWorldPos) const = 0;

    // 用户数据
    virtual void SetUserPointer(void* userPtr) = 0;

    virtual void* GetUserPointer() const = 0;

    // 是否可见(调试时使用)
    virtual void SetVisible(bool visible) = 0;

    virtual bool IsVisible() const = 0;

    // 设置刚体的绑定对象,可以使刚体位置发生变化时,自动通知其绑定对象
    // updateBinding是否将bindingID的变换矩阵设置为当前刚体的矩阵
    virtual void SetBindingObj(const PERSISTID& bindingID, bool updateBinding = false) = 0;

    virtual PERSISTID GetBindingObj() = 0;

    // 刚体与其他刚体发生碰撞后触发的回调
    // 需要先设置SetCollisionFlags(CNF_CUSTOM_MATERIAL_CALLBACK)才能触发
    virtual void SetCollisionCallback(RigidBodyCollisionCallback callback) = 0;

    // 刚体激活属性变化时触发的回调
    virtual void SetActivationStateCallback(RigidBodyActivationStateCallback callback) = 0;
};

// --------------------------------------------------------------------------------------

#endif
