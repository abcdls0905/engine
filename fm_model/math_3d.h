//--------------------------------------------------------------------
// 文件名:		math_3d.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年5月4日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _MATH_3D_H
#define _MATH_3D_H

#include "../public/macros.h"
#include "../math/fm_math.h"
#include "location.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

// 球体与三角形碰撞
bool math_sphere_triangle_collision(const FmVec3& center, float radius,
	const FmVec3& v1, const FmVec3& v2, const FmVec3& v3);
// 球体与三角形包围盒碰撞
bool math_sphere_triangle_aabb_collision(const FmVec3& center, 
	float radius, const FmVec3& v1, const FmVec3& v2, 
	const FmVec3& v3);
// 三角形与三角形碰撞
bool math_triangle_triangle_collision(const FmVec3& v1, const FmVec3& v2, const FmVec3& v3,
	const FmVec3& u1, const FmVec3& u2, const FmVec3& u3);

void math_rotate_bounding_box(const FmVec3* srcMin, 
	const FmVec3* srcMax, FmVec3* dstMin, FmVec3* dstMax, 
	const FmMat4* mtxRotateMatrix);
void math_create_texture_ani_matrix(FmMat4* pOut, 
	float fOffsetU, float fOffsetV, float fRotateU, float fRotateV, 
	float fRotateW, float fTilingU, float fTilingV);
int math_intersect_bb_and_sphere(const FmVec3* srcMin, 
	const FmVec3* srcMax, const FmVec3* center, float radius);
int math_intersect_bb_and_triangle(const FmVec3* srcMin, 
	const FmVec3* srcMax, const FmVec3* v0, const FmVec3* v1, const FmVec3* v2);

inline bool math_point_in_bb(const FmVec3* point, const FmVec3* min, 
	const FmVec3* max)
{
	Assert(point != NULL);
	Assert(min != NULL);
	Assert(max != NULL);

	if (point->x >= min->x && point->y >= min->y && point->z >= min->z &&
		point->x <= max->x && point->y <= max->y && point->z <= max->z)
	{
		return true;
	}

	return false;
}

inline int math_intersect_bb_and_bb(const FmVec3* srcMin, 
	const FmVec3* srcMax, const FmVec3* dstMin, 
	const FmVec3* dstMax)
{
	Assert(srcMin != NULL);
	Assert(srcMax != NULL);
	Assert(dstMin != NULL);
	Assert(dstMax != NULL);

	bool bMinIn = math_point_in_bb(dstMin, srcMin, srcMax);
	bool bMaxIn = math_point_in_bb(dstMax, srcMin, srcMax);

	if (bMinIn && bMaxIn)
	{
		return 1;
	}
	else if (!bMinIn && !bMaxIn)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

inline bool math_intersect_bb_and_planes(const FmVec3* bbMin, 
	const FmVec3* bbMax, const FmPlane* planes, unsigned int nPlaneCnt, 
	const FmVec3* center, float fRadius)
{
	// Is bounding box is in the bounding sphere.
	FmVec3 bbCenter = *bbMin + *bbMax;

	bbCenter *= 0.5F;

	FmVec3 delta = bbCenter - *center;
	float fDistanceSq = FmVec3LengthSq(&delta);
	FmVec3 bbDelta = ((*bbMax) - (*bbMin)) * 0.5F;
	float bbBigRadius = FmVec3Length(&bbDelta);

	if (fDistanceSq < (fRadius + bbBigRadius) * (fRadius + bbBigRadius))
	{
		return true;
	}

	if (0 == nPlaneCnt)
	{
		return false;
	}

	// Is bounding box's vertex in bounding mesh?
	FmVec3 boxs[8] = 
	{
		FmVec3(bbMin->x, bbMin->y, bbMin->z),
		FmVec3(bbMax->x, bbMin->y, bbMin->z),
		FmVec3(bbMin->x, bbMax->y, bbMin->z),
		FmVec3(bbMin->x, bbMin->y, bbMax->z),
		FmVec3(bbMax->x, bbMax->y, bbMax->z),
		FmVec3(bbMin->x, bbMax->y, bbMax->z),
		FmVec3(bbMax->x, bbMin->y, bbMax->z),
		FmVec3(bbMax->x, bbMax->y, bbMin->z),
	};

	for (unsigned int v = 0; v < 8; v++)
	{
		bool bIn = true;

		for (unsigned int p = 0; p < nPlaneCnt; p++)
		{
			float fDistance = FmPlaneDotCoord(planes + p, &boxs[v]);

			if (fDistance > 0.0F)
			{
				bIn = false;
				break;
			}
		}

		if (bIn)
		{
			return true;
		}
	} 

	return false;
}

inline void math_get_bounding_box_center(const FmVec3* min, 
	const FmVec3* max, FmVec3* center)
{
	Assert(min != NULL);
	Assert(max != NULL);
	Assert(center != NULL);

	center->x = (min->x + max->x) * 0.5F;
	center->y = (min->y + max->y) * 0.5F;
	center->z = (min->z + max->z) * 0.5F;
}

inline float math_get_small_value(float v0, float v1, float v2)
{
	v0 = min(v0, v1);

	return min(v0, v2);
}

inline float math_get_big_value(float v0, float v1, float v2)
{
	v0 = max(v0, v1);

	return max(v0, v2);
}

inline float math_round_f(float val)
{
	if (val >= 0.0F)
	{
		if (fabs(val - (int)val) > 0.5F)
		{
			return ceil(val);
		}
	}
	else
	{
		if (fabs(val - (int)val) > 0.5F)
		{
			return floor(val);
		}
	}

	return (float)(int)val;
}

inline float math_get_remain(float src, float unit)
{
	if (src <= 0.0F || unit <= 0.0F)
	{
		return 0.0F;
	}

	return src - ((int)(src / unit)) * unit;
}

inline void math_get_frame_lerp(unsigned int nFrameCnt, float fFPS, 
	float fTime, bool bLoop, unsigned int* nFrame, float* fFactor)
{
	if (nFrameCnt == 0)
	{
		*nFrame = 0;
		*fFactor = 0.0F;
		return;
	}

	float fCurFrame = fTime * fFPS;

	if (!bLoop)
	{
		if (fCurFrame >= (float)nFrameCnt)
		{
			// 定格在最后一帧
			*nFrame = nFrameCnt - 1;
			*fFactor = 0.9999F;
			return;
		}
	}

	*nFrame = (unsigned int)(math_get_remain(fCurFrame, (float)nFrameCnt));
	*fFactor = fCurFrame - (int)fCurFrame;
	//if (*nFrame >= nFrameCnt && bLoop)
	//{
	//	*nFrame = nFrameCnt - 1;
	//	*fFactor = 0.0F;
	//}
	//else if (*nFrame >= nFrameCnt && !bLoop)
	//{
	//	*nFrame = nFrameCnt - 1;
	//	*fFactor = 0.0F;
	//}
}

inline unsigned int math_get_next_frame(unsigned int nFrameCnt, 
	unsigned int nFrame, bool bLoop)
{
	if (nFrame < nFrameCnt - 1)
	{
		return nFrame + 1;
	}
	else
	{
		if (bLoop)
		{
			return 0;
		}
		else
		{
			return nFrameCnt - 1;
		}
	}
}

// 四元数球面插值
inline void math_quaternion_slerp(FmQuat* res, 
	const FmQuat* from, const FmQuat* to, float t)
{
	FmQuatSlerp(res,from,to,t);
	/*
	
	float tol[4];
	float omega, sinom, scale0, scale1;
	float cosom = FmQuatDot(from, to);

	if (cosom < 0.0F)
	{
		cosom = -cosom;
		tol[0] = -to->x;
		tol[1] = -to->y;
		tol[2] = -to->z;
		tol[3] = -to->w;
	}
	else
	{
		tol[0] = to->x;
		tol[1] = to->y;
		tol[2] = to->z;
		tol[3] = to->w;
	}

	if ((1.0F - cosom) > 0.0001F)
	{
		//标准情形 (slerp)
		omega = acosf(cosom);
		sinom = sinf(omega);
		scale0 = sinf((1.0F - t) * omega) / sinom;
		scale1 = sinf(t * omega) / sinom;
	}
	else
	{
		//from 与to四元数非常接近
		//因此可以用线性插值
		scale0 = 1.0F - t;
		scale1 = t;
	}

	// 计算最终值
	res->x = scale0 * from->x + scale1 * tol[0];
	res->y = scale0 * from->y + scale1 * tol[1];
	res->z = scale0 * from->z + scale1 * tol[2];
	res->w = scale0 * from->w + scale1 * tol[3];*/
	
}

//#include <d3dx.h>
inline void math_location_lerp(location_t* final, const location_t* start, 
	const location_t* end, float fFactor)
{
	/*
	D3DXVec3Lerp((D3DXVECTOR3*)&final->position, (D3DXVECTOR3*)&start->position, (D3DXVECTOR3*)&end->position, fFactor);
	D3DXVec3Lerp((D3DXVECTOR3*)&final->scale, (D3DXVECTOR3*)&start->scale, (D3DXVECTOR3*)&end->scale, fFactor);
	D3DXQuaternionSlerp((D3DXQUATERNION *)&final->rotate, (D3DXQUATERNION *)&start->rotate, (D3DXQUATERNION *)&end->rotate, fFactor);
*/

	
		
	FmVec3Lerp(&final->position, &start->position, &end->position, fFactor);
	FmVec3Lerp(&final->scale, &start->scale, &end->scale, fFactor);
	FmQuatSlerp(&final->rotate, &start->rotate, &end->rotate, 
		fFactor);
}

inline float math_saturate(float fValue)
{
	if (fValue < 0.0F)
	{
		fValue = 0.0F;
	}
	else if(fValue > 1.0F)
	{
		fValue = 1.0F;
	}

	return fValue;
}

inline void math_quaternion_to_pitch_yaw_roll(const FmQuat* q, 
	float* fPitch, float* fYaw, float* fRoll)
{
	float sp = -2.0F * (q->y * q->z - q->w * q->x);

	if ((sp > 0.9999f) || (sp < -0.9999f))
	{
		*fPitch = 3.1415926F * 0.5F * sp;
		*fYaw = atan2(-q->x * q->z + q->w * q->y, 
			0.5F - q->y * q->y - q->z * q->z);
		*fRoll = 0.0F;
	}
	else
	{
		*fPitch	= asin(sp);
		*fYaw = atan2(q->x * q->z + q->w * q->y, 
			0.5F - q->x * q->x - q->y * q->y);
		*fRoll = atan2(q->x * q->y + q->w * q->z, 
			0.5F - q->x * q->x - q->z * q->z);
	}
}

inline void math_compress_location_cr(const location_t* src, 
	location_cr_t* dst)
{
	static const float PI2 = 2.0F * 3.1415926F;

	float pitch, yaw, roll;

	math_quaternion_to_pitch_yaw_roll(&src->rotate, &pitch, &yaw, &roll);
	pitch = math_get_remain(pitch, PI2);
	yaw = math_get_remain(yaw, PI2);
	roll = math_get_remain(roll, PI2);

	if (pitch < 0.0F)
	{
		pitch += PI2;
	}

	if (yaw < 0.0F)
	{
		yaw += PI2;
	}

	if (roll < 0.0F)
	{
		roll += PI2;
	}

	dst->pitch = (unsigned short)(pitch * 65535.0F / PI2);
	dst->yaw = (unsigned short)(yaw * 65535.0F / PI2);
	dst->roll = (unsigned short)(roll * 65535.0F / PI2);
}

inline void math_uncompress_location_cr(const location_cr_t* src, 
	location_t* dst)
{
	static const float PI2 = 2.0F * 3.1415926F;

	float pitch, yaw, roll;

	pitch = (float)src->pitch * PI2 / 65535.0F;
	yaw = (float)src->yaw * PI2 / 65535.0F;
	roll = (float)src->roll * PI2 / 65535.0F;
	dst->position.x = 0.0F;
	dst->position.y = 0.0F;
	dst->position.z = 0.0F;
	dst->scale.x = 0.0F;
	dst->scale.y = 0.0F;
	dst->scale.z = 0.0F;
	FmQuatRotationYawPitchRoll(&dst->rotate, yaw, pitch, roll);
}

inline void math_compressLocation_ctr(const location_t* src, 
	location_ctr_t* dst)
{
	static const float PI2 = 2.0F * 3.1415926F;

	float pitch, yaw, roll;

	math_quaternion_to_pitch_yaw_roll(&src->rotate, &pitch, &yaw, &roll);
	pitch = math_get_remain(pitch, PI2);
	yaw = math_get_remain(yaw, PI2);
	roll = math_get_remain(roll, PI2);

	if (pitch < 0.0F)
	{
		pitch += PI2;
	}

	if (yaw < 0.0F)
	{
		yaw += PI2;
	}

	if (roll < 0.0F)
	{
		roll += PI2;
	}

	dst->px = src->position.x;
	dst->py = src->position.y;
	dst->pz = src->position.z;
	dst->pitch = (unsigned short)(pitch * 65535.0F / PI2);
	dst->yaw = (unsigned short)(yaw * 65535.0F / PI2);
	dst->roll = (unsigned short)(roll * 65535.0F / PI2);
}

inline void math_compress_location_csr(const location_t* src, 
	location_csr_t* dst)
{
	static const float PI2 = 2.0F * 3.1415926F;

	float pitch, yaw, roll;

	math_quaternion_to_pitch_yaw_roll(&src->rotate, &pitch, &yaw, &roll);
	pitch = math_get_remain(pitch, PI2);
	yaw = math_get_remain(yaw, PI2);
	roll = math_get_remain(roll, PI2);

	if (pitch < 0.0F)
	{
		pitch += PI2;
	}

	if (yaw < 0.0F)
	{
		yaw += PI2;
	}

	if (roll < 0.0F)
	{
		roll += PI2;
	}

	dst->sx = src->scale.x;
	dst->sy = src->scale.y;
	dst->sz = src->scale.z;
	dst->pitch = (unsigned short)(pitch * 65535.0F / PI2);
	dst->yaw = (unsigned short)(yaw * 65535.0F / PI2);
	dst->roll = (unsigned short)(roll * 65535.0F / PI2);
}

inline void math_uncompress_location_ctr(const location_ctr_t* src, 
	location_t* dst)
{
	static const float PI2 = 2.0F * 3.1415926F;

	float pitch, yaw, roll;

	pitch = (float)src->pitch * PI2 / 65535.0F;
	yaw = (float)src->yaw * PI2 / 65535.0F;
	roll = (float)src->roll * PI2 / 65535.0F;
	dst->position.x = src->px;
	dst->position.y = src->py;
	dst->position.z = src->pz;
	dst->scale.x = 0.0F;
	dst->scale.y = 0.0F;
	dst->scale.z = 0.0F;
	FmQuatRotationYawPitchRoll(&dst->rotate, yaw, pitch, roll);
}

inline void math_uncompress_location_csr(const location_csr_t* src, 
	location_t* dst)
{
	static const float PI2 = 2.0F * 3.1415926F;

	float pitch, yaw, roll;

	pitch = (float)src->pitch * PI2 / 65535.0F;
	yaw = (float)src->yaw * PI2 / 65535.0F;
	roll = (float)src->roll * PI2 / 65535.0F;
	dst->scale.x = src->sx;
	dst->scale.y = src->sy;
	dst->scale.z = src->sz;
	dst->position.x = 0.0F;
	dst->position.y = 0.0F;
	dst->position.z = 0.0F;
	FmQuatRotationYawPitchRoll(&dst->rotate, yaw, pitch, roll);
}

inline void math_location_to_matrix(const location_t* location, 
	FmMat4* pOut)
{
	float rx = location->rotate.x;
	float ry = location->rotate.y;
	float rz = location->rotate.z;
	float rw = location->rotate.w;
	float sx = location->scale.x;
	float sy = location->scale.y;
	float sz = location->scale.z;

	pOut->_11 = sx * (1.0F - 2.0F * (ry * ry + rz * rz));
	pOut->_12 = sx * 2.0F * (rx * ry + rz * rw);
	pOut->_13 = sx * 2.0F * (rx * rz - ry * rw);
	pOut->_14 = 0.0F;
	pOut->_21 = sy * 2.0F * (rx * ry - rz * rw);
	pOut->_22 = sy * (1.0F - 2.0F * (rx * rx + rz * rz));
	pOut->_23 = sy * 2.0F * (ry * rz + rx * rw);
	pOut->_24 = 0.0F;
	pOut->_31 = sz * 2.0F * (rx * rz + ry * rw);
	pOut->_32 = sz * 2.0F * (ry * rz - rx * rw);
	pOut->_33 = sz * (1.0F - 2.0F * (rx * rx + ry * ry));
	pOut->_34 = 0.0F;
	pOut->_41 = location->position.x;
	pOut->_42 = location->position.y;
	pOut->_43 = location->position.z;
	pOut->_44 = 1.0F;
	//D3DXMatrixTransformation(pOut, NULL, NULL, &location->scale, 
	//	NULL, &location->rotate, &location->position);
}

void CreateTextureAniMatrix(FmMat4* pOut, 
	float fOffsetU, float fOffsetV, float fRotateU, float fRotateV, 
	float fRotateW, float fTilingU, float fTilingV);
#endif // _MATH_3D_H
