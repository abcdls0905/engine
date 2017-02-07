//--------------------------------------------------------------------
// 文件名:		round_box.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年3月20日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "round_box.h"

/// \file round_box.cpp
/// \brief 圆形控件

/// entity: RoundBox
/// \brief 圆形控件实体
/// \see 继承自IControl
DECLARE_ENTITY(RoundBox, 0, IControl);

/// readonly: int ImageWidth
/// \brief 图片宽度
DECLARE_PROPERTY_GET(int, RoundBox, ImageWidth, GetImageWidth);
/// readonly: int ImageHeight
/// \brief 图片高度
DECLARE_PROPERTY_GET(int, RoundBox, ImageHeight, GetImageHeight);

/// property: string Image
/// \brief 贴图名
DECLARE_PROPERTY(const char*, RoundBox, Image, GetImage, SetImage);
/// property: double ZoomWidth
/// \brief 宽度缩放比例
DECLARE_PROPERTY(double, RoundBox, ZoomWidth, GetZoomWidth, SetZoomWidth);
/// property: double ZoomHeight
/// \brief 高度缩放比例
DECLARE_PROPERTY(double, RoundBox, ZoomHeight, GetZoomHeight, SetZoomHeight);
/// property: int CenterX
/// \brief 控件中心点在图片上的位置X
DECLARE_PROPERTY(int, RoundBox, CenterX, GetCenterX, SetCenterX);
/// property: int CenterY
/// \brief 控件中心点在图片上的位置Y
DECLARE_PROPERTY(int, RoundBox, CenterY, GetCenterY, SetCenterY);
/// property: int FanCount
/// \brief 扇面数量
DECLARE_PROPERTY(int, RoundBox, FanCount, GetFanCount, SetFanCount);
/// property: bool Transparent
/// \brief 是否判断透明
DECLARE_PROPERTY(bool, RoundBox, Transparent, GetTransparent, SetTransparent);

/// event: int on_click(object self, float offset_x, float offset_y)
/// \brief 鼠标点击图片的回调
/// \param self 本控件
/// \param offset_x X偏移
/// \param offset_y Y偏移
DECLARE_EVENT(on_click);

/// event: int on_mouse_move(object self, float offset_x, float offset_y)
/// \brief 鼠标在本控件内移动
/// \param self 本控件
/// \param offset_x X偏移
/// \param offset_y Y偏移
DECLARE_EVENT(on_mouse_move);

// RoundBox

RoundBox::RoundBox()
{
	m_dZoomWidth = 1.0;
	m_dZoomHeight = 1.0;
	m_nCenterX = 0;
	m_nCenterY = 0;
	m_nFanCount = 16;
	// 缺省为判断透明
	m_bTransparent = false;
	
	SetDesignSize(80, 80);
}

RoundBox::~RoundBox()
{
}

bool RoundBox::Init(const IVarList& args)
{
	if (!IControl::Init(args))
	{
		return false;
	}

	return true;
}

bool RoundBox::Shut()
{
	if (!m_TexId.IsNull())
	{
		GetPainter()->ReleaseGlyph(m_TexId);
		m_TexId = PERSISTID();
	}

	return IControl::Shut();
}

void RoundBox::PrepareDesignInfo(IDesignInfo* di)
{
	IControl::PrepareDesignInfo(di);
	
    CVar v(VTYPE_DOUBLE, 1.0);
	di->AddPropInfo("Image", IDesignInfo::TYPE_FILE, true, true);
	di->AddPropInfo("ZoomWidth", IDesignInfo::TYPE_DOUBLE, true, true,
		&v );
    v = CVar(VTYPE_DOUBLE, 1.0);
	di->AddPropInfo("ZoomHeight", IDesignInfo::TYPE_DOUBLE, true, true,
		&v );
	di->AddPropInfo("CenterX", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("CenterY", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("FanCount", IDesignInfo::TYPE_INT, true, true);
    v = CVar(VTYPE_BOOL, false);
	di->AddPropInfo("Transparent", IDesignInfo::TYPE_BOOL, true, true, 
		&v);
	
	di->AddEventInfo("on_click");
	di->AddEventInfo("on_mouse_move");
}

bool RoundBox::GetLoadFinish()
{
    if (GetLoadFinishValue())
    {
        return true;
    }

	if (!m_TexId.IsNull())
	{
		if (!GetPainter()->GetGlyphLoadComplete(m_TexId))
		{
			return false;
		}
	}
	
	return IControl::GetLoadFinish();
}

void RoundBox::Paint(float seconds)
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

	if (!m_TexId.IsNull())
	{
		// 图片的尺寸
		int tex_width = GetPainter()->GetGlyphWidth(m_TexId);
		int tex_height = GetPainter()->GetGlyphHeight(m_TexId);

		// 图片可见部分的尺寸
		int show_width = int(GetWidth() / m_dZoomWidth);
		int show_height = int(GetHeight() / m_dZoomHeight);

		int left = m_nCenterX - show_width / 2;
		int top = m_nCenterY - show_height / 2;

		if ((left + show_width) > tex_width)
		{
			left = tex_width - show_width;
		}

		if ((top + show_height) > tex_height)
		{
			top = tex_height - show_height;
		}

		if (left < 0)
		{
			left = 0;
		}

		if (top < 0)
		{
			top = 0;
		}

		int right = left + show_width;
		int bottom = top + show_height;

		if (right > tex_width)
		{
			right = tex_width;
		}

		if (bottom > tex_height)
		{
			bottom = tex_height;
		}

		GetPainter()->DrawGlyphRound(x1, y1, x2, y2, left, top, right, bottom,
			m_TexId, m_nFanCount);
	}
	else if (HasBackImage())
	{
		InnerGetBackImage().DrawRound(GetDrawMode(), x1, y1, x2, y2, 
			m_nFanCount);
	}
	else
	{
		GetPainter()->FillRound(x1, y1, x2, y2, GetBackColor(), m_nFanCount);
	}
}

void RoundBox::SetImage(const char* value)
{
	Assert(value != NULL);

	m_strImage = value;
	
	if (!m_TexId.IsNull())
	{
		GetPainter()->ReleaseGlyph(m_TexId);
	}
	
	m_TexId = GetPainter()->CreateGlyph(value, GetAsyncLoad());
}

const char* RoundBox::GetImage() const
{
	return m_strImage.c_str();
}

void RoundBox::SetZoomWidth(double value)
{
	if ((value < 0.1) || (value > 10.0))
	{
		return;
	}

	m_dZoomWidth = value;
}

double RoundBox::GetZoomWidth() const
{
	return m_dZoomWidth;
}

void RoundBox::SetZoomHeight(double value)
{
	if ((value < 0.1) || (value > 10.0))
	{
		return;
	}

	m_dZoomHeight = value;
}

double RoundBox::GetZoomHeight() const
{
	return m_dZoomHeight;
}

void RoundBox::SetCenterX(int value)
{
	m_nCenterX = value;
}

int RoundBox::GetCenterX() const
{
	return m_nCenterX;
}

void RoundBox::SetCenterY(int value)
{
	m_nCenterY = value;
}

int RoundBox::GetCenterY() const
{
	return m_nCenterY;
}

void RoundBox::SetFanCount(int value)
{
	if (value < 4)
	{
		return;
	}

	m_nFanCount = value;
}

int RoundBox::GetFanCount() const
{
	return m_nFanCount;
}

void RoundBox::SetTransparent(bool value)
{
	m_bTransparent = value;
}

bool RoundBox::GetTransparent() const
{
	return m_bTransparent;
}

int RoundBox::GetImageWidth() const
{
	if (m_TexId.IsNull())
	{
		return 0;
	}

	return GetPainter()->GetGlyphWidth(m_TexId);
}

int RoundBox::GetImageHeight() const
{
	if (m_TexId.IsNull())
	{
		return 0;
	}

	return GetPainter()->GetGlyphHeight(m_TexId);
}

bool RoundBox::TestTransparent(int x, int y)
{
	int x1 = InnerGetAbsLeft();
	int y1 = InnerGetAbsTop();
	int x2 = x1 + GetWidth();
	int y2 = y1 + GetHeight();

	if (!GuiUtil_PointInRound(x, y, x1, y1, x2, y2))
	{
		// 不在椭圆范围内
		return true;
	}

	if (m_bTransparent)
	{
		// 完全透明
		return true;
	}

    if (!GetTestTrans())
    {
        // 不进行透明测试，则认为不透明
        return false;
    }

	if (!HasBackImage())
	{
		return true;
	}

	int offset_x = x - x1;
	int offset_y = y - y1;

	int alpha;

	if (!InnerGetBackImage().GetAlpha(GetDrawMode(), 0, 0, 
		GetWidth(), GetHeight(), offset_x, offset_y, alpha))
	{
		return true;
	}

	return (alpha == 0);
}

int RoundBox::OnLButtonDown(int x, int y)
{ 
	if (!GetEnabled())
	{
		return 0;
	}
	
	/*
	int res = 1;

	if (TestTransparent(x, y))
	{
		res = 0;
	}
	
	if (HasBackImage() && (res == 1))
	{
		int x1 = InnerGetAbsLeft();
		int y1 = InnerGetAbsTop();
		
		int offset_x = x - x1;
		int offset_y = y - y1;

		// 点击图片事件
		GuiUtil_RunCallback(this, "on_click", 
			CVarList() << offset_x << offset_y);
	}
	
	return res;
	*/
	int x1 = InnerGetAbsLeft();
	int y1 = InnerGetAbsTop();

	int offset_x = x - x1;
	int offset_y = y - y1;

	// 点击图片事件
	return GuiUtil_RunCallback(this, "on_click", 
		CVarList() << offset_x << offset_y);
}

// 鼠标移动
int RoundBox::OnMouseMove(int x, int y)
{
    int x1 = InnerGetAbsLeft();
    int y1 = InnerGetAbsTop();

    int offset_x = x - x1;
    int offset_y = y - y1;

    // 点击图片事件
    return GuiUtil_RunCallback(this, "on_mouse_move", 
        CVarList() << offset_x << offset_y);
}

IControl* RoundBox::OnGetInWhat(int x, int y)
{
	if (GetDesignMode())
	{
		// 设计状态
		return this;
	}
	
	if (TestTransparent(x, y))
	{
		// 透明
		return NULL;
	}

	return this;
}

