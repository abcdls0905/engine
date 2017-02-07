//--------------------------------------------------------------------
// 文件名:		tinyhtml.h
// 内  容:		解析HTML文件的类
// 说  明:		实现图文混排框数据读取解析类
// 创建日期:	2006年10月24日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef tinyhtml_h_27_9_2006_10_24
#define tinyhtml_h_27_9_2006_10_24

#include "../public/persistid.h"
#include "gui_utils.h"
#pragma  warning (disable:4786)
#include <string>
#include <vector>

typedef unsigned long dword;

inline	rect_t MAKE_RECT(long x1,long y1,long x2,long y2)
{
	rect_t r1;
	
	r1.top = y1;
	r1.left = x1;
	r1.bottom = y2;
	r1.right = x2;
	
	return r1;
}

// 注意：字体尽量用偶数大小的如12,14，不要用13
struct HtmlItem
{
    long m_Type;
    long m_Width;
};

// 初始化一个矩形结构
// 图片格式
struct HtmlImageItem : public HtmlItem
{
	HtmlImageItem(): m_imagesrc(L""), m_Rect(MAKE_RECT(0,0,0,0)),
		m_HAlign(L""), m_VAlign(L""),m_nOnly(0), m_bNeedRelease(false),
		m_bIsAnimation(false), m_MarkStr(""), m_nType(0){}

	std::wstring m_imagesrc;	// 图元名
	rect_t m_Rect;				// 矩形区域
	std::wstring m_HAlign;		// 左右对齐方式
	std::wstring m_VAlign;		// 上下对齐方式
	PERSISTID m_imageID;		// 图元编号,这是通过dx创建后得到的
    int m_nType;				// 0表示TGA,1表示PNG,2表示animation	
    int m_nOnly;				// 1(true)为独占行自为一行,0(false)为不独占文字环绕模式,2(line)为和文字一起单行,不再环绕
	bool m_bNeedRelease;		// 删除时是否需要手动清理
	bool m_bIsAnimation;		// 是否是动画图元
	std::string m_MarkStr;		// 图片数据
};

// 文字格式
struct HtmlTextItem : public HtmlItem
{
	HtmlTextItem(dword incolor = 0xffffffff, const char* inszFace = "Default",
		long inlFontSize = 12)
	{
		m_text = "";
		m_wtext = L"";
		// 默认是 黑色 宋体 12
		m_Color	= incolor;
		m_Face = inszFace;
		m_FontSize = inlFontSize;
		m_IsHyperLink = false;
	}

	std::string m_text;
	std::wstring m_wtext;
	dword m_Color;		// 颜色
	long m_FontSize;		// 大小
	//-------------------------2006_09_27--add by ylp------
	std::string m_Face;			// 字体 
	//-------------------------2008_08_22--add by ylp------新加入超链接支持
	bool m_IsHyperLink;
	std::string m_szHPStyle;	//style="XXX"
	std::wstring m_wcsLink;	//href="XXX"
};

// 段格式，
// 所有要换行的地方都要设为[段]即一个TextSegment,[段]中也有图片和文本
// 文字居中的格式形式作为居中的独立段来处理
struct TextSegment
{
	TextSegment()
	{
		m_bInsertTwoSpace=false;
		m_bCenter = false;
		m_TextItemList.clear();
		m_ImageItemList.clear();
	}

	std::vector<HtmlTextItem> m_TextItemList;
	std::vector<HtmlImageItem> m_ImageItemList;
	// 文字和图片处理顺序
	std::vector<bool> m_processlist;//true为Text,false为image
	//-------------------------2006_09_27--add by ylp------
	// 居中的功能只能在该段所有文字长度小于行宽的时候有效，不然将被乎略
	bool m_bCenter;
	// 可设置<s/>默认为两格，也可加入size属性指定空的个数<s size="3"/>
	// 这个仅在m_bCenter==false时才被使用
	bool m_bInsertTwoSpace;
};

/**
一般格式
<body>
	文字&图片
</body>

图片格式:<img src="" rect="" align="" />
文本格式_换    行	:<br/>
文本格式_首行缩进	:<s size=""/>,默认缩进两个字符时直接用<s/>即可,必须紧跟在<br/>之后
文本格式_居    中	:<center>文本</center>
文本格式_字体颜色大小:<font face="字体" color="#ffffff(颜色)" size="大小"> 
					      文字
					  </font>
*/
/**
当前支持的标签说明

<body></body>
如果有这个标签,则处理时只会取这两个标签内的文本进行处理,其它的将被忽略将被处理其它的将被乎略；如果没这个标签，则所有文本都将被处理

<font color="" face="" >文本</font>，font标签不能嵌套其它标签
	例：<font color="#ffff00" face="宋体" >文本</font>

<img src="" halign="" valign="" only="true/false/line" data="item:65730011"/> 
    src 中可以直接配置图元路径,或者配置ImageAnimation.xml中配置的关键帧动画图元名
    halign可以是"left","right","center"
	valign在行高上的对齐如果行高和图片一样，则无效,可以是"top", "center", "bottom"
	only表示是否独占行:only="false"或不设表示文本环绕图片,only="true" 表示图片独占行,only="line" 表示图片和文本一起独占行,文本不环绕,
	rect 表示目标显示区域大小，可以将小图缩小，大图放大rect="0,0,255,255"
	例：<img src="Interface\\ui\\ot1.tga"  align="right" only="true"/>
    表示：图元为："Interface\\ui\\ot1.tga"，右对齐且独占行，不与文本混排的图片

<br/>换行，分段
<s size="4"/>,默认缩进两个字符时直接用<s/>即可,必须紧跟在<br/>之后
<center>文本</center>文字居中时使用,仅这个标签可以嵌入其它标签
//--------------208_08_20----超链接支持-----------------------
<a href="" style="">文本</a>
href 中的内容表示超链接信息,可以配置自定义数据,实际不显示
style 中仅能配置HyperLinkStyle.xml中配置的类型
文本 为显示文本
注:标签要求小写
-------------------------------------------------------------------------
*/
class TinyHtml
{
public:
	TinyHtml();
	virtual ~TinyHtml();

	// 从文件读取内容文本
	bool CreateFromHtml(const char* szFileName, dword defColor = 0xffffffff,
		const char* defFont = "Default", long defSize = 12);
	// 直接设入文本,设好后会自动解析,后可通过GetSegments()获取解析后的内容
	bool SetText(const wchar_t* szText, dword defColor = 0xffffffff,
		const char* defFont = "Default", long defSize = 12);

	std::vector<TextSegment>* GetSegments() { return &m_Segments; }

    void SetDefColor(dword dwColor) { m_defColor = dwColor; }
    void SetDefFont(const char* pFontName) { m_defFont = pFontName; }

    bool RestoreToHtml(HtmlTextItem& item, std::wstring& buffer, bool bReturnFontFormat = true);
    bool RestoreToHtml(HtmlImageItem& item, std::wstring& buffer);

private:
	void Clean();
	// 读取HTML格式的文本
	bool LoadFile( const char* filename);
	// 分析,将文本内容转化为一定的数据结构存入m_Segment内,标检要求小写
	bool Analysis();
	inline static bool IsWhiteSpace(char c);
	const char* SkipWhiteSpace(char* p);

private:
	dword m_defColor;
	std::string	m_defFont;
	long m_defSize;
	std::wstring m_data;
	std::vector<TextSegment> m_Segments;
};

#endif // tinyhtml_h_27_9_2006_10_24

