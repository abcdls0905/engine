//--------------------------------------------------------------------
// �ļ���:		XmlFile.h
// ��  ��:		
// ˵  ��:		
// ��������:	2011��1��4��
// ������:		�
// ��Ȩ����:	������ţ�������޹�˾
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

	// ��ʼ�����ɴ�����������ʵ�壩
	virtual bool Init(const IVarList& args) { return true; }
	// �رգ���ɾ���������õĸ���ʵ�壬����Դ�ͷ�Ӧ�����������н��У�
	virtual bool Shut() { return true; }

	//�ͷ�������Դ
	virtual void Release();
	
	// �����ļ���
	void SetFileName(const char* filename);

	// ����ļ���
	const char* GetFileName() const;

    // �����ļ�
    bool LoadFromFile();
	bool LoadFromFile(std::string& errorLog);

	bool LoadFromFile_Rapid();

	// �����ַ���
	bool LoadFromString(const char* str, size_t size);

	// ���Ҷε�����ֵ
	bool FindSectionIndex(const char* section_name, size_t& index) const;

	// ����ָ�����еļ�����ֵ
	bool FindSectionItemIndex(size_t sect_index, const char* key, 
		size_t& item_index) const;

	// ��ö����б�
	size_t GetSectionList(const IVarList& args, IVarList& result) const;

	// ��ö����б�
	size_t GetSectionList(IVarList& result) const;

	// ���ָ�����µļ�����
	size_t GetSectionItemCount(size_t sect_index) const;


	// ���ָ�����µ�ָ������
	const char* GetSectionItemKey(size_t sect_index, size_t item_index) const;

	// ���ָ�����µ�ָ����ֵ
	const char* GetSectionItemValue(size_t sect_index, size_t item_index) const;

	// ��ö������м�������
	size_t GetItemCount(const char* section_name) const;

	// ��ö������м����б�
	size_t GetItemList(const IVarList& args, IVarList& result) const;

	// ��ö������м����б�
	size_t GetItemList(const char* section_name, IVarList& result) const;

	// ��öε�����
	size_t GetSectionCount() const;

	// ���ָ������
	const char* GetSectionByIndex(size_t index) const;

	// ��ö�������ָ������ֵ�б�
	size_t GetItemValueList(const IVarList& args, IVarList& result) const;

	// ��ö�������ָ������ֵ�б�
	size_t GetItemValueList(const char* section_name, const char* key, IVarList& result) const;

	// ���Ҷ�
	bool FindSection(const char* section_name) const;

	// ��Ӷ�
	bool AddSection(const char* section_name);

	// ɾ����
	bool DeleteSection(const char* section_name);

	// ���Ҷ��µļ�
	bool FindItem(const char* section_name, const char* key) const;

	// ɾ����
	bool DeleteItem(const char* section_name, const char* key);

	// ��ȡ�����ݣ����������ڵ�ʱ�򷵻�ȱʡֵ(def)
	int ReadInteger(const char* section_name, const char* key, int def) const;
	int64_t ReadInt64(const char* section_name, const char* key, int64_t def) const;
	char* ReadString(const char* section_name, const char* key, char* value, size_t maxlen) const;
	const char* ReadString(const char* section_name, const char* key, const char* def) const;
	float ReadFloat(const char* section_name, const char* key, float def) const;

	// ��д�����ݣ�����λ������������ӣ�
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
	// �����ļ�
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
 
    // ��ȡxmlʱ�����ڴ�, rapidxml��parse()���޸Ĵ���ȥ���ַ���
    // �����Ҫ��Ϊ���Ա����, �����������ͷ��ڴ�
    char * m_pXmlValue; 
    rapidxml::xml_document<> m_doc;

	std::vector<section_t_xml *> m_sectionVec;
	std::vector<item_t_xml *> m_itemVec;

    // hash_value��vector������ӳ��, �����Ż�section�Ĳ���
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
