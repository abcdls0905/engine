//--------------------------------------------------------------------
// �ļ���:		actor_look.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2008��12��3��
// ������:		
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "actor.h"
#include "../visual/i_action_player.h"
#include "../visual/i_context.h"
#include "../visual/vis_utils.h"
#include "../public/var_list.h"
#include "../public/inlines.h"
#include "../public/core_log.h"
#include "../public/portable.h"
// look at

#define PITCH_MAX_ANGLE FLOAT_PI / 4.0F		// תͷ���ܣ��������ֵ
#define PITCH_MIN_ANGLE -FLOAT_PI / 3.0F
#define ORIENT_MAX_ANGLE FLOAT_PI / 3.0F		// תͷ���ܣ�Yת�����ֵ
#define ANGLE_SPEED_RATIO 0.04F				// �Ƕȸı�����
#define MAX_ANGLE_CHANGE_STEP 0.06F			// ��󵥲��ٶ�

// ��һ�������С�Ļ����۵���[0,2PI)��
inline float nip_radian(float r)
{
	float ret = r - (float(int(r / (2.0F * FLOAT_PI))) * (2.0F * FLOAT_PI));

	if (ret < 0)
	{
		ret = ret + 2.0F * FLOAT_PI;
	}

	return ret;
}

// ��һ�������С�Ļ����۵���[-PI,PI)��
inline float nip_radianII(float r)
{
	float ret = nip_radian(r);

	if (ret > FLOAT_PI)
	{
		ret = ret - 2.0F * FLOAT_PI;
	}

	return ret;
}

// ��ȡ�ڲ�������Ƕ�
inline bool get_node_angle(Actor* pActor, const char* pNodeName, 
	float& angle)
{
	FmVec3 vAngle;
	
	if (!pActor->GetNodeAngle(pNodeName, vAngle))
	{
		return false;
	}

	angle = nip_radian(vAngle.y);

	return true;
}

// ��ȡ����ת��
inline bool get_bone_angle(Actor* pActor, const char* pNodeName, 
	float& angle)
{
	CVarList result;

	pActor->GetBoneAngle(CVarList() << pNodeName, result);
	
	if (0 == result.GetCount())
	{
		return false;
	}

	angle = result.FloatVal(0);

	return true;
}


// �󻡶�
inline float math_get_angle(float sub_x, float sub_z, float dist = -1.0F)
{
	float ret = 0.0F;

	if (dist < 0.0F)
	{
		dist = sqrt(sub_z * sub_z + sub_x * sub_x);
	}

	if (dist > 0.0001F)
	{
		if (sub_z < 0.0F)
		{
			ret = acosf(-sub_z / dist);
			ret = FLOAT_PI - ret;
		}
		else
		{
			ret = acosf(sub_z / dist);
		}

		if (sub_x < 0.0F)
		{
			ret = -ret;
		}

		// ��Ҫ�����Ƿ����������ֵļ�飬��Ϊacosf�Ľ�������Ѿ��Ƿ�
		if (Port_IsNaN(ret))
		{
			return 0.0F;
		}
	}
	
	return nip_radian(ret);
}

// ��ȡ��ǰĿ��Ƕȵ�ʵ����ת����Ƕ�0-2PI
inline float get_cur_target_angle(Actor* pVisSelf, IVisBase* pVisTarget)
{
	FmVec3 vTarget = pVisTarget->GetPosition();
	FmVec3 vSelf = pVisSelf->GetPosition();
	FmVec3 vSubVec3 = vTarget - vSelf;

	return math_get_angle(vSubVec3.x, vSubVec3.z);
}

inline bool get_cur_pitch_angle(Actor* pVisSelf, IVisBase* pVisTarget, 
	float& pitch)
{
	FmVec3 vTarget, vSelf;

    if (pVisTarget->GetEntInfo()->IsKindOf("Actor"))
    {
	    Actor* pTargetActorRole = (Actor*)pVisTarget;
	    PERSISTID actor_role = pTargetActorRole->GetLinkObject("actor_role");

	    if (!actor_role.IsNull())
	    {
		    pTargetActorRole = (Actor*)pVisSelf->GetCore()->GetEntity(
				actor_role);

		    if (pTargetActorRole == NULL)
		    {
			    pTargetActorRole = (Actor*)pVisTarget;
		    }
	    }

	    if (!pTargetActorRole->GetNodePosition("Bip01 Head", vTarget))
	    {
		    return false;
	    }
    }
    else
    {
        vTarget = pVisTarget->GetPosition();
    }

	Actor* pSelfActorRole = pVisSelf;
	PERSISTID actor_role = pVisSelf->GetLinkObject("actor_role");

	if (!actor_role.IsNull())
	{
		pSelfActorRole = (Actor*)pVisSelf->GetCore()->GetEntity(actor_role);

		if (pSelfActorRole == NULL)
		{
			pSelfActorRole = pVisSelf;
		}
	}

	if (!pSelfActorRole->GetNodePosition("Bip01 Head", vSelf))
	{
		return false;
	}

	FmVec3 vSubVec3 = vTarget - vSelf;
	float xz_dist = sqrt(vSubVec3.x * vSubVec3.x + vSubVec3.z * vSubVec3.z);

	pitch = nip_radianII(math_get_angle(-vSubVec3.y, xz_dist)); 

	return true;
}

// ��ȡ��ǰĿ��Ƕȵ�ʵ����ת����Ƕ�
inline float get_cur_fit_target_angle(float dest_angle, float begin_angle, 
	float end_angle)
{
	// ȡ�����ķ�Χ��
	dest_angle = nip_radian(dest_angle);

	// ��ȡ�����ķ�Χ��

	if ((dest_angle > begin_angle && dest_angle < end_angle) 
		|| (dest_angle + FLOAT_PI * 2.0F >= begin_angle 
		&& dest_angle + FLOAT_PI * 2.0F <= end_angle) 
		|| (dest_angle - FLOAT_PI * 2.0F >= begin_angle 
		&& dest_angle - FLOAT_PI * 2.0F <= end_angle))
	{
		// ������Χangle����
		return dest_angle;
	}
	else
	{
		// Ӧ��Ŀ�귶Χ
		float angle1 = fabs(nip_radianII(begin_angle - dest_angle));
		float angle2 = fabs(nip_radianII(end_angle - dest_angle));

		if (angle1 < angle2)
		{
			return nip_radian(begin_angle);
		}
		else
		{
			return nip_radian(end_angle);
		}
	}
}

bool Actor::UpdateAimObjectAngle()
{
	if (m_pActionPlayer == NULL)
	{
		return false;
	}

	// ���Ŀ�겻�����ˣ���ԭ
	if (m_AimObject.IsNull())
	{
		return ResumeAimObjectAngle();
	}

	IVisBase* pAimObj = (IVisBase*)GetCore()->GetEntity(m_AimObject);

	if (NULL == pAimObj)
	{
		m_AimObject = PERSISTID();
		return ResumeAimObjectAngle();
	}

	FmVec3 vSubPos = pAimObj->GetPosition() - this->GetPosition();

	//if (D3DXVec3Length(&vSubPos) > 10.0F)
	if (FmVec3Length(&vSubPos) > (10.0F * 10.0F))
	{
		return ResumeAimObjectAngle();
	}

	// 1.����ȥ��ĳЩ������Ӧ��ת��
	FmVec3 vAngle;

	if (!this->GetNodeAngle("Bip01 Spine1", vAngle) || 
        fabs(vAngle.x) > 0.2F ||
        fabs(nip_radianII(vAngle.z - 1.57F)) > 0.5F)
	{
		return ResumeAimObjectAngle();
	}

	UpdateAimObjectPitchAngle();
	UpdateAimObjectOrientAngle();

	return true;
}

bool Actor::UpdateAimObjectPitchAngle()
{
	IVisBase* pAimObj = (IVisBase*)GetCore()->GetEntity(m_AimObject);

	if (NULL == pAimObj)
	{
		return false;
	}

	// ��ȡĿ�����ǵĽǶ�Ϊ0-2PI
	float dest_pitch_angle = 0.0F;
	
	if (!get_cur_pitch_angle(this, pAimObj, dest_pitch_angle))
	{
		return ResumeAimObjectPitchAngle();
	}

	// ��ȡʵ�ʿɵ������ĵ����Ƕ�
	if (dest_pitch_angle < PITCH_MIN_ANGLE)
	{
		dest_pitch_angle = PITCH_MIN_ANGLE;
	}
	else if (dest_pitch_angle > PITCH_MAX_ANGLE)
	{
		dest_pitch_angle = PITCH_MAX_ANGLE;
	}

	// ��������x��б���ܽ���ת������Ȼ�������
	FmVec3 vAngle;
	
	if (!this->GetNodeAngle("Bip01 Head", vAngle))
	{
		return ResumeAimObjectPitchAngle();
	}

	if (vAngle.x > PITCH_MAX_ANGLE)
	{
		return ResumeAimObjectPitchAngle(vAngle.x - dest_pitch_angle);
	}
	else if (vAngle.x < PITCH_MIN_ANGLE)
	{
		return ResumeAimObjectPitchAngle(dest_pitch_angle - vAngle.x);
	}

	// ��������---------------------------------------------------------------
	// ͷ�����Ǹ���,GetBoneAngleAppend����ӿڻ�ȡ��y��x��ʵ�ʵ�x,y
	FmVec3 angle;

	if (!m_pActionPlayer->GetBoneAngleAppend("Bip01 Head", &angle))
	{
		return ResumeAimObjectPitchAngle();
	}

	float add_head_pitch = -angle.y;

	// ʵ��ͷ������
	if (!this->GetNodeAngle("Bip01 Head", vAngle))
	{
		return ResumeAimObjectPitchAngle();
	}

	float cur_head_pitch = vAngle.x;

	// ʵ��δ����ת��ʱ�ĽǶ�
	float real_head_pitch = nip_radianII(cur_head_pitch - add_head_pitch);

	// Ŀ�����Ǻ͵�ǰ���ǵĲ�, ������ͺ�ķ�ʽ������Ŀ��ת
	// ����Ϊ��������ͻȻת�������
	float new_add_head_pitch = dest_pitch_angle - real_head_pitch;
	float sub_pitch_angle = dest_pitch_angle - cur_head_pitch;

	//ͷ�����Ǹ���,SetBoneAngle����ӿ����õ�x,y��ʵ�ʵ�y,x
	float sub_add = nip_radianII(new_add_head_pitch - add_head_pitch);
	float pitch_step = fabs(sub_add * ANGLE_SPEED_RATIO);

	if (new_add_head_pitch - add_head_pitch < 0.0F)
	{
		pitch_step = -pitch_step;
	}

	float now_add_head_pitch = add_head_pitch + pitch_step;

	if (now_add_head_pitch > PITCH_MAX_ANGLE)
	{
		now_add_head_pitch = PITCH_MAX_ANGLE - 0.0001F;
	}
	else if (now_add_head_pitch < PITCH_MIN_ANGLE)
	{
		now_add_head_pitch = PITCH_MIN_ANGLE + 0.0001F;
	}

	// ��ֹ��������ֵ������ۻ�
	SetBoneAngleNoScale("Bip01 Head", angle.x, -(now_add_head_pitch), angle.z);

	return true;
}

bool Actor::UpdateAimObjectOrientAngle()
{
	IVisBase* pAimObj = (IVisBase*)GetCore()->GetEntity(m_AimObject);

	if (NULL == pAimObj)
	{
		return false;
	}
	// ת��ֻת�ĸ�����Bip01 Head��Bip01 Spine2��Bip01 Spine1��Bip01 Spine
	// NPC����û��Bip01 Spine2�������ﲻ������
	// ��ȡͷ����������ת��
	//float fface_angle_y = get_logic_node_angle(this, "main_model::head_01");
	float fface_angle_y = 0.0F;
	
	if (!get_node_angle(this, "Bip01 Head", fface_angle_y))
	{
		return ResumeAimObjectOrientAngle();
	}

	// ��ȡ������������ʵ����,�����Ϊ�˵����巽��Ϊ�β�����
	float fbody_angle_y = 0.0F;
	
	if (!get_node_angle(this, "Bip01 Pelvis", fbody_angle_y))
	{
		return ResumeAimObjectOrientAngle();
	}

	// 3.��Ҳ������������ת������, ����ǰ1��2��ʱ�����΢�����
	float test_orient_angle = fabs(nip_radianII(fface_angle_y - fbody_angle_y)) 
		- ORIENT_MAX_ANGLE;

	bool need_resume = false;

	if (test_orient_angle > 0.0F)
	{
		need_resume = true;
	}

	// ��ȡ��ǰ����Χ�������꣬ �����������60�ȣ���Χ
	float min_face_angle = fbody_angle_y - ORIENT_MAX_ANGLE;
	float max_face_angle = fbody_angle_y + ORIENT_MAX_ANGLE;

	// ��ȡ��������ת��,
	float fbody_spine_y = 0.0F;
	
	if (!get_node_angle(this, "Bip01 Spine", fbody_spine_y))
	{
		return ResumeAimObjectOrientAngle();
	}

	// ת��ֻת�ĸ�����Bip01 Neck��Bip01 Spine2��Bip01 Spine1��Bip01 Spine
	// NPC����û��Bip01 Spine2�������ﲻ������
	// ��ȡ��ǰʵ��ת���ĽǶ�
	float add_neck_angle = 0.0F;

	if (!get_bone_angle(this, "Bip01 Neck", add_neck_angle))
	{
		return ResumeAimObjectOrientAngle();
	}

	float add_spine1_angle = 0.0F;
	
	if (!get_bone_angle(this, "Bip01 Spine1", add_spine1_angle))
	{
		return ResumeAimObjectOrientAngle();
	}

	float add_spine_angle = 0.0F;

	if (!get_bone_angle(this, "Bip01 Spine", add_spine_angle))
	{
		return ResumeAimObjectOrientAngle();
	}

	float total_add_angle = add_neck_angle + add_spine1_angle 
		+ add_spine_angle;

	// ��ȡ�沿���β�(����)��ԭʼ�����ǶȲ�, Ҳ����δ������תʱ�ĽǶȲ�
	float sub_face_to_body = fface_angle_y - total_add_angle - fbody_angle_y;

	sub_face_to_body = nip_radianII(sub_face_to_body);

	// ���˵ĽǶ�Ϊ0-2PI
	float dest_face_angle = get_cur_target_angle(this, pAimObj);
	float test_dest_face_angle = nip_radianII(dest_face_angle - fbody_angle_y);

	dest_face_angle = fbody_angle_y + test_dest_face_angle;

	// 0-2PI
	dest_face_angle = get_cur_fit_target_angle(dest_face_angle, 
		min_face_angle, max_face_angle);

	// Ŀ��ǶȺ͵�ǰʵ�ʽǶȵĲ�ֵ
	float sub_dest_face_angle = nip_radianII(dest_face_angle - fbody_angle_y);
	float sub_cur_face_angle = nip_radianII(fface_angle_y - fbody_angle_y);
	float sub_face_angle = sub_dest_face_angle - sub_cur_face_angle;

	// ���Ŀ�귽�����resume��������Ҫresume,������ǲ���Ҫ
	if (need_resume)
	{
		if (fface_angle_y - fbody_angle_y > 0.0F && sub_face_angle < 0.0F 
			|| fface_angle_y - fbody_angle_y < 0.0F && sub_face_angle > 0.0F)
		{
			// ����Ҫresume
		}
		else
		{
			return ResumeAimObjectOrientAngle(test_orient_angle);
		}
	}

	total_add_angle = nip_radianII(total_add_angle);

	// ��һ֡ʵ�������ĽǶ��ĵȷ�
	float step = sub_face_angle * ANGLE_SPEED_RATIO;

	float new_add_angle = (total_add_angle + step) / 4.0F;

	// ƽ�����ӵ��ĸ�������
	SetBoneAngleNoScale("Bip01 Spine", new_add_angle, 0.0F, 0.0F);
	SetBoneAngleNoScale("Bip01 Spine1", new_add_angle, 0.0F, 0.0F);
	SetBoneAngleNoScale("Bip01 Neck", new_add_angle * 2, 0.0F, 0.0F);

	// Ϊ������ͷ���������������
	UpdateAimObjectHeadAngleZ();

	return true;
}

bool Actor::UpdateAimObjectHeadAngleZ()
{
	FmVec3 vAngle;
	
	if (!GetNodeAngle("Bip01 Head", vAngle))
	{
		return ResumeAimObjectHeadAngleZ();
	}

	CVarList result;

	GetBoneAngle(CVarList() << "Bip01 Head", result);

	if (0 == result.GetCount())
	{
		return ResumeAimObjectHeadAngleZ();
	}

	float append_angle_x = result.FloatVal(0);
	float append_angle_y = result.FloatVal(1);
	float append_angle_z = result.FloatVal(2);
	// ���ԭʼ״̬���������ģ����軹ԭ,����Ҫ�ٸ��ӽǶ���
	float real_angle_z = vAngle.z - append_angle_z;

	if (real_angle_z > 1.5F && real_angle_z < 1.6F)
	{
		return ResumeAimObjectHeadAngleZ(vAngle.z - append_angle_z);
	}

	float dest_angle_z = append_angle_z;

	if (vAngle.z > 1.6F)
	{
		dest_angle_z -= (vAngle.z - 1.6F) * 0.5F;
	}
	else if (vAngle.z < 1.4F)
	{
		dest_angle_z += (1.4F - vAngle.z) * 0.5F;
	}

	SetBoneAngleNoScale("Bip01 Head", append_angle_x, append_angle_y, 
		dest_angle_z);

	return true;
}

bool Actor::ResumeAimObjectHeadAngleZ(float sub_orient)
{
	CVarList result;

	GetBoneAngle(CVarList() << "Bip01 Head", result);

	if (0 == result.GetCount())
	{
		return false;
	}

	float append_angle_x = result.FloatVal(0);
	float append_angle_y = result.FloatVal(1);
	float append_angle_z = result.FloatVal(2);

	// ����Ƕȵ���0����Ҫ��ת��
	if (FloatEqual(append_angle_z, 0.0F))
	{
		return true;
	}

	float dest_angle_z = append_angle_z;
	float step = fabs(append_angle_z) * 0.4F;

	if (dest_angle_z > 0.0F)
	{
		if (dest_angle_z > step)
		{
			dest_angle_z -= step;
		}
		else
		{
			dest_angle_z = 0.0F;
		}
	}
	else
	{
		if (dest_angle_z < -step)
		{
			dest_angle_z += step;
		}
		else
		{
			dest_angle_z = 0.0F;
		}
	}

	SetBoneAngleNoScale("Bip01 Head", append_angle_x, append_angle_y, 
		dest_angle_z);

	return false;
}

bool Actor::ResumeAimObjectAngle()
{
	bool result = ResumeAimObjectPitchAngle(0.0F);

	result = result & ResumeAimObjectOrientAngle(0.0F);

	if (result)
	{
		m_bAutoAimObjAngle = false;
		m_AimObject = PERSISTID();
	}

	return true;
}

bool Actor::ResumeAimObjectPitchAngle(float sub_pitch)
{
	// ͷ������
	FmVec3 angle;

	m_pActionPlayer->GetBoneAngleAppend("Bip01 Head", &angle);
	
	float fadd_pitch_angle = -angle.y;

	// ����Ƕȵ���0����Ҫ��ת��
	if (FloatEqual(fadd_pitch_angle, 0.0F))
	{
		//if (m_AimObject.IsNull())
		//{
			//m_bAutoAimObjAngle = false;
		//}

		return true;
	}

	float step = 0.0F;

	if (sub_pitch > 0.000001F)
	{
		step = fabs(sub_pitch * ANGLE_SPEED_RATIO);

		if (fadd_pitch_angle > 0.0F)
		{
			step = -step;
		}
	}
	else
	{
		step = -fadd_pitch_angle * ANGLE_SPEED_RATIO;
	}

	/*if (step > 0.0F && step > MAX_ANGLE_CHANGE_STEP)
	{
		step = MAX_ANGLE_CHANGE_STEP;
	}
	else if (step < 0.0F && step < -MAX_ANGLE_CHANGE_STEP)
	{
		step = -MAX_ANGLE_CHANGE_STEP;
	}*/

	if (fabs(step) > 0.01F)
	{
		step = step;
	}

	float now_add_pitch = fadd_pitch_angle + step;

	SetBoneAngleNoScale("Bip01 Head", angle.x, -(now_add_pitch), angle.z);

	return false;
}

bool Actor::ResumeAimObjectOrientAngle(float sub_orient)
{
	float cur_add_angle_spine = 0.0F;
	float cur_add_angle_spine1 = 0.0F;
	float cur_add_angle_neck = 0.0F;

	if (!get_bone_angle(this, "Bip01 Spine", cur_add_angle_spine) 
		|| !get_bone_angle(this, "Bip01 Spine1", cur_add_angle_spine1) 
		|| !get_bone_angle(this, "Bip01 Neck", cur_add_angle_neck))
	{
		return false;
	}

	float fOldAngleTotal = cur_add_angle_spine + cur_add_angle_spine1 
		+ cur_add_angle_neck;
	// ת�Ǻ����Ҫͬʱ��ԭ
	bool result = ResumeAimObjectHeadAngleZ();

	// ����Ƕȵ���0����Ҫ��ת��
	if (FloatEqual(cur_add_angle_neck, 0.0F))
	{
		//if (m_AimObject.IsNull())
		//{
			//m_bAutoAimObjAngle = false;
		//}
		
		// ֻ�ж��ص�0�ŷ���true
		return result;
	}

	if (sub_orient >= -0.0F && !FloatEqual(cur_add_angle_neck, 0.0F))
	{
		float step = 0.0F;

		if (sub_orient > 0.0F)
		{
			step = fabs(sub_orient * ANGLE_SPEED_RATIO);

			if (fOldAngleTotal > 0.0F)
			{
				step = -step;
			}
		}
		else
		{
			step = -fOldAngleTotal * ANGLE_SPEED_RATIO;
		}

		if (step > 0.0F && step > MAX_ANGLE_CHANGE_STEP)
		{
			step = MAX_ANGLE_CHANGE_STEP;
		}
		else if (step < 0.0F && step < -MAX_ANGLE_CHANGE_STEP)
		{
			step = -MAX_ANGLE_CHANGE_STEP;
		}

		float fNowAngleTotal = fOldAngleTotal + step;
		float fdest_angle = (fNowAngleTotal) / 4.0F;

		SetBoneAngleNoScale("Bip01 Spine", fdest_angle, 0.0F, 0.0F);
		SetBoneAngleNoScale("Bip01 Spine1", fdest_angle, 0.0F, 0.0F);
		SetBoneAngleNoScale("Bip01 Neck", fdest_angle * 2.0F, 0.0F, 0.0F);
	}

	// ֻ�ж��ص�0�ŷ���true, ����ȫ��false
	return false;
}

void Actor::SetAimObject(PERSISTID id)
{
	// ���Ϊ�գ�������
	if (id.IsNull())
	{
		m_AimObject = PERSISTID();
		return;
	}

	// �����Ϊ�գ����������ܷ�ת����
	if (!NodeIsExist("Bip01 Spine1"))
	{
		// ��Actor����ת�������Ƿ�����ң�����ҵĻ����Ӷ����ת��
		PERSISTID actor_role = GetLinkObject("actor_role");

		if (actor_role.IsNull())
		{
			m_AimObject = PERSISTID();
			return;
		}

		Actor* pActor = (Actor*)GetCore()->GetEntity(actor_role);

		if (pActor == NULL)
		{
			m_AimObject = PERSISTID();
			return;
		}

		// �Ӷ�����������Ӷ���
		pActor->SetAimObject(id);
		m_AimObject = id;
		return;
	}

	// ��Actor��ת����ֱ�����ò�����ת��
	m_AimObject = id;
	m_bAutoAimObjAngle = true;
}

PERSISTID Actor::GetAimObject() const
{
	return m_AimObject;
}
