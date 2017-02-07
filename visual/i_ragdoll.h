
/****************************************************************

  File name   :  i_ragdoll.h
  Author      :  Ҷ��
  Version     :  1.0a
  Create Date :  2014/03/03  
  Description :  ����Ragdoll�ӿ�

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

    // ÿ֡����
    virtual void Update() = 0;

    virtual unsigned int GetNumRigidBodies() const = 0;

    virtual IRigidBody* GetRigidBody(const char* name) = 0;

    virtual IRigidBody* GetRigidBody(unsigned int index) = 0;

    // ���ð󶨽�ɫ
    virtual void SetActor(IActor* pActor) = 0;

    // Լ��,����������IActor����ܴ���Լ��
    virtual bool BuildJoints() = 0;

    virtual void DestroyJoints() = 0;

    // ��������任
    virtual bool SetTransform(const FmMat4& mtx, bool teleport = false) = 0;

    // ������ѧ����,�����еĸ��嶼Ϊ����ѧ����,���ڽ�ɫ����
    // vVelocityΪ�������й����ĳ��ٶ�
    virtual void SetDynamicDrive(const FmVec3& vVelocity) = 0;

    // ���ؼ�֡����
    virtual void SetKeyFrameDrive() = 0;

    // �Ƿ��ڴ�����ѧ������
    virtual bool IsDynamicDriving() const = 0;

    // �û�����
    virtual void SetUserPointer(void* userPtr) = 0;

    virtual void* GetUserPointer() const = 0;

};

// --------------------------------------------------------------------------------------

#endif
