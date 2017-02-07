
/****************************************************************

  File name   :  i_collision_shape.h
  Author      :  Ҷ��
  Version     :  1.0a
  Create Date :  2013/11/27  
  Description :  ��ײ�νӿ�

*****************************************************************/

#ifndef __i_collision_shape_H__
#define __i_collision_shape_H__

// --------------------------------------------------------------------------------------

#include "../math/fm_vector.h"
#include "../math/fm_matrix.h"

// --------------------------------------------------------------------------------------

enum YeCollisionShapeType
{
    YE_CST_INVALID              = 0,
    YE_CST_BOX                  = 1,    // ������
    YE_CST_CUBE                 = 2,    // ������
    YE_CST_CYLINDER             = 3,    // Բ����
    YE_CST_SPHERE               = 4,    // ����
    YE_CST_CAPSULE              = 5,    // ������
    YE_CST_CONE                 = 6,    // ׶��
    YE_CST_CONVEX               = 7,    // ͹������
    YE_CST_COMPOUND             = 8,    // �����״
    YE_CST_DYNAMIC_MESH         = 9,    // ��̬��Mesh
    YE_CST_STATIC_PLANE         = 10,   // ƽ��
    YE_CST_STATIC_MESH          = 11,   // ��̬��Mesh
    YE_CST_STATIC_SCALED_MESH   = 12,   // ��̬�Ŀ��������ŵ�Mesh
    YE_CST_TERRAIN              = 13,   // ����
    YE_CST_EMPTY                = 14,
};

// --------------------------------------------------------------------------------------

class ICollisionShape
{
public:
    virtual ~ICollisionShape() {}

    virtual void    Release() = 0;

    virtual YeCollisionShapeType GetShapeType() const = 0;

    virtual void    GetAabb(const FmMat4& mtx, FmVec3& aabbMin, FmVec3& aabbMax) const = 0;

    virtual void    GetBoundingSphere(FmVec3& center, float& radius) const = 0;

    virtual void    SetMargin(float margin) = 0;

    virtual float   GetMargin() const = 0;

    // ��������
    // bullet��Shape����ʵʱ��Ӱ�����Ӧ����
    virtual void    SetScaling(const FmVec3& vScale) = 0;

    virtual void    GetScaling(FmVec3& vScale) = 0;

    virtual void    SetUserPointer(void* userPtr) = 0;

    virtual void*   GetUserPointer() const = 0;
};

// --------------------------------------------------------------------------------------

#endif
