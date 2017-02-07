//--------------------------------------------------------------------
// 文件名:		KeyFrameController.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2012年8月6日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#include "key_frame_controller.h"
#include "time_axis.h"
#include "time_axis_macro.h"
#include "utility.h"
#include "../public/core_log.h"
#include "rapid_xml_helper.h"
#include "helper.h"
#include "../visual/i_terrain.h"
#include "../visual/i_time_axis.h"

#define  S_BILI 0.3f
#define  TIME_ERROR 65535.f

using namespace rapidxml;

const sKey* sKey::Copy(const sKey* pData)
{
	*this = *pData;

	return this;
}

const sKey* sKeyBezier::Copy(const sKey* pData)
{
	sKeyBezier *pKey = (sKeyBezier*)pData;
	*this = *pKey;

	return this;
}

const sKey* sKeyOnce::Copy(const sKey* pData)
{
	sKeyOnce *pKey = (sKeyOnce*)pData;
	*this = *pKey;

	str_param = pKey->str_param;
	return this;
}

const sKey* sKeyLine::Copy(const sKey* pData)
{
	sKeyLine *pKey = (sKeyLine*)pData;
	*this = *pKey;

	return this;
}

const sKey* sKeyScale::Copy(const sKey* pData)
{
	sKeyScale *pKey = (sKeyScale*)pData;
	*this = *pKey;

	str_param = pKey->str_param;
	return this;
}

sKeyBezier::sKeyBezier()
{
	kl = 0.f;
	kr = 0.f;
	sl = 0.f;
	sr = 0.f;
	mode = SHOW_DOC;
}

ITimeAxis * KeyFrameController::s_pTimeAxis = NULL;

KeyFrameController::KeyFrameController()
{
	m_pObject = NULL;
	m_bFirstFrame = false;
	m_bLastFrame = false;
}

KeyFrameController::~KeyFrameController()
{
	ClearKeyFrames();
}

void KeyFrameController::SetTimeAxis(ITimeAxis *pEntiy)
{
	s_pTimeAxis = pEntiy;
}

KeyFrameController::KeyFrameController(int controller_type, int usage_mode, sObjectData* pOwner)
{

	m_nControllerType = controller_type;
	m_nUsageMode = usage_mode;
	m_fLastTime = -1;

	m_pObject = pOwner;
}

KeyFrameController::KeyFrameController(const KeyFrameController& right)
{

	m_nControllerType = right.m_nControllerType;
	m_nUsageMode = right.m_nUsageMode;
	m_pObject = right.m_pObject;

	m_fLastTime = -1;

	int size = (int)right.m_keyFrames.size();
	m_keyFrames.reserve(size);
	for (int i = 0; i < size; ++i)
	{
		sKey *p = AddKey(right.m_keyFrames[i]->time);
		p->Copy(right.m_keyFrames[i]);
	}
}

KeyFrameController& KeyFrameController::operator=(const KeyFrameController& right)
{

	m_nControllerType = right.m_nControllerType;
	m_nUsageMode = right.m_nUsageMode;
	m_pObject = right.m_pObject;

	m_fLastTime = -1;

	int size = (int)right.m_keyFrames.size();
	m_keyFrames.reserve(size);
	for (int i = 0; i < size; ++i)
	{
		sKey *p = AddKey(right.m_keyFrames[i]->time);
		p->Copy(right.m_keyFrames[i]);
	}

	return *this;
}

float KeyFrameController::GetBegin() const
{
	int size = (int)m_keyFrames.size();
	if (size == 0)
	{
		return TIME_ERROR;
	}

	return m_keyFrames[0]->time;
}

float KeyFrameController::GetEnd() const
{
	int size = (int)m_keyFrames.size();
	if (size == 0)
	{
		return -TIME_ERROR;
	}

	return m_keyFrames[size - 1]->time;
}

sKey* KeyFrameController::AddKey(float fTime, int nIndex)
{

	sKey *p = NULL;
	switch(m_nUsageMode)
	{
	case CONTROLLER_USAGE_MODE_BEZIER:
		p = CORE_NEW(sKeyBezier);
		break;
	case CONTROLLER_USAGE_MODE_ONCE:
		p = CORE_NEW(sKeyOnce);
		break;
	case CONTROLLER_USAGE_MODE_LINEAE:
		p = CORE_NEW(sKeyLine);
		break;
	case CONTROLLER_USAGE_MODE_SCALE:
		p = CORE_NEW(sKeyScale);
		break;
	}

	if (NULL != p)
	{
		if (nIndex != -1)
		{
			m_keyFrames.insert(nIndex, p);

		}else
		{
			m_keyFrames.push_back(p);
			nIndex = (int)m_keyFrames.size() - 1;
		}

		p->time = fTime;
	}

	return p;
}

void KeyFrameController::DeleteKey(sKey* ptr)
{
	if (ptr)
	{
		switch(m_nUsageMode)
		{
		case CONTROLLER_USAGE_MODE_BEZIER:
			CORE_DELETE((sKeyBezier*)ptr);
			break;
		case CONTROLLER_USAGE_MODE_ONCE:
			CORE_DELETE((sKeyOnce*)ptr);
			break;
		case CONTROLLER_USAGE_MODE_LINEAE:
			CORE_DELETE((sKeyLine*)ptr);
			break;
		case CONTROLLER_USAGE_MODE_SCALE:
			CORE_DELETE((sKeyScale*)ptr);
			break;
		}
	}
}

//找出的index等于当前时刻帧index,或等于前关键帧index加1
sKey* KeyFrameController::FindKeyFrame(float fTime, int* pIndex) const
{

	int size = (int)m_keyFrames.size();
	if (size == 0)
	{
		if (NULL != pIndex)
		{
			*pIndex = 0;
		}
		return NULL;
	}

	// 采用二叉树查找.
	int start = 0;
	int end = size - 1;
	int midle = 0;
	int InsertIndex = -1;
	sKey *pFindKey = NULL;

	// 符合跟末尾元素的判断关系
	if (fm_movie::FloatEqualLocal(fTime, m_keyFrames[end]->time))
	{
		InsertIndex = end;
		pFindKey = m_keyFrames[end];
	}
	else if(fTime > m_keyFrames[end]->time)
	{
		InsertIndex = end + 1;
	}
	// 符合跟首个元素的判断关系
	else if (fm_movie::FloatEqualLocal(fTime, m_keyFrames[start]->time))
	{
		InsertIndex = start;
		pFindKey = m_keyFrames[start];
	}
	else if(fTime < m_keyFrames[start]->time)
	{
		InsertIndex = start;
	}
	else
	{
		while(true)
		{
			// 查找到了最后,start跟end相等或者相邻
			if (end - start == 1)
			{
				InsertIndex = end;
				break;
			}

			// 取中间一个元素进行判断,并缩小范围
			midle = (start + end)/2;
			if (fm_movie::FloatEqualLocal(fTime, m_keyFrames[midle]->time))
			{
				// 等于中间的值
				InsertIndex = midle;
				pFindKey = m_keyFrames[midle];
				break;
			}
			else if (fTime < m_keyFrames[midle]->time)
			{
				// 向前缩小查找范围
				end = midle;
			}
			else
			{
				// 向后缩小查找范围
				start = midle;
			}
		}
	}

	if (NULL != pIndex)
	{
		*pIndex = InsertIndex;
	}
	
	return pFindKey;
}

void KeyFrameController::AddBezierKey(float time, float value)
{
	if (m_nUsageMode != CONTROLLER_USAGE_MODE_BEZIER)
	{
		CORE_TRACE("(KeyFrameController::SetKeyBezierMode) Not Bezier Mode Controller.");
		//return false;
	}

	// 查找是否存在该时刻的关键帧,有则修改,无则按次序添加
	int nIndex = -1;
	sKeyBezier* pKeyB = (sKeyBezier*)FindKeyFrame(time, &nIndex);
	if (NULL == pKeyB)
	{
		pKeyB = (sKeyBezier*)AddKey(time, nIndex);
		Assert(pKeyB);

	}

	pKeyB->time = time;
	pKeyB->value = value;
	pKeyB->left.x = time;
	pKeyB->left.y = value;
	pKeyB->right.x = time;
	pKeyB->right.y = value;
	pKeyB->mode = SHOW_CURVE1;

	CreateOneControlPoint(nIndex - 1);
	CreateOneControlPoint(nIndex);
	CreateOneControlPoint(nIndex + 1);

	//发送更新某个关键帧的消息
	SendUpdateBezierKey(nIndex);

	//发送增加关键帧消息
	fm_movie::TimeAxis_RunCallback(s_pTimeAxis, "on_key_update", CVarList()<<
		m_pObject->strName.c_str()<< m_nControllerType<< CHANGE_ADD_KEY<< time);
}

int KeyFrameController::GetFrontBezierKey(float time, float &value) const
{
	if (m_nUsageMode != CONTROLLER_USAGE_MODE_BEZIER)
	{
		CORE_TRACE("(KeyFrameController::SetKeyBezierMode) Not Bezier Mode Controller.");
		return -1;
	}

	size_t size = m_keyFrames.size();
	if (0 == size)
	{
		return -1;
	}

	// 查找是否存在该时刻的关键帧
	int nIndex = -1;
	FindKeyFrame(time, &nIndex);
	nIndex -= 1; 

	if (nIndex < 0)
	{
		nIndex = 0;
	}
	
	value = ((sKeyBezier*)m_keyFrames[nIndex])->value;
	return nIndex;
}

void KeyFrameController::GetAdjacencyBezierKey(float time, IVarList& result) const
{

	if (m_nUsageMode != CONTROLLER_USAGE_MODE_BEZIER)
	{
		CORE_TRACE("(KeyFrameController::GetAdjacencyBezierKey) Not Bezier Mode Controller.");
		return;
	}

	int nSize = (int)m_keyFrames.size();
	if (0 == nSize)
	{
		return;
	}

	int nIndex = -1;

	sKey* pKey = FindKeyFrame(time, &nIndex);
	if (NULL == pKey)
	{
		return;
	}

	int nFrontIndex = nIndex - 1;
	int nBackIndex = nIndex + 1;

	result.AddPointer((void*)m_keyFrames[nFrontIndex]);

	if (nFrontIndex >= 0 && nFrontIndex < nSize)
	{
		result.AddPointer((void*)m_keyFrames[nFrontIndex]);
	}

	if (nBackIndex >= 0 && nBackIndex < nSize)
	{
		result.AddPointer((void*)m_keyFrames[nBackIndex]);
	}
}

const sKey* KeyFrameController::GetKeyFrameByTime(float time, int* index) const
{

	*index = -1;

	sKey* pKey = FindKeyFrame(time, index);
	if (NULL == pKey)
	{
		return NULL;
	}

	return pKey;
}

void KeyFrameController::SendUpdateBezierKey(int nIndex)
{
	CVarList param;
	param<< m_pObject->strName.c_str()<< m_nControllerType<< CHANGE_ADD_BEZIER_KEY;

	int count = (int)m_keyFrames.size();
	for (int i = nIndex - 1; i < nIndex + 2; ++i)
	{
		if (i >= 0 && i < count)
		{
			sKeyBezier *pKey = (sKeyBezier*)m_keyFrames[i];
			param<< pKey->time<< pKey->value<< pKey->left_time<< pKey->left_value
				<< pKey->right_time<< pKey->right_value;
		}
	}

	fm_movie::TimeAxis_RunCallback(s_pTimeAxis, "on_key_update",param);
}

bool KeyFrameController::SetKeyFrameBezierMode(float time, float value, 
									 float left_time, float left_value, 
									 float right_time, float right_value, bool bCallback)
{
	if (m_nUsageMode != CONTROLLER_USAGE_MODE_BEZIER)
	{
		CORE_TRACE("(KeyFrameController::SetKeyBezierMode) Not Bezier Mode Controller.");
		return false;
	}

	// 查找是否存在该时刻的关键帧,有则修改,无则按次序添加
	int nIndex = -1;
	sKeyBezier* pKeyB = (sKeyBezier*)FindKeyFrame(time, &nIndex);
	if (NULL == pKeyB)
	{
		pKeyB = (sKeyBezier*)AddKey(time, nIndex);
		Assert(pKeyB);

	}

	pKeyB->time = time;
	pKeyB->value = value;
	pKeyB->left_time = left_time;
	pKeyB->left_value = left_value;
	pKeyB->right_time = right_time;
	pKeyB->right_value = right_value;

	if (bCallback)
	{
		//发送增加关键帧消息
		fm_movie::TimeAxis_RunCallback(s_pTimeAxis, "on_key_update", CVarList()<<
			m_pObject->strName.c_str()<< m_nControllerType<< CHANGE_ADD_KEY<< time);
	}

	return true;
}

bool KeyFrameController::SetKeyFrameLinearMode(float time, float value, bool bCallback)
{
	if (m_nUsageMode != CONTROLLER_USAGE_MODE_LINEAE)
	{
		CORE_TRACE("(KeyFrameController::SetKeyLinearMode) Not Linear Mode Controller.");
		return false;
	}

	int index = -1;
	sKeyLine* pKey = (sKeyLine*)FindKeyFrame(time, &index);
	if (NULL == pKey)
	{
		pKey = (sKeyLine*)AddKey(time, index);
		Assert(pKey);
	}

	pKey->time = time;
	pKey->value = value;

	if (bCallback)
	{
		//发送增加关键帧消息
		fm_movie::TimeAxis_RunCallback(s_pTimeAxis, "on_key_update", CVarList()<<
			m_pObject->strName.c_str()<< m_nControllerType<< CHANGE_ADD_KEY<< time);

	}
	return true;
}

bool KeyFrameController::SetKeyFrameOnceMode(float time, const char* param, bool bCallback)
{
	if (m_nUsageMode != CONTROLLER_USAGE_MODE_ONCE)
	{
		CORE_TRACE("(KeyFrameController::SetKeyOnceMode) Not Once Mode Controller.");
		return false;
	}

	int nIndex = -1;
	sKeyOnce* pKey = (sKeyOnce*)FindKeyFrame(time, &nIndex);
	if (NULL == pKey)
	{
		pKey = (sKeyOnce*)AddKey(time, nIndex);
		Assert(pKey);
	}

	pKey->time = time;
	pKey->str_param = param;

	if (bCallback)
	{
		//发送增加关键帧消息
		fm_movie::TimeAxis_RunCallback(s_pTimeAxis, "on_key_update", CVarList()<<
			m_pObject->strName.c_str()<< m_nControllerType<< CHANGE_ADD_KEY<< time);

	}
	return true;
}

bool KeyFrameController::SetKeyFrameScaleMode(float time, 
	const char* action_name, float time_length, float speed, bool bCallback)
{
	if (m_nUsageMode != CONTROLLER_USAGE_MODE_SCALE)
	{
		CORE_TRACE("(KeyFrameController::SetKeyScaleMode) Not Once Mode Controller.");
		return false;
	}

	int nIndex = -1;
	sKeyScale* pKey = (sKeyScale*)FindKeyFrame(time, &nIndex);
	if (NULL == pKey)
	{
		pKey = (sKeyScale*)AddKey(time, nIndex);
		Assert(pKey);
	}

	pKey->time = time;
	pKey->str_param = action_name;
	pKey->time_length = time_length;
	pKey->speed = speed;

	if (bCallback)
	{
		//发送控制点变化消息
		fm_movie::TimeAxis_RunCallback(s_pTimeAxis, "on_key_update", CVarList()<<
			m_pObject->strName.c_str()<< m_nControllerType<< CHANGE_ADD_KEY<< time);

	}

	return true;
}

void KeyFrameController::DeleteControllerKey(float time)
{

	int nIndex = -1;
	sKey *pKey = FindKeyFrame(time, &nIndex);
	if (pKey != NULL && nIndex != -1)
	{
		DeleteKey(pKey);
		m_keyFrames.remove(nIndex);
	}

	if (m_nUsageMode == CONTROLLER_USAGE_MODE_BEZIER)
	{
		CreateOneControlPoint(nIndex-1);
		CreateOneControlPoint(nIndex);
		CreateOneControlPoint(nIndex+1);

	}

	//发送删除关键帧消息
	fm_movie::TimeAxis_RunCallback(s_pTimeAxis, "on_key_update", CVarList()<<
		m_pObject->strName.c_str()<< m_nControllerType<< CHANGE_DLETEKEY<< time);

}

void KeyFrameController::ClearKeyFrames()
{
	for (int i = 0; i < (int)m_keyFrames.size(); ++i)
	{
		DeleteKey(m_keyFrames[i]);
	}
	m_keyFrames.clear();
}

bool KeyFrameController::GetKeyInfo(IVisBase* pObject, float time, IVarList &param)
{
	if (m_keyFrames.empty())
	{
		return false;
	}
	
	if (CONTROLLER_USAGE_MODE_BEZIER == m_nUsageMode)
	{
		param<< m_nControllerType;
		param<<(GetValueBezierMode(time));
	}
	else if (CONTROLLER_USAGE_MODE_LINEAE == m_nUsageMode)
	{
		param<< m_nControllerType;
		param<<(GetValueLinearMode(time));
	}
	else if (CONTROLLER_USAGE_MODE_ONCE == m_nUsageMode)
	{
		const char* param_list = GetValueOnceMode(time);
		m_fLastTime = time;

		if (param_list == NULL)
		{
			return false;
		}

		param<< m_nControllerType;
		param<<param_list;
		
	}
	else if (CONTROLLER_USAGE_MODE_SCALE == m_nUsageMode)
	{
		core_string action_name;
		float action_time = 0;
		float speed = 1.0f;
		bool bExecute = GetValueScaleMode(time, action_name, action_time, speed);
		if (!bExecute)
		{
			return false;
		}

		param<< m_nControllerType;
		param<<action_name.c_str()<<action_time<<speed;
	}
	else
	{
		param<< -1;
		param<< -1;
	}

	return true;
}

// Helper functions:
float slopeFromT (float t, float A, float B, float C)
{
	float dtdx = 1.0f/(3.0f*A*t*t + 2.0f*B*t + C); 
	return dtdx;
}
float xFromT (float t, float A, float B, float C, float D)
{
	float x = A*(t*t*t) + B*(t*t) + C*t + D;
	return x;
}
float yFromT (float t, float E, float F, float G, float H)
{
	float y = E*(t*t*t) + F*(t*t) + G*t + H;
	return y;
}

float constrain(float value, float min, float max)
{
	if (value < min)
	{
		return min;
	}
	else if (value > max)
	{
		return max;
	}
	return value;
}

float KeyFrameController::GetBezierValue(float x, float c0x, float c0y, float c1x, float c1y, 
										 float c2x, float c2y, float c3x, float c3y) const
{
	float A = c3x - 3 * c2x + 3 * c1x - c0x;
	float B = 3 * c2x - 6 * c1x + 3 * c0x;
	float C = 3 * c1x - 3 * c0x;
	float D = c0x;

	float E = c3y - 3 * c2y + 3 * c1y - c0y;    
	float F = 3 * c2y - 6 * c1y + 3 * c0y;
	float G = 3 * c1y - 3 * c0y;
	float H = c0y;

	// Solve for t given x (using Newton-Raphelson), then solve for y given t.
	// 先假设t=0.5f
	float currentt = 0.5f;
	int nRefinementIterations = 5;
	for (int i=0; i < nRefinementIterations; i++)
	{
		float currentx = xFromT (currentt, A,B,C,D); 
		float currentslope = slopeFromT (currentt, A,B,C);
		currentt -= (currentx - x)*(currentslope);
		currentt = constrain(currentt, 0,1);
	} 

	float y = yFromT (currentt,  E,F,G,H);
/*
	if (fm_movie::FloatEqualLocal(c0x, c3x))
	{
		return (c0y + c3y)/2.f;
	}

	float y = yFromT ((x - c0x)/(c3x - c0x),  E, F, G, H);
*/
	return y;
}

float KeyFrameController::GetValueBezierMode(float time) const
{
	if (m_keyFrames.empty())
	{
		//Assert(0);
		return 0.0f;
	}

	// 在第一帧前或就在第一帧上
	const sKeyBezier* pKey = (sKeyBezier*)m_keyFrames[0];
	if (time <= pKey->time)
	{
		return pKey->value;
	}

	// 在最后一帧后或就在最后一帧上
	pKey = (sKeyBezier*)m_keyFrames.back();
	if (time >= pKey->time)
	{
		return pKey->value;
	}

	// 找到第一个不小于当前时间的关键帧
	int nIndex = -1;
	const sKeyBezier* pKeyNext = (const sKeyBezier*)FindKeyFrame(time, &nIndex);

	// 正好在关键帧上
	if (NULL != pKeyNext)
	{
		return pKeyNext->value;
	}

	Assert(nIndex != -1);

	// 当前时间后面的关键帧
	pKeyNext = (sKeyBezier*)m_keyFrames[nIndex];
	// 当前时间前面的关键帧
	const sKeyBezier* pKeyPrev = (sKeyBezier*)m_keyFrames[nIndex - 1];
	//Keys::const_iterator ite_prev = ite_next;
	//--ite_prev;
	//const sKey& key_frame_prev = (*ite_prev).second;

	//// 突变方式
	//if (key_frame_prev.bStep || key_frame_next.bStep)
	//{
	//	return key_frame_prev.value;
	//}

	// 插值
	return GetBezierValue(time, pKeyPrev->time, pKeyPrev->value,
		pKeyPrev->right_time, pKeyPrev->right_value,
		pKeyNext->left_time, pKeyNext->left_value,
		pKeyNext->time, pKeyNext->value);
}

const char* KeyFrameController::GetValueOnceMode(float time) const
{
	if (m_keyFrames.empty())
	{
		return NULL;
	}

	// 在第一帧前或就在第一帧上
	const sKeyOnce* pKeyFirst = (sKeyOnce*)m_keyFrames[0];
	if (time < pKeyFirst->time)
	{
		return NULL;
	}

	// 在最后一帧后或就在最后一帧上
	const sKeyOnce* pKeyEnd = (sKeyOnce*)m_keyFrames.back();
	if (time > pKeyEnd->time)
	{
		// 如果上一次没执行过最后一帧，则这次执行
		if (m_fLastTime < pKeyEnd->time)
		{
			return pKeyEnd->str_param.c_str();
		}
		else
		{
			return NULL;
		}
	}

	// 找到第一个不小于当前时间的关键帧
	int nIndex = -1;
	const sKeyOnce* pKeyNext = (sKeyOnce*)FindKeyFrame(time, &nIndex);

	// 正好在关键帧上
	if (NULL != pKeyNext)
	{
		return pKeyNext->str_param.c_str();
	}

	Assert(nIndex > 0);

	// 当前时间前面的关键帧
	const sKeyOnce* pKeyPrev = (sKeyOnce*)m_keyFrames[nIndex - 1];

	// 如果刚超过前面的关键帧
	if (m_fLastTime < pKeyPrev->time)
	{
		return pKeyPrev->str_param.c_str();
	}

	return NULL;
}

float KeyFrameController::GetValueLinearMode(float time) const
{
	if (m_keyFrames.empty())
	{
		return 0.0f;
	}

	// 在第一帧前或就在第一帧上
	const sKeyLine* pKeyFirst = (sKeyLine*)m_keyFrames[0];
	if (time <= pKeyFirst->time)
	{
		return pKeyFirst->value;
	}

	// 在最后一帧后或就在最后一帧上
	const sKeyLine* pKeyEnd = (sKeyLine*)m_keyFrames.back();
	if (time >= pKeyEnd->time)
	{
		return pKeyEnd->value;
	}

	// 找到第一个不小于当前时间的关键帧
	int nIndex = -1;
	const sKeyLine* pKeyNext = (sKeyLine*)FindKeyFrame(time, &nIndex);

	// 正好在关键帧上
	if (NULL != pKeyNext)
	{
		return pKeyNext->value;
	}

	Assert(nIndex != -1);

	// 当前时间后面的关键帧
	pKeyNext = (sKeyLine*)m_keyFrames[nIndex];
	// 当前时间前面的关键帧
	const sKeyLine* pKeyPrev = (sKeyLine*)m_keyFrames[nIndex - 1];

	// 插值
	return pKeyPrev->value + (pKeyNext->value - pKeyPrev->value) *
		((time - pKeyPrev->time) / (pKeyNext->time - pKeyPrev->time));
}

bool KeyFrameController::GetValueScaleMode(float time, 
			core_string& action_name, float& action_time, float& speed)
{
	if (m_keyFrames.empty())
	{
		return false;
	}

	// 找到第一个大于当前时间的关键帧
	int nIndex = -1;
	const sKeyScale* pKeyPrev = (sKeyScale*)FindKeyFrame(time, &nIndex);

	// 如果正好在关键帧上.
	if (pKeyPrev)
	{
		action_name = pKeyPrev->str_param.c_str();
		speed = pKeyPrev->speed;
		action_time = 0;

		return true;
	}

	//  取靠左边的关键帧
	if (nIndex > 0)
	{
		pKeyPrev = (sKeyScale*)m_keyFrames[nIndex - 1];
	}
	else
	{
		pKeyPrev = (sKeyScale*)m_keyFrames[0];
	}

	// 判断是否执行该动作帧数据
	if (time < pKeyPrev->time)
	{
		// 如果还未到动画时间
		if (m_bFirstFrame)
		{
			return false;
		}

		action_name = pKeyPrev->str_param.c_str();
		speed = pKeyPrev->speed;
		action_time = 0;
		m_bFirstFrame = true;
	}
	else
	{
		action_name = pKeyPrev->str_param.c_str();
		action_time = (time - pKeyPrev->time) * pKeyPrev->speed;
		speed = pKeyPrev->speed;

		// 如果超过动画时间
		if (action_time > pKeyPrev->time_length)
		{
			if (m_bLastFrame)
			{
				return false;
			}
			action_time = pKeyPrev->time_length;
			m_bLastFrame = true;
		}
		else
		{
			m_bFirstFrame = false;
			m_bLastFrame = false;
		}
	}

	return true;
}

void KeyFrameController::GetKeyFrameValue(float time, IVarList& result) const
{
	result.Clear();

	int nUsage = GetUsageMode();
	int nFrameIndex = 0;

	const sKey* pFrameKey = FindKeyFrame(time, &nFrameIndex);
	if (NULL == pFrameKey)
	{
		return;
	}

	if (CONTROLLER_USAGE_MODE_BEZIER == nUsage)
	{
		const sKeyBezier* pKeyBezier = (sKeyBezier*)pFrameKey;

		result.AddFloat(pKeyBezier->time);
		result.AddFloat(pKeyBezier->value);
		result.AddFloat(pKeyBezier->left_time);
		result.AddFloat(pKeyBezier->left_value);
		result.AddFloat(pKeyBezier->right_time);
		result.AddFloat(pKeyBezier->right_value);
	}
	else if (CONTROLLER_USAGE_MODE_ONCE == nUsage)
	{
		const sKeyOnce* pKeyOnce = (sKeyOnce*)pFrameKey;

		result.AddFloat(pKeyOnce->time);
		result.AddString(pKeyOnce->str_param.c_str()); 
	}
	else if (CONTROLLER_USAGE_MODE_LINEAE == nUsage)
	{
		const sKeyLine* pKeyLine = (sKeyLine*)pFrameKey;

		result.AddFloat(pKeyLine->time);
		result.AddFloat(pKeyLine->value); 
	}
	else if (CONTROLLER_USAGE_MODE_SCALE == nUsage)
	{
		const sKeyScale* pKeyScale = (sKeyScale*)pFrameKey;

		result.AddFloat(pKeyScale->time);
		result.AddString(pKeyScale->str_param.c_str()); 
		result.AddFloat(pKeyScale->time_length); 
		result.AddFloat(pKeyScale->speed); 
	}
}

bool KeyFrameController::LoadController(xml_node<>* xml_controller)
{
	if (CONTROLLER_USAGE_MODE_BEZIER == m_nUsageMode)
	{
		return InnerLoadBezierController(xml_controller);
	}
	else if (CONTROLLER_USAGE_MODE_LINEAE == m_nUsageMode)
	{
		return InnerLoadLinearController(xml_controller);
	}
	else if (CONTROLLER_USAGE_MODE_ONCE == m_nUsageMode)
	{
		return InnerLoadOnceController(xml_controller);
	}
	else if (CONTROLLER_USAGE_MODE_SCALE == m_nUsageMode)
	{
		return InnerLoadScaleController(xml_controller);
	}
	else
	{
		CORE_TRACE("(KeyFrameController::LoadController) Invalid controller usage mode.");
		return false;
	}

	return false;
}

bool KeyFrameController::SaveController(rapidxml::xml_node<>* xml_controller)
{
	if (m_keyFrames.size() == 0)
	{
		return false;
	}

	if (CONTROLLER_USAGE_MODE_BEZIER == m_nUsageMode)
	{
		return InnerSaveBezierController(xml_controller);
	}
	else if (CONTROLLER_USAGE_MODE_LINEAE == m_nUsageMode)
	{
		return InnerSaveLinearController(xml_controller);
	}
	else if (CONTROLLER_USAGE_MODE_ONCE == m_nUsageMode)
	{
		return InnerSaveOnceController(xml_controller);
	}
	else if (CONTROLLER_USAGE_MODE_SCALE == m_nUsageMode)
	{
		return InnerSaveScaleController(xml_controller);
	}
	else
	{
		CORE_TRACE("(KeyFrameController::SaveController) Invalid controller usage mode.");
		return false;
	}

	return false;
}

bool KeyFrameController::InnerSaveBezierController(rapidxml::xml_node<>* xml_controller)
{
	// 控制器类型
	int type = GetControllerType();
	if (type < 0)
	{
		CORE_TRACE("(KeyFrameController::SaveController) Invalid controller type.");
		return false;
	}

	xml_document<>* doc = xml_controller->document();
	Assert(doc != NULL);

	xml_add_attr(doc, xml_controller, XML_ATTR_TYPE_ID, type);
	xml_add_attr(doc, xml_controller, XML_ATTR_USAGE_MODE, m_nUsageMode);

	// 关键帧
	for (int i = 0; i < (int)m_keyFrames.size(); ++i)
	{
		const sKeyBezier& key_frame = *(sKeyBezier*)m_keyFrames[i];

		xml_node<>* xml_key_frame = xml_add_node(doc, xml_controller, XML_NODE_KEYFRAME);
		Assert(xml_key_frame != NULL);

		xml_add_attr(doc, xml_key_frame, XML_ATTR_TIME, key_frame.time);
		xml_add_attr(doc, xml_key_frame, XML_ATTR_VALUE, key_frame.value);
		xml_add_attr(doc, xml_key_frame, XML_ATTR_PREV_CP_TIME, 
			key_frame.left_time);
		xml_add_attr(doc, xml_key_frame, XML_ATTR_PREV_CP_VALUE, 
			key_frame.left_value);
		xml_add_attr(doc, xml_key_frame, XML_ATTR_NEXT_CP_TIME, 
			key_frame.right_time);
		xml_add_attr(doc, xml_key_frame, XML_ATTR_NEXT_CP_VALUE, 
			key_frame.right_value);
	}
	return true;
}

bool KeyFrameController::InnerSaveLinearController(rapidxml::xml_node<>* xml_controller)
{
	// 控制器类型
	int type = GetControllerType();
	if (type < 0)
	{
		CORE_TRACE("(KeyFrameController::SaveController) Invalid controller type.");
		return false;
	}

	xml_document<>* doc = xml_controller->document();
	Assert(doc != NULL);

	xml_add_attr(doc, xml_controller, XML_ATTR_TYPE_ID, type);
	xml_add_attr(doc, xml_controller, XML_ATTR_USAGE_MODE, m_nUsageMode);

	// 关键帧
	for (int i = 0; i < (int)m_keyFrames.size(); ++i)
	{
		const sKeyLine& key_frame = *(sKeyLine*)m_keyFrames[i];

		xml_node<>* xml_key_frame = xml_add_node(doc, xml_controller, XML_NODE_KEYFRAME);
		Assert(xml_key_frame != NULL);

		xml_add_attr(doc, xml_key_frame, XML_ATTR_TIME, key_frame.time);
		xml_add_attr(doc, xml_key_frame, XML_ATTR_VALUE, key_frame.value);

	}
	return true;
}

bool KeyFrameController::InnerSaveOnceController(rapidxml::xml_node<>* xml_controller)
{
	// 控制器类型
	int type = GetControllerType();
	if (type < 0)
	{
		CORE_TRACE("(KeyFrameController::SaveController) Invalid controller type.");
		return false;
	}

	xml_document<>* doc = xml_controller->document();
	Assert(doc != NULL);

	xml_add_attr(doc, xml_controller, XML_ATTR_TYPE_ID, type);
	xml_add_attr(doc, xml_controller, XML_ATTR_USAGE_MODE, m_nUsageMode);

	// 关键帧
	for (int i = 0; i < (int)m_keyFrames.size(); ++i)
	{
		const sKeyOnce& key_frame = *(sKeyOnce*)m_keyFrames[i];

		xml_node<>* xml_key_frame = xml_add_node(doc, xml_controller, XML_NODE_KEYFRAME);
		Assert(xml_key_frame != NULL);

		xml_add_attr(doc, xml_key_frame, XML_ATTR_TIME, key_frame.time);
		xml_add_attr(doc, xml_key_frame, XML_ATTR_PARAM, key_frame.str_param.c_str());

	}
	return true;
}

bool KeyFrameController::InnerSaveScaleController(rapidxml::xml_node<>* xml_controller)
{
	// 控制器类型
	int type = GetControllerType();
	if (type < 0)
	{
		CORE_TRACE("(KeyFrameController::SaveController) Invalid controller type.");
		return false;
	}

	xml_document<>* doc = xml_controller->document();
	Assert(doc != NULL);

	xml_add_attr(doc, xml_controller, XML_ATTR_TYPE_ID, type);
	xml_add_attr(doc, xml_controller, XML_ATTR_USAGE_MODE, m_nUsageMode);

	// 关键帧
	for (int i = 0; i < (int)m_keyFrames.size(); ++i)
	{
		const sKeyScale& key_frame = *(sKeyScale*)m_keyFrames[i];

		xml_node<>* xml_key_frame = xml_add_node(doc, xml_controller, XML_NODE_KEYFRAME);
		Assert(xml_key_frame != NULL);

		xml_add_attr(doc, xml_key_frame, XML_ATTR_TIME, key_frame.time);
		xml_add_attr(doc, xml_key_frame, XML_ATTR_PARAM, key_frame.str_param.c_str());
		xml_add_attr(doc, xml_key_frame, XML_ATTR_TIME_LENGTH, key_frame.time_length);
		xml_add_attr(doc, xml_key_frame, XML_ATTR_SPEED, key_frame.speed);

	}
	return true;
}

void KeyFrameController::GetControllerKeyTime(IVarList& result) const
{
	for (int i = 0; i < (int)m_keyFrames.size(); ++i)
	{
		result<<m_keyFrames[i]->time;
	}
}

int KeyFrameController::GetKeyFrameListBezierMode(IVarList& result) const
{
	for (int i = 0; i < (int)m_keyFrames.size(); ++i)
	{
		const sKeyBezier& key_frame = *(sKeyBezier*)m_keyFrames[i];

		result<<key_frame.time<<key_frame.value
			<<key_frame.left_time
			<<key_frame.left_value
			<<key_frame.right_time
			<<key_frame.right_value;
	}
	return result.GetCount();
}

int KeyFrameController::GetKeyFrameListOnceMode(IVarList& result) const
{
	for (int i = 0; i < (int)m_keyFrames.size(); ++i)
	{
		const sKeyOnce& key_frame = *(sKeyOnce*)m_keyFrames[i];

		result<<key_frame.time<<key_frame.str_param.c_str();

	}
	return result.GetCount();
}

int KeyFrameController::GetKeyFrameListLinearMode(IVarList& result) const
{
	for (int i = 0; i < (int)m_keyFrames.size(); ++i)
	{
		const sKeyLine& key_frame = *(sKeyLine*)m_keyFrames[i];

		result<<key_frame.time<<key_frame.value;

	}
	return result.GetCount();
}

int KeyFrameController::GetKeyFrameListScaleMode(IVarList& result) const
{
	for (int i = 0; i < (int)m_keyFrames.size(); ++i)
	{
		const sKeyScale& key_frame = *(sKeyScale*)m_keyFrames[i];

		result<<key_frame.time<<key_frame.str_param<<key_frame.time_length
			<<key_frame.speed;

	}
	return result.GetCount();
}

bool KeyFrameController::InnerLoadBezierController(xml_node<>* xml_controller)
{
	xml_node<>* xml_key_frame = xml_controller->first_node(XML_NODE_KEYFRAME);
	// 没有关键帧
	if (NULL == xml_key_frame)
	{
		return true;
	}

	xml_attribute<>* xml_attr = NULL;
	while (xml_key_frame)
	{
		// 关键帧时间
		xml_attr = xml_key_frame->first_attribute(XML_ATTR_TIME);
		if (NULL == xml_attr)
		{
			CORE_TRACE("(KeyFrameController::InnerLoadBezierController) no time attribute.");
			Assert(0);
			return false;
		}
		float time = (float)atof(xml_attr->value());

		// 关键帧值
		xml_attr = xml_key_frame->first_attribute(XML_ATTR_VALUE);
		if (NULL == xml_attr)
		{
			CORE_TRACE("(KeyFrameController::InnerLoadBezierController) no value attribute.");
			Assert(0);
			return false;
		}
		float value = (float)atof(xml_attr->value());

		// 关键帧前控制点时间
		xml_attr = xml_key_frame->first_attribute(XML_ATTR_PREV_CP_TIME);
		if (NULL == xml_attr)
		{
			CORE_TRACE("(KeyFrameController::InnerLoadBezierController) no prev_cp_time attribute.");
			Assert(0);
			return false;
		}
		float prev_cp_time = (float)atof(xml_attr->value());

		// 关键帧前控制点值
		xml_attr = xml_key_frame->first_attribute(XML_ATTR_PREV_CP_VALUE);
		if (NULL == xml_attr)
		{
			CORE_TRACE("(KeyFrameController::InnerLoadBezierController) no prev_cp_value attribute.");
			Assert(0);
			return false;
		}
		float prev_cp_value = (float)atof(xml_attr->value());

		// 关键帧后控制点时间
		xml_attr = xml_key_frame->first_attribute(XML_ATTR_NEXT_CP_TIME);
		if (NULL == xml_attr)
		{
			CORE_TRACE("(KeyFrameController::InnerLoadBezierController) no next_cp_time attribute.");
			Assert(0);
			return false;
		}
		float next_cp_time = (float)atof(xml_attr->value());

		// 关键帧后控制点值
		xml_attr = xml_key_frame->first_attribute(XML_ATTR_NEXT_CP_VALUE);
		if (NULL == xml_attr)
		{
			CORE_TRACE("(KeyFrameController::InnerLoadBezierController) no next_cp_value attribute.");
			Assert(0);
			return false;
		}
		float next_cp_value = (float)atof(xml_attr->value());

		// 添加关键帧
		SetKeyFrameBezierMode(time, value, prev_cp_time, prev_cp_value, 
			next_cp_time, next_cp_value, false);

		xml_key_frame = xml_key_frame->next_sibling(XML_NODE_KEYFRAME);
	}

	return true;
}

bool KeyFrameController::InnerLoadLinearController(xml_node<>* xml_controller)
{
	xml_node<>* xml_key_frame = xml_controller->first_node(XML_NODE_KEYFRAME);
	// 没有关键帧
	if (NULL == xml_key_frame)
	{
		return true;
	}

	xml_attribute<>* xml_attr = NULL;
	while (xml_key_frame)
	{
		// 关键帧时间
		xml_attr = xml_key_frame->first_attribute(XML_ATTR_TIME);
		if (NULL == xml_attr)
		{
			CORE_TRACE("(KeyFrameController::InnerLoadLinearController) no time attribute.");
			Assert(0);
			return false;
		}
		float time = (float)atof(xml_attr->value());

		// 关键帧值
		xml_attr = xml_key_frame->first_attribute(XML_ATTR_VALUE);
		if (NULL == xml_attr)
		{
			CORE_TRACE("(KeyFrameController::InnerLoadLinearController) no value attribute.");
			Assert(0);
			return false;
		}
		float value = (float)atof(xml_attr->value());

		// 添加关键帧
		SetKeyFrameLinearMode(time, value, false);

		xml_key_frame = xml_key_frame->next_sibling(XML_NODE_KEYFRAME);
	}

	return true;
}

bool KeyFrameController::InnerLoadOnceController(xml_node<>* xml_controller)
{
	xml_node<>* xml_key_frame = xml_controller->first_node(XML_NODE_KEYFRAME);
	// 没有关键帧
	if (NULL == xml_key_frame)
	{
		return true;
	}

	xml_attribute<>* xml_attr = NULL;
	while (xml_key_frame)
	{
		// 关键帧时间
		xml_attr = xml_key_frame->first_attribute(XML_ATTR_TIME);
		if (NULL == xml_attr)
		{
			CORE_TRACE("(KeyFrameController::InnerLoadOnceController) no time attribute.");
			Assert(0);
			return false;
		}
		float time = (float)atof(xml_attr->value());

		// 关键帧值
		xml_attr = xml_key_frame->first_attribute(XML_ATTR_PARAM);
		if (NULL == xml_attr)
		{
			CORE_TRACE("(KeyFrameController::InnerLoadOnceController) no value attribute.");
			Assert(0);
			return false;
		}
		const char* param = xml_attr->value();

		// 添加关键帧
		SetKeyFrameOnceMode(time, param, false);

		xml_key_frame = xml_key_frame->next_sibling(XML_NODE_KEYFRAME);
	}

	return true;
}

bool KeyFrameController::InnerLoadScaleController(rapidxml::xml_node<>* xml_controller)
{
	xml_node<>* xml_key_frame = xml_controller->first_node(XML_NODE_KEYFRAME);
	// 没有关键帧
	if (NULL == xml_key_frame)
	{
		return true;
	}

	xml_attribute<>* xml_attr = NULL;
	while (xml_key_frame)
	{
		// 关键帧时间
		xml_attr = xml_key_frame->first_attribute(XML_ATTR_TIME);
		if (NULL == xml_attr)
		{
			CORE_TRACE("(KeyFrameController::InnerLoadScaleController) no time attribute.");
			Assert(0);
			return false;
		}
		float time = (float)atof(xml_attr->value());

		// 动作名
		xml_attr = xml_key_frame->first_attribute(XML_ATTR_PARAM);
		if (NULL == xml_attr)
		{
			CORE_TRACE("(KeyFrameController::InnerLoadScaleController) no value attribute.");
			Assert(0);
			return false;
		}
		const char* action_name = xml_attr->value();

		// 动画时长
		xml_attr = xml_key_frame->first_attribute(XML_ATTR_TIME_LENGTH);
		if (NULL == xml_attr)
		{
			CORE_TRACE("(KeyFrameController::InnerLoadScaleController) no time attribute.");
			Assert(0);
			return false;
		}
		float time_length = (float)atof(xml_attr->value());

		// 播放速度
		float speed = 1.0f;
		xml_attr = xml_key_frame->first_attribute(XML_ATTR_SPEED);
		if (xml_attr)
		{
			speed = (float)atof(xml_attr->value());
		}

		// 添加关键帧
		SetKeyFrameScaleMode(time, action_name, time_length, speed, false);

		xml_key_frame = xml_key_frame->next_sibling(XML_NODE_KEYFRAME);
	}

	return true;
}

// 创建所有关键点的左右控制点
int KeyFrameController::CreateAllControlPoints()
{
	if (CONTROLLER_USAGE_MODE_BEZIER != m_nUsageMode)
	{
		return 0;
	}

	for(int j = 1; j < (int)m_keyFrames.size(); ++j)
	{
		int ret = CreateOneControlPoint(j, false);
		if (ret == 0)
		{
			return 0;
		}
	}
	CreateOneControlPoint(0, false);

	//发送重新计算控制点消息
	fm_movie::TimeAxis_RunCallback(s_pTimeAxis, "on_key_update", CVarList()<<
		m_pObject->strName.c_str()<< m_nControllerType<< CHANGE_COMPUTE_CURVE);

	return 1;

}

void KeyFrameController::ChangeComputeTypeKey(float time, int type)
{
	if (CONTROLLER_USAGE_MODE_BEZIER != m_nUsageMode)
	{
		return;
	}

	int index = -1;
	sKeyBezier* pKey = (sKeyBezier*)FindKeyFrame(time, &index);
	if (NULL == pKey)
	{
		return;
	}

	if (type < SHOW_DOC || type > SHOW_EDITE2)
	{
		return;
	}

	pKey->mode = type;

	CreateOneControlPoint(index);

}

void KeyFrameController::MoveAllKeyTime(float time_add)
{
	int size = (int)m_keyFrames.size();
	if (CONTROLLER_USAGE_MODE_BEZIER != m_nUsageMode)
	{
		for (int i = 0; i< size; ++i)
		{
			m_keyFrames[i]->time += time_add;
		}
	}
	else
	{
		for (int i = 0; i< size; ++i)
		{
			sKeyBezier *pKey = (sKeyBezier*)m_keyFrames[i];

			pKey->time += time_add;
			pKey->left_time += time_add;
			pKey->right_time += time_add;
		}

	}

	//发送移动所有关键帧的时间消息
	fm_movie::TimeAxis_RunCallback(s_pTimeAxis, "on_key_update", CVarList()<<
		m_pObject->strName.c_str()<< m_nControllerType<< CHANGE_MOVE_ALL_KEY);

}

void KeyFrameController::MoveAllKeyValue(float value_add)
{
	if (CONTROLLER_USAGE_MODE_BEZIER != m_nUsageMode)
	{
		return;
	}

	int size = (int)m_keyFrames.size();
	for (int i = 0; i< size; ++i)
	{
		sKeyBezier *pKey = (sKeyBezier*)m_keyFrames[i];

		pKey->value += value_add;
		pKey->left_value += value_add;
		pKey->right_value += value_add;
	}

	//发送改变所有beizer关键帧的值消息
	fm_movie::TimeAxis_RunCallback(s_pTimeAxis, "on_key_update", CVarList()<<
		m_pObject->strName.c_str()<< m_nControllerType<< CHANGE_MOVE_ALL_VALUE);

}

void KeyFrameController::CopyKey(float time, float new_time)
{
	sKey* pOldKey = FindKeyFrame(time);
	if (NULL == pOldKey)
	{
		return;
	}

	// 查找是否存在该时刻的关键帧,有则修改,无则按次序添加
	int nIndex = -1;
	sKey* pNewKey = FindKeyFrame(new_time, &nIndex);
	if (NULL == pNewKey)
	{
		pNewKey = AddKey(new_time, nIndex);
		Assert(pNewKey);

	}

	pNewKey->Copy(pOldKey);
	pNewKey->time = new_time;

	if (CONTROLLER_USAGE_MODE_BEZIER == m_nUsageMode)
	{
		sKeyBezier *pNewKeyB = (sKeyBezier*)pNewKey;

		CreateOneControlPoint(nIndex-1);
		CreateOneControlPoint(nIndex);
		CreateOneControlPoint(nIndex+1);

	}

	//发送复制关键帧的消息
	fm_movie::TimeAxis_RunCallback(s_pTimeAxis, "on_key_update", CVarList()<<
		m_pObject->strName.c_str()<< m_nControllerType<< CHANGE_COPY<< time<< new_time);

}

sKeyBezier KeyFrameController::GetBezierKey(size_t index) const
{
	if (index < 0 || index >= m_keyFrames.size())
	{
		return sKeyBezier();
	}

	sKeyBezier &key = *((sKeyBezier*)m_keyFrames[index]);

	return key;

}

bool KeyFrameController::SetBezierKey(size_t index, const sKeyBezier& key)
{
	if (index < 0 || index >= m_keyFrames.size())
	{
		return false;
	}

	m_keyFrames[index]->Copy(&key);
	return true;
}

void KeyFrameController::MoveKey(float time, float new_time)
{
	if (fm_movie::FloatEqualLocal(time, new_time))
	{
		return;
	}

	int nIndex = -1;
	sKey* pOldKey = FindKeyFrame(time, &nIndex);
	if (NULL == pOldKey)
	{
		return;
	}
	m_keyFrames.remove(nIndex);

	// 查找是否存在该时刻的关键帧,有则修改,无则按次序添加
	
	sKey* pNewKey = FindKeyFrame(new_time, &nIndex);
	if (NULL != pNewKey)
	{
		m_keyFrames.remove(nIndex);
		DeleteKey(pNewKey);
	}

	pOldKey->time = new_time;
	m_keyFrames.insert(nIndex, pOldKey);

	if (CONTROLLER_USAGE_MODE_BEZIER == m_nUsageMode)
	{
		float time_add = new_time - time;
		sKeyBezier *pOldKeyB = (sKeyBezier*)pOldKey;

		pOldKeyB->left_time += time_add;
		pOldKeyB->right_time += time_add;

		CreateOneControlPoint(nIndex-1);
		CreateOneControlPoint(nIndex);
		CreateOneControlPoint(nIndex+1);
	}

	//发送移动某时刻关键帧消息
	fm_movie::TimeAxis_RunCallback(s_pTimeAxis, "on_key_update", CVarList()<<
		m_pObject->strName.c_str()<< m_nControllerType<< CHANGE_MOVE_KEY<< time<< new_time);

}

// 收集索引的位移关键帧的时刻
void KeyFrameController::CollectPosKeyTime(TArrayPod<float, 1> &array) const
{
	size_t clollect_count = array.size();
	size_t key_count = m_keyFrames.size();

	if (clollect_count < key_count)
	{
		array.resize(key_count);
		memset(array.data(), 0, sizeof(float)*key_count);
	}

	if (clollect_count == 0)
	{
		for (size_t i = 0; i < key_count; ++i)
		{
			array[i] = m_keyFrames[i]->time;
		}

		return;
	}

	for (size_t i = 0; i < key_count; ++i)
	{
		// 采用二叉树查找.
		int size = (int)array.size();
		float fTime = m_keyFrames[i]->time;
		int start = 0;
		int end = size - 1;
		int midle = 0;
		int InsertIndex = -1;
		bool bFind = false;
	
		while(true)
		{
			midle = (start + end)/2;
	
			if (fm_movie::FloatEqualLocal(fTime, array[midle]))
			{
				InsertIndex = midle;
				bFind = true;
				break;
			}
	
			// 查找到了最后,start跟end相等或者相邻.
			if (midle == start)
			{
				if (fm_movie::FloatEqualLocal(fTime, array[end]))
				{
					InsertIndex= end;
					bFind = true;
					break;
				}
	
				if (fTime < array[start])
				{
					InsertIndex = start;
				}
				else if (fTime < array[end])
				{
					InsertIndex = end;
				}
				else
				{
					InsertIndex = end + 1;				
				}
				break;
			}
	
			// 不等于中间的值, 向前或向后缩小查找范围
			if (fTime < array[midle])
			{
				end = midle;
			}
			else
			{
				start = midle;
			}
		}

		if (bFind)
		{
			continue;
		}

		array.insert(InsertIndex, fTime);
	}
}

// 创建某关键点的控制点
int KeyFrameController::CreateOneControlPoint(int index, bool send_msg)
{
	//sKeyBezier &m_keyFrames = m_keyFrames;
	int count = (int)m_keyFrames.size();
	if (index < 0 || index >= count )
	{
		return 0;
	}

	sKeyBezier* pKey = (sKeyBezier*)m_keyFrames[index];
	sKeyBezier* pKeyFront = NULL;
	sKeyBezier* pKeyNext = NULL;
	if (count == 1)
	{
		pKey->left_time = pKey->time;
		pKey->left_value = pKey->value;
		pKey->right_time = pKey->time;
		pKey->right_value = pKey->value;

		return 0;
	}

	float addLx, addRx, addLy, addRy;
	if (count == 2)
	{
		pKeyFront = (sKeyBezier*)m_keyFrames[0];
		pKeyNext = (sKeyBezier*)m_keyFrames[1];
		if (pKey->mode == SHOW_CURVE2)
		{
			pKey->kl = (pKeyFront->value - pKeyNext->value) / (pKeyFront->time - pKeyNext->time);
			if (index == 0)
			{
				// 计算出右控制点x增加
				addLx = (pKeyNext->time - pKeyFront->time) * S_BILI;
				// 计算出右控制点y增加
				addLy = addLx * pKeyFront->kl;
				pKeyFront->right_time = pKeyFront->time + addLx;
				pKeyFront->right_value = pKeyFront->value + addLy;
			}
			else
			{
				// 计算出左控制点x增加
				addRx = (pKeyFront->time - pKeyNext->time) * S_BILI;
				// 计算出左控制点y增加
				addRy = addRx * pKeyNext->kl;
				pKeyNext->left_time = pKeyNext->time + addRx;
				pKeyNext->left_value = pKeyNext->value + addRy;
			}

			return 0;
		}

	}

	if (index == 0)
	{
		pKeyNext = (sKeyBezier*)m_keyFrames[1];
		pKey->left_time = pKey->time;
		pKey->left_value = pKey->value;

		switch(pKey->mode)
		{
		case SHOW_CURVE1:
			{
				addRx = (pKeyNext->time - pKey->time) * S_BILI;
				pKey->right_time = pKey->time + addRx;
				pKey->right_value = pKey->value;
			}
			break;
		case SHOW_CURVE2:
			{
				//端点控制线与前点右控制线等角
                FmVec2 vKSP = pKey->GetSelfPoint();
                FmVec2 vKNSP = pKeyFront->GetSelfPoint();
				GetSymmetryPoint(*(FmVec2*)&pKey->right, vKSP, vKNSP, *(FmVec2*)&pKeyNext->left);
			}
			break;
		case SHOW_LINE:
			{
				addRx = (pKeyNext->time - pKey->time) * S_BILI;
				pKey->kr = (pKeyNext->value - pKey->value) / (pKeyNext->time - pKey->time);
				addRy = pKey->kr * addRx;

				pKey->right_time = pKey->time + addRx;
				pKey->right_value = pKey->value + addRy;
			}
			break;
		case SHOW_EDITE1:
			{
				addRx = (pKeyNext->time - pKey->time) * pKey->sr;
				addRy = pKey->kl * addRx;

				pKey->right_time = pKey->time + addRx;
				pKey->right_value = pKey->value + addRy;

			}
			break;
		case SHOW_EDITE2:
			{
				addRx = (pKeyNext->time - pKey->time) * pKey->sr;
				addRy = pKey->kr * addRx;

				pKey->right_time = pKey->time + addRx;
				pKey->right_value = pKey->value + addRy;

			}
			break;
		}

	}
	else if (index == count - 1)
	{
		pKeyFront = (sKeyBezier*)m_keyFrames[index-1];

		pKey->right_time = pKey->time;
		pKey->right_value = pKey->value;

		switch(pKey->mode)
		{
		case SHOW_CURVE1:
			{
				addLx = (pKeyFront->time - pKey->time) * S_BILI;
				pKey->left_time = pKey->time + addLx;
				pKey->left_value = pKey->value;

			}
			break;
		case SHOW_CURVE2:
			{
				//端点控制线与前点右控制线等角
                FmVec2 vKSP = pKey->GetSelfPoint();
                FmVec2 vKFSP = pKeyFront->GetSelfPoint();
				GetSymmetryPoint(*(FmVec2*)&pKey->left, vKSP, vKFSP, *(FmVec2*)&pKeyFront->right);
			}
			break;
		case SHOW_LINE:
			{
				addLx = (pKeyFront->time - pKey->time) * S_BILI;
				pKey->kl = (pKeyFront->value - pKey->value) / (pKeyFront->time - pKey->time);
				addLy = pKey->kl * addLx;

				pKey->left_time = pKey->time + addLx;
				pKey->left_value = pKey->value + addLy;
			}
			break;
		case SHOW_EDITE1:
		case SHOW_EDITE2:
			{
				addLx = (pKeyFront->time - pKey->time) * pKey->sl;
				addLy = pKey->kl * addLx;

				pKey->left_time = pKey->time + addLx;
				pKey->left_value = pKey->value + addLy;

			}
			break;
		}

	}
	else
	{
		pKeyFront = (sKeyBezier*)m_keyFrames[index-1];
		pKeyNext = (sKeyBezier*)m_keyFrames[index+1];
		ComputLeftAndRightPoint(*pKeyFront, *pKey, *pKeyNext);
	}

	return 1;

}

// 给定前后关键点,计算自己的左右控制点
int KeyFrameController::ComputLeftAndRightPoint(sKeyBezier &front, sKeyBezier &self, sKeyBezier &next)
{
	float addLx, addRx, addLy, addRy;
	float sk, k1, k2;
	FmVec2 p1 = front.GetSelfPoint();
	FmVec2 p = self.GetSelfPoint();
	FmVec2 p2 = next.GetSelfPoint();

	switch(self.mode)
	{
	case SHOW_DOC:
		{
			return 0;
		}
		break;
	case SHOW_CURVE1:
		{
			// 计算出前后控制点x增加
			addLx = (p1.x - p.x) * S_BILI;
			addRx = (p2.x - p.x) * S_BILI;

			// 控制线斜率计算
			if (FloatEqual(p1.y, p.y) || FloatEqual(p2.y, p.y))
			{
				// 如果前后点相对,则控制线水平
				self.kl = 0.f;
			}
			else
			{
				// 前后两点连线的平行线斜率
				self.kl = (p1.y - p2.y) / (p1.x - p2.x);
			}
			//self.kr = self.kl;

			// 计算出前后控制点y增加
			addLy = addLx * self.kl;
			addRy = addRx * self.kl;

			// 计算出前后控制点
			self.left_time = p.x + addLx;
			self.right_time = p.x + addRx;
			self.left_value = p.y + addLy;
			self.right_value = p.y + addRy;

			// 优化前后控制点, 左控制点超出前点值
			if((p.y < p1.y && self.left_value > p1.y) || (p.y > p1.y && self.left_value < p1.y))
			{
				self.left_value = p1.y;
				self.kl = (self.left_value - p.y) / (self.left_time - p.x);

				// 更新右控制点的值
				addRy = addRx * self.kl;
				self.right_value = p.y + addRy;
			}

			// 优化前后控制点, 右控制点超出后点值
			if((p.y < p2.y && self.right_value > p2.y) || (p.y > p2.y && self.right_value < p2.y))
			{
				self.right_value = p2.y;
				self.kl = (self.right_value - p.y) / (self.right_time - p.x);

				// 更新左控制点的值
				addLy = addLx * self.kl;
				self.left_value = p.y + addLy;
			}

		}
		break;
	case SHOW_CURVE2:
		{
			// 计算出前后控制点x增加
			addLx = (p1.x - p.x) * S_BILI;
			addRx = (p2.x - p.x) * S_BILI;

			// 控制线斜率计算
			// k1到k2斜率的线性变化
			sk = (p.x - p1.x) / (p2.x - p1.x);
			k1 = (p1.y - p.y) / (p1.x - p.x);
			k2 = (p2.y - p.y) / (p2.x - p.x);
			self.kl = k1 + (k2 - k1) * sk;
			//self.kr = self.kl;

			// 计算出前后控制点x增加
			addLy = addLx * self.kl;
			addRy = addRx * self.kl;

			// 计算出前后控制点
			self.left_time = p.x + addLx;
			self.right_time = p.x + addRx;
			self.left_value = p.y + addLy;
			self.right_value = p.y + addRy;


		}
		break;
	case SHOW_EDITE1:
		{
			// 计算出前后控制点x增加
			addLx = (p1.x - p.x) * self.sl;
			addRx = (p2.x - p.x) * self.sr;

			// 计算出前后控制点y增加
			addLy = addLx * self.kl;
			addRy = addRx * self.kl;

			// 计算出前后控制点
			self.left_time = p.x + addLx;
			self.right_time = p.x + addRx;
			self.left_value = p.y + addLy;
			self.right_value = p.y + addRy;
		}
		break;
	case SHOW_EDITE2:
		{
			// 计算出前后控制点x增加
			addLx = (p1.x - p.x) * self.sl;
			addRx = (p2.x - p.x) * self.sr;

			// 计算出前后控制点y增加
			addLy = addLx * self.kl;
			addRy = addRx * self.kr;

			// 计算出前后控制点
			self.left_time = p.x + addLx;
			self.right_time = p.x + addRx;
			self.left_value = p.y + addLy;
			self.right_value = p.y + addRy;
		}
		break;
	case SHOW_LINE:
		{
			// 计算出前后控制点x增加
			addLx = (p1.x - p.x) * S_BILI;
			addRx = (p2.x - p.x) * S_BILI;

			// 计算前后控制线的斜率
			self.kl = (p1.y - p.y) / (p1.x - p.x);
			self.kr = (p2.y - p.y) / (p2.x - p.x);

			// 计算出前后控制点y增加
			addLy = addLx * self.kl;
			addRy = addRx * self.kr;

			// 计算出前后控制点
			self.left_time = p.x + addLx;
			self.right_time = p.x + addRx;
			self.left_value = p.y + addLy;
			self.right_value = p.y + addRy;

		}
		break;
	}

	return 1;
}

// 由前一个关键点的控制点,求下一个关键点的控制点,(两个控制点关于两关键点中垂线对称)
void KeyFrameController::GetSymmetryPoint(FmVec2 &out, FmVec2 &A, FmVec2 &B, FmVec2 &C)
{
	float Rabc;
	float ab, bc;
	float xadd, yadd;


	if (fm_movie::FloatEqualLocal(B.x, A.x))
	{
		return;
	}


	//极端情况处理,三角函数
	if (B.y == A.y)
	{

		ab = sqrtf(powf((float)(A.x - B.x), 2) + powf((float)(A.y - B.y), 2));
		bc = sqrtf(powf((float)(C.x - B.x), 2) + powf((float)(C.y - B.y), 2));
		Rabc = acosf(((A.x - B.x) * (C.x - B.x) + (A.y - B.y) * (C.y - B.y)) / (ab * bc));

		xadd = B.x - C.x;
		if (C.y == B.y)
		{
			yadd = 0;
		}
		else if (C.y > B.y)
		{
			yadd = (tanf(Rabc) * xadd);
		}
		else
		{
			yadd = -(tanf(Rabc) * xadd);
		}

		out.x = A.x + xadd;
		out.y = A.y + yadd;
		return;
	}

	//一般情况下处理,直线方程求解
	float k = (float)(B.y - A.y) / (B.x - A.x);
	float k1 = (powf((float)B.x, 2) - powf((float)A.x, 2)) / (B.y - A.y) + A.y + B.y;

	float x = (k * C.x - C.y * 2 - 1.f / k * C.x + k1) / (k + 1.f / k);
	float y = k * x - k * C.x + C.y;

	out.x = x;
	out.y = y;


	////////////////////////////////////////////////////////////////////////////////////////
	float xadd_f, yadd_f;

	//极端情况处理,三角函数
	if (FloatEqual(B.y, A.y))
	{

		ab = sqrtf(powf((float)(A.x - B.x), 2) + powf((float)(A.y - B.y), 2));
		bc = sqrtf(powf((float)(C.x - B.x), 2) + powf((float)(C.y - B.y), 2));
		Rabc = acosf(((A.x - B.x) * (C.x - B.x) + (A.y - B.y) * (C.y - B.y)) / (ab * bc));

		xadd_f = B.x - C.x;
		if (FloatEqual(C.y, B.y))
		{
			yadd_f = 0;
		}
		else if (C.y > B.y)
		{
			yadd_f = (tanf(Rabc) * xadd_f);
		}
		else
		{
			yadd_f = -(tanf(Rabc) * xadd_f);
		}

		out.x = A.x + xadd_f;
		out.y = A.y + yadd_f;
		return;
	}

	//一般情况下处理,直线方程求解
	k = (float)(B.y - A.y) / (B.x - A.x);
	k1 = (powf((float)B.x, 2) - powf((float)A.x, 2)) / (B.y - A.y) + A.y + B.y;

	x = (k * C.x - C.y * 2 - 1.f / k * C.x + k1) / (k + 1.f / k);
	y = k * x - k * C.x + C.y;

	out.x = x;
	out.y = y;

	return;
}
