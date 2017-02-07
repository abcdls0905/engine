//--------------------------------------------------------------------
// 文件名:		RichEdit.h
// 内  容:		多格式文本输入框 多行 (HTML部分支持)
// 说  明:		
// 创建日期:	2008年9月18日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef RichEdit_h_2008_9_18
#define RichEdit_h_2008_9_18

#include "i_scrollable.h"
#include "tinyhtml.h"
//#include <windef.h>

// RichEdit
// 文字输入框 多行

class ImageAnimationManager;
class HyperLinkStyleManager;


class RichEdit: public IScrollable
{
private:
	// 显示类型
	enum
	{
		PARTTYPE_CHAR,	// 字符
		PARTTYPE_IMAGE,	// 图元
	};
	
	struct LineItem
	{
		int nLineHeight;
		int nStartPos;
		int nEndPos;
	};
	
	typedef std::vector<HtmlItem*> PART_CONTAINER;
	
public:
	RichEdit();
	virtual ~RichEdit();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	virtual int OnWidthChanged(int old);
	virtual int OnHeightChanged(int old);
	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonUp(int x, int y);
	virtual int OnDragMove(int x, int y);
	virtual int OnKeyDown(unsigned int vk);
    virtual int OnKeyUp(unsigned int vk);
	virtual int OnChar(unsigned int ch);
    virtual int OnEnter();
	virtual int OnGetFocus(const PERSISTID& lost_id);
	virtual int OnLostFocus(const PERSISTID& get_id);
	virtual int OnChanged(const PERSISTID& changed_id);

	virtual void SetText(const wchar_t* val);
	virtual const wchar_t* GetText() const;
	virtual point_t GetCaretPos();
	// 输入光标名称
	virtual void SetCaretName(const char* name);
	virtual const char* GetCaretName() const;

    // 设置显示区域如:0,0,120,40
    void SetViewRect(const char* RectStr);
	result_string GetViewRect();

	// 只读状态
	void SetReadOnly(bool value); 
	bool GetReadOnly() const;
	
	// 编辑器模式
	void SetEditMode(bool value); 
	bool GetEditMode() const;

	// HTML支持
	void SetSupportHtml(bool value); 
	bool GetSupportHtml() const;

	// 是否产生拖动事件
	void SetDragEvent(bool value);
	bool GetDragEvent() const;
	
    // 是否产生变化事件
    void SetChangedEvent(bool value);
    bool GetChangedEvent() const;

	// 最大长度
	void SetMaxLength(int value);
	int GetMaxLength() const;
	
	// 对齐方式
	void SetAlignString(const char* value);
	result_string GetAlignString() const;
	
	// 密码显示字符
	void SetPasswordChar(const wchar_t* value);
	const wchar_t* GetPasswordChar() const;
	
	// 选中时的颜色
	void SetSelectForeColorString(const char* value);
	result_string GetSelectForeColorString() const;
	
	// 选中时的背景颜色
	void SetSelectBackColorString(const char* value);
	result_string GetSelectBackColorString() const;

    // 行高
    void SetLineHeight(int height);
    int GetLineHeight();

    // 是否获取完整格式
    void SetReturnAllFormat(bool bFull);
    bool GetReturnAllFormat();

	// 是否返回字体格式
	void SetReturnFontFormat(bool bValue);
	bool GetReturnFontFormat();

	// 记录输入
	void SetRemember(bool bRemember);
	bool GetRemember() const;

	// 设置记录输入文本最大条数
	void SetMaxRemember(int max);
	int GetMaxRemember() const;

	// 获得选中部分的字符串
	std::wstring GetSelectText();
	// 是否有选中的文字
	bool HasSelectText() const;
	// 清除选中状态
	void ClearSelect();

    // 将选择项变色
    bool ChangeSelectColor(const char* color);

    // 将选择项变字体
    bool ChangeSelectFont(const char*font);

	// 删除
	bool Cut();
	// 拷贝
	bool Copy();
	// 粘贴
	bool Paste();

    // 添加文字mode 0 添加在后,1添加在前 -1添加在当前光标位置,当前无光标则添加在最后
    bool Append(const wchar_t* text, int mode);
	
    // 获取行数
    int GetLineNum();

	// 获得当前长度
	int GetSize() const;
private:    
	// 继承IScrollable相关模板方法
    // 垂直滚动总数量
    virtual int GetVerticalTotal() const;
    // 可以显示的数量
    virtual int GetVerticalCanDisp() const;
    // 设置当前值
    virtual void SetVerticalValue(int value);
    // 获取当前值
    virtual int GetVerticalValue() const;

    // 获取内容所占的行数
    virtual int GetContentLines() const;
       
    virtual bool UpdateScroll();

private:
    // 获取当前当标所在的实际item位置
    // 通常m_Buffer[GetCareRealPos()]得到的是插入点之后的item
    // 需要获得插入点之前的item时，需要GetCareRealPos() - 1,但需注意m_Buffer为空的情况
    int GetCareRealPos(const int line, const int linepos);
    void RefreshScrollByCaret();
    
	// 所有输入文本最后一个一定是换行标志
    void CorrectFinalFlag();

	void ReleasePart(HtmlItem* part);
	void ReleaseAll();
	
	// 设置光标
	bool SetCaretID(const char* name);


	// 获得最大长度
	int GetMaxSize() const;

	// 生成文本
	void GenerateText(PART_CONTAINER& parts, int pos, int num, 
		std::wstring& ws);
	
	// 计算可见文本
	void CalcVisText();

	// 添加文本
	int AppendText(int pos, const wchar_t* ws, int size);
	// 插入文字
	int InsertText(int pos, const wchar_t* ws, int size);
    // 左删除文字
    bool DeleteLeftText(int pos);
	// 删除文字
	bool DeleteText(int pos, int num);
	// 字符输入
	int InputChar(wchar_t ch);
	// 键盘输入
	int InputKey(unsigned int ch);
	// 设置输入位置
    void SetInputPos(int line, int pos);
	void SetInputPos(int pos);
	// 光标宽度
	int GetCaretWidth() const;
	// 获得在指定宽度内的可见长度
	int GetVisualCount(int pos, int width, int&height);
	// 显示宽度
	int GetShowWidth() const;
	// 获得内容宽度
	int GetTotalWidth(PART_CONTAINER& parts, int pos, int num);

	// 复位输入光标
	void ResetCaret();

	// 规范位置
	int NormalizePos(int pos) const;
	// 当前显示文字的起始位置
	int GetVisTextStart(int startpos, int endpos);
	// 鼠标在当前文字中的位置
	int GetCursorInPos(int x, int y, int&line, int&linepos);
	
	// 选定文字起始位置
	int GetSelectBeg() const;
	// 选定文字结束位置
	int GetSelectEnd() const;
	// 增加选定文字
	bool SetSelect(int begin,int end);
	// 删除选定文字
	bool DeleteSelectText();
	// 绘制选定文字
	void PaintSelectText();

    // 尝试越南文组合字符输入
    bool TryVietnamesInput(wchar_t ch);
    // 尝试越南文组合字符输入
    bool TryThaiInput(wchar_t ch);

	// 记录输入
	bool RememberInput();
	// 显示上、下条文字
	bool ShowLastText();
	bool ShowNextText();

    ImageAnimationManager* InnerGetAnimation();
    HyperLinkStyleManager* InnerGetHyperlinkMng();

    // 获取实际光标位置
    int get_real_pos();
private:
	int m_nTopIndex;        // 滚动用的
	int m_nRealTopIndex;    // 这是实际的显示在控件上的开始行
	int m_nRealEndIndex;        
	int m_nCaretInLine;     // 光标所有行
	int m_nCaretLinePos;    // 光标在所有行上的位置 
	int m_nLineHeight;      // 用于计算的行高
	int m_TextWidth;

	std::vector<LineItem> m_vLineItems; // 实际每行的高度

	bool m_bReturnFontFormat;	// 是否返回字体格式
    bool m_bOneLine;     // 单行输入还是多行
	bool m_bReadOnly;
	bool m_bSupportHtml;
	bool m_bOnlyDigit;
	bool m_bMarked;
	bool m_bDragEvent;
    bool m_bChangedEvent;
    bool m_bReturnAllFormat;
    bool m_bEditMode;
	int m_nDragOldX;
	int m_nDragOldY;
	int m_nMaxLength;
	int m_nAlign;
	int m_nScrollStep;
	int m_nSelectBeg;
	int m_nSelectEnd;

	std::wstring m_wsPswdChar;
	std::wstring m_wsText;
	PART_CONTAINER m_Buffer;
    // 这个rect决定了可视区域,相当于窗口的大小
    rect_t m_ViewRect;
    point_t m_CaretPos;
	CColorData m_SelectForeColor;
	CColorData m_SelectBackColor;
	std::string m_strCaretName;
	PERSISTID m_CaretID;
    TinyHtml m_Html;
    std::vector< TextSegment > m_vSimpleText;

	// 记录输入
	bool m_bRemember;
	int m_nMaxRemember;
	int m_nCurRemember;
	std::vector<std::wstring> m_vRemembers;
};

#endif //RichEdit_h_2008_9_18

