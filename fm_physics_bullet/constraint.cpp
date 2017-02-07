
/****************************************************************

  File name   :  Constraint.cpp
  Author      :  Ò¶·å
  Version     :  1.0a
  Create Date :  2013/10/24  
  Description :  

*****************************************************************/

// --------------------------------------------------------------------------------------

#include "bullet_dynamics/constraint_solver/bt_typed_constraint.h"
#include "bullet_dynamics/constraint_solver/bt_point_2_point_constraint.h"
#include "bullet_dynamics/constraint_solver/bt_hinge_constraint.h"
#include "bullet_dynamics/constraint_solver/bt_hinge_2_constraint.h"
#include "bullet_dynamics/constraint_solver/bt_cone_twist_constraint.h"
#include "bullet_dynamics/constraint_solver/bt_generic_6_dof_constraint.h"
#include "bullet_dynamics/constraint_solver/bt_generic_6_dof_spring_constraint.h"
#include "bullet_dynamics/constraint_solver/bt_slider_constraint.h"
#include "bullet_dynamics/constraint_solver/bt_gear_constraint.h"
#include "bullet_dynamics/constraint_solver/bt_fixed_constraint.h"

#include "common.h"
#include "rigid_body.h"
#include "physics_bullet.h"
#include "constraint.h"
#include <assert.h>

// --------------------------------------------------------------------------------------

Constraint::Constraint(btTypedConstraint* pBtConstraint, YeConstraintType type)
{
    assert(pBtConstraint);
    m_pBtConstraint = pBtConstraint;
    m_type = type;

    m_next = NULL;
    m_prev = NULL;

    m_ref = 1;
}

Constraint::~Constraint()
{
    
}

void    Constraint::Release()
{
    m_ref--;
    if (m_ref == 0)
    {
        g_physicsBulletPtr->DestoryConstraint(this);
    }
}

void    Constraint::AddRef()
{
    m_ref++;
}

IRigidBody* Constraint::GetRigidBodyA()
{
    btRigidBody* pA = &m_pBtConstraint->getRigidBodyA();
    if (!pA)
    {
        return NULL;
    }

    return (IRigidBody*)(pA->getUserPointer());
}

IRigidBody* Constraint::GetRigidBodyB()
{
    btRigidBody* pB = &m_pBtConstraint->getRigidBodyB();
    if (!pB)
    {
        return NULL;
    }

    return (IRigidBody*)(pB->getUserPointer());
}

bool Constraint::GetPositionPivotA(FmVec3& vPos) const
{
    switch (m_type)
    {
    case YE_CONSTRAINT_TYPE_POINT_2_POINT:
        {
            btPoint2PointConstraint* constraint = static_cast<btPoint2PointConstraint*>(m_pBtConstraint);
            const btVector3& pos = constraint->getPivotInA();
            vPos = BtVector3ToFmVec3(pos);
            return true;
        }
        break;
    case YE_CONSTRAINT_TYPE_HINGE:
        {
            btHingeConstraint* constraint = static_cast<btHingeConstraint*>(m_pBtConstraint);
            const btVector3& pos = constraint->getFrameOffsetA().getOrigin();
            vPos = BtVector3ToFmVec3(pos);
            return true;
        }
        break;
    case YE_CONSTRAINT_TYPE_CONE_TWIST:
        {
            btConeTwistConstraint* constraint = static_cast<btConeTwistConstraint*>(m_pBtConstraint);
            const btVector3& pos = constraint->getFrameOffsetA().getOrigin();
            vPos = BtVector3ToFmVec3(pos);
            return true;
        }
        break;
    case YE_CONSTRAINT_TYPE_D6:
        {
            btGeneric6DofConstraint* constraint = static_cast<btGeneric6DofConstraint*>(m_pBtConstraint);
            const btVector3& pos = constraint->getFrameOffsetA().getOrigin();
            vPos = BtVector3ToFmVec3(pos);
            return true;
        }
        break;
    case YE_CONSTRAINT_TYPE_D6_SPRING:
        {
            btGeneric6DofSpringConstraint* constraint = static_cast<btGeneric6DofSpringConstraint*>(m_pBtConstraint);
            const btVector3& pos = constraint->getFrameOffsetA().getOrigin();
            vPos = BtVector3ToFmVec3(pos);
            return true;
        }
        break;
    case YE_CONSTRAINT_TYPE_SLIDER:
        {
            btSliderConstraint* constraint = static_cast<btSliderConstraint*>(m_pBtConstraint);
            const btVector3& pos = constraint->getFrameOffsetA().getOrigin();
            vPos = BtVector3ToFmVec3(pos);
            return true;
        }
        break;
    case YE_CONSTRAINT_TYPE_GEAR:
        {
            //btGearConstraint* constraint = static_cast<btGearConstraint*>(m_pBtConstraint);
            return false;
        }
        break;
    case YE_CONSTRAINT_TYPE_FIX:
        {
            //btFixedConstraint* constraint = static_cast<btFixedConstraint*>(m_pBtConstraint);
            return false;
        }
        break;
    default:
        return false;
    }

    return false;
}

bool Constraint::GetPositionPivotB(FmVec3& vPos) const
{
    switch (m_type)
    {
    case YE_CONSTRAINT_TYPE_POINT_2_POINT:
        {
            btPoint2PointConstraint* constraint = static_cast<btPoint2PointConstraint*>(m_pBtConstraint);
            const btVector3& pos = constraint->getPivotInB();
            vPos = BtVector3ToFmVec3(pos);
            return true;
        }
        break;
    case YE_CONSTRAINT_TYPE_HINGE:
        {
            btHingeConstraint* constraint = static_cast<btHingeConstraint*>(m_pBtConstraint);
            const btVector3& pos = constraint->getFrameOffsetB().getOrigin();
            vPos = BtVector3ToFmVec3(pos);
            return true;
        }
        break;
    case YE_CONSTRAINT_TYPE_CONE_TWIST:
        {
            btConeTwistConstraint* constraint = static_cast<btConeTwistConstraint*>(m_pBtConstraint);
            const btVector3& pos = constraint->getFrameOffsetB().getOrigin();
            vPos = BtVector3ToFmVec3(pos);
            return true;
        }
        break;
    case YE_CONSTRAINT_TYPE_D6:
        {
            btGeneric6DofConstraint* constraint = static_cast<btGeneric6DofConstraint*>(m_pBtConstraint);
            const btVector3& pos = constraint->getFrameOffsetB().getOrigin();
            vPos = BtVector3ToFmVec3(pos);
            return true;
        }
        break;
    case YE_CONSTRAINT_TYPE_D6_SPRING:
        {
            btGeneric6DofSpringConstraint* constraint = static_cast<btGeneric6DofSpringConstraint*>(m_pBtConstraint);
            const btVector3& pos = constraint->getFrameOffsetB().getOrigin();
            vPos = BtVector3ToFmVec3(pos);
            return true;
        }
        break;
    case YE_CONSTRAINT_TYPE_SLIDER:
        {
            btSliderConstraint* constraint = static_cast<btSliderConstraint*>(m_pBtConstraint);
            const btVector3& pos = constraint->getFrameOffsetB().getOrigin();
            vPos = BtVector3ToFmVec3(pos);
            return true;
        }
        break;
    case YE_CONSTRAINT_TYPE_GEAR:
        {
            //btGearConstraint* constraint = static_cast<btGearConstraint*>(m_pBtConstraint);
            return false;
        }
        break;
    case YE_CONSTRAINT_TYPE_FIX:
        {
            //btFixedConstraint* constraint = static_cast<btFixedConstraint*>(m_pBtConstraint);
            return false;
        }
        break;
    default:
        return false;
    }

    return false;
}

bool Constraint::SetPositionPivotA(const FmVec3& vPos)
{
    btVector3 pos = FmVec3ToBtVector3(vPos);

    switch (m_type)
    {
    case YE_CONSTRAINT_TYPE_POINT_2_POINT:
        {
            btPoint2PointConstraint* constraint = static_cast<btPoint2PointConstraint*>(m_pBtConstraint);
            constraint->setPivotA(pos);
            return true;
        }
        break;
    case YE_CONSTRAINT_TYPE_HINGE:
        {
            btHingeConstraint* constraint = static_cast<btHingeConstraint*>(m_pBtConstraint);
            btTransform frameA = constraint->getFrameOffsetA();
            const btTransform& frameB = constraint->getFrameOffsetB();
            frameA.setOrigin(pos);
            constraint->setFrames(frameA, frameB);
            return true;
        }
        break;
    case YE_CONSTRAINT_TYPE_CONE_TWIST:
        {
            btConeTwistConstraint* constraint = static_cast<btConeTwistConstraint*>(m_pBtConstraint);
            btTransform frameA = constraint->getFrameOffsetA();
            const btTransform& frameB = constraint->getFrameOffsetB();
            frameA.setOrigin(pos);
            constraint->setFrames(frameA, frameB);
            return true;
        }
        break;
    case YE_CONSTRAINT_TYPE_D6:
        {
            btGeneric6DofConstraint* constraint = static_cast<btGeneric6DofConstraint*>(m_pBtConstraint);
            btTransform frameA = constraint->getFrameOffsetA();
            const btTransform& frameB = constraint->getFrameOffsetB();
            frameA.setOrigin(pos);
            constraint->setFrames(frameA, frameB);
            return true;
        }
        break;
    case YE_CONSTRAINT_TYPE_D6_SPRING:
        {
            btGeneric6DofSpringConstraint* constraint = static_cast<btGeneric6DofSpringConstraint*>(m_pBtConstraint);
            btTransform frameA = constraint->getFrameOffsetA();
            const btTransform& frameB = constraint->getFrameOffsetB();
            frameA.setOrigin(pos);
            constraint->setFrames(frameA, frameB);
            return true;
        }
        break;
    case YE_CONSTRAINT_TYPE_SLIDER:
        {
            btSliderConstraint* constraint = static_cast<btSliderConstraint*>(m_pBtConstraint);
            btTransform frameA = constraint->getFrameOffsetA();
            const btTransform& frameB = constraint->getFrameOffsetB();
            frameA.setOrigin(pos);
            constraint->setFrames(frameA, frameB);
            return true;
        }
        break;
    case YE_CONSTRAINT_TYPE_GEAR:
        {
            //btGearConstraint* constraint = static_cast<btGearConstraint*>(m_pBtConstraint);
            return false;
        }
        break;
    case YE_CONSTRAINT_TYPE_FIX:
        {
            //btFixedConstraint* constraint = static_cast<btFixedConstraint*>(m_pBtConstraint);
            return false;
        }
        break;
    default:
        return false;
    }

    return false;
}

bool Constraint::SetPositionPivotB(const FmVec3& vPos)
{
    btVector3 pos = FmVec3ToBtVector3(vPos);

    switch (m_type)
    {
    case YE_CONSTRAINT_TYPE_POINT_2_POINT:
        {
            btPoint2PointConstraint* constraint = static_cast<btPoint2PointConstraint*>(m_pBtConstraint);
            constraint->setPivotB(pos);
            return true;
        }
        break;
    case YE_CONSTRAINT_TYPE_HINGE:
        {
            btHingeConstraint* constraint = static_cast<btHingeConstraint*>(m_pBtConstraint);
            const btTransform& frameA = constraint->getFrameOffsetA();
            btTransform frameB = constraint->getFrameOffsetB();
            frameB.setOrigin(pos);
            constraint->setFrames(frameA, frameB);
            return true;
        }
        break;
    case YE_CONSTRAINT_TYPE_CONE_TWIST:
        {
            btConeTwistConstraint* constraint = static_cast<btConeTwistConstraint*>(m_pBtConstraint);
            const btTransform& frameA = constraint->getFrameOffsetA();
            btTransform frameB = constraint->getFrameOffsetB();
            frameB.setOrigin(pos);
            constraint->setFrames(frameA, frameB);
            return true;
        }
        break;
    case YE_CONSTRAINT_TYPE_D6:
        {
            btGeneric6DofConstraint* constraint = static_cast<btGeneric6DofConstraint*>(m_pBtConstraint);
            const btTransform& frameA = constraint->getFrameOffsetA();
            btTransform frameB = constraint->getFrameOffsetB();
            frameB.setOrigin(pos);
            constraint->setFrames(frameA, frameB);
            return true;
        }
        break;
    case YE_CONSTRAINT_TYPE_D6_SPRING:
        {
            btGeneric6DofSpringConstraint* constraint = static_cast<btGeneric6DofSpringConstraint*>(m_pBtConstraint);
            const btTransform& frameA = constraint->getFrameOffsetA();
            btTransform frameB = constraint->getFrameOffsetB();
            frameB.setOrigin(pos);
            constraint->setFrames(frameA, frameB);
            return true;
        }
        break;
    case YE_CONSTRAINT_TYPE_SLIDER:
        {
            btSliderConstraint* constraint = static_cast<btSliderConstraint*>(m_pBtConstraint);
            const btTransform& frameA = constraint->getFrameOffsetA();
            btTransform frameB = constraint->getFrameOffsetB();
            frameB.setOrigin(pos);
            constraint->setFrames(frameA, frameB);
            return true;
        }
        break;
    case YE_CONSTRAINT_TYPE_GEAR:
        {
            //btGearConstraint* constraint = static_cast<btGearConstraint*>(m_pBtConstraint);
            return false;
        }
        break;
    case YE_CONSTRAINT_TYPE_FIX:
        {
            //btFixedConstraint* constraint = static_cast<btFixedConstraint*>(m_pBtConstraint);
            return false;
        }
        break;
    default:
        return false;
    }

    return false;
}

bool Constraint::SetProperties(const BaseConstraintData* pConstraintData, bool updatePivot)
{
    if (pConstraintData->m_objectType != m_type)
    {
        return false;
    }

    switch (m_type)
    {
    case YE_CONSTRAINT_TYPE_POINT_2_POINT:
        {
            btPoint2PointConstraint* constraint = static_cast<btPoint2PointConstraint*>(m_pBtConstraint);
            Point2PointConstraintData* p2pData = (Point2PointConstraintData*)pConstraintData;
            if (updatePivot)
            {
                btVector3 pivotInA = FmVec3ToBtVector3(p2pData->m_pivotInA);
                btVector3 pivotInB = FmVec3ToBtVector3(p2pData->m_pivotInB);

                constraint->setPivotA(pivotInA);
                constraint->setPivotB(pivotInB);
            }
        }
        break;
    case YE_CONSTRAINT_TYPE_HINGE:
        {
            btHingeConstraint* constraint = static_cast<btHingeConstraint*>(m_pBtConstraint);
            HingeConstraintData* hingeData = (HingeConstraintData*)pConstraintData;

            if (updatePivot)
            {
                btTransform rbAFrame;
                btTransform rbBFrame;
                rbAFrame.setFromOpenGLMatrix((const btScalar*)&hingeData->m_rbAFrame);
                rbBFrame.setFromOpenGLMatrix((const btScalar*)&hingeData->m_rbBFrame);

                constraint->setFrames(rbAFrame, rbBFrame);
            }

            if (hingeData->m_enableAngularMotor)
            {
                constraint->enableAngularMotor(true,hingeData->m_motorTargetVelocity,hingeData->m_maxMotorImpulse);
            }
            constraint->setAngularOnly(hingeData->m_angularOnly!=0);
            constraint->setLimit(btScalar(hingeData->m_lowerLimit),
                btScalar(hingeData->m_upperLimit),
                btScalar(hingeData->m_limitSoftness),
                btScalar(hingeData->m_biasFactor),
                btScalar(hingeData->m_relaxationFactor));
        }
        break;
    case YE_CONSTRAINT_TYPE_CONE_TWIST:
        {
            btConeTwistConstraint* constraint = static_cast<btConeTwistConstraint*>(m_pBtConstraint);
            ConeTwistConstraintData* coneData = (ConeTwistConstraintData*)pConstraintData;

            if (updatePivot)
            {
                btTransform rbAFrame;
                btTransform rbBFrame;
                rbAFrame.setFromOpenGLMatrix((const btScalar*)&coneData->m_rbAFrame);
                rbBFrame.setFromOpenGLMatrix((const btScalar*)&coneData->m_rbBFrame);

                constraint->setFrames(rbAFrame, rbBFrame);
            }

            constraint->setLimit(coneData->m_swingSpan1,
                coneData->m_swingSpan2,
                coneData->m_twistSpan,
                coneData->m_limitSoftness,
                coneData->m_biasFactor,
                coneData->m_relaxationFactor);
            constraint->setDamping(coneData->m_damping);
        }
        break;
    case YE_CONSTRAINT_TYPE_D6:
        {
            btGeneric6DofConstraint* constraint = static_cast<btGeneric6DofConstraint*>(m_pBtConstraint);
            Generic6DofConstraintData* dofData = (Generic6DofConstraintData*)pConstraintData;

            if (updatePivot)
            {
                btTransform rbAFrame;
                btTransform rbBFrame;
                rbAFrame.setFromOpenGLMatrix((const btScalar*)&dofData->m_rbAFrame);
                rbBFrame.setFromOpenGLMatrix((const btScalar*)&dofData->m_rbBFrame);

                constraint->setFrames(rbAFrame, rbBFrame);
            }

            btVector3 angLowerLimit = FmVec3ToBtVector3(dofData->m_angularLowerLimit);
            btVector3 angUpperLimit = FmVec3ToBtVector3(dofData->m_angularUpperLimit);
            btVector3 linLowerLimit = FmVec3ToBtVector3(dofData->m_linearLowerLimit);
            btVector3 linUpperlimit = FmVec3ToBtVector3(dofData->m_linearUpperLimit);

            constraint->setAngularLowerLimit(angLowerLimit);
            constraint->setAngularUpperLimit(angUpperLimit);
            constraint->setLinearLowerLimit(linLowerLimit);
            constraint->setLinearUpperLimit(linUpperlimit);
        }
        break;
    case YE_CONSTRAINT_TYPE_D6_SPRING:
        {
            btGeneric6DofSpringConstraint* constraint = static_cast<btGeneric6DofSpringConstraint*>(m_pBtConstraint);
            Generic6DofSpringConstraintData* dofData = (Generic6DofSpringConstraintData*)pConstraintData;

            if (updatePivot)
            {
                btTransform rbAFrame;
                btTransform rbBFrame;
                rbAFrame.setFromOpenGLMatrix((const btScalar*)&dofData->m_rbAFrame);
                rbBFrame.setFromOpenGLMatrix((const btScalar*)&dofData->m_rbBFrame);

                constraint->setFrames(rbAFrame, rbBFrame);
            }

            btVector3 angLowerLimit = FmVec3ToBtVector3(dofData->m_angularLowerLimit);
            btVector3 angUpperLimit = FmVec3ToBtVector3(dofData->m_angularUpperLimit);
            btVector3 linLowerLimit = FmVec3ToBtVector3(dofData->m_linearLowerLimit);
            btVector3 linUpperlimit = FmVec3ToBtVector3(dofData->m_linearUpperLimit);

            angLowerLimit.setW(0.f);
            constraint->setAngularLowerLimit(angLowerLimit);
            constraint->setAngularUpperLimit(angUpperLimit);
            constraint->setLinearLowerLimit(linLowerLimit);
            constraint->setLinearUpperLimit(linUpperlimit);

            for (int i=0; i<6; i++)
            {
                constraint->setStiffness(i, dofData->m_springStiffness[i]);
                constraint->setEquilibriumPoint(i, dofData->m_equilibriumPoint[i]);
                constraint->enableSpring(i, dofData->m_springEnabled[i]!=0);
                constraint->setDamping(i, dofData->m_springDamping[i]);
            }
        }
        break;
    case YE_CONSTRAINT_TYPE_SLIDER:
        {
            btSliderConstraint* constraint = static_cast<btSliderConstraint*>(m_pBtConstraint);
            SliderConstraintData* sliderData = (SliderConstraintData*)pConstraintData;

            if (updatePivot)
            {
                btTransform rbAFrame;
                btTransform rbBFrame;
                rbAFrame.setFromOpenGLMatrix((const btScalar*)&sliderData->m_rbAFrame);
                rbBFrame.setFromOpenGLMatrix((const btScalar*)&sliderData->m_rbBFrame);

                constraint->setFrames(rbAFrame, rbBFrame);
            }

            constraint->setLowerLinLimit(sliderData->m_linearLowerLimit);
            constraint->setUpperLinLimit(sliderData->m_linearUpperLimit);
            constraint->setLowerAngLimit(sliderData->m_angularLowerLimit);
            constraint->setUpperAngLimit(sliderData->m_angularUpperLimit);
            constraint->setUseFrameOffset(sliderData->m_useOffsetForConstraintFrame!=0);
        }
        break;
    case YE_CONSTRAINT_TYPE_GEAR:
        {
            btGearConstraint* constraint = static_cast<btGearConstraint*>(m_pBtConstraint);
            GearConstraintData* gearData = (GearConstraintData*)pConstraintData;

            if (updatePivot)
            {
                btVector3 axisInA = FmVec3ToBtVector3(gearData->m_axisInA);
                btVector3 axisInB = FmVec3ToBtVector3(gearData->m_axisInB);

                constraint->setAxisA(axisInA);
                constraint->setAxisB(axisInB);
                constraint->setRatio(gearData->m_ratio);
            }
        }
        break;
    case YE_CONSTRAINT_TYPE_FIX:
        {
            btFixedConstraint* constraint = static_cast<btFixedConstraint*>(m_pBtConstraint);
            FixConstraintData* gearData = (FixConstraintData*)pConstraintData;
            if (updatePivot)
            {
                btTransform rbAFrame;
                btTransform rbBFrame;
                rbAFrame.setFromOpenGLMatrix((const btScalar*)&gearData->m_rbAFrame);
                rbBFrame.setFromOpenGLMatrix((const btScalar*)&gearData->m_rbBFrame);

                constraint->setFrames(rbAFrame, rbBFrame);
            }
        }
        break;
    default:
        return false;
    }

    m_pBtConstraint->setDbgDrawSize(pConstraintData->m_dbgDrawSize);
    
    ///those fields didn't exist and set to zero for pre-280 versions
    m_pBtConstraint->setBreakingImpulseThreshold(pConstraintData->m_breakingImpulseThreshold);
    m_pBtConstraint->setEnabled(pConstraintData->m_isEnabled!=0);
    m_pBtConstraint->setOverrideNumSolverIterations(pConstraintData->m_overrideNumSolverIterations);

    return true;
}
