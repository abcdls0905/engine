//--------------------------------------------------------------------
// 文件名:		i_ini_file.h
// 内  容:		
// 说  明:		
// 创建日期:	2006年1月30日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_I_INI_FILE_H
#define _PUBLIC_I_INI_FILE_H

#include "macros.h"
#include "i_var_list.h"

// 读写配置文件接口

class IIniFile
{
public:
	enum { READ_RAW_FILE = 1 };

public:
	virtual ~IIniFile() = 0;
	
	// 释放
	virtual void Release() = 0;

	// 设置文件名
	virtual void SetFileName(const char* filename) = 0;
	// 获得文件名
	virtual const char* GetFileName() const = 0;
	// 加载文件
	virtual bool LoadFromFile() = 0;
	// 解析字符串
	virtual bool LoadFromString(const char* str, size_t size) = 0;
	// 保存文件
	virtual bool SaveToFile() const = 0;
	// 创建哈希索引
	virtual bool CreateHashIndex() = 0;

	// 添加段
	virtual bool AddSection(const char* section) = 0;
	// 查找段
	virtual bool FindSection(const char* section) const = 0;
	// 查找段的索引值
	virtual bool FindSectionIndex(const char* section, 
		size_t& index) const = 0;
	// 查找指定段中的键索引值
	virtual bool FindSectionItemIndex(size_t sect_index, const char* key, 
		size_t& item_index) const = 0;
	// 删除段
	virtual bool DeleteSection(const char* section) = 0;
	// 获得段的数量
	virtual size_t GetSectionCount() const = 0;
	// 获得指定段名
	virtual const char* GetSectionByIndex(size_t index) const = 0;
	// 获得段名列表
	virtual size_t GetSectionList(IVarList& result) const = 0;
	// 获得指定段下的键数量
	virtual size_t GetSectionItemCount(size_t sect_index) const = 0;
	// 获得指定段下的指定键名
	virtual const char* GetSectionItemKey(size_t sect_index, 
		size_t item_index) const = 0;
	// 获得指定段下的指定键值
	virtual const char* GetSectionItemValue(size_t sect_index, 
		size_t item_index) const = 0;

	// 查找段下的键
	virtual bool FindItem(const char* section, const char* key) const = 0;
	// 删除键
	virtual bool DeleteItem(const char* section, const char* key) = 0;
	// 获得段下所有键的数量
	virtual size_t GetItemCount(const char* section) const = 0;
	// 获得段下所有键名列表
	virtual size_t GetItemList(const char* section, 
		IVarList& result) const = 0;
	// 获得段下所有指定键的值列表
	virtual size_t GetItemValueList(const char* section, const char* key,
		IVarList& result) const = 0;

	// 读取键数据，当键不存在的时候返回缺省值(def)
	virtual int ReadInteger(const char* section, const char* key, 
		int def) const = 0;
	virtual char* ReadString(const char* section, const char* key, char* value, 
		size_t maxlen) const = 0;
	virtual const char* ReadString(const char* section, const char* key, 
		const char* def) const = 0;
	virtual float ReadFloat(const char* section, const char* key, 
		float def) const = 0;

	// 读取键数据，当键不存在的时候返回缺省值(def)
	virtual int ReadInteger(size_t sect_index, const char* key, 
		int def) const = 0;
	virtual char* ReadString(size_t sect_index, const char* key, char* value, 
		size_t maxlen) const = 0;
	virtual const char* ReadString(size_t sect_index, const char* key, 
		const char* def) const = 0;
	virtual float ReadFloat(size_t sect_index, const char* key, 
		float def) const = 0;
	
	// 改写键数据（如果段或键不存在则添加）
	virtual bool WriteInteger(const char* section, const char* key, 
		int value) = 0;
	virtual bool WriteString(const char* section, const char* key, 
		const char* value) = 0;
	virtual bool WriteFloat(const char* section, const char* key, 
		float value) = 0;

	// 添加有可能重复的键数据（如果段不存在则添加）
	virtual bool AddInteger(const char* section, const char* key, 
		int value) = 0;
	virtual bool AddString(const char* section, const char* key, 
		const char* value) = 0;
	virtual bool AddFloat(const char* section, const char* key, 
		float value) = 0;
};

inline IIniFile::~IIniFile() {}

#endif // _PUBLIC_I_INI_FILE_H
