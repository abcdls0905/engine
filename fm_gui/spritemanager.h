
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

    // ��ʼ�����ɴ�����������ʵ�壩
    virtual bool Init(const IVarList& args) ;
    // �رգ���ɾ���������õĸ���ʵ�壬����Դ�ͷ�Ӧ�����������н��У�
    virtual bool Shut() ;

    // ÿ֡���ã��������ӵ����ж��У�
    virtual void Execute(float seconds);

    // �������ݺ�����------------------------------------------------
    //���������
    bool LoadImagePoolFromIni(const char* strFileName);
    //�����켣��
    bool LoadTrackPoolFromIni(const char* strFileName);
    //��������ģ���
    bool LoadSingleTypeFromIni(const char* strFileName);
    //������еĳ�

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

    // ��ȡ����ģ������Ļ�ϵ�����
    bool GetScreenPos(PERSISTID target, const float offsetY, 
        const float offsetX, int& screen_x, int& screen_y);

    bool ReloadIni();

    long SingleWidth(const char* index, const char*);
    long SingleHeight(const char* index, const char*);

    Tracklogic* GetTracklogic(const char* index);
    CImageInfo* GetImageInfo(const char* index);
    SingleType* GetSingleType(const char* index);

    // ����Ч��������ʱ����
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
        float f_time;            // �ȴ���ʱʱ��
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
    unsigned int m_lMaxPool; //��������ǰ��������
    float m_fOffsetY;
    float m_fOffsetX;

    // Ԥ�ص������м䲻��ı�
    //std::map<std::string, Tracklogic*>      m_mapTrackPool;               //���й켣����Ϣ
    //std::map<std::string, IMAGEINFO*>       m_mapImagePool;               //ÿ�����ֶ�Ӧ��ͼƬ
    //std::map<std::string, SingleType*>      m_mapSingleTypePool;          //�������ͳ�
	TStringPod<char, Tracklogic*, TStringTraits<char>,
		TCoreAlloc> m_mapTrackPool; //���й켣����Ϣ
	TStringPod<char, CImageInfo*, TStringTraits<char>,
		TCoreAlloc> m_mapImagePool; //ÿ�����ֶ�Ӧ��ͼƬ
	TStringPod<char, SingleType*, TStringTraits<char>,
		TCoreAlloc> m_mapSingleTypePool; //�������ͳ�

    // ��̬��ӵ�, 
    TArrayPod<PERSISTID, 1, TCoreAlloc> m_vSpritePool;
    TArrayPod<size_t, 1, TCoreAlloc> m_vSpritePoolFree; //���ж���
    TArrayPod<size_t, 1, TCoreAlloc> m_vSpritePoolUse; //��ǰ����ʹ�õĶ���

    //std::map<std::string, size_t>           m_mapSingletonSprite;         //����ר��
    //std::map<size_t, bool>                  m_mapSingletonFree;           //�жϵ����Ƿ�����ʹ��
	TStringPod<char, size_t, TStringTraits<char>, 
		TCoreAlloc> m_mapSingletonSprite; //����ר��
	TPodHashMap<size_t, bool, TPodTraits<size_t>, 
		TCoreAlloc> m_mapSingletonFree; //�жϵ����Ƿ�����ʹ��

    std::string m_StrKeyword;                 //���ĸ�ֵ��up����down����arc����normal��

    // ʵ�ʵĿ���Ʈ�ִ��������
    TArrayPod<SpriteInfo*, 1, TCoreAlloc> m_vSpriteList; //��ǰʵ�ʵ�Ʈ�ֶ���

    // ��ʾ�������
    bool m_bAddedExecute;
    long m_lLastTime;
    bool m_bDestroy; // �Ƿ�����ɾ������

};

#endif
