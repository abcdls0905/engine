//--------------------------------------------------------------------
// 文件名:		design_res.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月21日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _DESIGN_RES_H
#define _DESIGN_RES_H

#include "../public/macros.h"
#include "../public/i_var_list.h"
#include "../public/core_mem.h"
#include "../utils/string_pod.h"

// 界面设计资源

class CDesignRes
{
public:
	enum RESOURCE_TYPES
	{
		//RESTYPE_COLOR,		// 颜色
		//RESTYPE_IMAGE,		// 图元
		RESTYPE_FONT,		// 字体
		RESTYPE_CURSOR,		// 鼠标形状
        RESTYPE_SOUND,      // 界面音效
		RESTYPE_MAX,
	};

private:
	typedef TStringPod<char, int, TStringTraitsCI<char>, 
		TCoreAlloc> collect_container_t;
	typedef TStringPod<char, int, TStringTraitsCI<char>, 
		TCoreAlloc> name_container_t;

public:
	CDesignRes();
	~CDesignRes();

	// 资源集合文件
	bool FindCollect(const char* file) const;
	bool AddCollect(const char* file);
	bool ClearCollect();

	// 获得资源文件数量
	size_t GetResFileCount(size_t type) const;
	// 获得资源文件名
	void GetResFileList(size_t type, IVarList& result);
	// 查找资源文件
	bool FindResFile(size_t type, const char* file) const;
	// 添加资源文件
	bool AddResFile(size_t type, const char* file);
	// 删除资源文件（资源文件的内容不会被移除）
	bool RemoveResFile(size_t type, const char* file);
	// 清除资源文件
	bool ClearResFile(size_t type);

	// 获得对应的类型
	size_t GetResType(const char* res_name) const;
	// 获得类型名称
	const char* GetResName(size_t type) const;

private:
	CDesignRes(const CDesignRes&);
	CDesignRes& operator=(const CDesignRes&);

private:
	collect_container_t m_Collects;
	name_container_t m_Resources[RESTYPE_MAX];
};

#endif // _DESIGN_RES_H

