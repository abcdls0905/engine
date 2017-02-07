//--------------------------------------------------------------------
// 文件名:		material_loader.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年3月8日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _MATERIAL_LOADER_H
#define _MATERIAL_LOADER_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"

#include "../visual/i_res_loader.h"

// 模型材质加载器

class IIniFile;
class CModelPlayer;
class IRender;

class CMaterialLoader: public IResLoader
{
public:
	static CMaterialLoader* NewInstance();

public:
	CMaterialLoader();
	virtual ~CMaterialLoader();

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
	void SetOwner(CModelPlayer* pOwner);
	// 贴图文件名
	void SetFileName(const char* file_name);
	// 模型材质数量
	void SetMaterialCount(size_t value);
	
private:
	CMaterialLoader(const CMaterialLoader&);
	CMaterialLoader& operator=(const CMaterialLoader&);
	
private:
	CModelPlayer* m_pOwner;
	core_string m_strFileName;
	IIniFile* m_pIniFile;
};

#endif // _MATERIAL_LOADER_H
