
/****************************************************************

  File name   :  RigidBody.h
  Author      :  Ҷ��
  Version     :  1.0a
  Create Date :  2013/10/22  
  Description :  �������

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

    // ���ø�����
    void SetName(const char* szName);

    // ���ظ�����
    const char* GetName() const;

    // ����λ����ģ��λ�õ�ƫ��
    bool SetOffset(FmVec3& offset);

    const FmVec3* GetOffset() const;

    // ���ø���λ��
    bool SetPosition(const FmVec3& pos);

    // ��ø���λ��
    bool GetPosition(FmVec3& pos) const;

    // ���ø�����ת
    bool SetRotation(const FmQuat& rot);

    // ��ø�����ת
    bool GetRotation(FmQuat& rot) const;

    // ���ø���任����
    bool SetTransform(const FmMat4& mtx);

    // ��ø���任����
    bool GetTransform(FmMat4& mtx) const;

    // ���ø���任
    bool SetTransform(const FmVec3& position, const FmQuat& rot);

    // ��ø���任
    bool GetTransform(FmVec3& position, FmQuat& rot) const;

    // ��ø���任ʵ��λ��
    bool GetPositionRB(FmVec3& pos) const;

    // ��ø���任ʵ�ʾ���
    bool GetTransformRB(FmMat4& mtx) const;

    // ���ø�������(�޸�MASSʱ���Զ��޸�����Գ���inertia tensor)
    bool SetMass(float mass);

    // ��ø�������
    bool GetMass(float& mass) const;

    // ��������
    bool ResetGravity(const FmVec3& acceleration);  

    // ��ȡ����
    bool GetGravity(FmVec3& vGravity) const;

    // ����ƽ�Ƶ�����
    bool SetLinearFactor(const FmVec3& linFac);

    // ������ת������
    bool SetAngularFactor(const FmVec3& angFac);

    // ���ٶ�����.
    bool GetLinearDamping(float& d) const;

    bool SetLinearDamping(float d);

    // ���ٶ�����.
    bool GetAngularDamping(float& d) const;

    bool SetAngularDamping(float d);

    bool SetDamping(float linearD, float angularD);

    // ���ٶ�Sleep��ֵ.
    bool GetLinearSleepingThreshold(float& d) const;

    bool SetLinearSleepingThreshold(float d);

    // ���ٶ�Sleep��ֵ.
    bool GetAngularSleepingThreshold(float& d) const;

    bool SetAngularSleepingThreshold(float d);

    bool SetSleepingThresholds(float linearD, float angularD);

    // ���嵱ǰ���ܵ����������ٶ���
    bool GetTotalForce(FmVec3& vForce) const;

    // ���嵱ǰ���ܵ������н��ٶ���
    bool GetTotalTorque(FmVec3& vTorque) const;

    // ��������ĳһλ��ʩ��һ����(����һֱ����,�����������ٶȺͽ��ٶ�)
    bool ApplyForce(const FmVec3& force, const FmVec3& rel_pos);

    // ����������λ��ʩ��һ����(����һֱ����,ֻ���������ٶ�)
    bool ApplyForce(const FmVec3& force);

    // ������ʩ��һ�����ٶ���(����һֱ����,ֻ��������ٶ�)
    bool ApplyTorque(const FmVec3& torque);

    // ���������(����Ҳ�ᱻ���)
    bool ClearForceAndTorque();

    // ������һ������,�൱�ڸ���һ������,���ı������ٶ�
    bool ApplyCentralImpulse(const FmVec3& impulse);

    // ������һ������,�൱�ڸ���һ������,���ı�����ٶ�
    bool ApplyTorqueImpulse(const FmVec3& torque);

    // ��������ĳһλ��ʩ��һ������,���ı������ٶȺͽ��ٶ�
    bool ApplyImpulse(const FmVec3& impulse, const FmVec3& rel_pos);

    // ���ø������ٶ�
    bool SetLinearVelocity(const FmVec3& vel);

    // ��ø������ٶ�
    bool GetLinearVelocity(FmVec3& vel) const;

    // ���ø�����ٶ�
    bool SetAngularVelocity(const FmVec3& vel);

    // ��ø�����ٶ�
    bool GetAngularVelocity(FmVec3& vel) const;

    // ����ϵ��
    bool SetRestitution(float rest);

    bool GetRestitution(float& rest) const;

    // ��Ħ��ϵ��
    bool SetFriction(float frict);

    bool GetFriction(float& frict) const;

    // ����Ħ��ϵ��
    bool SetRollingFriction(float frict);

    bool GetRollingFriction(float& frict) const;

    // ��ײ���Ա�־
    bool SetCollisionFlags(int flags);

    bool GetCollisionFlags(int& flags) const;

    // ������ײ�������ֵ
    bool SetCcdMotionThreshold(float threshold);

    bool GetCcdMotionThreshold(float& threshold) const;

    // Swept sphere radius (0.0 by default), see btConvexConvexAlgorithm::
    bool SetCcdSweptSphereRadius(float radius);

    bool GetCcdSweptSphereRadius(float& radius) const;

    // �Ƿ�Ϊ��̬����
    bool IsStatic() const;

    // �Ƿ�Ϊ֡��������
    bool IsKinematic() const;

    // ֡���������붯̬�������л�
    bool EnableKinematic(bool kinematic);

    // ���÷Ǽ���ʱ��
    bool SetDeactivationTime(float time);

    bool GetDeactivationTime(float& time) const;    

    // ����ļ���״̬
    bool SetActivationState(ActivationState state);

    bool GetActivationState(ActivationState& state) const;

    // �����������ת��Ϊ���������
    bool ConvertPosFromWorldToLocal(const FmVec3& vWorldPos, FmVec3& vLocalPos) const;

    // �����������ת��Ϊ���������
    bool ConvertPosFromLocalToWorld(const FmVec3& vLocalPos, FmVec3& vWorldPos) const;

	// �����־
	bool SetFlags(int flags);

	bool GetFlags(int& flags) const;

    // �û�����
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

    // ���ø���İ󶨶���,����ʹ����λ�÷����仯ʱ,�Զ�֪ͨ��󶨶���
    void SetBindingObj(const PERSISTID& bindingID, bool updateBinding = false);

    PERSISTID GetBindingObj();

    // �������������巢����ײ�󴥷��Ļص�
    void SetCollisionCallback(RigidBodyCollisionCallback callback)
    {
        m_funcCollisionCallback = callback;
    }

    RigidBodyCollisionCallback GetCollisionCallback()
    {
        return m_funcCollisionCallback;
    }

    // ���弤�����Ա仯ʱ�����Ļص�
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
