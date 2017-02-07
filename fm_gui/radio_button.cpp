//--------------------------------------------------------------------
// 文件名:		radio_button.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "radio_button.h"
#include "i_container.h"

/// \file radio_button.cpp
/// \brief 单选框控件

/// entity: RadioButton
/// \brief 单选框控件实体
/// \see 继承自IControl 
DECLARE_ENTITY(RadioButton, 0, IControl);

/// property: bool Checked
/// \brief 是否选中
DECLARE_PROPERTY(bool, RadioButton, Checked, GetChecked, SetChecked);
/// property: bool HideBox
/// \brief 是否隐藏选择框
DECLARE_PROPERTY(bool, RadioButton, HideBox, GetHideBox, SetHideBox);
/// property: bool ClickEvent
/// \brief 是否产生点击事件
DECLARE_PROPERTY(bool, RadioButton, ClickEvent, GetClickEvent, SetClickEvent);
/// property: bool BoxSize
/// \brief 选择框的尺寸
DECLARE_PROPERTY(int, RadioButton, BoxSize, GetBoxSize, SetBoxSize);
/// property: string NormalImage
/// \brief 正常状态的图元
DECLARE_PROPERTY(const char*, RadioButton, NormalImage, GetNormalImage, 
	SetNormalImage);
/// property: string FocusImage
/// \brief 鼠标选中状态的图元
DECLARE_PROPERTY(const char*, RadioButton, FocusImage, GetFocusImage, 
	SetFocusImage);
/// property: string CheckedImage
/// \brief 被选中状态的图元
DECLARE_PROPERTY(const char*, RadioButton, CheckedImage, GetCheckedImage, 
	SetCheckedImage);
/// property: string DisableImage
/// \brief 被禁止操作状态的图元
DECLARE_PROPERTY(const char*, RadioButton, DisableImage, GetDisableImage, 
	SetDisableImage);

/// property: string FocusBlendColor
/// \brief 鼠标移入状态的混合颜色
DECLARE_PROPERTY(result_string, RadioButton, FocusBlendColor, GetFocusBlendColorString, 
SetFocusBlendColorString);
/// property: string PushBlendColor
/// \brief 被按下状态的混合颜色
DECLARE_PROPERTY(result_string, RadioButton, PushBlendColor, GetPushBlendColorString, 
SetPushBlendColorString);
/// property: string DisableBlendColor
/// \brief 被禁止操作状态的混合颜色
DECLARE_PROPERTY(result_string, RadioButton, DisableBlendColor, GetDisableBlendColorString, 
SetDisableBlendColorString);

/// property: string NormalColor
/// \brief 正常状态的颜色
DECLARE_PROPERTY(result_string, RadioButton, NormalColor, GetNormalColorString, 
	SetNormalColorString);
/// property: string FocusColor
/// \brief 鼠标选中状态的颜色
DECLARE_PROPERTY(result_string, RadioButton, FocusColor, GetFocusColorString, 
	SetFocusColorString);
/// property: string PushColor
/// \brief 被按下状态的颜色
DECLARE_PROPERTY(result_string, RadioButton, PushColor, GetPushColorString, 
	SetPushColorString);
/// property: string DisableColor
/// \brief 被禁止操作状态的颜色
DECLARE_PROPERTY(result_string, RadioButton, DisableColor, 
	GetDisableColorString, SetDisableColorString);

/// property: string NormalFont
/// \brief 正常状态的字体
DECLARE_PROPERTY(const char*, RadioButton, NormalFont, GetNormalFontName, SetNormalFontName);
/// property: string FocusFont
/// \brief 鼠标选中状态的字体
DECLARE_PROPERTY(const char*, RadioButton, FocusFont, GetFocusFontName, SetFocusFontName);
/// property: string PushFont
/// \brief 被按下状态的字体
DECLARE_PROPERTY(const char*, RadioButton, PushFont, GetPushFontName, SetPushFontName);
/// property: string DisableFont
/// \brief 被禁止操作状态的字体
DECLARE_PROPERTY(const char*, RadioButton, DisableFont, GetDisableFontName, SetDisableFontName);

/// event: int on_checked_changed(object self)
/// \brief 选择状态改变的回调
/// \param self 本控件
DECLARE_EVENT(on_checked_changed);

// RadioButton

RadioButton::RadioButton()
{
}

RadioButton::~RadioButton()
{
}

bool RadioButton::Init(const IVarList& args)
{
	if (!CheckButton::Init(args))
	{
		return false;
	}
	
	return true;
}

bool RadioButton::Shut()
{
	return CheckButton::Shut();
}

bool RadioButton::SetCheckValue(bool value)
{
	if (GetChecked())
	{
		return false;
	}
	
	if (!CheckButton::SetCheckValue(value))
	{
		return false;
	}
	
	// 协调同一容器中的其他同类的选中状态
	if (value)
	{
		IControl* parent = GetParentControl();
		
		if (NULL == parent)
		{
			return true;
		}
		
		if (!parent->IsContainer())
		{
			return true;
		}
		
		IContainer* pContainer = (IContainer*)parent;
		
		CVarList child_list;
		
		size_t count = pContainer->GetChildList(child_list);
		
		for (size_t i = 0; i < count; ++i)
		{
			IEntity* pEntity = 
				GetCore()->GetEntity(child_list.ObjectVal(i));

			if (NULL == pEntity)
			{
				continue;
			}
			
			if (!pEntity->GetEntInfo()->IsKindOf("RadioButton"))
			{
				continue;
			}

			RadioButton* p = (RadioButton*)pEntity;
			
			if ((p != this) && p->GetChecked())
			{
				p->ApplyCheckValue(false);
			}
		}
	}
	
	return true;
}

