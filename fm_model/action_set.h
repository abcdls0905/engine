//--------------------------------------------------------------------
// 文件名:		action_set.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年7月13日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _ACTION_SET_H
#define _ACTION_SET_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../visual/i_res_base.h"
#include "skeleton_data.h"

// 动作集

class IRender;
class IResLoader;
class CResManager;
struct action_set_t;

class CActionSet: public IResBase
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
		RES_CATEGORY_ACTION_SET,	// 动作集数据
	};
	
	long time;
public:
	static CActionSet* NewInstance();

public:
	CActionSet();
	virtual ~CActionSet();

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

	// 获得动作集数据
	action_set_t* GetActionSet() const { return m_pActionSet; }
	
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
	bool Build(action_set_t* pActionSet, skeleton_t* pMainSkeleton);

	// 根据名字获得动作的索引值
	int GetActionIndex(const char* action_name);
	// 添加动作名索引
	bool AddActionIndex(const char* action_name, int action_index);
	// 移除动作名索引
	bool RemoveActionIndex(const char* action_name, int action_index);

	// 移除动作的最后使用时间
	bool RemoveActionLastUse(int action_index);
	// 更新动作的最后使用时间
	bool UpdateActionLastUse(int action_index);
	// 回收长时间未使用的动作数据
	int CollectUnuseAction(unsigned int cur_tick, unsigned int ms);

	// 设置管理器
	void SetManager(CResManager* value) { m_pManager = value; }
	// 获得管理器
	CResManager* GetManager() const { return m_pManager; }
	
	// 设置附加资源路径名
	void SetAppendPath(const char* value) { m_strAppendPath = value; }
	const char* GetAppendPath() const { return m_strAppendPath.c_str(); }

private:
	CActionSet(const CActionSet&);
	CActionSet& operator=(const CActionSet&);

private:
	CResManager* m_pManager;
	IResLoader* m_pLoader;
	core_string m_strAppendPath;
	core_string m_strName;
	int m_nState;
	action_set_t* m_pActionSet;
};

#endif // _ACTION_SET_H
