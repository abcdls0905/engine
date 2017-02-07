//--------------------------------------------------------------------
// 文件名:		FlexString.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年9月26日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

/*
例子:
std::string fmtinfo="单价{@0:价格}银币,一共购买{@1:数量}个{@2:物品名字}";
std::string sResult = CFlexString(fmtinfo)<< nPrice << nNum << sItemName << FmtEnd;
注意在代码中用"<<"运算符号输入变量的顺序和变量标记 {@X:xxx}中的序号保持一致!

说明,除了正常文本外,形如” {@2:物品名字}”这样的标记里各段的意义:
{@			标记的开始
2:			变量序号
物品名字	变量注释,不影响组合句子的结果
}			标记的结尾
*/

#ifndef __Flexstring_h
#define __Flexstring_h

#include <stdarg.h>
#include <string>
#include <vector>

#define FmtEnd ((wchar_t)0x01)
#define LOCALIZE(s)	FindLocalizeName(pKernel, s)
#define COMMSTR(s) FindLocalizestd::string(pKernel, s)

// CFlexString

class CFlexString
{
public:
	enum {VT_FLOAT, VT_STRING, VT_INTERNAL};

	struct ValueUnit
	{
		int type;

		union
		{
			float _f;
			long  _l;
			const wchar_t* _s;
		} Value;
	};

private:
	struct StringItem
	{
		bool IsVariable;
		int VariableIndex;
		std::wstring wcsText;
	};

public:
	CFlexString(const std::wstring& sFmtText);//输入参数是格式化的支持变量的文本资源
	virtual ~CFlexString();

	CFlexString& operator <<(float);
	CFlexString& operator <<(int);
	CFlexString& operator <<(unsigned int);
	CFlexString& operator <<(long);
	CFlexString& operator <<(unsigned long);
	CFlexString& operator <<(const wchar_t*);
	CFlexString& operator <<(const std::wstring&);
	//
	std::wstring operator <<(wchar_t);//  XXX<<(FmtEnd) 的效果等同于 XXX.GetStr()
	
	std::wstring GetStr();//得到组合好的正常的字符串
	int GetVarAmount();

    // 获取变量的附加信息
    std::wstring  GetVarInfo(const int index);

    // 设置浮点数，小数位数
    void SetFloatNumber(const int number);
    int GetFloatNumber();
private:
	void Clear(); //初始化内部的成员变量,为再次使用做准备

	inline std::wstring GetVarStr(int nIndex);

	int FindMark(int nCurPos, std::wstring& sTxtRes,int& nIndex);

	void PushVarIndex_NoRpt(int nIndex);

private:
    int m_nFloatNumber; // 浮点数小数位数
	std::vector<ValueUnit>	m_values;
	std::vector<int> m_IndexVec;
	std::wstring m_sTxtresbuf;//用于保存格式化的字符串资源
    std::vector<StringItem> m_vItems;
};

#endif 
