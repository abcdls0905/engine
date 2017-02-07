//--------------------------------------------------------------------
// 文件名:		design_info.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _DESIGN_INFO_H
#define _DESIGN_INFO_H

#include "../public/macros.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "i_design_info.h"

// 设计器信息

class CDesignInfo: public IDesignInfo
{
private:
	struct prop_info_t
	{
		core_string strName;
		unsigned int nHash;
		int nType;
		bool bSave;
		bool bSuborSave;
		CVar DefValue;
	};

	struct subor_info_t
	{
		core_string strName;
		unsigned int nHash;
		core_string strType;
	};

	struct event_info_t
	{
		core_string strName;
		unsigned int nHash;
	};

public:
	CDesignInfo();
	virtual ~CDesignInfo();

	// 查找属性信息
	virtual bool FindPropInfo(const char* name) const;
	// 添加属性信息
	virtual bool AddPropInfo(const char* name, int type, 
		bool save, bool child_save, const IVar* def_value = NULL);
	// 删除属性信息
	virtual bool RemovePropInfo(const char* name);
	// 获得属性数量
	virtual size_t GetPropCount() const;
	// 获得属性名称列表
	virtual size_t GetPropList(IVarList& result) const;
	// 获得属性名
	virtual const char* GetPropName(size_t index) const;
	// 获得属性类型
	virtual int GetPropType(const char* name) const;
	// 获得属性是否保存
	virtual bool GetPropSave(const char* name) const;
	// 获得作为附属控件时属性是否保存
	virtual bool GetPropSuborSave(const char* name) const; 
	// 获得属性的缺省值（如果当前值和缺省值相同则不需要保存）
	virtual bool GetPropDefValue(const char* name, 
		IVar& def_value) const;
	// 获得属性类型的字符串表示
	virtual const char* GetPropTypeString(int type) const;
	
	// 查找附属控件信息
	virtual bool FindSuborInfo(const char* name) const;
	// 添加附属控件信息
	virtual bool AddSuborInfo(const char* name, const char* type);
	// 删除附属控件信息
	virtual bool RemoveSuborInfo(const char* name);
	// 获得附属控件数量
	virtual size_t GetSuborCount() const;
	// 获得附属控件名称列表
	virtual size_t GetSuborList(IVarList& result) const;
	// 获得附属控件实体类型
	virtual const char* GetSuborType(const char* name) const;
	
	// 查找事件信息
	virtual bool FindEventInfo(const char* name) const;
	// 添加事件信息
	virtual bool AddEventInfo(const char* name);
	// 删除事件信息
	virtual bool RemoveEventInfo(const char* name);
	// 获得事件数量
	virtual size_t GetEventCount() const;
	// 获得事件名称列表
	virtual size_t GetEventList(IVarList& result) const;

private:
	CDesignInfo(const CDesignInfo&);
	CDesignInfo& operator=(const CDesignInfo&);

	bool FindPropInfoIndex(const char* name, size_t& index) const;
	bool FindSuborInfoIndex(const char* name, size_t& index) const;
	bool FindEventInfoIndex(const char* name, size_t& index) const;
	
private:
	TArrayPod<prop_info_t*, 1, TCoreAlloc> m_PropInfo;
	TArrayPod<subor_info_t*, 1, TCoreAlloc> m_SuborInfo;
	TArrayPod<event_info_t*, 1, TCoreAlloc> m_EventInfo;
};

#endif // _DESIGNINFO_H

