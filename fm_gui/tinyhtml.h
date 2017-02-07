//--------------------------------------------------------------------
// �ļ���:		tinyhtml.h
// ��  ��:		����HTML�ļ�����
// ˵  ��:		ʵ��ͼ�Ļ��ſ����ݶ�ȡ������
// ��������:	2006��10��24��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
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

// ע�⣺���御����ż����С����12,14����Ҫ��13
struct HtmlItem
{
    long m_Type;
    long m_Width;
};

// ��ʼ��һ�����νṹ
// ͼƬ��ʽ
struct HtmlImageItem : public HtmlItem
{
	HtmlImageItem(): m_imagesrc(L""), m_Rect(MAKE_RECT(0,0,0,0)),
		m_HAlign(L""), m_VAlign(L""),m_nOnly(0), m_bNeedRelease(false),
		m_bIsAnimation(false), m_MarkStr(""), m_nType(0){}

	std::wstring m_imagesrc;	// ͼԪ��
	rect_t m_Rect;				// ��������
	std::wstring m_HAlign;		// ���Ҷ��뷽ʽ
	std::wstring m_VAlign;		// ���¶��뷽ʽ
	PERSISTID m_imageID;		// ͼԪ���,����ͨ��dx������õ���
    int m_nType;				// 0��ʾTGA,1��ʾPNG,2��ʾanimation	
    int m_nOnly;				// 1(true)Ϊ��ռ����Ϊһ��,0(false)Ϊ����ռ���ֻ���ģʽ,2(line)Ϊ������һ����,���ٻ���
	bool m_bNeedRelease;		// ɾ��ʱ�Ƿ���Ҫ�ֶ�����
	bool m_bIsAnimation;		// �Ƿ��Ƕ���ͼԪ
	std::string m_MarkStr;		// ͼƬ����
};

// ���ָ�ʽ
struct HtmlTextItem : public HtmlItem
{
	HtmlTextItem(dword incolor = 0xffffffff, const char* inszFace = "Default",
		long inlFontSize = 12)
	{
		m_text = "";
		m_wtext = L"";
		// Ĭ���� ��ɫ ���� 12
		m_Color	= incolor;
		m_Face = inszFace;
		m_FontSize = inlFontSize;
		m_IsHyperLink = false;
	}

	std::string m_text;
	std::wstring m_wtext;
	dword m_Color;		// ��ɫ
	long m_FontSize;		// ��С
	//-------------------------2006_09_27--add by ylp------
	std::string m_Face;			// ���� 
	//-------------------------2008_08_22--add by ylp------�¼��볬����֧��
	bool m_IsHyperLink;
	std::string m_szHPStyle;	//style="XXX"
	std::wstring m_wcsLink;	//href="XXX"
};

// �θ�ʽ��
// ����Ҫ���еĵط���Ҫ��Ϊ[��]��һ��TextSegment,[��]��Ҳ��ͼƬ���ı�
// ���־��еĸ�ʽ��ʽ��Ϊ���еĶ�����������
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
	// ���ֺ�ͼƬ����˳��
	std::vector<bool> m_processlist;//trueΪText,falseΪimage
	//-------------------------2006_09_27--add by ylp------
	// ���еĹ���ֻ���ڸö��������ֳ���С���п��ʱ����Ч����Ȼ��������
	bool m_bCenter;
	// ������<s/>Ĭ��Ϊ����Ҳ�ɼ���size����ָ���յĸ���<s size="3"/>
	// �������m_bCenter==falseʱ�ű�ʹ��
	bool m_bInsertTwoSpace;
};

/**
һ���ʽ
<body>
	����&ͼƬ
</body>

ͼƬ��ʽ:<img src="" rect="" align="" />
�ı���ʽ_��    ��	:<br/>
�ı���ʽ_��������	:<s size=""/>,Ĭ�����������ַ�ʱֱ����<s/>����,���������<br/>֮��
�ı���ʽ_��    ��	:<center>�ı�</center>
�ı���ʽ_������ɫ��С:<font face="����" color="#ffffff(��ɫ)" size="��С"> 
					      ����
					  </font>
*/
/**
��ǰ֧�ֵı�ǩ˵��

<body></body>
����������ǩ,����ʱֻ��ȡ��������ǩ�ڵ��ı����д���,�����Ľ������Խ������������Ľ������ԣ����û�����ǩ���������ı�����������

<font color="" face="" >�ı�</font>��font��ǩ����Ƕ��������ǩ
	����<font color="#ffff00" face="����" >�ı�</font>

<img src="" halign="" valign="" only="true/false/line" data="item:65730011"/> 
    src �п���ֱ������ͼԪ·��,��������ImageAnimation.xml�����õĹؼ�֡����ͼԪ��
    halign������"left","right","center"
	valign���и��ϵĶ�������иߺ�ͼƬһ��������Ч,������"top", "center", "bottom"
	only��ʾ�Ƿ��ռ��:only="false"�����ʾ�ı�����ͼƬ,only="true" ��ʾͼƬ��ռ��,only="line" ��ʾͼƬ���ı�һ���ռ��,�ı�������,
	rect ��ʾĿ����ʾ�����С�����Խ�Сͼ��С����ͼ�Ŵ�rect="0,0,255,255"
	����<img src="Interface\\ui\\ot1.tga"  align="right" only="true"/>
    ��ʾ��ͼԪΪ��"Interface\\ui\\ot1.tga"���Ҷ����Ҷ�ռ�У������ı����ŵ�ͼƬ

<br/>���У��ֶ�
<s size="4"/>,Ĭ�����������ַ�ʱֱ����<s/>����,���������<br/>֮��
<center>�ı�</center>���־���ʱʹ��,�������ǩ����Ƕ��������ǩ
//--------------208_08_20----������֧��-----------------------
<a href="" style="">�ı�</a>
href �е����ݱ�ʾ��������Ϣ,���������Զ�������,ʵ�ʲ���ʾ
style �н�������HyperLinkStyle.xml�����õ�����
�ı� Ϊ��ʾ�ı�
ע:��ǩҪ��Сд
-------------------------------------------------------------------------
*/
class TinyHtml
{
public:
	TinyHtml();
	virtual ~TinyHtml();

	// ���ļ���ȡ�����ı�
	bool CreateFromHtml(const char* szFileName, dword defColor = 0xffffffff,
		const char* defFont = "Default", long defSize = 12);
	// ֱ�������ı�,��ú���Զ�����,���ͨ��GetSegments()��ȡ�����������
	bool SetText(const wchar_t* szText, dword defColor = 0xffffffff,
		const char* defFont = "Default", long defSize = 12);

	std::vector<TextSegment>* GetSegments() { return &m_Segments; }

    void SetDefColor(dword dwColor) { m_defColor = dwColor; }
    void SetDefFont(const char* pFontName) { m_defFont = pFontName; }

    bool RestoreToHtml(HtmlTextItem& item, std::wstring& buffer, bool bReturnFontFormat = true);
    bool RestoreToHtml(HtmlImageItem& item, std::wstring& buffer);

private:
	void Clean();
	// ��ȡHTML��ʽ���ı�
	bool LoadFile( const char* filename);
	// ����,���ı�����ת��Ϊһ�������ݽṹ����m_Segment��,���Ҫ��Сд
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

