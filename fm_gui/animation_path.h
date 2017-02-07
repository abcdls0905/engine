//--------------------------------------------------------------------
// 文件名:		AnimationPath.h
// 内  容:		动画控
// 说  明:		因为动画本身不大希望被裁剪,配合ImageAnimationPathManger实现动画功能
// 创建日期:	2011年2月9日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
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
    // 绘制控件
    virtual void Paint(float seconds);
    // 设置设计器信息
    virtual void PrepareDesignInfo(IDesignInfo* di);
    // 透明测试处理
    virtual bool TestTransparent(int x, int y);

    // 背景图片
    void SetAnimationImage(const char* value);
    result_string GetAnimationImage() const;

    //// 透明
    //void SetTransparent(const bool bTrans);
    //const bool GetTransparent();
	
    void SetColorString(const char* val);
    result_string GetColorString() const;

    // 是否循环
    void SetLoop(bool value);
    bool GetLoop(){return m_bLoop;};

    // 是否逆向
    void SetAthwart(bool value);
    bool GetAthwart(){return m_bAthwart;};

    // 是否光滑轨迹运动
    void SetSmoothPath(bool value);
    bool GetSmoothPath(){return m_bSmoothPath;};

    // 是否闭合路径
    void SetClosePath(bool value);
    bool GetClosePath(){return m_bClosePath;};

    bool ClearPathPoints();
    bool AddPathPoint(float x, float y);
    bool AddPathPointFinish();

    // 添加特效点
    void AddElement(unsigned int time);

    // 获取动画的当前时间
    int GetCurTime();

    // 设置动画周期
    void SetMaxTime(int value);
    int GetMaxTime(){return m_nMaxTime;}

    // 设置旋转速度
    void SetRotateSpeed(float value){m_fRotateSpeed = value;}
    float GetRotateSpeed(){return m_fRotateSpeed;}

    // 设置子面片起始透明度
    void SetBeginAlpha(float value){m_fBeginAlpha = value;}
    float GetBeginAlpha(){return m_fBeginAlpha;}

    // 设置旋转速度
    void SetAlphaChangeSpeed(float value){m_fAlphaChangeSpeed = value;}
    float GetAlphaChangeSpeed(){return m_fAlphaChangeSpeed;}

    // 设置子面片起始放缩量
    void SetBeginScale(float value){m_fBeginScale = value;}
    float GetBeginScale(){return m_fBeginScale;}

    // 设置子面片缩放速度
    void SetScaleSpeed(float value){m_fScaleSpeed = value;}
    float GetScaleSpeed(){return m_fScaleSpeed;}

    // 设置直接运动时和路径方向垂直的噪量
    void SetMaxWave(float value){m_fMaxWave = value;}
    float GetMaxWave(){return m_fMaxWave;}

    // 设置子面片创建最小间隔
    void SetCreateMinInterval(int value){m_nCreateMinInterval = value;}
    int GetCreateMinInterval(){return m_nCreateMinInterval;}

    // 设置子面片创建最大间隔
    void SetCreateMaxInterval(int value){m_nCreateMaxInterval = value;}
    int GetCreateMaxInterval(){return m_nCreateMaxInterval;}

    // 设置动画播放模式
    void SetPlayMode(int value);
    int GetPlayMode();

    // 独立控制函数
    bool Play();
    bool Pause();
    bool Stop();
    bool PlayPauseAt(int time);

private:
    FmVec2 GetLinePos(unsigned int time);
    FmVec2 GetSmoothPos(unsigned int time);

    //bool m_bTransparent;                // 是否透明
    bool m_bLoop;                       // 是否循环
    bool m_bAthwart;                    // 是否逆向
    bool m_bSmoothPath;                 // 是否是光滑由线轨迹, 否则为直线
    bool m_bClosePath;
    int m_nLastCreateTime;              // 上次创建点的时间
    int m_nCurTimeCount;                // 当前计时
    int m_nMaxTime;                     // 完成整个运动一次的时间
    int m_nCurIndex;                    // 当前运动到哪个序号的路径关键点
    CColorData m_EffectColor;           // 特效颜色

    float m_fRotateSpeed;               // 旋转速度
    float m_fAlphaChangeSpeed;          // 透明度改变速度
    float m_fBeginAlpha;                // 起始透明度
    float m_fBeginScale;                // 起始的缩放处理
    float m_fScaleSpeed;                // 放缩速度
    float m_fMaxWave;                   // 最大噪幅
    int m_nCreateMinInterval;           // 发射最小间隔
    int m_nCreateMaxInterval;           // 发射最大间隔
    
    std::string m_szAnimationImage;     // 图元
    PERSISTID m_ImageID;
    // 独立控制动画
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
        float dist_to_next; // 用于直接运动
        float keep_time;    // 用于光滑轨迹
    };

    float m_fTotalDist;     // 非闭合
    float m_fCloseDist;     // 闭合

    TArrayPod<PathPoint, 1, TCoreAlloc> m_vPathPoints;

    // 多点插值--------------------------
    FmVec2 m_vLast;
    FmVec2 m_vStart;
    FmVec2 m_vTarget;
    FmVec2 m_vNext;
    // -----------------------------------
};

#endif //AnimationPath_h_2011_2_9

