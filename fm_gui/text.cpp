//--------------------------------------------------------------------
// 文件名:		Text.h
// 内  容:		文本管理器，由天子文本管理(张玲春所写) 改得
// 说  明:		
// 创建日期:	2008年9月3日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "text.h"
#include "../tools/string_util.h"
#include "../public/i_ini_file.h"
#include "../public/var_list.h"
#include "../public/auto_mem.h"
#include "../public/converts.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/core_file.h"
#include "../public/module.h"

//文件头
namespace TextBinaryFile
{
	struct Head
	{
		long version; //版本号
		long number; //文本总数
		char desc[64]; //备用
	};

	//文件数据
	struct Data
	{
		char nameid_len; //文本标识长度
		char* nameid; //文本标识数据
		short text_len; //正文长度
		wchar_t* text; //正文数据
	};
}

/// entity: Text
/// \brief 文本管理类
DECLARE_ENTITY(Text, 0, IEntity);

///// property: int FloatNumber
///// \brief 小数点后面保留几位小数
//DECLARE_PROPERTY(int, Text, FloatNumber, GetFloatNumber, SetFloatNumber);
//
///// property: bool CheckRepeatID
///// \brief 加载时是否检查重复键值
//DECLARE_PROPERTY(bool, Text, CheckRepeatID, GetCheckRepeatID, SetCheckRepeatID);
//
//
///// method: void LoadFiles(const char* path)
///// \brief  读取指定目录中的文本
//DECLARE_METHOD_1(bool, Text, LoadFiles, const char*);
//
///// method: bool Text::Clear()
///// \brief 清理
//DECLARE_METHOD_0(bool, Text, Clear);
//
///// method: std::wstring GetText(string IDName)
///// \brief  通过ID获取配置文本
//DECLARE_METHOD_1(std::wstring, Text, GetText, const char*);
//
///// method: bool SetText(string IDName, wstring wcsText)
///// \brief  设置文件ID对应内容，
//DECLARE_METHOD_2(bool, Text, SetText, const char*, const wchar_t*);
//
///// method: bool IsIDName(string IDName)
///// \brief  通过ID获取配置文本
//DECLARE_METHOD_1(bool, Text, IsIDName, const char*);
//
///// virtual bool Format_SetIDName(const char *pIDName);
//DECLARE_METHOD_1(bool, Text, Format_SetIDName, const char*);
//
///// virtual void Format_AddParam(value);
//DECLARE_METHOD_N(Text, Format_AddParam);
//
///// virtual std::string Format_GetText();
//DECLARE_METHOD_0(std::wstring, Text, Format_GetText);
//
///// method: const wchar_t* GetFormatText(string idname, [args])
///// \brief 获取格式化字符串
///// \param idname 格式化文本编号
///// \param args 可变数量的参数
//DECLARE_METHOD_N(Text, GetFormatText);
//
///// method: const bool AddConvertionFormatRule(wstring wcsFlag, int showmode, align, [args])
///// \brief 单位换算
///// 参数1 换算文本标识，宽字符串
///// 参数2 显示模式0,1,2
///// 参数3 是否对齐，比如一个单位是两位数，但现在实际只有一位数字要显示，是否补满两位
//
///// 参数4 最高的单位
///// 参数5 最高的单位对应进制
//
///// 参数6 次高的单位
///// 参数7 次高的单位对应进行
//
/////... 参数个数不限
//
//DECLARE_METHOD_N(Text, AddConvertionFormatRule);

Text::Text()
{
    m_nFloatNumber = 2;
	m_pTextIndices = NULL;
	m_nTextIndexSize = 0;
    m_bCheckRepeatID = false;
    m_pCheckUsedIDFile = NULL;
    m_pNoAddIDFile = NULL;
}

Text::~Text()
{
	Clear();
}

// 初始化（可创建其他附属实体）
bool Text::Init(const IVarList& args)
{
	return true;
}

// 语言包，转国际化时检查遗漏
bool Text::TextUsedTest()
{
    const char* main_filename = "textdump.ini";
    IIniFile* pMainIni = GetCore()->CreateIniFile(main_filename);

    if (!pMainIni->LoadFromFile())
    {
        pMainIni->Release();
		return false;
    }

    // 所有ID
    const char* allid_filename = pMainIni->ReadString(size_t(0), "allidname", 
		"allidname.ini");
    IIniFile* pAllIni = GetCore()->CreateIniFile(allid_filename);

    if (!pAllIni->LoadFromFile())
    {
        pAllIni->Release();
		pMainIni->Release();
		//MessageBox(NULL, allid_filename, "file not exist!", MB_OK);
        return false;
    }

    // 使用过的
    const char* usedid_filename = pMainIni->ReadString(size_t(0), "usedidname", 
		"usedidname.ini");
    IIniFile* pUsedIdIni = GetCore()->CreateIniFile(usedid_filename);

    if (!pUsedIdIni->LoadFromFile())
    {
        pUsedIdIni->Release();
		pAllIni->Release();
		pMainIni->Release();
		//MessageBox(NULL, usedid_filename, "file not exist!", MB_OK);
        return false;
    }

    // 
    const char* noadd_filename = pMainIni->ReadString(size_t(0), 
		"noaddidname", "noaddidname.ini");
    IIniFile* pTestIni = GetCore()->CreateIniFile(noadd_filename);

    if (!pTestIni->LoadFromFile())
    {
		pTestIni->Release();
		pUsedIdIni->Release();
		pAllIni->Release();
		pMainIni->Release();
        //MessageBox(NULL, noadd_filename, "file not exist!", MB_OK);
        return false;
    }

    // 国内版本所有ID
    size_t section_count = pAllIni->GetSectionCount();

    if (section_count > 0)
    {
        size_t item_count = pAllIni->GetSectionItemCount(0);

        for (size_t i = 0; i < item_count; ++i)
        {
            const char* key = pAllIni->GetSectionItemKey(0, i);
			
			m_mapAllIDName.Set(StringUtil::ToLower(key).c_str(), true);
        }
    }

    // 记录使用过的ID
    section_count = pUsedIdIni->GetSectionCount();

    if (section_count > 0)
    {
        size_t item_count = pUsedIdIni->GetSectionItemCount(0);

        for (size_t i = 0; i < item_count; ++i)
        {
			const char* key = pUsedIdIni->GetSectionItemKey(0, i);

			m_mapCheckUsedID.Set(StringUtil::ToLower(key).c_str(), true);
        }
    }

    m_pCheckUsedIDFile = fopen(usedid_filename, "a");

    if (m_pCheckUsedIDFile != NULL && section_count == 0)
    {
        fprintf(m_pCheckUsedIDFile, "\r\n[version]\r\n");
    }
    else
    {
        fprintf(m_pCheckUsedIDFile, "\r\n");
    }

    // 记录漏网的ID
    section_count = pTestIni->GetSectionCount();

    if (section_count > 0)
    {
        size_t item_count = pTestIni->GetSectionItemCount(0);

        for (size_t i = 0; i < item_count; ++i)
        {
            const char* key = pTestIni->GetSectionItemKey(0, i);
			
			m_mapNoAddIDName.Set(StringUtil::ToLower(key).c_str(), true);
			//m_mapNoAddIDName[StringUtil::ToLower(pTestIni->GetSectionItemKey(0, i))] = true;
        }
    }

    m_pNoAddIDFile = fopen(noadd_filename, "a");

    if (m_pNoAddIDFile != NULL && section_count == 0)
    {
        fprintf(m_pNoAddIDFile, "\r\n[version]\r\n");
    }
    else
    {
        fprintf(m_pNoAddIDFile, "\r\n");
    }

	pTestIni->Release();
	pUsedIdIni->Release();
	pAllIni->Release();
	pMainIni->Release();

    return true;
}

// 关闭（可删除其他引用的附属实体，而资源释放应在析构函数中进行）
bool Text::Shut()
{
    if (m_pCheckUsedIDFile != NULL)
    {
        fclose(m_pCheckUsedIDFile);
        m_pCheckUsedIDFile = NULL;
    }

    if (m_pNoAddIDFile != NULL)
    {
        fclose(m_pNoAddIDFile);
        m_pNoAddIDFile = NULL;
    }

	return true;
}

// 设置浮点数，小数位数
void Text::SetFloatNumber(const int number)
{
    if (number < 0 || number > 10)
    {
        return;
    }

    m_nFloatNumber = number;
}

int  Text::GetFloatNumber()
{
    return m_nFloatNumber;
}

// 加载时是否检查重复键值
void Text::SetCheckRepeatID(const bool value)
{
    m_bCheckRepeatID = value;
}

bool Text::GetCheckRepeatID()
{
    return m_bCheckRepeatID;
}

const char* Text::GetFullTextPath() const
{
	return m_strFullPath.c_str();
}

bool Text::LoadFiles(const char* path)
{	
	m_strFullPath = GetCore()->GetResourcePath();
	m_strFullPath += path;

	//先常试读取二进制文件，读取不成功才读取ascii文件
	if (!LoadTextByBinary())
	{
		LoadTextByAscii();
	}

	bool result = RefreshIndices();

	//if (!GetCore()->GetRetailVersion())
	//{
	//	static bool test = TextUsedTest();
	//}

	return result;
}

// 读取完毕后生成index, 加速查找
bool Text::RefreshIndices()
{
	// 生成索引表
	unsigned int text_size = m_Texts.size();
	unsigned int index_size = text_size + 1;

	if (m_pTextIndices)
	{
		CORE_FREE(m_pTextIndices, sizeof(unsigned int) * m_nTextIndexSize);
	}

	m_nTextIndexSize = index_size;
	m_pTextIndices = (unsigned int*)CORE_ALLOC(
		sizeof(unsigned int) * index_size);
	memset(m_pTextIndices, 0xFF, sizeof(unsigned int) * index_size);

	for (unsigned int i = 0; i < text_size; ++i)
	{
		text_item_t* p = m_Texts[i];
		unsigned int bucket = p->nHash % index_size;

		p->nNext = m_pTextIndices[bucket];
		m_pTextIndices[bucket] = i;
	}

    return true;
}

// 清理
bool Text::Clear()
{
    size_t text_num = m_Texts.size();
	
	for (size_t i = 0; i < text_num; ++i)
	{
#ifdef __USE_MEMPOOL__
		size_t name_len = strlen(m_Texts[i]->pName);
		size_t text_size = (wcslen(m_Texts[i]->pText) + 1) * sizeof(wchar_t);
		size_t item_size = sizeof(text_item_t) + name_len + text_size;
		m_mempool.Free(m_Texts[i], item_size);
#else
		//delete[] (char*)m_Texts[i];
		size_t name_len = strlen(m_Texts[i]->pName);
		size_t text_size = (wcslen(m_Texts[i]->pText) + 1) * sizeof(wchar_t);
		size_t item_size = sizeof(text_item_t) + name_len + text_size;
		CORE_FREE(m_Texts[i], item_size);
#endif
	}

	for (size_t k = 0; k < m_vConvertions.size(); ++k)
	{
		ConvertionFormat* convertion = m_vConvertions[k];

		for (size_t j = 0; j < convertion->m_vConverItems.size(); ++j)
		{
			CORE_DELETE(convertion->m_vConverItems[j]);
		}
		
		CORE_DELETE(convertion);
	}
	
	m_Texts.clear();
    //m_TextIndex.Clear();
    m_vConvertions.clear();
    m_mapConvertions.Clear();

	if (m_pTextIndices)
	{
		CORE_FREE(m_pTextIndices, sizeof(unsigned int) * m_nTextIndexSize);
		m_pTextIndices = NULL;
		m_nTextIndexSize = 0;
	}

    return true;
}

// 读取文本的二进制格式
bool Text::LoadTextByBinary()
{
	std::string binaryfile = m_strFullPath + "text.bin";

    FILE* fp = core_file::fopen(binaryfile.c_str(), "rb");

	if (NULL == fp)
	{
		return false;
	}

	core_file::fseek(fp, 0, SEEK_END);

	size_t file_size = core_file::ftell(fp);

	core_file::fseek(fp, 0, SEEK_SET);
	
	if (0 == file_size)
	{
		core_file::fclose(fp);
		return false;
	}

	TextBinaryFile::Head head;
	
	if (core_file::fread(&head, sizeof(head), 1, fp) != 1)
	{
		CORE_TRACE("(Text::LoadTextBinary)read head failed");
		CORE_TRACE(binaryfile.c_str());
		core_file::fclose(fp);
		return false;
	}
	
	m_Texts.resize(head.number);
	
	for (int i = 0; i < head.number; i++)
	{
		TextBinaryFile::Data data;

        // 1.1获取NameID长度
		if (core_file::fread(&data.nameid_len, sizeof(data.nameid_len), 1, fp) 
			!= 1)
		{
			CORE_TRACE("(Text::LoadTextBinary)read nameid_len failed");
			CORE_TRACE(binaryfile.c_str());
			core_file::fclose(fp);
			return false;
		}

		// 1.2分配NameID内存
        TAutoMem<char, 256> autostring(data.nameid_len + 1);

		data.nameid = autostring.GetBuffer();
		data.nameid[data.nameid_len] = 0;

        // 1.3读取文本标识
		if (core_file::fread(data.nameid, sizeof(char), data.nameid_len, fp) 
			!= data.nameid_len)
		{
			CORE_TRACE("(Text::LoadTextBinary)read nameid failed");
			CORE_TRACE(binaryfile.c_str());
			core_file::fclose(fp);
			return false;
		}

		Assert(data.nameid[data.nameid_len] == 0);
		
		// 2.1读取文本长度
		if (core_file::fread(&data.text_len, sizeof(data.text_len), 1, fp) 
			!= 1)
		{
			CORE_TRACE("(Text::LoadTextBinary)read text_len failed");
			CORE_TRACE(binaryfile.c_str());
			core_file::fclose(fp);
			return false;
		}
        
        // 2.2分配文本内存
        TAutoMem<wchar_t, 256> autowstring(data.text_len + 1);

		data.text = autowstring.GetBuffer();
		data.text[data.text_len] = 0;

        // 2.3读取文本
		if (core_file::fread(data.text, sizeof(wchar_t), data.text_len, fp) 
			!= data.text_len)
		{
			CORE_TRACE("(Text::LoadTextBinary)read text failed");
			CORE_TRACE(binaryfile.c_str());
			core_file::fclose(fp);
			return false;
		}

		Assert(data.text[data.text_len] == 0);
		
		// 3设置方本内容
		size_t text_size = (data.text_len + 1) * sizeof(wchar_t);
		size_t item_size = sizeof(text_item_t) + data.nameid_len + text_size;
#ifdef __USE_MEMPOOL__
		text_item_t* pItem = (text_item_t*)m_mempool.Alloc(item_size);
#else
		//text_item_t* pItem = (text_item_t*)NEW char[item_size];
		text_item_t* pItem = (text_item_t*)CORE_ALLOC(item_size);
#endif

		pItem->pText = (wchar_t*)(pItem->pName + data.nameid_len + 1);
		memcpy(pItem->pName, data.nameid, data.nameid_len + 1);
		memcpy(pItem->pText, data.text, text_size);
		pItem->nNext = 0xFFFFFFFF;
		pItem->nHash = GetHashValue(pItem->pName);

		m_Texts[i] = pItem;
		
		// 4.建立文本索引
		//m_TextIndex.Add(data.nameid, i);
	}

	core_file::fclose(fp);

	return true;
}

// 读取文本的ascii格式
void Text::LoadTextByAscii()
{
	std::string strFileName = m_strFullPath + "textfiles.ini";
    IIniFile* pIni = GetCore()->CreateIniFile(strFileName.c_str());

    if (!pIni->LoadFromFile())
    {
		char szBuffer[256];
		SafeSprintf(szBuffer, sizeof(szBuffer), "Load File %s Fail", strFileName.c_str());
		CORE_TRACE(szBuffer);

        pIni->Release();
		return;
    }

    size_t sec_index;

    pIni->FindSectionIndex("textfile", sec_index);

    size_t count = pIni->GetSectionItemCount(sec_index);

	for (size_t i = 0; i < count; ++i)
    {
        char szBuffer[32];

        SafeSprintf(szBuffer, sizeof(szBuffer), "%d", i);

		std::string filename = pIni->ReadString(sec_index, szBuffer, "");
		filename = m_strFullPath + filename;
		//CORE_TRACE(filename.c_str());

		LoadFile(filename.c_str());
	}

    if (m_bCheckRepeatID)
    {
        // 彻底释放内存
		TLiteralPod<char, bool, TStringTraits<char>, TTextAlloc> temp;

		m_mapCheckData.Swap(temp);
		//m_vCheckData.clear();
    }

	pIni->Release();
}

// 加载单个文件
void Text::LoadFile(const char* filename)
{
	IIniFile* pIni = GetCore()->CreateIniFile(filename);

	if (!pIni->LoadFromFile())
	{
		char szBuffer[256];
		SafeSprintf(szBuffer, sizeof(szBuffer), "Load File %s Fail", filename);
		CORE_TRACE(szBuffer);

		pIni->Release();
		return;
	}

	size_t sect_index;

	if (!pIni->FindSectionIndex("", sect_index))
	{
		pIni->Release();
		return;
	}
	
	size_t item_num = pIni->GetSectionItemCount(sect_index);

	for (size_t i = 0; i < item_num; ++i)
	{
		const char* key = pIni->GetSectionItemKey(sect_index, i);

        if (((unsigned char)key[0]) > 127)
        {
            continue;
        }

        const char* value = pIni->GetSectionItemValue(sect_index, i);
		if(strlen(value) == 0)
		{
			continue;
		}
		

		//设置方本内容`
		size_t size = Port_GetUTF8ToWideStrLen(value);
		TAutoMem<wchar_t, 256, TCoreAlloc> auto_buf(size);
		wchar_t* text = auto_buf.GetBuffer();

		Port_UTF8ToWideStr(value, text, size*sizeof(wchar_t));

		//size_t size = StringUtil::GetToWideStrLength(value, CP_UTF8);
		//TAutoMem<wchar_t, 256> auto_mem(size);
		//wchar_t* text = auto_mem.GetBuffer();

		//StringUtil::StringToWideStr(value, text, size * sizeof(wchar_t), CP_UTF8);

		size_t name_len = strlen(key);
		size_t text_size = (wcslen(text) + 1) * sizeof(wchar_t);
		size_t item_size = sizeof(text_item_t) + name_len + text_size;

#ifdef __USE_MEMPOOL__
		text_item_t* pItem = (text_item_t*)m_mempool.Alloc(item_size);
#else
		//text_item_t* pItem = (text_item_t*)NEW char[item_size];
		text_item_t* pItem = (text_item_t*)CORE_ALLOC(item_size);
#endif

		pItem->pText = (wchar_t*)(pItem->pName + name_len + 1);
		memcpy(pItem->pName, key, name_len + 1);
		memcpy(pItem->pText, text, text_size);
		pItem->nNext = 0xFFFFFFFF;
		pItem->nHash = GetHashValue(pItem->pName);

		m_Texts.push_back(pItem);
		
		if (m_bCheckRepeatID)
		{
			if (!m_mapCheckData.Exists(key))
			{
				m_mapCheckData.Add(pItem->pName, true);
			}
			else
			{
                // 非ASCII码不处理
				if (key[0] > 0 && key[0] < 128)
				{
					char szBuffer[256];
					
					SafeSprintf(szBuffer, sizeof(szBuffer), 
						"file:%s key=%s repeat", filename, key);
					CORE_TRACE(szBuffer);
				}
			}
		}
	}

	pIni->Release();
}

// 根据文本标识编号得到文本id
long Text::GetTextID(const char* name)  
{
	Assert(name != NULL);

	if (NULL == m_pTextIndices)
	{
		return -1;
	}
	
	unsigned int text_size = m_Texts.size();
	unsigned int hash = GetHashValue(name);
	unsigned int bucket = hash % (text_size + 1);
	unsigned int index = m_pTextIndices[bucket];

	//if (GetCore()->GetRetailVersion())
	//{
	//	while (index < text_size)
	//	{
	//		text_item_t* p = m_Texts[index];

	//		if ((stricmp(p->pName, name) == 0))
	//		{
	//			return long(index);
	//		}

	//		index = p->nNext;
	//	}

	//	// 检查记录，漏网的ID
	//	SaveNoAddID(name);
	//}
	//else
	{
		while (index < text_size)
		{
			text_item_t* p = m_Texts[index];

			if ((stricmp(p->pName, name) == 0))
			{
				// 检查记录,使用过的ID
				SaveUsedID(name);

				return long(index);
			}

			index = p->nNext;
		}

		// 检查记录，漏网的ID
		SaveNoAddID(name);
	}

	return -1;
}

bool Text::IsVaildID(long textid)
{
	return size_t(textid) < m_Texts.size();
}

std::wstring Text::GetText(long textid)
{
    Assert(IsVaildID(textid));

	std::wstring wcsText = m_Texts[textid]->pText;
	size_t pos = wcsText.find(L"{@IDName:");
	size_t end = -1;
	
    while (pos != std::wstring::npos)
	{
		end = wcsText.find(L"}", pos);

		// 配置的IDName开始位置
		size_t begin_pos = pos + 9;

        if (end > begin_pos && end != std::wstring::npos)
		{
			std::wstring wcsIDName = wcsText.substr(begin_pos, 
				end - begin_pos);
			std::wstring wcsSubText = GetText(
				StringUtil::WideStrAsString(wcsIDName.c_str()).c_str());

			wcsText.erase(pos, end - pos + 1);
			wcsText.insert(pos, wcsSubText);
		}
		else
		{
			break;
		}

		pos = wcsText.find(L"{@IDName:");
	}

    return wcsText;
}

bool Text::IsIDName(const char* IDName)
{
    long id = GetTextID(IDName);

    return IsVaildID(id);
}

// 根据数据标识取得文本(不进行任何转换)
const wchar_t* Text::GetTextSource(long textid)
{
	return m_Texts[textid]->pText;
}

std::wstring Text::GetText(const char* pIDName)
{
	long id = GetTextID(pIDName);

	if (id < 0)
	{
		return StringUtil::StringAsWideStr(pIDName);
	}
	else
	{
		return GetText(id);
	}
}

// 从数据流中取得显示文本
std::wstring Text::GetText(const char* IDName, IVarList& Params, 
	size_t& paramnum)
{
	const std::wstring str = GetText(IDName);

	// 如果找不到文本则返回IDName
	if (str.empty())
	{
		return StringUtil::StringAsWideStr(IDName);
	}

	CFlexString fstr(str);

    fstr.SetFloatNumber(m_nFloatNumber);

	return GetText(fstr, IDName, Params, paramnum);
}

// 从数据流中取得显示文本
std::wstring Text::GetText(CFlexString& fstr, const char* IDName, 
	IVarList& Params, size_t& paramnum)
{
	//检查该文本是否有变量
	paramnum = fstr.GetVarAmount();

	if (paramnum == 0)
	{
		// 如果该文本无变量则直接返回配置文本
		return GetText(IDName);
	}
	
	for (size_t nindex = 0; nindex < Params.GetCount(); nindex++)
	{
        std::wstring wcsInfo = fstr.GetVarInfo(nindex);
        size_t pos = wcsInfo.find(L"ToInt");

        if (pos != std::wstring::npos)
        {
            // LUA本身没有整型，配上ToInt,会将浮点转成整形
            int64_t value = 0;
            int type = Params.GetType(nindex);

            if (type == VTYPE_STRING)
            {
                value = StringUtil::StringAsInt64(Params.StringVal(nindex));
            }
            else if (type == VTYPE_WIDESTR)
            {
                value = StringUtil::WideStrAsInt64(Params.WideStrVal(nindex));
            }
            else if (type == VTYPE_INT)
            {
                value = Params.IntVal(nindex);
            }
            else if (type == VTYPE_BOOL)
            {
                value = Params.BoolVal(nindex) ? 1 : 0;
            }
            else if (type == VTYPE_FLOAT || type == VTYPE_DOUBLE)
            {
                value = (int64_t)Params.FloatVal(nindex);
            }
            else
            {
                value = Params.Int64Val(nindex);
            }

            //wchar_t wcsBuffer[128];
            //_i64tow(value, wcsBuffer, 10);
			std::wstring wcsBuffer = StringUtil::IntAsWideStr(value);

            fstr << wcsBuffer.c_str();
            continue;
        }
        else
        {
            bool bIsConvertion = false;

            for (size_t c = 0; c < m_vConvertions.size(); ++c)
            {
                pos = wcsInfo.find(m_vConvertions[c]->wcsFlag);

                if (pos != std::wstring::npos)
                {
                    bIsConvertion = true;
                    fstr << GetConvertionFormatString(
						m_vConvertions[c]->wcsFlag.c_str(), 
						Params.Int64Val(nindex));
					break;
                }
            }

            if (bIsConvertion)
            {
                continue;
            }
        }


		long type = Params.GetType(nindex);

		{
			switch(type)
			{
			case VTYPE_BOOL:
			case VTYPE_INT:
				{
					int value = Params.IntVal(nindex);

					fstr << value;
				}
				break;
            case VTYPE_INT64:
                {
                    //wchar_t wcsBuffer[260];
                    int64_t value = Params.Int64Val(nindex);

					std::wstring wcsBuffer = StringUtil::IntAsWideStr(value);
                    //_i64tow(value, wcsBuffer, 10);

                    fstr << wcsBuffer;
                }
                break;
			case VTYPE_FLOAT:
			case VTYPE_DOUBLE:
				{
					float value = Params.FloatVal(nindex);					

					fstr << value;
				}
				break;
			case VTYPE_STRING:
				{
					//普通串就是一定是IDNAME
					const char* value = Params.StringVal(nindex);

                    if (IsIDName(value))
                    {
                        fstr << GetText(value);
                    }
                    else
                    {
                        fstr << StringUtil::StringAsWideStr(value);
                    }
				}
				break;
			case VTYPE_WIDESTR:
				{
					//宽串就是直接显示的字符串
					const wchar_t* pKey = Params.WideStrVal(nindex);

					Assert(pKey);

					fstr << pKey;
				}
				break;
			default:
				//CORE_LOG_USER("OnServerMsg_System_info : invalidation type %d,index%d",type, nindex);
				continue;
				break;
			}
		}
	}

	fstr << FmtEnd;

	return fstr.GetStr();
}

//从数据流中取得显示文本
std::wstring Text::GetText(const char* IDName, IVarList& Params)
{
	size_t ParamNum;

	return GetText(IDName, Params, ParamNum);
}

// 设置文本ID，对应内容
bool Text::SetText(const char* szIDName, const wchar_t* wcsText)
{
	long text_id = GetTextID(szIDName);

	if (!IsVaildID(text_id))
	{
        // 如果没有则添加
	    size_t name_len = strlen(szIDName);
	    size_t text_size = (wcslen(wcsText) + 1) * sizeof(wchar_t);
	    size_t item_size = sizeof(text_item_t) + name_len + text_size;

#ifdef __USE_MEMPOOL__
		text_item_t* pItem = (text_item_t*)m_mempool.Alloc( item_size);
#else
		//text_item_t* pItem = (text_item_t*)NEW char[item_size];
		text_item_t* pItem = (text_item_t*)CORE_ALLOC(item_size);
#endif

		pItem->pText = (wchar_t*)(pItem->pName + name_len + 1);
		memcpy(pItem->pName, szIDName, name_len + 1);
		memcpy(pItem->pText, wcsText, text_size);
		pItem->nNext = 0xFFFFFFFF;
		pItem->nHash = GetHashValue(pItem->pName);

		m_Texts.push_back(pItem);

        return RefreshIndices();
	}

	text_item_t* pOldItem = m_Texts[text_id];

	size_t name_len = strlen(szIDName);
	size_t text_size = (wcslen(wcsText) + 1) * sizeof(wchar_t);
	size_t item_size = sizeof(text_item_t) + name_len + text_size;

#ifdef __USE_MEMPOOL__
	text_item_t* pItem = (text_item_t*)m_mempool.Alloc(item_size);
#else
	//text_item_t* pItem = (text_item_t*)NEW char[item_size];
	text_item_t* pItem = (text_item_t*)CORE_ALLOC(item_size);
#endif

	pItem->pText = (wchar_t*)(pItem->pName + name_len + 1);
	memcpy(pItem->pName, szIDName, name_len + 1);
	memcpy(pItem->pText, wcsText, text_size);

	m_Texts[text_id] = pItem;

#ifdef __USE_MEMPOOL__
	size_t old_name_len = strlen(pOldItem->pName);
	size_t old_text_size = (wcslen(pOldItem->pText) + 1) * sizeof(wchar_t);
	size_t old_item_size = sizeof(text_item_t) + old_name_len + old_text_size;
	m_mempool.Free(pOldItem, old_item_size);
#else
	//delete[] (char*)pOldItem;
	size_t old_name_len = strlen(pOldItem->pName);
	size_t old_text_size = (wcslen(pOldItem->pText) + 1) * sizeof(wchar_t);
	size_t old_item_size = sizeof(text_item_t) + old_name_len + old_text_size;
	CORE_FREE(pOldItem, old_item_size);
#endif

	return true;
}

//根据IDNAME得到其参数数量
size_t Text::GetTextParam(const char* IDName)
{
	CFlexString fstr(GetText(IDName));

	return (size_t)fstr.GetVarAmount();
}

bool Text::Format_SetIDName(const char* pIDName)
{
    if (pIDName)
    {
        m_FormatString = pIDName;
        m_FormatParams.Clear();
        return true;
    }

    m_FormatString = "";

    return false;
}

void Text::Format_AddParam(const IVarList& args, IVarList& ret)
{
    m_FormatParams << args;
}

std::wstring Text::Format_GetText()
{
    return GetText(m_FormatString.c_str(), m_FormatParams);
}

void Text::GetFormatText(const IVarList& args, IVarList& ret)
{
    if (args.GetCount() < 1)
    {
        ret << L"";
        return;
    }
    else if (args.GetCount() == 1)
    {
        ret << GetText(args.StringVal(0)).c_str();
        return;
    }

    std::string szIDName = args.StringVal(0);

    CVarList params;

    params.Append(args, 1, args.GetCount() - 1);

    ret.AddWideStr(GetText(szIDName.c_str(), params).c_str());
}

// 添加单位换算格式化规则
void Text::AddConvertionFormatRule(const IVarList& args, IVarList& ret)
{
    //参数1 换算文本标识，宽字符串
    //参数2 显示模式0,1,2
    //参数3 是否对齐，比如一个单位是两位数，但现在实际只有一位数字要显示，是否补满两位

    //参数4 最高的单位
    //参数5 最高的单位对应进制

    //参数6 次高的单位
    //参数7 次高的单位对应进行

    //...

    // 从高位到低位，理论上可以有无数单位

    size_t nArgCount = args.GetCount();

    if (nArgCount < 5)
    {
        return;
    }

    ConvertionFormat* convertion = CORE_NEW(ConvertionFormat);

    convertion->wcsFlag = args.WideStrVal(0);
    convertion->eFormatMode = (ConverShowMode)args.IntVal(1);
    convertion->isAlign = args.BoolVal(2);

    int i = 3;

    while (i + 1 < (int)nArgCount)
    {
        ConverItem* item = CORE_NEW(ConverItem);

        item->wcsDanWei = args.WideStrVal(i);
        item->nJinZhi = args.IntVal(i + 1);

        if (convertion->m_vConverItems.size() > 0)
        {
            int value = convertion->m_vConverItems.back()->nJinZhi 
				/ item->nJinZhi - 1;

            item->nNum = 0;

            while (value > 0)
            {
                value /= 10;
                item->nNum++;
            }
        }
        else
        {
            item->nNum = -1;
        }

        convertion->m_vConverItems.push_back(item);
        i = i + 2;
    }

    m_mapConvertions.Add(convertion->wcsFlag.c_str(), m_vConvertions.size());
	m_vConvertions.push_back(convertion);
}

// 获取单位换算后的文本
std::wstring Text::GetConvertionFormatString(const wchar_t* pFlag, 
	int64_t value)
{
	size_t index;

	if (!m_mapConvertions.GetData(pFlag, index))
	{
		return pFlag;
	}

    ConvertionFormat* convertion = m_vConvertions[index];
    int64_t nUseValue = value;
    //wchar_t wcsBuffer[260];
	std::wstring wcsBuffer;
    std::wstring wcsResult = L"";

	// 数值为0， 且显示格式 不是 显示所有 0锭1两0文
	// 要返回： 0文
	if (nUseValue == 0
		&& (convertion->eFormatMode != MONEY_MODE_SHOW_ALL))
	{
		wcsResult += L"0";
		size_t count = convertion->m_vConverItems.size();
		count = count - 1;
		wcsResult += convertion->m_vConverItems[count]->wcsDanWei;
		return wcsResult;
	}

	// 显示所有,         如:0锭1两0文, 0锭也被显示
    if (convertion->eFormatMode == MONEY_MODE_SHOW_ALL)
    {
        for (size_t i = 0; i<convertion->m_vConverItems.size(); ++i)
        {
            int64_t value = nUseValue / convertion->m_vConverItems[i]->nJinZhi;

            nUseValue = nUseValue % convertion->m_vConverItems[i]->nJinZhi;
            //_i64tow(value, wcsBuffer, 10);
			wcsBuffer = StringUtil::IntAsWideStr(value);

            if (convertion->isAlign)
            {
                int n0num = convertion->m_vConverItems[i]->nNum- wcslen(wcsBuffer.c_str());

                for (int z = 0; z < n0num; ++z)
                {
                    wcsResult += L"0";
                }
            }

            wcsResult +=  wcsBuffer + convertion->m_vConverItems[i]->wcsDanWei;
        }
    }
	// 显示存在及之后    如:1两0文, 0锭在之前不被显示，0文在之后被显示
    else if (convertion->eFormatMode == MONEY_MODE_SHOW_BEHIND)
    {
        bool bIsBegin = false;

        for (size_t i = 0; i < convertion->m_vConverItems.size(); ++i)
        {
            int64_t value = nUseValue / convertion->m_vConverItems[i]->nJinZhi;

            nUseValue = nUseValue % convertion->m_vConverItems[i]->nJinZhi;

            if (value == 0 && !bIsBegin)
            {
                continue;
            }

            bIsBegin = true;
			wcsBuffer = StringUtil::IntAsWideStr(value);
            //_i64tow(value, wcsBuffer, 10);

            if (convertion->isAlign)
            {
                int n0num = convertion->m_vConverItems[i]->nNum - wcslen(wcsBuffer.c_str());

                for (int z = 0; z < n0num; ++z)
                {
                    wcsResult += L"0";
                }
            }

            wcsResult += wcsBuffer + convertion->m_vConverItems[i]->wcsDanWei;
        }
    }
	// 仅显示存在        如:1两   不存在的0锭和0文都不被显示  
    else
    {
        for (size_t i = 0; i < convertion->m_vConverItems.size(); ++i)
        {
            int64_t value = nUseValue / convertion->m_vConverItems[i]->nJinZhi;

            nUseValue = nUseValue % convertion->m_vConverItems[i]->nJinZhi;

            if (value == 0)
            {
                continue;
            }

			wcsBuffer = StringUtil::IntAsWideStr(value);
            //_i64tow(value, wcsBuffer, 10);

            if (convertion->isAlign)
            {
                int n0num = convertion->m_vConverItems[i]->nNum - wcslen(wcsBuffer.c_str());

                for (int z = 0; z < n0num; ++z)
                {
                    wcsResult += L"0";
                }
            }

            wcsResult += wcsBuffer + convertion->m_vConverItems[i]->wcsDanWei;
        }
    }

    return wcsResult;
}

// 记录被使用过的ID
void Text::SaveUsedID(const char* idname)
{
    if (NULL == m_pCheckUsedIDFile)
    {
        return;
    }

    std::string low_idname = StringUtil::ToLower(idname);

    //if (m_mapCheckUsedID.find(low_idname) == m_mapCheckUsedID.end())
	if (!m_mapCheckUsedID.Exists(low_idname.c_str()))
    {
        // 找不到
        //m_mapCheckUsedID[low_idname] = true;
		m_mapCheckUsedID.Add(low_idname.c_str(), true);
        fprintf(m_pCheckUsedIDFile, "%s=0\r\n", low_idname.c_str());
        fflush(m_pCheckUsedIDFile);
    }
}

// 记录漏网的ID
void Text::SaveNoAddID(const char* idname)
{
    if (NULL == m_pNoAddIDFile)
    {
        return;
    }

    std::string low_idname = StringUtil::ToLower(idname);

    //if (m_mapAllIDName.find(low_idname) != m_mapAllIDName.end() && 
    //    m_mapNoAddIDName.find(low_idname) == m_mapNoAddIDName.end())
	if (m_mapAllIDName.Exists(low_idname.c_str())
		&& (!m_mapNoAddIDName.Exists(low_idname.c_str())))
    {
        // 找不到
        //m_mapNoAddIDName[low_idname] = true;
		m_mapNoAddIDName.Add(low_idname.c_str(), true);
        fprintf(m_pNoAddIDFile, "%s=0\r\n", low_idname.c_str());
        fflush(m_pNoAddIDFile);
    }
}
