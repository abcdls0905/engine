//--------------------------------------------------------------------
// 文件名:		float_edit.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年11月20日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#include <stdlib.h>
#include <wchar.h>
#include "float_edit.h"
#include "gui_input.h"
//#include "StringUtil.h"
#include "../public/auto_mem.h"
#include "../public/converts.h"

/// \file float_edit.cpp
/// \brief 浮点数输入框

/// entity: FloatEdit
/// \brief 浮点数输入框实体
/// \see 继承自Edit
DECLARE_ENTITY(FloatEdit, 0, Edit);

/// property: const wchar_t* Format
/// \brief 浮点值精度
DECLARE_PROPERTY(const wchar_t*, FloatEdit, Format, GetFormat, SetFormat);
/// property: float DragStep
/// \brief 拖动时的变化步长
DECLARE_PROPERTY(float, FloatEdit, DragStep, GetDragStep, SetDragStep);
/// property: float Default
/// \brief 默认值
DECLARE_PROPERTY(float, FloatEdit, Default, GetDefault, SetDefault);
/// property: float Min
/// \brief 最小值
DECLARE_PROPERTY(float, FloatEdit, Min, GetMin, SetMin);
/// property: float Max
/// \brief 最大值
DECLARE_PROPERTY(float, FloatEdit, Max, GetMax, SetMax);
/// property: bool Loop
/// \brief 拖拽超过最大(小)值时是否自动循环到最小(大)值
DECLARE_PROPERTY(bool, FloatEdit, Loop, GetLoop, SetLoop);
/// property: bool ShowHelpInfo
/// \brief 设置是否显示帮助信息
DECLARE_PROPERTY(bool, FloatEdit, ShowHelpInfo, GetShowHintText, 
	SetShowHintText);
/// property: const wchar_t* HelpInfo
/// \brief 帮助信息
DECLARE_PROPERTY(const wchar_t*, FloatEdit, HelpInfo, GetAppendHintText, 
	SetAppendHintText);

FloatEdit::FloatEdit()
{
	memset(&m_ptLButtonDownPos, 0, sizeof(m_ptLButtonDownPos));

	m_wsFormat = L"%.4f";
	m_bIsInteger = false;
	m_bIsDragMethod = false;
	m_fDragStep = 0.001F;
	m_fOldDragStep = 0.001F;
	m_fDefault = 0.0F;
	m_fMin = 0.0F;
	m_fMax = 100.0F;
	m_bLoop = false;
	m_bHadChangedStep_s = false;
	m_bHadChangedStep_c = false;
	m_OldPoint.x = 0;
	m_OldPoint.y = 0;

	m_bShowHintText = true;
	m_wsAppendHintText = L"";

	UpdateHintText(m_fMin, m_fMax);

	m_pHintTextLabel = NULL;
}

FloatEdit::~FloatEdit()
{
}

bool FloatEdit::Init(const IVarList& args)
{
	if (!IControl::Init(args))
	{
		return false;
	}

	SetCaretName("Default");
	SetCursorName("WIN_IBEAM");

	// 设置区别于Edit的属性
	// 仅能处理浮点值
	this->SetOnlyDigit(true);
	// 有默认值
	this->SetText(L"0.0");
	// 默认不可以拖动
	this->SetDragEvent(false);
	// 默认打开检测浮点数文本框内是否发生变化的功能
	this->SetChangedEvent(true);

	// 默认不显示提示信息
	SetShowHintText(false);
	// 默认没有附加提示信息
	SetAppendHintText(L"");

	return true;
}

bool FloatEdit::Shut()
{
	return IControl::Shut();
}

void FloatEdit::PrepareDesignInfo(IDesignInfo* di)
{
	Edit::PrepareDesignInfo(di);

	di->AddPropInfo("Format", IDesignInfo::TYPE_WIDESTR, true, true);
	di->AddPropInfo("Min", IDesignInfo::TYPE_FLOAT, true, true);
	di->AddPropInfo("Max", IDesignInfo::TYPE_FLOAT, true, true);
	di->AddPropInfo("DragStep", IDesignInfo::TYPE_FLOAT, true, true);
	di->AddPropInfo("Loop", IDesignInfo::TYPE_BOOL, true, true);
}

static inline bool is_integer(const wchar_t* format)
{
	size_t size = GetToStringLen(format);
	TAutoMem<char, 128, TCoreAlloc> auto_buf(size);
	char* buf = auto_buf.GetBuffer();

	ToString(format, buf, size);

	size_t len = strlen(buf);
	size_t k = 0;

	while (k < len)
	{
		if ((buf[k] == '%') || (buf[k] == '.') || (buf[k] == 'f'))
		{
			memmove(buf + k, buf + k + 1, len - k);
			--len;
		}
		else
		{
			++k;
		}
	}

	//std::string convert = buf;
	//StringUtil::StrReplace(convert, "%", "");
	//StringUtil::StrReplace(convert, ".", "");
	//StringUtil::StrReplace(convert, "f", "");

	//int precision = atoi(convert.c_str());
	int precision = atoi(buf);

	if (precision > 0)
	{
		return false;
	}

	return true;
}

void FloatEdit::SetFormat(const wchar_t* value)
{
	m_wsFormat = value;

	m_bIsInteger = is_integer(value);

	SetValue((float)wcstod(GetText(), NULL));
}

const wchar_t* FloatEdit::GetFormat() const
{
	return m_wsFormat.c_str();
}

void FloatEdit::SetDragStep(float value)
{
	m_fDragStep = m_fOldDragStep = value;
}

float FloatEdit::GetDragStep() const
{
	return m_fDragStep;
}

void FloatEdit::SetDefault(float value)
{
	m_fDefault = value;
}

float FloatEdit::GetDefault() const
{
	return m_fDefault;
}

void FloatEdit::SetMin(float value)
{
	m_fMin = value;
	
	if (m_fDefault < m_fMin)
	{
		m_fDefault = m_fMin;
	}

	UpdateHintText(m_fMin, m_fMax);
}

float FloatEdit::GetMin() const
{
	return m_fMin;
}

void FloatEdit::SetMax(float value)
{
	m_fMax = value;

	if (m_fDefault > m_fMax)
	{
		m_fDefault = m_fMax;
	}

	UpdateHintText(m_fMin, m_fMax);
}

float FloatEdit::GetMax() const
{
	return m_fMax;
}

void FloatEdit::SetLoop(bool value)
{
	m_bLoop = value;
}

bool FloatEdit::GetLoop() const
{
	return m_bLoop;
}

void FloatEdit::SetAppendHintText(const wchar_t* name)
{
	m_wsAppendHintText = name;

	UpdateHintText(m_fMin, m_fMax);
}

const wchar_t* FloatEdit::GetAppendHintText() const
{
	return m_wsAppendHintText.c_str();
}

void FloatEdit::SetShowHintText(bool value)
{
	m_bShowHintText = value;

	if (m_bShowHintText && !m_pHintTextLabel)
	{
		// 创建缺省浮动提示框
		Label* pLabel = (Label*)CreateSuborControl(this, "Label");

		pLabel->SetVisible(false);
		pLabel->SetSolid(true);
		pLabel->SetNoFrame(false);
		pLabel->SetAutoSize(true);
		pLabel->SetTransparent(false);
		pLabel->SetBackColor(CGuiConst::DEF_HINT_COLOR);
		pLabel->SetAlignString("Center");

		m_pHintTextLabel = pLabel;
	}
	else if (!m_bShowHintText && m_pHintTextLabel)
	{
		SAFE_RELEASE(m_pHintTextLabel);
	}
}

bool FloatEdit::GetShowHintText() const
{
	return m_bShowHintText;
}

void FloatEdit::UpdateHintText(float min, float max)
{
	wchar_t wsValue[512];

	//SafeSwprintf(wsValue, sizeof(wsValue), m_wsFormat.c_str(), min);
	if (m_bIsInteger)
	{
		SafeSwprintf(wsValue, sizeof(wsValue), L"%d", (int)min);
	}
	else
	{
		SafeSwprintf(wsValue, sizeof(wsValue), m_wsFormat.c_str(), min);
	}

	m_wsHintText = L"Min=";
	m_wsHintText += wsValue;

	//SafeSwprintf(wsValue, sizeof(wsValue), m_wsFormat.c_str(), max);
	if (m_bIsInteger)
	{
		SafeSwprintf(wsValue, sizeof(wsValue), L"%d", (int)max);
	}
	else
	{
		SafeSwprintf(wsValue, sizeof(wsValue), m_wsFormat.c_str(), max);
	}

	m_wsHintText += L"; Max=";
	m_wsHintText += wsValue;

	m_wsHintText += L"; ";
	m_wsHintText += m_wsAppendHintText.c_str();
}

bool FloatEdit::IsValid()
{
	if (m_fMin > m_fMax || m_fDefault < m_fMin || m_fDefault > m_fMax)
	{
		return false;
	}

	const wchar_t* wsValue = GetText();
	size_t len = wcslen(wsValue);
	bool bHadDot = false;

	for (size_t i = 0; i < len; i++)
	{
		if (wsValue[i] >= L'0' && wsValue[i] <= L'9')
		{
			continue;
		}

		if (0 == i && L'-' == wsValue[i])
		{
			continue;
		}

		if (L'.' == wsValue[i])
		{
			if (m_bIsInteger)
			{
				return false;
			}

			if (!bHadDot)
			{
				bHadDot = true;
				continue;
			}
			else
			{
				return false;
			}
		}

		//if (L'.' == wsValue[i] && !bHadDot)
		//{
		//	bHadDot = true;
		//	continue;
		//}

		return false;
	}

	float fValue = (float)wcstod(wsValue, NULL);

	if (fValue > m_fMax || fValue < m_fMin)
	{
		return false;
	}

	return true;
}

void FloatEdit::SetValue(float value)
{
	if (value < m_fMin)
	{
		value = m_bLoop ? m_fMax : m_fMin;
	}
	else if (value > m_fMax)
	{
		value = m_bLoop ? m_fMin : m_fMax;
	}

	wchar_t wsValue[512];

	//SafeSwprintf(wsValue, sizeof(wsValue), m_wsFormat.c_str(), value);

	if (m_bIsInteger)
	{
		SafeSwprintf(wsValue, sizeof(wsValue), L"%d", (int)value);
	}
	else
	{
		SafeSwprintf(wsValue, sizeof(wsValue), m_wsFormat.c_str(), value);
	}

	SetText(wsValue);
}

int FloatEdit::OnLButtonDown(int x, int y)
{ 
	if (!GetEnabled())
	{
		return 0;
	}

	//GetGui()->GetCursorCoord(m_OldPoint);
	//GetCursorPos(&m_OldPoint);

	int width = GetTotalWidth(GetBuffer(), GetShowPos(), GetShowCount());
	int sx = x - InnerGetAbsLeft();

	if (sx > width)
	{
		if (HasSelectText())
		{
			ClearSelect();
		}
		else
		{
			IncSelect(0, GetSize());
		}

		SetInputPos(GetSize());
	}
	else
	{
		if (HasSelectText())
		{
			ClearSelect();
		}

		int pos = GetCursorInPos(x, y);

		if ((pos >= 0) && (pos < GetShowCount()))
		{
			SetInputPos(GetShowPos() + pos);
		}
	}

	if (GetDragEvent())
	{
		GetGui()->GetCursorCoord(m_ptLButtonDownPos);

		this->SetOldText(GetText());

		if(!GetReadOnly())
		{
			m_bIsDragMethod = true;
		}
#ifdef _WIN32
		SetCursor(LoadCursor(NULL, IDC_SIZENS));
#endif
	}

	return 1;
}

int FloatEdit::OnLButtonUp(int x, int y)
{
	if (GetDragEvent())
	{
		m_bIsDragMethod = false;
	}

	return 1; 
}

int FloatEdit::OnRButtonDown(int x, int y)
{
	if (GetDragEvent())
	{
		if (m_bIsDragMethod)
		{
			SetText(GetOldText());
			m_bIsDragMethod = false;
		}
	}

	return 1;
}

int FloatEdit::OnDragMove(int x, int y)
{
//	if (GetDragEvent())
//	{
//		if (!m_bIsDragMethod)
//		{
//			return 1;
//		}
//
//		point_t ptCurrent;
//		point_t ptCurrentInGui;
//
//		//GetCursorPos(&ptCurrent);
//		GetGui()->GetCursorCoord(ptCurrentInGui);
//
//		point_t point;
//
//		point.x = (m_ptLButtonDownPos.x - ptCurrentInGui.x);
//		point.y = (m_ptLButtonDownPos.y - ptCurrentInGui.y);
//
//		int nHeight = GetSystemMetrics(SM_CYSCREEN);
//
//		// 如果鼠标移动到了屏幕最上
//		if (0 == ptCurrent.y)
//		{
//			m_ptLButtonDownPos.y += nHeight;
//			ptCurrent.y = nHeight - 2;
//			//SetCursorPos(ptCurrent.x, ptCurrent.y);
//		}
//		// 如果鼠标移动到了屏幕最下
//		else if (ptCurrent.y == nHeight - 1)
//		{
//			m_ptLButtonDownPos.y -= nHeight;
//			ptCurrent.y = 1;
//			SetCursorPos(ptCurrent.x, ptCurrent.y);
//		}
//		else
//		{
//			int dis = m_OldPoint.y - ptCurrent.y;
//			float value = dis * m_fDragStep + (float)_wtof(GetText());
//
//			SetValue(value);
//		}
//
//		m_OldPoint = ptCurrent;
//		GuiUtil_RunCallback(this, "on_drag", 
//			CVarList() << point.x << point.y);
//	}

	return 1;
}

int FloatEdit::OnKeyDown(unsigned int vk)
{
	if (vk == CGuiInput::KEY_TAB)
	{
		return 0;
	}

	if (vk == CGuiInput::KEY_HOME)
	{
		wchar_t wsValue[512];

		//SafeSwprintf(wsValue, sizeof(wsValue), m_wsFormat.c_str(), m_fDefault);

		if (m_bIsInteger)
		{
			SafeSwprintf(wsValue, sizeof(wsValue), L"%d", (int)m_fDefault);
		}
		else
		{
			SafeSwprintf(wsValue, sizeof(wsValue), m_wsFormat.c_str(), m_fDefault);
		}

		SetText(wsValue);
	}

	if (GetDragEvent())
	{
		if (vk == CGuiInput::KEY_SHIFT)
		{
			if (!m_bHadChangedStep_s)
			{
				SetOldText(GetText());

				m_fDragStep *= 10.0F;
				m_bHadChangedStep_s = true;

				GetGui()->GetCursorCoord(m_ptLButtonDownPos);
			}
		}

		if (vk == CGuiInput::KEY_CONTROL)
		{
			if (!m_bHadChangedStep_c)
			{
				SetOldText(GetText());

				m_fDragStep *= 0.1F;
				m_bHadChangedStep_c = true;

				GetGui()->GetCursorCoord(m_ptLButtonDownPos);
			}
		}
	}

	if (vk == CGuiInput::KEY_RETURN)
	{
		return GuiUtil_RunCallback(this, "on_enter", CVarList());
	}

	int res = InputKey(vk);

	if (res == 1)
	{
		ResetCaret();
		CalcVisText();
		OnChanged(GetID());
	}

	return 1;
}

int FloatEdit::OnKeyUp(unsigned int vk)
{
	if (GetDragEvent())
	{
		if (vk == CGuiInput::KEY_SHIFT)
		{
			if (m_bHadChangedStep_s)
			{
				m_fDragStep = m_fOldDragStep;
				m_bHadChangedStep_s = false;

				SetOldText(GetText());
			}
		}

		if (vk == CGuiInput::KEY_CONTROL)
		{
			if (m_bHadChangedStep_c)
			{
				m_fDragStep = m_fOldDragStep;
				m_bHadChangedStep_c = false;

				SetOldText(GetText());
			}
		}
	}

	return 1;
}

//ch 为 unicode
int FloatEdit::OnChar(unsigned int ch)
{
	if (m_bIsInteger)
	{
		if (ch == L'.')
		{
			return 1;
		}
	}

	return Edit::OnChar(ch);
}

int FloatEdit::OnGetCapture(const PERSISTID& lost_id)
{
	if (!m_bShowHintText)
	{
		return 0;
	}

	if (!m_pHintTextLabel)
	{
		return 0;
	}

	m_pHintTextLabel->SetVisible(true);
	m_pHintTextLabel->SetTop(-GetHeight());
	m_pHintTextLabel->SetLeft(GetWidth());
	m_pHintTextLabel->SetText(m_wsHintText.c_str());

	GetRootControl()->AddTopLayer(m_pHintTextLabel->GetID());

	return 1;
}

int FloatEdit::OnLostCapture(const PERSISTID& get_id)
{
	if (!m_bShowHintText)
	{
		return 0;
	}

	if (!m_pHintTextLabel)
	{
		return 0;
	}

	GetRootControl()->RemoveTopLayer(m_pHintTextLabel->GetID());

	m_pHintTextLabel->SetVisible(false);

	return 1;
}

int FloatEdit::OnChanged(const PERSISTID& changed_id)
{
	if (!GetChangedEvent())
	{
		return 0;
	}

	if (wcscmp(GetText(), GetOldText()) != 0)
	{
		if (!IsValid())
		{
			bool old_change_event = GetChangedEvent();
			const result_wstring wStr = GetOldText();

			SetChangedEvent(false);
			SetText(wStr.c_str());
			SetChangedEvent(old_change_event);
			SetInputPos((int)wStr.length());
			return 0;
		}

		GuiUtil_RunCallback(this, "on_changed", CVarList() << changed_id);
		SetOldText(GetText());
		return 1;
	}

	return 0;
}
