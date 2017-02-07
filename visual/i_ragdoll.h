
/****************************************************************

  File name   :  i_ragdoll.h
  Author      :  叶峰
  Version     :  1.0a
  Create Date :  2014/03/03  
  Description :  物理Ragdoll接口

*****************************************************************/

#ifndef __i_ragdoll_H__
#define __i_ragdoll_H__

// --------------------------------------------------------------------------------------

#include "../math/fm_vector.h"
#include "../math/fm_matrix.h"
#include "../math/fm_quaternion.h"
#include "../public/persistid.h"

// --------------------------------------------------------------------------------------

class ICollisionShape;
class IRigidBody;
class IConstraint;
class IActor;

// --------------------------------------------------------------------------------------

class IRagdoll
{
public:
    virtual ~IRagdoll() {}

    virtual void Release() = 0;

    virtual bool IsReady() const = 0;

    // 每帧更新
    virtual void Update() = 0;

    virtual unsigned int GetNumRigidBodies() const = 0;

    virtual IRigidBody* GetRigidBody(const char* name) = 0;

    virtual IRigidBody* GetRigidBody(unsigned int index) = 0;

    // 设置绑定角色
    virtual void SetActor(IActor* pActor) = 0;

    // 约束,必需设置了IActor后才能创建约束
    virtual bool BuildJoints() = 0;

    virtual void DestroyJoints() = 0;

    // 设置世界变换
    virtual bool SetTransform(const FmMat4& mtx, bool teleport = false) = 0;

    // 纯动力学驱动,即所有的刚体都为动力学刚体,用于角色死亡
    // vVelocity为设置所有骨骼的初速度
    virtual void SetDynamicDrive(const FmVec3& vVelocity) = 0;

    // 纯关键帧驱动
    virtual void SetKeyFrameDrive() = 0;

    // 是否处于纯动力学驱动中
    virtual bool IsDynamicDriving() const = 0;

    // 用户数据
    virtual void SetUserPointer(void* userPtr) = 0;

    virtual void* GetUserPointer() const = 0;

};

// --------------------------------------------------------------------------------------

#endif
