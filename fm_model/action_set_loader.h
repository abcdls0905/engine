//--------------------------------------------------------------------
// 文件名:		action_set_loader.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年7月13日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _ACTION_SET_LOADER_H
#define _ACTION_SET_LOADER_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../visual/i_res_loader.h"
#include "skeleton_data.h"

// 动作集加载器

class CActionSet;
struct action_set_t;

class CActionSetLoader: public IResLoader
{
public:
	static CActionSetLoader* NewInstance();

public:
	CActionSetLoader();
	virtual ~CActionSetLoader();

	// 释放
	virtual void Release();
	// 获得名字
	virtual const char* GetName();
	// 加载
	virtual bool Load(bool async);
	// 创建
	virtual bool Create(bool async);
	// 结束异步创建 
	virtual bool EndCreate(int category, bool succeed);

	// 资源主体
	void SetOwner(CActionSet* pOwner);
	// 模型文件名
	void SetFileName(const char* file_name);
	// 设置附加资源路径名
	void SetAppendPath(const char* append_path);
	
private:
	CActionSetLoader(const CActionSetLoader&);
	CActionSetLoader& operator=(const CActionSetLoader&);

private:
	CActionSet* m_pOwner;
	core_string m_strFileName;
	core_string m_strAppendPath;
	action_set_t* m_pActionSet;
	skeleton_t* m_pMainSkeleton;
};

#endif // _ACTION_SET_LOADER_H
