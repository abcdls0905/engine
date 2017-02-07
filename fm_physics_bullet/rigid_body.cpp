
/****************************************************************

  File name   :  RigidBody.cpp
  Author      :  Ҷ��
  Version     :  1.0a
  Create Date :  2013/10/22  
  Description :  �������

*****************************************************************/

// --------------------------------------------------------------------------------------

#include "bullet_dynamics/dynamics/bt_rigid_body.h"
#include "bullet_collision/collision_shapes/bt_collision_shape.h"
#include "common.h"
#include "physics_bullet.h"
#include "collision_shape.h"
#include "my_motion_state.h"
#include "rigid_body.h"

#include <assert.h>

// --------------------------------------------------------------------------------------

RigidBody::RigidBody(btRigidBody* pBtRigidBody, CollisionShape* pCollisionShape, long id, const char* szName)
{
    assert(pBtRigidBody);
    m_pBtRigidBody = pBtRigidBody;
    m_pBtRigidBody->setUserPointer(this);
    m_pUserPointer = NULL;
    m_pCollisionShape = pCollisionShape;
    m_next = NULL;
    m_prev = NULL;
    SetName(szName);
    m_id = id;
    m_ref = 1;
    m_bVisible = false;

    m_funcCollisionCallback = NULL;
    m_funcActiveCallback = NULL;

    m_activationState = (ActivationState)m_pBtRigidBody->getActivationState();
}

RigidBody::~RigidBody() 
{
    
}

void    RigidBody::Release()
{
    m_ref--;
    if (m_ref == 0)
    {
        g_physicsBulletPtr->DestoryRigidBody(this);
    }
}

void    RigidBody::AddRef()
{
    m_ref++;
}

ICollisionShape* RigidBody::GetCollisionShape()
{
    return m_pCollisionShape;
}

bool RigidBody::IsReady() const
{
    return (m_pBtRigidBody != 0);
}

// ���ø�����
void RigidBody::SetName(const char* szName)
{
    if (szName)
    {
        strncpy(m_szName, szName, YD_RIGIDBODY_NAME_SIZE);
    }
    else
    {
        memset(m_szName, 0, YD_RIGIDBODY_NAME_SIZE);
    }
}

// ���ظ�����
const char* RigidBody::GetName() const
{
    return m_szName;
}

// ����λ����ģ��λ�õ�ƫ��
bool RigidBody::SetOffset(FmVec3& offset)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    btMotionState* pMS = m_pBtRigidBody->getMotionState();
    if (!pMS)
    {
        return false;
    }

    ((MyMotionState*)pMS)->m_offset = offset;

    return true;
}

const FmVec3* RigidBody::GetOffset() const
{
    btMotionState* pMS = m_pBtRigidBody->getMotionState();
    if (!pMS)
    {
        return NULL;
    }
    
    return &((MyMotionState*)pMS)->m_offset;
}

// ���ø���λ��
bool RigidBody::SetPosition(const FmVec3& pos)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    btTransform tm = m_pBtRigidBody->getCenterOfMassTransform();
    const FmVec3* pOffset = GetOffset();
    if (!pOffset)
    {
        tm.setOrigin(FmVec3ToBtVector3(pos));
        m_pBtRigidBody->setCenterOfMassTransform(tm);
    }
    else
    {
        FmMat4 mtx;
        tm.getOpenGLMatrix((btScalar*)&mtx);
        FmVec3 vTmp;
        FmVec3TransformCoord(&vTmp, pOffset, &mtx);
        tm.setFromOpenGLMatrix((const btScalar*)&mtx);
        m_pBtRigidBody->setCenterOfMassTransform(tm);
    }

    return true;
}

// ��ø���λ��
bool RigidBody::GetPosition(FmVec3& pos) const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    const btTransform& tm = m_pBtRigidBody->getCenterOfMassTransform();
    const FmVec3* pOffset = GetOffset();
    if (!pOffset)
    {
        const btVector3& v = tm.getOrigin();
        pos= BtVector3ToFmVec3(v);
    }
    else
    {
        FmMat4 mtx;
        tm.getOpenGLMatrix((btScalar*)&mtx);
        pos = -(*pOffset);
        FmVec3TransformCoord(&pos, &pos, &mtx);
    }
    return true;
}

// ���ø�����ת
bool RigidBody::SetRotation(const FmQuat& rot)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    btTransform tm = m_pBtRigidBody->getCenterOfMassTransform();
    tm.setRotation(*(const btQuaternion*)&rot);
    m_pBtRigidBody->setCenterOfMassTransform(tm);
    return true;
}

// ��ø�����ת
bool RigidBody::GetRotation(FmQuat& rot) const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    const btTransform& tm = m_pBtRigidBody->getCenterOfMassTransform();
    btQuaternion r = tm.getRotation();
    memcpy(&rot, &r, sizeof(FmQuat));
    return true;
}

// ���ø���任����
bool RigidBody::SetTransform(const FmMat4& mtx)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    btTransform tm;
    tm.setFromOpenGLMatrix((const btScalar*)&mtx);
    
    const FmVec3* pOffset = GetOffset();
    if (pOffset)
    {
        FmVec3 vPos;
        FmVec3TransformCoord(&vPos, pOffset, &mtx);
        tm.setOrigin(btVector3(vPos.x, vPos.y, vPos.z));
    }

    m_pBtRigidBody->setCenterOfMassTransform(tm);

    btMotionState* pMS = m_pBtRigidBody->getMotionState();
    if (pMS)
    {
        pMS->setWorldTransform(tm);
    }

    return true;
}

// ��ø���任����
bool RigidBody::GetTransform(FmMat4& mtx) const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    const btTransform& tm = m_pBtRigidBody->getCenterOfMassTransform();
    tm.getOpenGLMatrix((btScalar*)&mtx);

    const FmVec3* pOffset = GetOffset();
    if (pOffset)
    {
        FmVec3 vPos = -(*pOffset);
        FmVec3TransformCoord(&vPos, &vPos, &mtx);
        mtx._41 = vPos.x;
        mtx._42 = vPos.y;
        mtx._43 = vPos.z;
    }
    
    return true;
}

// ��ø���任ʵ��λ��
bool RigidBody::GetPositionRB(FmVec3& pos) const
{
    if (!m_pBtRigidBody)
    {
        pos = FmVec3(0.0f);
        return false;
    }

    const btTransform& tm = m_pBtRigidBody->getCenterOfMassTransform();
    const btVector3& v = tm.getOrigin();
    pos= BtVector3ToFmVec3(v);

    return true;
}

// ��ø���任ʵ�ʾ���
bool RigidBody::GetTransformRB(FmMat4& mtx) const
{
    if (!m_pBtRigidBody)
    {
        FmMat4Identity(&mtx);
        return false;
    }

    const btTransform& tm = m_pBtRigidBody->getCenterOfMassTransform();
    tm.getOpenGLMatrix((btScalar*)&mtx);

    return true;
}

// ���ø���任
bool RigidBody::SetTransform(const FmVec3& position, const FmQuat& rot)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    btTransform tm = m_pBtRigidBody->getCenterOfMassTransform();
    tm.setOrigin(FmVec3ToBtVector3(position));
    tm.setRotation(*(const btQuaternion*)&rot);
    m_pBtRigidBody->setCenterOfMassTransform(tm);

    btMotionState* pMS = m_pBtRigidBody->getMotionState();
    if (pMS)
    {
        pMS->setWorldTransform(tm);
    }

    return true;
}

// ��ø���任
bool RigidBody::GetTransform(FmVec3& position, FmQuat& rot) const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    const btTransform& tm = m_pBtRigidBody->getCenterOfMassTransform();
    const btVector3& v = tm.getOrigin();
    position = BtVector3ToFmVec3(v);
    btQuaternion r = tm.getRotation();
    memcpy(&rot, &r, sizeof(FmQuat));
    return true;
}

// ���ø�������(�޸�MASSʱ���Զ��޸�����Գ���inertia tensor)
bool RigidBody::SetMass(float mass)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    if (mass < 0.0f)
    {
        mass = 0.0f;
    }

    btCollisionShape* pBtColShape = m_pBtRigidBody->getCollisionShape();
    btVector3 localInertia(0,0,0);
    pBtColShape->calculateLocalInertia(mass, localInertia);

    m_pBtRigidBody->setMassProps(mass, localInertia);

    return true;
}

// ��ø�������
bool RigidBody::GetMass(float& mass) const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    mass = m_pBtRigidBody->getInvMass();
    if (mass < FLT_EPSILON)
    {
        return false;
    }
    else
    {
        mass = 1.0f/mass;
        return true;
    }
}

// ��������
bool RigidBody::ResetGravity(const FmVec3& acceleration)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    m_pBtRigidBody->setGravity(FmVec3ToBtVector3(acceleration));
    return true;
}

// ��ȡ����
bool RigidBody::GetGravity(FmVec3& vGravity) const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    const btVector3& v = m_pBtRigidBody->getGravity();
    vGravity.x = v.m_floats[0];
    vGravity.y = v.m_floats[1];
    vGravity.z = v.m_floats[2];
    return true;
}

// ����ƽ�Ƶ�����
bool RigidBody::SetLinearFactor(const FmVec3& linFac)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    m_pBtRigidBody->setLinearFactor(FmVec3ToBtVector3(linFac));
    return true;
}

// ������ת������
bool RigidBody::SetAngularFactor(const FmVec3& angFac)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    m_pBtRigidBody->setLinearFactor(FmVec3ToBtVector3(angFac));
    return true;
}

// ���ٶ�����.
bool RigidBody::GetLinearDamping(float& d) const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    d = m_pBtRigidBody->getLinearDamping();
    return true;
}

bool RigidBody::SetLinearDamping(float d)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    float ad = m_pBtRigidBody->getAngularDamping();
    m_pBtRigidBody->setDamping(d, ad);
    return true;
}

// ���ٶ�����.
bool RigidBody::GetAngularDamping(float& d) const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    d = m_pBtRigidBody->getAngularDamping();
    return true;
}

bool RigidBody::SetAngularDamping(float d)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    float ld = m_pBtRigidBody->getLinearDamping();
    m_pBtRigidBody->setDamping(ld, d);
    return true;
}

bool RigidBody::SetDamping(float linearD, float angularD)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    m_pBtRigidBody->setDamping(linearD, angularD);
    return true;
}

// ���ٶ�Sleep��ֵ.
bool RigidBody::GetLinearSleepingThreshold(float& d) const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    d = m_pBtRigidBody->getLinearSleepingThreshold();
    return true;
}

bool RigidBody::SetLinearSleepingThreshold(float d)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    float ad = m_pBtRigidBody->getAngularSleepingThreshold();
    m_pBtRigidBody->setSleepingThresholds(d, ad);
    return true;
}

// ���ٶ�Sleep��ֵ.
bool RigidBody::GetAngularSleepingThreshold(float& d) const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    d = m_pBtRigidBody->getAngularSleepingThreshold();
    return true;
}

bool RigidBody::SetAngularSleepingThreshold(float d)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    float ld = m_pBtRigidBody->getLinearSleepingThreshold();
    m_pBtRigidBody->setSleepingThresholds(ld, d);
    return true;
}

bool RigidBody::SetSleepingThresholds(float linearD, float angularD)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    m_pBtRigidBody->setSleepingThresholds(linearD, angularD);
    return true;
}

// ���嵱ǰ���ܵ����������ٶ���
bool RigidBody::GetTotalForce(FmVec3& vForce) const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    const btVector3& v = m_pBtRigidBody->getTotalForce();
    vForce = BtVector3ToFmVec3(v);
    return true;
}

// ���嵱ǰ���ܵ������н��ٶ���
bool RigidBody::GetTotalTorque(FmVec3& vTorque) const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    const btVector3& v = m_pBtRigidBody->getTotalTorque();
    vTorque = BtVector3ToFmVec3(v);
    return true;
}

// ��������ĳһλ��ʩ��һ����(����һֱ����,�����������ٶȺͽ��ٶ�)
bool RigidBody::ApplyForce(const FmVec3& force, const FmVec3& rel_pos)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    m_pBtRigidBody->applyForce(FmVec3ToBtVector3(force), FmVec3ToBtVector3(rel_pos));
    return true;
}

// ����������λ��ʩ��һ����(����һֱ����,ֻ���������ٶ�)
bool RigidBody::ApplyForce(const FmVec3& force)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    m_pBtRigidBody->applyCentralForce(FmVec3ToBtVector3(force));
    return true;
}

// ������ʩ��һ�����ٶ���(����һֱ����,ֻ��������ٶ�)
bool RigidBody::ApplyTorque(const FmVec3& torque)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    m_pBtRigidBody->applyTorque(FmVec3ToBtVector3(torque));
    return true;
}

// ���������(����Ҳ�ᱻ���)
bool RigidBody::ClearForceAndTorque()
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    m_pBtRigidBody->clearForces();
    return true;
}

// ������һ������,�൱�ڸ���һ������,���ı������ٶ�
bool RigidBody::ApplyCentralImpulse(const FmVec3& impulse)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    m_pBtRigidBody->applyCentralImpulse(FmVec3ToBtVector3(impulse));
    return true;
}

// ������һ������,�൱�ڸ���һ������,���ı�����ٶ�
bool RigidBody::ApplyTorqueImpulse(const FmVec3& torque)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    m_pBtRigidBody->applyTorqueImpulse(FmVec3ToBtVector3(torque));
    return true;
}

// ��������ĳһλ��ʩ��һ������,���ı������ٶȺͽ��ٶ�
bool RigidBody::ApplyImpulse(const FmVec3& impulse, const FmVec3& rel_pos)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    m_pBtRigidBody->applyImpulse(FmVec3ToBtVector3(impulse), FmVec3ToBtVector3(rel_pos));
    return true;
}

// ���ø������ٶ�
bool RigidBody::SetLinearVelocity(const FmVec3& vel)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    m_pBtRigidBody->setLinearVelocity(FmVec3ToBtVector3(vel));
    return true;
}

// ��ø������ٶ�
bool RigidBody::GetLinearVelocity(FmVec3& vel) const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    const btVector3& v = m_pBtRigidBody->getLinearVelocity();
    vel = BtVector3ToFmVec3(v);
    return true;
}

// ���ø�����ٶ�
bool RigidBody::SetAngularVelocity(const FmVec3& vel)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    m_pBtRigidBody->setAngularVelocity(FmVec3ToBtVector3(vel));
    return true;
}

// ��ø�����ٶ�
bool RigidBody::GetAngularVelocity(FmVec3& vel) const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    const btVector3& v = m_pBtRigidBody->getAngularVelocity();
    vel = BtVector3ToFmVec3(v);
    return true;
}

// ����ϵ��
bool RigidBody::SetRestitution(float rest)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    m_pBtRigidBody->setRestitution(rest);
    return true;
}

bool RigidBody::GetRestitution(float& rest) const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    rest = m_pBtRigidBody->getRestitution();
    return true;
}

// ��Ħ��ϵ��
bool RigidBody::SetFriction(float frict)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    m_pBtRigidBody->setFriction(frict);
    return true;
}

bool RigidBody::GetFriction(float& frict) const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    frict = m_pBtRigidBody->getFriction();
    return true;
}

// ����Ħ��ϵ��
bool RigidBody::SetRollingFriction(float frict)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    m_pBtRigidBody->setRollingFriction(frict);
    return true;
}

bool RigidBody::GetRollingFriction(float& frict) const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    frict = m_pBtRigidBody->getRollingFriction();
    return true;
}

// ��ײ���Ա�־
bool RigidBody::SetCollisionFlags(int flags)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    m_pBtRigidBody->setCollisionFlags(flags);
    return true;
}

bool RigidBody::GetCollisionFlags(int& flags) const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    flags = m_pBtRigidBody->getCollisionFlags();
    return true;
}

// ������ײ�������ֵ
bool RigidBody::SetCcdMotionThreshold(float threshold)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    m_pBtRigidBody->setCcdMotionThreshold(threshold);
    return true;
}

bool RigidBody::GetCcdMotionThreshold(float& threshold) const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    threshold = m_pBtRigidBody->getCcdMotionThreshold();
    return true;
}

// Swept sphere radius (0.0 by default), see btConvexConvexAlgorithm::
bool RigidBody::SetCcdSweptSphereRadius(float radius)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    m_pBtRigidBody->setCcdSweptSphereRadius(radius);
    return true;
}

bool RigidBody::GetCcdSweptSphereRadius(float& radius) const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    radius = m_pBtRigidBody->getCcdSweptSphereRadius();
    return true;
}

// �Ƿ�Ϊ��̬����
bool RigidBody::IsStatic() const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    return m_pBtRigidBody->isStaticObject();
}

// �Ƿ�Ϊ֡��������
bool RigidBody::IsKinematic() const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    return m_pBtRigidBody->isKinematicObject();
}

// ֡���������붯̬�������л�
bool RigidBody::EnableKinematic(bool kinematic)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    int flags = m_pBtRigidBody->getCollisionFlags();
    if (kinematic)
    {
        flags |= btCollisionObject::CF_KINEMATIC_OBJECT;
    }
    else
    {
        flags &= ~btCollisionObject::CF_KINEMATIC_OBJECT;
    }

    m_pBtRigidBody->setCollisionFlags(flags);
    return true;
}

// ���÷Ǽ���ʱ��
bool RigidBody::SetDeactivationTime(float time)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    m_pBtRigidBody->setDeactivationTime(time);
    return true;
}

bool RigidBody::GetDeactivationTime(float& time) const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    time = m_pBtRigidBody->getDeactivationTime();
    return true;
}  

// ����ļ���״̬
bool RigidBody::SetActivationState(ActivationState state)
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    m_pBtRigidBody->forceActivationState(state);
    m_pBtRigidBody->setDeactivationTime(0.0f);
    m_activationState = state;
    return true;
}

bool RigidBody::GetActivationState(ActivationState& state) const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    state = (ActivationState)m_pBtRigidBody->getActivationState();
    return true;
}

// �����������ת��Ϊ���������
bool RigidBody::ConvertPosFromWorldToLocal(const FmVec3& vWorldPos, FmVec3& vLocalPos) const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    btVector3 worldPos = FmVec3ToBtVector3(vWorldPos);
    btVector3 localPos = m_pBtRigidBody->getCenterOfMassTransform().inverse() * worldPos;
    vLocalPos = BtVector3ToFmVec3(localPos);
    return true;
}

// �����������ת��Ϊ���������
bool RigidBody::ConvertPosFromLocalToWorld(const FmVec3& vLocalPos, FmVec3& vWorldPos) const
{
    if (!m_pBtRigidBody)
    {
        return false;
    }

    btVector3 localPos = FmVec3ToBtVector3(vLocalPos);
    btVector3 worldPos = m_pBtRigidBody->getCenterOfMassTransform() * localPos;
    vWorldPos = BtVector3ToFmVec3(worldPos);
    return true;
}

// �����־
bool RigidBody::SetFlags(int flags)
{
	if (!m_pBtRigidBody)
	{
		return false;
	}

	m_pBtRigidBody->setFlags(flags);
	return true;
}

bool RigidBody::GetFlags(int& flags) const
{
	if (!m_pBtRigidBody)
	{
		return false;
	}

	flags = m_pBtRigidBody->getFlags();
	return true;
}

// �û�����
void RigidBody::SetUserPointer(void* userPtr)
{
    m_pUserPointer = userPtr;
}

void* RigidBody::GetUserPointer() const
{
    return m_pUserPointer;
}

// ���ø���İ󶨶���,����ʹ����λ�÷����仯ʱ,�Զ�֪ͨ��󶨶���
void RigidBody::SetBindingObj(const PERSISTID& bindingID, bool updateBinding)
{
    if (!m_pBtRigidBody)
    {
        return;
    }

    MyMotionState* msPtr = (MyMotionState*)m_pBtRigidBody->getMotionState();
    if (msPtr)
    {
        msPtr->m_bindingID = bindingID;
    }

    if (updateBinding)
    {
        IVisBase* pVisBase = GetVisBase(bindingID);
        if (pVisBase)
        {
            FmMat4 mtx;
            const btTransform& tm = m_pBtRigidBody->getCenterOfMassTransform();
            tm.getOpenGLMatrix((btScalar*)&mtx);
            pVisBase->SetWorldMatrix(mtx);
        }
    }
}

PERSISTID RigidBody::GetBindingObj()
{
    if (!m_pBtRigidBody)
    {
        return PERSISTID();
    }

    MyMotionState* msPtr = (MyMotionState*)m_pBtRigidBody->getMotionState();
    if (!msPtr)
    {
        return PERSISTID();
    }

    return msPtr->m_bindingID;
}

void RigidBody::UpdateState()
{
    ActivationState activationState = (ActivationState)m_pBtRigidBody->getActivationState();
    if (activationState != m_activationState)
    {
        // ����״̬�����仯ʱ�Ļص�
        if (m_funcActiveCallback)
        {
            (*m_funcActiveCallback)(this, m_activationState, activationState);
        }

        m_activationState = activationState;
    }
}
