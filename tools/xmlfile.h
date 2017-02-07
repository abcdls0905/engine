//--------------------------------------------------------------------
// 文件名:		XmlFile.h
// 内  容:		
// 说  明:		
// 创建日期:	2011年1月4日
// 创建人:		李超
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef XML_FILE_H
#define XML_FILE_H

#include "../public/macros.h"
#include "../extern/rapidxml/rapidxml.hpp"
//#include "../extern/rapidxml/rapidxml_iterators.hpp"
#include "../extern/rapidxml/rapidxml_print.hpp"
#include "../extern/rapidxml/rapidxml_utils.hpp"
#include "../public/var_list.h"
#include "../tools/util_func.h"

#include <string>
#include <vector>
#include <map>



#define  _XMLREAD_  
using namespace std;

template <typename T>
T ConvertFormString(const char* n);

template <>
inline std::string ConvertFormString(const char* n)
{
	return n;
}

template <>
inline int ConvertFormString(const char* n)
{
	return ::atoi(n);
}

template <>
inline float ConvertFormString(const char* n)
{
	return (float)::atof(n);
}

template <>
inline double ConvertFormString(const char* n)
{
	return ::atof(n);
}

class CXmlFile
{
public:
	CXmlFile();
	CXmlFile(const char* filename);
	virtual ~CXmlFile();

	// 初始化（可创建其他附属实体）
	virtual bool Init(const IVarList& args) { return true; }
	// 关闭（可删除其他引用的附属实体，而资源释放应在析构函数中进行）
	virtual bool Shut() { return true; }

	//释放所有资源
	virtual void Release();
	
	// 设置文件名
	void SetFileName(const char* filename);

	// 获得文件名
	const char* GetFileName() const;

    // 加载文件
    bool LoadFromFile();
	bool LoadFromFile(std::string& errorLog);

	bool LoadFromFile_Rapid();

	// 解析字符串
	bool LoadFromString(const char* str, size_t size);

	// 查找段的索引值
	bool FindSectionIndex(const char* section_name, size_t& index) const;

	// 查找指定段中的键索引值
	bool FindSectionItemIndex(size_t sect_index, const char* key, 
		size_t& item_index) const;

	// 获得段名列表
	size_t GetSectionList(const IVarList& args, IVarList& result) const;

	// 获得段名列表
	size_t GetSectionList(IVarList& result) const;

	// 获得指定段下的键数量
	size_t GetSectionItemCount(size_t sect_index) const;


	// 获得指定段下的指定键名
	const char* GetSectionItemKey(size_t sect_index, size_t item_index) const;

	// 获得指定段下的指定键值
	const char* GetSectionItemValue(size_t sect_index, size_t item_index) const;

	// 获得段下所有键的数量
	size_t GetItemCount(const char* section_name) const;

	// 获得段下所有键名列表
	size_t GetItemList(const IVarList& args, IVarList& result) const;

	// 获得段下所有键名列表
	size_t GetItemList(const char* section_name, IVarList& result) const;

	// 获得段的数量
	size_t GetSectionCount() const;

	// 获得指定段名
	const char* GetSectionByIndex(size_t index) const;

	// 获得段下所有指定键的值列表
	size_t GetItemValueList(const IVarList& args, IVarList& result) const;

	// 获得段下所有指定键的值列表
	size_t GetItemValueList(const char* section_name, const char* key, IVarList& result) const;

	// 查找段
	bool FindSection(const char* section_name) const;

	// 添加段
	bool AddSection(const char* section_name);

	// 删除段
	bool DeleteSection(const char* section_name);

	// 查找段下的键
	bool FindItem(const char* section_name, const char* key) const;

	// 删除键
	bool DeleteItem(const char* section_name, const char* key);

	// 读取键数据，当键不存在的时候返回缺省值(def)
	int ReadInteger(const char* section_name, const char* key, int def) const;
	int64_t ReadInt64(const char* section_name, const char* key, int64_t def) const;
	char* ReadString(const char* section_name, const char* key, char* value, size_t maxlen) const;
	const char* ReadString(const char* section_name, const char* key, const char* def) const;
	float ReadFloat(const char* section_name, const char* key, float def) const;

	// 改写键数据（如果段或键不存在则添加）
	bool WriteInteger(const char* section_name, const char* key, int value);
	bool WriteString(const char* section_name, const char* key, const char* value);
	bool WriteFloat(const char* section_name, const char* key, float value);

	bool AddInteger(const char* section_name, const char* key, int value);
	bool AddString(const char* section_name, const char* key, const char* value);
	bool AddFloat(const char* section_name, const char* key, float value);

	template <typename T, class L>
	void ReadList(const char* sec_name, const char* name, const char* delims,
		L* list) const
	{
		std::string str = ReadString(sec_name, name, "");
		CVarList var;
		::util_split_string(var, str, delims);
		for (int i = 0; i < (int)var.GetCount(); i++)
		{
			list->push_back(ConvertFormString<T>(var.StringVal(i)));
		}
	}

public:
	// 保存文件
	bool SaveToFile() const;


private:
	struct item_t_xml
	{
        const char * m_strName;
		unsigned int m_nHash;
        const char * m_strValue;
		bool m_bDelName;
		bool m_bDelValue;

		item_t_xml(const char * strName,
			unsigned int nHash,
			const char * strValue,
			bool bDelName = false,
			bool bDelValue = false)
		{
			m_strName = strName;
			m_nHash = nHash;
			m_strValue = strValue;
			m_bDelName = bDelName;
			m_bDelValue = bDelValue;
		}

		~item_t_xml()
		{
			if (m_bDelName)
			{
				delete [] m_strName;
			}
			if (m_bDelValue)
			{
				delete [] m_strValue;
			}
		}
	};

	struct section_t_xml
	{
		const char * m_strName;
		unsigned int m_nHash;
		size_t m_nItemStart;
		size_t m_nItemCount;
		bool m_bDelName;

        section_t_xml(const char * strName,
			unsigned int nHash,
			size_t nItemStart,
			size_t nItemCount,
			bool bDelName = false)
		{
			m_strName = strName;
			m_nHash = nHash;
			m_nItemStart = nItemStart;
			m_nItemCount = nItemCount;
			m_bDelName = bDelName;
		}

		~section_t_xml()
		{
			if (m_bDelName)
			{
				delete [] m_strName;
			}
		}
	};

private:
	void ReleaseRes();

	const char* GetData(const char* section_name, const char* key) const;
	const section_t_xml* GetSection(const char* section_name) const;
	const item_t_xml* GetItem(const section_t_xml* section, const char* name) const;

	bool SetData(const char* section_name, const char* key, const char* data);
	bool AddData(const char* section_name, const char* key, const char* data);

private:
	std::string m_strFileName;
 
    // 读取xml时分配内存, rapidxml的parse()会修改传进去的字符串
    // 因此需要存为类成员变量, 析构函数里释放内存
    char * m_pXmlValue; 
    rapidxml::xml_document<> m_doc;

	std::vector<section_t_xml *> m_sectionVec;
	std::vector<item_t_xml *> m_itemVec;

    // hash_value到vector索引的映射, 用于优化section的查找
#if 1
    typedef std::multimap<size_t, size_t> SectionMapType;
    typedef std::multimap<size_t, size_t>::const_iterator SectionMapTypeConstItr;
#else
    typedef stdext::hash_multimap<size_t, size_t> SectionMapType;
    typedef stdext::hash_multimap<size_t, size_t>::const_iterator SectionMapTypeConstItr;
#endif

    SectionMapType m_sectionMap;
};

#endif
