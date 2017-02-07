//--------------------------------------------------------------------
// 文件名:		Path.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2012年7月4日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#include "path.h"

/// entity: Path
/// \brief Path实体
/// \see 继承自IEntity
DECLARE_ENTITY("", Path, IVisBase);

/// property: float PointDistance
/// \brief 插值密度，即插值点的间距
DECLARE_PROPERTY(float, Path, PointDistance, GetPointDistance, SetPointDistance);
/// property: string InterpolationMode
/// \brief 插值方式
DECLARE_PROPERTY(std::string, Path, InterpolationMode, GetInterpolationModeString, 
	SetInterpolationModeString);
/// property: object BindObject
/// \brief 路径控制的对象
DECLARE_PROPERTY(PERSISTID, Path, BindObject, GetBindObject, SetBindObject);
/// property: object TargetObject
/// \brief 朝向的对象
DECLARE_PROPERTY(PERSISTID, Path, TargetObject, GetTargetObject, SetTargetObject);
/// property: float CurTime
/// \brief 当前时间
DECLARE_PROPERTY(float, Path, CurTime, GetCurTime, SetCurTime);
/// property: float TotalTime
/// \brief 总时间
DECLARE_PROPERTY(float, Path, TotalTime, GetTotalTime, SetTotalTime);
/// property: bool Pause
/// \brief 是否暂停
DECLARE_PROPERTY(bool, Path, Pause, GetPause, SetPause);

/// method: size_t AddKeyPoint(float x, float y, float z, float time);
/// \brief 增加一个关键点
/// \param x x坐标
/// \param y y坐标
/// \param z z坐标
/// \param time 关键帧时间
DECLARE_METHOD_4(size_t, Path, AddKeyPoint, float, float, float, float);
/// method: bool DelKeyPoint(size_t index);
/// \brief 删除一个关键点
/// \param index 关键点索引
DECLARE_METHOD_1(bool, Path, DelKeyPoint, size_t);

#define LINEAR "Linear"
#define CURVE "Curve"

Path::Path()
{
	m_nInterpolationMode = InterpolationModeCurve;
	//m_nInterpolationMode = InterpolationModeLinear;
	m_fPointDistance = 0.1f;

	m_nOrientationMode = OrientationModeTarget;

	FmMatrixIdentity(&m_matWorld);
	m_uLineColor = 0xffff0000;
	m_uPointColor = 0xffffffff;
	m_uKeyPointCubeColor = 0xffff0000;

	m_nCurKeyPoint = 0;
	m_fCurTime = 0.0f;
	m_fTotalTime = 0.0f;
	m_bPause = true;
}

bool Path::Init(const IVarList& args)
{
	if (!IVisual::Init(args))
	{
		return false;
	}
	return true;
}

bool Path::Shut()
{
	return IVisual::Shut();
}

void Path::Update(float seconds)
{
	if (GetPause())
	{
		return;
	}

	// 上帧已经达到最大时间
	if (m_fCurTime >= m_fTotalTime)
	{
		// 从头开始
		m_fCurTime = 0.0f;
	}
	else
	{
		m_fCurTime += seconds;
		if (m_fCurTime > m_fTotalTime)
		{
			m_fCurTime = m_fTotalTime;
		}
	}

	InnerUpdateObject();
}

void Path::Realize(float offset_seconds)
{
	if (!GetVisible())
	{
		return;
	}

	if (m_lines.size() > 1)
	{
		m_pRender->DrawDesignLine(m_matWorld, D3DPT_LINELIST, m_lines.size() / 2, 
			&m_lines[0], sizeof(path_vertex_t));
	}
}

void Path::SetPointDistance(float value)
{
	if (value <= 0.0f)
	{
		return;
	}
	m_fPointDistance = value;
}

void Path::SetInterpolationModeString(const char* value)
{
	if (stricmp(value, LINEAR) == 0)
	{
		InnerSetInterpolationMode(InterpolationModeLinear);
	}
	else if (stricmp(value, CURVE) == 0)
	{
		InnerSetInterpolationMode(InterpolationModeCurve);
	}
}

std::string Path::GetInterpolationModeString() const
{
	switch (m_nInterpolationMode)
	{
	case InterpolationModeLinear:
		return std::string(LINEAR);
	case InterpolationModeCurve:
		return std::string(CURVE);
	default:
		Assert(0);
		break;
	}
	return std::string();
}

size_t Path::AddKeyPoint(float x, float y, float z, float time)
{
	m_keyPoints.push_back(key_point_t());
	key_point_t& key_point = m_keyPoints.back();
	key_point.pos.x = x;
	key_point.pos.y = y;
	key_point.pos.z = z;
	key_point.time = time;

	if (time > m_fTotalTime)
	{
		m_fTotalTime = time;
	}

	RebuildInterpolationPoint();

	Assert(!m_keyPoints.empty());
	return m_keyPoints.size() - 1;
}

bool Path::DelKeyPoint(size_t index)
{
	if (index >= m_keyPoints.size())
	{
		return false;
	}
	m_keyPoints.erase(m_keyPoints.begin() + index);
	RebuildInterpolationPoint();
	return true;
}

bool Path::SetKeyPointPosition(size_t index, float x, float y, float z)
{
	if (index >= m_keyPoints.size())
	{
		return false;
	}

	FmVec3& pos = m_keyPoints[index].pos;
	pos.x = x;
	pos.y = y;
	pos.z = z;
	return true;
}

bool Path::SetKeyPointTime(size_t index, float time)
{
	if (index >= m_keyPoints.size())
	{
		return false;
	}
	m_keyPoints[index].time = time;
	return true;
}

void Path::InnerSetInterpolationMode(int value)
{
	if (value == m_nInterpolationMode)
	{
		return;
	}
	m_nInterpolationMode = value;
	RebuildInterpolationPoint();
}

bool Path::RebuildInterpolationPoint()
{
	m_points.clear();
	m_lines.clear();
	m_keyPointCube.clear();

	if (m_keyPoints.empty())
	{
		return true;
	}

	// 预先分配空间
	m_points.reserve(m_keyPoints.size() * 10);
	m_lines.reserve(m_points.size() * 2);
	m_keyPointCube.reserve(m_keyPoints.size() * 4);

	if (m_keyPoints.size() < 2)
	{
		//InnerAddPoint(m_keyPoints[0], m_points);
		return true;
	}

	FmVec3 tmp;
	for (size_t i = 1; i < m_keyPoints.size(); ++i)
	{
		const FmVec3& pos_prev = m_keyPoints[i - 1].pos;
		const FmVec3& pos_cur = m_keyPoints[i].pos;
		float dx = pos_cur.x - pos_prev.x;
		float dy = pos_cur.y - pos_prev.y;
		float dz = pos_cur.z - pos_prev.z;

		// 插入前一个关键点
		InnerAddPoint(pos_prev, m_uLineColor, m_lines);
		// 如果前一个关键点不是第一个关键点
		if (i != 1)
		{
			InnerAddPoint(pos_prev, m_uLineColor, m_lines);
		}

		// 如果两个关键点坐标一样则忽略第二个点，并且不需要插值
		if (0.0f == dx && 0.0f == dy && 0.0f == dz)
		{
			continue;
		}

		float distance = sqrtf(dx * dx + dy * dy + dz * dz);
		Assert(distance != 0.0f);

		if (InterpolationModeLinear == m_nInterpolationMode)
		{
			for (float dis = m_fPointDistance; dis <= distance; dis += m_fPointDistance)
			{
				float factor = dis / distance;
				tmp.x = pos_prev.x + dx * factor;
				tmp.y = pos_prev.y + dy * factor;
				tmp.z = pos_prev.z + dz * factor;
				InnerAddPoint(tmp, m_uLineColor, m_lines);
				InnerAddPoint(tmp, m_uLineColor, m_lines);
			}
		}
		else if (InterpolationModeCurve == m_nInterpolationMode)
		{
			const FmVec3* pV0 = &pos_prev;
			const FmVec3* pV1 = &pos_prev;
			const FmVec3* pV2 = &pos_cur;
			const FmVec3* pV3 = &pos_cur;
			// 如果前一个点不是第一个关键点
			if (i - 1 > 0)
			{
				pV0 = &m_keyPoints[i-2].pos;
			}
			// 如果后一个点不是最后一个关键点
			if (i + 1 < m_keyPoints.size())
			{
				pV3 = &m_keyPoints[i+1].pos;
			}
			FmVec3 pos;
			for (float dis = m_fPointDistance; dis <= distance; dis += m_fPointDistance)
			{
				D3DXVec3CatmullRom(&pos, pV0, pV1, pV2, pV3, dis / distance);
				InnerAddPoint(pos, m_uLineColor, m_lines);
				InnerAddPoint(pos, m_uLineColor, m_lines);
			}
		}

		// 插入当前关键点
		InnerAddPoint(pos_cur, m_uLineColor, m_lines);
		// 如果当前关键点不是最后一个关键点
		if (i + 1 != m_keyPoints.size())
		{
			InnerAddPoint(pos_cur, m_uLineColor, m_lines);
		}
	}
	return true;
}

void Path::InnerAddPoint(const FmVec3& pos, unsigned int uColor,
						 std::vector<path_vertex_t>& v)
{
	v.push_back(path_vertex_t());
	path_vertex_t& point = v.back();
	point.pos = pos;
	point.color = uColor;
}

void Path::SetBindObject(const PERSISTID& value)
{
	m_bindObject = value;
	InnerUpdateObject();
}

PERSISTID Path::GetBindObject() const
{
	return m_bindObject;
}

void Path::SetTargetObject(const PERSISTID& value)
{
	m_targetObject = value;
}

PERSISTID Path::GetTargetObject() const
{
	return m_targetObject;
}

void Path::SetCurTime(float value)
{
	if (value >= 0.0f && value <= m_fTotalTime)
	{
		m_fCurTime = value;
		// 重新计算当前关键点
		m_nCurKeyPoint = 0;
		InnerUpdateObject();
	}
}

float Path::GetCurTime() const
{
	return m_fCurTime;
}

void Path::SetTotalTime(float value)
{
	// 不能小于最后一帧的时间
	if (value < m_keyPoints.back().time)
	{
		value = m_keyPoints.back().time;
	}
	m_fTotalTime = value;
}

float Path::GetTotalTime() const
{
	return m_fTotalTime;
}

void Path::SetPause(bool value)
{
	m_bPause = value;
}

bool Path::GetPause() const
{
	return m_bPause;
}

void Path::InnerUpdateObject()
{
	// 当前时间是否超过第一个关键帧
	bool bBeyondFirstKeyPoint = false;

	// 验证上一次当前关键点的有效性
	if (m_fCurTime < m_keyPoints[m_nCurKeyPoint].time)
	{
		m_nCurKeyPoint = 0;
	}

	// 朝向点坐标
	FmVec3 posTarget;

	if (OrientationModeTarget == m_nOrientationMode)
	{
		IVisBase* pTargetObject = GetVisBase(m_targetObject);
		if (pTargetObject)
		{
			posTarget = pTargetObject->GetPosition();
		}
	}

	FmVec3 tmp;
	int k = m_nCurKeyPoint;
	for (; k < (int)m_keyPoints.size(); ++k)
	{
		if (m_fCurTime <= m_keyPoints[k].time)
		{
			// 如果恰好是第一帧
			if (0 == k && m_fCurTime == m_keyPoints[0].time)
			{
				tmp = m_keyPoints[0].pos;
				bBeyondFirstKeyPoint = true;
			}
			else if (k > 0)
			{
				// 在两个关键点间插值
				key_point_t& prev_key_point = m_keyPoints[k-1];
				key_point_t& next_key_point = m_keyPoints[k];
				FmVec3& prev_pos = prev_key_point.pos;
				FmVec3& next_pos = next_key_point.pos;

				float factor = (m_fCurTime - prev_key_point.time) / 
					(next_key_point.time - prev_key_point.time);

				if (InterpolationModeLinear == m_nInterpolationMode)
				{
					tmp.x = prev_pos.x + (next_pos.x - prev_pos.x) * factor;
					tmp.y = prev_pos.y + (next_pos.y - prev_pos.y) * factor;
					tmp.z = prev_pos.z + (next_pos.z - prev_pos.z) * factor;
				}
				else if (InterpolationModeCurve == m_nInterpolationMode)
				{
					const FmVec3* pV0 = &prev_pos;
					const FmVec3* pV1 = &prev_pos;
					const FmVec3* pV2 = &next_pos;
					const FmVec3* pV3 = &next_pos;
					// 如果前一个点不是第一个关键点
					if (k - 1 > 0)
					{
						pV0 = &m_keyPoints[k-2].pos;
					}
					// 如果后一个点不是最后一个关键点
					if (k + 1 < (int)m_keyPoints.size())
					{
						pV3 = &m_keyPoints[k+1].pos;
					}
					D3DXVec3CatmullRom(&tmp, pV0, pV1, pV2, pV3, factor);
				}

				bBeyondFirstKeyPoint = true;
			}
			break;
		}
	}

	// 是否超过最后一帧
	if (k >= (int)m_keyPoints.size())
	{
		bBeyondFirstKeyPoint = true;
		tmp = m_keyPoints.back().pos;
	}

	if (bBeyondFirstKeyPoint)
	{
		IVisBase* pBindObject = GetVisBase(m_bindObject);
		if (pBindObject)
		{
			pBindObject->SetPosition(tmp.x, tmp.y, tmp.z);

			if (m_nOrientationMode != OrientationModeNull)
			{
				FmVec3 angle;
				GetFaceAngle(&angle, &pBindObject->GetPosition(), &posTarget);
				pBindObject->SetAngle(angle.x, angle.y, angle.z);
			}
		}
	}
}

inline void fx_quaternion_from_rot_matrix(FmQuat *quat, 
										  const FmMat4* kRot)
{
	// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
	// article "Quaternion Calculus and Fast Animation".

	float fTrace = kRot->m[0][0]+kRot->m[1][1]+kRot->m[2][2];
	float fRoot;

	if ( fTrace > 0.0 )
	{
		// |w| > 1/2, may as well choose w > 1/2
		fRoot = sqrt(fTrace + 1.0f);  // 2w
		quat->w = 0.5f*fRoot;
		fRoot = 0.5f/fRoot;  // 1/(4w)
		quat->x = (kRot->m[2][1]-kRot->m[1][2])*fRoot;
		quat->y = (kRot->m[0][2]-kRot->m[2][0])*fRoot;
		quat->z = (kRot->m[1][0]-kRot->m[0][1])*fRoot;
	}
	else
	{
		// |w| <= 1/2
		static size_t s_iNext[3] = { 1, 2, 0 };
		size_t i = 0;
		if ( kRot->m[1][1] > kRot->m[0][0] )
			i = 1;
		if ( kRot->m[2][2] > kRot->m[i][i] )
			i = 2;
		size_t j = s_iNext[i];
		size_t k = s_iNext[j];

		fRoot = sqrt(kRot->m[i][i]-kRot->m[j][j]-kRot->m[k][k] + 1.0f);
		float* apkQuat[3] = { &quat->x, &quat->y, &quat->z };
		*apkQuat[i] = 0.5f*fRoot;
		fRoot = 0.5f/fRoot;
		quat->w = (kRot->m[k][j]-kRot->m[j][k])*fRoot;
		*apkQuat[j] = (kRot->m[j][i]+kRot->m[i][j])*fRoot;
		*apkQuat[k] = (kRot->m[k][i]+kRot->m[i][k])*fRoot;
	}
}

inline void fx_quaternion_from_axes(FmQuat *quat, 
									const FmVec3* x, const FmVec3* y, const FmVec3* z)
{
	FmMat4 kRot;

	kRot.m[0][0] = x->x;
	kRot.m[1][0] = x->y;
	kRot.m[2][0] = x->z;

	kRot.m[0][1] = y->x;
	kRot.m[1][1] = y->y;
	kRot.m[2][1] = y->z;

	kRot.m[0][2] = z->x;
	kRot.m[1][2] = z->y;
	kRot.m[2][2] = z->z;

	fx_quaternion_from_rot_matrix(quat, &kRot);
}

FmVec3* Path::GetFaceAngle(FmVec3* pOut, FmVec3* pEye, FmVec3* pAt)
{
	FmVec3 up(0.0f, 1.0f, 0.0f);

	// 朝向向量
	FmVec3 v = *pAt - *pEye;
	FmVec3Normalize(&v, &v);

	// 右方向向量
	FmVec3Cross(pOut, &up, &v);
	FmVec3Normalize(pOut, pOut);

	// 向上的向量
	FmVec3Cross(&up, &v, pOut);
	FmVec3Normalize(&up, &up);

	// 方法1
	FmQuat quater_rotate;
	fx_quaternion_from_axes(&quater_rotate, pOut, &up, &v);
	VisUtil_QuaternionToPitchYawRoll(&quater_rotate, &pOut->x, 
		&pOut->y, &pOut->z);

	//// 方法2
	//FmMat4 mat;
	////static FmVec3 up(0.0f, 1.0f, 0.0f);
	//D3DXMatrixLookAtLH(&mat, pEye, pAt, &up);
	//VisUtil_GetMatrixAngle(pOut, &mat);

	return pOut;
}

IVisBase* Path::GetVisBase(const PERSISTID& id) const
{
	IEntity* pEntity = GetCore()->GetEntity(id);

	if (NULL == pEntity)
	{
		return NULL;
	}

	if (!pEntity->GetEntInfo()->IsKindOf("IVisBase"))
	{
		return NULL;
	}

	return (IVisBase*)pEntity;
}
