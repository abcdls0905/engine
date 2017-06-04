
#ifndef _MODEL_LOADER_H
#define _MODEL_LOADER_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"

#include "../visual/i_res_loader.h"
#include "model_data.h"

// 模型加载器

class CResModel;

class CModelLoader: public IResLoader
{
public:
	static CModelLoader* NewInstance();

public:
	CModelLoader();
	virtual ~CModelLoader();

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
	void SetOwner(CResModel* pOwner);
	// 模型文件名
	void SetFileName(const char* file_name);
	
private:
	CModelLoader(const CModelLoader&);
	CModelLoader& operator=(const CModelLoader&);

private:
	CResModel* m_pOwner;
	core_string m_strFileName;
	model_t* m_pModel;
};

#endif // _MODEL_LOADER_H
