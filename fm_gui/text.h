//--------------------------------------------------------------------
// �ļ���:		Text.h
// ��  ��:		�ı����������������ı�����(���ᴺ��д) �ĵ�
// ˵  ��:		
// ��������:	2008��9��3��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef Text_h_2008_9_3
#define Text_h_2008_9_3

#include "../public/module.h"
#include "../public/i_var_list.h"
#include "../utils/string_pod.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "../utils/string_pod.h"
#include "../utils/literal_pod.h"
#include "flexstring.h"

//#include "../public/CoreType.h"
//#include "../utils/ArrayPod.h"
//#include "../utils/string_pod.h"
//

#include <string>
#include <vector>

class TTextAlloc
{
public:
	TTextAlloc() {}
	~TTextAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TTextAlloc& src) {}
};

//#define __USE_MEMPOOL__
#ifdef __USE_MEMPOOL__
#include "MemPool2.h"
#endif

// Text

class Text: public IEntity
{
private:
	// ���ֵ���λ�����ʽ�ṹ������λ>=2ʱ����Ҫ�õ��������------------------------
	// ���ֵ���λ������ʾģʽ
	enum ConverShowMode
	{
		MONEY_MODE_SHOW_ALL = 0,        // ��ʾ����,         ��:0��1��1��, 0��Ҳ����ʾ
		MONEY_MODE_SHOW_BEHIND = 1,     // ��ʾ���ڼ�֮��    ��:1��0��, 0����֮ǰ������ʾ��0����֮����ʾ
		MONEY_MODE_SHOW_HAVE = 2,       // ����ʾ����        ��:1��   �����ڵ�0����0�Ķ�������ʾ   
	};

	// ���ֵ���λ�����еĽ��Ƶ�λ,�綧,��,�Ķ���
	struct ConverItem
	{
		std::wstring wcsDanWei;         // ��λʵ���ı�,�綧
		int nJinZhi;                    // �ﵽ�����λ��Ҫ��ֵ��1����Ҫ100����nJinzhi����100
		int nNum;                       // λ��
	};

	// ���ֵ���λ�����ʽ�ܽṹ
	struct ConvertionFormat
	{
		bool isAlign;                   // �Ƿ�λ����,��������
		std::wstring wcsFlag;           // �ı������еı��
		ConverShowMode eFormatMode;     // ��ʾģʽ
		TArrayPod<ConverItem*, 1, TCoreAlloc> m_vConverItems; // ���еĵ�λ
	};

	struct text_item_t
	{
		unsigned int nNext;
		unsigned int nHash;
		wchar_t* pText;
		char pName[1];
	};

public:
	Text();
	virtual ~Text();
	
	// ��ʼ�����ɴ�����������ʵ�壩
	virtual bool Init(const IVarList& args);
	// �رգ���ɾ���������õĸ���ʵ�壬����Դ�ͷ�Ӧ�����������н��У�
	virtual bool Shut();

    // ���ø�������С��λ��
    void SetFloatNumber(const int number);
    int GetFloatNumber();

    // ����ʱ�Ƿ����ظ���ֵ
    void SetCheckRepeatID(const bool number);
    bool GetCheckRepeatID();

    // ���԰���ת���ʻ�ʱ�����©
    bool TextUsedTest();

	// ȡ�������ı���Դ��ȡĿ¼��Ĭ�϶�ȡĿ¼Ϊ resource\ini\text\china
	const char* GetFullTextPath() const;
	// ��ȡ�ı���Դ
	bool LoadFiles(const char* path);
    // ��ȡ��Ϻ�����index, ���ٲ���
    bool RefreshIndices();
    // ����
    bool Clear();
	// �����ı��ַ�����ʶȡ�������ֱ�ʶ
	long GetTextID(const char* IDName);
    // ��֤�ı����ֱ�ʶ�Ƿ�Ϸ�
    bool IsVaildID(long textid);
    // �������ݱ�ʶȡ����ʾ�ı�
   	std::wstring GetText(long textid);
    // �ж��ı��Ƿ���IDNAME
    bool IsIDName(const char* IDName);
	// ͨ���ı�ID��ȡԭʼ�����ı�
	const wchar_t* GetTextSource(long textid);
	// �����ı���ʶ��ȡ����ʾ�ı�
	std::wstring GetText(const char* pIDName);
	// ����������ȡ����ʾ�ı�
	std::wstring GetText(const char* IDName, IVarList& Params, 
		size_t& paramnum);
	// ����������ȡ����ʾ�ı�
	std::wstring GetText(const char* IDName, IVarList& Params);
	// �����ı�ID����Ӧ����
	bool SetText(const char* szIDName, const wchar_t* wcsText);
	// ����IDNAME�õ����������
	size_t GetTextParam(const char* IDName);
    // ��ʽ���ַ�����һ��ӿ�
    bool Format_SetIDName(const char* pIDName);
    void Format_AddParam(const IVarList& args, IVarList& ret);
    std::wstring Format_GetText();
    // ��ȡ��ʽ���ַ���
    void GetFormatText(const IVarList& args, IVarList& ret);
    // ��ӵ�λ�����ʽ������
    void AddConvertionFormatRule(const IVarList& args, IVarList& ret);
    // ��ȡ��λ�������ı�
    std::wstring GetConvertionFormatString(const wchar_t* pFlag, 
		int64_t value);

private:
	// ����������ȡ����ʾ�ı�
	std::wstring GetText(CFlexString& FlexStr, const char* IDName, 
		IVarList& Params, size_t& paramnum);
	// ��ȡ�ı��Ķ����Ƹ�ʽ
	bool LoadTextByBinary();
	// ��ȡ�ı���ascii��ʽ
	void LoadTextByAscii();
	// �����ı�id����ǰ����λ������һ���µ��ı�id
	long MakeCopy(long id, long copy);
	// ���µ�ǰ�ļ���Դ��ȡ����·��
	void UpdataFullPath();
	// ���ص����ļ�
	void LoadFile(const char* filename);

    // ��¼��ʹ�ù���ID
    void SaveUsedID(const char* idname);
    // ��¼©����ID
    void SaveNoAddID(const char* idname);

private:
    // �Ƿ����ظ���ֵ
    bool m_bCheckRepeatID;
    //std::map<std::string, bool> m_vCheckData;
	TLiteralPod<char, bool, TStringTraits<char>, TTextAlloc> m_mapCheckData;

    //std::map<std::string, bool> m_mapCheckUsedID;
	TStringPod<char, bool, TStringTraits<char>, TTextAlloc> m_mapCheckUsedID;
    FILE* m_pCheckUsedIDFile;       // ��¼���ù���ID

    //std::map<std::string, bool> m_mapAllIDName;
    //std::map<std::string, bool> m_mapNoAddIDName;
	TStringPod<char, bool, TStringTraits<char>, TTextAlloc> m_mapAllIDName;
	TStringPod<char, bool, TStringTraits<char>, TTextAlloc> m_mapNoAddIDName;
    FILE* m_pNoAddIDFile;           // ��¼©����ID
    // ������������λС��
    int m_nFloatNumber;
	
	TArrayPod<text_item_t*, 1, TTextAlloc> m_Texts;
	// �ı���������������Ѱ���ı�,section��m_text�±��MAP
	unsigned int* m_pTextIndices;
	size_t m_nTextIndexSize;

	// �ı���Դ������ȡ·���ڵ���LoadFilesʱ�ɽű�����
	std::string m_strFullPath;

    std::string m_FormatString;
    CVarList m_FormatParams;

    // Ӧ�þ�����
    // 1.Ӧ�÷�Χ
    // ����һ�����ֽ��ж���ƶ൥λ��ʽ����ʾʱ�õ��˹��ܣ���������ʹ��
    // ��:23400466 => 2340��4��66��   13 => 1Ķ3��
    TArrayPod<ConvertionFormat*, 1, TCoreAlloc> m_vConvertions;
   	TStringPod<wchar_t, size_t, TStringTraits<wchar_t>, 
		TTextAlloc> m_mapConvertions;

#ifdef __USE_MEMPOOL__
	CMemPool m_mempool;
#endif
};

#endif //Text_h_2008_9_3

