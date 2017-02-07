//--------------------------------------------------------------------
// �ļ���:		Animation.h
// ��  ��:		������
// ˵  ��:		��Ϊ����������ϣ�����ü�,���ImageAnimationMangerʵ�ֶ�������
// ��������:	2008��10��23��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef Animation_h_2008_10_23
#define Animation_h_2008_10_23

#include "i_control.h"
#include "imageanimation_manager.h"

// Animation

class Animation : public IControl
{
public:
    Animation();
    virtual ~Animation();

    virtual bool Init(const IVarList& args);
    virtual bool Shut();
    virtual void Execute(float seconds);
    // ���ƿؼ�
    virtual void Paint(float seconds);
    // �����������Ϣ
    virtual void PrepareDesignInfo(IDesignInfo* di);
    // ͸�����Դ���
    virtual bool TestTransparent(int x, int y);

    // �Զ��ü�
    void SetAutoClip(bool IsAutoClip) { m_IsAutoClip = IsAutoClip; }
    bool GetAutoClip() { return m_IsAutoClip; }

    // ����Xƫ��
    void SetOffsetX(int offset) { m_nOffsetX = offset; }
    int GetOffsetX() { return m_nOffsetX; }

    // ����Yƫ��
    void SetOffsetY(int offset) { m_nOffsetY = offset; }
    int GetOffsetY() { return m_nOffsetY; }

    // ����ͼƬ
    void SetAnimationImage(const char* value);
    result_string GetAnimationImage() const;

    //// ͸��
    //void SetTransparent(const bool bTrans);
    //const bool GetTransparent();

    // ��������ͼ
    bool DrawAnimationImage(float seconds, int x1, int y1, int x2, int y2);
    // �Ƿ��ǻ�ϵ���ģʽ
    bool IsBlendAnimation();

    // �������ƶ���
    void SetIndependent(bool value);
    bool GetIndependent(){return m_bIndependent;};

    // �Ƿ�ѭ��
    void SetLoop(bool value);
    bool GetLoop(){return m_control.loop;};

    // �Ƿ�����
    void SetAthwart(bool value);
    bool GetAthwart(){return m_control.athwart;};

    // ��ȡ�����ĵ�ǰʱ��
    int GetCurTime();

    // ���ö�������ģʽ
    void SetPlayMode(int value);
    int GetPlayMode();

    // �������ƺ���
    bool Play();
    bool Pause();
    bool Stop();
    bool PlayPauseAt(int time);

private:
    ImageAnimationManager* InnerGetAnimation();

private:
	bool m_IsAutoClip;
    int m_nOffsetX;
    int m_nOffsetY;
    int m_nAnimationWidth;
    int m_nAnimationHeight;
    PERSISTID m_ImageAnimationMng;
    result_string m_szAnimationName;
    //bool m_bTransparent;

    // �������ƶ���
    bool m_bIndependent;
    int m_nPauseAtTime;

    enum ePlayerMode
    {
        MODE_PLAY = 0,
        MODE_PAUSE = 1,
        MODE_STOP = 2,
        MODE_MAX = 3,
    };

    ePlayerMode m_ePlayMode;
    ImageAnimationManager::ControlParam m_control;
};

#endif //Animation_h_2008_10_23

