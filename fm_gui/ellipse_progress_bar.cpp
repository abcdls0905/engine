//--------------------------------------------------------------------
// 文件名:		EllipseProgressBar.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2010年11月5日
// 创建人:		单悦
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "ellipse_progress_bar.h"
#include "../public/inlines.h"
#include "../visual/vis_utils.h"

/// \file ProgBar.cpp
/// \brief 进度条

/// entity: EllipseProgressBar
/// \brief 进度条实体
/// \see 继承自IControl
DECLARE_ENTITY(EllipseProgressBar, 0, ProgressBar);

DECLARE_PROPERTY(int, EllipseProgressBar, BeginDelta, GetBeginDelta, SetBeginDelta);

DECLARE_PROPERTY(int, EllipseProgressBar, EndDelta, GetEndDelta, SetEndDelta);

/// property: string ProgressImage
/// \brief 显示图元名
DECLARE_PROPERTY(const char*, EllipseProgressBar, ProgressImage, GetProgressImage, 
SetProgressImage);

// EllipseProgressBar

EllipseProgressBar::EllipseProgressBar()
:m_nBeginDelta(0)
,m_nEndDelta(360)
{

}

EllipseProgressBar::~EllipseProgressBar()
{
}

bool EllipseProgressBar::Init(const IVarList& args)
{
	return ProgressBar::Init(args);
}

float EllipseProgressBar::GetDelta( )
{
	float delta = (float)(GetCurValue() - GetMinimum()) 
		/ (float)(GetMaximum() - GetMinimum());
	return delta;
}

void EllipseProgressBar::PrepareDesignInfo(IDesignInfo* di)
{
	ProgressBar::PrepareDesignInfo(di);
    di->RemovePropInfo("ProgressMode");
	CVar varDefault;
	di->AddPropInfo("BeginDelta", IDesignInfo::TYPE_INT, true, true ,&(varDefault.SetInt(0),varDefault));
	di->AddPropInfo("EndDelta", IDesignInfo::TYPE_INT, true, true ,&(varDefault.SetInt(360),varDefault));
}

bool EllipseProgressBar::Shut()
{
	if (!m_TexId.IsNull())
	{
		GetPainter()->ReleaseGlyph(m_TexId);
		m_TexId = PERSISTID();
	}
	return ProgressBar::Shut();
}

bool EllipseProgressBar::GetLoadFinish()
{
	return ProgressBar::GetLoadFinish();
}

void EllipseProgressBar::SetBeginDelta(int val)
{
	m_nBeginDelta = val;
}

int  EllipseProgressBar::GetBeginDelta() const
{
	return m_nBeginDelta;
}

void EllipseProgressBar::SetEndDelta(int val)
{
	m_nEndDelta = val;
}

int  EllipseProgressBar::GetEndDelta() const
{
	return m_nEndDelta;
}

void EllipseProgressBar::SetProgressImage(const char* value)
{
	ProgressBar::SetProgressImage(value);

	if (!m_TexId.IsNull())
	{
		GetPainter()->ReleaseGlyph(m_TexId);
	}

	m_TexId = GetPainter()->CreateGlyph(value, GetAsyncLoad());
}

void EllipseProgressBar::Paint(float seconds)
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

	if ((m_nMaximum > m_nMinimum) && (m_nValue >= m_nMinimum))
	{
		if (m_ProgressImage.IsReady())
		{
			GetPainter()->SetViewport(x1, y1, x2, y2);
			float delta = GetDelta();
			int w = m_ProgressImage.GetWidth();
			int h = m_ProgressImage.GetHeight();
			if (RIGHT_TO_LEFT==m_eProgressMode || LEFT_TO_RIGHT==m_eProgressMode) 
			{
				if ( RIGHT_TO_LEFT == m_eProgressMode)
				{
					delta = -delta;
				}

				if (!m_TexId.IsNull())
				{
					float pai = 3.1415926f;
					float begin_angle = 2.0f * pai * GetBeginDelta() / 360.0f;

					while ( begin_angle > 2 * pai )
					{
						begin_angle -= 2 * pai;
					}

					if ( begin_angle < 0 )
					{
						begin_angle = 2 * pai + begin_angle;
					}

					float end_angle = (2 * pai * delta 
						* (GetEndDelta() - GetBeginDelta())) / 360.f + begin_angle;

					while ((end_angle - begin_angle) > (2 * pai + 0.001f))
					{
						end_angle -= 2 * pai;
					}

			        GetPainter()->DrawGlyphFan(x1, y1, x1 + w, y1 + h, 0, 0, 
						m_ProgressImage.GetWidth(), m_ProgressImage.GetHeight(),
						m_TexId, 100, begin_angle, end_angle);
				}
			}
			else if (m_eProgressMode == BOTTOM_TO_TOP)
			{
				int y3 = y2 - int(float(y2 - y1) * delta);

				rect_t rdest = {x1, y3, x2, y2};
				rect_t rsrc = {0, (int)((1 - delta) * h), w, h};

				m_ProgressImage.DrawParts(GetDrawMode(), rdest, rsrc);
			}
			else if (m_eProgressMode == TOP_TO_BOTTOM)
			{
				int y3 = y1 + int(float(y2 - y1) * delta);

				rect_t rdest = {x1, y1, x2, y3};

				int s_h = (int)(delta * h);

				rect_t rsrc = {0, 0, w, s_h};

				m_ProgressImage.DrawParts(GetDrawMode(), rdest, rsrc);
			}

			GetPainter()->ResetViewport();
		}
	}

	GetPainter()->SetRotateTransform(NULL);
}

