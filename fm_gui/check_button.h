//--------------------------------------------------------------------
// 文件名:		check_button.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _CHECK_BUTTON_H
#define _CHECK_BUTTON_H

#include "../public/core_type.h"
#include "i_control.h"

// 单选框

class CheckButton: public IControl
{
private:
	enum { MIN_BOX_SIZE = 4 };

public:
	CheckButton();
	virtual ~CheckButton();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// 是否加载完成
	virtual bool GetLoadFinish();

	virtual int OnKeyDown(unsigned int vk);
	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonUp(int x, int y);

	virtual bool SetCheckValue(bool value);

	bool ApplyCheckValue(bool value);

	// 选中状态
	void SetChecked(bool value);
	bool GetChecked() const;
	
	// 隐藏选中框
	void SetHideBox(bool value);
	bool GetHideBox() const;

	// 是否产生点击事件
	void SetClickEvent(bool value);
	bool GetClickEvent() const;

	// 选中框的尺寸
	void SetBoxSize(int value);
	int GetBoxSize() const;

	// 正常状态下的贴图
	void SetNormalImage(const char* value);
	const char* GetNormalImage() const;

	// 鼠标定位时的贴图
	void SetFocusImage(const char* value);
	const char* GetFocusImage() const;

	// 选中时的贴图
	void SetCheckedImage(const char* value);
	const char* GetCheckedImage() const;
	
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
	
private:
	// 是否处于按下状态
	bool GetPressed() const
	{
		return m_bPush ^ m_bChecked;
	}
	
private:
	bool m_bChecked;
	bool m_bHideBox;
	bool m_bClickEvent;
	bool m_bPush;
	int m_nBoxSize;
	int m_nClickX;
	int m_nClickY;
	CImagePlayer m_FocusImage;
	CImagePlayer m_CheckedImage;
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

#endif // _CHECK_BUTTON_H

