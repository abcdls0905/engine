//--------------------------------------------------------------------
// 文件名:		edit.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _EDIT_H
#define _EDIT_H

#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "i_control.h"
//#include <windef.h>

// 文字输入框

class Edit: public IControl
{
private:
	// 显示类型
	enum
	{
		PARTTYPE_CHAR,	// 字符
		PARTTYPE_IMAGE,	// 图元
	};
	
	struct part_base_t
	{
		int nType;		// 类型
		int nWidth;		// 宽度
		int nAdvance;	// 步长
		int nOffset;	// 偏移
		part_base_t()
			:nOffset(0)
		{}
	};
	
	struct part_char_t: public part_base_t
	{
		wchar_t nOrigin;	// 实际的字符
		wchar_t nVisual;	// 显示的字符
	};
	
	struct part_image_t: public part_base_t
	{
		core_wstring wsText;
		CImagePlayer Image;		// 图元
	};	
	
	typedef TArrayPod<part_base_t*, 1, TCoreAlloc> part_container_t;
	
public:
	Edit();
	virtual ~Edit();

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
	virtual void SetOldText(const wchar_t* val);
	virtual const wchar_t* GetText() const;
	virtual const wchar_t* GetOldText() const;
    virtual point_t GetCaretPos();
	virtual bool GetCanIMEFocus() const { return true; }
	
	// 输入光标名称
	virtual void SetCaretName(const char* name);
	virtual const char* GetCaretName() const;

	// 只读状态
	void SetReadOnly(bool value); 
	bool GetReadOnly() const;
	
	// 只允许输入数字
	void SetOnlyDigit(bool value);
	bool GetOnlyDigit() const;
	
    // 设置最大输入数字，在m_bOnlyDigit为true时有效
    void SetMaxDigit(int value);
    int GetMaxDigit() const ;

	// 是否包含转义符号
	void SetMarked(bool value);
	bool GetMarked() const;
	
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
	
	bool GetPasswordStage() const;

	// 对齐方式
	void SetAlignString(const char* value);
	const char* GetAlignString() const;
	
	// 密码显示字符
	void SetPasswordChar(const wchar_t* value);
	const wchar_t* GetPasswordChar() const;
	
    // 输入位置
    void SetInputPos(int pos);
    const int GetInputPos() const;

	// 选中时的颜色
	void SetSelectForeColorString(const char* value);
	result_string GetSelectForeColorString() const;
	
	// 选中时的背景颜色
	void SetSelectBackColorString(const char* value);
	result_string GetSelectBackColorString() const;
	
	// 获得选中部分的字符串
	result_wstring GetSelectText();
	// 是否有选中的文字
	bool HasSelectText() const;
	// 清除选中状态
	bool ClearSelect();
	// 删除选定文字
	bool DeleteSelectText();
	// 删除文字
	bool DeleteText(int pos, int num);
	// 重设Edit信息
	bool ResetEditInfo();
	// 删除
	bool Cut();
	// 拷贝
	bool Copy();
	// 粘贴
	bool Paste();

	// 添加文字
	bool Append(const wchar_t* text);
	
protected:
	//virtual bool GetAllowTab() const { return false; }
	//virtual bool GetAllowReturn() const { return false; }

	void ReleasePart(part_base_t* part);
	void ReleaseAll();
	
	// 设置光标
	bool SetCaretID(const char* name);

	// 获得当前长度
	int GetSize() const;
	// 获得最大长度
	int GetMaxSize() const;

	// 生成文本
	void GenerateText(part_container_t& parts, int pos, int num, 
		core_wstring& ws);
	
	// 计算可见文本
	void CalcVisText();

	// 添加文本
	int AppendText(int pos, const wchar_t* ws, int size);
	// 插入文字
	int InsertText(int pos, const wchar_t* ws, int size);
	
	// 字符输入
	int InputChar(wchar_t ch);
	// 键盘输入
	int InputKey(unsigned int ch);
	// 设置显示位置
	void SetShowPos(int pos);
	// 光标宽度
	int GetCaretWidth() const;
	// 获得在指定宽度内的可见长度
	int GetVisualCount(int pos, int width);
	// 显示宽度
	int GetShowWidth() const;
	// 获得内容宽度
	int GetTotalWidth(part_container_t& parts, int pos, int num);

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
	// 绘制选定文字
	void PaintSelectText(int x1, int y1);

	part_container_t& GetBuffer() { return m_Buffer; }
	// 获得显示位置
	int GetShowPos() const { return m_nShowPos; }
	// 获得可见文本数
	int GetShowCount() const { return m_nShowCount; }

private:
	bool m_bReadOnly;
	bool m_bOnlyDigit;
	bool m_bMarked;
	bool m_bDragEvent;
	bool m_bChangedEvent;
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
    int m_nMaxDigit;
    point_t m_CaretPos;
	core_wstring m_wsPswdChar;
	core_wstring m_wsText;
	core_wstring m_wsOldText;
	part_container_t m_Buffer;
	CColorData m_SelectForeColor;
	CColorData m_SelectBackColor;
	core_string m_strCaretName;
	PERSISTID m_CaretID;
};

#endif // _EDIT_H

