//--------------------------------------------------------------------
// 文件名:		ini_file.h
// 内  容:		
// 说  明:		
// 创建日期:	2006年1月30日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _INI_FILE_H
#define _INI_FILE_H

#include "../public/macros.h"
#include "../public/i_ini_file.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../utils/array_pod.h"

class TIniFileAlloc
{
public:
	TIniFileAlloc() {}
	~TIniFileAlloc() {}

	// 申请内存
	void* Alloc(size_t size) 
	{ 
		return CORE_ALLOC(size); 
	}
	// 释放内存
	void Free(void* ptr, size_t size) 
	{ 
		CORE_FREE(ptr, size); 
	}
	// 交换
	void Swap(TIniFileAlloc& src) {}
};

// 读写配置文件

class CIniFile: public IIniFile
{
private:
	struct item_t
	{
		size_t nNamePos;
		unsigned int nHash;
		size_t nValuePos;
	};

	struct section_t
	{
		size_t nNamePos;
		unsigned int nHash;
		size_t nItemStart;
		size_t nItemCount;
		section_t* pHashNext;
	};

public:
	static CIniFile* NewInstance(const char* filename, int read_raw_file);

public:
	CIniFile();
	CIniFile(const char* filename, int read_raw_file);
	virtual ~CIniFile();
	
	// 释放
	virtual void Release();

	// 设置文件名
	virtual void SetFileName(const char* filename);
	// 获得文件名
	virtual const char* GetFileName() const;
	// 加载文件
	virtual bool LoadFromFile();
	// 解析字符串
	virtual bool LoadFromString(const char* str, size_t size);
	// 保存文件
	virtual bool SaveToFile() const;
	// 创建哈希索引
	virtual bool CreateHashIndex();

	// 添加段
	virtual bool AddSection(const char* section);
	// 查找段
	virtual bool FindSection(const char* section) const;
	// 查找段的索引值
	virtual bool FindSectionIndex(const char* section, size_t& index) const;
	// 查找指定段中的键索引值
	virtual bool FindSectionItemIndex(size_t sect_index, const char* key, 
		size_t& item_index) const;
	// 删除段
	virtual bool DeleteSection(const char* section);
	// 获得段的数量
	virtual size_t GetSectionCount() const;
	// 获得指定段名
	virtual const char* GetSectionByIndex(size_t index) const;
	// 获得段名列表
	virtual size_t GetSectionList(IVarList& result) const;
	// 获得指定段下的键数量
	virtual size_t GetSectionItemCount(size_t sect_index) const;
	// 获得指定段下的指定键名
	virtual const char* GetSectionItemKey(size_t sect_index, 
		size_t item_index) const;
	// 获得指定段下的指定键值
	virtual const char* GetSectionItemValue(size_t sect_index, 
		size_t item_index) const;

	// 查找段下的键
	virtual bool FindItem(const char* section, const char* key) const;
	// 删除键
	virtual bool DeleteItem(const char* section, const char* key);
	// 获得段下所有键的数量
	virtual size_t GetItemCount(const char* section) const;
	// 获得段下所有键名列表
	virtual size_t GetItemList(const char* section, IVarList& result) const;
	// 获得段下所有指定键的值列表
	virtual size_t GetItemValueList(const char* section, const char* key,
		IVarList& result) const;

	// 读取键数据，当键不存在的时候返回缺省值(def)
	virtual int ReadInteger(const char* section, const char* key, 
		int def) const;
	virtual char* ReadString(const char* section, const char* key, 
		char* value, size_t maxlen) const;
	virtual const char* ReadString(const char* section, const char* key, 
		const char* def) const;
	virtual float ReadFloat(const char* section, const char* key, 
		float def) const;

	// 读取键数据，当键不存在的时候返回缺省值(def)
	virtual int ReadInteger(size_t sect_index, const char* key, 
		int def) const;
	virtual char* ReadString(size_t sect_index, const char* key, char* value, 
		size_t maxlen) const;
	virtual const char* ReadString(size_t sect_index, const char* key, 
		const char* def) const;
	virtual float ReadFloat(size_t sect_index, const char* key, 
		float def) const;

	// 改写键数据（如果段或键不存在则添加）
	virtual bool WriteInteger(const char* section, const char* key, int value);
	virtual bool WriteString(const char* section, const char* key, 
		const char* value);
	virtual bool WriteFloat(const char* section, const char* key, float value);

	// 添加有可能重复的键数据（如果段不存在则添加）
	virtual bool AddInteger(const char* section, const char* key, int value);
	virtual bool AddString(const char* section, const char* key, 
		const char* value);
	virtual bool AddFloat(const char* section, const char* key, float value);

private:
	bool Build(char* buffer, size_t buffer_size);
	size_t AddToBuffer(const char* val);
	bool SetData(const char* section, const char* key, const char* data);
	bool AddData(const char* section, const char* key, const char* data);
	const char* GetData(const char* section, const char* key) const;
	const char* GetData2(size_t sect_index, const char* key) const;
	const CIniFile::section_t* GetSection(const char* name) const;
	const CIniFile::item_t* GetItem(const section_t* pSection, 
		const char* name) const;

private:
	TIniFileAlloc m_Alloc;
	TFastStr<char, 16, TCharTraits<char>, TIniFileAlloc> m_strFileName;
	int m_nReadRawFile;
	char* m_pBuffer;
	size_t m_nLength;
	TArrayPod<section_t, 8, TIniFileAlloc> m_Sections;
	TArrayPod<item_t, 8, TIniFileAlloc> m_Items;
	section_t** m_pBuckets;
	size_t m_nBucketNum;
};

#endif // _INI_FILE_H
