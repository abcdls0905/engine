
/****************************************************************

  File name   :  Constraint.h
  Author      :  Ò¶·å
  Version     :  1.0a
  Create Date :  2013/10/24  
  Description :  Ô¼Êø

*****************************************************************/

#ifndef __Constraint_H__
#define __Constraint_H__

// --------------------------------------------------------------------------------------

#include "../visual/i_constraint.h"

// --------------------------------------------------------------------------------------

class btTypedConstraint;

// --------------------------------------------------------------------------------------

class Constraint : public IConstraint
{
public:
    Constraint(btTypedConstraint* pBtConstraint, YeConstraintType type);

    ~Constraint();

    void Release();

    void AddRef();

    YeConstraintType GetConstraintType() const { return m_type; };

    IRigidBody* GetRigidBodyA();

    IRigidBody* GetRigidBodyB();

    bool GetPositionPivotA(FmVec3& vPos) const;

    bool GetPositionPivotB(FmVec3& vPos) const;

    bool SetPositionPivotA(const FmVec3& vPos);

    bool SetPositionPivotB(const FmVec3& vPos);

    bool SetProperties(const BaseConstraintData* pConstraintData, bool updatePivot = false);

public:
    btTypedConstraint* m_pBtConstraint;
    YeConstraintType m_type;

    Constraint* m_next;
    Constraint* m_prev;

    long m_ref;
};

// --------------------------------------------------------------------------------------

#endif
