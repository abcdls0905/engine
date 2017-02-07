//--------------------------------------------------------------------
// �ļ���:		AnimationPath.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2011��2��9��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "animation_path.h"

/// entity: AnimationPath
/// \brief ͼƬ�����ؼ�
DECLARE_ENTITY(AnimationPath, 0, IControl);

/// property: string AnimationPathImage
/// \brief ����ͼԴ
DECLARE_PROPERTY(result_string, AnimationPath, AnimationImage, GetAnimationImage, 
	SetAnimationImage);
///// property: bool Transparent
///// \brief �Ƿ��ж�͸��
//DECLARE_PROPERTY(bool, AnimationPath, Transparent, GetTransparent, SetTransparent);

/// property: bool Loop
/// \brief ����IndependentΪtrueʱLoopΪfalse����Ч
DECLARE_PROPERTY(bool, AnimationPath, Loop, GetLoop, SetLoop);

/// property: bool Athwart
/// \brief ���򲥷�, �������ͷ��ʼ����Athwartֵ�仯�����һ��Stop
DECLARE_PROPERTY(bool, AnimationPath, Athwart, GetAthwart, SetAthwart);

/// property: bool SmoothPath
/// \brief 
DECLARE_PROPERTY(bool, AnimationPath, SmoothPath, GetSmoothPath, SetSmoothPath);

/// property: bool ClosePath
/// \brief 
DECLARE_PROPERTY(bool, AnimationPath, ClosePath, GetClosePath, SetClosePath);

/// property: string Color
/// \brief ��Ч��ɫ
DECLARE_PROPERTY(result_string, AnimationPath, Color, GetColorString, 
	SetColorString);

/// property: int CurTime
DECLARE_PROPERTY_GET(int, AnimationPath, CurTime, GetCurTime);

/// property: int MaxTime
/// \brief ����,���ǲ���һ�ε���ʱ��
DECLARE_PROPERTY(int, AnimationPath, MaxTime, GetMaxTime, SetMaxTime);

/// property: int PlayMode
/// \brief ��ǰ����״̬,0��ʾ���ڲ��ţ�1��ʾ��ͣ��2��ʾ����
DECLARE_PROPERTY(int, AnimationPath, PlayMode, GetPlayMode, SetPlayMode);

/// property: float RotateSpeed
/// \brief ת��
DECLARE_PROPERTY(float, AnimationPath, RotateSpeed, GetRotateSpeed, SetRotateSpeed);

/// property: float AlphaChangeSpeed
/// \brief ͸���ȸı��ٶ�
DECLARE_PROPERTY(float, AnimationPath, AlphaChangeSpeed, GetAlphaChangeSpeed, SetAlphaChangeSpeed);

/// property: float BeginAlpha
/// \brief ��ʼ͸����
DECLARE_PROPERTY(float, AnimationPath, BeginAlpha, GetBeginAlpha, SetBeginAlpha);

/// property: int CreateMinInterval
/// \brief ������С���
DECLARE_PROPERTY(int, AnimationPath, CreateMinInterval, GetCreateMinInterval, SetCreateMinInterval);

/// property: int CreateMaxInterval
/// \brief ���������
DECLARE_PROPERTY(int, AnimationPath, CreateMaxInterval, GetCreateMaxInterval, SetCreateMaxInterval);

/// property: float BeginScale
/// \brief ��ʼ������
DECLARE_PROPERTY(float, AnimationPath, BeginScale, GetBeginScale, SetBeginScale);

/// property: float BeginScale
/// \brief �����ٶ�
DECLARE_PROPERTY(float, AnimationPath, ScaleSpeed, GetScaleSpeed, SetScaleSpeed);

/// property: float MaxWave
/// \brief ֱ���˶�ʱ��������
DECLARE_PROPERTY(float, AnimationPath, MaxWave, GetMaxWave, SetMaxWave);

#if 0
/*
    m_fRotateSpeed = 2.0f;                // ��ת�ٶ�
    m_fAlphaChangeSpeed = 0.3f;           // ͸���ȸı��ٶ�
    m_fBeginAlpha = 0.8f;                 // ��ʼ͸����
    m_fEndAlpha = 0.0f;                   // ����͸����
    m_nCreateMinInterval = 10;            // ������
    m_nCreateMaxInterval = 20;
    m_fBeginScale = 0.4f;
    m_fScaleSpeed = 0.0f;
    m_fMaxWave = 0.05f;*/
/// ���·���ؼ��ڵ�
DECLARE_METHOD_0(bool, AnimationPath, ClearPathPoints);
DECLARE_METHOD_2(bool, AnimationPath, AddPathPoint, float, float);
DECLARE_METHOD_0(bool, AnimationPath, AddPathPointFinish);

DECLARE_METHOD_0(bool, AnimationPath, Play);
DECLARE_METHOD_0(bool, AnimationPath, Pause);
DECLARE_METHOD_0(bool, AnimationPath, Stop);
DECLARE_METHOD_1(bool, AnimationPath, PlayPauseAt, int);
#endif
/// event: int on_animation_start(object self, string name)
/// \brief ������ʼ����
/// param self �ؼ�����
/// param name ������
DECLARE_EVENT(on_animation_start)

/// event:int on_animation_loop(object self, string name)
/// \brief һ�鶯������,ѭ�������ٴο�ʼ����
/// param self �ؼ�����
/// param name ������
DECLARE_EVENT(on_animation_loop)

// ������������ѭ��������ֹͣ����
/// \brief ������ʼ����
/// param self �ؼ�����
/// param name ������
DECLARE_EVENT(on_animation_end)

AnimationPath::AnimationPath()
{
    //m_bTransparent = true;
	//SetTransparentDefault(true);
    m_bSmoothPath = false;
    m_ePlayMode = MODE_STOP;
    m_nPauseAtTime = -1;
    m_bAthwart = false;
    m_bLoop = false;
    m_bClosePath = false;
    m_EffectColor.SetValue(0xff08AAFF);

    m_nLastCreateTime = 0;
    m_nCurTimeCount = 0;
    m_nMaxTime = 2000;

    m_fRotateSpeed = 2.0f;                // ��ת�ٶ�
    m_fAlphaChangeSpeed = 0.3f;           // ͸���ȸı��ٶ�
    m_fBeginAlpha = 0.8f;                 // ��ʼ͸����
    m_nCreateMinInterval = 10;            // ������
    m_nCreateMaxInterval = 20;
    m_fBeginScale = 0.4f;
    m_fScaleSpeed = 0.0f;
    m_fMaxWave = 0.05f;
}

AnimationPath::~AnimationPath()
{
}

void AnimationPath::SetLoop(bool value)
{
    m_bLoop = value;
}

void AnimationPath::SetAthwart(bool value)
{
    m_bAthwart = value;
}

// �Ƿ�⻬�켣�˶�
void AnimationPath::SetSmoothPath(bool value)
{
    if (!m_bSmoothPath && value)
    {
        m_bSmoothPath = value;

        // �ĳɹ⻬���߹켣
        Stop();
        m_vElement.clear();
        Play();
    }
    else if (m_bSmoothPath && !value)
    {
        m_bSmoothPath = value;
        // �ĳ�ֱ���˶�
        Stop();
        m_vElement.clear();
        Play();
    }
}


// �Ƿ�պ�·��
void AnimationPath::SetClosePath(bool value)
{
    m_bClosePath = value;

    float dist = m_fTotalDist;

    if (value)
    {
        dist = m_fCloseDist;
    }

    size_t size = m_vPathPoints.size();

    // 
    for(size_t i = 0; i < size; ++i)
    {
        m_vPathPoints[i].keep_time = m_vPathPoints[i].dist_to_next * m_nMaxTime * 0.001f / dist;
    }
}

void AnimationPath::SetColorString(const char* val)
{
	Assert(val != NULL);

	m_EffectColor.SetName(GetGui(), val);
}

result_string AnimationPath::GetColorString() const
{
	return m_EffectColor.GetName();
}

void AnimationPath::SetPlayMode(int value)
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

int AnimationPath::GetPlayMode()
{
    return (int)m_ePlayMode;
}

int AnimationPath::GetCurTime()
{
    return m_nCurTimeCount;
}

// ���ö�������
void AnimationPath::SetMaxTime(int value)
{
    if (value < 10)
    {
        return;
    }

    m_nMaxTime = value;
}

// ����ͼƬ
void AnimationPath::SetAnimationImage(const char* value)
{
    if (value == NULL)
    {
        return;
    }

    m_szAnimationImage = value;
    m_ImageID = GetPainter()->CreateGlyph(m_szAnimationImage.c_str(), false);
}

result_string AnimationPath::GetAnimationImage() const
{
    return m_szAnimationImage.c_str();
}

bool AnimationPath::Init(const IVarList& args)
{
	if (!IControl::Init(args))
	{
		return false;
	}

    SetEnabled(false);
	//SetTransparentDefault(true);

    GetCore()->AddExecute(this);

    return true;
}

bool AnimationPath::Shut()
{
    GetCore()->RemoveExecute(this);

    if (!m_ImageID.IsNull())
    {
        IPainter* pPainter = GetPainter();

        if (pPainter)
        {
            pPainter->ReleaseGlyph(m_ImageID);
        }

        m_ImageID = PERSISTID();
    }

    return IControl::Shut();
}

void AnimationPath::Execute(float seconds)
{
    int interval = (int)(seconds * 1000.0f);

    m_nCurTimeCount += interval;

    size_t element_count = m_vElement.size();

    size_t show_count = 0;

    for(size_t i = 0; i < element_count; ++i)
    {
        if (m_vElement[i].alpha < 0.0f)
        {
            continue;
        }

        ++show_count;

        // ͸������
        m_vElement[i].alpha -= seconds * m_fAlphaChangeSpeed;

        if (m_vElement[i].alpha < 0.0f)
        {
            m_vElement[i].alpha = -1.0f;
            continue;
        }

        // ��ת����
        m_vElement[i].orient += seconds * m_fRotateSpeed;
        if (m_vElement[i].orient > FM_PI * 2.0f)
        {
            m_vElement[i].orient -= FM_PI * 2.0f;
        }

        // ����
        if (m_fScaleSpeed > -0.001f)
        {
            m_vElement[i].scale -= seconds * m_fScaleSpeed;
            if (m_vElement[i].scale < 0.0f)
            {
                // ����
                m_vElement[i].alpha = -1.0f;
                continue;
            }
        }
    }

    // ���ڲ���״̬�򷵻�
    if (m_ePlayMode != MODE_PLAY)
    {
        if (show_count == 0 && m_vElement.size() > 0)
        {
            GuiUtil_RunCallback(this, "on_animation_end", 
				CVarList() << this->GetID());
            m_vElement.clear();
        }
        return;
    }

    // ��ȡ��ǰ�������
    int cur_create_interval = m_nCreateMinInterval;

    int sub_interval = (m_nCreateMaxInterval - m_nCreateMinInterval);
    if (sub_interval > 0)
    {
        cur_create_interval += rand() % sub_interval;
    }

    // ��ѭ��
    if (m_bLoop)
    {
        if (m_nCurTimeCount > m_nMaxTime)
        {
            m_nCurTimeCount -= m_nMaxTime;
            m_nLastCreateTime -= m_nMaxTime;
        }

        if (m_nCurTimeCount - m_nLastCreateTime > cur_create_interval)
        {
            m_nLastCreateTime = m_nCurTimeCount;

            AddElement(m_nLastCreateTime);
        }
    }
    else
    {
        if (m_nCurTimeCount >= m_nMaxTime)
        {
            // ��ѭ����ֱ�ӽ���
            
            m_nCurTimeCount = m_nMaxTime;
            m_nLastCreateTime = m_nCurTimeCount;
            AddElement(m_nMaxTime);
            Stop();
            return;
        }

        if (m_nCurTimeCount - m_nLastCreateTime > cur_create_interval)
        {
            m_nLastCreateTime = m_nCurTimeCount;

            AddElement(m_nLastCreateTime);
        }
    }
}

void AnimationPath::PrepareDesignInfo(IDesignInfo* di)
{
	CVar varDefault;
    di->AddPropInfo("Left", IDesignInfo::TYPE_INT, true, true);
    di->AddPropInfo("Top", IDesignInfo::TYPE_INT, true, true);
    di->AddPropInfo("Width", IDesignInfo::TYPE_INT, true, true);
    di->AddPropInfo("Height", IDesignInfo::TYPE_INT, true, true);

    //�����Ƿ���г����ü�
    di->AddPropInfo("AnimationImage", IDesignInfo::TYPE_IMAGE, true, true);
    di->AddPropInfo("TestTrans", IDesignInfo::TYPE_BOOL, true, true);
  //  di->AddPropInfo("Transparent", IDesignInfo::TYPE_BOOL, true, true, 
		//&CVar(IDesignInfo::TYPE_BOOL, true));
    di->AddPropInfo("Loop", IDesignInfo::TYPE_BOOL, true, true, 
        &(varDefault.SetBool(false),varDefault));
    di->AddPropInfo("SmoothPath", IDesignInfo::TYPE_BOOL, true, true, 
        &(varDefault.SetBool(false),varDefault));
    di->AddPropInfo("ClosePath", IDesignInfo::TYPE_BOOL, true, true, 
        &(varDefault.SetBool(false),varDefault));
    di->AddPropInfo("CreateMinInterval", IDesignInfo::TYPE_INT, true, true);
    di->AddPropInfo("CreateMaxInterval", IDesignInfo::TYPE_INT, true, true);
    di->AddPropInfo("RotateSpeed", IDesignInfo::TYPE_FLOAT, true, true);
    di->AddPropInfo("BeginAlpha", IDesignInfo::TYPE_FLOAT, true, true);
    di->AddPropInfo("AlphaChangeSpeed", IDesignInfo::TYPE_FLOAT, true, true);
    di->AddPropInfo("BeginScale", IDesignInfo::TYPE_FLOAT, true, true);
    di->AddPropInfo("ScaleSpeed", IDesignInfo::TYPE_FLOAT, true, true);
    di->AddPropInfo("MaxWave", IDesignInfo::TYPE_FLOAT, true, true);
    di->AddPropInfo("PlayMode", IDesignInfo::TYPE_INT, true, true);

	di->AddPropInfo("Color", IDesignInfo::TYPE_COLOR, true, true, 
		&(varDefault.SetString(GuiUtil_IntToColor(0xff08AAFF).c_str()),varDefault));

    di->AddPropInfo("MaxTime", IDesignInfo::TYPE_INT, true, true);    

    di->AddEventInfo("on_animation_start");
    di->AddEventInfo("on_animation_loop");
    di->AddEventInfo("on_animation_end");
}

//void AnimationPath::SetTransparent(const bool bTrans)
//{
//    m_bTransparent = bTrans;
//}
//
//const bool AnimationPath::GetTransparent()
//{
//    return m_bTransparent;
//}

bool AnimationPath::TestTransparent(int x, int y)
{
//     if (GetTransparent())
//     {
//         // ��ȫ͸��
//         return true;
//     }

    return IControl::TestTransparent(x, y);
}

void AnimationPath::Paint(float seconds)
{
    if (!GetVisible())
    {
        return;
    }

	RefreshInnerAbsPos();

    size_t emelent_count = m_vElement.size();

    if (emelent_count == 0)
    {
        return;
    }

    IPainter * pPainter = GetPainter();

    //pPainter->SetViewport(0, 0, GetPainter()->GetRenderWidth(), 
	//	GetPainter()->GetRenderHeight(), false);

    pPainter->SetBlendAlpha(255);

    TArrayPod<float, 32, TCoreAlloc> vX;
    TArrayPod<float, 32, TCoreAlloc> vY;
    TArrayPod<float, 32, TCoreAlloc> vOrient;
    TArrayPod<float, 32, TCoreAlloc> vScale;
    TArrayPod<float, 32, TCoreAlloc> vAlpha;

    int show_count = 0;

    for (size_t i = emelent_count - 1; i >= 0 ; --i)
    {
        if (m_vElement[i].alpha > -0.1f)
        {
            ++show_count;
            vX.push_back(m_vElement[i].x);
            vY.push_back(m_vElement[i].y);
            vOrient.push_back(m_vElement[i].orient);
            vScale.push_back(m_vElement[i].scale);
            vAlpha.push_back(m_vElement[i].alpha);
        }
    }

    if (vX.size() == 0)
    {
        return;
    }

    /*FmVec2 pos = GetCurPos();
    vX.push_back(pos.x);
    vY.push_back(pos.y);
    vOrient.push_back(0.0f);
    vScale.push_back(1.0f);
    vAlpha.push_back(1.0f);*/

    pPainter->DrawGlyphList(&vX[0], &vY[0], &vOrient[0], 
        &vScale[0], &vAlpha[0], show_count/* + 1*/, m_ImageID, 2, 
		m_EffectColor.GetValue());
}

// �������ƺ���
bool AnimationPath::Play()
{
    m_ePlayMode = MODE_PLAY;

	if (m_bAthwart)
	{
		// ���򲥷�
		if (m_nCurTimeCount >= m_nMaxTime)
		{
			// ����������ʼ�¼�
			GuiUtil_RunCallback(this, "on_animation_start", 
				CVarList() << m_szAnimationImage.c_str());
		}
	}
    else
	{
		// ���򲥷�
		if (m_nCurTimeCount == 0)
		{
			// ����������ʼ�¼�
			GuiUtil_RunCallback(this, "on_animation_start", 
				CVarList() << m_szAnimationImage.c_str());
		}
	}

    m_nCurTimeCount = 0;
    m_nLastCreateTime = 0;

    return true;
}

// �������ƺ���
bool AnimationPath::PlayPauseAt(int time)
{
    m_ePlayMode = MODE_PLAY;
    m_nPauseAtTime = time;
    return true;
}

bool AnimationPath::Pause()
{
    m_ePlayMode = MODE_PAUSE;
    return true;
}

bool AnimationPath::Stop()
{
    // ͣ�����ʼ
    m_ePlayMode = MODE_STOP;

    return true;
}

bool AnimationPath::ClearPathPoints()
{
    m_vPathPoints.clear();
    m_fTotalDist = 0.0f;
    m_fCloseDist = 0.0f;
    return true;
}

bool AnimationPath::AddPathPoint(float x, float y)
{
    size_t count = m_vPathPoints.size();

    m_vPathPoints.resize(count + 1);
    m_vPathPoints[count].pos.x = x;
    m_vPathPoints[count].pos.y = y;
    m_vPathPoints[count].dist_to_next = 0.0f;

    if (count > 0)
    {   
        FmVec2 sub_vec2 = m_vPathPoints[count].pos - m_vPathPoints[count - 1].pos;
        m_vPathPoints[count - 1].dist_to_next = FmVec2Length(&sub_vec2);
        m_fTotalDist += m_vPathPoints[count - 1].dist_to_next;
    }

    return true;
}

bool AnimationPath::AddPathPointFinish()
{
    size_t size = m_vPathPoints.size();

    FmVec2 sub_vec2 = m_vPathPoints[0].pos - m_vPathPoints[size - 1].pos;
    m_vPathPoints[size - 1].dist_to_next = FmVec2Length(&sub_vec2);
    m_fCloseDist = m_fTotalDist + m_vPathPoints[size - 1].dist_to_next;

    if (m_bClosePath)
    {
        // 
        for(size_t i = 0; i < size; ++i)
        {
            m_vPathPoints[i].keep_time = m_vPathPoints[i].dist_to_next * m_nMaxTime * 0.001f / m_fCloseDist;
        }
    }
    else
    {
        for(size_t i = 0; i < size; ++i)
        {
            m_vPathPoints[i].keep_time = m_vPathPoints[i].dist_to_next * m_nMaxTime * 0.001f / m_fTotalDist;
        }
    }


    return true;
}


FmVec2 AnimationPath::GetSmoothPos(unsigned int time)
{
    FmVec2 result_pos(0.0f, 0.0f);

    float cur_time = time / 1000.0f;

    size_t size = m_vPathPoints.size();
    int cur_index = -1;

    for (size_t i = 0 ;i <= size; ++i)
    {
        size_t index = i % size;
        if (cur_time > m_vPathPoints[index].keep_time)
        {
            cur_time -= m_vPathPoints[index].keep_time;
        }
        else
        {
            cur_index = index;
            break;
        }
    }

    if (cur_index == -1)
    {
        cur_index = size - 1;
    }

    // �õ���Ŀ���ʱ��
    float fCurKeepTime = m_vPathPoints[cur_index].keep_time;

    if (!m_bClosePath)
    {
        if (cur_index == size - 1)
        {
            result_pos.x = m_vPathPoints[cur_index].pos.x;
            result_pos.y = m_vPathPoints[cur_index].pos.y;
            return result_pos;
        }

        // ��������ǵ�һ���㣬����һ��Ҳ�ǵ�һ����,��������һ����
        if (cur_index == 0)
        {
            m_vLast = m_vPathPoints[0].pos;
        }
        else
        {
            m_vLast = m_vPathPoints[cur_index - 1].pos;
        }

        
        m_vStart = m_vPathPoints[cur_index].pos;

        // ��������һ����
        m_vTarget = m_vPathPoints[cur_index + 1].pos;

        // ���Ŀ����ȡ��һ����, ��nextҲ�����һ���㣬��������һ����
        if (cur_index + 1 == size - 1)
        {
            m_vNext = m_vTarget;
        }
        else
        {
            m_vNext = m_vPathPoints[cur_index + 2].pos;
        }
    }
    else
    {
        if (cur_index == 0)
        {
            m_vLast = m_vPathPoints[size - 1].pos;
        }
        else
        {
            m_vLast = m_vPathPoints[cur_index - 1].pos;
        }

        m_vStart = m_vPathPoints[cur_index].pos;

        // �������һ����
        if (cur_index == size - 1)
        {
            m_vTarget = m_vPathPoints[0].pos;
            m_vNext = m_vPathPoints[1].pos;
        }
        else
        {
            m_vTarget = m_vPathPoints[cur_index + 1].pos;

            // Ŀ����ȡ��һ����
            if (cur_index + 1 == size - 1)
            {
                m_vNext = m_vPathPoints[0].pos;
            }
            else
            {
                m_vNext = m_vPathPoints[cur_index + 2].pos;
            }
        }
    }

    float cur_ratio = 0.0f;

    if (fCurKeepTime > 0.0001f)
    {
        //��ֹ����
        cur_ratio = cur_time / fCurKeepTime;
    }

    // �����߲�ֵ��ʽ��Pi+1(t)= ��-4*t^3+4*t^2-t��*Pi +(12*t^3-10*t^2+1) Pi+1 +(-12*t^3+8*t^2+t) Pi+2 +(4*t^3��2*t^2) Pi+3     (0=< ti <=0.5)
    float t = cur_ratio * 0.5f;
    float t1 = t;
    float t2 = t1 * t1;
    float t3 = t2 * t1;
    float a = (-4*t3 + 4*t2-t);
    float b = (12*t3 - 10*t2+1);
    float c = (-12*t3 + 8*t2 + t);
    float d = (4*t3 - 2*t2);
    float cur_x = a * m_vLast.x + b * m_vStart.x + c * m_vTarget.x + d * m_vNext.x;
    float cur_y = a * m_vLast.y + b * m_vStart.y + c * m_vTarget.y + d * m_vNext.y;
    //float cur_z = a * m_vLast.z + b * m_vStart.z + c * m_vTarget.z + d * m_vNext.z;

    result_pos.x = cur_x;
    result_pos.y = cur_y;
    //vCurPoint.z = cur_z;

    return result_pos;
}

FmVec2 AnimationPath::GetLinePos(unsigned int time)
{
    FmVec2 result_pos(0.0f, 0.0f);

    float total_dist = m_fTotalDist;

    if (m_bClosePath)
    {
        total_dist = m_fCloseDist;
    }

    float fCurDist = total_dist * time / m_nMaxTime;

    size_t count = m_vPathPoints.size();

    for (size_t i = 0 ;i <= count; ++i)
    {
        size_t index = i % count;
        if (fCurDist > m_vPathPoints[index].dist_to_next)
        {
            fCurDist -= m_vPathPoints[index].dist_to_next;
        }
        else
        {
            float radio = fCurDist / m_vPathPoints[index].dist_to_next;

            FmVec2 & cur_pos = m_vPathPoints[index].pos;
            FmVec2 & next_pos = m_vPathPoints[ (index + 1) % count ].pos;

            FmVec2 sub_pos = next_pos - cur_pos;

            // �����貨���� -------------------------------------
            FmMat4 mat_angle;

            FmVec3 vec90(sub_pos.x, sub_pos.y, 0.0f);

            FmMatrixRotationZ(&mat_angle, FM_PI / 2.0f);

            FmVec3TransformCoord(&vec90, &vec90, &mat_angle);

            FmVec3Normalize(&vec90, &vec90);

            // �����貨����
            vec90 *= (rand() % 100 - 50) * m_fMaxWave;
            // --------------------------------------------------

            result_pos.x = (next_pos.x - cur_pos.x) * radio + cur_pos.x + vec90.x;
            result_pos.y = (next_pos.y - cur_pos.y) * radio + cur_pos.y + vec90.y;
            
            return result_pos;
        }
    }

    return result_pos;
}

// �����Ч��
void AnimationPath::AddElement(unsigned int time)
{
    FmVec2 pos;

    if (m_bSmoothPath && m_vPathPoints.size() > 2)
    {
        pos = GetSmoothPos(time);
    }
    else
    {
        pos = GetLinePos(time);
    }

    int size = (int)m_vElement.size();
    int j = 0;
    for(j = 0; j < size; ++j)
    {
        // ���ϵĸ���ͳһalpha���-1
        if (m_vElement[j].alpha < -0.1f)
        {
            break;
        }
    }
    // ʵ�ʿյĸ���
    if (j >= 0 && j < size)
    {
        // �����ѵĸ���
    }
    else
    {   
        // û�������� 
        m_vElement.resize(m_vElement.size() + 1);
        j = m_vElement.size() - 1;
    }

    m_vElement[j].x = pos.x;
    m_vElement[j].y = pos.y;
    m_vElement[j].orient = (rand() % 360) * FM_PI * 2.0f / 360.0f;
    m_vElement[j].scale = m_fBeginScale;
    m_vElement[j].alpha = m_fBeginAlpha;
}