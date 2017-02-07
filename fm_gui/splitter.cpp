//--------------------------------------------------------------------
// 文件名:		splitter.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年4月23日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "splitter.h"
#include "group_box.h"
#include "adjuster.h"

/// \file splitter.cpp
/// \brief 平面分割器

/// entity: Splitter
/// \brief 平面分割器实体
/// \see 继承自IControl 
DECLARE_ENTITY(Splitter, 0, IControl);

/// readonly: object Adjuster
/// \brief 调节器对象
DECLARE_PROPERTY_GET(PERSISTID, Splitter, Adjuster, GetAdjuster);
/// readonly: object Panel1
/// \brief 面板1 
DECLARE_PROPERTY_GET(PERSISTID, Splitter, Panel1, GetPanel1);
/// readonly: object Panel2
/// \brief 面板2 
DECLARE_PROPERTY_GET(PERSISTID, Splitter, Panel2, GetPanel2);

/// property: string Orient
/// \brief 方向（"Vertical"-垂直，"Horizon"-水平）
DECLARE_PROPERTY(const char*, Splitter, Orient, GetOrient, SetOrient);
/// property: int MinPanelSize
/// \brief 最小面板尺寸
DECLARE_PROPERTY(int, Splitter, MinPanelSize, GetMinPanelSize, 
	SetMinPanelSize);
/// property: int PaddingWidth
/// \brief 附加边界宽度
DECLARE_PROPERTY(int, Splitter, PaddingWidth, GetPaddingWidth, 
	SetPaddingWidth);
/// property: int AdjusterWidth
/// \brief 调节器宽度
DECLARE_PROPERTY(int, Splitter, AdjusterWidth, GetAdjusterWidth, 
	SetAdjusterWidth);
/// property: bool AdjusterFixed
/// \brief 调节器是否固定
DECLARE_PROPERTY(bool, Splitter, AdjusterFixed, GetAdjusterFixed, 
	SetAdjusterFixed);

// Splitter

Splitter::Splitter()
{
	m_pAdjuster = NULL;
	m_pPanel1 = NULL;
	m_pPanel2 = NULL;
	m_nOrient = 0;
	m_nMinPanelSize = 16;
	m_nPaddingWidth = 1;
	m_nAdjusterWidth = 2;
	m_bAdjusterFixed = false;
	
	SetDesignSize(16, 60);
}

Splitter::~Splitter()
{
}

bool Splitter::Init(const IVarList& args)
{
	if (!IControl::Init(args))
	{
		return false;
	}

	m_pAdjuster = (Adjuster*)CreateSuborControl(this, "Adjuster");
	m_pPanel1 = (GroupBox*)CreateSuborControl(this, "GroupBox");
	m_pPanel2 = (GroupBox*)CreateSuborControl(this, "GroupBox");

	m_pAdjuster->SetNoFrame(true);
	m_pPanel1->SetNoFrame(true);
	m_pPanel2->SetNoFrame(true);

	CalcSuborRect();

	return true;
}

bool Splitter::Shut()
{
	SAFE_RELEASE(m_pAdjuster);
	SAFE_RELEASE(m_pPanel1);
	SAFE_RELEASE(m_pPanel2);

	return IControl::Shut();
}

void Splitter::PrepareDesignInfo(IDesignInfo* di)
{
	IControl::PrepareDesignInfo(di);

	di->RemovePropInfo("Font");
	di->RemovePropInfo("Text");

    CVar v = CVar(VTYPE_STRING, "Vertical");
	di->AddPropInfo("Orient", IDesignInfo::TYPE_STRING, true, false,
		&v );
    v = CVar(VTYPE_INT, 16);
	di->AddPropInfo("MinPanelSize", IDesignInfo::TYPE_INT, true, false,
		&v );
    
    v = CVar(VTYPE_INT, 2);
	di->AddPropInfo("PaddingWidth", IDesignInfo::TYPE_INT, true, false,
		&v );
    
    v = CVar(VTYPE_INT, 4);
	di->AddPropInfo("AdjusterWidth", IDesignInfo::TYPE_INT, true, false,
		&v );
	di->AddPropInfo("AdjusterFixed", IDesignInfo::TYPE_BOOL, true, false);
	
	di->AddSuborInfo("Adjuster", "Adjuster");
	di->AddSuborInfo("Panel1", "GroupBox");
	di->AddSuborInfo("Panel2", "GroupBox");
}

bool Splitter::GetLoadFinish()
{
    if (GetLoadFinishValue())
    {
        return true;
    }

	if (m_pAdjuster && !m_pAdjuster->GetLoadFinish())
	{
		return false;
	}

	if (m_pPanel1 && !m_pPanel1->GetLoadFinish())
	{
		return false;
	}

	if (m_pPanel2 && !m_pPanel2->GetLoadFinish())
	{
		return false;
	}
	
	return IControl::GetLoadFinish();
}

void Splitter::Paint(float seconds)
{
	if (!GetVisible())
	{
		return;
	}
	
    RefreshInnerAbsPos();

	int x1 = InnerGetAbsLeft();
	int y1 = InnerGetAbsTop();
	int x2 = x1 + GetWidth();
	int y2 = y1 + GetHeight();

	if (HasBackImage())
	{
		DrawBackImage(seconds, x1, y1, x2, y2);
	}
	else
	{
		GetPainter()->FillRect(x1, y1, x2, y2, GetBackColor());
		
		if (!GetNoFrame())
		{
			GetPainter()->DrawRect(x1, y1, x2, y2, GetLineColor());
		}
	}

	m_pAdjuster->Paint(seconds);
	m_pPanel1->Paint(seconds);
	m_pPanel2->Paint(seconds);
}

PERSISTID Splitter::GetAdjuster() const
{
	if (NULL == m_pAdjuster)
	{
		return PERSISTID();
	}

	return m_pAdjuster->GetID();
}

PERSISTID Splitter::GetPanel1() const
{
	if (NULL == m_pPanel1)
	{
		return PERSISTID();
	}

	return m_pPanel1->GetID();
}

PERSISTID Splitter::GetPanel2() const
{
	if (NULL == m_pPanel2)
	{
		return PERSISTID();
	}

	return m_pPanel2->GetID();
}

void Splitter::SetOrient(const char* value)
{
	Assert(value != NULL);

	if (stricmp(value, "Horizon") == 0)
	{
		if (m_nOrient == ORIENT_VERTICAL)
		{
			m_nOrient = ORIENT_HORIZON;

			CalcSuborRect();
		}
	}
	else
	{
		if (m_nOrient == ORIENT_HORIZON)
		{
			m_nOrient = ORIENT_VERTICAL;
			
			CalcSuborRect();
		}
	}
}

const char* Splitter::GetOrient() const
{
	if (m_nOrient == ORIENT_VERTICAL)
	{
		return "Vertical";
	}
	else
	{
		return "Horizon";
	}
}

void Splitter::SetMinPanelSize(int value)
{
	if (value < 0)
	{
		return;
	}
	
	m_nMinPanelSize = value;
}

int Splitter::GetMinPanelSize() const
{
	return m_nMinPanelSize;
}

void Splitter::SetPaddingWidth(int value)
{
	if (value < 0)
	{
		return;
	}
	
	m_nPaddingWidth = value;

	CalcSuborRect();
}

int Splitter::GetPaddingWidth() const
{
	return m_nPaddingWidth;
}

void Splitter::SetAdjusterWidth(int value)
{
	if (value < 0)
	{
		return;
	}
	
	if (value > (GetSplitSize() / 2))
	{
		return;
	}
	
	m_nAdjusterWidth = value;

	CalcSuborRect();
}

int Splitter::GetAdjusterWidth() const
{
	return m_nAdjusterWidth;
}

void Splitter::SetAdjusterFixed(bool value)
{
	m_bAdjusterFixed = value;
	
	m_pAdjuster->SetEnabled(!m_bAdjusterFixed);
}

bool Splitter::GetAdjusterFixed() const
{
	return m_bAdjusterFixed;
}

bool Splitter::ResetPanelSize()
{
	CalcSuborRect();

	return true;
}

int Splitter::GetSplitSize() const
{
	if (m_nOrient == ORIENT_VERTICAL)
	{
		return GetHeight();
	}
	else
	{
		return GetWidth();
	}
}

void Splitter::CalcSuborRect()
{
	if (m_nOrient == ORIENT_VERTICAL)
	{
		m_pPanel1->SetWidth(GetWidth() - GetPaddingWidth() * 2);
		m_pPanel2->SetWidth(GetWidth() - GetPaddingWidth() * 2);

		float percent1 = float(m_pPanel1->GetHeight()) 
			/ float(m_pPanel1->GetHeight() + m_pPanel2->GetHeight());
		float percent2 = float(m_pPanel2->GetHeight()) 
			/ float(m_pPanel1->GetHeight() + m_pPanel2->GetHeight());

		int subor_height = GetHeight() 
			- GetAdjusterWidth() - GetPaddingWidth() * 2;
		int height1 = int(subor_height* percent1);
		int height2 = subor_height - height1;

		m_pPanel1->SetTop(GetPaddingWidth());
		m_pPanel1->SetLeft(GetPaddingWidth());
		m_pPanel1->SetHeight(height1);

		m_pPanel2->SetTop(height1 + GetAdjusterWidth() + GetPaddingWidth());
		m_pPanel2->SetLeft(GetPaddingWidth());
		m_pPanel2->SetHeight(height2);
		
		m_pAdjuster->SetLeft(m_pPanel1->GetLeft());
		m_pAdjuster->SetTop(
			m_pPanel1->GetTop() + m_pPanel1->GetHeight());
		m_pAdjuster->SetWidth(m_pPanel1->GetWidth());
		m_pAdjuster->SetHeight(GetAdjusterWidth());
		m_pAdjuster->SetCursorName("WIN_SIZENS");
	}
	else
	{
		m_pPanel1->SetHeight(GetHeight() - GetPaddingWidth() * 2);
		m_pPanel2->SetHeight(GetHeight() - GetPaddingWidth() * 2);
		
		float percent1 = float(m_pPanel1->GetWidth()) 
			/ float(m_pPanel1->GetWidth() + m_pPanel2->GetWidth());
		float percent2 = float(m_pPanel2->GetWidth()) 
			/ float(m_pPanel1->GetWidth() + m_pPanel2->GetWidth());
		
		int subor_width = GetWidth() 
			- GetAdjusterWidth() - GetPaddingWidth() * 2;
		int width1 = int(subor_width* percent1);
		int width2 = subor_width - width1;
		
		m_pPanel1->SetTop(GetPaddingWidth());
		m_pPanel1->SetLeft(GetPaddingWidth());
		m_pPanel1->SetWidth(width1);
		
		m_pPanel2->SetTop(GetPaddingWidth());
		m_pPanel2->SetLeft(width1 + GetAdjusterWidth() + GetPaddingWidth());
		m_pPanel2->SetWidth(width2);
		
		m_pAdjuster->SetLeft(
			m_pPanel1->GetLeft() + m_pPanel1->GetWidth());
		m_pAdjuster->SetTop(m_pPanel1->GetTop());
		m_pAdjuster->SetWidth(GetAdjusterWidth());
		m_pAdjuster->SetHeight(m_pPanel1->GetHeight());
		m_pAdjuster->SetCursorName("WIN_SIZEWE");
	}
}

int Splitter::OnWidthChanged(int old)
{
	CalcSuborRect();
	
	return 1;
}

int Splitter::OnHeightChanged(int old)
{
	CalcSuborRect();
	
	return 1;
}

int Splitter::OnSuborEvent(IControl* subor, int event, 
						   const IVarList& args)
{
	if (!GetEnabled())
	{
		return 0;
	}
	
	/*
	if (event == CGuiConst::EVENT_CLICK)
	{
	}
	*/
	
	if (event == CGuiConst::EVENT_DRAG_MOVE)
	{
		if (subor == m_pAdjuster)
		{
			if (m_bAdjusterFixed)
			{
				return 0;
			}
			
			int x = args.IntVal(2);
			int y = args.IntVal(3);
			
			if (m_nOrient == ORIENT_VERTICAL)
			{
				int top = InnerGetAbsTop();
				int y1 = top + GetPaddingWidth();
				int y2 = top + GetHeight() - GetPaddingWidth();
				
				int total = y2 - y1 - GetAdjusterWidth();
				
				if (total < (GetMinPanelSize() * 2))
				{
					return 0;
				}
				
				if (y < y1)
				{
					y = y1;
				}
				
				if (y > y2)
				{
					y = y2;
				}
				
				int p1 = y - y1;
				
				if (p1 < GetMinPanelSize())
				{
					p1 = GetMinPanelSize();
				}
				
				if (p1 >= (total - GetMinPanelSize()))
				{
					p1 = total - GetMinPanelSize();
				}
				
				int p2 = total - p1;
				
				m_pPanel1->SetHeight(p1);
				m_pPanel2->SetHeight(p2);
				
				CalcSuborRect();
			}
			else
			{
				int left = InnerGetAbsLeft();
				int x1 = left + GetPaddingWidth();
				int x2 = left + GetWidth() - GetPaddingWidth();
				
				int total = x2 - x1 - GetAdjusterWidth();
				
				if (total < (GetMinPanelSize() * 2))
				{
					return 0;
				}
				
				if (x < x1)
				{
					x = x1;
				}
				
				if (x > x2)
				{
					x = x2;
				}
				
				int p1 = x - x1;
				
				if (p1 < GetMinPanelSize())
				{
					p1 = GetMinPanelSize();
				}
				
				if (p1 >= (total - GetMinPanelSize()))
				{
					p1 = total - GetMinPanelSize();
				}
				
				int p2 = total - p1;
				
				m_pPanel1->SetWidth(p1);
				m_pPanel2->SetWidth(p2);
				
				CalcSuborRect();
			}
			
			return 1;
		}
	}

	if (event == CGuiConst::EVENT_DRAG_LEAVE)
	{
		IControl* parent = GetParentControl();
		
		if (parent != NULL)
		{
			return parent->OnSuborEvent(this, event, args);
		}
	}

	return 0;
}

