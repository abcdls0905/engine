//--------------------------------------------------------------------
// �ļ���:		image_player.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��14��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "image_player.h"
#include "gui_const.h"
#include "gui_utils.h"
#include "gui.h"
#include "../visual/i_painter.h"

// CImagePlayer

CImagePlayer::CImagePlayer()
{
    m_bLoadFinish = false;
	m_pPainter = NULL;
	m_pGui = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nImageType = 0;
    m_strName = "";
}

CImagePlayer::~CImagePlayer()
{
}

void CImagePlayer::SetName(const char* value)
{
	Assert(value != NULL);
	
    m_bLoadFinish = false;
	m_strName = value;
}

const char* CImagePlayer::GetName() const
{
	return m_strName.c_str();
}

int CImagePlayer::GetWidth() const
{
	return m_nWidth;
}

int CImagePlayer::GetHeight() const
{
	return m_nHeight;
}

bool CImagePlayer::IsReady()
{
	if (NULL == m_pPainter)
	{
		return false;
	}

    bool ready = false;

	switch (m_nImageType)
	{
	case IMAGETYPE_GLYPH:
		ready = m_pPainter->GetGlyphReady(m_ResourceId);
        break;
	//case IMAGETYPE_TEXTURE:
	//	ready = m_pPainter->GetTextureReady(m_ResourceId);
    //  break;
	case IMAGETYPE_ANIMATION:
		return true;
	default:
		Assert(0);
		break;
	}

    if (ready && !m_bLoadFinish)
    {
        if (m_nImageType == IMAGETYPE_GLYPH)
        {
		    m_nWidth = m_pPainter->GetGlyphWidth(m_ResourceId);
		    m_nHeight = m_pPainter->GetGlyphHeight(m_ResourceId);
        }
        //else if (m_nImageType == IMAGETYPE_TEXTURE)
        //{
          //  m_nWidth = m_pPainter->GetTextureWidth(m_ResourceId);
          //  m_nHeight = m_pPainter->GetTextureHeight(m_ResourceId);
        //}

        m_bLoadFinish = true;
    }

	return ready;
}

bool CImagePlayer::IsLoadCompelte()
{
	if (NULL == m_pPainter)
	{
		// û�д���
		return true;
	}

    bool loaded = false;

	switch (m_nImageType)
	{
	case IMAGETYPE_GLYPH:
		loaded = m_pPainter->GetGlyphLoadComplete(m_ResourceId);
		break;
	//case IMAGETYPE_TEXTURE:
	//	loaded = m_pPainter->GetTextureLoadComplete(m_ResourceId);
	//	break;
	case IMAGETYPE_ANIMATION:
		return true;
	default:
		Assert(0);
		break;
	}

    if (loaded && !m_bLoadFinish)
    {
        if (m_nImageType == IMAGETYPE_GLYPH)
        {
		    m_nWidth = m_pPainter->GetGlyphWidth(m_ResourceId);
		    m_nHeight = m_pPainter->GetGlyphHeight(m_ResourceId);
        }
        //else if (m_nImageType == IMAGETYPE_TEXTURE)
        //{
          //  m_nWidth = m_pPainter->GetTextureWidth(m_ResourceId);
          //  m_nHeight = m_pPainter->GetTextureHeight(m_ResourceId);
        //}

        m_bLoadFinish = true;
    }

	return loaded;
}

bool CImagePlayer::Create(IPainter* pPainter, Gui* pGui, bool async_load)
{
	Assert(pPainter != NULL);
	Assert(pGui != NULL);
	
    m_pPainter = pPainter;
	m_pGui = pGui;

	//if (m_VertexCache.pData)
	//{
	//	delete m_VertexCache.pData;
	//	memset(&m_VertexCache, 0, sizeof(m_VertexCache));
    //    m_VertexCache.pData = NULL;
	//}

	if (m_strName.empty())
	{
		return false;
	}

//    // 1.����Ƕ����������贴��ֱ�ӷ���
  //  ImageAnimationManager* pAniMng = pGui->GetAnimationManager();
//
//	if (NULL == pAniMng)
//	{
//		return false;
//	}
//
  //  if (pAniMng->IsAnimation(m_strName.c_str()))
//    {
//        POINT pt = pAniMng->GetAnimationSize(m_strName.c_str());
//        m_nWidth = pt.x;
//        m_nHeight = pt.y;
//		m_nImageType = IMAGETYPE_ANIMATION;
//        m_bLoadFinish = true;
//        return true;
//    }

    // 2.���Ƕ�������ִ�����´�������
	if (!m_ResourceId.IsNull())
	{
		this->Release();
	}

//	bool is_tex = false;
//
//	if (strchr(m_strName.c_str(), ',') != NULL)
//	{
//		is_tex = true;
//	}
//	else
//	{
//		char buf[256];
//
//		CopyString(buf, sizeof(buf), m_strName.c_str());
//		
//		strlwr(buf);
//
//		if (strstr(buf, ".png") == NULL)
//		{
//			is_tex = true;
//		}
//	}

	//bool is_tex = (strchr(m_strName.c_str(), ',') != NULL)
	//	|| (strstr(m_strName.c_str(), ".png") == NULL);

//	if (is_tex)
//	{
//		char name[256];
//		const char* tex_name = m_strName.c_str();
//		const char* comma = strchr(tex_name, ',');
//
//		if (comma)
//		{
//			// ��ȡ��ͼ��
//			CopyString(name, sizeof(name), tex_name);
//
//			size_t pos = comma - tex_name;
//
//			if (pos < sizeof(name))
//			{
//				name[pos] = 0;
//			}
//
//			tex_name = name;
//		}
//		
//		// ��ͼ
//		m_ResourceId = pPainter->CreateTexture(tex_name, async_load);
//
//		if (m_ResourceId.IsNull())
//		{
//			return false;
//		}
//
//        if (pPainter->GetTextureReady(m_ResourceId))
//     {
//		    m_nWidth = pPainter->GetTextureWidth(m_ResourceId);
//            m_nHeight = pPainter->GetTextureHeight(m_ResourceId);
//            m_bLoadFinish = true;
//        }
//
//		m_nImageType = IMAGETYPE_TEXTURE;
//	}
	
	if (strchr(m_strName.c_str(), ',') != NULL)
	{
		char name[256];
		const char* tex_name = m_strName.c_str();
		const char* comma = strchr(tex_name, ',');

		if (comma)
		{
			// ��ȡ��ͼ��
			CopyString(name, sizeof(name), tex_name);

			size_t pos = comma - tex_name;

			if (pos < sizeof(name))
			{
				name[pos] = 0;
			}

			tex_name = name;
		}
		
		// ��ͼ
		m_ResourceId = pPainter->CreateGlyph(tex_name, async_load);
	}
	else
	{
		// �����Ľ���ͼƬ
		m_ResourceId = pPainter->CreateGlyph(m_strName.c_str(), async_load);
	}

	if (m_ResourceId.IsNull())
	{
		return false;
	}

    if (pPainter->GetGlyphReady(m_ResourceId))
    {
	    m_nWidth = pPainter->GetGlyphWidth(m_ResourceId);
	    m_nHeight = pPainter->GetGlyphHeight(m_ResourceId);
        m_bLoadFinish = true;
    }

	m_nImageType = IMAGETYPE_GLYPH;
	
	return true;
}

bool CImagePlayer::Release()
{
	if (NULL == m_pPainter)
	{
		return false;
	}
	
	if (m_ResourceId.IsNull())
	{
		return false;
	}

	//if (m_nImageType == IMAGETYPE_GLYPH)
	//{
		m_pPainter->ReleaseGlyph(m_ResourceId);
	//}
	//else
	//{
	//	m_pPainter->ReleaseTexture(m_ResourceId);
	//}
	
	m_ResourceId = PERSISTID();

	return true;
}

// ��ǰ������������
bool CImagePlayer::DrawParts(int mode, const rect_t& dest, const rect_t& src)
{
    if (NULL == m_pPainter)
	{
		return false;
	}

	if (m_ResourceId.IsNull())
    {
        return false;
    }

	int left = 0;
	int top = 0;
	int right = m_nWidth;
	int bottom = m_nHeight;

    right = left + src.right;
    bottom = top + src.bottom;
    left += src.left;
    top += src.top;

    switch (mode)
    {
    case CGuiConst::DRAWMODE_TILE:		// ƽ��
        {
			int w = dest.right - dest.left;
			int h = dest.bottom - dest.top;

			m_pPainter->DrawGlyph(dest.left, dest.top, dest.right, 
				dest.bottom, left, top, left + w, top + h, m_ResourceId);
        }
        break;
    case CGuiConst::DRAWMODE_CENTER:	// ����
        {
			int w = dest.right - dest.left;
			int h = dest.bottom - dest.top;

            left += (right - left - w) / 2;
            top += (bottom - top - h) / 2;

			m_pPainter->DrawGlyph(dest.left, dest.top, dest.right, 
				dest.bottom, left, top, left + w, top + h, m_ResourceId);
        }
        break;
    case CGuiConst::DRAWMODE_FITWINDOW:	// ��Ӧ���ڴ�С
        {
			m_pPainter->DrawGlyph(dest.left, dest.top, dest.right, 
				dest.bottom, left, top, right, bottom, m_ResourceId);
        }
        break;
    case CGuiConst::DRAWMODE_EXPAND:	// ͼƬ��չ
        {
            // �ָ������䴰��
			m_pPainter->DrawGlyphExpand(dest.left, dest.top, dest.right, 
				dest.bottom, left, top, right, bottom, m_ResourceId); //, NULL);
        }
        break;
    case CGuiConst::DRAWMODE_EXPANDH:	// ͼƬ��չ
        {
            // �ָ������䴰��
			m_pPainter->DrawGlyphExpandH(dest.left, dest.top, dest.right, 
				dest.bottom, left, top, right, bottom, m_ResourceId);
        }
        break;
    case CGuiConst::DRAWMODE_EXPANDV:	// ͼƬ��չ
        {
            // �ָ������䴰��
            m_pPainter->DrawGlyphExpandV(dest.left, dest.top, dest.right, 
				dest.bottom, left, top, right, bottom, m_ResourceId);
        }
        break;
    default:
        Assert(0);
        break;
    }

    return true;
}

bool CImagePlayer::Draw(int mode, int x1, int y1, int x2, int y2, 
	int blend_color)
{
//    if (m_nImageType == IMAGETYPE_ANIMATION)
//    {
//		ImageAnimationManager* pAniMng = m_pGui->GetAnimationManager();
//
//        if (NULL == pAniMng)
//        {
//            return false;
//        }
//
//		RECT rect = { x1, y1, x2, y2 };
//
//		pAniMng->DrawAnimation(m_strName.c_str(), x1, y1, rect, blend_color);
//
//		return true;
//        //return DrawAnimation(mode, x1, y1, x2, y2, blend_color);
//    }
//    else
//    {
        return DrawImage(mode, x1, y1, x2, y2, blend_color);
//    }
}

//bool CImagePlayer::DrawAnimation(int mode, int x1, int y1, int x2, int y2, int blend_color)
//{
//	ImageAnimationManager* pAniMng = m_pGui->GetAnimationManager();

//	RECT rect = { x1, y1, x2, y2 };

//	pAniMng->DrawAnimation(m_strName.c_str(), x1, y1, rect, blend_color);

//	return true;
//}

bool CImagePlayer::DrawImage(int mode, int x1, int y1, int x2, int y2, 
	int blend_color)
{
	if (NULL == m_pPainter)
	{
		return false;
	}

	if (m_ResourceId.IsNull())
	{
		return false;
	}

    if (m_nWidth == 0)
    {
        if (IsLoadCompelte())
        {
            //if (m_nImageType == IMAGETYPE_TEXTURE)
            //{
              //  m_nWidth = m_pPainter->GetTextureWidth(m_ResourceId);
		      //  m_nHeight = m_pPainter->GetTextureHeight(m_ResourceId);
            //}
            //else
            //{
		        m_nWidth = m_pPainter->GetGlyphWidth(m_ResourceId);
		        m_nHeight = m_pPainter->GetGlyphHeight(m_ResourceId);
            //}
        }
        else
        {
            return false;
        }
    }

    if (x2 - x1 == 0 && y2 - y1 == 0)
    {
        x2 = x1 + m_nWidth;
        y2 = y1 + m_nHeight;
    }

	int left = 0;
	int top = 0;
	int right = m_nWidth;
	int bottom = m_nHeight;

	switch (mode)
	{
	case CGuiConst::DRAWMODE_TILE:		// ƽ��
		{
			int w = x2 - x1;
			int h = y2 - y1;
			
			//if (m_nImageType == IMAGETYPE_TEXTURE)
			//{
			//	m_pPainter->DrawImage(x1, y1, x2, y2, left, top, left + w, 
			//		top + h, m_ResourceId);
			//}
			//else
			//{
				if (blend_color != 0xffffffff)
				{
					m_pPainter->DrawBWGlyph(x1, y1, x2, y2, left, top, 
						left + w, top + h, m_ResourceId, blend_color);
				}
				else
				{
                    
					m_pPainter->DrawGlyph(x1, y1, x2, y2, left, top, left + w, 
						top + h, m_ResourceId);
				}
			//}
		}
		break;
	case CGuiConst::DRAWMODE_CENTER:	// ����
		{
			int w = x2 - x1;
			int h = y2 - y1;

			left += (right - left - w) / 2;
			top += (bottom - top - h) / 2;
			
			//if (m_nImageType == IMAGETYPE_TEXTURE)
			//{
			//	m_pPainter->DrawImage(x1, y1, x2, y2, left, top, left + w, 
			//		top + h, m_ResourceId);
			//}
			//else
			//{
				if (blend_color != 0xffffffff)
				{
					m_pPainter->DrawBWGlyph(x1, y1, x2, y2, left, top, 
						left + w, top + h, m_ResourceId, blend_color);
				}
				else
				{
					m_pPainter->DrawGlyph(x1, y1, x2, y2, left, top, 
						left + w, top + h, m_ResourceId);
				}
			//}
		}
		break;
	case CGuiConst::DRAWMODE_FITWINDOW:	// ��Ӧ���ڴ�С
		{
			//if (m_nImageType == IMAGETYPE_TEXTURE)
			//{
			//	m_pPainter->DrawImage(x1, y1, x2, y2, left, top, right, 
			//		bottom, m_ResourceId);
			//}
			//else
			//{
				if (blend_color != 0xffffffff)
				{
					m_pPainter->DrawBWGlyph(x1, y1, x2, y2, left, top, right, 
						bottom, m_ResourceId, blend_color);
				}
				else
				{
					m_pPainter->DrawGlyph(x1, y1, x2, y2, left, top, right, 
						bottom, m_ResourceId);
				}
			//}
		}
		break;
	case CGuiConst::DRAWMODE_EXPAND:	// ͼƬ��չ
		{
			// �ָ������䴰��
			//if (m_nImageType == IMAGETYPE_TEXTURE)
			//{
			//	m_pPainter->DrawImageExpand(x1, y1, x2, y2, left, top, right, 
			//		bottom, m_ResourceId, &m_VertexCache);
			//}
			//else
			//{
				if (blend_color != 0xffffffff)
				{
					m_pPainter->DrawBWGlyph(x1, y1, x2, y2, left, top, right, 
						bottom, m_ResourceId, blend_color);
				}
				else
				{
					m_pPainter->DrawGlyphExpand(x1, y1, x2, y2, left, top, 
						right, bottom, m_ResourceId); //, &m_VertexCache);
				}
			//}
		}
		break;
	case CGuiConst::DRAWMODE_EXPANDH:	// ͼƬ��չ
		{
			// �ָ������䴰��
			//if (m_nImageType == IMAGETYPE_TEXTURE)
			//{
			//	m_pPainter->DrawImageExpandH(x1, y1, x2, y2, left, top, right, 
			//		bottom, m_ResourceId);
			//}
			//else
			//{
				m_pPainter->DrawGlyphExpandH(x1, y1, x2, y2, left, top, right, 
					bottom, m_ResourceId);
			//}
		}
		break;
	case CGuiConst::DRAWMODE_EXPANDV:	// ͼƬ��չ
		{
			// �ָ������䴰��
			//if (m_nImageType == IMAGETYPE_TEXTURE)
			//{
			//	m_pPainter->DrawImageExpandV(x1, y1, x2, y2, left, top, right, 
			//		bottom, m_ResourceId);
			//}
			//else
			//{
				m_pPainter->DrawGlyphExpandV(x1, y1, x2, y2, left, top, right, 
					bottom, m_ResourceId);
			//}
		}
		break;
	default:
		Assert(0);
		break;
	}
	
	return true;
}

// ������ͼ
bool CImagePlayer::DrawImageFan(int x1, int y1, int x2, int y2, float begin_angle, float end_angle)
{
    if (NULL == m_pPainter || m_ResourceId.IsNull())
    {
        return false;
    }

    if (m_nWidth == 0)
    {
        if (IsLoadCompelte())
        {
            m_nWidth = m_pPainter->GetGlyphWidth(m_ResourceId);
            m_nHeight = m_pPainter->GetGlyphHeight(m_ResourceId);
        }
        else
        {
            return false;
        }
    }

    if (x2 - x1 == 0 && y2 - y1 == 0)
    {
        x2 = x1 + m_nWidth;
        y2 = y1 + m_nHeight;
    }

    int left = 0;
    int top = 0;
    int right = m_nWidth;
    int bottom = m_nHeight;
    int w = x2 - x1;
    int h = y2 - y1;

    m_pPainter->DrawGlyphFan(x1, y1, x2, y2, left, top, left + w, 
        top + h, m_ResourceId, 100, begin_angle, end_angle);

    return true;
}

// ��ǰ��roundbox����
bool CImagePlayer::DrawRound(int mode, int x1, int y1, int x2, int y2, 
	int fan_num)
{
	if (NULL == m_pPainter)
	{
		return false;
	}
	
	if (m_ResourceId.IsNull())
	{
		return false;
	}
	
    if (m_nWidth == 0)
    {
        if (IsLoadCompelte())
        {
            //if (m_nImageType == IMAGETYPE_TEXTURE)
            //{
              //  m_nWidth = m_pPainter->GetTextureWidth(m_ResourceId);
		      //  m_nHeight = m_pPainter->GetTextureHeight(m_ResourceId);
            //}
            //else
            //{
		        m_nWidth = m_pPainter->GetGlyphWidth(m_ResourceId);
		        m_nHeight = m_pPainter->GetGlyphHeight(m_ResourceId);
            //}
        }
        else
        {
            return false;
        }
    }

	int left = 0;
	int top = 0;
	int right = m_nWidth;
	int bottom = m_nHeight;
	
	switch (mode)
	{
	case CGuiConst::DRAWMODE_TILE:		// ƽ��
		{
			int w = x2 - x1;
			int h = y2 - y1;
			
			//if (m_nImageType == IMAGETYPE_TEXTURE)
			//{
			//	m_pPainter->DrawImageRound(x1, y1, x2, y2, left, top, 
			//		left + w, top + h, m_ResourceId, fan_num);
			//}
			//else
			//{
				m_pPainter->DrawGlyphRound(x1, y1, x2, y2, left, top, 
					left + w, top + h, m_ResourceId, fan_num);
			//}
		}
		break;
	case CGuiConst::DRAWMODE_CENTER:	// ����
		{
			int w = x2 - x1;
			int h = y2 - y1;

			left += (right - left - w) / 2;
			top += (bottom - top - h) / 2;
			
			//if (m_nImageType == IMAGETYPE_TEXTURE)
			//{
			//	m_pPainter->DrawImageRound(x1, y1, x2, y2, left, top, 
			//		left + w, top + h, m_ResourceId, fan_num);
			//}
			//else
			//{
				m_pPainter->DrawGlyphRound(x1, y1, x2, y2, left, top, 
					left + w, top + h, m_ResourceId, fan_num);
			//}	
		}
		break;
	case CGuiConst::DRAWMODE_FITWINDOW:	// ��Ӧ���ڴ�С
		{
			//if (m_nImageType == IMAGETYPE_TEXTURE)
			//{
			//	m_pPainter->DrawImageRound(x1, y1, x2, y2, left, top, right, 
			//		bottom, m_ResourceId, fan_num);
			//}
			//else
			//{
				m_pPainter->DrawGlyphRound(x1, y1, x2, y2, left, top, right, 
					bottom, m_ResourceId, fan_num);
			//}
		}
		break;
	default:
		return false;
	}
	
	return true;
}

// ��ǰû�й�
bool CImagePlayer::DrawZoomAt(double zoom, int x_offs, int y_offs, int x1, 
	int y1, int x2, int y2)
{
	if (NULL == m_pPainter)
	{
		return false;
	}
	
	if (m_ResourceId.IsNull())
	{
		return false;
	}
	
    if (m_nWidth == 0)
    {
        if (IsLoadCompelte())
        {
            //if (m_nImageType == IMAGETYPE_TEXTURE)
            //{
              //  m_nWidth = m_pPainter->GetTextureWidth(m_ResourceId);
		      //  m_nHeight = m_pPainter->GetTextureHeight(m_ResourceId);
            //}
            //else
            //{
		        m_nWidth = m_pPainter->GetGlyphWidth(m_ResourceId);
		        m_nHeight = m_pPainter->GetGlyphHeight(m_ResourceId);
            //}
        }
        else
        {
            return false;
        }
    }

	int left = 0;
	int top = 0;
	int right = m_nWidth;
	int bottom = m_nHeight;

	// Ҫ��ʾ����ͼ�ߴ�
	int w = int(right - left) - x_offs;
	int h = int(bottom - top) - y_offs;
	
	// ���ź�Ĵ�С�������Ǳ߽�
	int x3 = x1 + int(w * zoom);
	int y3 = y1 + int(h * zoom);
	
	//if (m_nImageType == IMAGETYPE_TEXTURE)
	//{
	//	m_pPainter->DrawImage(x1, y1, x3, y3, left + x_offs, top + y_offs, 
	//		right, bottom, m_ResourceId);
	//}
	//else
	//{
		m_pPainter->DrawGlyph(x1, y1, x3, y3, left + x_offs, top + y_offs, 
			right, bottom, m_ResourceId);
	//}
	
	return true;
}

bool CImagePlayer::GetAlpha(int mode, int x1, int y1, int x2, int y2, int x, 
	int y, int& alpha)
{
	if (NULL == m_pPainter)
	{
		return false;
	}
	
	if (m_ResourceId.IsNull())
	{
		return false;
	}
	
	int left = 0;
	int top = 0;
	int right = m_nWidth;
	int bottom = m_nHeight;
	
	switch (mode)
	{
	case CGuiConst::DRAWMODE_TILE:		// ƽ��
		{
			int w = x2 - x1;
			int h = y2 - y1;
			
			right = left + w;
			bottom = top + h;
		}
		break;
	case CGuiConst::DRAWMODE_CENTER:	// ����
		{
			int w = x2 - x1;
			int h = y2 - y1;
			
			left += (right - left - w) / 2;
			top += (bottom - top - h) / 2;
			right = left + w;
			bottom = top + h;
		}
		break;
	case CGuiConst::DRAWMODE_FITWINDOW:	// ��Ӧ���ڴ�С
		break;
	default:
		return false;
	}
	
	if ((right <= left) || (bottom <= top))
	{
		return false;
	}
	
	// λ�ñ���
	float fx = float(x - x1) / float(x2 - x1);
	float fy = float(y - y1) / float(y2 - y1);
	// ��ͼλ��
	int tx = int(left + fx * float(right - left));
	int ty = int(top + fy * float(bottom - top));
	
	return m_pPainter->GetGlyphAlpha(m_ResourceId, tx, ty, alpha);
}

