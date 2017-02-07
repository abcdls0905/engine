//--------------------------------------------------------------------
// �ļ���:		MultiTextBox.h
// ��  ��:		֧��HTML��ǩ��ͼ�Ļ��ſؼ�
// ˵  ��:		
// ��������:	2008��8��20��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef MultiTextBox_h_2008_8_20
#define MultiTextBox_h_2008_8_20

#include "i_scrollable.h"
#include <vector>
#include "tinyhtml.h"

// MultiTextBox
#define ERROR_HEIGHT_CONTENT -1000000

class ImageAnimationManager;
class HyperLinkStyleManager;

class MultiTextBox: public IScrollable
{
public:
	enum
	{
		VERTICAL_SNAP,
		HORIZON_SNAP,
		INDEPEND_SNAP,
		MAXSNAPSIZE
	};

	struct DRAWLIMIT
	{
		int m_Left;
		int m_Right;
	};

	struct DRAWLIMITLIST
	{
		std::vector<DRAWLIMIT> m_OneLineLimits;
	};

	// ���Ƶ�ͼƬ
	struct DRAWIMAGEITEM
	{
		DRAWIMAGEITEM()
		{
			x = y = 0;
			bHasSuspend = false;
		}

		HtmlImageItem imageItem;
		int x;
		int y;
		bool bHasSuspend;
	};

	// ����
	struct DRAWTEXTITEM
	{
		rect_t rect;
		std::string szFont;
		dword dColor;
		std::wstring szText;
	};

	struct DRAWTEXTLINE
	{
		DRAWTEXTLINE()
		{
            nTopY = 0;
			nMaxLineHeight = 0;
		}

        int nTopY;
		int nMaxLineHeight;
		std::vector<DRAWTEXTITEM> m_TextLine;
	};

	// �������ڻ��ƽṹ�е�λ��
	struct HYPERLINKITEM
	{
		int m_nLinkIndex;
		std::wstring m_wcsLink;
		std::string m_szHPStyle;
		// һ�������ӣ���������¿��ֳܷɼ���
		// point_t x��ʾ�У�y��ʾ�����е����
		std::vector<point_t> m_OneLink;
	};

	// ÿ�λ��ʱ���Ǵ�0��ʼ
	int m_lCurLink;

	struct DrawHtmlItem
	{
		DrawHtmlItem(): m_ContentHeight(0), m_ContentWidth(0), m_bSelectable(true), 
			m_ContentTop(ERROR_HEIGHT_CONTENT),m_ContentBottom(ERROR_HEIGHT_CONTENT)
		{
		}

		// 1.Դʼ�ı�����
		std::wstring m_wcsText;
		
		// 2.�������HTML�ṹ,�Զ���ʽ��֯
		// Դ��
		TinyHtml m_Html;

		// 3.ͨ��Ԥ���ƺ�õ������ڻ��Ƶ��ı�
		// �����ı�����
		std::vector<DRAWTEXTLINE> m_TextList;//���е�����
		std::vector<DRAWIMAGEITEM> m_ImageList;  //����ͼƬ�б�
		std::vector<DRAWLIMITLIST> m_LinesLimits;  //ͼƬ���ƺ��ı����ܻ��,��˵õ��ı��ɻ��Ƶ���������

		// ��ǰʹ�õ����ı��߶�
		// ��ʵ�ʵ�����ͼƬ��ռ�ø߶����,�ڵ�һ��Ԥ��ʱ���
		// ����������ؼ������ɴ���,����;����˴��ں���Ĳ��ĸ߶�
		int m_ContentHeight;
        // �ı����,��ǰ����TIP���õ�
        int m_ContentWidth;
        // �Ƿ��ѡ��
        bool m_bSelectable;
        // ɸѡ��ֵ
        int m_nKey;
		// 4.���������,�������m_TextList�еĳ����Ӳ���
		std::vector<HYPERLINKITEM> m_vHyperLinks;

		// ����ڿؼ���ʾ��Χ���ϱߺ��±�
		int m_ContentTop;
		int m_ContentBottom;
	};

public:	
	MultiTextBox();
	virtual ~MultiTextBox();

	// ʵ��IControl����麯��
	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// �Ƿ�������
	virtual bool GetLoadFinish();

	// �¼�
	virtual IControl* OnGetInWhat(int x, int y);
	// ������̧��
	virtual int OnLButtonUp(int x, int y);
	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonDoubleClick(int x, int y);
	virtual int OnRButtonUp(int x, int y);
	virtual int OnMouseMove(int x, int y);
	// �ߴ�ı�
	virtual int OnWidthChanged(int old);
	virtual int OnHeightChanged(int old);
	// top Ϊitem�����������ʾ�����ϱߵ�λ��,����Ϊ������Ϊ��
	// ptMouse Ϊ�ڿؼ��ϵ����λ��
	bool OnWitchImage(int olditemindex, int itemindex, int top, 
		point_t ptMouse);
	bool OnWitchHyperLink(int olditemindex, int itemindex, int top, 
		point_t ptMouse);

    bool OnWitchImageTest(int olditemindex, int itemindex, int top, 
		point_t ptMouse);
    bool OnWitchHyperLinkTest(int olditemindex, int itemindex, int top, 
		point_t ptMouse);

	// ���ô�ֱ���뷽ʽ
	void SetVAlignString(const char* szText);
	result_string GetVAlignString() const;

	// ����ˮƽ���뷽ʽ
	void SetHAlignString(const char* szText);
	result_string GetHAlignString() const;

	// ��������
	void SetHtmlText(const wchar_t* szText);
	const wchar_t* GetHtmlText();

    // ���һ���ı�
    // szText ���ַ����ı�
    // nKey ɸѡ��ֵ-1��Ϊ������ɸѡ��һֱ��ʾ,keyֵһ��Ϊ�Ǹ�����
    // �����к�
	int AddHtmlText(const wchar_t* szText, int nKey);

	// ɾ��ָ����
	bool DelHtmlItem(size_t index);

	// �ı�һ�������
	bool ChangeHtmlText(size_t index, const wchar_t* szText);

    size_t GetItemCount() { return m_HtmlItems.size(); }
	// ��ȡָ�����ı�
	const wchar_t* GetHtmlItemText(size_t index);

    // ����ָ�����Ƿ��ѡ��
    bool SetHtmlItemSelectable(size_t index, bool bSelectable);
	bool SetHtmlItemSelected(size_t index, bool bSelect);
	// ���
	bool Clear();
    // �������htmlitem
    bool ClearHtmlItem(DrawHtmlItem& htmlitem);
    // ���ݵ�ǰ���������һ�°�,��Ҫ�ڿؼ���С�ı�ʱ����
    bool Reset();

    // �������
    bool GotoEnd();

    // ��ʾ�˼�ֵ��Ӧ�ı���nKey == -1ʱ��ʾ��ʾ����
    // �˺������ظ���������ͬʱ��ʾ��ͬKeyֵ�����ݣ���Ҫ��ʾ��nKey�ᱻ��¼
    // ����-1ʱ��ʾ���У���ʱ��¼��Key���ᱻ���
    // ����ֻ��ʾkeyΪ1������
    //    ShowKeyItems(-1); // �����¼
    //    ShowKeyItems(1);  // ������ʾ��ֵ
    //    ���ͬʱҪ��ʾkeyΪ2��������ֻ���ټ���
    //   ShowKeyItems(2)��
    bool ShowKeyItems(int nKey);

	// ������
	virtual void SetFontName(const char* val);

	// ����Ĭ����ɫ
	void SetTextColor(const char* szColor);
	result_string GetTextColor();

	// ����Ĭ����ɫ
	void SetSelectBarColor(const char* szColor);
	result_string GetSelectBarColor();

	// ����Ĭ����ɫ
	void SetMouseInBarColor(const char* szColor);
	result_string GetMouseInBarColor();

    // ѡ����ͼƬ
	void SetSelectBarDraw(const char* pDraw);
    const char* GetSelectBarDraw();

    // ���������ͼƬ
    void SetMouseInBarDraw(const char* pDraw);
    const char* GetMouseInBarDraw();

	// ������ʾ������:0,0,120,40
	void SetViewRect(const char* RectStr);
	result_string GetViewRect();

	// �ı��߶�
	void SetLineHeight(int height);
	const int GetLineHeight();

    // ͸����
    void SetAlpha(int alpha);
    const int GetAlpha();

    //// ͸��
    //void SetTransparent(bool bTrans);
    //const bool GetTransparent();

    // �Զ����������
    bool GetAutoScroll() { return m_bAutoScroll; }
    void SetAutoScroll(bool AutoScroll) { m_bAutoScroll = AutoScroll; }

    // �Զ������������ı���
    bool GetTipAutoSize() { return m_bTipAutoSize; }
    void SetTipAutoSize(bool AutoSize) { m_bTipAutoSize = AutoSize; }

    int GetTipMaxWidth() { return m_nTipMaxWidth; }
    void SetTipMaxWidth(int MaxWidth) { m_nTipMaxWidth = MaxWidth; }

    // �ı�������и���ֱ�����ϵĶ��뷽ʽ
    const char* GetLineTextAlign();
    void SetLineTextAlign(const char* pAlign);

	int GetSelectItemIndex() { return m_lCurSelectItem; }
	int GetMouseInItemIndex() { return m_lCurMouseInItem; }

	int GetSelectImageIndex() { return m_lCurSelectImage; }
	int GetMouseInImageIndex() { return m_lCurSelectImage; }

	int GetMouseInHyperLink() { return m_lCurMouseInHyperLink; }
	int GetSelectHyperLink() { return m_lCurSelectHyperLink; }

    // ��ͨ�ı���Ϣ����
    void SetTransNormalText(bool bTrans) { m_bTransNormalText = bTrans; }
    bool GetTransNormalText() { return m_bTransNormalText; }

    // ͼƬ��Ϣ����
    void SetTransImage(bool bTrans) { m_bTransImage = bTrans; }
    bool GetTransImage() { return m_bTransImage; }

    // ��������Ϣ����
    void SetTransHyperLink(bool bTrans) { m_bTransHyperlink = bTrans; }
    bool GetTransHyperLink() { return m_bTransHyperlink; }

	// �õ�ͼƬ�ľ����������
	rect_t GetImageRect(int itemindex, int imageindex);
    bool ChangeImage(int itemindex, int imageindex, const char* image_res);

	const char* GetImageData(int itemindex, int imageindex);
	const wchar_t* GetHyperLinkData(int itemindex, int linkindex);
    void GetImagePlace(const IVarList& params, IVarList& result);

    // ��ȡ�����ӵ���Ļ����
    void GetHyperLinkRect(const IVarList& key, IVarList& result); 

    // ͨ����Ż�ȡkey
    int GetItemKeyByIndex(int itemindex);
    // ͨ��key��ȡ���
    int GetItemIndexByKey(int key);
	
	int GetItemTop(int index);
	int GetItemHeight(int index);
    int GetContentHeight();
    int GetContentWidth();

	// �й�����ʱ�ɹ���������,�޹�����ʱ�ɵ�
    virtual int GetVerticalTotal() const;
    virtual int GetVerticalCanDisp() const;
    virtual void SetVerticalValue(int value);
    virtual int GetVerticalValue() const;

    //
    int GetVerticalMaxValue();

	bool GetTransparent(){return false;}

	void OnMouseInItem(int index);
	void OnMouseOutItem(int index);

	// ����ͼƬ��Ⱦģʽ
	void SetSelectBarDrawModeString(const char* value);
	result_string GetSelectBarDrawModeString() const;

	void SetMouseInBarDrawModeString(const char* value);
	result_string GetMouseInBarDrawModeString() const;
	
private:
	// ��ȡ��ǰʵ����ʾ���������������б��еĸ߶�
    int GetDrawItemTop();
	// ��ȡ��ǰ��ʾ���ݵ���ʼy����,�������Ϸ��Ļ
	int GetDrawBeginTop();

    bool IsShowKey(int nKey) const;
    bool IsItemSelectable(size_t index);
    void UpdateScroll();
	void ChangeHyperLinkState(DrawHtmlItem& htmlitem, int link, int state);

	//�����������е�TextSegment
	bool DrawTextSegmentCenter(DrawHtmlItem& htmlitem, int xbase, int ybase, 
		int& x, int& y, TextSegment* pSegment);
	// �����ĳ��λ�ÿ�ʼ��TextItem
	// ˼�룺ֻҪͨ����ǰ��x,y����Ϳ���֪����ǰ����һ��ʲôλ�ÿ�ʼд����
	void DrawTextItem(DrawHtmlItem& htmlitem, int xbase, int ybase, int& x,
		int& y, HtmlTextItem* pItem, bool bCenter = false);
	void DrawImageItem(DrawHtmlItem& htmlitem, int xbase, int ybase, int& x,
		int& y, HtmlImageItem* pItem, bool bCenter = false);
	// Ԥ���ı�
	void VirtualDrawText(DrawHtmlItem& htmlitem, const char* font, dword color, 
		int x, int y, const wchar_t* szDraw, bool IsHyperLink);
	// ʵ���ı�
	void RealDrawText(const char* font, dword color, int x, int y,
		const wchar_t* szDraw);

	// ��ȡ��������������������ֹ����Ӣ��ʱ��Ӧ��ʾһ�����ĵĽ���ʾ�˰���Ӣ�ĵ�����
	void CorrectShowString(std::wstring& szBuffer, std::wstring& szDraw, 
		int lWidth);
	// �˺��������������׳��ֲ���ȷ�ı��������ʹ������Ű�涨
	void CorrectInterpunction(std::wstring& szBuffer, std::wstring& szDraw);
	// ��ȡ��width����ڣ�wcsBuffer�п���ʾ���ַ�����
	int GetCanShowWordsNum(std::wstring & wcsBuffer, const int width);
	// ��ǰʹ�õĻ��ƺ���
	// ģ��HTML����ִ�е�ԭ��,Ҳ�Ǳ߻����ֱ߶�λ/����ͼƬ,�߶�λ/����ͼƬ�߻����ֵ�
	void VirtualDrawHTML(DrawHtmlItem& htmlitem, int xbase = 0,int ybase = 0);
	// ͨ�������õ��������ٸ��ݵ�ǰ�Ĺ�����viewrect���л���
	int RealDrawHTML(DrawHtmlItem& htmlitem, int ContentTop, 
		int ContentBottom, int ContentBegin, int x, int y);

    PERSISTID CreateGlyph(const char* pName, int& type, int& width, 
		int& height);

    // ��ȡ������ڵ���
    int GetMouseInItem(int x, int y, point_t& pt, int& lContentBegin);

	void OnScroll(dword wParam, dword lParam);

	// ���ͼƬ���ʵ�λ��,��ǰ���ÿ�����ж�,�ڸ߶ȿռ䲻�����������,
	// ���ܳ���ʧ��,��ü���߶ȵ��ж�
	point_t* SetImagePos(DrawHtmlItem& htmlitem, int& x, int& y, 
		HtmlImageItem* pimage);

    void ProcessY(bool IsLineChanged, DrawHtmlItem& htmlitem, int& y, 
		point_t& point, HtmlImageItem* pimage);

	void SetImageLimit(DrawHtmlItem& htmlitem, point_t* pPoint, 
		HtmlImageItem* pImage);

    bool AppendLimits(DrawHtmlItem& htmlitem, 
        DRAWIMAGEITEM& draw_imageitem, 
        int old_y, int new_y);
    // �и߸ı�ʱͼƬ���뷽ʽ��ͬ�߶������µ���
    bool ChangeMaxLineHeight(DrawHtmlItem& htmlitem, 
            const int top_y, const int new_line_height);

    // ��ȡͼƬ���������¶�����yֵ
    int GetImageVAlignPos(HtmlImageItem* pimage, int line_y, 
        const int cur_line_height);

	// ���������parent�ַ������Ƿ����sub�ַ���,���򷵻�true
	bool IsFound(const wchar_t* sub, const wchar_t* parent);

    //
     ImageAnimationManager* InnerGetAnimation();
     HyperLinkStyleManager* InnerGetHyperlinkMng();

	int GetTextItemWidth(HtmlTextItem* pItem);
	int GetImageItemWidth(HtmlImageItem* pItem);

	bool RefreshContentPos();

private:
	//--XML�󶨵ı���-------------------------------------------------
	// ����������ؼ������ɴ���,����;����˴��ں���Ĳ��Ŀ��,
	// �ֲ��������������Խ���viewrect�Ŀ��
	int m_TextWidth;
	// �����ı��߶�
	int m_LineHeight;
	// ���rect�����˿�������,�൱�ڴ��ڵĴ�С
	rect_t m_ViewRect;	

	unsigned int m_TextColor;

	unsigned int m_SelectBarColor;
	unsigned int m_MouseInBarColor;
	int m_nSelectBarDrawMode;		// ѡ�����滭��ʽ
	int m_nMouseInBarDrawMode;		// ѡ�����滭��ʽ
	CImagePlayer m_SelectBarDraw;	// ѡ����ͼԪ
	CImagePlayer m_MouseInBarDraw;	// ѡ����ͼԪ

	std::vector<DrawHtmlItem> m_HtmlItems;
	// �ܳ���
	int m_nMaxLines;

	std::string m_CurFont;
	int m_lCurLineHeight;

	int m_lCurMouseInItem;
	int m_lCurSelectItem;

	// ������ͼƬ
	int m_lCurMouseInImage;
	int m_lCurSelectImage;
	// ��ǰ������г�����
	int m_lCurSelectHyperLink;
	int m_lCurMouseInHyperLink;

	int m_nTopIndex;
    bool m_bAutoScroll;
    bool m_bTipAutoSize;
    int m_nTipMaxWidth;
    std::vector<int> m_vShowKeys;

    //�����ı����ı��߶�С���и�ʱ�ı��Ĵ�ֱ���뷽ʽ,Ĭ�Ͼ���,0���£�1���У�2����
    int m_nLineTextAlign; 

    //��Ϊ���ú������õ�
    std::wstring m_wcsHtmlText;

    int m_nAlpha;
	int m_nVAlign;		// ��ֱ���뷽ʽ
	int m_nHAlign;		// ˮƽ���뷽ʽ

    //bool m_bTransparent; // �Ƿ�ȫ͸������������Ϣ
    bool m_bTransNormalText; // ��ͨ�ı��Ƿ񲻴�����Ϣ
    bool m_bTransImage; // ͼƬ�Ƿ񲻴�����Ϣ
    bool m_bTransHyperlink; // �������Ƿ񲻴�����Ϣ

    // ����ؼ��Ƚ����⣬͸����Ч����Ȼ������꽹��ؼ�����������Ҫ�������Ƴ���Ϣ
    bool m_bIsMouseCaptured;
    bool m_bCanDelete;

    rect_t m_RealRect;
};

#endif //MultiTextBox_h_2008_8_20