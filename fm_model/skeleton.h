//--------------------------------------------------------------------
// 文件名:		skeleton.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年7月12日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SKELETON_H
#define _SKELETON_H

#include "../public/macros.h"
#include "../public/core_type.h"

#include "../visual/i_res_base.h"
#include "../visual/i_model_system.h"
#include "skeleton_data.h"

// 骨骼动画

class IRender;
class IResLoader;
class CResManager;

class CSkeleton: public IResBase
{
public:
	enum STATE_ENUM
	{
		STATE_INIT,		// 初始状态
		STATE_LOADING,	// 正在加载
		STATE_READY,	// 就绪
		STATE_FAILED,	// 加载失败
	};

	enum
	{
		RES_CATEGORY_SKELETON,	// 模型数据
	};
	
	int time;
public:
	static CSkeleton* NewInstance();

public:
	CSkeleton();
	virtual ~CSkeleton();

	// 释放
	virtual void Release();

	// 销毁
	void Destroy();

	// 是否就绪
	bool IsReady()
	{
		return (m_nState == STATE_READY);
	}

	// 是否加载结束（成功或失败）
	bool IsLoadComplete()
	{
		return (m_nState == STATE_READY) || (m_nState == STATE_FAILED);
	}

	// 获得骨骼动作数据
	skeleton_t* GetSkeletonData() const { return m_pSkeleton; }
	
	// 设置名称
	void SetName(const char* value)
	{ 
		Assert(value != NULL);

		m_strName = value; 
	}

	// 获得名称
	const char* GetName() const
	{
		return m_strName.c_str();
	}

	// 状态
	void SetState(int value) { m_nState = value; }
	int GetState() const { return m_nState; }

	// 同步加载
	bool Create();
	// 是否可以异步加载
	bool CanAsync();
	// 开始异步创建 
	bool BeginCreate();
	// 结束异步创建 
	bool EndCreate(int category, bool succeed);
	// 获得异步加载器
	IResLoader* GetLoader();

	// 创建
	bool Build(skeleton_t* pSkeleton);

	// 设置管理器
	void SetManager(CResManager* value) { m_pManager = value; }
	
private:
	CSkeleton(const CSkeleton&);
	CSkeleton& operator=(const CSkeleton&);

private:
	CResManager* m_pManager;
	IResLoader* m_pLoader;
	core_string m_strName;
	int m_nState;
	skeleton_t* m_pSkeleton;
};

#endif // _SKELETON_H
