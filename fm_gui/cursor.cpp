//--------------------------------------------------------------------
// 文件名:		cursor.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "cursor.h"
#include "../public/inlines.h"
#include "../public/core_log.h"
#include "../visual/i_painter.h"

/// \file cursor.cpp
/// \brief 鼠标外观

/// entity: Cursor
/// \brief 鼠标外观实体
DECLARE_ENTITY(Cursor, 0, IEntity);

/// readonly: bool IsWinCursor
/// \brief 是否操作系统内置的外观
DECLARE_PROPERTY_GET(bool, Cursor, IsWinCursor, GetIsWinCursor);

/// property: string Name
/// \brief 名字
DECLARE_PROPERTY(const char*, Cursor, Name, GetName, SetName);
/// property: string Image
/// \brief 贴图名称
DECLARE_PROPERTY(const char*, Cursor, Image, GetImage, SetImage);
/// property: int HotSpotX
/// \brief 焦点的X偏移
DECLARE_PROPERTY(int, Cursor, HotSpotX, GetHotSpotX, SetHotSpotX);
/// property: int HotSpotY
/// \brief 焦点的Y偏移
DECLARE_PROPERTY(int, Cursor, HotSpotY, GetHotSpotY, SetHotSpotY);

// Cursor

Cursor::Cursor()
{
	m_pPainter = NULL;
	//m_hWinCursor = NULL;
	//m_hOldCursor = NULL;
	m_nHash = 0;
	m_nHotSpotX = 0;
	m_nHotSpotY = 0;
	m_nState = 0;
	m_nStateImage = 0;
	m_fTimeCount = 0.0F;
}

Cursor::~Cursor()
{
	for (size_t i = 0; i < STATE_MAX; i++)
	{
		for (size_t k = 0; k < m_StateImages[i].size(); k++)
		{
			CORE_DELETE(m_StateImages[i][k]);
		}
	}
}

bool Cursor::Init(const IVarList& args)
{
	const char* painter_name = args.StringVal(0);
	
	m_pPainter = (IPainter*)GetCore()->GetInterface(painter_name);

	if (NULL == m_pPainter)
	{
		CORE_TRACE("(Cursor::Init)no painter");
		CORE_TRACE(painter_name);
		return false;
	}
	
	return true;
}

bool Cursor::Shut()
{
    Clear();

	SAFE_RELEASE(m_pPainter);

	return true;
}

// 清理，并不是删除
bool Cursor::Clear()
{
//    if (m_hWinCursor != NULL)
//    {
//        DestroyCursor(m_hWinCursor);
//        m_hWinCursor = NULL;
//    }

    if (!m_TexId.IsNull())
    {
		m_pPainter->ReleaseGlyph(m_TexId);
        m_TexId = PERSISTID();
    }

    for (size_t i = 0; i < STATE_MAX; i++)
    {
        for (size_t k = 0; k < m_StateImages[i].size(); k++)
        {
            m_pPainter->ReleaseGlyph(m_StateImages[i][k]->TexId);
			CORE_DELETE(m_StateImages[i][k]);
        }

        m_StateImages[i].clear();
    }

    return true;
}

void Cursor::SetName(const char* value)
{
	Assert(value != NULL);
	
	m_strName = value;
	m_nHash = GetHashValueCase(value);
}

const char* Cursor::GetName() const
{
	return m_strName.c_str();
}

unsigned int Cursor::GetHash() const
{
	return m_nHash;
}

void Cursor::SetImage(const char* value)
{
	Assert(value != NULL);

    if (m_pPainter == NULL)
    {
        return;
    }

	m_strImage = value;
	
	/*
	if (strcmp(value, "IDC_APPSTARTING") == 0)
	{
		m_hWinCursor = LoadCursor(NULL, IDC_APPSTARTING);
	}
	else if (strcmp(value, "IDC_ARROW") == 0)
	{
		m_hWinCursor = LoadCursor(NULL, IDC_ARROW);
	}
	else if (strcmp(value, "IDC_CROSS") == 0)
	{
		m_hWinCursor = LoadCursor(NULL, IDC_CROSS);
	}
	else if (strcmp(value, "IDC_HELP") == 0)
	{
		m_hWinCursor = LoadCursor(NULL, IDC_HELP);
	}
	else if (strcmp(value, "IDC_IBEAM") == 0)
	{
		m_hWinCursor = LoadCursor(NULL, IDC_IBEAM);
	}
	else if (strcmp(value, "IDC_NO") == 0)
	{
		m_hWinCursor = LoadCursor(NULL, IDC_NO);
	}
	else if (strcmp(value, "IDC_SIZEALL") == 0)
	{
		m_hWinCursor = LoadCursor(NULL, IDC_SIZEALL);
	}
	else if (strcmp(value, "IDC_SIZENESW") == 0)
	{
		m_hWinCursor = LoadCursor(NULL, IDC_SIZENESW);
	}
	else if (strcmp(value, "IDC_SIZENS") == 0)
	{
		m_hWinCursor = LoadCursor(NULL, IDC_SIZENS);
	}
	else if (strcmp(value, "IDC_SIZENWSE") == 0)
	{
		m_hWinCursor = LoadCursor(NULL, IDC_SIZENWSE);
	}
	else if (strcmp(value, "IDC_SIZEWE") == 0)
	{
		m_hWinCursor = LoadCursor(NULL, IDC_SIZEWE);
	}
	else if (strcmp(value, "IDC_UPARROW") == 0)
	{
		m_hWinCursor = LoadCursor(NULL, IDC_UPARROW);
	}
	else if (strcmp(value, "IDC_WAIT") == 0)
	{
		m_hWinCursor = LoadCursor(NULL, IDC_WAIT);
	}
	else if (strcmp(value, "IDC_HELP") == 0)
	{
		m_hWinCursor = LoadCursor(NULL, IDC_HELP);
	}
	else
	{
		// 尝试加载WINDOWS光标
		char fname[256];

		SafeSprintf(fname, sizeof(fname), "%s%s", 
			m_pPainter->GetDefaultPath(), value);
		
		m_hWinCursor = LoadCursorFromFile(fname);
		
		if (NULL == m_hWinCursor)
		{
			//m_TexId = m_pPainter->CreateCursor(value);

			//if (m_TexId.IsNull())
			//{
			//	CORE_TRACE("(Cursor::SetImage)create failed");
			//	CORE_TRACE(GetName());
			//	CORE_TRACE(GetImage());
			//}
		}
	}
	*/
}

const char* Cursor::GetImage() const
{
	return m_strImage.c_str();
}

bool Cursor::GetIsWinCursor() const
{
	return false;
	//return (m_hWinCursor != NULL);
}

void Cursor::SetHotSpotX(int value)
{
	if (size_t(value) >= 32)
	{
		return;
	}

	m_nHotSpotX = value;
}

int Cursor::GetHotSpotX() const
{
	return m_nHotSpotX;
}

void Cursor::SetHotSpotY(int value)
{
	if (size_t(value) >= 32)
	{
		return;
	}
	
	m_nHotSpotY = value;
}

int Cursor::GetHotSpotY() const
{
	return m_nHotSpotY;
}

int Cursor::GetWidth() const
{
	if (NULL == m_pPainter)
	{
		return 0;
	}
	
	return 32;
	//if (m_TexId.IsNull())
	//{
	//	return 0;
	//}
	
	//return m_pPainter->GetTextureWidth(m_TexId);
}

int Cursor::GetHeight() const
{
	if (NULL == m_pPainter)
	{
		return 0;
	}

	return 32;

	//if (m_TexId.IsNull())
	//{
    //  if (m_nState < STATE_MAX && m_nStateImage < (int)m_StateImages[m_nState].size())
    //    {
    //        PERSISTID texid = m_StateImages[m_nState][m_nStateImage].TexId;
    //        return m_pPainter->GetTextureHeight(texid);
    //    }
    //    else
    //    {
    //        return 0;
    //    }
	//}
	
	//return m_pPainter->GetTextureHeight(m_TexId);
}

bool Cursor::SetState(int value)
{
	if (size_t(value) > STATE_MAX)
	{
		return false;
	}

	if ((value == STATE_LEFTDOWN) && (m_StateImages[value].empty()))
	{
		return false;
	}
	
	if ((value == STATE_RIGHTDOWN) && (m_StateImages[value].empty()))
	{
		return false;
	}
	
	m_nState = value;
	Reset();
	Apply();

	return true;
}

int Cursor::GetState() const
{
	return m_nState;
}

bool Cursor::AddStateImage(int state, const char* image, float interval)
{
	Assert(image != NULL);

	if (size_t(state) >= STATE_MAX)
	{
		return false;
	}

	if (interval < 0.1F)
	{
		return false;
	}
	
	//PERSISTID tex_id = m_pPainter->CreateCursor(image);

	//if (tex_id.IsNull())
	//{
	//	return false;
	//}

	//image_info_t* data = CORE_NEW(image_info_t);

	//data->strName = image;
	//data->TexId = tex_id;
	//data->fInterval = interval;

	//m_StateImages[state].push_back(data);
	
	//return true;
	return false;
}

bool Cursor::Create()
{
	return true;
}

bool Cursor::IsReady() const
{
	return (!m_TexId.IsNull() || GetIsWinCursor());
}

void Cursor::Reset()
{
	m_nStateImage = 0;
	m_fTimeCount = 0.0F;
}

void Cursor::Flash(float time)
{
	if (GetIsWinCursor())
	{
		return;
	}
	
	if (!m_TexId.IsNull())
	{
		return;
	}

	size_t image_num = m_StateImages[m_nState].size();

	if (image_num < 2)
	{
		return;
	}

	if (size_t(m_nStateImage) >= image_num)
	{
		return;
	}
	
	float interval = m_StateImages[m_nState][m_nStateImage]->fInterval;
	
	m_fTimeCount += time;

	while (m_fTimeCount >= interval)
	{
		m_fTimeCount -= interval;

		if (m_fTimeCount < interval)
		{
			Apply();
		}
		
		m_nStateImage = (int)(size_t(m_nStateImage + 1) % image_num);
	}
}

bool Cursor::Apply()
{
//	if (GetIsWinCursor())
//	{
//		SetCursor(m_hWinCursor);
//		return true;
//	}

	return false;
//	Assert(m_pPainter != NULL);
//
//	PERSISTID tex_id;
//	
  //  if (!m_TexId.IsNull())
    //{
//        tex_id = m_TexId;
//    }
//    else
//    {
//        if (size_t(m_nStateImage) >= m_StateImages[m_nState].size())
//        {
//	        return false;
//        }
//		
//        tex_id = m_StateImages[m_nState][m_nStateImage]->TexId;
//    }
//
//    return m_pPainter->DrawCursor(tex_id, m_nHotSpotX, m_nHotSpotY);
}

bool Cursor::Show()
{
	if (GetIsWinCursor())
	{
		// 显示WINDOWS鼠标
		//ShowCursor(TRUE);
	}
	else
	{
        // 硬鼠标时才需设置
        //m_pPainter->ShowCursor(true);
	}
	
	return true;
}

bool Cursor::Hide()
{
	if (GetIsWinCursor())
	{
		//ShowCursor(FALSE);
	}
	else
	{
		//m_pPainter->ShowCursor(false);
	}
	
	return true;
}

/*
bool Cursor::Draw(int x, int y)
{
    Assert(m_pPainter != NULL);

    // windows鼠标就不用画了
    if (GetIsWinCursor())
    {
        m_hOldCursor = SetCursor(m_hWinCursor);

        return (m_hOldCursor != NULL);
    }

    // 是普通贴图的情况
    PERSISTID tex_id;

    if (!m_TexId.IsNull())
    {
        tex_id = m_TexId;
    }
    else
    {
        if (size_t(m_nStateImage) >= m_StateImages[m_nState].size())
        {
            return false;
        }

        tex_id = m_StateImages[m_nState][m_nStateImage].TexId;
    }

    int nHeight = m_pPainter->GetTextureHeight(tex_id);
    int nWidth = m_pPainter->GetTextureWidth(tex_id);

    return m_pPainter->DrawImage(x, y, x + nWidth, y + nHeight, 0, 0, 
		nWidth, nHeight, tex_id);
}
*/

