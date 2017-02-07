

#include "string_util.h"
#include "../public/i_var_list.h"
#include "../public/Converts.h"
#include "../public/auto_mem.h"
#include "../public/Inlines.h"


//�ַ����Ƿ�Ϊ��
bool StringUtil::CharIsNull(const char* strChar)
{
    if (!strChar || strChar[0] == '\0')
    {
        return true;
    }

    return false;
}

// UTF8�ַ���ת���ɿ��ַ���
std::wstring StringUtil::UTF8StringAsWideStr(const char* s)
{
    size_t size = Port_GetUTF8ToWideStrLen(s);
    TAutoMem<wchar_t, 256> auto_buf(size);
    wchar_t* buffer = auto_buf.GetBuffer();
    Port_UTF8ToWideStr(s, buffer, size * sizeof(wchar_t));
    return std::wstring(buffer);
}

// ��ÿ��ַ���ת����UTF8�ַ���
std::string StringUtil::WideStrAsUTF8String(const wchar_t* ws)
{
    size_t size = Port_GetWideStrToUTF8Len(ws);
    TAutoMem<char, 256> auto_buf(size);
    char* buffer = auto_buf.GetBuffer();
    Port_WideStrToUTF8(ws, buffer, size);
    return std::string(buffer);
}


std::wstring StringUtil::Utf8ToUtf16(const char* str)
{
	// ��ó���
	size_t size = Port_GetUTF8ToWideStrLen(str);
	TAutoMem<wchar_t, 256> auto_buf(size);
	wchar_t* buf = auto_buf.GetBuffer();
	const wchar_t* wstr = Port_UTF8ToWideStr(str, buf, size*sizeof(wchar_t));
	return std::wstring(wstr);
}

std::string StringUtil::Utf16ToUtf8(const wchar_t* str)
{
	// ��ó���
	size_t size = Port_GetWideStrToUTF8Len(str);
	TAutoMem<char, 256> auto_buf(size);
	char* buf = auto_buf.GetBuffer();
	const char* utf8 = Port_WideStrToUTF8(str, buf, size*sizeof(char));
	return std::string(utf8);
}


// ת�������ַ���
std::wstring StringUtil::StringAsWideStr(const char* s)
{
	//size_t size = ::GetToWideStrLen(s);
	//TAutoMem<wchar_t, 256> auto_buf(size);
	//wchar_t* buffer = auto_buf.GetBuffer();
	//::ToWideStr(s, buffer, size * sizeof(wchar_t));
	//return std::wstring(buffer);
	return UTF8StringAsWideStr(s);
}

// ת�����ַ���
std::string StringUtil::WideStrAsString(const wchar_t* ws)
{
	/*size_t size = ::GetToStringLen(ws);
	TAutoMem<char, 256> auto_buf(size);
	char* buffer = auto_buf.GetBuffer();
	::ToString(ws, buffer, size);
	return std::string(buffer);*/
    return WideStrAsUTF8String(ws);
}

// ��һ���ַ���ת����const char*,����䴮Ϊ���򷵻�def_value
const char* StringUtil::ParseStrAsStr(const char* str, const char* def_value)
{
	if (NULL == str || str[0] ==  '\0')
	{
		return def_value;
	}

	return str;
}

// ��һ���ַ���ת����long,����䴮Ϊ���򷵻�def_value
long StringUtil::ParseStrAsLong(const char* str, long def_value)
{
	if (NULL == str || str[0] ==  '\0')
	{
		return def_value;
	}

	return StringAsInt(str, def_value);
}

// ��һ���ַ���ת����float,����䴮Ϊ���򷵻�def_value
float StringUtil::ParseStrAsFloat(const char* str, float def_value)
{
	if (NULL == str || str[0] == '\0')
	{
		return def_value;
	}

	return StringAsFloat(str);
}

//��һ���ַ���ת����bool,����䴮Ϊ���򷵻�def_value
bool StringUtil::ParseStrAsBool(const char* str, bool def_value)
{
	if (NULL == str || str[0] == '\0')
	{
		return def_value;
	}

	return (stricmp(str, "true") == 0);
}


// ����ת��Ϊ�ַ���
std::string StringUtil::IntAsString(int value)
{
	char str[32];

	SafeSprintf(str, 32, "%d", value);

	return std::string(str);
}

std::string StringUtil::UIntAsString(unsigned int value)
{
    char str[32];

    SafeSprintf(str, 32, "%u", value);

    return std::string(str);
}

std::string StringUtil::Int64AsString(int64_t value)
{
    char str[32];
    Port_Int64ToString(value, str, sizeof(str));

    return std::string(str);
}

std::wstring StringUtil::Int64AsWideStr(int64_t value)
{
	wchar_t str[32];
	Port_Int64ToWideStr(value, str, sizeof(str));

	return std::wstring(str);
}

// ������ת��Ϊ�ַ���
std::string StringUtil::FloatAsString(float value, int decimals)
{
	char str[32];

	if (0 == decimals)
	{
		SafeSprintf(str, 32, "%f", (double)value);
	}
	else
	{
		switch (decimals)
		{
		case 1:
			SafeSprintf(str, 32, "%.01f", (double)value);
			break;
		case 2:
			SafeSprintf(str, 32, "%.02f", (double)value);
			break;
		case 3:
			SafeSprintf(str, 32, "%.03f", (double)value);
			break;
		case 4:
			SafeSprintf(str, 32, "%.04f", (double)value);
			break;
		case 5:
			SafeSprintf(str, 32, "%.05f", (double)value);
			break;
		default:
			SafeSprintf(str, 32, "%f", (double)value);
			break;
		}
	}

	int len = (int)strlen(str);

	for(int i = len - 1; str[i]!= '.'; i--)
	{
		if (str[i] == '0')
		{
			str[i] = 0;
		}
		else
		{
			if (str[i] == '.')
			{
				str[i] = 0;
			}

			break;
		}
	}

	return std::string(str);
}

// ˫������ת�����ַ���
std::string StringUtil::DoubleAsString(double value, int decimals)
{
	char str[64];

	if (0 == decimals)
	{
		SafeSprintf(str, 64, "%f", value);
	}
	else
	{
		switch (decimals)
		{
		case 1:
			SafeSprintf(str, 64, "%.01f", value);
			break;
		case 2:
			SafeSprintf(str, 64,  "%.02f", value);
			break;
		case 3:
			SafeSprintf(str, 64,  "%.03f", value);
			break;
		case 4:
			SafeSprintf(str, 64,  "%.04f", value);
			break;
		case 5:
			SafeSprintf(str, 64,  "%.05f", value);
			break;
		default:
			SafeSprintf(str, 64,  "%f", value);
			break;
		}
	}

	return std::string(str);
}

// �ַ���ת��Ϊ����
int StringUtil::StringAsInt(const char* value, int def_value)
{
    if (!value || !*value)
    {
        return def_value;
    }
	return atoi(value);
}

// �ַ���ת��Ϊ64����
int64_t StringUtil::StringAsInt64(const char* value)
{
	return Port_StringToInt64(value);
}

// �ַ���ת��Ϊ������
float StringUtil::StringAsFloat(const char* value, float def_value)
{
    if (!value || !*value)
    {
        return def_value;
    }
	return (float)atof(value);
}

// �ַ���ת��Ϊ����
int StringUtil::StringAsInt(const std::string& value, int def_value)	
{
	return StringAsInt(value.c_str(), def_value);
}

// �ַ���ת��Ϊ������
float StringUtil::StringAsFloat(const std::string& value, float def_value)
{
    if (value.empty())
    {
        return def_value;
    }
	return (float)atof(value.c_str());
}

// �ַ���ת��Ϊ������
 bool StringUtil::StringAsBool(const std::string& value, bool def_value)
 {
	if (stricmp(value.c_str(), "true")==0)
	{
		return true;
	}
	return false;
 }

// ����ת��Ϊ���ַ���
std::wstring StringUtil::IntAsWideStr(int value)
{
	wchar_t buf[32];

	Port_SafeSwprintf(buf, 32,L"%d", value);

	return std::wstring(buf);
}

// ������ת��Ϊ���ַ���
std::wstring StringUtil::FloatAsWideStr(float value, int decimals)
{
	wchar_t buf[32];

	if (0 == decimals)
	{
		Port_SafeSwprintf(buf, 32,L"%f", (double)value);
	}
	else
	{
		switch (decimals)
		{
		case 1:
			Port_SafeSwprintf(buf, 32,L"%.01f", (double)value);
			break;
		case 2:
			Port_SafeSwprintf(buf, 32,L"%.02f", (double)value);
			break;
		case 3:
			Port_SafeSwprintf(buf, 32,L"%.03f", (double)value);
			break;
		case 4:
			Port_SafeSwprintf(buf, 32,L"%.04f", (double)value);
			break;
		case 5:
			Port_SafeSwprintf(buf, 32,L"%.05f", (double)value);
			break;
		default:
			Port_SafeSwprintf(buf, 32,L"%f", (double)value);
			break;
		}
	}

	return std::wstring(buf);
}

// ���ַ���ת��Ϊ����
int StringUtil::WideStrAsInt(const wchar_t* value)
{
	char buf[32];

	ToString(value, buf, sizeof(buf));

	return atoi(buf);
}

// ���ַ���ת��Ϊ����
int64_t StringUtil::WideStrAsInt64(const wchar_t* value)
{
	return Port_WideStrToInt64(value);
}


// ���ַ���ת��Ϊ������
float StringUtil::WideStrAsFloat(const wchar_t* value)
{
	char buf[32];

	ToString(value, buf, sizeof(buf));

	return (float)atof(buf);
}

// ���ַ���ת��Ϊ����
int StringUtil::WideStrAsInt(const std::wstring& value)
{
	return WideStrAsInt(value.c_str());
}

// ���ַ���ת��Ϊ������
float StringUtil::WideStrAsFloat(const std::wstring& value)
{
	return WideStrAsFloat(value.c_str());
}

// ת��Ϊ��д���ַ���
std::string StringUtil::ToUpper(const char* str)
{
	TAutoMem<char, 256> auto_mem(strlen(str) + 1);
	char* data = auto_mem.GetBuffer();

	strcpy(data, str);

	return Port_StringUpper(data);
}

// ת��ΪСд���ַ���
std::string StringUtil::ToLower(const char* str)
{
	//Assert(str != NULL);

	TAutoMem<char, 256> auto_mem(strlen(str) + 1);

	char* data = auto_mem.GetBuffer();

	strcpy(data, str);

	return Port_StringLower(data);
}

// ȡ�Ӵ���pos��0��ʼ
std::string StringUtil::Substr(const char* str, size_t pos, size_t len)
{
	Assert(str != NULL);

	if (len == 0)
	{
		return std::string();
	}

	size_t i = pos + len;

	if (i > strlen(str))
	{
		return std::string();
	}

	std::string s(str);

	return s.substr(pos, len);
}

// ȡ��ָ���ַ����ָ����Ӵ��ֽ⵽Vector��
size_t StringUtil::ParseToVector(const char* str, char splitter, IVarList& result)
{
	result.Clear();

	if (str[0] == 0)
	{
		return 0;
	}

	size_t size = strlen(str) + 1;

	TAutoMem<char, 256> auto_buf(size);

	char* buffer = auto_buf.GetBuffer();

	memcpy(buffer, str, size);

	const char* res = buffer;

	char* s = buffer;
	for (; *s; ++s)
	{
		if (*s == splitter)
		{
			*s = 0;

			result.AddString(res);

			res = s + 1;
		}
	}

	if (res <= s)
	{
		result.AddString(res);
	}

	return result.GetCount();
}



// ���汾��ȡ��ָ���ַ����ָ����Ӵ��ֽ⵽Vector��
size_t StringUtil::ParseToVectorW(const wchar_t* str, wchar_t splitter, IVarList& result)
{
	result.Clear();

	if (str[0] == 0)
	{
		return 0;
	}

	size_t size = wcslen(str) + 1;

	TAutoMem<wchar_t, 256> auto_buf(size);

	wchar_t* buffer = auto_buf.GetBuffer();

	memcpy(buffer, str, size * sizeof(wchar_t));

	const wchar_t* res = buffer;

	wchar_t* s = buffer;
	for (; *s; ++s)
	{
		if (*s == splitter)
		{
			*s = 0;

			result.AddWideStr(res);

			res = s + 1;
		}
	}

	if (res <= s)
	{
		result.AddWideStr(res);
	}

	return result.GetCount();
}



// ��str�ַ������pat������replace
void StringUtil::StrReplace(std::string& str, const std::string& pat, const std::string& replace)
{
	size_t pos = 0;

	while (pos < str.size())
	{
		pos = str.find(pat, pos);

		if (pos == std::string::npos)
		{
			break;
		}

		str.replace(pos, pat.size(), replace);

		pos += replace.size();
	}
}

// ȥ��str�ַ�����ǰ���ָ���ַ�
std::string StringUtil::Trim(const std::string& str, const char ch)
{
	if (str.empty())
	{
		return "";
	}

	if (str[0] != ch && str[str.size()-1] != ch)
	{
		return str;
	}

	size_t pos_begin = str.find_first_not_of(ch, 0);
	size_t pos_end = str.find_last_not_of(ch, str.size());

	if (pos_begin == std::string::npos || pos_end == std::string::npos)
	{
		//���ʾ����ַ�������ַ�ȫ����ch��ɣ���������Ϊ�մ�
		return "";
	}

	return str.substr(pos_begin, pos_end - pos_begin + 1);
}

// ��ָ�����ַ��ָ��ַ���
size_t StringUtil::SplitString(const char* str, char splitter, char* buffer, 
							   size_t buffer_size, const char* result[], size_t result_max)
{
	Assert(str != NULL);
	Assert(buffer != NULL);

	if (str[0] == 0)
	{
		return 0;
	}

	CopyString(buffer, buffer_size, str);

	size_t count = 0;
	const char* res = buffer;

	char* s = buffer;
	for (; *s; ++s)
	{
		if (*s == splitter)
		{
			*s = 0;

			result[count++] = res;

			if (count == result_max)
			{
				return count;
			}

			res = s + 1;
		}
	}

	if (res <= s)
	{
		result[count++] = res;
	}

	return count;
}

void StringUtil::SplitString(IVarList & result, const std::string& str,
                        const std::string& delims, unsigned int max_splits)
{
    if (0 == str.compare(""))
    {
        return;
    }

    unsigned int num_splits = 0;

    size_t start, pos;
    start = 0;
    do 
    {
        pos = str.find_first_of(delims, start);
        if (pos == std::string::npos || (max_splits > 0 && num_splits == max_splits))
        {
            result << Trim(str.substr(start)).c_str();
            break;
        }
        else
        {
            result << Trim(str.substr(start, pos - start)).c_str();
            start = pos + delims.length();
        }

        num_splits++;

    } while (pos != std::string::npos);
    return;
}


// ���ַ��汾�ĺ�����ͬ��
std::wstring StringUtil::ToUpperW(const wchar_t* str)
{
	Assert(str != NULL);

	TAutoMem<wchar_t, 256> auto_mem(wcslen(str) + 1);

	wchar_t* data = auto_mem.GetBuffer();

	wcscpy(data, str);

	return Port_WideStrUpper(data);
}

std::wstring StringUtil::ToLowerW(const wchar_t* str)
{
	Assert(str != NULL);

	TAutoMem<wchar_t, 256> auto_mem(wcslen(str) + 1);

	wchar_t* data = auto_mem.GetBuffer();

	wcscpy(data, str);

	return Port_WideStrLower(data);
}

std::wstring StringUtil::SubstrW(const wchar_t* str, size_t pos, size_t len)
{
	Assert(str != NULL);

	if (len == 0)
	{
		return std::wstring();
	}

	size_t i = pos + len;

	if (i > wcslen(str))
	{
		return std::wstring();
	}

	std::wstring s(str);

	return s.substr(pos, len);
}

void StringUtil::StrReplaceW(std::wstring& str, const std::wstring& pat, const std::wstring& replace)
{
	size_t pos = 0;

	while (pos < str.size())
	{
		pos = str.find(pat, pos);

		if (pos == std::wstring::npos)
		{
			break;
		}

		str.replace(pos, pat.size(), replace);

		pos += replace.size();
	}
}

// ȥ��str�ַ�����ǰ���ָ���ַ�
std::wstring StringUtil::TrimW(const std::wstring& str, const wchar_t ch)
{
	if (str.empty())
	{
		return L"";
	}

	if (str[0] != ch && str[str.size() - 1] != ch)
	{
		return str;
	}

	size_t pos_begin = str.find_first_not_of(ch, 0);
	size_t pos_end = str.find_last_not_of(ch, str.size());

	if (pos_begin == std::wstring::npos || pos_end == std::wstring::npos)
	{
		//���ʾ����ַ�������ַ�ȫ����ch��ɣ���������Ϊ�մ�
		return L"";
	}

	return str.substr(pos_begin, pos_end - pos_begin);
}

// ��ָ�����ַ��ָ���ַ���
size_t StringUtil::SplitStringW(const wchar_t* str, wchar_t splitter, 
								wchar_t* buffer, size_t buffer_size, const wchar_t* result[], size_t result_max)
{
	Assert(str != NULL);
	Assert(buffer != NULL);

	if (str[0] == 0)
	{
		return 0;
	}

	CopyWideStr(buffer, buffer_size, str);

	size_t count = 0;
	const wchar_t* res = buffer;

	wchar_t* s = buffer;
	for (; *s; ++s)
	{
		if (*s == splitter)
		{
			*s = 0;

			result[count++] = res;

			if (count == result_max)
			{
				return count;
			}

			res = s + 1;
		}
	}

	if (res <= s)
	{
		result[count++] = res;
	}

	return count;
}

// �ָ��ַ���
void StringUtil::SplitStringW(IVarList & result, const std::wstring& str,
                        const std::wstring& delims, unsigned int max_splits)
{
    if (0 == str.compare(L""))
    {
        return;
    }

    unsigned int num_splits = 0;

    size_t start, pos;
    start = 0;
    do 
    {
        pos = str.find_first_of(delims, start);
        if (pos == start)
        {
            start = pos + 1;
        }
        else if (pos == std::wstring::npos || (max_splits > 0 && num_splits == max_splits))
        {
            result << TrimW(str.substr(start)).c_str();
            break;
        }
        else
        {
            result << TrimW(str.substr(start, pos - start)).c_str();
            start = pos + 1;
        }

        num_splits++;

    } while (pos != std::wstring::npos);
    return;
}

// �Ӵ����ļ�·������ȡ����������
std::string StringUtil::FormFileToName(const std::string& form_file)
{
	size_t lst_slash = form_file.find_last_of('/');
	size_t lst_dot = form_file.find_last_of('.');

	return form_file.substr(lst_slash+1, lst_dot-lst_slash-1);
}

