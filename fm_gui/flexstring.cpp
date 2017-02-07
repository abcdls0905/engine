//--------------------------------------------------------------------
// 文件名:		FlexString.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年9月26日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "flexstring.h"
#include "../public/inlines.h"
#include "../public/core_mem.h"
#include "../tools/string_util.h"

bool IsNumber(wchar_t cchar)
{
	if ((unsigned char)cchar >= 0x30 && (unsigned char)cchar <= 0x39)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//sFmtText是格式化的文本资源
CFlexString::CFlexString(const std::wstring& sFmtText)
{
	Clear();	

    m_nFloatNumber = 2;
	m_IndexVec.reserve(256);	
	m_sTxtresbuf = sFmtText;

    m_vItems.clear();

    StringItem item;

    item.IsVariable = false;
    item.wcsText = L"";

    for (int i = 0; i < int(m_sTxtresbuf.size()); i++)
    {
        int nIndex = -1;//标记里面保存的数字
        int nOffset = 0;

        nOffset = FindMark(i, m_sTxtresbuf, nIndex);

        if (nOffset != -1)
        {
            if (item.wcsText.size() > 0)
            {
                m_vItems.push_back(item);
            }

            item.IsVariable = true;
            item.VariableIndex = nIndex;
            item.wcsText = m_sTxtresbuf.substr(i, nOffset);

            m_vItems.push_back(item);
            
            item.IsVariable = false;
            item.wcsText = L"";

            i += nOffset;
        }
        else
        {
            item.wcsText += m_sTxtresbuf[i];
        }
    }

    if (item.wcsText.size() > 0)
    {
        m_vItems.push_back(item);
    }
}

CFlexString::~CFlexString()
{
	Clear();
}

void CFlexString::Clear()
{
	for (size_t i = 0; i < m_values.size(); i++)
	{
		if (m_values[i].type == VT_STRING)
		{
			wchar_t* ws = (wchar_t*)m_values[i].Value._s;

			CORE_FREE(ws, (wcslen(ws) + 1) * sizeof(wchar_t));
		}
	}

	m_IndexVec.clear();
	m_values.clear();
	m_sTxtresbuf = L"";
}

CFlexString& CFlexString::operator<<(float value)
{
	ValueUnit v;

	v.type = VT_FLOAT;
	v.Value._f = value;
	
	m_values.push_back(v);

	return *this;
}

CFlexString& CFlexString::operator<<(int value)
{
	ValueUnit v;

	v.type = VT_INTERNAL;
	v.Value._l  = value;
	
	m_values.push_back(v);

	return *this;
}

CFlexString& CFlexString::operator<<(unsigned int value)
{
	return operator<<((int)value);
}

CFlexString& CFlexString::operator<<(long value)
{
	ValueUnit v;

	v.type = VT_INTERNAL;
	v.Value._l = value;

	m_values.push_back(v);

	return *this;
}

CFlexString& CFlexString::operator<<(unsigned long value)
{
	return operator<<((long)value);
}

CFlexString& CFlexString::operator <<(const wchar_t* s)
{
	size_t val_size = (wcslen(s) + 1) * sizeof(wchar_t);
	wchar_t* val = (wchar_t*)CORE_ALLOC(val_size);

	memcpy(val, s, val_size);
	
	ValueUnit v;

	v.type = VT_STRING;
	v.Value._s = val;

	m_values.push_back(v);

	return *this;
}

CFlexString& CFlexString::operator <<(const std::wstring& str)
{
	return (*this) << str.c_str();
}

std::wstring CFlexString::operator<<(wchar_t cchar)
{
	if (cchar == FmtEnd)
	{
		return GetStr();
	}
	else
	{
		return L"";
	}
}

//如果返回-1,说明没有找到标记或者标记有问题
int CFlexString::FindMark(int nCurPos, std::wstring& sTxtRes,int& nIndex)
{
	if (sTxtRes[nCurPos] == '{')
	{
		int xx = 0;
	}

	std::wstring sNumbuf;

	sNumbuf.reserve(8);

	//{@0:}	,如果有标记在,至少从'{'开始,右边有4个字符
	//{@0}	,如果不写注释,没有':',则从'{'开始,右边有3个字符
	//CFlexString中不鼓励这样的情况
	if (size_t(nCurPos + 3) < sTxtRes.size() && sTxtRes[nCurPos] == '{' 
		&& sTxtRes[nCurPos + 1] == '@')
	{
		int i = nCurPos + 2;

		for (; i < int(sTxtRes.size()); i++)
		{
			wchar_t cxxxx = sTxtRes[i];

			if (IsNumber(sTxtRes[i]))
			{
				sNumbuf += sTxtRes[i];
			}
			else if (sTxtRes[i] == ':')
			{
				if (sNumbuf.size() == 0 || i + 1 >= int(sTxtRes.size()))
				{
					return -1;
				}

				nIndex = StringUtil::WideStrAsInt(sNumbuf);
				//nIndex = _wtoi(sNumbuf.c_str());
				if (nIndex < 0)
				{
					return -1;
				}

				int n = 0;

				for (n = i + 1; n < int(sTxtRes.size()); n++)
				{
					if (sTxtRes[n] == '}')
					{
						break;
					}
				}

				return (n - nCurPos);//返回
			}
			else if (sTxtRes[i] == '}')
			{
				if (sNumbuf.size() == 0)
				{
					return -1;
				}
				nIndex = StringUtil::WideStrAsInt(sNumbuf);
				//nIndex = _wtoi(sNumbuf.c_str());

				if (nIndex < 0)
				{
					return -1;
				}

				return (i - nCurPos);
			}
			else
			{
				return -1;
			}
		}
	}

	return -1;
}

//尚未支持类似于sprintf中的"Format Specification"( "%8.2d这样的东西
//现在还没有指定m_values[nIndex].sFmtInfo中的信息的接口
std::wstring CFlexString::GetVarStr(int nIndex)
{
	if (size_t(nIndex) >= m_values.size())
	{
		return L"";
	}
	
	int type = m_values[nIndex].type;

	wchar_t buf[2048];

	switch (type)
	{
	case VT_FLOAT:
        if (m_nFloatNumber == 0)
        {
            SafeSwprintf(buf, sizeof(buf), L"%d", 
				(int)m_values[nIndex].Value._f);
        }
        else
        {
            SafeSwprintf(buf, sizeof(buf), L"%df", m_nFloatNumber);
            std::wstring wcsBuffer = L"%.";
            wcsBuffer += buf;
            SafeSwprintf(buf, sizeof(buf), wcsBuffer.c_str(), 
				m_values[nIndex].Value._f);
        }
        break;
	case VT_INTERNAL:
	    SafeSwprintf(buf, sizeof(buf), L"%d", m_values[nIndex].Value._l);
		break;
	case VT_STRING:
		CopyWideStr(buf, sizeof(buf), m_values[nIndex].Value._s);
		break;
	}

	return buf;
}

//计划做的扩展:格式化的字符资源如下. 如果存在fmtinfo的值,fmtinfo后面的字符串的定义规则等同于printf
//fmt2=FMTSTRING fmtstr("单价{@0:价格}银币,购买@{1:数量}个{@2:物品名},共花费{@6:总费用}银币,确定吗?");
//0.fmtinfo=%6d;
std::wstring CFlexString::GetStr()
{
	std::wstring sResult;

	sResult.reserve(512);

	for (size_t i = 0; i < m_vItems.size();i++)
	{
		if (m_vItems[i].IsVariable)
		{
             sResult += GetVarStr(m_vItems[i].VariableIndex);
		}
		else
		{
			sResult += m_vItems[i].wcsText;
		}
	}

	return sResult;
}

int CFlexString::GetVarAmount()
{
	if (m_IndexVec.size() != 0)
	{
		return (int)m_IndexVec.size();
	}

	int nVarAmount = 0;

	// 统计有几个不同的变量序号
	for (int i = 0; i < int(m_sTxtresbuf.size()); i++)
	{
		int nIndex = -1;//标记里面保存的数字
		int nOffset = 0;

		nOffset = FindMark(i, m_sTxtresbuf, nIndex);

		if (nOffset != -1)
		{
			PushVarIndex_NoRpt(nIndex);

			i += nOffset;
		}
	}

	return (int)m_IndexVec.size();
}

void CFlexString::PushVarIndex_NoRpt(int nIndex)
{
	std::vector<int>::iterator it1 = m_IndexVec.begin();
	std::vector<int>::iterator end1 = m_IndexVec.end();

	for (; it1 != end1; ++it1)
	{
		if (nIndex == (*it1))
		{
			return;
		}
	}

	m_IndexVec.push_back(nIndex);
}

std::wstring CFlexString::GetVarInfo(int index)
{
    for (size_t i = 0; i < m_vItems.size(); ++i)
    {
        if (m_vItems[i].IsVariable && m_vItems[i].VariableIndex == index)
        {
            return m_vItems[i].wcsText;
        }
    }

	return std::wstring();
}

// 设置浮点数，小数位数
void  CFlexString::SetFloatNumber(const int number)
{
    if (number < 0 || number > 10)
    {
        return;
    }

    m_nFloatNumber = number;
}

int  CFlexString::GetFloatNumber()
{
    return m_nFloatNumber;
}
