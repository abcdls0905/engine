
#ifndef _SPRITEMANAGER_H
#define _SPRITEMANAGER_H

#include "../public/i_entity.h"
#include "../public/core_mem.h"
#include "../utils/array_pod.h"
#include "../utils/string_pod.h"
#include "../utils/pod_hash_map.h"
#include "../visual/fm_header.h"
//#include <vector>
#include <string>
//#include <map>
#include "sprite.h"

class Gui;

class SpriteManager: public IEntity
{
public:
    SpriteManager();
    virtual ~SpriteManager();

    // 初始化（可创建其他附属实体）
    virtual bool Init(const IVarList& args) ;
    // 关闭（可删除其他引用的附属实体，而资源释放应在析构函数中进行）
    virtual bool Shut() ;

    // 每帧调用（如果已添加到运行队列）
    virtual void Execute(float seconds);

    // 加载数据和清理------------------------------------------------
    //创建字体池
    bool LoadImagePoolFromIni(const char* strFileName);
    //创建轨迹池
    bool LoadTrackPoolFromIni(const char* strFileName);
    //创建单体模板池
    bool LoadSingleTypeFromIni(const char* strFileName);
    //清空所有的池

    void ClearAllPool();
    // --------------------------------------------------------------
    
    PERSISTID GetSpriteID(unsigned int index);

    int GetSprite(const char* name);

    bool SetKeyword(const char* pKeyword);

    bool LoadResource();

    unsigned int GetMaxPool();
    void SetMaxPool(unsigned int);

    float GetOffsetY(void);
    void SetOffsetY(float);

    float GetOffsetX(void);
    void SetOffsetX(float);

    unsigned int GetPoolSize();

    IEntity* GetGlobalEntity(const char*);
    IEntity* GetEntity(PERSISTID id);
    IEntity* QueryCustomEntity(IEntity* pEntity, const char* name);
    const char* QueryCustomString(IEntity* pEntity, const char* name);

    bool ShowBallFormModelPos(const char* index, const char* imagetext, 
        PERSISTID target, const char* text = "");
    bool ShowBallFormScreenPos(const char* index, const char* imagetext, 
        long pos_x, long pos_y, const char* text = "", bool bAlign = false,
        PERSISTID target = PERSISTID());

    // 获取人物模型在屏幕上的坐标
    bool GetScreenPos(PERSISTID target, const float offsetY, 
        const float offsetX, int& screen_x, int& screen_y);

    bool ReloadIni();

    long SingleWidth(const char* index, const char*);
    long SingleHeight(const char* index, const char*);

    Tracklogic* GetTracklogic(const char* index);
    CImageInfo* GetImageInfo(const char* index);
    SingleType* GetSingleType(const char* index);

    // 子特效动画结束时调用
    void OnSpriteShowEnd(const int sprite_index);
    void OnSpriteDestroy(const int sprite_index);

private:
    struct SpriteInfo 
    {
        std::string strIndex;
        std::string strImageText;
        std::string strText;
        //std::string target_ident;
        PERSISTID target;
        float f_time;            // 等待超时时间
        long lPosX;
        long lPosY;
        bool bAlign;
    };

private:
    void ClearTrackPool();
    void ClearImagePool();
    void ClearSingleTypePool();
    void ClearSpritePool();
    void ClearSpriteList();
    bool CanShowNew(const char* name, long& cur_time, int& sprite_index);
    bool AddWaitSpriteItem(const char* index, const char* imagetext, 
		long pos_x, long pos_y, const char* text, bool bAlign, 
		PERSISTID target, const long cur_time);
    bool ShowSpriteItemNow(const char* index, const char* imagetext, 
		long pos_x, long pos_y, const char* text, bool bAlign, 
		PERSISTID target, const int sprite_index, const long cur_time);

private:
    unsigned int m_lMaxPool; //现在至当前最大的数量
    float m_fOffsetY;
    float m_fOffsetX;

    // 预载的数据中间不会改变
    //std::map<std::string, Tracklogic*>      m_mapTrackPool;               //所有轨迹的信息
    //std::map<std::string, IMAGEINFO*>       m_mapImagePool;               //每个文字对应的图片
    //std::map<std::string, SingleType*>      m_mapSingleTypePool;          //单体类型池
	TStringPod<char, Tracklogic*, TStringTraits<char>,
		TCoreAlloc> m_mapTrackPool; //所有轨迹的信息
	TStringPod<char, CImageInfo*, TStringTraits<char>,
		TCoreAlloc> m_mapImagePool; //每个文字对应的图片
	TStringPod<char, SingleType*, TStringTraits<char>,
		TCoreAlloc> m_mapSingleTypePool; //单体类型池

    // 动态添加的, 
    TArrayPod<PERSISTID, 1, TCoreAlloc> m_vSpritePool;
    TArrayPod<size_t, 1, TCoreAlloc> m_vSpritePoolFree; //空闲队列
    TArrayPod<size_t, 1, TCoreAlloc> m_vSpritePoolUse; //当前正在使用的队列

    //std::map<std::string, size_t>           m_mapSingletonSprite;         //单件专用
    //std::map<size_t, bool>                  m_mapSingletonFree;           //判断单件是否正在使用
	TStringPod<char, size_t, TStringTraits<char>, 
		TCoreAlloc> m_mapSingletonSprite; //单件专用
	TPodHashMap<size_t, bool, TPodTraits<size_t>, 
		TCoreAlloc> m_mapSingletonFree; //判断单件是否正在使用

    std::string m_StrKeyword;                 //有四个值“up”“down”“arc”“normal”

    // 实际的控制飘字待处理队列
    TArrayPod<SpriteInfo*, 1, TCoreAlloc> m_vSpriteList; //当前实际的飘字队列

    // 显示规则控制
    bool m_bAddedExecute;
    long m_lLastTime;
    bool m_bDestroy; // 是否正在删除自身

};

#endif
