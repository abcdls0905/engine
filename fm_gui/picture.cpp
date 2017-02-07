 //--------------------------------------------------------------------
// 文件名:		picture.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月22日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "picture.h"

/// \file picture.cpp
/// \brief 图片控件

/// entity: Picture
/// \brief 图片控件实体
/// \see 继承自IControl
DECLARE_ENTITY(Picture, 0, IControl);

/// readonly: int ImageWidth
/// \brief 图片宽度
DECLARE_PROPERTY_GET(int, Picture, ImageWidth, GetImageWidth);
/// readonly: int ImageHeight
/// \brief 图片高度
DECLARE_PROPERTY_GET(int, Picture, ImageHeight, GetImageHeight);
/// readonly: string ImageFormat
/// \brief 图片格式
DECLARE_PROPERTY_GET(const char*, Picture, ImageFormat, GetImageFormat);

/// property: string Image
/// \brief 贴图名
DECLARE_PROPERTY(const char*, Picture, Image, GetImage, SetImage);
/// property: double ZoomWidth
/// \brief 宽度缩放比例
DECLARE_PROPERTY(double, Picture, ZoomWidth, GetZoomWidth, SetZoomWidth);
/// property: double ZoomHeight
/// \brief 高度缩放比例
DECLARE_PROPERTY(double, Picture, ZoomHeight, GetZoomHeight, SetZoomHeight);

/// property: int CenterX
/// \brief 控件中心点在图片上的位置X
DECLARE_PROPERTY(int, Picture, CenterX, GetCenterX, SetCenterX);
/// property: int CenterY
/// \brief 控件中心点在图片上的位置Y
DECLARE_PROPERTY(int, Picture, CenterY, GetCenterY, SetCenterY);

/// property: bool Transparent
/// \brief 是否透明处理
DECLARE_PROPERTY(bool, Picture, Transparent, GetTransparent, SetTransparent);

/// event: int on_left_down(object self, int x, int y)
/// \brief 鼠标左键点击回调
/// \param self 本控件
/// \param x X偏移
/// \param y Y偏移
DECLARE_EVENT(on_left_down);


/// event: int on_left_up(object self, float offset_x, float offset_y)
/// \brief 左键弹起
/// \param self 本控件
/// \param new_x 新的X坐标
/// \param new_y 新的Y坐标
DECLARE_EVENT(on_left_up);

/// event: int on_right_down(object self, int x, int y)
/// \brief 鼠标右键点击回调
/// \param self 本控件
/// \param x X偏移
/// \param y Y偏移
DECLARE_EVENT(on_right_down);

/// event: int on_right_up(object self, float x, float y)
/// \brief 右键弹起
/// \param self 本控件
/// \param x X偏移
/// \param y Y偏移
DECLARE_EVENT(on_right_up);

/// event: int on_mouse_move(object self, float offset_x, float offset_y)
/// \brief 鼠标在本控件内移动
/// \param self 本控件
/// \param offset_x X偏移
/// \param offset_y Y偏移
DECLARE_EVENT(on_mouse_move);

/// event: int on_drag_move(object self, float offset_x, float offset_y)
/// \brief 拖动中的回调
/// \param self 本控件
/// \param new_x 新的X坐标
/// \param new_y 新的Y坐标
DECLARE_EVENT(on_drag_move);

/// event: int on_drag_leave(object self)
/// \brief 拖动中的回调
/// \param self 本控件
DECLARE_EVENT(on_drag_leave);

// Picture

Picture::Picture()
{
	m_dZoomWidth = 1.0;
	m_dZoomHeight = 1.0;
    m_nCenterX = -1;
    m_nCenterY = -1;
	m_nClickX = 0;
	m_nClickY = 0;
    // 缺省为判断透明
    m_bTransparent = false;

	SetDesignSize(50, 50);
}

Picture::~Picture()
{
}

bool Picture::Init(const IVarList& args)
{
	if (!IControl::Init(args))
	{
		return false;
	}
	
	return true;
}

bool Picture::Shut()
{
	if (!m_TexId.IsNull())
	{
		GetPainter()->ReleaseGlyph(m_TexId);
		m_TexId = PERSISTID();
	}
	
	return IControl::Shut();
}

void Picture::PrepareDesignInfo(IDesignInfo* di)
{
	IControl::PrepareDesignInfo(di);
	
	di->RemovePropInfo("Font");
	di->RemovePropInfo("Text");
	di->RemovePropInfo("DrawMode");
	di->RemovePropInfo("BackImage");
	CVar v(VTYPE_DOUBLE, 1.0);
	di->AddPropInfo("Image", IDesignInfo::TYPE_FILE, true, true);
	di->AddPropInfo("ZoomWidth", IDesignInfo::TYPE_DOUBLE, true, true,
		&v );
    v = CVar(VTYPE_DOUBLE, 1.0);
	di->AddPropInfo("ZoomHeight", IDesignInfo::TYPE_DOUBLE, true, true,
		&v );
    v = CVar(VTYPE_BOOL, false);
    di->AddPropInfo("CenterX", IDesignInfo::TYPE_INT, true, true);
    di->AddPropInfo("CenterY", IDesignInfo::TYPE_INT, true, true);
    di->AddPropInfo("Transparent", IDesignInfo::TYPE_BOOL, true, true, 
        &v);

    di->RemoveEventInfo("on_left_double_click");
	di->AddEventInfo("on_left_down");
	di->AddEventInfo("on_left_up");
    di->AddEventInfo("on_right_down");
    di->AddEventInfo("on_right_up");
    di->AddEventInfo("on_double_click");
    di->AddEventInfo("on_mouse_move");
	di->AddEventInfo("on_drag_move");
	di->AddEventInfo("on_drag_leave");
}

bool Picture::GetLoadFinish()
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

void Picture::Paint(float seconds)
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

		GetPainter()->DrawGlyph(x1, y1, x2, y2, left, top, right, bottom, 
			m_TexId);
	}
	
	if (!GetNoFrame())
	{
		GetPainter()->DrawRect(x1, y1, x2, y2, GetLineColor());
	}
}

void Picture::SetImage(const char* value)
{
	Assert(value != NULL);

	m_strImage = value;

	if (!m_TexId.IsNull())
	{
		GetPainter()->ReleaseGlyph(m_TexId);
		m_TexId = PERSISTID();
	}

	if (value[0] == 0)
	{
		return;
	}

	m_TexId = GetPainter()->CreateGlyph(value, GetAsyncLoad());

	if (GetPainter()->GetGlyphReady(m_TexId))
	{
		if (GetAutoSize())
		{
            double w = GetPainter()->GetGlyphWidth(m_TexId);
            double h = GetPainter()->GetGlyphHeight(m_TexId);

            SetWidth((int)w);
            SetHeight((int)h);
		}
		else
		{
			double w = GetPainter()->GetGlyphWidth(m_TexId);
			double h = GetPainter()->GetGlyphHeight(m_TexId);
			
			m_dZoomWidth = double(GetWidth()) / w;
			m_dZoomHeight = double(GetHeight()) / h;
		}
	}
}

const char* Picture::GetImage() const
{
	return m_strImage.c_str();
}

void Picture::SetZoomWidth(double value)
{
	if (GetAutoSize())
	{
		return;
	}
	
	m_dZoomWidth = value;
}

double Picture::GetZoomWidth() const
{
	return m_dZoomWidth;
}

void Picture::SetZoomHeight(double value)
{
	if (GetAutoSize())
	{
		return;
	}
	
	m_dZoomHeight = value;
}

double Picture::GetZoomHeight() const
{
	return m_dZoomHeight;
}

int Picture::GetImageWidth() const
{
	if (m_TexId.IsNull())
	{
		return 0;
	}

	return GetPainter()->GetGlyphWidth(m_TexId);
}

int Picture::GetImageHeight() const
{
	if (m_TexId.IsNull())
	{
		return 0;
	}

	return GetPainter()->GetGlyphHeight(m_TexId);
}

bool Picture::TestTransparent(int x, int y)
{
	int x1 = InnerGetAbsLeft();
	int y1 = InnerGetAbsTop();
	int x2 = x1 + GetWidth();
	int y2 = y1 + GetHeight();

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

const char* Picture::GetImageFormat()
{
	if (m_TexId.IsNull())
	{
		return "";
	}

//	unsigned int format = GetPainter()->GetGlyphFormat(m_TexId);
//	
//	if (format == DXGI_FORMAT_BC1_UNORM)
//	{
//		return "BC1";
//	}
//
//	if (format == DXGI_FORMAT_BC3_UNORM)
//	{
//		return "BC3";
//	}
//	
//	if (format == DXGI_FORMAT_B8G8R8A8_UNORM)
//	{
//		return "B8G8R8A8";
//	}
//	
//	if (format == DXGI_FORMAT_B8G8R8X8_UNORM)
//	{
//		return "B8G8R8X8";
//	}

	return "unknown";
}

void Picture::SetCenterX(int value)
{
    m_nCenterX = value;
}

int Picture::GetCenterX() const
{
    return m_nCenterX;
}

void Picture::SetCenterY(int value)
{
    m_nCenterY = value;
}

int Picture::GetCenterY() const
{
    return m_nCenterY;
}

void Picture::SetTransparent(bool value)
{
    m_bTransparent = value;
}

bool Picture::GetTransparent() const
{
    return m_bTransparent;
}

int Picture::OnWidthChanged(int old)
{
	if (m_TexId.IsNull())
	{
		return 0;
	}
	
	int tex_width = GetPainter()->GetGlyphWidth(m_TexId);

	if (0 == tex_width)
	{
		return 0;
	}
	
	m_dZoomWidth = double(GetWidth()) / double(tex_width);
	
	return 1;
}

int Picture::OnHeightChanged(int old)
{
	if (m_TexId.IsNull())
	{
		return 0;
	}

	int tex_height = GetPainter()->GetGlyphHeight(m_TexId);

	if (0 == tex_height)
	{
		return 0;
	}
	
	m_dZoomHeight = double(GetHeight())	/ double(tex_height);
	
	return 1;
}

int Picture::OnLButtonDown(int x, int y)
{
	if (!GetEnabled())
	{
		return 0;
	}

	m_nClickX = x - InnerGetAbsLeft();
	m_nClickY = y - InnerGetAbsTop();

	Assert( (m_nClickX >= 0) && (m_nClickX < GetWidth()) );
	Assert( (m_nClickY >= 0) && (m_nClickY < GetHeight()) );

	return GuiUtil_RunCallback(this, "on_left_down", 
		CVarList() << m_nClickX << m_nClickY);
}

int Picture::OnLButtonUp(int x, int y)
{
	if (!GetEnabled())
	{
		return 0;
	}

	int click_x = x - InnerGetAbsLeft();
	int click_y = y - InnerGetAbsTop();

	return GuiUtil_RunCallback(this, "on_left_up", 
		CVarList() << click_x << click_y);
}

int Picture::OnRButtonDown(int x, int y)
{
    if (!GetEnabled())
    {
        return 0;
    }

    int click_x = x - InnerGetAbsLeft();
    int click_y = y - InnerGetAbsTop();

    return GuiUtil_RunCallback(this, "on_right_down", 
        CVarList() << click_x << click_y);
}

int Picture::OnRButtonUp(int x, int y)
{
    if (!GetEnabled())
    {
        return 0;
    }

    int click_x = x - InnerGetAbsLeft();
    int click_y = y - InnerGetAbsTop();

    return GuiUtil_RunCallback(this, "on_right_up", 
        CVarList() << click_x << click_y);
}

int Picture::OnLButtonDoubleClick(int x, int y)
{
	if (!GetEnabled())
	{
		return 0;
	}

	int click_x = x - InnerGetAbsLeft();
	int click_y = y - InnerGetAbsTop();

	return GuiUtil_RunCallback(this, "on_double_click", 
		CVarList() << click_x << click_y);
}

// 鼠标移动
int Picture::OnMouseMove(int x, int y)
{
	if (!GetEnabled())
	{
		return 0;
	}

    int x1 = InnerGetAbsLeft();
    int y1 = InnerGetAbsTop();

    int offset_x = x - x1;
    int offset_y = y - y1;

    // 点击图片事件
    return GuiUtil_RunCallback(this, "on_mouse_move", 
        CVarList() << offset_x << offset_y);
}

int Picture::OnDragMove(int x, int y)
{
	int new_x = x - InnerGetAbsLeft();
	int new_y = y - InnerGetAbsTop();
	int delta_x = new_x - m_nClickX; 
	int delta_y = new_y - m_nClickY;

	return  GuiUtil_RunCallback(this, "on_drag_move", 
		CVarList() << delta_x << delta_y);
}

int Picture::OnDragLeave(int x, int y) 
{ 
	return GuiUtil_RunCallback(this, "on_drag_leave", CVarList());
}
