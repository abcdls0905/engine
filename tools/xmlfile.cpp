//--------------------------------------------------------------------
// 文件名:		XmlFile.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2011年1月4日
// 创建人:		李超1
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "../public/module.h"
#include "xmlfile.h"
#include "../public/inlines.h"
#include "../public/auto_mem.h"
#include "../public/core_file.h"



/// \file CXmlFile.cpp
/// \brief xml文件


CXmlFile::CXmlFile()
{
    m_pXmlValue = 0; 
}

CXmlFile::CXmlFile(const char* filename):
m_strFileName(filename)
{
	Assert(filename != NULL);
    
    m_pXmlValue = 0; 
}

CXmlFile::~CXmlFile()
{
	ReleaseRes();
}

void CXmlFile::Release()
{
    ReleaseRes();
}

void CXmlFile::ReleaseRes()
{
	const int section_size = static_cast<int>(m_sectionVec.size()); 
	for (int i = 0; i < section_size; ++i)
	{
		const section_t_xml * pSection = m_sectionVec[i]; 
		if (0 != pSection)
		{
			delete pSection; 
			pSection = 0; 
		}
	}

	m_sectionVec.clear();

	const int item_size = static_cast<int>(m_itemVec.size()); 
	for (int i = 0; i < item_size; ++i)
	{
		const item_t_xml * pItem = m_itemVec[i]; 
		if (0 != pItem)
		{
			delete pItem; 
			pItem = 0; 
		}
	}

	m_itemVec.clear();

	m_sectionMap.clear();

	m_doc.clear(); 
	if (0 != m_pXmlValue)
	{
		delete[] m_pXmlValue; 
		m_pXmlValue = 0; 
	}
}

void CXmlFile::SetFileName(const char *filename)
{
	Assert(filename != NULL);

	m_strFileName = filename;
}

const char* CXmlFile::GetFileName() const
{
	return m_strFileName.c_str();
}

bool CXmlFile::LoadFromFile()
{
    string strErrorLog;
    return LoadFromFile(strErrorLog);
}

bool CXmlFile::LoadFromFile(std::string& errorLog)
{
	ReleaseRes();
	
    errorLog += m_strFileName;

	FILE *file = NULL;

	file = core_file::fopen(m_strFileName.c_str(), "rb");
	if (file == NULL)
	{
        errorLog += ": file is not exist!";
        return false;
	}

	long lengths = 0;
	core_file::fseek( file, 0, SEEK_END );
	lengths = core_file::ftell( file );
	core_file::fseek(file, 0, SEEK_SET);
	
    m_pXmlValue = NEW char[lengths +1 ];

	memset(m_pXmlValue, 0, lengths +1 );
	size_t result = core_file::fread(m_pXmlValue, sizeof(char),lengths, file );
	if (result == 0)
	{
		delete[] m_pXmlValue;
        m_pXmlValue = 0; 
    
        errorLog += ": file is empty!";
        return false;
	}

	m_pXmlValue[lengths] = '\0';
	core_file::fclose(file);

//    rapidxml::xml_document<>  doc;
	rapidxml::xml_node<char> * pNodeRoot = NULL;

	//将文件加载到数据表
	m_doc.clear();
	m_doc.parse<0>(m_pXmlValue);

	pNodeRoot = m_doc.first_node("Object");
	if(NULL == pNodeRoot)
	{
		m_doc.clear();
		delete[] m_pXmlValue;
        m_pXmlValue = 0;

        errorLog += ": file has no Ojbect or capitalization errors!";
        return false;
	}

	rapidxml::xml_node<char> *pNodeProperty = pNodeRoot->first_node("Property");

	if (NULL == pNodeProperty)
	{
		m_doc.clear();
		delete[] m_pXmlValue;
        m_pXmlValue = 0;

        errorLog += ": file first_node Property load errors!";
		return true;
	}

	while (pNodeProperty)
	{
		rapidxml::xml_attribute<char> *attr = pNodeProperty->first_attribute();

		if (attr)
		{
			const char * name = attr->value();

            size_t hash_value = GetHashValue(name);

			// 缺省名字为空的段
			section_t_xml * pSection = 
                NEW section_t_xml(name, (unsigned int)hash_value, m_itemVec.size(), 0);

			rapidxml::xml_attribute<char>* pXmlAttribut =  attr->next_attribute();

			while(pXmlAttribut)
			{
				const char * itemname = pXmlAttribut->name();
				const char * itemvalue = pXmlAttribut->value();
                const int hash = GetHashValue(itemname); 

				item_t_xml * pItem = 
                    NEW item_t_xml(itemname, hash, itemvalue);

				m_itemVec.push_back(pItem);

				pSection->m_nItemCount++;
				pXmlAttribut = pXmlAttribut->next_attribute();
			}

            m_sectionMap.insert(std::make_pair(hash_value, 
                                               m_sectionVec.size()));

			m_sectionVec.push_back(pSection);
		}

		pNodeProperty = pNodeProperty->next_sibling("Property");
	}
	//m_doc.clear();
	//delete[] m_pXmlValue;
	return true;
}

bool CXmlFile::LoadFromFile_Rapid()
{
    return LoadFromFile(); 
}

bool CXmlFile::LoadFromString(const char* str, size_t size)
{
	Assert(str != NULL);

    ReleaseRes(); 

    const size_t len = strlen(str) + 1; 
    
    m_pXmlValue = NEW char[len]; 
    strncpy(m_pXmlValue, str, len); 
    m_pXmlValue[len] = '\0'; 

//    rapidxml::xml_document<>  doc;
	rapidxml::xml_node<char> * pNodeRoot = NULL;

	//将文件加载到数据表
	m_doc.clear();
	m_doc.parse<0>((char*)m_pXmlValue);

	pNodeRoot = m_doc.first_node("Object");
	if(pNodeRoot == NULL)
    {
        m_doc.clear(); 
        delete[] m_pXmlValue; 
        m_pXmlValue = 0; 

		return false;
    }

	rapidxml::xml_node<char> *pNodeProperty = pNodeRoot->first_node("Property");

	if (NULL == pNodeProperty)
	{
        m_doc.clear(); 
        delete[] m_pXmlValue; 
        m_pXmlValue = 0; 
		
        return true;
	}

	while (pNodeProperty)
	{
		rapidxml::xml_attribute<char> *attr = pNodeProperty->first_attribute();

		if (attr)
		{
			const char * name = attr->value();

			// 缺省名字为空的段
			section_t_xml * pSection = 
                NEW section_t_xml(name, GetHashValue(name), m_itemVec.size(), 0);

			rapidxml::xml_attribute<char>* pXmlAttribut =  attr->next_attribute();

			while(pXmlAttribut)
			{
				const char * itemname = pXmlAttribut->name();
				const char * itemvalue = pXmlAttribut->value();
                const int hash = GetHashValue(itemname); 
				
                item_t_xml * pItem = 
                    NEW item_t_xml(itemname, hash, itemvalue);

				m_itemVec.push_back(pItem);

				pSection->m_nItemCount++;
				pXmlAttribut =  pXmlAttribut->next_attribute();

			}

			m_sectionVec.push_back(pSection);
		}

		pNodeProperty = pNodeProperty->next_sibling("Property");
	}


	return true;
}

bool CXmlFile::FindSection(const char* section_name) const
{
	Assert(section_name != NULL);

	return (GetSection(section_name) != NULL);
}

bool CXmlFile::FindSectionIndex(const char* section_name, size_t& index) const
{
	Assert(section_name != NULL);

	unsigned int hash = GetHashValue(section_name);

	const size_t SIZE1 = m_sectionVec.size();

	for (size_t i = 0; i < SIZE1; ++i)
	{
        const section_t_xml * pSection = m_sectionVec[i]; 
		if (0 != pSection && 
            pSection->m_nHash == hash && 
            stricmp(pSection->m_strName, section_name) == 0)
		{
			index = i;
			return true;
		}
	}

	return false;
}

bool CXmlFile::FindSectionItemIndex(size_t sect_index, const char* key, 
									size_t& item_index) const
{
	Assert(sect_index < m_sectionVec.size());
	Assert(key != NULL);

	const section_t_xml * pSection = m_sectionVec[sect_index];
    if (0 == pSection)
    {
        return false; 
    }

	unsigned int hash = GetHashValue(key);

	const size_t SIZE1 = pSection->m_nItemStart + pSection->m_nItemCount;

	for (size_t i = pSection->m_nItemStart; i < SIZE1; ++i)
	{
        const item_t_xml * pItem = m_itemVec[i]; 
		if (0 != pItem && 
            pItem->m_nHash == hash && 
            stricmp(pItem->m_strName, key) == 0)
		{
			item_index = i - pSection->m_nItemStart;
			return true;
		}
	}

	return false;
}

size_t CXmlFile::GetSectionList(const IVarList& args, IVarList& result) const
{
	return GetSectionList(result);
}

size_t CXmlFile::GetSectionList(IVarList& result) const
{
	result.Clear();

	const size_t SIZE1 = m_sectionVec.size();

	for (size_t i = 0; i < SIZE1; ++i)
	{
        const section_t_xml * pSection = m_sectionVec[i]; 
        if (0 != pSection)
        {
            result.AddString(pSection->m_strName);
        }
        else
        {
            result.AddString(""); 
        }
	}

	return result.GetCount();
}

size_t CXmlFile::GetSectionItemCount(size_t sect_index) const
{
	Assert(sect_index < m_sectionVec.size());

	const section_t_xml * pSection = m_sectionVec[sect_index];

	return (0 != pSection) ? pSection->m_nItemCount : 0;
}

const char* CXmlFile::GetSectionItemKey(size_t sect_index, 
										size_t item_index) const
{
	Assert(sect_index < m_sectionVec.size());

	const section_t_xml * pSection = m_sectionVec[sect_index];
    if (0 == pSection)
    {
        return "";
    }

	Assert(item_index < pSection->m_nItemCount);

    const item_t_xml * pItem = 
        m_itemVec[pSection->m_nItemStart + item_index];
	return (0 != pItem) ? pItem->m_strName : "";
}

const char* CXmlFile::GetSectionItemValue(size_t sect_index, 
										  size_t item_index) const
{
	Assert(sect_index < m_sectionVec.size());

	const section_t_xml * pSection = m_sectionVec[sect_index];
    if (0 == pSection)
    {
        return "";
    }

	Assert(item_index < pSection->m_nItemCount);

    const item_t_xml * pItem = 
        m_itemVec[pSection->m_nItemStart + item_index];
	return (0 != pItem) ? pItem->m_strValue : "";
}

bool CXmlFile::FindItem(const char* section_name, const char* key) const
{
	Assert(section_name != NULL);
	Assert(key != NULL);

	const section_t_xml *section = GetSection(section_name);

	if (NULL == section)
	{
		return false;
	}

	return (GetItem(section, key) != NULL);
}

size_t CXmlFile::GetItemCount(const char* section_name) const
{
	const section_t_xml *section = GetSection(section_name);

	if (NULL == section)
	{
		return 0;
	}

	return section->m_nItemCount;
}

size_t CXmlFile::GetItemList(const IVarList& args, IVarList& result) const
{
	const char* section = args.StringVal(0);

	return GetItemList(section, result);
}

size_t CXmlFile::GetItemList(const char* section_name, IVarList& result) const
{
	result.Clear();

	const section_t_xml *section = GetSection(section_name);

	if (NULL == section)
	{
		return 0;
	}

	const size_t SIZE1 = section->m_nItemStart + section->m_nItemCount;

	for (size_t i = section->m_nItemStart; i < SIZE1; ++i)
	{
        const item_t_xml * pItem = m_itemVec[i]; 
        if (0 != pItem)
        {
            result.AddString(pItem->m_strName);
        }
        else
        {
            result.AddString(""); 
        }
	}

	return result.GetCount();
}


size_t CXmlFile::GetSectionCount() const
{
	return m_sectionVec.size();
}

const char* CXmlFile::GetSectionByIndex(size_t index) const
{
	Assert(index < m_sectionVec.size());

    const section_t_xml * pSection = m_sectionVec[index]; 
	return (0 != pSection) ? pSection->m_strName : "";
}

int64_t CXmlFile::ReadInt64(const char* section_name, const char* key, int64_t def) const
{
	const char* str = GetData(section_name, key);

	if (str && strcmp(str, "") != 0)
	{
	//	return ::_atoi64(str);
		return Port_StringToInt64(str);
	}
	else
	{
		return def;
	}
}

int CXmlFile::ReadInteger(const char* section_name, const char* key, int def) const
{
	const char* str = GetData(section_name, key);

	if (str && strcmp(str, "") != 0)
	{
		return atoi(str);
	}
	else
	{
		return def;
	}
}

const char* CXmlFile::GetData(const char* section_name, const char* key) const
{
	Assert(section_name != NULL);
	Assert(key != NULL);

	const section_t_xml *section = GetSection(section_name);

	if (NULL == section)
	{
		return NULL;
	}

	const item_t_xml *item = GetItem(section, key);

	if (NULL == item)
	{
		return NULL;
	}

	return item->m_strValue;
}

size_t CXmlFile::GetItemValueList(const IVarList& args, IVarList& result) const
{
	const char* section = args.StringVal(0);
	const char* key = args.StringVal(1);

	return GetItemValueList(section, key, result);
}

size_t CXmlFile::GetItemValueList(const char* section_name, const char* key, IVarList& result) const
{
	result.Clear();

	const section_t_xml *section = GetSection(section_name);

	if (NULL == section)
	{
		return 0;
	}

	const size_t SIZE1 = section->m_nItemStart + section->m_nItemCount;

	for (size_t i = section->m_nItemStart; i < SIZE1; ++i)
	{
        const item_t_xml * pItem = m_itemVec[i]; 
		if (0 != pItem && 0 == stricmp(pItem->m_strName, key))
		{
			result.AddString(pItem->m_strValue);
		}
	}

	return result.GetCount();
}

const CXmlFile::section_t_xml* CXmlFile::GetSection(const char* section_name) const
{
	Assert(section_name != NULL);

	unsigned int hash = GetHashValue(section_name);

	const size_t SIZE1 = m_sectionVec.size();

    if (SIZE1 == m_sectionMap.size())
    {
        // 进行优化查找
        std::pair<SectionMapTypeConstItr, SectionMapTypeConstItr> ii = 
            m_sectionMap.equal_range(hash);

        for (SectionMapTypeConstItr i = ii.first; i != ii.second; ++i)
        {
            const section_t_xml * pSection = m_sectionVec[i->second]; 
            if (0 != pSection && 
                pSection->m_nHash == hash && 
                stricmp(pSection->m_strName, section_name) == 0)
            {
                return pSection;
            }
        }
    }
    else
    {
        for (size_t i = 0; i < SIZE1; ++i)
        {
            const section_t_xml * pSection = m_sectionVec[i]; 
            if (0 != pSection && 
                pSection->m_nHash == hash && 
                stricmp(pSection->m_strName, section_name) == 0)
            {
                return pSection;
            }
        }
    }

	return NULL;
}


const CXmlFile::item_t_xml* CXmlFile::GetItem(const section_t_xml* section, const char* name) const
{
	Assert(name != NULL);
	Assert(section != NULL);


	unsigned int hash = GetHashValue(name);

	const size_t SIZE1 = section->m_nItemStart + section->m_nItemCount;

	for (size_t i = section->m_nItemStart; i < SIZE1; ++i)
	{
        const item_t_xml * pItem = m_itemVec[i]; 
		if (0 != pItem && 
            pItem->m_nHash == hash && 
			stricmp(pItem->m_strName, name) == 0)
		{
			return pItem;
		}
	}

	return NULL;
}

char* CXmlFile::ReadString(const char *section_name, const char *key, char *value, size_t maxlen) const
{
	Assert(value != NULL);
	Assert(maxlen > 0);

	const char* str = GetData(section_name, key);

	if (str)
	{
		size_t len = strlen(str);

		if (len >= maxlen)
		{
			len = maxlen - 1;
		}

		memcpy(value, str, len);

		value[len] = 0;
	}

	return value;
}

const char* CXmlFile::ReadString(const char *section_name, const char *key, const char *def) const
{
	Assert(def != NULL);

	const char* str = GetData(section_name, key);

	if (str && strcmp(str, "") != 0)
	{
		return str;
	}
	else
	{
		return def;
	}
}

float CXmlFile::ReadFloat(const char *section_name, const char *key, float def) const
{
	const char* str = GetData(section_name, key);

	if (str && strcmp(str, "") != 0)
	{
		return (float)atof(str);
	}
	else
	{
		return def;
	}
}

bool CXmlFile::AddSection(const char *section_name)
{
	Assert(section_name != NULL);

	if (NULL != GetSection(section_name))
	{
		return false;
	}

	const size_t sec_len = strlen(section_name) + 1; 
	char *name = NEW char[sec_len];
	strncpy(name, section_name, sec_len);

	section_t_xml * pSection = 
        NEW section_t_xml(name, GetHashValue(section_name), m_itemVec.size(), 0, true);
	m_sectionVec.push_back(pSection);

	return true;
}

bool CXmlFile::DeleteSection(const char *section_name)
{
	std::vector<item_t_xml *>::iterator item_It_begin = m_itemVec.begin();
	std::vector<item_t_xml *>::iterator item_It_end = m_itemVec.end();
	std::vector<section_t_xml *>::iterator section_It_earse = m_sectionVec.begin();
	
	Assert(section_name != NULL);

	size_t sect_index;

	if (!FindSectionIndex(section_name, sect_index))
	{
		return false;
	}

	const section_t_xml * pSection = m_sectionVec[sect_index];
    if (0 == pSection)
    {
        return false; 
    }

    const size_t xml_section_size = m_sectionVec.size();
	for (size_t k = sect_index + 1; k < xml_section_size; ++k)
	{
        if (0 != m_sectionVec[k])
        {
		    m_sectionVec[k]->m_nItemStart -= pSection->m_nItemCount;
	    }
    }

	std::vector<item_t_xml *>::iterator item_it = m_itemVec.begin();	
	for (size_t i = 0; i < m_itemVec.size(); ++i)
	{
		if( i == pSection->m_nItemStart)
		{
			item_It_begin = item_it;
		}

		if(i == (pSection->m_nItemStart + pSection->m_nItemCount))
		{
			item_It_end = item_it;
		}
		++item_it;
	}
	
	if (pSection->m_nItemStart + pSection->m_nItemCount == m_itemVec.size())
	{
		item_It_end = m_itemVec.end();
	}

	std::vector<section_t_xml *>::iterator section_it = m_sectionVec.begin();
	
	for (size_t i = 0; i < xml_section_size; ++i)
	{
		if(i == sect_index)
		{
			section_It_earse = section_it;
			break;
		}

		++section_it;
	}
	
	if (pSection->m_nItemCount > 0)
	{
        for (std::vector<item_t_xml *>::iterator itr = item_It_begin; 
             itr != item_It_end && itr != m_itemVec.end(); 
             ++itr)
        {
            const item_t_xml * pItem = *itr; 
            if (0 != pItem)
            {
                delete pItem; 
                pItem = 0; 
            }
        }

		m_itemVec.erase(item_It_begin,item_It_end);
	}
	
    if (0 != (*section_It_earse))
    {
        delete (*section_It_earse); 
        *section_It_earse = 0; 
    }

	m_sectionVec.erase(section_It_earse);

	return true;
}

bool CXmlFile::DeleteItem(const char* section_name, const char* key)
{
	Assert(section_name != NULL);
	Assert(key != NULL);

	size_t sect_index;

	if (!FindSectionIndex(section_name, sect_index))
	{
		return false;
	}

	section_t_xml * pSection = m_sectionVec[sect_index];
    if (0 == pSection)
    {
        return false; 
    }

	unsigned int hash = GetHashValue(key);
	const size_t item_end = pSection->m_nItemStart + pSection->m_nItemCount;

	std::vector<item_t_xml *>::iterator item_it;

	item_it = m_itemVec.begin();

	for (size_t i = pSection->m_nItemStart; i < item_end; ++i)
	{
        const item_t_xml * pItem = m_itemVec[i]; 
		if (0 != pItem && 
            pItem->m_nHash == hash && 
            stricmp(pItem->m_strName, key) == 0)
		{
            const size_t xml_section_size = m_sectionVec.size();
			for (size_t k = sect_index + 1; k < xml_section_size; ++k)
			{
                if (0 != m_sectionVec[k])
                {
				    --m_sectionVec[k]->m_nItemStart;
                }
			}

            delete pItem; 
            pItem = 0; 

			m_itemVec.erase(item_it);
			--pSection->m_nItemCount;

			return true;
		}

		++item_it;
	}


	return false;
}

bool CXmlFile::WriteInteger(const char* section_name, const char* key, int value)
{
	char buf[32];

	sprintf(buf, "%d", value);

	return SetData(section_name, key, buf);
}

bool CXmlFile::WriteString(const char* section_name, const char* key, const char* value)
{
	return SetData(section_name, key, value);
}

bool CXmlFile::WriteFloat(const char *section_name, const char *key, float value)
{
	char buf[32];

	sprintf(buf, "%f", double(value));

	return SetData(section_name, key, buf);

}

bool CXmlFile::AddInteger(const char* section_name, const char* key, int value)
{
	char buf[32];

	sprintf(buf, "%d", value);

	return AddData(section_name, key, buf);
}

bool CXmlFile::AddString(const char* section_name, const char* key, const char* value)
{
	return AddData(section_name, key, value);
}

bool CXmlFile::AddFloat(const char *section_name, const char *key, float value)
{
	char buf[32];

	sprintf(buf, "%f", double(value));

	return AddData(section_name, key, buf);

}

bool CXmlFile::AddData(const char* section_name, const char* key, const char* data)
{
	Assert(section_name != NULL);
	Assert(key != NULL);
	Assert(data != NULL);

	size_t sect_index;

	if (!FindSectionIndex(section_name, sect_index))
	{
		sect_index = m_sectionVec.size();

		AddSection(section_name);
	}
	
	section_t_xml * pSection = m_sectionVec[sect_index];
    if (0 == pSection)
    {
        return false; 
    }

	size_t item_index = pSection->m_nItemStart + pSection->m_nItemCount;

	const size_t key_len = strlen(key) + 1; 
	char *item_name = NEW char[key_len];
	strncpy(item_name, key, key_len);

	const size_t data_len = strlen(data) + 1; 
	char *item_value = NEW char[data_len];
	strncpy(item_value, data, data_len);

    const int hash = GetHashValue(item_name); 

	item_t_xml * pItem = 
        NEW item_t_xml(item_name, hash, item_value, true, true);

	m_itemVec.insert(m_itemVec.begin()+item_index, 1, pItem);

	++pSection->m_nItemCount;

    const size_t xml_section_size = m_sectionVec.size();
	for (size_t k = sect_index + 1; k < xml_section_size; ++k)
	{
        if (0 != m_sectionVec[k])
        {
		    ++m_sectionVec[k]->m_nItemStart;
        }
	}

	return true;
}

bool CXmlFile::SetData(const char* section_name, const char* key, const char* data)
{
	Assert(section_name != NULL);
	Assert(key != NULL);
	Assert(data != NULL);

	size_t sect_index;

	if (!FindSectionIndex(section_name, sect_index))
	{
		sect_index = m_sectionVec.size();

		AddSection(section_name);

	}

	const section_t_xml * pSection = m_sectionVec[sect_index];
    if (0 == pSection)
    {
        return false; 
    }

	item_t_xml *item = (item_t_xml *)GetItem(pSection, key);

	if (NULL == item)
	{
		AddData(section_name, key, data);
	}
	else
	{
		if (item->m_bDelValue)
		{
			delete [] item->m_strValue;
		}
		
		const size_t data_len = strlen(data) + 1; 
		char *item_value = NEW char[data_len];
		strncpy(item_value, data, data_len);

		item->m_strValue = item_value;
		item->m_bDelValue = true;
	}

	return true;

}

bool CXmlFile::SaveToFile() const
{
	rapidxml::xml_document<> doc;
	rapidxml::xml_node<char> * pNodeRoot = doc.allocate_node(rapidxml::node_element,"Object");
	doc.append_node(pNodeRoot);

	rapidxml::xml_node<char>* pItemElm = NULL;

    const size_t xml_section_size = m_sectionVec.size();
	for (size_t i = 0; i < xml_section_size; i ++)
	{
        const section_t_xml * pSection = m_sectionVec[i]; 
        if (0 == pSection)
        {
            continue; 
        }

		pItemElm = doc.allocate_node(rapidxml::node_element,"Property");
		pItemElm->append_attribute(doc.allocate_attribute("ID", pSection->m_strName));
		pNodeRoot->append_node(pItemElm);

		size_t start_index = pSection->m_nItemStart;
		size_t end_index = pSection->m_nItemStart + pSection->m_nItemCount;

		for (size_t j = start_index; j < end_index; j ++)
		{
            const item_t_xml * pItem = m_itemVec[j]; 
			pItemElm->append_attribute(doc.allocate_attribute(pItem->m_strName, pItem->m_strValue));
		}
	}

	std::ofstream out(m_strFileName.c_str());
	out << "<?xml version=\"1.0\" encoding=\"gb2312\"?>\n";
	out << doc;
	out.close();

	return true;
}

// end of file XmlFile.cpp

