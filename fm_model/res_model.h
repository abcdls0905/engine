//--------------------------------------------------------------------
// 文件名:		res_model.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年5月4日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _RESMODEL_H
#define _RESMODEL_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"

#include "../visual/i_res_base.h"
#include "../visual/i_model_system.h"
#include "model_data.h"

// 模型

class IRender;
class IResLoader;
class CResManager;

class CResModel: public IResBase
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
		RES_CATEGORY_MODEL,	// 模型数据
	};
	
	float time;
public:
	static CResModel* NewInstance();

public:
	CResModel();
	virtual ~CResModel();

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

	// 获得模型
	model_t* GetModelData() const { return m_pModel; }
	
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

	// 贴图路径
	void SetTexPaths(const char* value);
	const char* GetTexPaths() const;

	// 所有贴图是否加载完毕
	bool GetTextureLoadComplete() const;
	// 释放所有贴图
	void ReleaseTextures();

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
	bool Build(model_t* pModel);

	// 设置管理器
	void SetManager(CResManager* value) { m_pManager = value; }
	
private:
	CResModel(const CResModel&);
	CResModel& operator=(const CResModel&);

private:
	CResManager* m_pManager;
	core_string m_strTexPaths;
	core_string m_strName;
	int m_nState;
	IResLoader* m_pLoader;
	model_t* m_pModel;
};

#endif // _RESMODEL_H
