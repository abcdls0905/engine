
/****************************************************************

  File name   :  Ragdoll.h
  Author      :  叶峰
  Version     :  1.0a
  Create Date :  2013/12/20 
  Description :  Ragdoll

*****************************************************************/

#ifndef _Ragdoll_H_
#define _Ragdoll_H_

// --------------------------------------------------------------------------------------

#include "../public/core_type.h"
#include "../visual/i_ragdoll.h"

// --------------------------------------------------------------------------------------

#define BODYPART_COUNT  16
#define JOINTPART_COUNT 15

class PhysicsBullet;

class Ragdoll : public IRagdoll
{
public:
    Ragdoll();

    ~Ragdoll();

    void AddRef();

    void Release();

    bool IsReady() const;

    bool Build(const FmVec3& vPos, float scale);

    void Destroy();

    bool BuildJoints();

    void DestroyJoints();

    // 每帧更新
    void Update();

    unsigned int GetNumRigidBodies() const
    {
        return BODYPART_COUNT;
    }

    IRigidBody* GetRigidBody(const char* name);

    IRigidBody* GetRigidBody(unsigned int index)
    {
        return m_bodies[index];
    }

    // 设置绑定角色
    void SetActor(IActor* pActor);

    // 设置世界变换
    bool SetTransform(const FmMat4& mtx, bool teleport = false);

    // 纯动力学驱动,即所有的刚体都为动力学刚体,用于角色死亡
    void SetDynamicDrive(const FmVec3& vVelocity);

    // 纯关键帧驱动
    void SetKeyFrameDrive();

    // 是否处于纯动力学驱动中
    bool IsDynamicDriving() const
    {
        return m_bDynamicDriving;
    }

    // 用户数据
    void SetUserPointer(void* userPtr)
    {
        m_pUserPointer = userPtr;
    }

    void* GetUserPointer() const
    {
        return m_pUserPointer;
    }

protected:
    ICollisionShape* m_shapes[BODYPART_COUNT];
    IRigidBody* m_bodies[BODYPART_COUNT];
    IConstraint* m_joints[JOINTPART_COUNT];

    IActor* m_pActor;
    bool m_bDynamicDriving;

    long m_ref;

    float m_scale;

    void* m_pUserPointer;

public:
    Ragdoll* m_next;
    Ragdoll* m_prev;
};

// --------------------------------------------------------------------------------------

#endif
