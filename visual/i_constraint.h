
/****************************************************************

  File name   :  i_constraint.h
  Author      :  叶峰
  Version     :  1.0a
  Create Date :  2013/11/27  
  Description :  约束接口

*****************************************************************/

#ifndef __i_constraint_H__
#define __i_constraint_H__

// --------------------------------------------------------------------------------------

#include "../math/fm_vector.h"

// --------------------------------------------------------------------------------------

enum YeConstraintType
{
    YE_CONSTRAINT_TYPE_INVALID          = 0,
    YE_CONSTRAINT_TYPE_POINT_2_POINT    = 1,
    YE_CONSTRAINT_TYPE_HINGE            = 2,
    YE_CONSTRAINT_TYPE_CONE_TWIST       = 3,
    YE_CONSTRAINT_TYPE_D6               = 4,
    YE_CONSTRAINT_TYPE_D6_SPRING        = 5,
    YE_CONSTRAINT_TYPE_SLIDER           = 6,
    YE_CONSTRAINT_TYPE_GEAR             = 7,
    YE_CONSTRAINT_TYPE_FIX              = 8,
    YE_CONSTRAINT_TYPE_CONTACT          = 9,
};

// --------------------------------------------------------------------------------------

class IRigidBody;

// --------------------------------------------------------------------------------------

struct BaseConstraintData
{
    YeConstraintType m_objectType;
    int	m_userConstraintType;
    int	m_userConstraintId;
    int	m_needsFeedback;

    float m_appliedImpulse;
    float m_dbgDrawSize;

    int	m_disableCollisionsBetweenLinkedBodies;
    int	m_overrideNumSolverIterations;

    float m_breakingImpulseThreshold;
    int	m_isEnabled;

    BaseConstraintData()
    {
        m_objectType = YE_CONSTRAINT_TYPE_INVALID;
        m_userConstraintType = -1;
       	m_userConstraintId = -1;
       	m_needsFeedback = false;

        m_appliedImpulse = 0.0f;
        m_dbgDrawSize = 0.3f;

       	m_disableCollisionsBetweenLinkedBodies = true;
       	m_overrideNumSolverIterations = -1;

        m_breakingImpulseThreshold = 3.402823466e+38F;
       	m_isEnabled = true;
    }
};

// 点对点约束
struct	Point2PointConstraintData : public BaseConstraintData
{
    FmVec3	m_pivotInA;
    FmVec3	m_pivotInB;

    Point2PointConstraintData()
    {
        m_objectType = YE_CONSTRAINT_TYPE_POINT_2_POINT;

        m_pivotInA = FmVec3(0.0f);
        m_pivotInB = FmVec3(0.0f);
    }
};

// 折叶约束
struct	HingeConstraintData : public BaseConstraintData
{
    FmMat4  m_rbAFrame; // constraint axii. Assumes z is hinge axis.
    FmMat4  m_rbBFrame;
    int     m_useReferenceFrameA;
    int     m_angularOnly;

    int     m_enableAngularMotor;
    float	m_motorTargetVelocity;
    float	m_maxMotorImpulse;

    float	m_lowerLimit;
    float	m_upperLimit;
    float	m_limitSoftness;
    float	m_biasFactor;
    float	m_relaxationFactor;

    HingeConstraintData()
    {
        m_objectType = YE_CONSTRAINT_TYPE_HINGE;

        FmMat4Identity(&m_rbAFrame);
        FmMat4Identity(&m_rbBFrame);

        m_useReferenceFrameA = true;
        m_angularOnly = false;

        m_enableAngularMotor = false;
        m_motorTargetVelocity = 0.0f;
        m_maxMotorImpulse = -1.0f;

        m_lowerLimit = 1.0f;
        m_upperLimit = -1.0f;
        m_limitSoftness = 0.9f;
        m_biasFactor = 0.3f;
        m_relaxationFactor = 1.0f;
    }
};

// 锥体约束
struct	ConeTwistConstraintData : public BaseConstraintData
{
    FmMat4  m_rbAFrame;
    FmMat4  m_rbBFrame;

    //limits
    float	m_swingSpan1;
    float	m_swingSpan2;
    float	m_twistSpan;
    float	m_limitSoftness;
    float	m_biasFactor;
    float	m_relaxationFactor;

    float	m_damping;

    ConeTwistConstraintData()
    {
        m_objectType = YE_CONSTRAINT_TYPE_CONE_TWIST;

        FmMat4Identity(&m_rbAFrame);
        FmMat4Identity(&m_rbBFrame);

        m_swingSpan1 = 0.0f;
        m_swingSpan2 = 0.0f;
        m_twistSpan = 0.0f;
        m_limitSoftness = 1.0f;
        m_biasFactor = 0.3f;
        m_relaxationFactor = 1.0f;

        m_damping = 0.01f;
    }
};

struct Generic6DofConstraintData : public BaseConstraintData
{
    FmMat4  m_rbAFrame; // constraint axii. Assumes z is hinge axis.
    FmMat4  m_rbBFrame;

    FmVec3	m_linearUpperLimit;
    FmVec3	m_linearLowerLimit;

    FmVec3  m_angularUpperLimit;
    FmVec3	m_angularLowerLimit;

    int	    m_useLinearReferenceFrameA;
    int     m_useOffsetForConstraintFrame;

    Generic6DofConstraintData()
    {
        m_objectType = YE_CONSTRAINT_TYPE_D6;

        FmMat4Identity(&m_rbAFrame);
        FmMat4Identity(&m_rbBFrame);

        m_linearUpperLimit = FmVec3(0.f,0.f,0.f);
        m_linearLowerLimit = FmVec3(0.f,0.f,0.f);

        m_angularUpperLimit = FmVec3(-1.0f, -1.0f, -1.0f);
        m_angularLowerLimit = FmVec3(1.0f, 1.0f, 1.0f);

        m_useLinearReferenceFrameA = false;
        m_useOffsetForConstraintFrame = true;
    }
};

struct Generic6DofSpringConstraintData : public Generic6DofConstraintData
{
    int			m_springEnabled[6];
    float		m_equilibriumPoint[6];
    float		m_springStiffness[6];
    float		m_springDamping[6];

    Generic6DofSpringConstraintData()
    {
        m_objectType = YE_CONSTRAINT_TYPE_D6_SPRING;

        for(int i = 0; i < 6; i++)
        {
            m_springEnabled[i] = false;
            m_equilibriumPoint[i] = 0.0f;
            m_springStiffness[i] =  0.0f;
            m_springDamping[i] =  1.0f;
        }
    }
};

// 滑动约束
struct SliderConstraintData : public BaseConstraintData
{
    FmMat4  m_rbAFrame; // constraint axii. Assumes z is hinge axis.
    FmMat4  m_rbBFrame;

    float	m_linearUpperLimit;
    float	m_linearLowerLimit;

    float	m_angularUpperLimit;
    float	m_angularLowerLimit;

    int	m_useLinearReferenceFrameA;
    int m_useOffsetForConstraintFrame;

    SliderConstraintData()
    {
        m_objectType = YE_CONSTRAINT_TYPE_SLIDER;

        FmMat4Identity(&m_rbAFrame);
        FmMat4Identity(&m_rbBFrame);

        m_linearUpperLimit = 0.f;
        m_linearLowerLimit = 0.f;

        m_angularUpperLimit = -1.0f;
        m_angularLowerLimit = 1.0;

        m_useLinearReferenceFrameA = false;
        m_useOffsetForConstraintFrame = true;
    }
};

// 齿轮约束
struct GearConstraintData : public BaseConstraintData
{
    FmVec3			m_axisInA;
    FmVec3			m_axisInB;

    float           m_ratio;

    GearConstraintData()
    {
        m_objectType = YE_CONSTRAINT_TYPE_GEAR;

        m_axisInA = FmVec3(0.0f, 1.0f, 0.0f);
        m_axisInB = FmVec3(0.0f, 1.0f, 0.0f);
        
        m_ratio = 1.0f;
    }
};

// 固定约束
struct	FixConstraintData : public BaseConstraintData
{
    FmMat4  m_rbAFrame;
    FmMat4  m_rbBFrame;

    FixConstraintData()
    {
        m_objectType = YE_CONSTRAINT_TYPE_FIX;

        FmMat4Identity(&m_rbAFrame);
        FmMat4Identity(&m_rbBFrame);
    }
};

// --------------------------------------------------------------------------------------

class IConstraint
{
public:
    virtual ~IConstraint() {}

    virtual void Release() = 0;

    virtual YeConstraintType GetConstraintType() const = 0;

    virtual IRigidBody* GetRigidBodyA() = 0;

    virtual IRigidBody* GetRigidBodyB() = 0;

    virtual bool GetPositionPivotA(FmVec3& vPos) const = 0;

    virtual bool GetPositionPivotB(FmVec3& vPos) const = 0;

    virtual bool SetPositionPivotA(const FmVec3& vPos) = 0;

    virtual bool SetPositionPivotB(const FmVec3& vPos) = 0;

    virtual bool SetProperties(const BaseConstraintData* pConstraintData, bool updatePivot = false) = 0;
};

// --------------------------------------------------------------------------------------

#endif
