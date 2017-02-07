
/****************************************************************

  File name   :  CollisionShape.h
  Author      :  Ò¶·å
  Version     :  1.0a
  Create Date :  2013/10/23  
  Description :  Åö×²ÐÎ

*****************************************************************/

#ifndef __CollisionShape_H__
#define __CollisionShape_H__

// --------------------------------------------------------------------------------------

#include "../visual/i_collision_shape.h"

// --------------------------------------------------------------------------------------

class btCollisionShape;

// --------------------------------------------------------------------------------------

class CollisionShape : public ICollisionShape
{
public:
    CollisionShape(btCollisionShape* pBtShape, YeCollisionShapeType type, long heightAxis);

    ~CollisionShape();

    void    Release();

    void    AddRef();

    YeCollisionShapeType GetShapeType() const { return m_type; }

    btCollisionShape* GetBtCollisionShape()
    {
        return m_pBtCollisionShape;
    }

    void    GetAabb(const FmMat4& mtx, FmVec3& aabbMin, FmVec3& aabbMax) const;

    void    GetBoundingSphere(FmVec3& center, float& radius) const;

    void    SetMargin(float margin);

    float   GetMargin() const;

    void    SetScaling(const FmVec3& vScale);

    void    GetScaling(FmVec3& vScale);

    void    SetUserPointer(void* userPtr);

    void*   GetUserPointer() const;

public:
    btCollisionShape* m_pBtCollisionShape;
    YeCollisionShapeType m_type;
    long m_heightAxis;
    float m_params[4];
    CollisionShape* m_next;
    CollisionShape* m_prev;
    long m_ref;
};

// --------------------------------------------------------------------------------------

#endif
