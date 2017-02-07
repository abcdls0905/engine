//--------------------------------------------------------------------
// 文件名:		design_saver.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _DESIGN_SAVER_H
#define _DESIGN_SAVER_H

#include "../public/module.h"

// 界面设计保存

class IControl;
class Designer;
class TiXmlDocument;
class TiXmlElement;

class CDesignSaver
{
public:
	CDesignSaver();
	~CDesignSaver();

	void SetDesigner(Designer* value);
	
	// 保存窗口到XML配置文件
	bool SaveForm(IControl* control, const char* path, const char* file, 
		const char* resource);
	// 保存资源集合文件
	bool SaveResource(const char* path, const char* file);

private:
	CDesignSaver(const CDesignSaver&);
	CDesignSaver& operator=(const CDesignSaver&);
	
	ICore* GetCore() const;
	
	// 保存附属控件
	bool SaveXmlSubor(IControl* parent, IControl* control, 
		const char* name, const char* file, TiXmlElement* root, bool is_utf8);
	// 保存控件
	bool SaveXmlControl(IControl* control, const char* file, 
		TiXmlElement* root, bool is_utf8);
	// 保存控件属性
	bool SaveXmlProperty(IControl* control, const char* name, 
		TiXmlElement* root, bool is_utf8);

private:
	Designer* m_pDesigner;
};

#endif // _DESIGN_SAVER_H

