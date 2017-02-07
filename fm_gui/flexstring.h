//--------------------------------------------------------------------
// �ļ���:		FlexString.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��9��26��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

/*
����:
std::string fmtinfo="����{@0:�۸�}����,һ������{@1:����}��{@2:��Ʒ����}";
std::string sResult = CFlexString(fmtinfo)<< nPrice << nNum << sItemName << FmtEnd;
ע���ڴ�������"<<"����������������˳��ͱ������ {@X:xxx}�е���ű���һ��!

˵��,���������ı���,���硱 {@2:��Ʒ����}�������ı������ε�����:
{@			��ǵĿ�ʼ
2:			�������
��Ʒ����	����ע��,��Ӱ����Ͼ��ӵĽ��
}			��ǵĽ�β
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
	CFlexString(const std::wstring& sFmtText);//��������Ǹ�ʽ����֧�ֱ������ı���Դ
	virtual ~CFlexString();

	CFlexString& operator <<(float);
	CFlexString& operator <<(int);
	CFlexString& operator <<(unsigned int);
	CFlexString& operator <<(long);
	CFlexString& operator <<(unsigned long);
	CFlexString& operator <<(const wchar_t*);
	CFlexString& operator <<(const std::wstring&);
	//
	std::wstring operator <<(wchar_t);//  XXX<<(FmtEnd) ��Ч����ͬ�� XXX.GetStr()
	
	std::wstring GetStr();//�õ���Ϻõ��������ַ���
	int GetVarAmount();

    // ��ȡ�����ĸ�����Ϣ
    std::wstring  GetVarInfo(const int index);

    // ���ø�������С��λ��
    void SetFloatNumber(const int number);
    int GetFloatNumber();
private:
	void Clear(); //��ʼ���ڲ��ĳ�Ա����,Ϊ�ٴ�ʹ����׼��

	inline std::wstring GetVarStr(int nIndex);

	int FindMark(int nCurPos, std::wstring& sTxtRes,int& nIndex);

	void PushVarIndex_NoRpt(int nIndex);

private:
    int m_nFloatNumber; // ������С��λ��
	std::vector<ValueUnit>	m_values;
	std::vector<int> m_IndexVec;
	std::wstring m_sTxtresbuf;//���ڱ����ʽ�����ַ�����Դ
    std::vector<StringItem> m_vItems;
};

#endif 
