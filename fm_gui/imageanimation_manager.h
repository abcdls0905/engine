//--------------------------------------------------------------------
// 文件名:		ImageAnimationManager.h
// 内  容:		聊天或其它使用到的动画表情管理器
// 说  明:		
// 创建日期:	2008年8月22日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef ImageAnimationManager_h_2008_8_22
#define ImageAnimationManager_h_2008_8_22

#include "../public/i_entity.h"
#include "../visual/i_painter.h"
#include "../utils/string_pod.h"
#include "gui_utils.h"


#include <string>
#include <vector>
#include <map>
//#include <windows.h>

typedef FmMat4 D3DXMATRIX;
// ImageAnimationManager

// 说明
// 静态创建和动态创建的都可以直接调用DrawAnimation,
// 静态创建的会在管理器初始化载入数据时就被创建，直到管理器析构时才被清理
// 动态创建的只有在被画的时候发现未创建的情况下才会被动态创建，同时记录绘制时间，
//  如果超过5秒不绘就会在Execute中被自动清理

class ImageAnimationManager: public IEntity
{
public:
    struct FaceFrame
    {
        FaceFrame()
        {
            lEndTime = 100;
        }

        std::string szFileName;		// 文件名
        PERSISTID ImageID;			// 创建后图元ID
        long lEndTime;			// 保持时间
    };

    struct AlphaItem
	{
        int starttime;
        int alpha;
    };

    struct RotateItem
	{
        int starttime;
        int angle;
    };

    struct ScaleItem
	{
        int starttime;
        float xvalue;
        float yvalue;
    };

    struct MoveItem
    {
        int x;
        int y;
        long lTimeIndex;
    };

    enum ePlayEvent
    {
        EVENT_PLAY_NONE  = 0,
        EVENT_PLAY_START = 1,
        EVENT_PLAY_END   = 2,
        EVENT_PLAY_LOOP  = 3,
        EVENT_PLAY_MAX = 4,
    };

    // 控制参数
    struct ControlParam
    {
        ControlParam()
        {
            lTimeCount = 0;
            lCurIndex = 0;
            x = 0;
            y = 0;

            nCurAlpha = 255;
            fCurXScaleValue = 1.0f;
            fCurYScaleValue = 1.0f;
            nCurAngle = 0;
            loop = true;
            athwart = false;
            cevent = EVENT_PLAY_NONE;
        }
        // 动画控制参数
        long lTime;        // 时间周期
        long lTimeCount;        // 关键帧动画总计时
        long lCurIndex;		    // 当前播放的图元序号

        int x;                  // 当前X位置坐标
        int y;                  // 当前Y位置坐标

        std::vector<ControlParam> vSubControls;
        // 参数改变导致的动画结束值
        int nCurAlpha;
        float fCurXScaleValue;
        float fCurYScaleValue;
        int nCurAngle;
        bool loop;              // 是否循环
        bool athwart;           // 是否是逆向播放
        // 返回参数
        ePlayEvent cevent;
    };

    struct SubAnimation
    {
        int x;
        int y;
        bool bFollowMove;
        bool bFollowAlpha;
        bool bFollowScale;
        bool bFollowRotate;
        bool bFollowPlay;
        int start_time;
        int end_time;
        std::string szSubAniName;   // 子特效名
    };

    enum ePartMotiveMode
    {
        PART_LTTOALL = 0,
        PART_LTOALL = 1,
        PART_LBTOALL = 2,
        PART_BTOALL = 3,
        PART_BRTOALL = 4,
        PART_RTOALL = 5,
        PART_TRTOALL = 6,
        PART_TTOALL = 7,
        PART_MTOLRSIDE = 8,
        PART_MTOTBSIDE = 9,
        PART_CTOALL = 10,
    };

    struct PartMotive
    {
        PartMotive()
        {
            x = 0;
            y = 0;
            lStartTime = 0;
            lEndTime = 0;
            width = 0;
            height = 0;
        }

        ePartMotiveMode mode;
        int x;
        int y;
        std::string szFileName;
        PERSISTID ImageID;
        // 创建后获得参数
        int width;
        int height;

        long lStartTime;
        long lEndTime;
    };

    struct FaceItem
    {
        FaceItem()
        {
            control.lTimeCount = 0;
            control.lCurIndex = 0;

            control.x = 0;
            control.y = 0;

            lWidth = 32;
            lHeight = 32;
            UseCount = 0;
            IsDynamicCreate = false;
            IsHasRotateMovite = false;
            IsHasScaleMotive = false;
            IsHasAlphaMotive = false;
            lLastDrawTime = 0;
            IsPng = false;
            IsEverCreate = false;
            IsCreated = false;
            nBlendMode = 0;
        }

        // 从配置中读取-------------------------------
        // 1 是否动态创建
        bool IsDynamicCreate;
        // 2 是否是PNG
        bool IsPng;
        // 3 融合模式0默认无混合,1正常, 2,变亮,3,变暗
        int nBlendMode;
        // 4 是否有旋转动画(旋转是以中心点旋转)
        bool IsHasRotateMovite;
        std::vector<RotateItem> vRotateMotive;
        // 5 是否有放缩动画(放缩是以中心点放缩)
        bool IsHasScaleMotive;
        std::vector<ScaleItem> vScaleMotive;
        // 6 是否有ALHPA透明动画
        bool IsHasAlphaMotive;
        std::vector<AlphaItem> vAlphaMotive;
        // 4-6是整个运动周期内的动画
        // 7 动画名称
        std::string szFaceName;
        // 8 动画的帧
        std::vector<FaceFrame> vFrames;	// 表情帧
        // 9 运动动画
        std::vector<MoveItem> vMoveMotive;
        // 10 局部变化动画
        std::vector<PartMotive> vPartMotive;
        // 11 子特效
        std::vector<SubAnimation> vSubAnimation;

        long UseCount;		    // 引用计数

        ControlParam control;

        long lLastDrawTime;     // 记录最近一次被绘制的系统时间
        bool IsEverCreate;      // 是否曾经创建过，是否至少创建过一次
        bool IsCreated;         // 是否已被创建

        long lWidth;
        long lHeight;
    };

private:
	struct AniCallBackItem
	{
		PERSISTID id;
		std::string szLuaFile;
		std::string szLuaFunc;
	};

	typedef std::vector<AniCallBackItem> AniCallBacks;

public:
	ImageAnimationManager();
    virtual ~ImageAnimationManager();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// 更新动画帧
	virtual void Execute(float seconds);

    // 获取动画配置文件
    void SetFilePath(const char* pFile);
    const char* GetFilePath();

	// 判断是否是动画
	bool IsAnimation(const char* pName);
    // 是否是混合动画
    bool IsBlendAnimation(const char* pName);
	// 绘制一种动画
    bool DrawAnimation(const char* pName, long x, long y, rect_t& rect, int blend_color = 0xffffffff);
	bool DrawAnimation(const char* pName, long x, long y, rect_t& rect, 
		ControlParam& control, int blend_color = 0xffffffff);
    bool DrawSubAnimation(const char* pName, long x, long y, rect_t& rect, 
        ControlParam& parentcontrol, const size_t control_index, 
		SubAnimation& subconfig, D3DXMATRIX& matRotate);
	// 获取图片动画的宽高
	point_t GetAnimationSize(const char* pName);
    // 设置动画是否循环
    bool SetAnimationLoop(const char* pName, bool value);
    // 启动动画,默认创建好就启动
    bool StartAnimation(const char* pName);
    float GetAnimationCurTime(const char* pName);
	bool SetAnimationCurTime(const char* pName, int millionseconds);
    float GetAnimationTotalTime(const char* pName);
    // 
    bool Register(const char* pName, const char* pLuaFile, 
		const char* pLuaFunc, PERSISTID id);
    bool UnRegister(const char* pName, const char* pLuaFile, 
		const char* pLuaFunc, PERSISTID id);

    bool Clear();

    // 重载所有动画
    bool Load();

    // 获取动画列表
    void GetAnimationList(const IVarList& param, IVarList& result);
    // 独立特效控制--------------------------------------------------------
    // 开始从0播放动画
    bool StartAnimation(const char* pName, ControlParam& control);
    // 更新动画帧
    bool UpdateAnimationByName(const char* pName, ControlParam& control, 
		long lDelta_Time);
    // 更新动画动态加载释放计时
    bool UpdateDynamicCount(const char* pName);
    // 初始化控制器
    bool InitControlParam(const char* pName, ControlParam& control);

private:
    // 只有动态创建的动画才会调这个函数
    bool DynamicCreateAnmation(FaceItem &faceitem);
    bool DoCallBack(const char* pName, const char* pMode);
    PERSISTID CreateGlyph(void* Pointer, bool IsPng, const char* pFileName, 
		long& Width, long& Height);

    //bool UpdateAnimation(size_t index, FaceItem& faceItem, long lDelta_Time);
    bool UpdateAnimation(FaceItem& faceitem, ControlParam& control, 
		long lDelta_Time);

    // 正序动画
    bool OrderUpdate(FaceItem& faceItem, ControlParam& control, 
		int delt_time);
    // 逆序动画
    bool AthwardUpdate(FaceItem& faceItem, ControlParam& control, 
		int delt_time);

    // 设到开始状态
    bool SetToBegin(FaceItem& faceitem, ControlParam& control);
    // 设到结束状态
    bool SetToEnd(FaceItem& faceitem, ControlParam& control);

	// 获得动画回调列表
	AniCallBacks& GetAniCallBacks(const char* name);

private:
	IPainter* m_pPainter;
    std::string m_szFilePath;
	std::vector<FaceItem> m_vFaces;
	TStringPod<char, size_t, TStringTraits<char>, 
		TCoreAlloc> m_mNameToIndexMap;
	//std::map<std::string, long> m_mNameToIndexMap;

    //struct AniCallBackItem
    //{
	//	PERSISTID id;
    //  std::string szLuaFile;
    //    std::string szLuaFunc;
    //};

	std::vector<AniCallBacks*> m_CallBacks;
	TStringPod<char, size_t, TStringTraits<char>, TCoreAlloc> m_CallBackIndex;
    //std::map<std::string, std::vector<AniCallBackItem>> m_mapCallBack;
    bool m_bDeleteSafe;
    //bool m_bNeedClear;
    std::vector<std::string> m_vNeedClear;
};

#endif //ImageAnimationManager_h_2008_8_22

