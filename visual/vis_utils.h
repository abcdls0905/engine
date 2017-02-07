//--------------------------------------------------------------------
// 文件名:		vis_utils.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月28日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _VISUAL_VIS_UTILS_H
#define _VISUAL_VIS_UTILS_H

#include "../public/module.h"
#include "../public/fast_str.h"
#include "../public/var.h"
#include "../public/var_list.h"
#include "../public/i_var_table.h"
#include "../public/inlines.h"
#include "../public/core_log.h"

#include "fm_define.h"
#include "fm_header.h"

#define COLOR_ARGB(a, r, g, b) ((unsigned int)((((a) & 0xFF)<<24) \
	| (((r) & 0xFF) << 16) | (((g) & 0xFF) << 8) | ((b) & 0xFF)))

#define COLOR_RGBA(r, g, b, a) COLOR_ARGB(a, r, g, b)


// 浮点数转整数
inline int VisUtil_FloatToInt(float val)
{
	return (int)val;
}

// 整数转浮点数
inline float VisUtil_IntToFloat(int val)
{
	return (float)val;
}


// 设置着色器常量
inline void VisUtil_SetShaderConst(void* dst, size_t offset, const void* src, 
	size_t count)
{
	memcpy((unsigned char*)dst + offset * sizeof(FmVec4), src, 
		count * sizeof(FmVec4));
}

// 工具函数

// 颜色值转换成字符串
inline result_string VisUtil_IntToColor(unsigned int val)
{
	char buf[64];
	
	SafeSprintf(buf, sizeof(buf), "%d,%d,%d,%d", (val >> 24) & 0xFF, 
		(val >> 16) & 0xFF, (val >> 8) & 0xFF, (val >> 0) & 0xFF);
	
	return result_string(buf);
}

// 字符串转换成颜色值
inline unsigned int VisUtil_ColorToInt(const char* val)
{
	Assert(val != NULL);
	
	char buf[128];
	
	CopyString(buf, sizeof(buf), val);
	
	char* s1 = strchr(buf, ',');
	
	if (NULL == s1)
	{
		return 0xFFFFFFFF;
	}
	
	char* s2 = strchr(s1 + 1, ',');
		
	if (NULL == s2)
	{
		return 0xFFFFFFFF;
	}
	
	char* s3 = strchr(s2 + 1, ',');
		
	if (NULL == s3)
	{
		return 0xFFFFFFFF;
	}
	
	*s1 = 0;
	*s2 = 0;
	*s3 = 0;
	
	unsigned int res = 0;

	res |= (atoi(buf) & 0xFF) << 24;
	res |= (atoi(s1 + 1) & 0xFF) << 16;
	res |= (atoi(s2 + 1) & 0xFF) << 8;
	res |= (atoi(s3 + 1) & 0xFF);
	
	return res;
}

// 向量值转换成字符串
inline result_string VisUtil_VectorToString(const FmVec3& val)
{
	char buf[128];
	
	SafeSprintf(buf, sizeof(buf), "%f,%f,%f", double(val.x), double(val.y), 
		double(val.z));
	
	return result_string(buf);
}

// 字符串转换成向量值
inline FmVec3 VisUtil_StringToVector(const char* val)
{
	Assert(val != NULL);
	
	char buf[128];
	
	CopyString(buf, sizeof(buf), val);
	
	char* s1 = strchr(buf, ',');
	
	if (NULL == s1)
	{
		return FmVec3(0.0F, 0.0F, 0.0F);
	}
	
	char* s2 = strchr(s1 + 1, ',');

	if (NULL == s2)
	{
		return FmVec3(0.0F, 0.0F, 0.0F);
	}
	
	*s1 = 0;
	*s2 = 0;

	float x = (float)atof(buf);
	float y = (float)atof(s1 + 1);
	float z = (float)atof(s2 + 1);
	
	return FmVec3(x, y, z);
}

// 字符串转换成向量值
inline FmVec4 VisUtil_StringToVector4(const char* val)
{
    Assert(val != NULL);

    char buf[128];

    CopyString(buf, sizeof(buf), val);

    char* s1 = strchr(buf, ',');

    if (NULL == s1)
    {
        return FmVec4(0.0F, 0.0F, 0.0F, 0.0F);
    }

    char* s2 = strchr(s1 + 1, ',');

    if (NULL == s2)
    {
        return FmVec4(0.0F, 0.0F, 0.0F, 0.0F);
    }

    char* s3 = strchr(s2 + 1, ',');

    if (NULL == s3)
    {
        return FmVec4(0.0F, 0.0F, 0.0F, 0.0F);
    }

    *s1 = 0;
    *s2 = 0;
    *s3 = 0;

    float x = (float)atof(buf);
    float y = (float)atof(s1 + 1);
    float z = (float)atof(s2 + 1);
    float w = (float)atof(s3 + 1);

    return FmVec4(x, y, z, w);
}

// 颜色值转为向量
inline FmVec3 VisUtil_ColorToVector(unsigned int color)
{
	unsigned int r = (color & 0xFF0000) >> 16;
	unsigned int g = (color & 0xFF00) >> 8;
	unsigned int b = color & 0xFF;
	
	return FmVec3(float(r) / 255.0F, float(g) / 255.0F, 
		float(b) / 255.0F);
}

// 向量转为颜色值
inline unsigned int VisUtil_VectorToColor(const FmVec3& v)
{
	return 0xFF000000 + ((int(v.x * 255.0F) & 0xFF) << 16)
		+ ((int(v.y * 255.0F) & 0xFF) << 8) + (int(v.z * 255.0F) & 0xFF);
}

// 颜色值转为向量
inline FmVec4 VisUtil_ColorToVector4(unsigned int color)
{
	unsigned int a = (color & 0xFF000000) >> 24;
	unsigned int r = (color & 0xFF0000) >> 16;
	unsigned int g = (color & 0xFF00) >> 8;
	unsigned int b = color & 0xFF;

	return FmVec4(float(r) / 255.0F, float(g) / 255.0F, 
		float(b) / 255.0F, float(a) / 255.0F);
}

// 向量转为颜色值
inline unsigned int VisUtil_Vector4ToColor(const FmVec4& v)
{
	return ((int(v.w * 255.0F) & 0xFF) << 24) 
		+ ((int(v.x * 255.0F) & 0xFF) << 16)
		+ ((int(v.y * 255.0F) & 0xFF) << 8) 
		+ (int(v.z * 255.0F) & 0xFF);
}

// 字符串转为对象ID
inline PERSISTID VisUtil_StringToID(const char* val)
{
	Assert(val != NULL);

	char buf[128];

	CopyString(buf, sizeof(buf), val);

	char* s1 = strchr(buf, '-');

	if (NULL == s1)
	{
		return PERSISTID();
	}

	*s1 = 0;

	return PERSISTID(atoi(buf), atoi(s1 + 1));
}

// 对象ID转为字符串
inline result_string VisUtil_IDToString(const PERSISTID& id)
{
	char buf[128];

	SafeSprintf(buf, sizeof(buf), "%d-%d", id.nIdent, id.nSerial);

	return result_string(buf);
}

// 半精度浮点数转换成单精度浮点数
inline float VisUtil_HalfToFloat(unsigned short val)
{
	union 
	{ 
		float f; 
		unsigned int i; 
	} res;

	register int s = (val >> 15) & 0x00000001;
	register int e = (val >> 10) & 0x0000001F;
	register int m = val & 0x000003FF;

	if (e == 0)
	{
		if (m == 0) // +0.0或-0.0
		{
			res.i = s << 31;
		}
		else // -1.0到+1.0
		{
			while (!(m & 0x00000400))
			{
				m <<= 1;
				e -= 1;
			}
         
			m &= ~0x00000400;
			e = e + 1 + (127 - 15);
			res.i = (s << 31) | (e << 23) | (m << 13);
		}
	}
	else if (e == 31)
	{
		if (m == 0) // INF
		{
			res.i = (s << 31) | 0x7F800000;
		}
		else // NAN
		{
			res.i = (s << 31) | 0x7F800000 | (m << 13);
		}
	}
	else
	{
		e = e + (127 - 15);
		res.i = (s << 31) | (e << 23) | (m << 13);
	}

	return res.f;
}

// 单精度浮点数转换成半精度浮点数
inline unsigned short VisUtil_FloatToHalf(float val)
{
	union 
	{ 
		float f; 
		unsigned int i; 
	} v;

	v.f = val;

	register int s = (v.i >> 16) & 0x00008000;
	register int e = ((v.i >> 23) & 0x000000FF) - (127 - 15);
	register int m = v.i & 0x007FFFFF;

	if (e <= 0)
	{
		if (e < -10)
		{
			return 0;
		}
		
		m = (m | 0x00800000) >> (1 - e);
		return (unsigned short)(s | (m >> 13));
	}
	else if (e == 0xFF - (127 - 15))
	{
		if (m == 0) // INF
		{
			return (unsigned short)(s | 0x7C00);
		} 
		else // NAN
		{
			m >>= 13;
			return (unsigned short)(s | 0x7C00 | m | (m == 0));
		}
	}
	else
	{
		if (e > 30) // Overflow
		{
			return (unsigned short)(s | 0x7C00);
		}

		return (unsigned short)(s | (e << 10) | (m >> 13));
	}
}

// 获得自定义属性
inline bool VisUtil_GetCustomBool(IEntity* pEntity, const char* name)
{
	IVar* pVar = pEntity->GetCustoms()->GetValue(name);

	if (NULL == pVar)
	{
		return false;
	}

	return pVar->BoolVal();
}

inline int VisUtil_GetCustomInt(IEntity* pEntity, const char* name)
{
	IVar* pVar = pEntity->GetCustoms()->GetValue(name);

	if (NULL == pVar)
	{
		return 0;
	}

	return pVar->IntVal();
}

inline float VisUtil_GetCustomFloat(IEntity* pEntity, const char* name)
{
	IVar* pVar = pEntity->GetCustoms()->GetValue(name);

	if (NULL == pVar)
	{
		return 0.0F;
	}

	return pVar->FloatVal();
}

inline const char* VisUtil_GetCustomString(IEntity* pEntity, const char* name)
{
	IVar* pVar = pEntity->GetCustoms()->GetValue(name);

	if (NULL == pVar)
	{
		return "";
	}

	return pVar->StringVal();
}

// 设置自定义属性
inline void VisUtil_SetCustomBool(IEntity* pEntity, const char* name, 
	bool value)
{
	pEntity->GetCustoms()->Assign(name, CVar(VTYPE_BOOL, value));
}

inline void VisUtil_SetCustomInt(IEntity* pEntity, const char* name, 
	int value)
{
	pEntity->GetCustoms()->Assign(name, CVar(VTYPE_INT, value));
}

inline void VisUtil_SetCustomFloat(IEntity* pEntity, const char* name, 
	float value)
{
	pEntity->GetCustoms()->Assign(name, CVar(VTYPE_FLOAT, value));
}

inline void VisUtil_SetCustomString(IEntity* pEntity, const char* name, 
	const char* value)
{
	pEntity->GetCustoms()->Assign(name, CVar(VTYPE_STRING, value));
}

// 对包围盒进行碰撞检测
inline bool VisUtil_TraceBoundBox(const FmMat4& mat, 
	const FmVec3& center, const FmVec3& box_size, 
	const FmVec3& src, const FmVec3& dst, trace_info_t& ti)
{
	FmVec3 v_min = center - (box_size * 0.5F);
	FmVec3 v_max = v_min + box_size;
	FmVec3 v0(v_min.x, v_min.y, v_min.z);
	FmVec3 v1(v_max.x, v_min.y, v_min.z);
	FmVec3 v2(v_min.x, v_max.y, v_min.z);
	FmVec3 v3(v_max.x, v_max.y, v_min.z);
	FmVec3 v4(v_min.x, v_min.y, v_max.z);
	FmVec3 v5(v_max.x, v_min.y, v_max.z);
	FmVec3 v6(v_min.x, v_max.y, v_max.z);
	FmVec3 v7(v_max.x, v_max.y, v_max.z);

	FmVec3TransformCoord(&v0, &v0, &mat);
	FmVec3TransformCoord(&v1, &v1, &mat);
	FmVec3TransformCoord(&v2, &v2, &mat);
	FmVec3TransformCoord(&v3, &v3, &mat);
	FmVec3TransformCoord(&v4, &v4, &mat);
	FmVec3TransformCoord(&v5, &v5, &mat);
	FmVec3TransformCoord(&v6, &v6, &mat);
	FmVec3TransformCoord(&v7, &v7, &mat);
	
	FmVec3 vec[36];

	vec[0] = v0;
	vec[1] = v2;
	vec[2] = v3;
	vec[3] = v0;
	vec[4] = v3;
	vec[5] = v1;
	vec[6] = v4;
	vec[7] = v6;
	vec[8] = v7;
	vec[9] = v4;
	vec[10] = v7;
	vec[11] = v5;
	vec[12] = v2;
	vec[13] = v6;
	vec[14] = v7;
	vec[15] = v2;
	vec[16] = v7;
	vec[17] = v3;
	vec[18] = v0;
	vec[19] = v4;
	vec[20] = v5;
	vec[21] = v0;
	vec[22] = v5;
	vec[23] = v1;
	vec[24] = v4;
	vec[25] = v6;
	vec[26] = v2;
	vec[27] = v4;
	vec[28] = v2;
	vec[29] = v0;
	vec[30] = v5;
	vec[31] = v7;
	vec[32] = v3;
	vec[33] = v5;
	vec[34] = v3;
	vec[35] = v1;

	FmVec3 dir = dst - src;
	FmVec3* pv = vec;
	bool hitted = false;
	
	// 逐个三角形做碰撞检测
	for (size_t k = 0; k < 12; ++k)
	{
		float t;
		float u;
		float v;
		
		// 射线与三角形相交
		if (FmIntersectTri(&pv[0], &pv[1], &pv[2], &src, &dir, &u, &v, &t))
		{
			if ((t >= 0.0F) && (t <= 1.0F))
			{
				if (ti.nHitMax > 1)
				{
					if (ti.nHitCount >= ti.nHitMax)
					{
						break;
					}
					
					hitted = true;

					if (t < ti.fDistance)
					{
						// 距离最近的碰撞点
						ti.fDistance = t;
					}
					
					trace_info_t::hit_t* pHit = &ti.Hits[ti.nHitCount];
					
					pHit->fDistance = t;
					pHit->fTraceU = u;
					pHit->fTraceV = v;
					pHit->TraceID = PERSISTID();
					pHit->nMaterialIndex = 0;
					pHit->Vertex[0] = pv[0];
					pHit->Vertex[1] = pv[1];
					pHit->Vertex[2] = pv[2];
					pHit->strTexName = "";
					
					if (++ti.nHitCount >= ti.nHitMax)
					{
						break;
					}
				}
				else if (t < ti.fDistance)
				{
					hitted = true;

					// 距离最近的碰撞点
					ti.fDistance = t;
					
					trace_info_t::hit_t* pHit = &ti.Hits[0];

					pHit->fDistance = t;
					pHit->fTraceU = u;
					pHit->fTraceV = v;
					pHit->TraceID = PERSISTID();
					pHit->nMaterialIndex = 0;
					pHit->Vertex[0] = pv[0];
					pHit->Vertex[1] = pv[1];
					pHit->Vertex[2] = pv[2];
					pHit->strTexName = "";

					ti.nHitCount = 1;
				}
			}
		}
		
		pv += 3;
	}
	
	return hitted;
}

// 点是否包含在盒子内
inline bool VisUtil_BoxContainPoint(const FmVec3& v_min, 
	const FmVec3& v_max, const FmVec3& v_pos)
{
	return (v_pos.x >= v_min.x) && (v_pos.x <= v_max.x)
		&& (v_pos.z >= v_min.z) && (v_pos.z <= v_max.z)
		&& (v_pos.y >= v_min.y) && (v_pos.y <= v_max.y);
}

// 球体是否与盒子相交
inline bool VisUtil_BoxIntersectSphere(const FmVec3& v_min,
	const FmVec3& v_max, const FmVec3& center, float radius)
{
	return (center.x >= v_min.x - radius) && (center.x <= v_max.x + radius)
		&& (center.z >= v_min.z - radius) && (center.z <= v_max.z + radius)
		&& (center.y >= v_min.y - radius) && (center.y <= v_max.y + radius);
}

// 两个盒子是否相交
inline bool VisUtil_BoxIntersectBox(const FmVec3& v_min_1,	
	const FmVec3& v_max_1, const FmVec3& v_min_2,
	const FmVec3& v_max_2)
{
	return (v_max_1.x >= v_min_2.x) && (v_max_2.x >= v_min_1.x)
		&& (v_max_1.z >= v_min_2.z) && (v_max_2.z >= v_min_1.z)
		&& (v_max_1.y >= v_min_2.y) && (v_max_2.y >= v_min_1.y);
}

// 法线转成四元数
inline void VisUtil_QuaternionFromNormal(FmQuat* pOut, 
	const FmVec3* vNormal)
{
	FmVec3 up(0.0F, 1.0F, 0.0F);
	float dot = FmVec3Dot(vNormal, &up);
	float angle;

	if (dot >= 1.0F)
	{
		angle = 0.0F;
	}
	else if (dot <= -1.0F)
	{
		angle = 3.141592654F;
	}
	else
	{
		angle = acosf(dot);
	}

	FmVec3 axis;

	FmVec3Cross(&axis, vNormal, &up);
	FmQuatRotationAxis(pOut, &axis, -angle);
}

// 四元数转换成欧拉角
inline void VisUtil_QuaternionToPitchYawRoll(const FmQUATERNION* q, 
	float* pitch, float* yaw, float* roll)
{
	float sp = -2.0F * (q->y * q->z - q->w * q->x);

	if ((sp > 0.9999F) || (sp < -0.9999F))
	{
		*pitch = FLOAT_PI * 0.5F * sp;
		*yaw = atan2(-q->x * q->z + q->w * q->y, 
			0.5F - q->y * q->y - q->z * q->z);
		*roll = 0.0F;
	}
	else
	{
		*pitch = asin(sp);
		*yaw = atan2(q->x * q->z + q->w * q->y, 
			0.5F - q->x * q->x - q->y * q->y);
		*roll = atan2(q->x * q->y + q->w * q->z, 
			0.5F - q->x * q->x - q->z * q->z);
	}
}

// 四元数乘法
inline void VisUtil_QuaternionMultiply(FmQUATERNION* pout, 
   const FmQUATERNION* q1, const FmQUATERNION* q2)
{
	pout->x = q1->w * q2->x + q1->x * q2->w + q1->y * q2->z - q1->z * q2->y;
	pout->y = q1->w * q2->y + q1->y * q2->w + q1->z * q2->x - q1->x * q2->z;
	pout->z = q1->w * q2->z + q1->z * q2->w + q1->x * q2->y - q1->y * q2->x;
	pout->w = q1->w * q2->w - q1->x * q2->x - q1->y * q2->y - q1->z * q2->z;
}

// 使用四元数转换点位置
inline void VisUtil_Vec3QuaternionTransform(FmVec3* pout,
	const FmVec3* v, const FmQuat* q)
{
	FmQuat qn(v->x, v->y, v->z, 0.0F);
	FmQuat conjugate(-q->x, -q->y, -q->z, q->w);
	FmQuat temp;

	FmVec3Normalize((FmVec3*)&qn, (FmVec3*)&qn);
	FmQuatMultiply(&temp, &qn, &conjugate);
	FmQuatMultiply(&temp, q, &temp);
	pout->x = temp.x;
	pout->y = temp.y;
	pout->z = temp.z;
}


// 设置转换矩阵
inline void VisUtil_MatrixTransform(FmMat4* pOut, const FmVec3* scale, 
	const FmQUATERNION* rotate, const FmVec3* position)
{
	float rx = rotate->x;
	float ry = rotate->y;
	float rz = rotate->z;
	float rw = rotate->w;
	float sx = scale->x;
	float sy = scale->y;
	float sz = scale->z;

	pOut->f[0*4+0] = sx * (1.0F - 2.0F * (ry * ry + rz * rz));
	pOut->f[0*4+1]  = sx * 2.0F * (rx * ry + rz * rw);
	pOut->f[0*4+2] = sx * 2.0F * (rx * rz - ry * rw);
	pOut->f[0*4+3] = 0.0F;
	pOut->f[1*4+0] = sy * 2.0F * (rx * ry - rz * rw);
	pOut->f[1*4+1] = sy * (1.0F - 2.0F * (rx * rx + rz * rz));
	pOut->f[1*4+2] = sy * 2.0F * (ry * rz + rx * rw);
	pOut->f[1*4+3] = 0.0F;
	pOut->f[2*4+0] = sz * 2.0F * (rx * rz + ry * rw);
	pOut->f[2*4+1] = sz * 2.0F * (ry * rz - rx * rw);
	pOut->f[2*4+2] = sz * (1.0F - 2.0F * (rx * rx + ry * ry));
	pOut->f[2*4+3] = 0.0F;
	pOut->f[3*4+0] = position->x;
	pOut->f[3*4+1] = position->y;
	pOut->f[3*4+2] = position->z;
	pOut->f[3*4+3] = 1.0F;
}

// 矩阵旋转和位移
inline void VisUtil_MatrixRotationTranslation(FmMat4* mtx, 
	const FmVec3* angle, const FmVec3* position)
{
	FmMat4RotationYawPitchRoll(mtx, angle->y, angle->x, angle->z);

	mtx->f[3*4+0] = position->x;
	mtx->f[3*4+1] = position->y;
	mtx->f[3*4+2] = position->z;
}


inline FmQUATERNION* VisUtil_QuaternionRotationMatrix(
	FmQuat* pout, const FmMat4* pm)
{
	float trace = pm->m[0][0] + pm->m[1][1] + pm->m[2][2] + 1.0F;

	if (trace > 1.0F)
	{
        float fsqrt_trace = sqrtf(trace);
		float frecp_sqrt_trace = 1.0F / fsqrt_trace * 0.5F;
        pout->x = (pm->m[1][2] - pm->m[2][1]) * frecp_sqrt_trace;
		pout->y = (pm->m[2][0] - pm->m[0][2]) * frecp_sqrt_trace;
        pout->z = (pm->m[0][1] - pm->m[1][0]) * frecp_sqrt_trace;
		pout->w = fsqrt_trace * 0.5F;
		return pout;
	}

	int maxi = 0;
	float maxdiag = pm->m[0][0];

	if (pm->m[1][1] > maxdiag)
	{
		maxi = 1;
		maxdiag = pm->m[1][1];
	}

	if (pm->m[2][2] > maxdiag)
	{
		maxi = 2;
		maxdiag = pm->m[2][2];
	}

	float S;

	switch (maxi)
	{
	case 0:
		S = 2.0F * sqrtf(1.0F + pm->m[0][0] - pm->m[1][1] - pm->m[2][2]);
		pout->x = 0.25F * S;
		pout->y = (pm->m[0][1] + pm->m[1][0]) / S;
		pout->z = (pm->m[0][2] + pm->m[2][0]) / S;
		pout->w = (pm->m[1][2] - pm->m[2][1]) / S;
		break;
	case 1:
		S = 2.0F * sqrtf(1.0F + pm->m[1][1] - pm->m[0][0] - pm->m[2][2]);
		pout->x = (pm->m[0][1] + pm->m[1][0]) / S;
		pout->y = 0.25F * S;
		pout->z = (pm->m[1][2] + pm->m[2][1]) / S;
		pout->w = (pm->m[2][0] - pm->m[0][2]) / S;
		break;
	case 2:
		S = 2.0F * sqrtf(1.0F + pm->m[2][2] - pm->m[0][0] - pm->m[1][1]);
		pout->x = (pm->m[0][2] + pm->m[2][0]) / S;
		pout->y = (pm->m[1][2] + pm->m[2][1]) / S;
		pout->z = 0.25F * S;
		pout->w = (pm->m[0][1] - pm->m[1][0]) / S;
		break;
	}

	return pout;
}


// 内联三角形射线计算
inline bool VisUtil_IntersectTri(const FmVec3* v0, const FmVec3* v1, 
	const FmVec3* v2, const FmVec3* orig, const FmVec3* dir, float* u, 
	float* v, float* t)
{   
	// Find vectors for two edges sharing vert0  
	FmVec3 edge1 = *v1 - *v0;   
	FmVec3 edge2 = *v2 - *v0;    

	// Begin calculating determinant - also used to calculate U parameter   
	FmVec3 pvec;   

	FmVec3Cross(&pvec, dir, &edge2); 

	// If determinant is near zero, ray lies in plane of triangle  
	float det = FmVec3Dot(&edge1, &pvec);   
	FmVec3 tvec; 

	if (det > 0)   
	{    
		tvec = *orig - *v0;   
	}  
	else   
	{      
		tvec = *v0 - *orig;  
		det = -det; 
	}

	if (det < 0.0001F) 
	{
		return false; 
	}

	// Calculate U parameter and test bounds  
	*u = FmVec3Dot(&tvec, &pvec);  

	if ((*u < 0.0F) || (*u > det))
	{
		return false;  
	}

	// Prepare to test V parameter    
	FmVec3 qvec; 

	FmVec3Cross(&qvec, &tvec, &edge1); 

	// Calculate V parameter and test bounds  
	*v = FmVec3Dot(dir, &qvec); 

	if ((*v < 0.0F) || ((*u + *v) > det))  
	{
		return false;   
	}

	// Calculate t, scale parameters, ray intersects triangle  
	*t = FmVec3Dot(&edge2, &qvec);  

	float fInvDet = 1.0F / det;  

	*t *= fInvDet;  
	*u *= fInvDet;   
	*v *= fInvDet;   

	return true;
}

// 矩阵乘法后转置
inline void VisUtil_MatrixMultiplyTranspose(FmMat4* pOut, 
	const FmMat4* pIn1, const FmMat4* pIn2)
{
	//VisUtil_MatrixMultiply(pOut, pIn1, pIn2);
	FmMat4Multiply(pOut,pIn1,pIn2);
	//VisUtil_MatrixTranspose(pOut, pOut);
	FmMat4Transpose(pOut,pOut);
}

// 设置无缩放矩阵的角度
inline void VisUtil_SetMatrixAngleNoScale(FmMat4* pOut, float ax, 
	float ay, float az)
{
	const float sin_ax = sinf(ax);
	const float cos_ax = cosf(ax);
	const float sin_ay = sinf(ay);
	const float cos_ay = cosf(ay);
	const float sin_az = sinf(az);
	const float cos_az = cosf(az);

	pOut->_11 = (cos_az * cos_ay + sin_az * sin_ax * sin_ay);
	pOut->_12 = (sin_az * cos_ax);
	pOut->_13 = (cos_az * -sin_ay + sin_az * sin_ax * cos_ay);
	pOut->_21 = (-sin_az * cos_ay + cos_az * sin_ax * sin_ay);
	pOut->_22 = (cos_az * cos_ax);
	pOut->_23 = (-sin_az * -sin_ay + cos_az * sin_ax * cos_ay);
	pOut->_31 = (cos_ax * sin_ay);
	pOut->_32 = (-sin_ax);
	pOut->_33 = (cos_ax * cos_ay);
}

// 获得矩阵的旋转
inline void VisUtil_GetMatrixRotate(FmQuat* pOut, const FmMat4* pIn)
{
	FmVec3 position;
	FmVec3 scale;

	//VisUtil_MatrixDecompose(&scale, pOut, &position, pIn);
	FmMat4Decompose(&scale,pOut,&position, pIn);
}

// 获得矩阵的旋转（无缩放）
inline void VisUtil_GetMatrixRotateNoScale(FmQuat* pOut, 
	const FmMat4* pIn)
{
	//VisUtil_QuaternionRotationMatrix(pOut, pIn);
	FmQuatRotationMat4(pOut,pIn);
}

// 设置矩阵的旋转
inline void VisUtil_SetMatrixRotate(FmMat4* pOut, const FmQuat* rotate)
{
	FmMat4RotationQuat(pOut,rotate);
	/*
	FmVec3 scale;

	VisUtil_GetMatrixScale(&scale, pOut);

	float rx = rotate->x;
	float ry = rotate->y;
	float rz = rotate->z;
	float rw = rotate->w;
	float sx = scale.x;
	float sy = scale.y;
	float sz = scale.z;

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
	*/
}

// 设置矩阵的旋转（无缩放）
inline void VisUtil_SetMatrixRotateNoScale(FmMat4* pOut, 
	const FmQUATERNION* rotate)
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
}

// 向量长度
inline float VisUtil_Vec3Length(const FmVec3* pIn)
{
	return FmVec3Length(pIn);
}

// 获得矩阵的缩放
inline void VisUtil_GetMatrixScale(FmVec3* pOut, const FmMat4* pIn)
{
	float v[4];

	v[0] = pIn->_11 * pIn->_11 + pIn->_12 * pIn->_12 + pIn->_13 * pIn->_13;
	v[1] = pIn->_21 * pIn->_21 + pIn->_22 * pIn->_22 + pIn->_23 * pIn->_23;
	v[2] = pIn->_31 * pIn->_31 + pIn->_32 * pIn->_32 + pIn->_33 * pIn->_33;
	v[3] = 1.0F;

	//float* pv = &v[0];

	pOut->x = VisUtil_Vec3Length((const FmVec3*)&pIn->_11);
	pOut->y = VisUtil_Vec3Length((const FmVec3*)&pIn->_21);
	pOut->z = VisUtil_Vec3Length((const FmVec3*)&pIn->_31);
}

// 设置矩阵的角度
inline void VisUtil_SetMatrixAngle(FmMat4* pOut, float ax, float ay, 
	float az)
{
	FmVec3 scale;

	VisUtil_GetMatrixScale(&scale, pOut);

	const float sin_ax = sinf(ax);
	const float cos_ax = cosf(ax);
	const float sin_ay = sinf(ay);
	const float cos_ay = cosf(ay);
	const float sin_az = sinf(az);
	const float cos_az = cosf(az);

	pOut->_11 = scale.x * (cos_az * cos_ay + sin_az * sin_ax * sin_ay);
	pOut->_12 = scale.x * (sin_az * cos_ax);
	pOut->_13 = scale.x * (cos_az * -sin_ay + sin_az * sin_ax * cos_ay);
	pOut->_21 = scale.y * (-sin_az * cos_ay + cos_az * sin_ax * sin_ay);
	pOut->_22 = scale.y * (cos_az * cos_ax);
	pOut->_23 = scale.y * (-sin_az * -sin_ay + cos_az * sin_ax * cos_ay);
	pOut->_31 = scale.z * (cos_ax * sin_ay);
	pOut->_32 = scale.z * (-sin_ax);
	pOut->_33 = scale.z * (cos_ax * cos_ay);
}

// 设置矩阵的缩放
inline void VisUtil_SetMatrixScale(FmMat4* pOut, float sx, float sy, 
	float sz)
{
	FmVec3 old_scale;

	VisUtil_GetMatrixScale(&old_scale, pOut);

	float fx = sx / old_scale.x;
	float fy = sy / old_scale.y;
	float fz = sz / old_scale.z;

	pOut->_11 *= fx;
	pOut->_12 *= fx;
	pOut->_13 *= fx;
	pOut->_21 *= fy;
	pOut->_22 *= fy;
	pOut->_23 *= fy;
	pOut->_31 *= fz;
	pOut->_32 *= fz;
	pOut->_33 *= fz;
}

// 四元数转换成欧拉角
inline void VisUtil_QuaternionToPitchYawRoll(const FmQuat* q, 
	float* pitch, float* yaw, float* roll)
{
	float sp = -2.0F * (q->y * q->z - q->w * q->x);

	if ((sp > 0.9999F) || (sp < -0.9999F))
	{
		*pitch = FLOAT_PI * 0.5F * sp;
		*yaw = atan2(-q->x * q->z + q->w * q->y, 
			0.5F - q->y * q->y - q->z * q->z);
		*roll = 0.0F;
	}
	else
	{
		*pitch = asin(sp);
		*yaw = atan2(q->x * q->z + q->w * q->y, 
			0.5F - q->x * q->x - q->y * q->y);
		*roll = atan2(q->x * q->y + q->w * q->z, 
			0.5F - q->x * q->x - q->z * q->z);
	}
}

// 获得矩阵的角度
inline void VisUtil_GetMatrixAngle(FmVec3* pOut, const FmMat4* pIn)
{
	FmVec3 scale;
	FmVec3 position;
	FmQuat rotation;

	FmMat4Decompose(&scale, &rotation, &position, pIn);
	VisUtil_QuaternionToPitchYawRoll(&rotation, &pOut->x, &pOut->y, &pOut->z);
}

// 获得颜色值
inline FmVec4 VisUtil_GetColor2(unsigned int color, float intensity)
{
	float r = ((color >> 16) & 0xFF) / 255.0F;
	float g = ((color >> 8) & 0xFF) / 255.0F;
	float b = (color& 0xFF) / 255.0F;

	r *= intensity;
	g *= intensity;
	b *= intensity;

	return FmVec4(r, g, b, 1.0F);
}

//调整x为2的幂
inline int VisUtil_Pow2(int x)
{
	int nvalue = 1;
	for(int i = 0; i < 31;i++ )
	{
		if(nvalue>= x)
		{
		   break;
		}
		nvalue = nvalue<<1;
	}
    return nvalue;
}

inline unsigned short Vis_ConvertFloatToHalfFloat(float *fValue)
{
	unsigned int x = *(unsigned int *) fValue;
	unsigned int sign = (unsigned short) ( x >> 31);
	unsigned short hf;
	// get mantissa
	unsigned int mantissa = x & (( 1 << 23) - 1);
	// get exponent bits
	unsigned int exp = x & FM_FLOAT_MAX_BIASED_EXP;
	if( exp >= FM_HALF_FLOAT_MAX_BIASED_EXP_AS_SINGLE_FP_EXP)
	{
		// check if the original single precision float number is a NaN
		if( mantissa && (exp == FM_FLOAT_MAX_BIASED_EXP))
		{
			// we have a single precision NaN
			mantissa = (1 << 23) - 1;
		}
		else
		{
			// 16-bit half-float representation stores number as Inf
			mantissa = 0;
		}
		hf = (((unsigned short)sign) << 15) | ( unsigned short)(FM_HALF_FLOAT_MAX_BIASED_EXP) | (unsigned short)(mantissa >> 13);
	}
	// check if exponent is <= -15
	else if (exp <= FM_HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP)
	{
		// store a denorm half-float value or zero
		exp = (FM_HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP - exp) >> 23;
		mantissa >>= (14 + exp);
		hf = (((unsigned short)sign) << 15) | (unsigned short)(mantissa);
	}
	else
	{
		hf = (((unsigned short)sign) << 15) |
			(unsigned short)((exp - FM_HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP) >> 13) |
			(unsigned short)(mantissa >> 13);
	}
	return hf;
}

inline unsigned short Vis_ConvertFloatToHalfFloat(float fValue)
{
	return Vis_ConvertFloatToHalfFloat(&fValue);
}

inline float Vis_ConvertHalfFloatToFloat(unsigned short hf)
{
	unsigned int sign = (unsigned int)(hf >> 15);
	unsigned int mantissa = (unsigned int)(hf & ((1 << 10) - 1));
	unsigned int exp = (unsigned int)(hf & FM_HALF_FLOAT_MAX_BIASED_EXP);
	unsigned int f;
	if (exp == FM_HALF_FLOAT_MAX_BIASED_EXP)
	{
		// we have a half-float NaN or Inf
		// half-float NaNs will be converted to a single precision NaN
		// half-float Infs will be converted to a single precision Inf
		exp = FM_FLOAT_MAX_BIASED_EXP;
		if (mantissa)
			mantissa = (1 << 23) - 1; // set all bits to indicate a NaN
	}
	else if (exp == 0x0)
	{
		// convert half-float zero/denorm to single precision value
		if (mantissa)
		{
			mantissa <<= 1;
			exp = FM_HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP;
			// check for leading 1 in denorm mantissa
			while ((mantissa & (1 << 10)) == 0)
			{
				// for every leading 0, decrement single precision exponent by 1
				// and shift half-float mantissa value to the left
				mantissa <<= 1;
				exp -= (1 << 23);
			}
			// clamp the mantissa to 10-bits
			mantissa &= ((1 << 10) - 1);
			// shift left to generate single-precision mantissa of 23-bits
			mantissa <<= 13;
		}
	}
	else
	{
		// shift left to generate single-precision mantissa of 23-bits
		mantissa <<= 13;
		// generate single precision biased exponent value
		exp = (exp << 13) + FM_HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP;
	}
	f = (sign << 31) | exp | mantissa;
	return *((float *)&f);
}

#endif // _VISUAL_VIS_UTILS_H
