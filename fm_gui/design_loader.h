//--------------------------------------------------------------------
// 文件名:		design_loader.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _DESIGN_LOADER_H
#define _DESIGN_LOADER_H

#include "../public/module.h"
#include "../utils/string_pod.h"
#include <map>
#include <vector>

// 界面设计加载
class Designer;

class CDesignLoader
{
public:
	CDesignLoader();
	~CDesignLoader();

	void SetDesigner(Designer* value);
	// 加载主窗口配置文件
	bool LoadDesktop(const char* path, const char* file, bool async_load);
	// 加载窗口配置文件
	PERSISTID LoadForm(const char* path, const char* file, bool async_load, bool bStatic=false);
	// 加载只包含资源的配置文件
	bool LoadResource(const char* path, const char* file);
	// 加载指定类型的资源配置文件
	bool LoadXmlResFile(int type, const char* path, const char* file);

private:
	CDesignLoader(const CDesignLoader&);
	CDesignLoader& operator=(const CDesignLoader&);

	bool LoadXmlRes(int type, const char* path, const char* file);
	

private:
	Designer* m_pDesigner;

	//
	TStringPod<char, size_t> m_mapIniFile;
	std::vector<char*> m_vecFileInf;
};

#endif // _DESIGN_LOADER_H

