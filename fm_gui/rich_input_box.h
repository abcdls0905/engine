//--------------------------------------------------------------------
// 文件名:		RichInputBox.h
// 内  容:		多格式文本输入框单行(HTML部分支持)
// 说  明:		
// 创建日期:	2008年9月18日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef RichInputBox_h_2008_9_18
#define RichInputBox_h_2008_9_18

#include "i_control.h"
#include "tinyhtml.h"
// RichEdit
// 文字输入框单行

class HyperLinkStyleManager;
class ImageAnimationManager;
class RichInputBox: public IControl
{
private:
	// 显示类型
	enum
	{
		PARTTYPE_CHAR,	// 字符
		PARTTYPE_IMAGE,	// 图元
	};
	
	typedef std::vector< HtmlItem* > PART_CONTAINER;
	
public:
	RichInputBox();
	virtual ~RichInputBox();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	virtual int OnWidthChanged(int old);
	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonUp(int x, int y);
	virtual int OnDragMove(int x, int y);
	virtual int OnKeyDown(unsigned int vk);
    virtual int OnKeyUp(unsigned int vk);
	virtual int OnChar(unsigned int ch);
	virtual int OnGetFocus(const PERSISTID& lost_id);
	virtual int OnLostFocus(const PERSISTID& get_id);
	virtual int OnChanged(const PERSISTID& changed_id);

	virtual void SetText(const wchar_t* val);
	virtual const wchar_t* GetText() const;
	virtual point_t GetCaretPos();
	// 输入光标名称
	virtual void SetCaretName(const char* name);
	virtual const char* GetCaretName() const;

	// 只读状态
	void SetReadOnly(bool value); 
	bool GetReadOnly() const;
	
	// 只允许输入数字
	void SetOnlyDigit(bool value);
	bool GetOnlyDigit() const;
	
	// 编辑器模式
	void SetEditMode(bool value); 
	bool GetEditMode() const;

	// 是否包含转义符号
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
	
	// 文字起始位移
	void SetTextOffsetX(int value);
	int GetTextOffsetX() const;
	
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
	
    // 记录输入
    void SetRemember(bool bRemember);
    bool GetRemember() const;

    // 设置记录输入文本最大条数
    void SetMaxRemember(int max);
    int GetMaxRemember() const;

    // 记录输入
    void SetReturnFontFormat(bool bValue);
    bool GetReturnFontFormat();

	// 获得选中部分的字符串
	std::wstring GetSelectText();
	// 是否有选中的文字
	bool HasSelectText() const;
	// 清除选中状态
	void ClearSelect();



	// 删除
	bool Cut();
	// 拷贝
	bool Copy();
	// 粘贴
	bool Paste();

	// 添加文字mode 0 添加在后,1添加在前 -1添加在当前光标位置,当前无光标则添加在最后
	bool Append(const wchar_t* text, int mode);
	
    // 尝试越南文组合字符输入
    bool TryVietnamesInput(wchar_t ch);
    // 尝试越南文组合字符输入
    bool TryThaiInput(wchar_t ch);

private:
	void ReleasePart(HtmlItem* part);
	void ReleaseAll();
	
	// 设置光标
	bool SetCaretID(const char* name);

	// 获得当前长度
	int GetSize() const;
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
	// 删除文字
	bool DeleteText(int pos, int num);
	
	// 字符输入
	int InputChar(wchar_t ch);
	// 键盘输入
	int InputKey(unsigned int ch);
	// 设置显示位置
	void SetShowPos(int pos);
	// 设置输入位置
	void SetInputPos(int pos);
	// 光标宽度
	int GetCaretWidth() const;
	// 获得在指定宽度内的可见长度
	int GetVisualCount(int pos, int width);
	// 显示宽度
	int GetShowWidth() const;
	// 获得内容宽度
	int GetTotalWidth(PART_CONTAINER& parts, int pos, int num);

	// 复位输入光标
	void ResetCaret();

	// 规范位置
	int NormalizePos(int pos) const;
	// 当前显示文字的起始位置
	int GetVisTextStart();
	// 鼠标在当前文字中的位置
	int GetCursorInPos(int x, int y);
	
	// 选定文字起始位置
	int GetSelectBeg() const;
	// 选定文字结束位置
	int GetSelectEnd() const;
	// 增加选定文字
	bool IncSelect(int pos, int num);
	// 删除选定文字
	bool DeleteSelectText();
    // 左删除
    bool DeleteLeftText(int pos);
	// 绘制选定文字
	void PaintSelectText(int x1, int y1);

    ImageAnimationManager* InnerGetAnimation();
    HyperLinkStyleManager* InnerGetHyperlinkMng();

private:
	bool m_bReadOnly;
    bool m_bEditMode;
	bool m_bOnlyDigit;
	bool m_bSupportHtml;
	bool m_bDragEvent;
    bool m_bChangedEvent;
    bool m_bReturnFontFormat;
	int m_nDragOldX;
	int m_nDragOldY;
	int m_nMaxLength;
	int m_nTextOffsetX;
	int m_nAlign;
	int m_nScrollStep;
	int m_nInputPos;
	int m_nShowPos;
	int m_nShowCount;
	int m_nSelectBeg;
	int m_nSelectEnd;
    bool m_bRemember;
    int m_nMaxRemember;
    int m_nCurRemember;
    point_t m_CaretPos;
	std::wstring m_wsPswdChar;
	std::wstring m_wsText;
	PART_CONTAINER m_Buffer;
	CColorData m_SelectForeColor;
	CColorData m_SelectBackColor;
	std::string m_strCaretName;
	PERSISTID m_CaretID;
    TinyHtml m_Html;
    std::vector<std::wstring> m_vRemembers;
};

#endif //RichInputBox_h_2008_9_18

