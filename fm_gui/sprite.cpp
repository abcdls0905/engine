//--------------------------------------------------------------------
// 文件名:		Sprite.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月14日
// 创建人:		lixj
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "sprite.h"
#include "i_container.h"
#include "mark_string.h"
#include "../public/core_log.h"
#include "spritemanager.h"

#ifdef WIN32
#pragma warning(disable : 4101)
#pragma warning(disable : 4018)
#endif

/// \file Sprite.cpp
/// \brief 文本或图片标签控件

/// entity: Sprite
/// \brief 文本或图片标签控件实体
/// \see 继承自IControl
DECLARE_ENTITY(Sprite, 0, IControl);

DECLARE_PROPERTY(bool,Sprite ,bPaint,GetPaint,SetPaint)

#if 0
DECLARE_METHOD_1(bool,Sprite ,SetSpriteTop,int)

DECLARE_METHOD_1(bool,Sprite ,SetSpriteLeft,int)
#endif

#ifdef WIN32
#pragma warning(disable : 4244 )
#endif

// Sprite

Sprite::Sprite()
{
	// 缺省为判断透明
	m_bTransparent = true;
	
	m_bIsPaint = false;

	SetDesignSize(80, 16);

	SetCanSelect(false);

	SetNoFrame(true);

    m_pSpriteManager = NULL;
    m_sprite_index = -1;
}

Sprite::~Sprite()
{
	ClearAllPool();
}

bool Sprite::Init(const IVarList& args)
{
	if (!IControl::Init(args))
	{
		return false;
	}

	GetCore()->AddExecute(this);
	return true;
}

bool Sprite::Shut()
{
    if (m_pSpriteManager)
    {
        m_pSpriteManager->OnSpriteDestroy(m_sprite_index);
    }

	ClearAllPool();
	
	return IControl::Shut();
}

bool Sprite::GetPaint()
{
	return m_bIsPaint;
}

void Sprite::Execute(float seconds)
{
	if(!m_bIsPaint)
		return;

	float fS = seconds;
	m_dTime += seconds;

	RefreshSprite(fS);
}
void Sprite::SetPaint(bool bPaint)
{
	m_bIsPaint = bPaint;
	SetDTime(0.0f);

    if (!bPaint)
    {
        if (m_pSpriteManager)
        {
            m_pSpriteManager->OnSpriteShowEnd(m_sprite_index/*, m_pSingleType->singleton*/);
        }
    }
};

void Sprite::PrepareDesignInfo(IDesignInfo* di)
{
	IControl::PrepareDesignInfo(di);

}

void Sprite::Paint(float seconds)
{
	if(!m_bIsPaint)
		return;

    RefreshInnerAbsPos();

	int i = 0;
	int x1 = GetLeft() + m_lChangePosx;
	int y1 = GetTop() + m_lChangePosy;
	int x2 = x1 + m_pSingleType->iBackImageW;
	int y2 = y1 + m_pSingleType->iBackImageH;

	long lLeft = x1 + m_pSingleType->FontOffsetX + m_lNowWidth;
	long lTop = y1 + m_pSingleType->FontOffsetY + m_lNowHeight / 2;
	long lRight = m_lNowWidth;
	long lBottom = m_lNowHeight;

	float fRatio = ((float)(m_pSingleType->OffsetX)/(float)(m_pTrackMode->m_fBackW));
	int w = x2 + m_lNowWidth - x1 + m_ImageSingle.size() * (lRight + m_lNowWidth * fRatio);

	int old_alpha = GetPainter()->GetBlendAlpha();
	GetPainter()->SetBlendAlpha(GetAlpha());

	int image_offset_w = (x2 + m_lNowWidth - x1) * 0.5;
	//先将底图绘制上去
	if (m_BackImageEx.IsReady())
	{
		m_BackImageEx.Draw(CGuiConst::DRAWMODE_FITWINDOW, x1 + image_offset_w,\
			y1, x2 + m_lNowWidth + image_offset_w, y2 + m_lNowHeight);
	}

	//开始绘制图片
	//std::vector<CImagePlayer>::iterator it = m_ImageSingle.begin();
	size_t k = 0;

	lLeft += image_offset_w;

	//while (it != m_ImageSingle.end())
	while (k < m_ImageSingle.size())
	{
		CImagePlayer* pImagePlayer = m_ImageSingle[k];
		
		if (pImagePlayer->IsReady())
		{
			//一次绘制每个字体
			pImagePlayer->Draw(CGuiConst::DRAWMODE_FITWINDOW, lLeft, lTop,
				lLeft + lRight, lTop + lBottom, GetBlendColor());
			lLeft = lLeft + lRight + m_lNowWidth * fRatio;
		}
		
		//it++;
		++k;
	}

	GetGui()->SetFont(m_pTrackMode->m_StrFont.c_str(), GetShadowColor());

	GetPainter()->WriteTextCenter(x1, y1, x2, y2, GetForeColor(), GetText());
	GetPainter()->SetBlendAlpha(old_alpha);
}

void Sprite::SetTransparent(bool value)
{
	m_bTransparent = value;
}

bool Sprite::GetTransparent() const
{
	return m_bTransparent;
}

void Sprite::SetDTime(float dTime)
{
	m_dTime = dTime;
}

//刷新精灵的信息
void Sprite::RefreshSprite(float fSeconds)
{
	float fmovetime , fstatictime = 0.0f;
	char buf[128];

	if(m_pTrackMode->m_bIsStop)
	{
		fmovetime = (m_pTrackMode->m_fStaticTime > m_dTime)?(0.0f):
			(m_dTime - m_pTrackMode->m_fStaticTime);

		fstatictime = (m_pTrackMode->m_fStaticTime > m_dTime) ?(m_dTime ):
			(m_pTrackMode->m_fStaticTime);
	}
	else
	{
		fstatictime = (m_pTrackMode->m_fMoveTime > m_dTime)?(0.0f):
			(m_dTime - m_pTrackMode->m_fMoveTime);

		fmovetime = (m_pTrackMode->m_fMoveTime > m_dTime) ?(m_dTime ):
			(m_pTrackMode->m_fMoveTime);

	}

	if (m_dTime >=(m_pTrackMode->m_fMoveTime + m_pTrackMode->m_fStaticTime))
	{
		SetPaint(false);
		IEntity* pEntity = GetCore()->GetEntity(GetParentFormID());

		if(!pEntity)
			return;

		IContainer*	pContainer = (IContainer*)pEntity;
		pContainer->RemoveChild(this);
		return;
	}
	
	m_lChangePosx = m_pTrackMode->GetPostionX(m_dTime,
		fSeconds, fmovetime, fstatictime);

	m_lChangePosy = m_pTrackMode->GetPostionY(m_dTime, 
		fSeconds, fmovetime, fstatictime);

	m_lNowHeight = m_pTrackMode->GetScaleH(m_dTime, fSeconds,
		fmovetime, fstatictime);

	m_lNowWidth = m_pTrackMode->GetScaleW(m_dTime, fSeconds,
		fmovetime, fstatictime);

	SetBlendColor(m_pTrackMode->GetBlendColor(m_dTime, fSeconds,
		fmovetime, fstatictime));
}

bool Sprite::TestTransparent(int x, int y)
{
    if (m_bTransparent)
    {
        // 透明的话就全透，不用考虑任何元素
        return true;
    }

    // 不全透的情况判断是否要进行透明测试
	if (!GetTestTrans())
	{
		// 不进行透明测试，则完全不透明
		return false;
	}
	
    // 进行透明测试
	if (!HasBackImage())
	{
		return true;
	}
	
	int x1 = InnerGetAbsLeft();
	int y1 = InnerGetAbsTop();
	int x2 = x1 + GetWidth();
	int y2 = y1 + GetHeight();
	
    // 获取相对于控件左上的未放缩的偏移
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

IControl* Sprite::OnGetInWhat(int x, int y)
{
	return this;
}

void Sprite::ClearAllPool()
{
	//清空当前的池
	for (size_t i = 0; i < m_ImageSingle.size(); ++i)
	{
		CORE_DELETE(m_ImageSingle[i]);
	}

	m_ImageSingle.clear();
}

//设置精灵的位置
bool Sprite::SetSpriteTop(int val)
{
	long posy = (!m_pSingleType->bIsScreenPos)?(val):
		(GetGui()->GetHeight()*m_pTrackMode->m_fBackY);
	SetTop(posy);
	return true;
}

//设置精灵的位置
bool Sprite::SetSpriteLeft(int val)
{
	long posx = (!m_pSingleType->bIsScreenPos) ? (val) :
		(GetGui()->GetWidth()*m_pTrackMode->m_fBackX);
	SetLeft(posx);
	return true;
}

//根据设置的文字创建文字图片
bool Sprite::CreateFontImage()
{
	int size = m_ImageSingle.size();

	for (int i = 0; i < size; i++)
	{
		CImagePlayer* pImagePlayer = m_ImageSingle[i];

		pImagePlayer->SetName("");
		pImagePlayer->Release();
	}

	m_BackImageEx.Release();
	m_BackImageEx.SetName(m_pSingleType->StrBackImage.c_str());
	m_BackImageEx.Create(GetPainter(),GetGui(),GetAsyncLoad());

	std::string StrText = GuiUtil_Utf16ToAnsi(GetImageText()).c_str();
	
	size = StrText.size();

	for (int i = 0; i < size; i++)
	{
		CImagePlayer* pImagePlayer = GetImagePlayer(i);

		if (!pImagePlayer)
		{
			return false;
		}

		char ca = StrText[i];
		const char* name = m_pImageInfo->FindName(ca);
		//IMAGEINFO::const_iterator it = m_pImageInfo->find(ca);

		//if (it == m_pImageInfo->end())
		if (NULL == name)
		{
			continue;
		}

		//pImagePlayer->SetName((it->second).c_str());
		pImagePlayer->SetName(name);
		pImagePlayer->Create(GetPainter(), GetGui(), GetAsyncLoad());
	}

	return true;
}

CImagePlayer* Sprite::GetImagePlayer(unsigned int index)
{
	if (index > 10)
	{
		return NULL;
	}

	if (index >= m_ImageSingle.size())
	{
		CImagePlayer* pImagePlayer = CORE_NEW(CImagePlayer);

		m_ImageSingle.push_back(pImagePlayer);

		return pImagePlayer;
	}

	return m_ImageSingle[index];
}

void Sprite::SetImageText(const wchar_t* value)
{
	m_ImageText = value;
	CreateFontImage();
}

const wchar_t* Sprite::GetImageText() const
{
	return m_ImageText.c_str();
}

bool Sprite::InitParam(SingleType* pParam1, Tracklogic* pParam2, 
	CImageInfo* pParam3, const char* pImageText,const char* pText,
	long posx, long posy)
{
	m_pTrackMode = pParam2;
	m_pSingleType = pParam1;
	m_pImageInfo = pParam3;

	unsigned int Color = 0xff000000 | (m_pTrackMode->m_dStartColor);

	SetPaint(true);
	SetForeColor(Color);
	SetImageText(GuiUtil_AnsiToUtf16(pImageText).c_str());
	SetText(GuiUtil_AnsiToUtf16(pText).c_str());
	SetSpriteLeft(posx);
	SetSpriteTop(posy);	
	
	return true;
}

// 设置附加信息
bool Sprite::SetParentInfo(SpriteManager* pMng, int sprite_index)
{
    m_pSpriteManager = pMng;
    m_sprite_index = sprite_index;
    return true;
}

//根据流失的时间算出具体的颜色
const unsigned int Tracklogic::GetBlendColor(double dtime,float dElapseTime,
									   float fmovetime, float fstatictime) const
{
	float fTotalTime = m_fMoveTime + m_fStaticTime;

	float fValue = 1.0f;
	
	if (0 < (fTotalTime - m_fColorTime))
	{
		float fBlendTime = 0.0f;

		if ((m_fColorTime < dtime) && (dtime < fTotalTime))
		{
			fBlendTime = fTotalTime - dtime;
		}
		
		fValue = fBlendTime / (fTotalTime - m_fColorTime);
	}

	long dAlpha = (m_dStartColor >> 24) - (m_dEndColor >> 24);
	dAlpha = (float)dAlpha * fValue;
	dAlpha = dAlpha + (m_dStartColor >> 24);

	long dRSColor = (m_dStartColor & 0x00ff0000) >> 16;
	long dREColor = (m_dEndColor & 0x00ff0000) >> 16;
	long dRColor = dRSColor + (dRSColor - dREColor) * fValue;
	dRColor = dRColor & 0xff;
	
	long dGSColor = (m_dStartColor & 0x0000ff00) >> 8;
	long dGEColor = (m_dEndColor & 0x0000ff00) >> 8;
	long dGColor = dGSColor + (dGSColor - dGEColor) * fValue;
	dGColor = dGColor & 0xff;
	
	long dBSColor = m_dStartColor & 0x000000ff;
	long dBEColor = m_dEndColor & 0x000000ff;
	long dBColor = dBSColor + (dBSColor - dBEColor) * fValue;
	dBColor = dBColor & 0xff;

	long dColor = (dRColor << 16) | (dGColor << 8) | dBColor;
	long dBlendColor = dColor | (dAlpha << 24);

	return dBlendColor;
}

//根据流失的时间算出具体的Y值
const float Tracklogic::GetPostionY(double dtime,float dElapseTime,
							  float fmovetime, float fstatictime) const
{
	float top = 0;

	float as = m_fMoveSpeedAY*fmovetime*fmovetime;

	top = top+m_fMoveSpeedY*fmovetime+as;

	return top;
}
//根据流失的时间算出具体的X值
const float Tracklogic::GetPostionX(double dtime,float dElapseTime,
							  float fmovetime, float fstatictime) const
{
	float left = 0;
	float as = m_fMoveSpeedAX * fmovetime * fmovetime;
	left = left+m_fMoveSpeedX * fmovetime + as;
	return left;
}
//根据流失的时间算出具体的缩放值
const float Tracklogic::GetScaleW(double dtime,float dElapseTime, 
							float fmovetime, float fstatictime) const
{
	float lScale = this->m_fBackW + this->m_fMoveScaleSpeedW * fmovetime
		+ this->m_fStaticScaleSpeedW*fstatictime;

	return lScale;
}
//根据流失的时间算出具体的缩放值
const float Tracklogic::GetScaleH(double dtime,float dElapseTime,
							float fmovetime ,float fstatictime) const
{
	float lScale = this->m_fBackH + this->m_fMoveScaleSpeedH
		* fmovetime + this->m_fStaticScaleSpeedH * fstatictime;

	return lScale;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/

