//--------------------------------------------------------------------
// 文件名:		button.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月14日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _BUTTON_H
#define _BUTTON_H

#include "../public/core_type.h"
#include "i_control.h"

// 按钮

class Button: public IControl
{
public:
	Button();
	virtual ~Button();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual int GetMinWidth() const { return 1; }
	virtual int GetMinHeight() const { return 1; }
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// 是否加载完成
	virtual bool GetLoadFinish();

	virtual int OnKeyDown(unsigned int vk);
	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonUp(int x, int y);
	virtual int OnLButtonDoubleClick(int x, int y);
	virtual int OnLButtonHover(int x, int y);
	virtual int OnDragMove(int x, int y);
	
    // 判断指定位置是否透明
    virtual bool TestTransparent(int x, int y);

	// 对齐方式
	void SetAlignString(const char* value);
	const char* GetAlignString() const;

	// 鼠标持续按下是否作为重复点击
	void SetHoverRepeat(bool value);
	bool GetHoverRepeat() const;

	// 鼠标按下时是否产生事件
	void SetPushEvent(bool value);
	bool GetPushEvent() const;
	
    // 鼠标移出放开时，是否产生点击事件
    void SetOutUpEvent(bool value);
    bool GetOutUpEvent() const;

    // 鼠标拖拽，是否产生点击事件
    void SetDragEvent(bool value);
    bool GetDragEvent() const;

	// 内置的几种外观
	void SetStyleString(const char* value);
	const char* GetStyleString() const;
	
	// 正常状态下的贴图
	void SetNormalImage(const char* value);
	const char* GetNormalImage() const;
	
	// 鼠标定位时的贴图
	void SetFocusImage(const char* value);
	const char* GetFocusImage() const;
	
	// 按下时的贴图
	void SetPushImage(const char* value);
	const char* GetPushImage() const;
	
	// 被禁止时的贴图
	void SetDisableImage(const char* value);
	const char* GetDisableImage() const;

	// 正常状态的字体名
	virtual void SetNormalFontName(const char* val);
	const char* GetNormalFontName() const;

	// 鼠标选中状态的字体名
	virtual void SetFocusFontName(const char* val);
	const char* GetFocusFontName() const;

	// 被按下状态的字体名
	virtual void SetPushFontName(const char* val);
	const char* GetPushFontName() const;

	// 被禁止操作状态的字体名
	virtual void SetDisableFontName(const char* val);
	const char* GetDisableFontName() const;

	// 鼠标移入状态的混合颜色字符串
	void SetFocusBlendColorString(const char* val);
	result_string GetFocusBlendColorString() const;

	// 被按下状态的混合颜色字符串
	void SetPushBlendColorString(const char* val);
	result_string GetPushBlendColorString() const;

	// 被禁止操作状态的混合颜色字符串
	void SetDisableBlendColorString(const char* val);
	result_string GetDisableBlendColorString() const;

	// 正常状态的颜色字符串
	void SetNormalColorString(const char* val);
	result_string GetNormalColorString() const;

	// 鼠标选中状态的颜色字符串
	void SetFocusColorString(const char* val);
	result_string GetFocusColorString() const;

	// 被按下状态的颜色字符串
	void SetPushColorString(const char* val);
	result_string GetPushColorString() const;

	// 被禁止操作状态的颜色字符串
	void SetDisableColorString(const char* val);
	result_string GetDisableColorString() const;

    // 设置按下状态
    void SetPressed(bool bValue);

private:
	// 是否处于按下状态
	bool GetPressed() const
	{
		return m_bPush;
	}

private:
	bool m_bPush;
	bool m_bHoverRepeat;
	bool m_bPushEvent;
    bool m_bOutUpEvent;
    bool m_bDragEvent;
	int m_nAlign;
	int m_nStyle;
	int m_nClickX;
	int m_nClickY;
    int m_nDragOldX;
    int m_nDragOldY;
	CImagePlayer m_FocusImage;
	CImagePlayer m_PushImage;
	CImagePlayer m_DisableImage;
	// 按钮各种状态下的混合颜色
	CColorData m_FocusBlendColor;
	CColorData m_PushBlendColor;
	CColorData m_DisableBlendColor;
	// 按钮各种状态下的文字颜色
	CColorData m_NormalColor;
	CColorData m_FocusColor;
	CColorData m_PushColor;
	CColorData m_DisableColor;
	// 按钮各种状态下的字体
	core_string m_strNormalFontName;
	core_string m_strFocusFontName;
	core_string m_strPushFontName;
	core_string m_strDisableFontName;
};

#endif // _BUTTON_H
