//--------------------------------------------------------------------
// �ļ���:		AnimationPath.h
// ��  ��:		������
// ˵  ��:		��Ϊ����������ϣ�����ü�,���ImageAnimationPathMangerʵ�ֶ�������
// ��������:	2011��2��9��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef AnimationPath_h_2011_2_9
#define AnimationPath_h_2011_2_9

#include "../public/core_mem.h"
#include "../utils/array_pod.h"
#include "i_control.h"
#include "imageanimation_manager.h"

// AnimationPath

class AnimationPath : public IControl
{
public:
    AnimationPath();
    virtual ~AnimationPath();

    virtual bool Init(const IVarList& args);
    virtual bool Shut();
    virtual void Execute(float seconds);
    // ���ƿؼ�
    virtual void Paint(float seconds);
    // �����������Ϣ
    virtual void PrepareDesignInfo(IDesignInfo* di);
    // ͸�����Դ���
    virtual bool TestTransparent(int x, int y);

    // ����ͼƬ
    void SetAnimationImage(const char* value);
    result_string GetAnimationImage() const;

    //// ͸��
    //void SetTransparent(const bool bTrans);
    //const bool GetTransparent();
	
    void SetColorString(const char* val);
    result_string GetColorString() const;

    // �Ƿ�ѭ��
    void SetLoop(bool value);
    bool GetLoop(){return m_bLoop;};

    // �Ƿ�����
    void SetAthwart(bool value);
    bool GetAthwart(){return m_bAthwart;};

    // �Ƿ�⻬�켣�˶�
    void SetSmoothPath(bool value);
    bool GetSmoothPath(){return m_bSmoothPath;};

    // �Ƿ�պ�·��
    void SetClosePath(bool value);
    bool GetClosePath(){return m_bClosePath;};

    bool ClearPathPoints();
    bool AddPathPoint(float x, float y);
    bool AddPathPointFinish();

    // �����Ч��
    void AddElement(unsigned int time);

    // ��ȡ�����ĵ�ǰʱ��
    int GetCurTime();

    // ���ö�������
    void SetMaxTime(int value);
    int GetMaxTime(){return m_nMaxTime;}

    // ������ת�ٶ�
    void SetRotateSpeed(float value){m_fRotateSpeed = value;}
    float GetRotateSpeed(){return m_fRotateSpeed;}

    // ��������Ƭ��ʼ͸����
    void SetBeginAlpha(float value){m_fBeginAlpha = value;}
    float GetBeginAlpha(){return m_fBeginAlpha;}

    // ������ת�ٶ�
    void SetAlphaChangeSpeed(float value){m_fAlphaChangeSpeed = value;}
    float GetAlphaChangeSpeed(){return m_fAlphaChangeSpeed;}

    // ��������Ƭ��ʼ������
    void SetBeginScale(float value){m_fBeginScale = value;}
    float GetBeginScale(){return m_fBeginScale;}

    // ��������Ƭ�����ٶ�
    void SetScaleSpeed(float value){m_fScaleSpeed = value;}
    float GetScaleSpeed(){return m_fScaleSpeed;}

    // ����ֱ���˶�ʱ��·������ֱ������
    void SetMaxWave(float value){m_fMaxWave = value;}
    float GetMaxWave(){return m_fMaxWave;}

    // ��������Ƭ������С���
    void SetCreateMinInterval(int value){m_nCreateMinInterval = value;}
    int GetCreateMinInterval(){return m_nCreateMinInterval;}

    // ��������Ƭ���������
    void SetCreateMaxInterval(int value){m_nCreateMaxInterval = value;}
    int GetCreateMaxInterval(){return m_nCreateMaxInterval;}

    // ���ö�������ģʽ
    void SetPlayMode(int value);
    int GetPlayMode();

    // �������ƺ���
    bool Play();
    bool Pause();
    bool Stop();
    bool PlayPauseAt(int time);

private:
    FmVec2 GetLinePos(unsigned int time);
    FmVec2 GetSmoothPos(unsigned int time);

    //bool m_bTransparent;                // �Ƿ�͸��
    bool m_bLoop;                       // �Ƿ�ѭ��
    bool m_bAthwart;                    // �Ƿ�����
    bool m_bSmoothPath;                 // �Ƿ��ǹ⻬���߹켣, ����Ϊֱ��
    bool m_bClosePath;
    int m_nLastCreateTime;              // �ϴδ������ʱ��
    int m_nCurTimeCount;                // ��ǰ��ʱ
    int m_nMaxTime;                     // ��������˶�һ�ε�ʱ��
    int m_nCurIndex;                    // ��ǰ�˶����ĸ���ŵ�·���ؼ���
    CColorData m_EffectColor;           // ��Ч��ɫ

    float m_fRotateSpeed;               // ��ת�ٶ�
    float m_fAlphaChangeSpeed;          // ͸���ȸı��ٶ�
    float m_fBeginAlpha;                // ��ʼ͸����
    float m_fBeginScale;                // ��ʼ�����Ŵ���
    float m_fScaleSpeed;                // �����ٶ�
    float m_fMaxWave;                   // ������
    int m_nCreateMinInterval;           // ������С���
    int m_nCreateMaxInterval;           // ���������
    
    std::string m_szAnimationImage;     // ͼԪ
    PERSISTID m_ImageID;
    // �������ƶ���
    int m_nPauseAtTime;

    enum ePlayerMode
    {
        MODE_PLAY = 0,
        MODE_PAUSE = 1,
        MODE_STOP = 2,
        MODE_MAX = 3,
    };

    ePlayerMode m_ePlayMode;

    enum ePlayEvent
    {
        EVENT_PLAY_NONE  = 0,
        EVENT_PLAY_START = 1,
        EVENT_PLAY_END   = 2,
        EVENT_PLAY_LOOP  = 3,
        EVENT_PLAY_MAX = 4,
    };

    struct SubElement
    {
        float x;
        float y;
        float orient;
        float scale;
        float alpha;
    };

    TArrayPod<SubElement, 1, TCoreAlloc> m_vElement;

    struct PathPoint
    {
        FmVec2 pos;
        float dist_to_next; // ����ֱ���˶�
        float keep_time;    // ���ڹ⻬�켣
    };

    float m_fTotalDist;     // �Ǳպ�
    float m_fCloseDist;     // �պ�

    TArrayPod<PathPoint, 1, TCoreAlloc> m_vPathPoints;

    // ����ֵ--------------------------
    FmVec2 m_vLast;
    FmVec2 m_vStart;
    FmVec2 m_vTarget;
    FmVec2 m_vNext;
    // -----------------------------------
};

#endif //AnimationPath_h_2011_2_9

