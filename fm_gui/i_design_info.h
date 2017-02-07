//--------------------------------------------------------------------
// 文件名:		i_design_info.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月15日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _I_DESIGN_INFO_H
#define _I_DESIGN_INFO_H

#include "../public/macros.h"
#include "../public/i_var.h"
#include "../public/i_var_list.h"

// 设计信息接口

class IDesignInfo
{
public:
	// 可编辑属性类型
	enum PROPINFO_TYPES
	{
		TYPE_UNKNOWN,
		TYPE_BOOL,
		TYPE_INT,
		TYPE_INT64,
		TYPE_FLOAT,
		TYPE_DOUBLE,
		TYPE_STRING,
		TYPE_WIDESTR,
		TYPE_FONT,		// 字体资源
		TYPE_COLOR,		// 颜色资源
		TYPE_IMAGE,		// 图元	
		TYPE_CURSOR,	// 鼠标外观
        TYPE_SOUND,     // 界面音效
		TYPE_FILE,		// 文件
	};

public:
	virtual ~IDesignInfo() = 0;

	// 查找属性信息
	virtual bool FindPropInfo(const char* name) const = 0;
	// 添加属性信息
	virtual bool AddPropInfo(const char* name, int type, 
		bool save, bool child_save, const IVar* def_value = NULL) = 0;
	// 删除属性信息
	virtual bool RemovePropInfo(const char* name) = 0;
	// 获得属性数量
	virtual size_t GetPropCount() const = 0;
	// 获得属性名称列表
	virtual size_t GetPropList(IVarList& result) const = 0;
	// 获得属性名
	virtual const char* GetPropName(size_t index) const = 0;
	// 获得属性类型
	virtual int GetPropType(const char* name) const = 0;
	// 获得属性是否保存
	virtual bool GetPropSave(const char* name) const = 0;
	// 获得作为附属控件时属性是否保存
	virtual bool GetPropSuborSave(const char* name) const = 0; 
	// 获得属性的缺省值（如果当前值和缺省值相同则不需要保存）
	virtual bool GetPropDefValue(const char* name, 
		IVar& def_value) const = 0;
	// 获得属性类型的字符串表示
	virtual const char* GetPropTypeString(int type) const = 0;

	// 查找附属控件信息
	virtual bool FindSuborInfo(const char* name) const = 0;
	// 添加附属控件信息
	virtual bool AddSuborInfo(const char* name, const char* type) = 0;
	// 删除附属控件信息
	virtual bool RemoveSuborInfo(const char* name) = 0;
	// 获得附属控件数量
	virtual size_t GetSuborCount() const = 0;
	// 获得附属控件名称列表
	virtual size_t GetSuborList(IVarList& result) const = 0;
	// 获得附属控件实体类型
	virtual const char* GetSuborType(const char* name) const = 0;

	// 查找事件信息
	virtual bool FindEventInfo(const char* name) const = 0;
	// 添加事件信息
	virtual bool AddEventInfo(const char* name) = 0;
	// 删除事件信息
	virtual bool RemoveEventInfo(const char* name) = 0;
	// 获得事件数量
	virtual size_t GetEventCount() const = 0;
	// 获得事件名称列表
	virtual size_t GetEventList(IVarList& result) const = 0;
};

inline IDesignInfo::~IDesignInfo() {}

#endif // _I_DESIGN_INFO_H
