//--------------------------------------------------------------------
// �ļ���:		radio_button.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "radio_button.h"
#include "i_container.h"

/// \file radio_button.cpp
/// \brief ��ѡ��ؼ�

/// entity: RadioButton
/// \brief ��ѡ��ؼ�ʵ��
/// \see �̳���IControl 
DECLARE_ENTITY(RadioButton, 0, IControl);

/// property: bool Checked
/// \brief �Ƿ�ѡ��
DECLARE_PROPERTY(bool, RadioButton, Checked, GetChecked, SetChecked);
/// property: bool HideBox
/// \brief �Ƿ�����ѡ���
DECLARE_PROPERTY(bool, RadioButton, HideBox, GetHideBox, SetHideBox);
/// property: bool ClickEvent
/// \brief �Ƿ��������¼�
DECLARE_PROPERTY(bool, RadioButton, ClickEvent, GetClickEvent, SetClickEvent);
/// property: bool BoxSize
/// \brief ѡ���ĳߴ�
DECLARE_PROPERTY(int, RadioButton, BoxSize, GetBoxSize, SetBoxSize);
/// property: string NormalImage
/// \brief ����״̬��ͼԪ
DECLARE_PROPERTY(const char*, RadioButton, NormalImage, GetNormalImage, 
	SetNormalImage);
/// property: string FocusImage
/// \brief ���ѡ��״̬��ͼԪ
DECLARE_PROPERTY(const char*, RadioButton, FocusImage, GetFocusImage, 
	SetFocusImage);
/// property: string CheckedImage
/// \brief ��ѡ��״̬��ͼԪ
DECLARE_PROPERTY(const char*, RadioButton, CheckedImage, GetCheckedImage, 
	SetCheckedImage);
/// property: string DisableImage
/// \brief ����ֹ����״̬��ͼԪ
DECLARE_PROPERTY(const char*, RadioButton, DisableImage, GetDisableImage, 
	SetDisableImage);

/// property: string FocusBlendColor
/// \brief �������״̬�Ļ����ɫ
DECLARE_PROPERTY(result_string, RadioButton, FocusBlendColor, GetFocusBlendColorString, 
SetFocusBlendColorString);
/// property: string PushBlendColor
/// \brief ������״̬�Ļ����ɫ
DECLARE_PROPERTY(result_string, RadioButton, PushBlendColor, GetPushBlendColorString, 
SetPushBlendColorString);
/// property: string DisableBlendColor
/// \brief ����ֹ����״̬�Ļ����ɫ
DECLARE_PROPERTY(result_string, RadioButton, DisableBlendColor, GetDisableBlendColorString, 
SetDisableBlendColorString);

/// property: string NormalColor
/// \brief ����״̬����ɫ
DECLARE_PROPERTY(result_string, RadioButton, NormalColor, GetNormalColorString, 
	SetNormalColorString);
/// property: string FocusColor
/// \brief ���ѡ��״̬����ɫ
DECLARE_PROPERTY(result_string, RadioButton, FocusColor, GetFocusColorString, 
	SetFocusColorString);
/// property: string PushColor
/// \brief ������״̬����ɫ
DECLARE_PROPERTY(result_string, RadioButton, PushColor, GetPushColorString, 
	SetPushColorString);
/// property: string DisableColor
/// \brief ����ֹ����״̬����ɫ
DECLARE_PROPERTY(result_string, RadioButton, DisableColor, 
	GetDisableColorString, SetDisableColorString);

/// property: string NormalFont
/// \brief ����״̬������
DECLARE_PROPERTY(const char*, RadioButton, NormalFont, GetNormalFontName, SetNormalFontName);
/// property: string FocusFont
/// \brief ���ѡ��״̬������
DECLARE_PROPERTY(const char*, RadioButton, FocusFont, GetFocusFontName, SetFocusFontName);
/// property: string PushFont
/// \brief ������״̬������
DECLARE_PROPERTY(const char*, RadioButton, PushFont, GetPushFontName, SetPushFontName);
/// property: string DisableFont
/// \brief ����ֹ����״̬������
DECLARE_PROPERTY(const char*, RadioButton, DisableFont, GetDisableFontName, SetDisableFontName);

/// event: int on_checked_changed(object self)
/// \brief ѡ��״̬�ı�Ļص�
/// \param self ���ؼ�
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
	
	// Э��ͬһ�����е�����ͬ���ѡ��״̬
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

