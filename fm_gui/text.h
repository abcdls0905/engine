//--------------------------------------------------------------------
// 文件名:		Text.h
// 内  容:		文本管理器，由天子文本管理(张玲春所写) 改得
// 说  明:		
// 创建日期:	2008年9月3日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
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

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
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
	// 数字到单位换算格式结构，当单位>=2时才需要用到这个功能------------------------
	// 数字到单位换算显示模式
	enum ConverShowMode
	{
		MONEY_MODE_SHOW_ALL = 0,        // 显示所有,         如:0锭1两1文, 0锭也被显示
		MONEY_MODE_SHOW_BEHIND = 1,     // 显示存在及之后    如:1两0文, 0锭在之前不被显示，0文在之后被显示
		MONEY_MODE_SHOW_HAVE = 2,       // 仅显示存在        如:1两   不存在的0锭和0文都不被显示   
	};

	// 数字到单位换算中的进制单位,如锭,两,文都是
	struct ConverItem
	{
		std::wstring wcsDanWei;         // 单位实际文本,如锭
		int nJinZhi;                    // 达到这个单位需要的值如1两就要100文那nJinzhi就是100
		int nNum;                       // 位数
	};

	// 数字到单位换算格式总结构
	struct ConvertionFormat
	{
		bool isAlign;                   // 是否位对齐,补满倍数
		std::wstring wcsFlag;           // 文本配置中的标记
		ConverShowMode eFormatMode;     // 显示模式
		TArrayPod<ConverItem*, 1, TCoreAlloc> m_vConverItems; // 所有的单位
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
	
	// 初始化（可创建其他附属实体）
	virtual bool Init(const IVarList& args);
	// 关闭（可删除其他引用的附属实体，而资源释放应在析构函数中进行）
	virtual bool Shut();

    // 设置浮点数，小数位数
    void SetFloatNumber(const int number);
    int GetFloatNumber();

    // 加载时是否检查重复键值
    void SetCheckRepeatID(const bool number);
    bool GetCheckRepeatID();

    // 语言包，转国际化时检查遗漏
    bool TextUsedTest();

	// 取得完整文本资源读取目录，默认读取目录为 resource\ini\text\china
	const char* GetFullTextPath() const;
	// 读取文本资源
	bool LoadFiles(const char* path);
    // 读取完毕后生成index, 加速查找
    bool RefreshIndices();
    // 清理
    bool Clear();
	// 根据文本字符串标识取得其数字标识
	long GetTextID(const char* IDName);
    // 验证文本数字标识是否合法
    bool IsVaildID(long textid);
    // 根据数据标识取得显示文本
   	std::wstring GetText(long textid);
    // 判断文本是否是IDNAME
    bool IsIDName(const char* IDName);
	// 通过文本ID获取原始配置文本
	const wchar_t* GetTextSource(long textid);
	// 根据文本标识名取得显示文本
	std::wstring GetText(const char* pIDName);
	// 从数据流中取得显示文本
	std::wstring GetText(const char* IDName, IVarList& Params, 
		size_t& paramnum);
	// 从数据流中取得显示文本
	std::wstring GetText(const char* IDName, IVarList& Params);
	// 设置文本ID，对应内容
	bool SetText(const char* szIDName, const wchar_t* wcsText);
	// 根据IDNAME得到其参数数量
	size_t GetTextParam(const char* IDName);
    // 格式化字符串的一组接口
    bool Format_SetIDName(const char* pIDName);
    void Format_AddParam(const IVarList& args, IVarList& ret);
    std::wstring Format_GetText();
    // 获取格式化字符串
    void GetFormatText(const IVarList& args, IVarList& ret);
    // 添加单位换算格式化规则
    void AddConvertionFormatRule(const IVarList& args, IVarList& ret);
    // 获取单位换算后的文本
    std::wstring GetConvertionFormatString(const wchar_t* pFlag, 
		int64_t value);

private:
	// 从数据流中取得显示文本
	std::wstring GetText(CFlexString& FlexStr, const char* IDName, 
		IVarList& Params, size_t& paramnum);
	// 读取文本的二进制格式
	bool LoadTextByBinary();
	// 读取文本的ascii格式
	void LoadTextByAscii();
	// 根据文本id，当前附本位置生成一个新的文本id
	long MakeCopy(long id, long copy);
	// 更新当前文件资源读取完整路径
	void UpdataFullPath();
	// 加载单个文件
	void LoadFile(const char* filename);

    // 记录被使用过的ID
    void SaveUsedID(const char* idname);
    // 记录漏网的ID
    void SaveNoAddID(const char* idname);

private:
    // 是否检查重复键值
    bool m_bCheckRepeatID;
    //std::map<std::string, bool> m_vCheckData;
	TLiteralPod<char, bool, TStringTraits<char>, TTextAlloc> m_mapCheckData;

    //std::map<std::string, bool> m_mapCheckUsedID;
	TStringPod<char, bool, TStringTraits<char>, TTextAlloc> m_mapCheckUsedID;
    FILE* m_pCheckUsedIDFile;       // 记录被用过的ID

    //std::map<std::string, bool> m_mapAllIDName;
    //std::map<std::string, bool> m_mapNoAddIDName;
	TStringPod<char, bool, TStringTraits<char>, TTextAlloc> m_mapAllIDName;
	TStringPod<char, bool, TStringTraits<char>, TTextAlloc> m_mapNoAddIDName;
    FILE* m_pNoAddIDFile;           // 记录漏网的ID
    // 浮点数保留几位小数
    int m_nFloatNumber;
	
	TArrayPod<text_item_t*, 1, TTextAlloc> m_Texts;
	// 文本索引，用来快速寻找文本,section到m_text下标的MAP
	unsigned int* m_pTextIndices;
	size_t m_nTextIndexSize;

	// 文本资源完整读取路径在调用LoadFiles时由脚本传入
	std::string m_strFullPath;

    std::string m_FormatString;
    CVarList m_FormatParams;

    // 应用举例：
    // 1.应用范围
    // 当以一个数字进行多进制多单位格式化表示时用到此功能，其它无需使用
    // 如:23400466 => 2340锭4两66文   13 => 1亩3分
    TArrayPod<ConvertionFormat*, 1, TCoreAlloc> m_vConvertions;
   	TStringPod<wchar_t, size_t, TStringTraits<wchar_t>, 
		TTextAlloc> m_mapConvertions;

#ifdef __USE_MEMPOOL__
	CMemPool m_mempool;
#endif
};

#endif //Text_h_2008_9_3

