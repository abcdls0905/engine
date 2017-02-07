
/****************************************************************

  File name   :  RigidBody.h
  Author      :  叶峰
  Version     :  1.0a
  Create Date :  2013/10/22  
  Description :  物理刚体

*****************************************************************/

#ifndef __RigidBody_H__
#define __RigidBody_H__

// --------------------------------------------------------------------------------------

#include "../visual/i_rigid_body.h"

// --------------------------------------------------------------------------------------

#define YD_RIGIDBODY_NAME_SIZE 32

// --------------------------------------------------------------------------------------

class btRigidBody;
class CollisionShape;

// --------------------------------------------------------------------------------------

class RigidBody : public IRigidBody
{
public:
    RigidBody(btRigidBody* pBtRigidBody, CollisionShape* pCollisionShape = 0, long id = 0, const char* szName = 0);

    ~RigidBody();

    void Release();

    void AddRef();

    ICollisionShape* GetCollisionShape();

    bool IsReady() const;

    long GetID() const
    {
        return m_id;
    }

    // 设置刚体名
    void SetName(const char* szName);

    // 返回刚体名
    const char* GetName() const;

    // 刚体位置与模型位置的偏移
    bool SetOffset(FmVec3& offset);

    const FmVec3* GetOffset() const;

    // 设置刚体位置
    bool SetPosition(const FmVec3& pos);

    // 获得刚体位置
    bool GetPosition(FmVec3& pos) const;

    // 设置刚体旋转
    bool SetRotation(const FmQuat& rot);

    // 获得刚体旋转
    bool GetRotation(FmQuat& rot) const;

    // 设置刚体变换矩阵
    bool SetTransform(const FmMat4& mtx);

    // 获得刚体变换矩阵
    bool GetTransform(FmMat4& mtx) const;

    // 设置刚体变换
    bool SetTransform(const FmVec3& position, const FmQuat& rot);

    // 获得刚体变换
    bool GetTransform(FmVec3& position, FmQuat& rot) const;

    // 获得刚体变换实际位置
    bool GetPositionRB(FmVec3& pos) const;

    // 获得刚体变换实际矩阵
    bool GetTransformRB(FmMat4& mtx) const;

    // 设置刚体重量(修改MASS时会自动修改其惯性常量inertia tensor)
    bool SetMass(float mass);

    // 获得刚体重量
    bool GetMass(float& mass) const;

    // 重设重力
    bool ResetGravity(const FmVec3& acceleration);  

    // 获取重力
    bool GetGravity(FmVec3& vGravity) const;

    // 设置平移的限制
    bool SetLinearFactor(const FmVec3& linFac);

    // 设置旋转的限制
    bool SetAngularFactor(const FmVec3& angFac);

    // 线速度阻尼.
    bool GetLinearDamping(float& d) const;

    bool SetLinearDamping(float d);

    // 角速度阻尼.
    bool GetAngularDamping(float& d) const;

    bool SetAngularDamping(float d);

    bool SetDamping(float linearD, float angularD);

    // 线速度Sleep阈值.
    bool GetLinearSleepingThreshold(float& d) const;

    bool SetLinearSleepingThreshold(float d);

    // 角速度Sleep阈值.
    bool GetAngularSleepingThreshold(float& d) const;

    bool SetAngularSleepingThreshold(float d);

    bool SetSleepingThresholds(float linearD, float angularD);

    // 刚体当前所受到的所有线速度力
    bool GetTotalForce(FmVec3& vForce) const;

    // 刚体当前所受到的所有角速度力
    bool GetTotalTorque(FmVec3& vTorque) const;

    // 给刚体上某一位置施加一个力(力会一直存在,并增加其线速度和角速度)
    bool ApplyForce(const FmVec3& force, const FmVec3& rel_pos);

    // 给刚体中心位置施加一个力(力会一直存在,只增加其线速度)
    bool ApplyForce(const FmVec3& force);

    // 给刚体施加一个角速度力(力会一直存在,只增加其角速度)
    bool ApplyTorque(const FmVec3& torque);

    // 清空所有力(重力也会被清空)
    bool ClearForceAndTorque();

    // 给刚体一个冲量,相当于给它一个动能,将改变其线速度
    bool ApplyCentralImpulse(const FmVec3& impulse);

    // 给刚体一个冲量,相当于给它一个动能,将改变其角速度
    bool ApplyTorqueImpulse(const FmVec3& torque);

    // 给刚体上某一位置施加一个冲量,将改变其线速度和角速度
    bool ApplyImpulse(const FmVec3& impulse, const FmVec3& rel_pos);

    // 设置刚体线速度
    bool SetLinearVelocity(const FmVec3& vel);

    // 获得刚体线速度
    bool GetLinearVelocity(FmVec3& vel) const;

    // 设置刚体角速度
    bool SetAngularVelocity(const FmVec3& vel);

    // 获得刚体角速度
    bool GetAngularVelocity(FmVec3& vel) const;

    // 反弹系数
    bool SetRestitution(float rest);

    bool GetRestitution(float& rest) const;

    // 静摩擦系数
    bool SetFriction(float frict);

    bool GetFriction(float& frict) const;

    // 滚动摩擦系数
    bool SetRollingFriction(float frict);

    bool GetRollingFriction(float& frict) const;

    // 碰撞属性标志
    bool SetCollisionFlags(int flags);

    bool GetCollisionFlags(int& flags) const;

    // 连续碰撞处理的阈值
    bool SetCcdMotionThreshold(float threshold);

    bool GetCcdMotionThreshold(float& threshold) const;

    // Swept sphere radius (0.0 by default), see btConvexConvexAlgorithm::
    bool SetCcdSweptSphereRadius(float radius);

    bool GetCcdSweptSphereRadius(float& radius) const;

    // 是否为静态刚体
    bool IsStatic() const;

    // 是否为帧驱动刚体
    bool IsKinematic() const;

    // 帧驱动刚体与动态刚体间的切换
    bool EnableKinematic(bool kinematic);

    // 设置非激活时间
    bool SetDeactivationTime(float time);

    bool GetDeactivationTime(float& time) const;    

    // 刚体的激活状态
    bool SetActivationState(ActivationState state);

    bool GetActivationState(ActivationState& state) const;

    // 将世界坐标点转化为本地坐标点
    bool ConvertPosFromWorldToLocal(const FmVec3& vWorldPos, FmVec3& vLocalPos) const;

    // 将本地坐标点转化为世界坐标点
    bool ConvertPosFromLocalToWorld(const FmVec3& vLocalPos, FmVec3& vWorldPos) const;

	// 刚体标志
	bool SetFlags(int flags);

	bool GetFlags(int& flags) const;

    // 用户数据
    void SetUserPointer(void* userPtr);

    void* GetUserPointer() const;

    void SetVisible(bool visible)
    {
        m_bVisible = visible;
    }

    bool IsVisible() const
    {
        return m_bVisible;
    }

    // 设置刚体的绑定对象,可以使刚体位置发生变化时,自动通知其绑定对象
    void SetBindingObj(const PERSISTID& bindingID, bool updateBinding = false);

    PERSISTID GetBindingObj();

    // 刚体与其他刚体发生碰撞后触发的回调
    void SetCollisionCallback(RigidBodyCollisionCallback callback)
    {
        m_funcCollisionCallback = callback;
    }

    RigidBodyCollisionCallback GetCollisionCallback()
    {
        return m_funcCollisionCallback;
    }

    // 刚体激活属性变化时触发的回调
    void SetActivationStateCallback(RigidBodyActivationStateCallback callback)
    {
        m_funcActiveCallback = callback;
    }

    RigidBodyActivationStateCallback GetActivationStateCallback()
    {
        return m_funcActiveCallback;
    }

    void UpdateState();

public:
    btRigidBody* m_pBtRigidBody;
    char m_szName[YD_RIGIDBODY_NAME_SIZE];
    CollisionShape* m_pCollisionShape;
    void* m_pUserPointer;
    RigidBody* m_next;
    RigidBody* m_prev;
    long m_id;
    long m_ref;
    bool m_bVisible;

    RigidBodyCollisionCallback m_funcCollisionCallback;
    RigidBodyActivationStateCallback m_funcActiveCallback;

    ActivationState m_activationState;
};

// --------------------------------------------------------------------------------------

#endif
