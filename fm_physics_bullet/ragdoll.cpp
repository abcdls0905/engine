
/****************************************************************

  File name   :  Ragdoll.cpp
  Author      :  叶峰
  Version     :  1.0a
  Create Date :  2013/12/20 
  Description :  Ragdoll-Demo

*****************************************************************/

// --------------------------------------------------------------------------------------

#include "../visual/i_physics_simulator.h"
#include "../visual/i_actor.h"
#include "../public/core_log.h"
#include "collision_shape.h"
#include "physics_bullet.h"
#include "rigid_body.h"
#include "ragdoll.h"

#include <assert.h>

// --------------------------------------------------------------------------------------

#ifndef M_PI
  #define M_PI       3.1415927f
#endif

#ifndef M_PI_2
  #define M_PI_2     1.570796f
#endif

#ifndef M_PI_4
  #define M_PI_4     0.7853982f
#endif

#ifndef M_PI_8
  #define M_PI_8     0.3926991f
#endif

static void FmMat4EulerZYX(FmMat4 *pOut, float eulerX, float eulerY, float eulerZ) 
{ 
    ///@todo proposed to reverse this since it's labeled zyx but takes arguments xyz and it will match all other parts of the code
    float ci(cosf(eulerX)); 
    float cj(cosf(eulerY)); 
    float ch(cosf(eulerZ)); 
    float si(sinf(eulerX)); 
    float sj(sinf(eulerY)); 
    float sh(sinf(eulerZ)); 
    float cc = ci * ch; 
    float cs = ci * sh; 
    float sc = si * ch; 
    float ss = si * sh;

    pOut->_11 = cj * ch;
    pOut->_12 = sj * sc - cs;
    pOut->_13 = sj * cc + ss;
    pOut->_14 = 0.0f;

    pOut->_21 = cj * sh;
    pOut->_22 = sj * ss + cc;
    pOut->_23 = sj * cs - sc;
    pOut->_24 = 0.0f;

    pOut->_31 = -sj;
    pOut->_32 = cj * si;
    pOut->_33 = cj * ci;
    pOut->_34 = 0.0f;

    pOut->_41 = 0.0f;
    pOut->_42 = 0.0f;
    pOut->_43 = 0.0f;
    pOut->_44 = 1.0f;
}

// --------------------------------------------------------------------------------------

enum BodyPart
{
    BODY_SPINE = 0,
    BODY_SPINE1,
    BODY_SPINE2,
    BODY_HEAD,

    BODY_L_THIGH,
    BODY_L_CALF,
    BODY_L_FOOT,
           
    BODY_R_THIGH,
    BODY_R_CALF,
    BODY_R_FOOT,
           
    BODY_L_UPPERARM,
    BODY_L_FOREARM,
    BODY_L_HAND,
           
    BODY_R_UPPERARM,
    BODY_R_FOREARM,
    BODY_R_HAND,
};

// Ragdoll所对应的骨骼名
static const char g_szBonesName[BODYPART_COUNT][32] = 
{
    "Bip01",
    "Bip01 Spine1",
    "Bip01 Spine2",
    "Bip01 Head",
     
    "Bip01 L Thigh",
    "Bip01 L Calf",
    "Bip01 L Foot",
     
    "Bip01 R Thigh",
    "Bip01 R Calf",
    "Bip01 R Foot",
     
    "Bip01 L UpperArm",
    "Bip01 L Forearm",
    "Bip01 L Hand",
     
    "Bip01 R UpperArm",
    "Bip01 R Forearm",
    "Bip01 R Hand",
};

// Ragdoll所对应的附加骨骼
static const char g_szBonesAttachName[BODYPART_COUNT][32] = 
{
    "",
    "",
    "",
    "",

    "",
    "",
    "",

    "",
    "",
    "",

    "Bip01 LUpArmTwist",
    "Bip01 L ForeTwist",
    "",

    "Bip01 RUpArmTwist",
    "Bip01 R ForeTwist",
    "",
};

struct BoneData 
{
    BodyPart body;
    float radius;
    float height;
    IPhysicsSimulator::HeightAxis heightAxis;
    FmVec3 position;
    FmVec3 rotation;
    FmVec3 offset;
};

static const BoneData g_BoneDatas[BODYPART_COUNT] = 
{
    {BODY_SPINE,     0.13f,  0.06f, IPhysicsSimulator::HEIGHT_AXIS_Y, FmVec3(0.0f, 1.0f, 0.0f),    FmVec3(0.0f, 0.0f, M_PI_2),  FmVec3(0.0f, 0.0f, 0.0f)},  // Spine
    {BODY_SPINE1,    0.13f,  0.08f, IPhysicsSimulator::HEIGHT_AXIS_Y, FmVec3(0.0f, 1.15f, 0.0f),   FmVec3(0.0f, 0.0f, M_PI_2),  FmVec3(0.03f, 0.0f, 0.0f)}, // Spine1
    {BODY_SPINE2,    0.14f,  0.10f, IPhysicsSimulator::HEIGHT_AXIS_Y, FmVec3(0.0f, 1.35f, 0.0f),   FmVec3(0.0f, 0.0f, M_PI_2),  FmVec3(0.1f, 0.0f, 0.0f)},  // Spine2
    {BODY_HEAD,      0.1f,   0.05f, IPhysicsSimulator::HEIGHT_AXIS_X, FmVec3(0.0f, 1.6f, 0.0f),    FmVec3(0.0f, 0.0f, M_PI_2),  FmVec3(0.025f, 0.0f, 0.0f)},// Head
                                                                                                  
    {BODY_L_THIGH,   0.08f,  0.45f, IPhysicsSimulator::HEIGHT_AXIS_X, FmVec3(-0.13f, 0.65f, 0.0f), FmVec3(0.0f, 0.0f, -M_PI_2), FmVec3(0.25f, 0.0f, 0.0f)}, // L Thigh
    {BODY_L_CALF,    0.06f,  0.40f, IPhysicsSimulator::HEIGHT_AXIS_X, FmVec3(-0.13f, 0.20f, 0.0f), FmVec3(0.0f, 0.0f, -M_PI_2), FmVec3(0.22f, 0.0f, 0.0f)}, // L Calf
    {BODY_L_FOOT,    0.04f,  0.15f, IPhysicsSimulator::HEIGHT_AXIS_Z, FmVec3(-0.13f, 0.0f, 0.075f),FmVec3(0.0f, 0.0f, 0.0f),    FmVec3(0.0f, 0.0f, 0.08f)}, // L Foot
                                                                                                  
    {BODY_R_THIGH,   0.08f,  0.45f, IPhysicsSimulator::HEIGHT_AXIS_X, FmVec3(0.13f, 0.65f, 0.0f),  FmVec3(0.0f, 0.0f, -M_PI_2), FmVec3(0.25f, 0.0f, 0.0f)}, // R Thigh
    {BODY_R_CALF,    0.06f,  0.40f, IPhysicsSimulator::HEIGHT_AXIS_X, FmVec3(0.13f, 0.20f, 0.0f),  FmVec3(0.0f, 0.0f, -M_PI_2), FmVec3(0.22f, 0.0f, 0.0f)}, // R Calf
    {BODY_R_FOOT,    0.04f,  0.15f, IPhysicsSimulator::HEIGHT_AXIS_Z, FmVec3(0.13f, 0.0f, 0.075f), FmVec3(0.0f, 0.0f, 0.0f),    FmVec3(0.0f, 0.0f, 0.08f)}, // R Foot
                                                                                                  
    {BODY_L_UPPERARM,0.05f,  0.33f, IPhysicsSimulator::HEIGHT_AXIS_X, FmVec3(-0.35f, 1.45f, 0.0f), FmVec3(0.0f, 0.0f, 0.0f),    FmVec3(0.18f, 0.0f, 0.0f)}, // L UpperArm
    {BODY_L_FOREARM, 0.04f,  0.25f, IPhysicsSimulator::HEIGHT_AXIS_X, FmVec3(-0.7f, 1.45f, 0.0f),  FmVec3(0.0f, 0.0f, 0.0f),    FmVec3(0.13f, 0.0f, 0.0f)}, // L Forearm
    {BODY_L_HAND,    0.03f,  0.12f, IPhysicsSimulator::HEIGHT_AXIS_X, FmVec3(-0.9f, 1.45f, 0.0f),  FmVec3(0.0f, 0.0f, 0.0f),    FmVec3(0.07f, 0.0f, 0.0f)}, // L Hand
                                                                                                  
    {BODY_R_UPPERARM,0.05f,  0.33f, IPhysicsSimulator::HEIGHT_AXIS_X, FmVec3(0.35f, 1.45f, 0.0f),  FmVec3(0.0f, 0.0f, M_PI),    FmVec3(0.18f, 0.0f, 0.0f)}, // R UpperArm
    {BODY_R_FOREARM, 0.04f,  0.25f, IPhysicsSimulator::HEIGHT_AXIS_X, FmVec3(0.7f, 1.45f, 0.0f),   FmVec3(0.0f, 0.0f, M_PI),    FmVec3(0.13f, 0.0f, 0.0f)}, // R Forearm
    {BODY_R_HAND,    0.04f,  0.12f, IPhysicsSimulator::HEIGHT_AXIS_X, FmVec3(0.9f, 1.45f, 0.0f),   FmVec3(0.0f, 0.0f, M_PI),    FmVec3(0.07f, 0.0f, 0.0f)}, // R Hand
};

enum JointPart
{
    JOINT_SPINE_0 = 0,
    JOINT_SPINE_1,
    JOINT_SPINE_HEAD,

    JOINT_L_HIP,
    JOINT_L_KNEE,
    JOINT_L_ANKLE,

    JOINT_R_HIP,
    JOINT_R_KNEE,
    JOINT_R_ANKLE,

    JOINT_L_SHOULDER,
    JOINT_L_ELBOW,
    JOINT_L_WRIST,

    JOINT_R_SHOULDER,
    JOINT_R_ELBOW,
    JOINT_R_WRIST,
};

enum JointType
{
    JT_POINT,
    JT_HINGE,
    JT_CONETWIST,
    JT_FIX,
};

struct JointData 
{
    JointPart jointPart;
    BodyPart bodyA;
    BodyPart bodyB;
    JointType jointType;
    FmVec3 rotationA;
    FmVec3 rotationB;
    FmVec4 params;
};

static const JointData g_JointDatas[JOINTPART_COUNT] = 
{
    {JOINT_SPINE_0,    BODY_SPINE,      BODY_SPINE1,     JT_CONETWIST, FmVec3(0,0,-M_PI_2),         FmVec3(M_PI_2,0,0),    FmVec4(M_PI_8, M_PI_8, M_PI_8,0)},
    {JOINT_SPINE_1,    BODY_SPINE1,     BODY_SPINE2,     JT_CONETWIST, FmVec3(0,0,0),               FmVec3(0,0,0),         FmVec4(M_PI_8, M_PI_8, M_PI_8,0)},
    {JOINT_SPINE_HEAD, BODY_SPINE2,     BODY_HEAD,       JT_CONETWIST, FmVec3(0,0,0),               FmVec3(0,0,0),         FmVec4(M_PI_8, M_PI_8, M_PI_8,0)},

    {JOINT_L_HIP,      BODY_SPINE,      BODY_L_THIGH,    JT_CONETWIST, FmVec3(M_PI,-M_PI_8,-M_PI_2),FmVec3(M_PI_2,0,0),    FmVec4(M_PI_4, M_PI_4,0,0)},
    {JOINT_L_KNEE,     BODY_L_THIGH,    BODY_L_CALF,     JT_HINGE,     FmVec3(M_PI_2,0,0),          FmVec3(M_PI_2,0,0),    FmVec4(0, M_PI_2,0,0)},
    {JOINT_L_ANKLE,    BODY_L_CALF,     BODY_L_FOOT,     JT_HINGE,     FmVec3(M_PI_2,0,0),          FmVec3(M_PI_2,0,0),    FmVec4(-M_PI_4,M_PI_4,0,0)},                      

    {JOINT_R_HIP,      BODY_SPINE,      BODY_R_THIGH,    JT_CONETWIST, FmVec3(M_PI, M_PI_8,-M_PI_2),FmVec3(M_PI_2,0,0),    FmVec4(M_PI_4, M_PI_4,0,0)},
    {JOINT_R_KNEE,     BODY_R_THIGH,    BODY_R_CALF,     JT_HINGE,     FmVec3(M_PI_2,0,0),          FmVec3(M_PI_2,0,0),    FmVec4(0, M_PI_2,0,0)},
    {JOINT_R_ANKLE,    BODY_R_CALF,     BODY_R_FOOT,     JT_HINGE,     FmVec3(M_PI_2,0,0),          FmVec3(M_PI_2,0,0),    FmVec4(-M_PI_4,M_PI_4,0,0)},

    {JOINT_L_SHOULDER, BODY_SPINE2,     BODY_L_UPPERARM, JT_CONETWIST, FmVec3(M_PI_2,0,M_PI_4*3.0f),FmVec3(M_PI_2,0,0),    FmVec4(M_PI_4, M_PI_8,0,0)},
    {JOINT_L_ELBOW,    BODY_L_UPPERARM, BODY_L_FOREARM,  JT_HINGE,     FmVec3(M_PI_2,0,0),          FmVec3(M_PI_2,0,0),    FmVec4(0, M_PI_2,0,0)},
    {JOINT_L_WRIST,    BODY_L_FOREARM,  BODY_L_HAND,     JT_CONETWIST, FmVec3(0,0,0),               FmVec3(M_PI_2,0,0),    FmVec4(M_PI_4, M_PI_4,0,0)},

    {JOINT_R_SHOULDER, BODY_SPINE2,     BODY_R_UPPERARM, JT_CONETWIST, FmVec3(M_PI_2,0,M_PI_4*3.0f),FmVec3(M_PI_2,0,0),    FmVec4(M_PI_4, M_PI_8,0,0)},
    {JOINT_R_ELBOW,    BODY_R_UPPERARM, BODY_R_FOREARM,  JT_HINGE,     FmVec3(M_PI_2,0,0),          FmVec3(M_PI_2,0,0),    FmVec4(0, M_PI_2,0,0)},
    {JOINT_R_WRIST,    BODY_R_FOREARM,  BODY_R_HAND,     JT_CONETWIST, FmVec3(0,0,0),               FmVec3(-M_PI_2,0,0),   FmVec4(M_PI_4, M_PI_4,0,0)},
};
                                                                     
// --------------------------------------------------------------------------------------

Ragdoll::Ragdoll()
{
    memset(m_shapes, 0, sizeof(m_shapes));
    memset(m_bodies, 0, sizeof(m_bodies));
    memset(m_joints, 0, sizeof(m_joints));

    m_pActor = NULL;
    m_bDynamicDriving = false;

    m_ref = 1;

    m_scale = 1.0f;

    m_pUserPointer = NULL;

    m_next = NULL;
    m_prev = NULL;
}

Ragdoll::~Ragdoll()
{
    Destroy();
}

void Ragdoll::Release()
{
    m_ref--;
    if (m_ref == 0)
    {
        g_physicsBulletPtr->DestoryRagdoll(this);
    }
}

void Ragdoll::AddRef()
{
    m_ref++;
}

bool Ragdoll::IsReady() const
{
    return m_pActor && m_bodies[0];
}

bool Ragdoll::Build(const FmVec3& vPos, float scale)
{
    Destroy();

    // Setup the shapes
    float params[4];
    memset(params, 0, sizeof(params));
    for (unsigned int i = 0; i < BODYPART_COUNT; i++)
    {
        params[0] = g_BoneDatas[i].radius*scale;
        params[1] = g_BoneDatas[i].height*scale;
        CollisionShape* pShape = g_physicsBulletPtr->FindCollisionShape(YE_CST_CAPSULE, g_BoneDatas[i].heightAxis, params);
        if (pShape)
        {
            pShape->AddRef();
            m_shapes[i] = pShape;
        }
        else
        {
            m_shapes[i] = g_physicsBulletPtr->CreateCapsuleCollisionShape(params[0], params[1], g_BoneDatas[i].heightAxis);
        }
    }

    // Setup all the rigid bodies
    FmMat4 transform;
    FmVec3 vOffset;
    FmVec3 vVelocity(0.0f);
    float mass = 1.0f;
    for (unsigned int i = 0; i < BODYPART_COUNT; i++)
    {
        FmMat4EulerZYX(&transform, g_BoneDatas[i].rotation.x, g_BoneDatas[i].rotation.y, g_BoneDatas[i].rotation.z);
        transform._41 = vPos.x + g_BoneDatas[i].position.x*scale;
        transform._42 = vPos.y + g_BoneDatas[i].position.y*scale;
        transform._43 = vPos.z + g_BoneDatas[i].position.z*scale;

        m_bodies[i] = g_physicsBulletPtr->CreateRigidBody(m_shapes[i], mass, transform, vVelocity);

        m_bodies[i]->SetDamping(0.05f, 0.85f);
        m_bodies[i]->SetDeactivationTime(0.8f);
        m_bodies[i]->SetSleepingThresholds(1.6f, 2.5f);

        vOffset = g_BoneDatas[i].offset*scale;
        m_bodies[i]->SetOffset(vOffset);

        m_bodies[i]->SetVisible(true);
    }

    m_scale = scale;

    return true;
}

void Ragdoll::Destroy()
{
    for (int i = 0; i < JOINTPART_COUNT; ++i)
    {
        SAFE_RELEASE(m_joints[i]);
    }

    for (int i = 0; i < BODYPART_COUNT; ++i)
    {
        SAFE_RELEASE(m_bodies[i]);
    }

    for (int i = 0; i < BODYPART_COUNT; ++i)
    {
        SAFE_RELEASE(m_shapes[i]);
    }
}

bool Ragdoll::BuildJoints()
{
    DestroyJoints();

    if (!m_pActor)
    {
        return false;
    }

    FmVec3 vPos;
    FmVec3 vAngle;
    FmMat4 mtx;
    for (unsigned int i = 0; i < BODYPART_COUNT; i++)
    {
        if (m_bodies[i] &&
            m_pActor->GetNodePosition(g_szBonesName[i], vPos) &&
            m_pActor->GetNodeAngle(g_szBonesName[i], vAngle))
        {
            FmMat4Identity(&mtx);
            VisUtil_SetMatrixAngle(&mtx, vAngle.x, vAngle.y, vAngle.z);
            mtx._41 = vPos.x;
            mtx._42 = vPos.y;
            mtx._43 = vPos.z;

            m_bodies[i]->SetTransform(mtx);
        }
    }

    HingeConstraintData hingeData;
    ConeTwistConstraintData coneTwistData;
    Point2PointConstraintData p2pData;
    FixConstraintData fixData;

    FmVec3 vPosA;
    FmVec3 vPosB;
    FmVec3 vLocalA;
    FmVec3 vLocalB;
    for (unsigned int i = 0; i < JOINTPART_COUNT; i++)
    {
        IRigidBody* rbA = m_bodies[g_JointDatas[i].bodyA];
        IRigidBody* rbB = m_bodies[g_JointDatas[i].bodyB];

        if (!rbA || !rbB)
        {
            continue;
        }

        if (i < 3)
        {
            ((RigidBody*)rbA)->GetPositionRB(vPosA);
            ((RigidBody*)rbB)->GetPositionRB(vPosB);

            vPos = (vPosA + vPosB) * 0.5f;
            rbA->ConvertPosFromWorldToLocal(vPos, vLocalA);
            rbB->ConvertPosFromWorldToLocal(vPos, vLocalB);
        }
        else
        {
            m_pActor->GetNodePosition(g_szBonesName[g_JointDatas[i].bodyB], vPosB);
            rbA->ConvertPosFromWorldToLocal(vPosB, vLocalA);
            rbB->ConvertPosFromWorldToLocal(vPosB, vLocalB);
        }


        if (g_JointDatas[i].jointType == JT_HINGE)
        {
            FmMat4EulerZYX(&hingeData.m_rbAFrame, g_JointDatas[i].rotationA.x, g_JointDatas[i].rotationA.y, g_JointDatas[i].rotationA.z);
            hingeData.m_rbAFrame._41 = vLocalA.x;
            hingeData.m_rbAFrame._42 = vLocalA.y;
            hingeData.m_rbAFrame._43 = vLocalA.z;

            FmMat4EulerZYX(&hingeData.m_rbBFrame, g_JointDatas[i].rotationB.x, g_JointDatas[i].rotationB.y, g_JointDatas[i].rotationB.z);
            hingeData.m_rbBFrame._41 = vLocalB.x;
            hingeData.m_rbBFrame._42 = vLocalB.y;
            hingeData.m_rbBFrame._43 = vLocalB.z;

            hingeData.m_lowerLimit = g_JointDatas[i].params.x;
            hingeData.m_upperLimit = g_JointDatas[i].params.y;

            m_joints[i] = g_physicsBulletPtr->CreateHingeConstraint(rbA, rbB, hingeData);
        }
        else if (g_JointDatas[i].jointType == JT_CONETWIST)
        {
            FmMat4EulerZYX(&coneTwistData.m_rbAFrame, g_JointDatas[i].rotationA.x, g_JointDatas[i].rotationA.y, g_JointDatas[i].rotationA.z);
            coneTwistData.m_rbAFrame._41 = vLocalA.x;
            coneTwistData.m_rbAFrame._42 = vLocalA.y;
            coneTwistData.m_rbAFrame._43 = vLocalA.z;

            FmMat4EulerZYX(&coneTwistData.m_rbBFrame, g_JointDatas[i].rotationB.x, g_JointDatas[i].rotationB.y, g_JointDatas[i].rotationB.z);
            coneTwistData.m_rbBFrame._41 = vLocalB.x;
            coneTwistData.m_rbBFrame._42 = vLocalB.y;
            coneTwistData.m_rbBFrame._43 = vLocalB.z;

            coneTwistData.m_swingSpan1 = g_JointDatas[i].params.x;
            coneTwistData.m_swingSpan2 = g_JointDatas[i].params.y;
            coneTwistData.m_twistSpan  = g_JointDatas[i].params.z;

            m_joints[i] = g_physicsBulletPtr->CreateConeTwistConstraint(rbA, rbB, coneTwistData);
        }
        else if (g_JointDatas[i].jointType == JT_FIX)
        {
            FmMat4EulerZYX(&fixData.m_rbAFrame, g_JointDatas[i].rotationA.x, g_JointDatas[i].rotationA.y, g_JointDatas[i].rotationA.z);
            fixData.m_rbAFrame._41 = vLocalA.x;
            fixData.m_rbAFrame._42 = vLocalA.y;
            fixData.m_rbAFrame._43 = vLocalA.z;

            FmMat4EulerZYX(&fixData.m_rbBFrame, g_JointDatas[i].rotationB.x, g_JointDatas[i].rotationB.y, g_JointDatas[i].rotationB.z);
            fixData.m_rbBFrame._41 = vLocalB.x;
            fixData.m_rbBFrame._42 = vLocalB.y;
            fixData.m_rbBFrame._43 = vLocalB.z;

            m_joints[i] = g_physicsBulletPtr->CreateFixConstraint(rbA, rbB, fixData);
        }
        else
        {
            p2pData.m_pivotInA = vLocalA;
            p2pData.m_pivotInB = vLocalB;

            m_joints[i] = g_physicsBulletPtr->CreatePoint2PointConstraint(rbA, rbB, p2pData);
        }
    }

    return true;
}

void Ragdoll::DestroyJoints()
{
    for (int i = 0; i < JOINTPART_COUNT; ++i)
    {
        SAFE_RELEASE(m_joints[i]);
    }
}

void Ragdoll::Update()
{
    if (!m_pActor)
    {
        return;
    }

    FmVec3 vPos;
    FmVec3 vAngle;
    FmMat4 mtx;

    if (!m_bDynamicDriving)
    {    
        // 纯关键帧驱动
        for (unsigned int i = 0; i < BODYPART_COUNT; i++)
        {
            if (m_bodies[i] &&
                m_pActor->GetNodeWorldMatrix(g_szBonesName[i], mtx))
            {
                m_bodies[i]->SetTransform(mtx);
            }
        }
    }
    else
    {
        // 纯动力学驱动
        for (unsigned int i = 0; i < BODYPART_COUNT; i++)
        {
            if (m_bodies[i] && m_bodies[i]->GetTransform(mtx))
            {
                m_pActor->SetNodeWorldMatrix(g_szBonesName[i], mtx);
                if (g_szBonesAttachName[i][0])
                {
                    m_pActor->SetNodeWorldMatrix(g_szBonesAttachName[i], mtx);
                }
            }
        }

        m_pActor->UserBoneUpdate();
    }
}

IRigidBody* Ragdoll::GetRigidBody(const char* name)
{
    for (unsigned int i = 0; i < BODYPART_COUNT; i++)
    {
        if (stricmp(g_szBonesName[i], name) == 0)
        {
            return m_bodies[i];
        }
    }
    return NULL;
}

void Ragdoll::SetActor(IActor* pActor)
{
    m_pActor = pActor;

    if (m_pActor)
    {
        // 将刚体的矩阵设置为角色的矩阵
        FmMat4 mtx;
        for (unsigned int i = 0; i < BODYPART_COUNT; i++)
        {
            if (m_bodies[i] &&
                m_pActor->GetNodeWorldMatrix(g_szBonesName[i], mtx))
            {
                m_bodies[i]->SetTransform(mtx);
            }
        }
    }
}

// 设置世界变换
bool Ragdoll::SetTransform(const FmMat4& mtx, bool teleport)
{
    if (!m_pActor)
    {
        return false;
    }

    m_pActor->SetWorldMatrix(mtx);

    // 将刚体的矩阵设置为角色的矩阵
    FmMat4 mtxBone;
    for (unsigned int i = 0; i < BODYPART_COUNT; i++)
    {
        if (m_bodies[i] &&
            m_pActor->GetNodeWorldMatrix(g_szBonesName[i], mtxBone))
        {
            m_bodies[i]->SetTransform(mtxBone);
        }
    }

    return true;
}

void Ragdoll::SetDynamicDrive(const FmVec3& vVelocity)
{
    if (!m_pActor || m_bDynamicDriving)
    {
        return;
    }

    m_bDynamicDriving = true;

    for (int i = 0; i < BODYPART_COUNT; i++)
    {
        int flags;
        m_bodies[i]->GetCollisionFlags(flags);
        flags &= ~IRigidBody::CNF_KINEMATIC_OBJECT;
        m_bodies[i]->SetCollisionFlags(flags);

        m_bodies[i]->SetActivationState(IRigidBody::AS_ACTIVE_TAG);

        m_bodies[i]->SetLinearVelocity(vVelocity);
        FmVec3 vAngle(0.0f);
        m_bodies[i]->SetAngularVelocity(vAngle);
    }

    m_pActor->EnableUserBone(true);
}

void Ragdoll::SetKeyFrameDrive()
{
    if (!m_pActor || !m_bDynamicDriving)
    {
        return;
    }

    m_bDynamicDriving = false;

    for (int i = 0; i < BODYPART_COUNT; i++)
    {
        int flags;
        m_bodies[i]->GetCollisionFlags(flags);
        flags |= IRigidBody::CNF_KINEMATIC_OBJECT;
        m_bodies[i]->SetCollisionFlags(flags);
    }

    m_pActor->EnableUserBone(false);
}