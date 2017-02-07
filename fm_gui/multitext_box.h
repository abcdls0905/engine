//--------------------------------------------------------------------
// 文件名:		MultiTextBox.h
// 内  容:		支持HTML标签的图文混排控件
// 说  明:		
// 创建日期:	2008年8月20日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
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

	// 绘制的图片
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

	// 文字
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

	// 超链接在绘制结构中的位置
	struct HYPERLINKITEM
	{
		int m_nLinkIndex;
		std::wstring m_wcsLink;
		std::string m_szHPStyle;
		// 一个超链接，折行情况下可能分成几段
		// point_t x表示行，y表示在行中的序号
		std::vector<point_t> m_OneLink;
	};

	// 每次绘的时候都是从0开始
	int m_lCurLink;

	struct DrawHtmlItem
	{
		DrawHtmlItem(): m_ContentHeight(0), m_ContentWidth(0), m_bSelectable(true), 
			m_ContentTop(ERROR_HEIGHT_CONTENT),m_ContentBottom(ERROR_HEIGHT_CONTENT)
		{
		}

		// 1.源始文本数据
		std::wstring m_wcsText;
		
		// 2.解析后的HTML结构,以段形式组织
		// 源如
		TinyHtml m_Html;

		// 3.通过预绘制后得到的用于绘制的文本
		// 绘制文本限制
		std::vector<DRAWTEXTLINE> m_TextList;//所有的文字
		std::vector<DRAWIMAGEITEM> m_ImageList;  //所有图片列表
		std::vector<DRAWLIMITLIST> m_LinesLimits;  //图片绘制后文本不能绘的,因此得到文本可绘制的区域限制

		// 当前使用到的文本高度
		// 由实际的文字图片的占用高度算得,在第一次预画时获得
		// 如果将整个控件比喻成窗户,那这就决定了窗口后面的布的高度
		int m_ContentHeight;
        // 文本宽度,当前仅在TIP中用到
        int m_ContentWidth;
        // 是否可选择
        bool m_bSelectable;
        // 筛选键值
        int m_nKey;
		// 4.超链接相关,标计了在m_TextList中的超链接部分
		std::vector<HYPERLINKITEM> m_vHyperLinks;

		// 相对于控件显示范围的上边和下边
		int m_ContentTop;
		int m_ContentBottom;
	};

public:	
	MultiTextBox();
	virtual ~MultiTextBox();

	// 实现IControl里的虚函数
	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// 是否加载完成
	virtual bool GetLoadFinish();

	// 事件
	virtual IControl* OnGetInWhat(int x, int y);
	// 鼠标左键抬起
	virtual int OnLButtonUp(int x, int y);
	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonDoubleClick(int x, int y);
	virtual int OnRButtonUp(int x, int y);
	virtual int OnMouseMove(int x, int y);
	// 尺寸改变
	virtual int OnWidthChanged(int old);
	virtual int OnHeightChanged(int old);
	// top 为item上沿相对于显示区域上边的位置,在上为负在下为正
	// ptMouse 为在控件上的相对位置
	bool OnWitchImage(int olditemindex, int itemindex, int top, 
		point_t ptMouse);
	bool OnWitchHyperLink(int olditemindex, int itemindex, int top, 
		point_t ptMouse);

    bool OnWitchImageTest(int olditemindex, int itemindex, int top, 
		point_t ptMouse);
    bool OnWitchHyperLinkTest(int olditemindex, int itemindex, int top, 
		point_t ptMouse);

	// 设置垂直对齐方式
	void SetVAlignString(const char* szText);
	result_string GetVAlignString() const;

	// 设置水平对齐方式
	void SetHAlignString(const char* szText);
	result_string GetHAlignString() const;

	// 设置文字
	void SetHtmlText(const wchar_t* szText);
	const wchar_t* GetHtmlText();

    // 添加一项文本
    // szText 宽字符串文本
    // nKey 筛选键值-1定为不参于筛选，一直显示,key值一般为非负整数
    // 返回行号
	int AddHtmlText(const wchar_t* szText, int nKey);

	// 删除指定项
	bool DelHtmlItem(size_t index);

	// 改变一项的内容
	bool ChangeHtmlText(size_t index, const wchar_t* szText);

    size_t GetItemCount() { return m_HtmlItems.size(); }
	// 获取指定项文本
	const wchar_t* GetHtmlItemText(size_t index);

    // 设置指定项是否可选择
    bool SetHtmlItemSelectable(size_t index, bool bSelectable);
	bool SetHtmlItemSelected(size_t index, bool bSelect);
	// 清除
	bool Clear();
    // 清除单个htmlitem
    bool ClearHtmlItem(DrawHtmlItem& htmlitem);
    // 根据当前宽高重新排一下版,主要在控件大小改变时调用
    bool Reset();

    // 滚到最后
    bool GotoEnd();

    // 显示此键值对应文本，nKey == -1时表示显示所有
    // 此函数可重复调用用于同时显示不同Key值的内容，需要显示的nKey会被记录
    // 当设-1时显示所有，此时记录的Key将会被清除
    // 例：只显示key为1的内容
    //    ShowKeyItems(-1); // 请除记录
    //    ShowKeyItems(1);  // 设置显示键值
    //    如果同时要显示key为2的内容则只需再加上
    //   ShowKeyItems(2)；
    bool ShowKeyItems(int nKey);

	// 字体名
	virtual void SetFontName(const char* val);

	// 设置默认颜色
	void SetTextColor(const char* szColor);
	result_string GetTextColor();

	// 设置默认颜色
	void SetSelectBarColor(const char* szColor);
	result_string GetSelectBarColor();

	// 设置默认颜色
	void SetMouseInBarColor(const char* szColor);
	result_string GetMouseInBarColor();

    // 选中条图片
	void SetSelectBarDraw(const char* pDraw);
    const char* GetSelectBarDraw();

    // 鼠标移入条图片
    void SetMouseInBarDraw(const char* pDraw);
    const char* GetMouseInBarDraw();

	// 设置显示区域如:0,0,120,40
	void SetViewRect(const char* RectStr);
	result_string GetViewRect();

	// 文本高度
	void SetLineHeight(int height);
	const int GetLineHeight();

    // 透明度
    void SetAlpha(int alpha);
    const int GetAlpha();

    //// 透明
    //void SetTransparent(bool bTrans);
    //const bool GetTransparent();

    // 自动滚动到最后
    bool GetAutoScroll() { return m_bAutoScroll; }
    void SetAutoScroll(bool AutoScroll) { m_bAutoScroll = AutoScroll; }

    // 自动根据内容来改变宽高
    bool GetTipAutoSize() { return m_bTipAutoSize; }
    void SetTipAutoSize(bool AutoSize) { m_bTipAutoSize = AutoSize; }

    int GetTipMaxWidth() { return m_nTipMaxWidth; }
    void SetTipMaxWidth(int MaxWidth) { m_nTipMaxWidth = MaxWidth; }

    // 文本相对于行高竖直方向上的对齐方式
    const char* GetLineTextAlign();
    void SetLineTextAlign(const char* pAlign);

	int GetSelectItemIndex() { return m_lCurSelectItem; }
	int GetMouseInItemIndex() { return m_lCurMouseInItem; }

	int GetSelectImageIndex() { return m_lCurSelectImage; }
	int GetMouseInImageIndex() { return m_lCurSelectImage; }

	int GetMouseInHyperLink() { return m_lCurMouseInHyperLink; }
	int GetSelectHyperLink() { return m_lCurSelectHyperLink; }

    // 普通文本消息屏蔽
    void SetTransNormalText(bool bTrans) { m_bTransNormalText = bTrans; }
    bool GetTransNormalText() { return m_bTransNormalText; }

    // 图片消息屏蔽
    void SetTransImage(bool bTrans) { m_bTransImage = bTrans; }
    bool GetTransImage() { return m_bTransImage; }

    // 超链接消息屏蔽
    void SetTransHyperLink(bool bTrans) { m_bTransHyperlink = bTrans; }
    bool GetTransHyperLink() { return m_bTransHyperlink; }

	// 得到图片的绝动坐标矩形
	rect_t GetImageRect(int itemindex, int imageindex);
    bool ChangeImage(int itemindex, int imageindex, const char* image_res);

	const char* GetImageData(int itemindex, int imageindex);
	const wchar_t* GetHyperLinkData(int itemindex, int linkindex);
    void GetImagePlace(const IVarList& params, IVarList& result);

    // 获取超链接的屏幕座标
    void GetHyperLinkRect(const IVarList& key, IVarList& result); 

    // 通过序号获取key
    int GetItemKeyByIndex(int itemindex);
    // 通过key获取序号
    int GetItemIndexByKey(int key);
	
	int GetItemTop(int index);
	int GetItemHeight(int index);
    int GetContentHeight();
    int GetContentWidth();

	// 有滚动条时由滚动条调用,无滚动条时可调
    virtual int GetVerticalTotal() const;
    virtual int GetVerticalCanDisp() const;
    virtual void SetVerticalValue(int value);
    virtual int GetVerticalValue() const;

    //
    int GetVerticalMaxValue();

	bool GetTransparent(){return false;}

	void OnMouseInItem(int index);
	void OnMouseOutItem(int index);

	// 背景图片渲染模式
	void SetSelectBarDrawModeString(const char* value);
	result_string GetSelectBarDrawModeString() const;

	void SetMouseInBarDrawModeString(const char* value);
	result_string GetMouseInBarDrawModeString() const;
	
private:
	// 获取当前实际显示内容在整个内容列表中的高度
    int GetDrawItemTop();
	// 获取当前显示内容的起始y坐标,相对于游戏屏幕
	int GetDrawBeginTop();

    bool IsShowKey(int nKey) const;
    bool IsItemSelectable(size_t index);
    void UpdateScroll();
	void ChangeHyperLinkState(DrawHtmlItem& htmlitem, int link, int state);

	//绘制整个居中的TextSegment
	bool DrawTextSegmentCenter(DrawHtmlItem& htmlitem, int xbase, int ybase, 
		int& x, int& y, TextSegment* pSegment);
	// 解决从某个位置开始画TextItem
	// 思想：只要通过当前的x,y座标就可以知道当前在那一行什么位置开始写文字
	void DrawTextItem(DrawHtmlItem& htmlitem, int xbase, int ybase, int& x,
		int& y, HtmlTextItem* pItem, bool bCenter = false);
	void DrawImageItem(DrawHtmlItem& htmlitem, int xbase, int ybase, int& x,
		int& y, HtmlImageItem* pItem, bool bCenter = false);
	// 预绘文本
	void VirtualDrawText(DrawHtmlItem& htmlitem, const char* font, dword color, 
		int x, int y, const wchar_t* szDraw, bool IsHyperLink);
	// 实绘文本
	void RealDrawText(const char* font, dword color, int x, int y,
		const wchar_t* szDraw);

	// 对取的文字数进行修正，防止都是英文时本应显示一行中文的仅显示了半行英文的现像
	void CorrectShowString(std::wstring& szBuffer, std::wstring& szDraw, 
		int lWidth);
	// 此函数用于修正行首出现不正确的标点的情况，使其符合排版规定
	void CorrectInterpunction(std::wstring& szBuffer, std::wstring& szDraw);
	// 获取在width宽度内，wcsBuffer中可显示的字符个数
	int GetCanShowWordsNum(std::wstring & wcsBuffer, const int width);
	// 当前使用的绘制函数
	// 模仿HTML解析执行的原理,也是边绘文字边定位/绘制图片,边定位/绘制图片边绘文字的
	void VirtualDrawHTML(DrawHtmlItem& htmlitem, int xbase = 0,int ybase = 0);
	// 通过解析得到的数据再根据当前的滚动及viewrect进行绘制
	int RealDrawHTML(DrawHtmlItem& htmlitem, int ContentTop, 
		int ContentBottom, int ContentBegin, int x, int y);

    PERSISTID CreateGlyph(const char* pName, int& type, int& width, 
		int& height);

    // 获取鼠标所在的项
    int GetMouseInItem(int x, int y, point_t& pt, int& lContentBegin);

	void OnScroll(dword wParam, dword lParam);

	// 获得图片合适的位置,当前仅用宽度来判断,在高度空间不够的情况且下,
	// 可能出现失误,最好加入高度的判断
	point_t* SetImagePos(DrawHtmlItem& htmlitem, int& x, int& y, 
		HtmlImageItem* pimage);

    void ProcessY(bool IsLineChanged, DrawHtmlItem& htmlitem, int& y, 
		point_t& point, HtmlImageItem* pimage);

	void SetImageLimit(DrawHtmlItem& htmlitem, point_t* pPoint, 
		HtmlImageItem* pImage);

    bool AppendLimits(DrawHtmlItem& htmlitem, 
        DRAWIMAGEITEM& draw_imageitem, 
        int old_y, int new_y);
    // 行高改变时图片对齐方式不同高度需重新调整
    bool ChangeMaxLineHeight(DrawHtmlItem& htmlitem, 
            const int top_y, const int new_line_height);

    // 获取图片行上上中下对齐后的y值
    int GetImageVAlignPos(HtmlImageItem* pimage, int line_y, 
        const int cur_line_height);

	// 查找在这个parent字符串中是否存在sub字符串,有则返回true
	bool IsFound(const wchar_t* sub, const wchar_t* parent);

    //
     ImageAnimationManager* InnerGetAnimation();
     HyperLinkStyleManager* InnerGetHyperlinkMng();

	int GetTextItemWidth(HtmlTextItem* pItem);
	int GetImageItemWidth(HtmlImageItem* pItem);

	bool RefreshContentPos();

private:
	//--XML绑定的变量-------------------------------------------------
	// 如果将整个控件比喻成窗户,那这就决定了窗口后面的布的宽度,
	// 现不允许横向滚动所以仅是viewrect的宽度
	int m_TextWidth;
	// 单行文本高度
	int m_LineHeight;
	// 这个rect决定了可视区域,相当于窗口的大小
	rect_t m_ViewRect;	

	unsigned int m_TextColor;

	unsigned int m_SelectBarColor;
	unsigned int m_MouseInBarColor;
	int m_nSelectBarDrawMode;		// 选择条绘画方式
	int m_nMouseInBarDrawMode;		// 选择条绘画方式
	CImagePlayer m_SelectBarDraw;	// 选择条图元
	CImagePlayer m_MouseInBarDraw;	// 选择条图元

	std::vector<DrawHtmlItem> m_HtmlItems;
	// 总长度
	int m_nMaxLines;

	std::string m_CurFont;
	int m_lCurLineHeight;

	int m_lCurMouseInItem;
	int m_lCurSelectItem;

	// 处理点击图片
	int m_lCurMouseInImage;
	int m_lCurSelectImage;
	// 当前鼠标所有超链接
	int m_lCurSelectHyperLink;
	int m_lCurMouseInHyperLink;

	int m_nTopIndex;
    bool m_bAutoScroll;
    bool m_bTipAutoSize;
    int m_nTipMaxWidth;
    std::vector<int> m_vShowKeys;

    //单行文本，文本高度小于行高时文本的垂直对齐方式,默认居下,0居下，1居中，2居上
    int m_nLineTextAlign; 

    //作为配置和属性用的
    std::wstring m_wcsHtmlText;

    int m_nAlpha;
	int m_nVAlign;		// 垂直对齐方式
	int m_nHAlign;		// 水平对齐方式

    //bool m_bTransparent; // 是否全透明，不处理消息
    bool m_bTransNormalText; // 普通文本是否不处理消息
    bool m_bTransImage; // 图片是否不处理消息
    bool m_bTransHyperlink; // 超链接是否不处理消息

    // 这个控件比较特殊，透明情效下虽然不是鼠标焦点控件，但是仍需要有移入移出消息
    bool m_bIsMouseCaptured;
    bool m_bCanDelete;

    rect_t m_RealRect;
};

#endif //MultiTextBox_h_2008_8_20