
/****************************************************************

  File name   :  i_rigid_body.h
  Author      :  Ҷ��
  Version     :  1.0a
  Create Date :  2013/11/27  
  Description :  �������ӿ�

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

// �������������巢����ײ�󴥷��Ļص�
typedef void (*RigidBodyCollisionCallback)(IRigidBody* pRigidBody, float speed);

// ���弤�����Ա仯ʱ�����Ļص�
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

    // ���ø�����
    virtual void SetName(const char* szName) = 0;

    // ���ظ�����
    virtual const char* GetName() const = 0;

    // ����λ����ģ��λ�õ�ƫ��
    virtual bool SetOffset(FmVec3& offset) = 0;

    // ���ø���λ��
    virtual bool SetPosition(const FmVec3& pos) = 0;

    // ��ø���λ��
    virtual bool GetPosition(FmVec3& pos) const = 0;

    // ���ø�����ת
    virtual bool SetRotation(const FmQuat& rot) = 0;

    // ��ø�����ת
    virtual bool GetRotation(FmQuat& rot) const = 0;

    // ���ø���任����(�����в�Ӧ����������Ϣ)
    virtual bool SetTransform(const FmMat4& mtx) = 0;

    // ��ø���任����
    virtual bool GetTransform(FmMat4& mtx) const = 0;

    // ���ø���任
    virtual bool SetTransform(const FmVec3& position, const FmQuat& rot) = 0;

    // ��ø���任
    virtual bool GetTransform(FmVec3& position, FmQuat& rot) const = 0;

    // ���ø�������(�޸�MASSʱ���Զ��޸�����Գ���inertia tensor)
    // ��mass <= 0.0ʱ���Զ�����ı�Ϊ��̬����,ͬ����mass > 0.0ʱ�Ὣ��ı�Ϊ��̬����
    virtual bool SetMass(float mass) = 0;

    // ��ø�������
    virtual bool GetMass(float& mass) const = 0;

    // ��������(��������󣬸�����Ĭ�ϵ�����Ӱ�죬�����øú�����������)
    virtual bool ResetGravity(const FmVec3& vGravity) = 0;  

    // ��ȡ����
    virtual bool GetGravity(FmVec3& vGravity) const = 0;

    // ����λ�Ƶ�����
    virtual bool SetLinearFactor(const FmVec3& linFac) = 0;

    // ������ת������
    virtual bool SetAngularFactor(const FmVec3& angFac) = 0;

    // ���ٶ�����.(buletĬ��0.0)
    virtual bool GetLinearDamping(float& d) const = 0;

    virtual bool SetLinearDamping(float d) = 0;

    // ���ٶ�����.(buletĬ��0.0)
    virtual bool GetAngularDamping(float& d) const = 0;

    virtual bool SetAngularDamping(float d) = 0;

    virtual bool SetDamping(float linearD, float angularD) = 0;

    // ���ٶ�Sleep��ֵ.(buletĬ��0.8)
    virtual bool GetLinearSleepingThreshold(float& d) const = 0;

    virtual bool SetLinearSleepingThreshold(float d) = 0;

    // ���ٶ�Sleep��ֵ.(buletĬ��1.0)
    virtual bool GetAngularSleepingThreshold(float& d) const = 0;

    virtual bool SetAngularSleepingThreshold(float d) = 0;

    virtual bool SetSleepingThresholds(float linearD, float angularD) = 0;

    // ���嵱ǰ���ܵ����������ٶ���
    virtual bool GetTotalForce(FmVec3& vForce) const = 0;

    // ���嵱ǰ���ܵ������н��ٶ���
    virtual bool GetTotalTorque(FmVec3& vTorque) const = 0;

    // ��������ĳһλ��ʩ��һ����(����һֱ����,�����������ٶȺͽ��ٶ�)
    virtual bool ApplyForce(const FmVec3& force, const FmVec3& rel_pos) = 0;

    // ����������λ��ʩ��һ����(����һֱ����,ֻ���������ٶ�)
    virtual bool ApplyForce(const FmVec3& force) = 0;

    // ������ʩ��һ�����ٶ���(����һֱ����,ֻ��������ٶ�)
    virtual bool ApplyTorque(const FmVec3& torque) = 0;

    // ���������(����Ҳ�ᱻ���)
    virtual bool ClearForceAndTorque() = 0;

    // ������һ������,�൱�ڸ���һ������,���ı������ٶ�
    virtual bool ApplyCentralImpulse(const FmVec3& impulse) = 0;

    // ������һ������,�൱�ڸ���һ������,���ı�����ٶ�
    virtual bool ApplyTorqueImpulse(const FmVec3& torque) = 0;

    // ��������ĳһλ��ʩ��һ������,���ı������ٶȺͽ��ٶ�
    virtual bool ApplyImpulse(const FmVec3& impulse, const FmVec3& rel_pos) = 0;

    // ���ø������ٶ�
    virtual bool SetLinearVelocity(const FmVec3& vel) = 0;

    // ��ø������ٶ�
    virtual bool GetLinearVelocity(FmVec3& vel) const = 0;

    // ���ø�����ٶ�
    virtual bool SetAngularVelocity(const FmVec3& vel) = 0;

    // ��ø�����ٶ�
    virtual bool GetAngularVelocity(FmVec3& vel) const = 0;

    // ����ϵ��
    virtual bool SetRestitution(float rest) = 0;

    virtual bool GetRestitution(float& rest) const = 0;

    // ��Ħ��ϵ��
    virtual bool SetFriction(float frict) = 0;

    virtual bool GetFriction(float& frict) const = 0;

    // ����Ħ��ϵ��
    virtual bool SetRollingFriction(float frict) = 0;

    virtual bool GetRollingFriction(float& frict) const = 0;

    // ��ײ���Ա�־
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

    // ������ײ�������ֵ(Ĭ��Ϊ0.0,��ʾ������CCD)
    // ��Ҫ����IPhysicsSimulator::SetUseContinuous(true)�Ż���Ч
    // threshold: Don't do continuous collision detection if the motion (in one step) is less then threshold
    // ��ٷ�DEMO����ΪHALF_EXTENTS
    virtual bool SetCcdMotionThreshold(float threshold) = 0;

    virtual bool GetCcdMotionThreshold(float& threshold) const = 0;

    // Swept sphere radius (0.0 by default), see btConvexConvexAlgorithm
    // ��Ҫ����IPhysicsSimulator::SetUseContinuous(true)�Ż���Ч
    // ��ٷ�DEMO����ΪCcdMotionThreshold*0.9
    virtual bool SetCcdSweptSphereRadius(float radius) = 0;

    virtual bool GetCcdSweptSphereRadius(float& radius) const = 0;

    // �Ƿ�Ϊ��̬����
    virtual bool IsStatic() const = 0;
   
    // �Ƿ�Ϊ֡��������
    virtual bool IsKinematic() const = 0;

    // ֡���������붯̬�������л�
    virtual bool EnableKinematic(bool kinematic) = 0;

    // ���÷Ǽ���ʱ��
    virtual bool SetDeactivationTime(float time) = 0;

    virtual bool GetDeactivationTime(float& time) const = 0;

    // ����ļ���״̬
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

    // �����������ת��Ϊ���������
    virtual bool ConvertPosFromWorldToLocal(const FmVec3& vWorldPos, FmVec3& vLocalPos) const = 0;

    // �����������ת��Ϊ���������
    virtual bool ConvertPosFromLocalToWorld(const FmVec3& vLocalPos, FmVec3& vWorldPos) const = 0;

    // �û�����
    virtual void SetUserPointer(void* userPtr) = 0;

    virtual void* GetUserPointer() const = 0;

    // �Ƿ�ɼ�(����ʱʹ��)
    virtual void SetVisible(bool visible) = 0;

    virtual bool IsVisible() const = 0;

    // ���ø���İ󶨶���,����ʹ����λ�÷����仯ʱ,�Զ�֪ͨ��󶨶���
    // updateBinding�Ƿ�bindingID�ı任��������Ϊ��ǰ����ľ���
    virtual void SetBindingObj(const PERSISTID& bindingID, bool updateBinding = false) = 0;

    virtual PERSISTID GetBindingObj() = 0;

    // �������������巢����ײ�󴥷��Ļص�
    // ��Ҫ������SetCollisionFlags(CNF_CUSTOM_MATERIAL_CALLBACK)���ܴ���
    virtual void SetCollisionCallback(RigidBodyCollisionCallback callback) = 0;

    // ���弤�����Ա仯ʱ�����Ļص�
    virtual void SetActivationStateCallback(RigidBodyActivationStateCallback callback) = 0;
};

// --------------------------------------------------------------------------------------

#endif
