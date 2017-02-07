
#include "spritemanager.h"
#include "../tools/string_util.h"
#include "../visual/vis_utils.h"
#include "../public/core_log.h"
#include "../public/var_list.h"
#include "../public/module.h"
#include "../public/i_ini_file.h"
#include "../public/portable.h"
#include "i_container.h"
#include "../visual/i_camera.h"
#include "../visual/i_scene.h"
#include "../visual/i_vis_base.h"

#ifdef WIN32
#pragma warning(disable : 4101 4244 4018)
#endif

DECLARE_ENTITY(SpriteManager, 0, IEntity);

#if 0
DECLARE_METHOD_1(PERSISTID, SpriteManager, GetSpriteID, unsigned int)

DECLARE_METHOD_0(bool, SpriteManager, LoadResource)

DECLARE_METHOD_1(bool, SpriteManager , SetKeyword, const char * )
#endif

/// property: int PoolSize
/// \brief 最大内存池
DECLARE_PROPERTY(unsigned int, SpriteManager, MaxPool, GetMaxPool, SetMaxPool);

/// property: float Offset
/// \brief 相对人物头顶偏移的值
DECLARE_PROPERTY(float, SpriteManager, OffsetY, GetOffsetY, SetOffsetY);

/// property: float Offset
/// \brief 相对人物头顶偏移的值
DECLARE_PROPERTY(float, SpriteManager, OffsetX, GetOffsetX, SetOffsetX);

/// property: int PoolSize
/// \brief 当前池的长度
DECLARE_PROPERTY_GET(unsigned int, SpriteManager, PoolSize, GetPoolSize);

#if 0
/// \brief 设置某个格子的图片(必须要保证当前界面没有字在飘)
/// \method bool ReloadIni()
/// \return true
DECLARE_METHOD_0(bool, SpriteManager, ReloadIni);

/// \method bool ShowBallFormScreenPos(const char * index, const char * imagetext,
///         long pos_x, long pos_y, const char * text)
/// \param index 格子序号
/// \param imagetext 使用图片替换的文字
/// \param pos_x 显示在屏幕上的坐标
/// \param pos_y 显示在屏幕上的坐标
/// \param text 控件显示的文字
/// \return true
DECLARE_METHOD_5(bool, SpriteManager, ShowBallFormScreenPos, const char *, 
    const char * , long ,long , const char * ,bool);

/// \brief 显示飘字
/// \method bool ShowBallFormModelPos(const char * index, const char * imagetext, 
///        const char * target_ident, const char * text = "")
/// \param index 飘字ID
/// \param imagetext 使用图片替换的文字
/// \param target_ident 显示目标对象
/// \param text 控件显示的文字
/// \return true
DECLARE_METHOD_4(bool, SpriteManager, ShowBallFormModelPos, const char*, 
    const char*, const char*, const char*);
#endif

#define TIMESIZE 30
#define MAXTIMESIZE 5000

// 分割文件名和扩展名
SpriteManager::SpriteManager()
{
    m_bAddedExecute = false;
    m_bDestroy = false;
    m_StrKeyword = "normal";
    m_lMaxPool = 20;
    m_fOffsetY = 1.0f;
    m_fOffsetX = 1.0f;
}

SpriteManager::~SpriteManager()
{
    m_bDestroy = true;
    m_StrKeyword = "";
    ClearAllPool();
}

// 初始化（可创建其他附属实体）
bool SpriteManager::Init(const IVarList& args)
{
    m_lLastTime = 0;
    //GetCore()->AddExecute(this);
    return true;
}

// 关闭（可删除其他引用的附属实体，而资源释放应在析构函数中进行）
bool SpriteManager::Shut()
{
    m_bDestroy = true;
    ClearAllPool();
    return true;
}

// 每帧调用（如果已添加到运行队列）
void SpriteManager::Execute(float seconds)
{
    long cur_time = Port_GetTickCount();

    // 检查等待队列---------------------------------------------------
    //std::vector<SpriteInfo*>::iterator it = m_vSpriteList.begin();
    //std::vector<SpriteInfo*>::iterator it_end = m_vSpriteList.end();
	size_t k = 0;

    // 有效性检查
    //while (it != it_end)
	while (k < m_vSpriteList.size())
    {
        //SpriteInfo* pSpriteInfo = *it;
		SpriteInfo* pSpriteInfo = m_vSpriteList[k];

        if (cur_time - pSpriteInfo->f_time > MAXTIMESIZE)
        {
            // 超时则直接删除
            //CORE_DELETE(*it);
            //it = m_vSpriteList.erase(it);
            //it_end = m_vSpriteList.end();
			CORE_DELETE(pSpriteInfo);
			m_vSpriteList.remove(k);
            continue;
        }

        // 检查依赖对像是否存在，不存在则删除
        if (pSpriteInfo->bAlign)
        {
            PERSISTID vis_object = pSpriteInfo->target;

            //GameVisual* pGameVisual = (GameVisual*)GetGlobalEntity("game_visual");
            //
            //if (pGameVisual)
            //{
            //    vis_object = pGameVisual->GetSceneObj(pSpriteInfo->target_ident.c_str());
            //}

            if (vis_object.IsNull())
            {
                //CORE_DELETE(*it);
                //it = m_vSpriteList.erase(it);
                //it_end = m_vSpriteList.end();
				CORE_DELETE(pSpriteInfo);
				m_vSpriteList.remove(k);
                continue;
            }
        }

        //++it;
		++k;
    }

    //it = m_vSpriteList.begin();
    //it_end = m_vSpriteList.end();

    //if(it != it_end)
	if (!m_vSpriteList.empty())
    {
        CVarList result;
        
        // 检查显示间隔，是否可以加一个显示
        if ((cur_time - m_lLastTime) > TIMESIZE)
        {
            //SpriteInfo* pSpriteInfo = *it;
			SpriteInfo* pSpriteInfo = m_vSpriteList[0];
            int sprite_index = GetSprite(pSpriteInfo->strIndex.c_str());

            // 找不到没有空闲的显示控件
            if (sprite_index < 0)
            {
                return;
            }

            // 有空闲的格子, 则直接显示
            if (ShowSpriteItemNow(pSpriteInfo->strIndex.c_str(), 
				pSpriteInfo->strImageText.c_str(), 
                pSpriteInfo->lPosX, pSpriteInfo->lPosY, 
                pSpriteInfo->strText.c_str(), 
                pSpriteInfo->bAlign, 
                pSpriteInfo->target, 
                sprite_index, cur_time))
            {
            }

            //CORE_DELETE(*it);
            //m_vSpriteList.erase(it);
			CORE_DELETE(pSpriteInfo);
			m_vSpriteList.remove(0);
        }
    }
    else
    {
        GetCore()->RemoveExecute(this);
        m_bAddedExecute = false;
    }
}

//
bool SpriteManager::ReloadIni()
{
    ClearAllPool();
    LoadResource();
    return true;
}

//
bool SpriteManager::LoadResource()
{
    LoadImagePoolFromIni("ini\\ui\\fightword\\WordImage.ini");
    //创建轨迹池
    LoadTrackPoolFromIni("ini\\ui\\fightword\\BalloonTrack.ini");
    //创建单体模板池
    LoadSingleTypeFromIni("ini\\ui\\fightword\\fightword.ini");
    return true;
}

//
bool SpriteManager::LoadImagePoolFromIni(const char * strFileName)
{
    std::string path = GetCore()->GetResourcePath();
    
	path = path + strFileName;
    
	IIniFile* pIni = GetCore()->CreateIniFile(path.c_str());

    if (!pIni->LoadFromFile())
	{
		pIni->Release();      
		return false;
	}

    ClearImagePool();

    const char* szSectionName = "";

    for(int i = 0; i < pIni->GetSectionCount(); i++)
    {
        szSectionName = pIni->GetSectionByIndex(i);

        //std::map<std::string, IMAGEINFO*>::iterator it = 
		//	m_mapImagePool.find(szSectionName);
        CImageInfo* pimageinfo = NULL;
        bool create_new = false;

        //if (it != m_mapImagePool.end())
		if (m_mapImagePool.GetData(szSectionName, pimageinfo))
        {
            //pimageinfo = it->second;
			std::string szInfo = "ERROR seciont same : ";
            
			szInfo = szInfo + szSectionName + " file:" + strFileName;
            CORE_TRACE(szInfo.c_str());
        }
        else
        {
            create_new = true;
            pimageinfo = CORE_NEW(CImageInfo);
        }

        //(*pimageinfo)['0'] = pIni->ReadString(i, "char_0", "");
        //(*pimageinfo)['1'] = pIni->ReadString(i, "char_1", "");
        //(*pimageinfo)['2'] = pIni->ReadString(i, "char_2", "");
        //(*pimageinfo)['3'] = pIni->ReadString(i, "char_3", "");
        //(*pimageinfo)['4'] = pIni->ReadString(i, "char_4", "");
        //(*pimageinfo)['5'] = pIni->ReadString(i, "char_5", "");
        //(*pimageinfo)['6'] = pIni->ReadString(i, "char_6", "");
        //(*pimageinfo)['7'] = pIni->ReadString(i, "char_7", "");
        //(*pimageinfo)['8'] = pIni->ReadString(i, "char_8", "");
        //(*pimageinfo)['9'] = pIni->ReadString(i, "char_9", "");
        //(*pimageinfo)['+'] = pIni->ReadString(i, "char_+", "");
        //(*pimageinfo)['-'] = pIni->ReadString(i, "char_-", "");
		pimageinfo->AddName('0', pIni->ReadString(i, "char_0", ""));
		pimageinfo->AddName('1', pIni->ReadString(i, "char_1", ""));
		pimageinfo->AddName('2', pIni->ReadString(i, "char_2", ""));
		pimageinfo->AddName('3', pIni->ReadString(i, "char_3", ""));
		pimageinfo->AddName('4', pIni->ReadString(i, "char_4", ""));
		pimageinfo->AddName('5', pIni->ReadString(i, "char_5", ""));
		pimageinfo->AddName('6', pIni->ReadString(i, "char_6", ""));
		pimageinfo->AddName('7', pIni->ReadString(i, "char_7", ""));
		pimageinfo->AddName('8', pIni->ReadString(i, "char_8", ""));
		pimageinfo->AddName('9', pIni->ReadString(i, "char_9", ""));
		pimageinfo->AddName('+', pIni->ReadString(i, "char_+", ""));
		pimageinfo->AddName('-', pIni->ReadString(i, "char_-", ""));

        if (create_new)
        {
            //m_mapImagePool[szSectionName] = pimageinfo;
			m_mapImagePool.Add(szSectionName, pimageinfo);
        }
    }

	pIni->Release();

    return true;
}

//
bool SpriteManager::LoadTrackPoolFromIni(const char * strFileName)
{
    std::string path = GetCore()->GetResourcePath();
    
	path = path + strFileName;
    
	IIniFile* pIni = GetCore()->CreateIniFile(path.c_str());

    if (!pIni->LoadFromFile())
	{
        pIni->Release();
		//读取ini失败了啊！
        return false;
	}

    ClearTrackPool();

    const char* SectionName = "";

    for (int i = 0; i < pIni->GetSectionCount(); i++)
    {
        SectionName = pIni->GetSectionByIndex(i);

        //std::map<std::string, Tracklogic*>::iterator it = 
		//	m_mapTrackPool.find(SectionName);
        Tracklogic* pTrackInfo = NULL;
        bool create_new = false;

        //if (it != m_mapTrackPool.end())
		if (m_mapTrackPool.GetData(SectionName, pTrackInfo))
        {
            //pTrackInfo = it->second;
            
			std::string szInfo = "ERROR same section ";
            
			szInfo = szInfo + SectionName + " file:" + strFileName;
            CORE_TRACE(szInfo.c_str());
        }
        else
        {
            create_new = true;
            pTrackInfo = CORE_NEW(Tracklogic);
        }

        pTrackInfo->m_dStartColor = VisUtil_ColorToInt(
			pIni->ReadString(i,"start_color","255,255,255,255"));
        pTrackInfo->m_dEndColor = VisUtil_ColorToInt(
			pIni->ReadString(i,"end_color","255,255,255,255"));
        pTrackInfo->m_fBackH = pIni->ReadInteger(i,"start_h",0);
        pTrackInfo->m_fBackW = pIni->ReadInteger(i,"start_w",0);
        pTrackInfo->m_fBackX = pIni->ReadFloat(i,"start_x",0);
        pTrackInfo->m_fBackY = pIni->ReadFloat(i,"start_y",0);
        pTrackInfo->m_StrFont = pIni->ReadString(i,"FontName","FZHKJT48");
        pTrackInfo->m_fMoveSpeedX = pIni->ReadInteger(i,"x_speed",0);
        pTrackInfo->m_fMoveSpeedY = pIni->ReadInteger(i,"y_speed",0);
        pTrackInfo->m_fMoveSpeedAX = pIni->ReadInteger(i,"ax_speed",0);
        pTrackInfo->m_fMoveSpeedAY = pIni->ReadInteger(i,"ay_speed",0);
        pTrackInfo->m_fMoveTime = pIni->ReadFloat(i,"movetime",0);
        pTrackInfo->m_fStaticTime = pIni->ReadFloat(i,"stoptime",0.0);
        pTrackInfo->m_fOffsetY = pIni->ReadFloat(i,"OffsetY",0.8f);
        pTrackInfo->m_fStaticScaleSpeedW = pIni->ReadInteger(i,"w_stop_speed",0);
        pTrackInfo->m_fStaticScaleSpeedH = pIni->ReadInteger(i,"h_stop_speed",0);
        pTrackInfo->m_fMoveScaleSpeedW = pIni->ReadInteger(i,"w_speed",0);
        pTrackInfo->m_fMoveScaleSpeedH = pIni->ReadInteger(i,"h_speed",0);
        pTrackInfo->m_fROffsetY = pIni->ReadInteger(i,"r_offset_y",0);
        pTrackInfo->m_fROffsetX = pIni->ReadInteger(i,"r_offset_x",0);
        pTrackInfo->m_fColorTime = pIni->ReadFloat(i, "colorTime",0.0f);
        pTrackInfo->m_bIsStop = 
			((1 == pIni->ReadInteger(i, "is_stop", 0)) ? false : true);

        if (create_new)
        {
            //m_mapTrackPool[SectionName] = pTrackInfo;
			m_mapTrackPool.Add(SectionName, pTrackInfo);
        }
    }

	pIni->Release();
    
    return true;
}

//
bool SpriteManager::LoadSingleTypeFromIni(const char * strFileName)
{
    std::string path = GetCore()->GetResourcePath();
    
	path = path + strFileName;
    
	IIniFile* pIni = GetCore()->CreateIniFile(path.c_str());

    if (!pIni->LoadFromFile())
	{
		pIni->Release();      
		return false;
	}

    ClearSingleTypePool();

    const char* szSectionName = "";

    for (int i = 0; i < pIni->GetSectionCount(); i++)
    {
        szSectionName = pIni->GetSectionByIndex(i);

        SingleType* pSingleTypeObj = NULL;
        //std::map<std::string, SingleType*>::iterator it = 
        //    m_mapSingleTypePool.find(szSectionName);
        bool create_new = false;

        //if (it != m_mapSingleTypePool.end())
		if (m_mapSingleTypePool.GetData(szSectionName, pSingleTypeObj))
        {
            //pSingleTypeObj = it->second;
            
			std::string szInfo = "ERROR same section ";
            
			szInfo = szInfo + szSectionName + " file:" + strFileName;
            CORE_TRACE(szInfo.c_str());
        }
        else
        {
            create_new = true;
            pSingleTypeObj = CORE_NEW(SingleType);
        }

        pSingleTypeObj->StrTrackMode = pIni->ReadString(i, "TrackMode", "");
        pSingleTypeObj->iBackImageW = pIni->ReadInteger(i, "BackImageW", 0);
        pSingleTypeObj->iBackImageH = pIni->ReadInteger(i, "BackImageH", 0);
        pSingleTypeObj->FontOffsetX = pIni->ReadInteger(i, "FontOffsetX", 0);
        pSingleTypeObj->FontOffsetY = pIni->ReadInteger(i, "FontOffsetY", 0);
        pSingleTypeObj->StrFontImage = pIni->ReadString(i, "font", "");
        pSingleTypeObj->StrBackImage = pIni->ReadString(i, "BackImage", "");
        pSingleTypeObj->OffsetX = pIni->ReadInteger(i, "offset", 0);
        //决定是从玩家头顶出来还是直接从窗口位置出来
        pSingleTypeObj->bIsScreenPos = 
			((1 == pIni->ReadInteger(i, "IsObjPos", 1)) ? false : true);
        pSingleTypeObj->singleton = 
			((1 == pIni->ReadInteger(i, "singleton", 0))? true : false);

        if (create_new)
        {
            //m_mapSingleTypePool[szSectionName] = pSingleTypeObj;
			m_mapSingleTypePool.Add(szSectionName, pSingleTypeObj);
        }
    }

	pIni->Release();

    return true;
}

//
void SpriteManager::ClearAllPool()
{
    ClearTrackPool();
    ClearImagePool();
    ClearSingleTypePool();
    ClearSpritePool();
    ClearSpriteList();
}

void SpriteManager::ClearTrackPool()
{
    //std::map<std::string, Tracklogic*>::iterator it = m_mapTrackPool.begin();
	TStringPod<char, Tracklogic*, TStringTraits<char>, 
		TCoreAlloc>::iterator it = m_mapTrackPool.Begin();

    //while(it != m_mapTrackPool.end())
	while(it != m_mapTrackPool.End())
    {
        //CORE_DELETE(it->second);
		CORE_DELETE(it.GetData());
        ++it;
    }

    //m_mapTrackPool.clear();
	m_mapTrackPool.Clear();
}

void SpriteManager::ClearImagePool()
{
    //std::map<std::string, IMAGEINFO*>::iterator it = m_mapImagePool.begin();
	TStringPod<char, CImageInfo*, TStringTraits<char>, 
		TCoreAlloc>::iterator it = m_mapImagePool.Begin();

    //while(it != m_mapImagePool.end())
	while(it != m_mapImagePool.End())
    {
        //CORE_DELETE(it->second);
		CORE_DELETE(it.GetData());
        ++it;
    }

    //m_mapImagePool.clear();
	m_mapImagePool.Clear();
}

void SpriteManager::ClearSingleTypePool()
{
    //std::map<std::string, SingleType*>::iterator it = m_mapSingleTypePool.begin();
	TStringPod<char, SingleType*, TStringTraits<char>, 
		TCoreAlloc>::iterator it = m_mapSingleTypePool.Begin();

    //while(it != m_mapSingleTypePool.end())
	while(it != m_mapSingleTypePool.End())
    {
		//CORE_DELETE(it->second);
        CORE_DELETE(it.GetData());
        ++it;
    }

    //m_mapSingleTypePool.clear();
	m_mapSingleTypePool.Clear();
}

void SpriteManager::ClearSpritePool()
{
    //std::vector<PERSISTID>::iterator it = m_vSpritePool.begin();
	size_t k = 0;

    //while(it != m_vSpritePool.end())
	while (k < m_vSpritePool.size())
    {
        //GetCore()->DeleteEntity(*it);
        //++it;
		GetCore()->DeleteEntity(m_vSpritePool[k]);
		++k;
    }

    m_vSpritePool.clear();
    m_vSpritePoolFree.clear();
    m_vSpritePoolUse.clear();
    //m_mapSingletonSprite.clear();
    //m_mapSingletonFree.clear();
	m_mapSingletonSprite.Clear();
	m_mapSingletonFree.Clear();
}

void SpriteManager::ClearSpriteList()
{
    //std::vector<SpriteInfo*>::iterator it = m_vSpriteList.begin();
	size_t k = 0;

    //while(it != m_vSpriteList.end())
	while (k < m_vSpriteList.size())
    {
        //CORE_DELETE(*it);
        //++it;
		CORE_DELETE(m_vSpriteList[k]);
		++k;
    }

    m_vSpriteList.clear();
}

//
float SpriteManager::GetOffsetY(void)
{
    return m_fOffsetY;
}

//
void SpriteManager::SetOffsetY(float fOffset)
{
    m_fOffsetY = fOffset;
}

//
float SpriteManager::GetOffsetX(void)
{
    return m_fOffsetX;
}

//
void SpriteManager::SetOffsetX(float fOffset)
{
    m_fOffsetX = fOffset;
}

//
SingleType* SpriteManager::GetSingleType(const char* index)
{
    SingleType* pSingleType;

	if (!m_mapSingleTypePool.GetData(index, pSingleType))
	{
		return NULL;
	}

	return pSingleType;
//	std::map<std::string, SingleType*>::iterator it = 
//        m_mapSingleTypePool.find(index);
//
//    //找出事先配置好的类型
//    if (it != m_mapSingleTypePool.end())
//    {
//        return (it->second);
//    }
//
//    return NULL;
}

//
Tracklogic* SpriteManager::GetTracklogic(const char * index)
{
    std::string szStrTrackName = std::string(index) + "_" + m_StrKeyword;
    Tracklogic* pTrackInfo;

	if (!m_mapTrackPool.GetData(szStrTrackName.c_str(), pTrackInfo))
	{
		return NULL;
	}

	return pTrackInfo;
//	std::map<std::string,Tracklogic*>::iterator it = m_mapTrackPool.find(szStrTrackName);
//
//    if (it != m_mapTrackPool.end())
//    {
//        return (it->second);
//    }
//
//    return NULL;
}

//
CImageInfo* SpriteManager::GetImageInfo(const char* index)
{
    CImageInfo* pImageInfo;

	if (!m_mapImagePool.GetData(index, pImageInfo))
	{
		return NULL;
	}

	return pImageInfo;
//	std::map<std::string,IMAGEINFO*>::iterator it = m_mapImagePool.find(index);
//
//    if (it != m_mapImagePool.end())
//    {
//        return (it->second);
//    }
//
//    return NULL;
}

//
PERSISTID SpriteManager::GetSpriteID(unsigned int index)
{
    if (size_t(index) >= m_vSpritePool.size())
    {
        return PERSISTID();
    }

    return (m_vSpritePool[index]);
}

//
bool SpriteManager::SetKeyword(const char* pKeyword)
{
    m_StrKeyword  = pKeyword;
    return true;
}

//
void SpriteManager::SetMaxPool(unsigned int MaxPool)
{
    m_lMaxPool = MaxPool;
}

//
unsigned int SpriteManager::GetMaxPool()
{
    return m_lMaxPool;
}


//
unsigned int SpriteManager::GetPoolSize()
{
    return m_vSpritePool.size();
}


//
bool SpriteManager::ShowBallFormModelPos(const char* index, 
    const char* imagetext, PERSISTID target, const char* text)
{
    if (m_bDestroy)
    {
        // 正在自我删除，不再处理
        return false;
    }

    int pos_x, pos_y;

    GetScreenPos(target, m_fOffsetY, m_fOffsetX, pos_x, pos_y);
    ShowBallFormScreenPos(index, imagetext, pos_x, pos_y, text, true, 
		target);

    return true;
}

bool SpriteManager::CanShowNew(const char* name, long& cur_time, int& sprite_index)
{
    cur_time = Port_GetTickCount();
	
	SingleType* pSingle = GetSingleType(name);

	if (pSingle && !pSingle->singleton)
	{
		// 间隔如果过短，则先不正式创建显示，
		if ((cur_time - m_lLastTime) < TIMESIZE)
		{
			return false;
		}
	}

    sprite_index = GetSprite(name);

    if (sprite_index >= 0)
    {
        return true;
    }

    return false;
}

bool SpriteManager::AddWaitSpriteItem(const char* index, const char* imagetext, 
	long pos_x, long pos_y, const char* text, bool bAlign,
    PERSISTID target, long cur_time)
{
    SpriteInfo * pInfo = CORE_NEW(SpriteInfo);

    if (NULL == pInfo)
    {
        return false;
    }

    pInfo->strIndex = index;
    pInfo->strImageText = imagetext;
    pInfo->strText = text;
    pInfo->lPosX = pos_x;
    pInfo->lPosY = pos_y;
    pInfo->bAlign = bAlign;
    pInfo->target = target;
    pInfo->f_time = cur_time;

    m_vSpriteList.push_back(pInfo);
    
    if (!m_bAddedExecute)
    {
        m_bAddedExecute = GetCore()->AddExecute(this);
    }

    return true;
}

bool SpriteManager::ShowSpriteItemNow(const char* name, const char* imagetext, 
	long pos_x, long pos_y, const char* text, bool bAlign, 
	PERSISTID target, const int sprite_index, const long cur_time)
{
    // >最小间隔，重置间隔时间
    m_lLastTime = cur_time;

    Gui* pGui = (Gui*)GetCore()->LookupEntity("Gui");

    if (NULL == pGui)
    {
        return false;
    }

    if (sprite_index < 0 || sprite_index >= m_vSpritePool.size())
    {
        return false;
    }

    PERSISTID sprite_id = m_vSpritePool[sprite_index];
    Sprite* pEntity = (Sprite*)GetEntity(sprite_id);
	SingleType* pSingleTypeInfo = GetSingleType(name);

	if (NULL == pSingleTypeInfo)
	{
		return false;
	}

    if (NULL == pEntity)
    {
        // 意外被删除，则重新创建

		int index = GetSprite(name);
		sprite_id = m_vSpritePool[index];
        pEntity = (Sprite*)GetEntity(sprite_id);

        if (NULL == pEntity)
        {
            return false;
        }
    }

    if (!pEntity->GetEntInfo()->IsKindOf("IControl"))
    {
        return false;
    }

    Tracklogic* pTrackMode = GetTracklogic(pSingleTypeInfo->StrTrackMode.c_str());
    CImageInfo* pImageFont = GetImageInfo(pSingleTypeInfo->StrFontImage.c_str());

    if (NULL == pTrackMode || NULL == pImageFont)
    {
        return 0;
    }

    int offset_y = 0;
    int offset_x = 0;

    if (0 < pTrackMode->m_fROffsetX)
    {
        offset_x = (rand() % int(pTrackMode->m_fROffsetX)) - (pTrackMode->m_fROffsetX / 2); 
    }

    if (0 < pTrackMode->m_fROffsetY)
    {
        offset_y = (rand() % int(pTrackMode->m_fROffsetY)) - (pTrackMode->m_fROffsetY / 2);
    }

    //CVarList result;
    //CVarList param;
    //
    //param.AddInt((int)pSingleTypeInfo);
    //param.AddInt((int)pTrackMode);
    //param.AddInt((int)pImageFont);
    //param.AddString(pImageText);
    //param.AddString(pText);

    if (bAlign)
    {
        offset_x -= (SingleWidth(name, imagetext) / 2 + pTrackMode->m_fBackW);
        offset_y -= (SingleHeight(name, imagetext) + pTrackMode->m_fBackH);
    }

    //// 不存在于列表中则添加
    pEntity->InitParam(pSingleTypeInfo, pTrackMode, pImageFont, imagetext, 
        text, pos_x + offset_x + m_fOffsetX, pos_y + offset_y + m_fOffsetY);

    PERSISTID desktop = pGui->GetDesktopID();
    IContainer* pDeskTopForm = (IContainer*)GetEntity(desktop);

    if (pDeskTopForm)
    {
        pDeskTopForm->Add(sprite_id);

        return true;
    }

    return false;
}

//
bool SpriteManager::ShowBallFormScreenPos(const char* index, const char* imagetext, 
	long pos_x, long pos_y, const char* text, bool bAlign, PERSISTID target)
{
    if (m_bDestroy)
    {
        // 正在自我删除，不再处理
        return false;
    }

    Gui* pGui = (Gui*)GetCore()->LookupEntity("Gui");

    if (NULL == pGui)
    {
        return false;
    }

    int sprite_index = -1;
    long cur_time = 0;

    if (CanShowNew(index, cur_time, sprite_index))
    {
        // 可以直接显示
        return ShowSpriteItemNow(index, imagetext, pos_x, pos_y, text, 
			bAlign, target, sprite_index, cur_time);
    }
    else
    {
        // 不可直接显示，放入缓存队列
        return AddWaitSpriteItem(index, imagetext, pos_x, pos_y, text, 
			bAlign, target, cur_time);
    }
}

//
IEntity* SpriteManager::GetGlobalEntity(const char* name)
{
    IVar& v = g_pCore->GetGlobalValue(name);

    if (v.GetType() != VTYPE_OBJECT)
    {
        return NULL;
    }

    return (Gui*)GetEntity(v.ObjectVal());
}

// 只有立即可添加到显示队列时才调用此函数
int SpriteManager::GetSprite(const char* name)
{
    Gui* pGui = (Gui*)GetCore()->LookupEntity("Gui");

    if (pGui == NULL)
    {
        return -1;
    }

    SingleType* pSingle = GetSingleType(name);

    if (!pSingle)
    {
        return -1;
    }

    // 获取空闲的，如果没有空闲，则创建新的
    bool singleton = pSingle->singleton;

    // 检查是否存在
    if (singleton)
    {
        // 单件情况
        //std::map<std::string, size_t>::iterator singleton_it = 
		//	m_mapSingletonSprite.find(name);
		size_t sprite_index;

        //if (singleton_it != m_mapSingletonSprite.end())
		if (m_mapSingletonSprite.GetData(name, sprite_index))
        {
            // 找到了单件对应的控件序号
            //int sprite_index = singleton_it->second;
            //std::map<size_t, bool>::iterator it = m_mapSingletonFree.find(sprite_index);
			bool is_free;

            //if (it != m_mapSingletonFree.end())
			if (m_mapSingletonFree.GetData(sprite_index, is_free))
            {
                //if (it->second)
				if (is_free)
                {
                    // 正空闲，则直接使用
                    //m_mapSingletonFree[sprite_index] = false;
					m_mapSingletonFree.Set(sprite_index, false);
                }
            }

             return sprite_index;
        }
        else
        {
            // 单件暂时还不存在
            // 不存在情况下，获取一个空闲对像
            if (m_vSpritePoolFree.size() > 0)
            {
                int sprite_index = m_vSpritePoolFree[0];

                //m_vSpritePoolFree.erase(m_vSpritePoolFree.begin());
				m_vSpritePoolFree.remove(0);
                // 单件不和其他共享
                //m_mapSingletonSprite.insert(std::make_pair(name, sprite_index));
                //m_mapSingletonFree[sprite_index] = false;
				m_mapSingletonSprite.Set(name, sprite_index);
				m_mapSingletonFree.Set(sprite_index, false);
                return sprite_index;
            }

            // 空闲对像不存在，则尝试创建
            //if (GetPoolSize() < GetMaxPool()) // 单件不受大小限制
            //{
                PERSISTID sprite_id = pGui->Create("Sprite");
                Sprite* pSprite = (Sprite*)GetEntity(sprite_id);
                char buf[32];
                size_t sprite_index = m_vSpritePool.size();

                SafeSprintf(buf, sizeof(buf), "Name%d", sprite_index);

                pSprite->SetName(buf);
                pSprite->SetParentInfo(this, sprite_index);

                m_vSpritePool.push_back(sprite_id);

                // 单件不和其他共享
                //m_mapSingletonSprite.insert(std::make_pair(name, sprite_index));
                //m_mapSingletonFree[sprite_index] = false;
				m_mapSingletonSprite.Set(name, sprite_index);
				m_mapSingletonFree.Set(sprite_index, false);
                return sprite_index;
            //}
        }

        return -1;
    }

    // 非单件

    // 不存在情况下，获取一个空闲对像
    if (m_vSpritePoolFree.size() > 0)
    {
        int sprite_index = m_vSpritePoolFree[0];

        //m_vSpritePoolFree.erase(m_vSpritePoolFree.begin());
		m_vSpritePoolFree.remove(0);
        m_vSpritePoolUse.push_back(sprite_index);
        return sprite_index;
    }

    // 空闲对像不存在，则尝试创建
    if (GetPoolSize() < GetMaxPool())
    {
        Gui* pGui = (Gui*)GetCore()->LookupEntity("Gui");

        if (pGui != NULL)
        {
            PERSISTID sprite_id = pGui->Create("Sprite");
            Sprite* pSprite = (Sprite*)GetEntity(sprite_id);
            char buf[32];
            size_t sprite_index = m_vSpritePool.size();

            SafeSprintf(buf, sizeof(buf), "Name%d", sprite_index);

            pSprite->SetName(buf);
            pSprite->SetParentInfo(this, sprite_index);
            m_vSpritePool.push_back(sprite_id);
            m_vSpritePoolUse.push_back(sprite_index);
            return sprite_index;
        }
    }

    return -1;
}

//
// 获取人物模型在屏幕上的坐标
bool SpriteManager::GetScreenPos(PERSISTID target, 
    const float offsetY, const float offsetX, int & screen_x, 
    int & screen_y)
{
    PERSISTID vis_object = target;

    //GameVisual* pGameVisual = (GameVisual*)GetGlobalEntity("game_visual");

    //if (NULL != pGameVisual)
    //{
    //    vis_object = pGameVisual->GetSceneObj(target_ident);
    //}

    if (vis_object.IsNull())
    {
        screen_x = -200;
        screen_y = -200;
        return false;
    }

    IVisBase * pVisBase = (IVisBase *)GetEntity(vis_object);

    if (pVisBase == NULL)
    {
        return false;
    }

    IScene * pScene = (IScene*)QueryCustomEntity(pVisBase, "scene");

    if (NULL == pScene)
    {
        return NULL;
    }

    ICamera* pCamera = pScene->GetCamera();

    if (pCamera == NULL)
    {
        return false;
    }

    FmVec3 pos = pVisBase->GetCenter();

    FmVec3 box_size = pVisBase->GetBoxSize();

    pos.y = pos.y + box_size.y / 2;

    CVarList result;

    pCamera->GetScreenPos(CVarList()
        << pos.x << pos.y << pos.z, result);

    screen_x = result.IntVal(0);
    screen_y = result.IntVal(1);
    return true;
}

//
long SpriteManager::SingleWidth(const char * index, const char * text)
{
    SingleType * pSingleTypeInfo = GetSingleType(index);

    if (NULL == pSingleTypeInfo)
    {
        return 0;
    }

    Tracklogic * pTrackMode = GetTracklogic(pSingleTypeInfo->StrTrackMode.c_str());

    if (NULL == pTrackMode)
    {
        return 0;
    }

    long lBackImageWidth = pSingleTypeInfo->iBackImageW + pSingleTypeInfo->FontOffsetX;
    std::string strText = text;

    std::string::iterator it = strText.begin();
    std::string::iterator end_it = strText.end();

    long lFontW = (pTrackMode->m_fBackW + pSingleTypeInfo->OffsetX) * strText.size();
    
    long lWidth = lFontW + lBackImageWidth;

    return lWidth;
}


//
long SpriteManager::SingleHeight(const char * index, const char * text)
{
    SingleType * pSingleTypeInfo = GetSingleType(index);

    if (NULL == pSingleTypeInfo)
    {
        return 0;
    }

    Tracklogic * pTrackMode = GetTracklogic(pSingleTypeInfo->StrTrackMode.c_str());

    if (NULL == pTrackMode)
    {
        return 0;
    }

    long lBackImageHeight = pSingleTypeInfo->iBackImageH + pSingleTypeInfo->FontOffsetY;
    long lFontH = pTrackMode->m_fBackH;
    long lHeight = lFontH + lBackImageHeight;

    return lHeight;
}

//
IEntity* SpriteManager::GetEntity(PERSISTID id)
{
    return g_pCore->GetEntity(id);
}

//
IEntity* SpriteManager::QueryCustomEntity(IEntity* pEntity, const char* name)
{
    if (NULL == pEntity)
    {
        return NULL;
    }

    IVar* pVar = pEntity->GetCustoms()->GetValue(name);

	if (NULL == pVar)
	{
		return NULL;
	}

	PERSISTID id = pVar->ObjectVal();

    if (id.IsNull())
    {
        return NULL;
    }

    return g_pCore->GetEntity(id);
}



//
const char* SpriteManager::QueryCustomString(IEntity* pEntity, const char* name)
{
    if (NULL == pEntity)
    {
        return "";
    }

    IVar* pVar = pEntity->GetCustoms()->GetValue(name);

    if (NULL == pVar)
    {
        return "";
    }

    return pVar->StringVal();
}

// 子特效动画结束时调用
void SpriteManager::OnSpriteShowEnd(const int sprite_index)
{
    if (m_bDestroy)
    {
        // 正在自我删除，不再处理
        return;
    }

    //if (m_mapSingletonFree.find(sprite_index) != m_mapSingletonFree.end())
	if (m_mapSingletonFree.Exists(sprite_index))
    {
        // free
        //m_mapSingletonFree[sprite_index] = true;
		m_mapSingletonFree.Set(sprite_index, true);
    }
    else if (sprite_index >= 0 && sprite_index < m_vSpritePool.size())
    {
        size_t count = m_vSpritePoolUse.size();

        for (size_t i = 0; i < count; ++i)
        {
            if (m_vSpritePoolUse[i] == sprite_index)
            {
                //m_vSpritePoolUse.erase(m_vSpritePoolUse.begin() + i);
				m_vSpritePoolUse.remove(i);
                break;
            }
        }

        m_vSpritePoolFree.push_back(sprite_index);
    }
}

void SpriteManager::OnSpriteDestroy(const int sprite_index)
{
    if (m_bDestroy)
    {
        // 是SpriteManager删除导致的删除不处理
        return;
    }

    // 意外被其他界面删除清除使用占用
    OnSpriteShowEnd(sprite_index);

    // 创建新的补上，不补上的话现在也会在获取空闲sprite时自动补上
}