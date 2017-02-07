//--------------------------------------------------------------------
// 文件名:		BrokenShadow.h
// 内  容:		
// 说  明:		
// 创建日期:	2011年9月13日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _BROKEN_SHADOW_H
#define _BROKEN_SHADOW_H

#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../utils/array_pod.h"
#include "../visual/i_model_system.h"
#include "../visual/i_model_player.h"
#include "../visual/i_vis_base.h"

// 残影效果

class BrokenShadow: public IVisBase
{
private:
	struct child_t 
	{
		PERSISTID id;
		skt_pose_t* pose;
	};

	struct bs_info_t
	{
		FmMat4 mat;
		float alpha;
		TArrayPod<child_t, 1, TCoreAlloc> child;
	};

	enum ObiectType
	{
		OBJECT_TYPE_INVALID = 0,
		OBJECT_TYPE_MODEL = 1,
		OBJECT_TYPE_ACTOR = 2,
		OBJECT_TYPE_EFFECTMODEL = 3,
	};

public:
	BrokenShadow();
	virtual ~BrokenShadow();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual void Update(float seconds);
	virtual void Realize();

	// 产生残影的对象
	void SetObject(const PERSISTID id);
	PERSISTID GetObjectID() const;

	// 产生一个残影的间隔时间
	void SetInterval(float value);
	float GetInterval() const;

	// 残影生命周期
	void SetLifeTime(float value);
	float GetLifeTime() const;

	// 残影数量
	void SetShadowCount(int value);
	int GetShadowCount();

	// 颜色
	void SetColorString(const char* value);
	result_string GetColorString() const;

	// 是否播放结束
	bool GetComplete() const { return m_bComplete; }

private:
	void AddNode(TArrayPod<child_t, 1, TCoreAlloc>& info, IVisBase* pVisBase);
	void ClearData();
	void ClearData(size_t index);

	void ClearBSInfo()
	{
		for (size_t i = 0; i < m_vBSInfo.size(); ++i)
		{
			CORE_DELETE(m_vBSInfo[i]);
		}

		m_vBSInfo.clear();
	}

private:
	PERSISTID m_ObjectID;				// 产生残影的对象
	ObiectType m_ObjectType;			// 残影对象类型
	TArrayPod<bs_info_t*, 1, TCoreAlloc> m_vBSInfo;	// 残影信息容器
	float m_fInterval;					// 产生一次残影的时间间隔
	float m_fLastCreateTime;			// 上一次创建残影经过的时间
	float m_fLifeTime;					// 残影生命周期
	int m_nShadowCount;					// 残影数
	int m_nMaxShadowCount;				// 同时存在的最大残影数
	unsigned int m_nColor;				// 颜色
	bool m_bComplete;					// 是否播放结束
};

#endif // _BROKEN_SHADOW_H
