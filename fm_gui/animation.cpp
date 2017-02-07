//--------------------------------------------------------------------
// 文件名:		Animation.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年10月23日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "animation.h"
#include "imageanimation_manager.h"

/// entity: Animation
/// \brief 图片动画控件
DECLARE_ENTITY(Animation, 0, IControl);

/// property: bool IsAutoClip
/// \brief 设置是否进行超出裁剪
DECLARE_PROPERTY(bool, Animation, IsAutoClip, GetAutoClip, SetAutoClip);
/// property: int OffsetX
/// \brief 绘制X偏移
DECLARE_PROPERTY(int, Animation, OffsetX, GetOffsetX, SetOffsetX);
/// property: int OffsetY
/// \brief 绘制Y偏移
DECLARE_PROPERTY(int, Animation, OffsetY, GetOffsetY, SetOffsetY);
/// property: string AnimationImage
/// \brief 动画图源
DECLARE_PROPERTY(result_string, Animation, AnimationImage, GetAnimationImage, 
	SetAnimationImage);
///// property: bool Transparent
///// \brief 是否判断透明
//DECLARE_PROPERTY(bool, Animation, Transparent, GetTransparent, SetTransparent);

/// property: bool Independent
/// \brief 是否是独立控制的动画
DECLARE_PROPERTY(bool, Animation, Independent, GetIndependent, SetIndependent);

/// property: bool Loop
/// \brief 仅在Independent为true时Loop为false才有效
DECLARE_PROPERTY(bool, Animation, Loop, GetLoop, SetLoop);

/// property: bool Athwart
/// \brief 逆向播放, 如果想重头开始，则Athwart值变化后需调一下Stop
DECLARE_PROPERTY(bool, Animation, Athwart, GetAthwart, SetAthwart);

/// property: int CurTime
DECLARE_PROPERTY_GET(int, Animation, CurTime, GetCurTime);

/// property: int PlayMode
/// \brief 当前播放状态,0表示正在播放，1表示暂停，2表示结束
DECLARE_PROPERTY(int, Animation, PlayMode, GetPlayMode, SetPlayMode);

#if 0
DECLARE_METHOD_0(bool, Animation, Play);
DECLARE_METHOD_0(bool, Animation, Pause);
DECLARE_METHOD_0(bool, Animation, Stop);
DECLARE_METHOD_1(bool, Animation, PlayPauseAt, int);
#endif
/// event: int on_animation_start(object self, string name)
/// \brief 动画开始播放
/// param self 控件本身
/// param name 动画名
DECLARE_EVENT(on_animation_start)

/// event:int on_animation_loop(object self, string name)
/// \brief 一遍动画结束,循环动画再次开始播放
/// param self 控件本身
/// param name 动画名
DECLARE_EVENT(on_animation_loop)

// 动画结束，不循环动画，停止播放
/// \brief 动画开始播放
/// param self 控件本身
/// param name 动画名
DECLARE_EVENT(on_animation_end)

Animation::Animation()
{
    m_IsAutoClip = false;
    m_nOffsetX = 0;
    m_nOffsetY = 0;
    m_szAnimationName = "";
//    m_bTransparent = true;
// 	SetTransparentDefault(true);
//     m_bIndependent = true;
    m_ePlayMode = MODE_STOP;
    m_nPauseAtTime = -1;
    m_control.athwart = false;
}

Animation::~Animation()
{
}

void Animation::SetIndependent(bool value)
{
    if (m_bIndependent == value)
    {
        return;
    }

    if (value)
    {
        GetCore()->AddExecute(this);
    }
    else
    {
        GetCore()->RemoveExecute(this);
    }

    m_bIndependent = value;

};

void Animation::SetLoop(bool value)
{
    m_control.loop = value;
}

void Animation::SetAthwart(bool value)
{
    m_control.athwart = value;
}

void Animation::SetPlayMode(int value)
{
    m_ePlayMode = (ePlayerMode)value;

    if (m_ePlayMode == MODE_PLAY)
    {
        Play();
    }
    else if (m_ePlayMode == MODE_PAUSE)
    {
        Pause();
    }
    else if (m_ePlayMode == MODE_STOP)
    {
        Stop();
    }
}

int Animation::GetPlayMode()
{
    return (int)m_ePlayMode;
}

int Animation::GetCurTime()
{
    return m_control.lTimeCount;
}

bool Animation::Init(const IVarList& args)
{
    if (!IControl::Init(args))
    {
        return false;
    }

    SetEnabled(false);
	//SetTransparentDefault(true);

    if (m_bIndependent)
    {
        GetCore()->AddExecute(this);
    }

    return true;
}

bool Animation::Shut()
{
    return IControl::Shut();
}

void Animation::Execute(float seconds)
{
    // 不在播放状态则返回
    if (m_ePlayMode != MODE_PLAY)
    {
        return;
    }

    // 正在播放则进行动画更新
    ImageAnimationManager* pAniManager = InnerGetAnimation();

    if (pAniManager)
    {
        long Delta_Time = (long)(seconds * 1000);

        if (m_nPauseAtTime > 0 && m_nPauseAtTime >= m_control.lTimeCount && 
            m_nPauseAtTime <= m_control.lTimeCount + Delta_Time)
        {
            Pause();

            // 产生动画开始事件
            GuiUtil_RunCallback(this, "on_animation_pause", 
				CVarList() << m_szAnimationName);
        }

        pAniManager->UpdateAnimationByName(m_szAnimationName.c_str(), 
			m_control, Delta_Time);

        if (m_control.cevent == ImageAnimationManager::EVENT_PLAY_START)
        {
            // 开始
            // 产生动画开始事件
            GuiUtil_RunCallback(this, "on_animation_start", 
				CVarList() << m_szAnimationName);
        }
        else if (m_control.cevent == ImageAnimationManager::EVENT_PLAY_LOOP)
        {
            // 重新开始新的循环
            GuiUtil_RunCallback(this, "on_animation_loop", 
				CVarList() << m_szAnimationName);
        }
        else if (m_control.cevent == ImageAnimationManager::EVENT_PLAY_END)
        {
            // 动画结束
            m_ePlayMode = MODE_STOP;

            GuiUtil_RunCallback(this, "on_animation_end", 
				CVarList() << m_szAnimationName);
        }
    }
}

void Animation::PrepareDesignInfo(IDesignInfo* di)
{
	CVar varDefault;
    di->AddPropInfo("Left", IDesignInfo::TYPE_INT, true, true);
    di->AddPropInfo("Top", IDesignInfo::TYPE_INT, true, true);
    di->AddPropInfo("Width", IDesignInfo::TYPE_INT, true, true);
    di->AddPropInfo("Height", IDesignInfo::TYPE_INT, true, true);

    di->AddPropInfo("VAnchor", IDesignInfo::TYPE_STRING, true, false, 
		&(varDefault.SetString("Top"),varDefault) );
    di->AddPropInfo("HAnchor", IDesignInfo::TYPE_STRING, true, false, 
		&(varDefault.SetString("Left"),varDefault) );
    //设置是否进行超出裁剪
    di->AddPropInfo("IsAutoClip", IDesignInfo::TYPE_BOOL, true, true,
        &(varDefault.SetBool(false),varDefault));
    di->AddPropInfo("OffsetX", IDesignInfo::TYPE_INT, true, true);
    di->AddPropInfo("OffsetY", IDesignInfo::TYPE_INT, true, true);

    di->AddPropInfo("AnimationImage", IDesignInfo::TYPE_STRING, true, true);

	di->AddPropInfo("BlendColor", IDesignInfo::TYPE_COLOR, true, true, 
		&(varDefault.SetString(GuiUtil_IntToColor(CGuiConst::WHITE_COLOR).c_str()),varDefault) );

    di->AddPropInfo("TestTrans", IDesignInfo::TYPE_BOOL, true, true);
  //  di->AddPropInfo("Transparent", IDesignInfo::TYPE_BOOL, true, true, 
		//&CVar(IDesignInfo::TYPE_BOOL, true));
    di->AddPropInfo("Independent", IDesignInfo::TYPE_BOOL, true, true, 
        &(varDefault.SetBool(true),varDefault));
    di->AddPropInfo("Loop", IDesignInfo::TYPE_BOOL, true, true, 
        &(varDefault.SetBool(true),varDefault));
    di->AddPropInfo("Athwart", IDesignInfo::TYPE_BOOL, true, true, 
        &(varDefault.SetBool(false),varDefault));
    di->AddPropInfo("PlayMode", IDesignInfo::TYPE_INT, true, true);

    di->AddEventInfo("on_animation_start");
    di->AddEventInfo("on_animation_loop");
    di->AddEventInfo("on_animation_end");
}

//void Animation::SetTransparent(const bool bTrans)
//{
//    m_bTransparent = bTrans;
//}
//
//const bool Animation::GetTransparent()
//{
//    return m_bTransparent;
//}

bool Animation::TestTransparent(int x, int y)
{
    //if (GetTransparent())
    //{
    //    // 完全透明
    //    return true;
    //}

    return IControl::TestTransparent(x, y);
}

ImageAnimationManager* Animation::InnerGetAnimation()
{
	const IVar& var = GetCore()->GetGlobalValue("ImageAnimationManager");
	return (ImageAnimationManager*)GetCore()->GetEntity(var.ObjectVal());
}

// 背景图片
void Animation::SetAnimationImage(const char* value)
{
    if (value == NULL)
    {
        return;
    }

    m_szAnimationName = value;

    ImageAnimationManager* pAniManager = InnerGetAnimation();

    if (pAniManager)
    {
        if (pAniManager->IsAnimation(value))
        {
            point_t pt = pAniManager->GetAnimationSize(
				m_szAnimationName.c_str());

            m_nAnimationWidth = pt.x;
            m_nAnimationHeight = pt.y;

            pAniManager->InitControlParam(m_szAnimationName.c_str(), 
				m_control);

            Stop();
        }
    }
}

result_string Animation::GetAnimationImage() const
{
    return m_szAnimationName;
}

void Animation::Paint(float seconds)
{
    if (!GetVisible())
    {
        return;
    }

    RefreshInnerAbsPos();

	int x1 = InnerGetAbsLeft() + m_nOffsetX;
	int y1 = InnerGetAbsTop() + m_nOffsetY;
	int x2 = x1 + m_nAnimationWidth;
	int y2 = y1 + m_nAnimationHeight;

    if (!m_IsAutoClip)
    {
        GetPainter()->SetViewport(0, 0, GetPainter()->GetRenderWidth(), 
			GetPainter()->GetRenderHeight(), false);
    }

    DrawAnimationImage(seconds, x1, y1, x2, y2);

    if (!m_IsAutoClip)
    {
        GetPainter()->ResetViewport();
    }
}

// 画背景贴图
bool Animation::DrawAnimationImage(float seconds, int x1, int y1, int x2, 
	int y2)
{
    ImageAnimationManager* pAniManager = InnerGetAnimation();

    if (pAniManager)
    {
        if (pAniManager->IsAnimation(m_szAnimationName.c_str()))
        {
            rect_t rect = { x1, y1, x2, y2 };

            if (m_bIndependent)
            {
                return pAniManager->DrawAnimation(m_szAnimationName.c_str(), 
				    x1, y1, rect, m_control, GetBlendColor());
            }
            else
            {
                return pAniManager->DrawAnimation(m_szAnimationName.c_str(), 
                    x1, y1, rect, GetBlendColor());
            }
        }
    }

    return true;
}

bool Animation::IsBlendAnimation()
{
    ImageAnimationManager* pAniManager = InnerGetAnimation();

    if (pAniManager)
    {
        return pAniManager->IsBlendAnimation(m_szAnimationName.c_str());
    }

    return false;
}

// 独立控制函数
bool Animation::Play()
{
	if (m_ePlayMode == MODE_STOP)
	{
		m_control.cevent = ImageAnimationManager::EVENT_PLAY_NONE;
	}

    m_ePlayMode = MODE_PLAY;

	if (m_control.athwart)
	{
		// 逆向播放
		if (m_control.lTimeCount == m_control.lTime)
		{
			// 产生动画开始事件
			GuiUtil_RunCallback(this, "on_animation_start", 
				CVarList() << m_szAnimationName);
		}
	}
    else
	{
		// 正向播放
		if (m_control.lTimeCount == 0)
		{
			// 产生动画开始事件
			GuiUtil_RunCallback(this, "on_animation_start", 
				CVarList() << m_szAnimationName);
		}
	}

    ImageAnimationManager* pAniManager = InnerGetAnimation();

    if (pAniManager)
    {
        pAniManager->UpdateDynamicCount(m_szAnimationName.c_str());
    }

    return true;
}

// 独立控制函数
bool Animation::PlayPauseAt(int time)
{
    m_ePlayMode = MODE_PLAY;
    m_nPauseAtTime = time;
    return true;
}

bool Animation::Pause()
{
    m_ePlayMode = MODE_PAUSE;
    return true;
}

bool Animation::Stop()
{
    // 停留在最开始
    m_ePlayMode = MODE_STOP;

    ImageAnimationManager* pAniManager = InnerGetAnimation();

    if (pAniManager)
    {
        return pAniManager->StartAnimation(m_szAnimationName.c_str(), 
			m_control);
    }

    return true;
}

