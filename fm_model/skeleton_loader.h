//--------------------------------------------------------------------
// 文件名:		skeleton_loader.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年7月12日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SKELETON_LOADER_H
#define _SKELETON_LOADER_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"

#include "../visual/i_res_loader.h"
#include "skeleton_data.h"

// 骨骼动画加载器

class CSkeleton;

class CSkeletonLoader: public IResLoader
{
public:
	static CSkeletonLoader* NewInstance();

public:
	CSkeletonLoader();
	virtual ~CSkeletonLoader();

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
	void SetOwner(CSkeleton* pOwner);
	// 模型文件名
	void SetFileName(const char* file_name);
	
private:
	CSkeletonLoader(const CSkeletonLoader&);
	CSkeletonLoader& operator=(const CSkeletonLoader&);

private:
	CSkeleton* m_pOwner;
	core_string m_strFileName;
	skeleton_t* m_pSkeleton;
};

#endif // _SKELETON_LOADER_H
