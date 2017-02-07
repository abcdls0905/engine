//--------------------------------------------------------------------
// 文件名:		Animation.h
// 内  容:		动画控
// 说  明:		因为动画本身不大希望被裁剪,配合ImageAnimationManger实现动画功能
// 创建日期:	2008年10月23日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
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
    // 绘制控件
    virtual void Paint(float seconds);
    // 设置设计器信息
    virtual void PrepareDesignInfo(IDesignInfo* di);
    // 透明测试处理
    virtual bool TestTransparent(int x, int y);

    // 自动裁剪
    void SetAutoClip(bool IsAutoClip) { m_IsAutoClip = IsAutoClip; }
    bool GetAutoClip() { return m_IsAutoClip; }

    // 绘制X偏移
    void SetOffsetX(int offset) { m_nOffsetX = offset; }
    int GetOffsetX() { return m_nOffsetX; }

    // 绘制Y偏移
    void SetOffsetY(int offset) { m_nOffsetY = offset; }
    int GetOffsetY() { return m_nOffsetY; }

    // 背景图片
    void SetAnimationImage(const char* value);
    result_string GetAnimationImage() const;

    //// 透明
    //void SetTransparent(const bool bTrans);
    //const bool GetTransparent();

    // 画背景贴图
    bool DrawAnimationImage(float seconds, int x1, int y1, int x2, int y2);
    // 是否是混合叠加模式
    bool IsBlendAnimation();

    // 独立控制动画
    void SetIndependent(bool value);
    bool GetIndependent(){return m_bIndependent;};

    // 是否循环
    void SetLoop(bool value);
    bool GetLoop(){return m_control.loop;};

    // 是否逆向
    void SetAthwart(bool value);
    bool GetAthwart(){return m_control.athwart;};

    // 获取动画的当前时间
    int GetCurTime();

    // 设置动画播放模式
    void SetPlayMode(int value);
    int GetPlayMode();

    // 独立控制函数
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

    // 独立控制动画
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

