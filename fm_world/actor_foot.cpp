//--------------------------------------------------------------------
// 文件名:		actor_foot.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2011年12月3日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "actor.h"
#include "../visual/i_action_player.h"
#include "../visual/i_context.h"
#include "../visual/i_terrain.h"
#include "../visual/vis_utils.h"
#include "../public/var_list.h"
#include "../public/inlines.h"
#include "../public/core_log.h"

static bool init_rotation_180(FmMat4* pOut)
{
	FmMatrixRotationY(pOut, 3.1415926f);
	return true;
}

static FmMat4 s_mtxRotation180;
static bool s_bInitRotation180 = init_rotation_180(&s_mtxRotation180);

// foot anchor

void Actor::SetFootAnchor(bool value) 
{ 
	m_bFootAnchor = value; 
	
	if (m_bFootAnchor)
	{
		m_fFootAnchorFactor = 0.0F;
	}
	else
	{
		ClearFootAnchor();
	}
}

bool Actor::GetFootAnchor() const 
{ 
	return m_bFootAnchor; 
}

void Actor::SetFootFloorID(const PERSISTID& value)
{
	IEntity* pEntity = GetCore()->GetEntity(value);

	if (NULL == pEntity)
	{
		return;
	}

	if (!pEntity->GetEntInfo()->IsKindOf("Terrain"))
	{
		return;
	}

	m_FootFloorID = value;
}

PERSISTID Actor::GetFootFloorID() const
{
	return m_FootFloorID;
}

// 设置矩阵的旋转和位移
inline void set_matrix_rotate_position(FmMat4* pOut, 
	const FmQuat* rotate, const FmVec3* position)
{
	float rx = rotate->x;
	float ry = rotate->y;
	float rz = rotate->z;
	float rw = rotate->w;

	pOut->_11 = 1.0F - 2.0F * (ry * ry + rz * rz);
	pOut->_12 = 2.0F * (rx * ry + rz * rw);
	pOut->_13 = 2.0F * (rx * rz - ry * rw);
	pOut->_14 = 0.0F;
	pOut->_21 = 2.0F * (rx * ry - rz * rw);
	pOut->_22 = 1.0F - 2.0F * (rx * rx + rz * rz);
	pOut->_23 = 2.0F * (ry * rz + rx * rw);
	pOut->_24 = 0.0F;
	pOut->_31 = 2.0F * (rx * rz + ry * rw);
	pOut->_32 = 2.0F * (ry * rz - rx * rw);
	pOut->_33 = 1.0F - 2.0F * (rx * rx + ry * ry);
	pOut->_34 = 0.0F;
	pOut->_41 = position->x;
	pOut->_42 = position->y;
	pOut->_43 = position->z;
	pOut->_44 = 1.0F;
}

// 四元数转成角度
inline void rotate_to_angle(FmVec3* angle, const FmQuat* rotate)
{
	VisUtil_QuaternionToPitchYawRoll(rotate, &angle->x, &angle->y, &angle->z);
}

// 获得碰撞位置的倾斜角度
static float get_slope_angle(const trace_info_t& ti, const FmMat4& mtxWorld)
{
	const trace_info_t::hit_t* pHit = &ti.Hits[0];
	// 点击到地面对象
	IVisBase* pVisBase = (IVisBase*)g_pCore->GetEntity(pHit->TraceID);

	if (NULL == pVisBase)
	{
		return 0.0F;
	}

	FmMat4 mtxModel;
	FmVec3 v0;
	FmVec3 v1;
	FmVec3 v2;

	pVisBase->GetWorldMatrix(mtxModel);
	
	FmVec3TransformCoord(&v0, &pHit->Vertex[0], &mtxModel);
	FmVec3TransformCoord(&v1, &pHit->Vertex[1], &mtxModel);
	FmVec3TransformCoord(&v2, &pHit->Vertex[2], &mtxModel);

	FmVec3 p1 = v1 - v0;
	FmVec3 p2 = v2 - v0;
	FmVec3 nor;

	FmVec3Cross(&nor, &p1, &p2);
	FmVec3Normalize(&nor, &nor);
	
	FmVec3 front(0.0F, 0.0F, 1.0F);

	FmVec3TransformNormal(&front, &front, &mtxWorld);
	FmVec3Normalize(&front, &front);

	float dot = FmVec3Dot(&nor, &front);
	float angle = FLOAT_PI / 2 - acosf(dot);

	if (angle > (FLOAT_PI / 4))
	{
		angle = FLOAT_PI / 4;
	}
	else if (angle < (-FLOAT_PI / 4))
	{
		angle = -FLOAT_PI / 4;
	}

	return -angle;
}

// 计算两条骨骼的IK角度
static bool calc_two_bone_ik(
	float* angle1, // Angle of bone 1
	float* angle2, // Angle of bone 2
	bool solvePosAngle2, // Solve for positive angle 2 instead of negative angle 2
	float length1, // Length of bone 1. Assumed to be >= zero
	float length2, // Length of bone 2. Assumed to be >= zero
	float targetX, // Target x position for the bones to reach
	float targetY) // Target y position for the bones to reach
{
	Assert(length1 >= 0);
	Assert(length2 >= 0);

	const float epsilon = 0.0001F; // used to prevent division by small numbers
	bool foundValidSolution = true;
	float targetDistSqr = (targetX * targetX + targetY * targetY);

	// Compute a new value for angle2 along with its cosine
	float sinAngle2;
	float cosAngle2;
	float cosAngle2_denom = 2 * length1 * length2;

	if (cosAngle2_denom > epsilon)
	{
		cosAngle2 = (targetDistSqr - length1 * length1 - length2 * length2)
			/ (cosAngle2_denom);

		// if our result is not in the legal cosine range, we can not find a
		// legal solution for the target
		if ((cosAngle2 < -1.0F) || (cosAngle2 > 1.0F))
		{
			foundValidSolution = false;
		}

		// clamp our value into range so we can calculate the best
		// solution when there are no valid ones
		cosAngle2 = FM_MAX(-1, FM_MIN(1, cosAngle2));
		// compute a new value for angle2
		*angle2 = acosf(cosAngle2);

		// adjust for the desired bend direction
		if (!solvePosAngle2)
		{
			*angle2 = -*angle2;
		}

		// compute the sine of our angle
		sinAngle2 = sinf(*angle2);
	}
	else
	{
		// At leaset one of the bones had a zero length. This means our
		// solvable domain is a circle around the origin with a radius
		// equal to the sum of our bone lengths.
		float totalLenSqr = (length1 + length2) * (length1 + length2);

		if (targetDistSqr < (totalLenSqr - epsilon)
			|| targetDistSqr > (totalLenSqr + epsilon))
		{
			foundValidSolution = false;
		}

		// Only the value of angle1 matters at this point. We can just
		// set angle2 to zero. 
		*angle2 = 0.0F;
		cosAngle2 = 1.0F;
		sinAngle2 = 0.0F;
	}

	// Compute the value of angle1 based on the sine and cosine of angle2
	float triAdjacent = length1 + length2 * cosAngle2;
	float triOpposite = length2 * sinAngle2;
	float tanY = targetY * triAdjacent - targetX * triOpposite;
	float tanX = targetX * triAdjacent + targetY * triOpposite;

	// Note that it is safe to call Atan2(0,0) which will happen if targetX and
	// targetY are zero
	*angle1 = atan2f(tanY, tanX);

	return foundValidSolution;
}

void Actor::ClearFootAnchor()
{
	if (NULL == m_pActionPlayer)
	{
		return;
	}
	
	m_pActionPlayer->SetBonePositionAppend("Bip01", 
		FmVec3(0.0F, 0.0F, 0.0F));
	m_pActionPlayer->SetBoneAngleAppend("Bip01 L Thigh", 
		FmVec3(0.0F, 0.0F, 0.0F));
	m_pActionPlayer->SetBoneAngleAppend("Bip01 L Calf", 
		FmVec3(0.0F, 0.0F, 0.0F));
	m_pActionPlayer->SetBoneAngleAppend("Bip01 L Foot", 
		FmVec3(0.0F, 0.0F, 0.0F));
	m_pActionPlayer->SetBoneAngleAppend("Bip01 R Thigh", 
		FmVec3(0.0F, 0.0F, 0.0F));
	m_pActionPlayer->SetBoneAngleAppend("Bip01 R Calf", 
		FmVec3(0.0F, 0.0F, 0.0F));
	m_pActionPlayer->SetBoneAngleAppend("Bip01 R Foot", 
		FmVec3(0.0F, 0.0F, 0.0F));
}

void Actor::UpdateFootAnchor(float seconds)
{
	if (NULL == m_pActionPlayer)
	{
		return;
	}

	ITerrain* pFloor = (ITerrain*)GetCore()->GetEntity(m_FootFloorID);

	if (NULL == pFloor)
	{
		return;
	}

	if (m_fFootAnchorFactor < 1.0F)
	{
		m_fFootAnchorFactor += seconds * 2;

		if (m_fFootAnchorFactor > 1.0F)
		{
			m_fFootAnchorFactor = 1.0F;
		}
	}

	int root_index = m_pActionPlayer->GetNodeIDFromName("Bip01");
	int pelvis_index = m_pActionPlayer->GetNodeIDFromName("Bip01 Pelvis");
	int lthigh_index = m_pActionPlayer->GetNodeIDFromName("Bip01 L Thigh");
	int lcalf_index = m_pActionPlayer->GetNodeIDFromName("Bip01 L Calf");
	int lankle_index = m_pActionPlayer->GetNodeIDFromName("Bip01 L Foot");
	int lfoot_index = m_pActionPlayer->GetNodeIDFromName("Tpose_footL_01");
	int rthigh_index = m_pActionPlayer->GetNodeIDFromName("Bip01 R Thigh");
	int rcalf_index = m_pActionPlayer->GetNodeIDFromName("Bip01 R Calf");
	int rankle_index = m_pActionPlayer->GetNodeIDFromName("Bip01 R Foot");
	int rfoot_index = m_pActionPlayer->GetNodeIDFromName("Tpose_footR_01");

	if ((root_index < 0) || (pelvis_index < 0) || (lthigh_index < 0)
		|| (lcalf_index < 0) || (lankle_index < 0) || (lfoot_index < 0)
		|| (rthigh_index < 0) || (rcalf_index < 0) || (rankle_index < 0)
		|| (rfoot_index < 0))
	{
		return;
	}
	
	// 未经过修正骨骼位置和旋转
	FmVec3 root_pos;
	FmVec3 pelvis_pos;
	FmVec3 lthigh_pos;
	FmVec3 lcalf_pos;
	FmVec3 lankle_pos;
	FmVec3 lfoot_pos;
	FmVec3 rthigh_pos;
	FmVec3 rcalf_pos;
	FmVec3 rankle_pos;
	FmVec3 rfoot_pos;
	FmQuat root_rotate;
	FmQuat pelvis_rotate;
	FmQuat lthigh_rotate;
	FmQuat lcalf_rotate;
	FmQuat lankle_rotate;
	FmQuat lfoot_rotate;
	FmQuat rthigh_rotate;
	FmQuat rcalf_rotate;
	FmQuat rankle_rotate;
	FmQuat rfoot_rotate;

	m_pActionPlayer->GetNodePositionFromID(root_index, &root_pos);
	m_pActionPlayer->GetNodePositionFromID(pelvis_index, &pelvis_pos);
	m_pActionPlayer->GetNodePositionFromID(lthigh_index, &lthigh_pos);
	m_pActionPlayer->GetNodePositionFromID(lcalf_index, &lcalf_pos);
	m_pActionPlayer->GetNodePositionFromID(lankle_index, &lankle_pos);
	m_pActionPlayer->GetNodePositionFromID(lfoot_index, &lfoot_pos);
	m_pActionPlayer->GetNodePositionFromID(rthigh_index, &rthigh_pos);
	m_pActionPlayer->GetNodePositionFromID(rcalf_index, &rcalf_pos);
	m_pActionPlayer->GetNodePositionFromID(rankle_index, &rankle_pos);
	m_pActionPlayer->GetNodePositionFromID(rfoot_index, &rfoot_pos);
	m_pActionPlayer->GetNodeRotateFromID(root_index, &root_rotate);
	m_pActionPlayer->GetNodeRotateFromID(pelvis_index, &pelvis_rotate);
	m_pActionPlayer->GetNodeRotateFromID(lthigh_index, &lthigh_rotate);
	m_pActionPlayer->GetNodeRotateFromID(lcalf_index, &lcalf_rotate);
	m_pActionPlayer->GetNodeRotateFromID(lankle_index, &lankle_rotate);
	m_pActionPlayer->GetNodeRotateFromID(lfoot_index, &lfoot_rotate);
	m_pActionPlayer->GetNodeRotateFromID(rthigh_index, &rthigh_rotate);
	m_pActionPlayer->GetNodeRotateFromID(rcalf_index, &rcalf_rotate);
	m_pActionPlayer->GetNodeRotateFromID(rankle_index, &rankle_rotate);
	m_pActionPlayer->GetNodeRotateFromID(rfoot_index, &rfoot_rotate);

	//FmVec3 root_angle;
	//FmVec3 pelvis_angle;
	//FmVec3 lthigh_angle;
	//FmVec3 lcalf_angle;
	//FmVec3 lankle_angle;
	//FmVec3 lfoot_angle;
	//FmVec3 rthigh_angle;
	//FmVec3 rcalf_angle;
	//FmVec3 rankle_angle;
	//FmVec3 rfoot_angle;

	//rotate_to_angle(&root_angle, &root_rotate);
	//rotate_to_angle(&pelvis_angle, &pelvis_rotate);
	//rotate_to_angle(&lthigh_angle, &lthigh_rotate);
	//rotate_to_angle(&lcalf_angle, &lcalf_rotate);
	//rotate_to_angle(&lankle_angle, &lankle_rotate);
	//rotate_to_angle(&lfoot_angle, &lfoot_rotate);
	//rotate_to_angle(&rthigh_angle, &rthigh_rotate);
	//rotate_to_angle(&rcalf_angle, &rcalf_rotate);
	//rotate_to_angle(&rankle_angle, &rankle_rotate);
	//rotate_to_angle(&rfoot_angle, &rfoot_rotate);

	FmMat4 mtxRoot;
	FmMat4 mtxPelvis;
	FmMat4 mtxLThigh;
	FmMat4 mtxLCalf;
	FmMat4 mtxLAnkle;
	FmMat4 mtxLFoot;
	FmMat4 mtxRThigh;
	FmMat4 mtxRCalf;
	FmMat4 mtxRAnkle;
	FmMat4 mtxRFoot;

	set_matrix_rotate_position(&mtxRoot, &root_rotate, &root_pos);
	set_matrix_rotate_position(&mtxPelvis, &pelvis_rotate, &pelvis_pos);
	set_matrix_rotate_position(&mtxLThigh, &lthigh_rotate, &lthigh_pos);
	set_matrix_rotate_position(&mtxLCalf, &lcalf_rotate, &lcalf_pos);
	set_matrix_rotate_position(&mtxLAnkle, &lankle_rotate, &lankle_pos);
	set_matrix_rotate_position(&mtxLFoot, &lfoot_rotate, &lfoot_pos);
	set_matrix_rotate_position(&mtxRThigh, &rthigh_rotate, &rthigh_pos);
	set_matrix_rotate_position(&mtxRCalf, &rcalf_rotate, &rcalf_pos);
	set_matrix_rotate_position(&mtxRAnkle, &rankle_rotate, &rankle_pos);
	set_matrix_rotate_position(&mtxRFoot, &rfoot_rotate, &rfoot_pos);

	FmMat4 mtxAbsPelvis;
	FmMat4 mtxAbsLThigh;
	FmMat4 mtxAbsLCalf;
	FmMat4 mtxAbsLAnkle;
	FmMat4 mtxAbsLFoot;
	FmMat4 mtxAbsRThigh;
	FmMat4 mtxAbsRCalf;
	FmMat4 mtxAbsRAnkle;
	FmMat4 mtxAbsRFoot;

	FmMatrixMultiply(&mtxAbsPelvis, &mtxPelvis, &mtxRoot);
	FmMatrixMultiply(&mtxAbsLThigh, &mtxLThigh, &mtxAbsPelvis);
	FmMatrixMultiply(&mtxAbsLCalf, &mtxLCalf, &mtxAbsLThigh);
	FmMatrixMultiply(&mtxAbsLAnkle, &mtxLAnkle, &mtxAbsLCalf);
	FmMatrixMultiply(&mtxAbsLFoot, &mtxLFoot, &mtxAbsLAnkle);
	FmMatrixMultiply(&mtxAbsRThigh, &mtxRThigh, &mtxAbsPelvis);
	FmMatrixMultiply(&mtxAbsRCalf, &mtxRCalf, &mtxAbsRThigh);
	FmMatrixMultiply(&mtxAbsRAnkle, &mtxRAnkle, &mtxAbsRCalf);
	FmMatrixMultiply(&mtxAbsRFoot, &mtxRFoot, &mtxAbsRAnkle);

	// 左右脚的绝对位置
	FmVec3 lfoot_abs_pos(mtxAbsLFoot._41, mtxAbsLFoot._42, 
		mtxAbsLFoot._43);
	FmVec3 rfoot_abs_pos(mtxAbsRFoot._41, mtxAbsRFoot._42, 
		mtxAbsRFoot._43);
	FmMat4 mtxWorld;

	FmMatrixMultiply(&mtxWorld, &s_mtxRotation180, &m_mtxWorld);
	FmVec3TransformCoord(&lfoot_abs_pos, &lfoot_abs_pos, &mtxWorld);
	FmVec3TransformCoord(&rfoot_abs_pos, &rfoot_abs_pos, &mtxWorld);

	// 足髁绝对位置
	FmVec3 lankle_abs_pos(mtxAbsLAnkle._41, mtxAbsLAnkle._42, 
		mtxAbsLAnkle._43);
	FmVec3 rankle_abs_pos(mtxAbsRAnkle._41, mtxAbsRAnkle._42, 
		mtxAbsRAnkle._43);

	FmVec3TransformCoord(&lankle_abs_pos, &lankle_abs_pos, &mtxWorld);
	FmVec3TransformCoord(&rankle_abs_pos, &rankle_abs_pos, &mtxWorld);

	// 膝盖绝对位置
	FmVec3 lcalf_abs_pos(mtxAbsLCalf._41, mtxAbsLCalf._42, 
		mtxAbsLCalf._43);
	FmVec3 rcalf_abs_pos(mtxAbsRCalf._41, mtxAbsRCalf._42, 
		mtxAbsRCalf._43);

	FmVec3TransformCoord(&lcalf_abs_pos, &lcalf_abs_pos, &mtxWorld);
	FmVec3TransformCoord(&rcalf_abs_pos, &rcalf_abs_pos, &mtxWorld);

	// 大腿起始绝对位置
	FmVec3 lthigh_abs_pos(mtxAbsLThigh._41, mtxAbsLThigh._42, 
		mtxAbsLThigh._43);
	FmVec3 rthigh_abs_pos(mtxAbsRThigh._41, mtxAbsRThigh._42, 
		mtxAbsRThigh._43);

	FmVec3TransformCoord(&lthigh_abs_pos, &lthigh_abs_pos, &mtxWorld);
	FmVec3TransformCoord(&rthigh_abs_pos, &rthigh_abs_pos, &mtxWorld);

	// 大腿和小腿的长度
	//float lthigh_len1 = FmVec3Length(&lcalf_pos);
	//float lcalf_len1 = FmVec3Length(&lankle_pos);
	//float rthigh_len1 = FmVec3Length(&rcalf_pos);
	//float rcalf_len1 = FmVec3Length(&rankle_pos);
	FmVec3 len = lthigh_abs_pos - lcalf_abs_pos;
	float lthigh_len = FmVec3Length(&len);
	len = lcalf_abs_pos - lankle_abs_pos;
	float lcalf_len = FmVec3Length(&len);
	len = rthigh_abs_pos - rcalf_abs_pos;
	float rthigh_len = FmVec3Length(&len);
	len = rcalf_abs_pos - rankle_abs_pos;
	float rcalf_len = FmVec3Length(&len);

	// 获得左右脚下地面的高度
	float trace_offset = (lthigh_len + lcalf_len) / 3.0F;
	FmVec3 src1(lfoot_abs_pos.x, lfoot_abs_pos.y + trace_offset, 
		lfoot_abs_pos.z);
	FmVec3 dst1(lfoot_abs_pos.x, lfoot_abs_pos.y - trace_offset, 
		lfoot_abs_pos.z);
	FmVec3 src2(rfoot_abs_pos.x, rfoot_abs_pos.y + trace_offset, 
		rfoot_abs_pos.z);
	FmVec3 dst2(rfoot_abs_pos.x, rfoot_abs_pos.y - trace_offset, 
		rfoot_abs_pos.z);
	trace_info_t tiLFoot;
	trace_info_t tiRFoot;
	float lheight;
	float rheight;
	
	unsigned int old_trace_mask = pFloor->GetTraceMaskValue();

	pFloor->SetTraceMaskValue(ITerrain::TRACEMASK_ROLE);
	
	if (pFloor->TraceDetail(src1, dst1, tiLFoot))
	{
		lheight = lfoot_abs_pos.y + trace_offset 
			- tiLFoot.fDistance * trace_offset * 2.0F;
	}
	else
	{
		lheight = lfoot_abs_pos.y;
	}

	if (pFloor->TraceDetail(src2, dst2, tiRFoot))
	{
		rheight = rfoot_abs_pos.y + trace_offset 
			- tiRFoot.fDistance * trace_offset * 2.0F;
	}
	else
	{
		rheight = rfoot_abs_pos.y;
	}

	pFloor->SetTraceMaskValue(old_trace_mask);

	// 左右脚需要调节的高度
	float ldelta = 0.0F;
	float rdelta = 0.0F;

	if (lheight > rheight)
	{
		// 需要调整左脚
		ldelta = lheight - rheight;
		
		// 根骨骼需要调节的高度
		float root_y = rheight - rfoot_abs_pos.y;
		
		m_pActionPlayer->SetBonePositionAppend("Bip01", 
			FmVec3(0.0F, root_y * m_fFootAnchorFactor, 0.0F));
	}
	else
	{
		// 需要调整右脚
		rdelta = rheight - lheight;
		
		// 根骨骼需要调节的高度
		float root_y = lheight - lfoot_abs_pos.y;

		m_pActionPlayer->SetBonePositionAppend("Bip01", 
			FmVec3(0.0F, root_y * m_fFootAnchorFactor, 0.0F));
	}

	// 左腿需要修正的角度
	float lthigh_delta = 0.0F;
	float lcalf_delta = 0.0F;
	
	if (ldelta > 0.0F)
	{
		// 大腿节点到目标点的位置
		FmVec3 ltarget(lankle_abs_pos.x, lankle_abs_pos.y + ldelta, 
			lankle_abs_pos.z);
		FmVec3 len = lthigh_abs_pos - ltarget;
		float ltarget_dist = FmVec3Length(&len);
		float langle1;
		float langle2;
		bool lfound = calc_two_bone_ik(&langle1, &langle2, true, lthigh_len, 
			lcalf_len, ltarget_dist, 0.0F);
		len = lthigh_abs_pos - lankle_abs_pos;
		float ltarget1 = FmVec3Length(&len);
		float langle3;
		float langle4;
		bool lfound1 = calc_two_bone_ik(&langle3, &langle4, true, lthigh_len, 
			lcalf_len, ltarget1, 0.0F);

		if (lfound && lfound1)
		{
			lthigh_delta = langle1 - langle3;
			lcalf_delta = langle2 - langle4;
		}
	}

	m_pActionPlayer->SetBoneAngleAppend("Bip01 L Thigh", 
		FmVec3(0.0F, lthigh_delta * m_fFootAnchorFactor, 0.0F));
	m_pActionPlayer->SetBoneAngleAppend("Bip01 L Calf", 
		FmVec3(0.0F, lcalf_delta * m_fFootAnchorFactor, 0.0F));

	// 右腿需要修正的角度
	float rthigh_delta = 0.0F;
	float rcalf_delta = 0.0F;

	if (rdelta > 0.0F)
	{
		// 大腿节点到目标点的位置
		FmVec3 rtarget(rankle_abs_pos.x, rankle_abs_pos.y + rdelta, 
			rankle_abs_pos.z);
		FmVec3 len = rthigh_abs_pos - rtarget;
		float rtarget_dist = FmVec3Length(&len);
		float rangle1;
		float rangle2;
		bool rfound = calc_two_bone_ik(&rangle1, &rangle2, true, rthigh_len, 
			rcalf_len, rtarget_dist, 0.0F);
		len = rthigh_abs_pos - rankle_abs_pos;
		float rtarget1 = FmVec3Length(&len);
		float rangle3;
		float rangle4;
		bool rfound1 = calc_two_bone_ik(&rangle3, &rangle4, true, rthigh_len, 
			rcalf_len, rtarget1, 0.0F);

		if (rfound && rfound1)
		{
			rthigh_delta = rangle1 - rangle3;
			rcalf_delta = rangle2 - rangle4;
		}
	}
	
	m_pActionPlayer->SetBoneAngleAppend("Bip01 R Thigh", 
		FmVec3(0.0F, rthigh_delta * m_fFootAnchorFactor, 0.0F));
	m_pActionPlayer->SetBoneAngleAppend("Bip01 R Calf", 
		FmVec3(0.0F, rcalf_delta * m_fFootAnchorFactor, 0.0F));

	// 改变左右脚背的倾斜度
	float lfoot_delta = 0.0F;
	float rfoot_delta = 0.0F;

	if (tiLFoot.nHitCount > 0)
	{
		lfoot_delta = get_slope_angle(tiLFoot, mtxWorld) 
			- (lthigh_delta + lcalf_delta);
	}

	if (tiRFoot.nHitCount > 0)
	{
		rfoot_delta = get_slope_angle(tiRFoot, mtxWorld) 
			- (rthigh_delta + rcalf_delta);
	}

	m_pActionPlayer->SetBoneAngleAppend("Bip01 L Foot", 
		FmVec3(0.0F, lfoot_delta * m_fFootAnchorFactor, 0.0F));
	m_pActionPlayer->SetBoneAngleAppend("Bip01 R Foot", 
		FmVec3(0.0F, rfoot_delta * m_fFootAnchorFactor, 0.0F));
}

