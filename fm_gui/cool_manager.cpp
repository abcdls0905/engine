//--------------------------------------------------------------------
// 文件名:		CoolManager.h
// 内  容:		冷却管理器
// 说  明:		
// 创建日期:	2008年8月27日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "cool_manager.h"
#include "imageanimation_manager.h"
#include "image_player.h"
//#include "../utils/read_ini.h"
#include "../public/auto_mem.h"
#include "../public/var_list.h"
#include "../public/portable.h"


long CoolItem::s_nAnimationTime = 0;

void CoolItem::RestoreCoolTrangles()
{
	long width = m_nWidth;
	long widthd2 = m_nWidth/2;
	long height = m_nHeight;
	long heightd2 = m_nHeight/2;

	m_vTrangles[0].point[0].x = 0 + widthd2;
	m_vTrangles[0].point[0].y = 0;
	m_vTrangles[0].point[1].x = 0 + width;
	m_vTrangles[0].point[1].y = 0;
	m_vTrangles[0].point[2].x = 0 + widthd2;
	m_vTrangles[0].point[2].y = 0 + heightd2;

	m_vTrangles[1].point[0].x = 0 + width;
	m_vTrangles[1].point[0].y = 0;
	m_vTrangles[1].point[1].x = 0 + width;
	m_vTrangles[1].point[1].y = 0 + height;
	m_vTrangles[1].point[2].x = 0 + widthd2;
	m_vTrangles[1].point[2].y = 0 + heightd2;

	m_vTrangles[2].point[0].x = 0 + width;
	m_vTrangles[2].point[0].y = 0 + height;
	m_vTrangles[2].point[1].x = 0;
	m_vTrangles[2].point[1].y = 0 + height;
	m_vTrangles[2].point[2].x = 0 + widthd2;
	m_vTrangles[2].point[2].y = 0 + heightd2;

	m_vTrangles[3].point[0].x = 0 ;
	m_vTrangles[3].point[0].y = 0 + height;
	m_vTrangles[3].point[1].x = 0 ;
	m_vTrangles[3].point[1].y = 0;
	m_vTrangles[3].point[2].x = 0 + widthd2;
	m_vTrangles[3].point[2].y = 0 + heightd2;

	m_vTrangles[4].point[0].x = 0;
	m_vTrangles[4].point[0].y = 0;
	m_vTrangles[4].point[1].x = 0 + widthd2;
	m_vTrangles[4].point[1].y = 0;
	m_vTrangles[4].point[2].x = 0 + widthd2;
	m_vTrangles[4].point[2].y = 0 + heightd2;
}

bool CoolItem::StartCool(const long CoolCategory, long BeginTime, const long CoolTime)
{
	// 数值保护
	if (BeginTime < 0 || BeginTime > CoolTime)
	{
		BeginTime = 0;
	}

	if (CoolCategory == m_CoolCategory)
	{
		m_CoolCount = BeginTime;
		m_CoolTime = CoolTime + COOL_ADD_SHOWTIME;
		m_IsCooling = true;

		RestoreCoolTrangles();

		return true;
	}

	return false;
}

void CoolItem::Clear()
{
	m_CoolCount = 0;
	m_CoolTime = 1;
	m_CoolCategory = 0;	
	m_IsCooling = false;
}

void CoolItem::DrawCooling(IPainter*pPainter, const long x, const long y, 
						   const unsigned long dwColor, 
						   ImageAnimationManager * pImageAnimation, const char* pAnimation)
{
	// 冷确已停止则返回
	if (!m_IsCooling)
	{
		return;
	}

	DrawCooling(pPainter, x, y, m_nWidth, m_nHeight, dwColor, pImageAnimation, pAnimation);
}

void CoolItem::DrawCooling(IPainter* pPainter, long x, long y, 
						   long width, long height, unsigned long dwColor, 
						   ImageAnimationManager * pImageAnimation, const char* pAnimation)
{
	// 冷确已停止则返回
	if (!m_IsCooling)
	{
		return;
	}

	if (width != m_nWidth || height != m_nHeight)
	{
		SetCoolSize(width, height);
	}

	// 已进入动画显示时间
	if (pImageAnimation != NULL &&
		pAnimation != NULL &&
		m_CoolCount > m_CoolTime - s_nAnimationTime)
	{
		pImageAnimation->SetAnimationCurTime(pAnimation, m_CoolCount - (m_CoolTime - s_nAnimationTime));
		rect_t rect = {x, y, x + width, y + height};
		pImageAnimation->DrawAnimation(pAnimation, x, y, rect);
		return;
	}

	long widthd2 = width / 2;
	long heightd2 = height / 2;

	size_t drawindex = 0;

	if (m_CoolCount * 8 <= m_CoolTime)
	{
		// 0-1/8 第一个三角形变化
		m_vTrangles[0].point[0].x = (long)(0 + widthd2 
			+ tan(m_CoolCount * COOL_PI * 2 / m_CoolTime) * heightd2);
	}
	else if (m_CoolCount * 8  <= m_CoolTime * 3)
	{
		//1/8-3/8 第二个三角形变化
		if (m_CoolCount * 8  <= m_CoolTime * 2)
		{
			double angle = COOL_PI / 4 
				- (m_CoolCount * COOL_PI * 2 / m_CoolTime - COOL_PI * 2 / 8);

			m_vTrangles[1].point[0].y = 
				(long)(0 + heightd2 - tan(angle) * widthd2);
		}
		else
		{
			double angle = m_CoolCount * COOL_PI * 2 / m_CoolTime 
				- COOL_PI * 2 / 4;

			m_vTrangles[1].point[0].y = 
				(long)(0 + heightd2 + tan(angle) * widthd2);
		}

		drawindex = 1;
	}
	else if (m_CoolCount* 8 <= m_CoolTime * 5)
	{
		//3/8-5/8 第三个三角形变化
		if (m_CoolCount * 8  <= m_CoolTime * 4)
		{
			double angle = COOL_PI/4 - (m_CoolCount * COOL_PI * 2 
				/ m_CoolTime - COOL_PI * 2 * 3 / 8);

			m_vTrangles[2].point[0].x = 
				(long)(0 + widthd2 + tan(angle) * heightd2);
		}
		else
		{
			double angle = m_CoolCount * COOL_PI * 2 / m_CoolTime 
				- COOL_PI * 2 / 2;

			m_vTrangles[2].point[0].x = 
				(long)(0 + widthd2 - tan(angle) * heightd2);
		}

		drawindex = 2;
	}
	else if (m_CoolCount * 8 <= m_CoolTime * 7)
	{
		//5/8-7/8 第四个三角形变化
		if (m_CoolCount * 8  <= m_CoolTime * 6)
		{
			double angle = 6 * 2 * COOL_PI / 8 
				- (m_CoolCount * COOL_PI * 2 / m_CoolTime);

			m_vTrangles[3].point[0].y = 
				(long)(0 + heightd2 +  tan(angle) * widthd2);
		}
		else
		{
			double angle = m_CoolCount * COOL_PI * 2 / m_CoolTime 
				- 6 * 2 * COOL_PI / 8;

			m_vTrangles[3].point[0].y = 
				(long)(0 + heightd2 - tan(angle) * widthd2);
		}

		drawindex = 3;
	}
	else if (m_CoolCount <= m_CoolTime)
	{
		//7/8-8/8 第五个三角形变化
		double angle = COOL_PI / 4 - ((m_CoolCount * COOL_PI * 2 / m_CoolTime) 
			- 7 * 2 * COOL_PI / 8);

		m_vTrangles[4].point[0].x = 
			(long)(0 + widthd2 - tan(angle) * heightd2);

		drawindex = 4;
	}

	for (size_t i = drawindex; i < 5; ++i)
	{
		pPainter->DrawTriangle(m_vTrangles[i].point[0].x + x, 
			m_vTrangles[i].point[0].y + y, m_vTrangles[i].point[1].x + x, 
			m_vTrangles[i].point[1].y + y, m_vTrangles[i].point[2].x + x, 
			m_vTrangles[i].point[2].y + y, dwColor);
	}
}

void CoolItem::DrawCoolingRound(IPainter* pPainter, long x, long y, 
								long width, long height, unsigned long dwColor)
{
	int show_fan = (m_CoolTime - m_CoolCount) * 100 / m_CoolTime;

	pPainter->FillRoundPart(x, y, x + width, y + height, dwColor, show_fan, 100);
}

void CoolItem::UpdateCool(float fliptime)
{
	m_CoolCount += (long)(fliptime * 1000);

	if (m_CoolCount >= m_CoolTime)
	{
		// 计时超时则结束
		m_IsCooling = false;
		return;
	}
}

void CoolItem::DrawCooling(CImagePlayer* pImagePlayer, long x, long y, long width, long height)
{
    if (!m_IsCooling)
    {
        return;
    }

    if (width != m_nWidth || height != m_nHeight)
    {
        SetCoolSize(width, height);
    }

    if (m_CoolCount >= m_CoolTime)
    {
        return;
    }

    float angle = (float)m_CoolCount / (float)m_CoolTime;
    float begin_angle = -COOL_PI / 2.0f;
    float end_angle = begin_angle + 2*COOL_PI*angle;
    pImagePlayer->DrawImageFan(x, y, width, height, begin_angle, end_angle);
}

// CoolManager

/// entity: MultiTextBox
/// \brief 列表框实体
DECLARE_ENTITY(CoolManager, 0, IEntity);

/// property: bool Transparent
/// \brief 是否透明不响应消息
DECLARE_PROPERTY(PERSISTID, CoolManager, AnimationMng, GetAniMng, SetAniMng);

/// method: int AnimationTime
/// \brief  动画时间
/// \param millionseconds 毫秒
DECLARE_PROPERTY(int, CoolManager, AnimationTime, GetAnimationTime, SetAnimationTime);

#if 0	//DECLARE_METHOD
/// method: bool LoadResource(string path)
/// \brief  读取冷却特殊配置
/// \param path 配置文件名
DECLARE_METHOD_1(bool, CoolManager, LoadResource, const char*);

/// method: bool SetCommonCoolRange(int lPublicCoolCategory, int begin_cool_type, int end_cool_type);
/// \brief 设置各类公共冷却的大分类区间
/// \param lPublicCoolCategory 公共冷却类型, 这个值不能大于10,约定
/// \param begin_team_type 冷却组起始序号
/// \param end_team_type 冷却组结束序号
DECLARE_METHOD_3(bool, CoolManager, SetCommonCoolRange, int, int, int);

/// method: bool SetCommonCoolNoEffectRange(int lPublicCoolCategory, int begin_cool_type, int end_cool_type);
/// \brief 设置各类公共冷却的大分类区间
/// \param lPublicCoolCategory 公共冷却类型, 这个值不能大于10,约定
/// \param begin_cool_type 冷却组起始序号
/// \param end_cool_type 冷却组结束序号
DECLARE_METHOD_3(bool, CoolManager, SetCommonCoolNoEffectRange, int, int, int);

/// method: bool SetCommonTeamCoolRange(int lPublicCoolCategory, int begin_team_type, int end_team_type);
/// \brief 设置公共组冷却的大分类区间
/// \param lPublicCoolCategory 公共冷却类型, 这个值不能大于10,约定
/// \param begin_team_type 冷却组起始序号
/// \param end_team_type 冷却组结束序号
DECLARE_METHOD_3(bool, CoolManager, SetCommonTeamCoolRange, int, int, int);

/// method: bool SetCommonTeamCoolNoEffectRange(int lPublicCoolCategory, int begin_cool_type, int end_cool_type);
/// \brief 设置各类公共冷却的大分类区间
/// \param lPublicCoolCategory 公共冷却类型, 这个值不能大于10,约定
/// \param begin_cool_type 冷却组起始序号
/// \param end_cool_type 冷却组结束序号
DECLARE_METHOD_3(bool, CoolManager, SetCommonTeamCoolNoEffectRange, int, int, int);

/// method: bool StartACool(int nCoolCategory, int TotalTime, int nCoolTeamIndex, int nBeginTime)
/// \brief  设置指定行的关联对象
/// \param lCoolCategory 冷却类型
/// \param TotalTime 冷却持继时间
/// \param lCoolTeamIndex 冷却分组
/// \param nBeginTime 冷却开始时间
DECLARE_METHOD_4(bool, CoolManager, StartACool, int, int, int, int);

/// method: bool StartACool(const int width, const int height)
/// \brief  设置默认的冷确大小
/// \param width 宽
/// \param height 高
DECLARE_METHOD_2(bool, CoolManager, SetDefCoolSize, int, int);

/// method: bool IsCooling(const int nCoolCategory, int nCoolTeam)
/// \brief  检查是否正在准确
/// \param lCoolCategory 冷却类型
DECLARE_METHOD_2(bool, CoolManager, IsCooling, int, int);

/// method: bool ClearCooling(const int nCoolCategory)
/// \brief  清除传入ID的冷却
/// \param lCoolCategory 冷却类型ID/组冷却ID
DECLARE_METHOD_1(bool, CoolManager, ClearCooling, int);

/// method: bool ClearAllCooling()
/// \brief  清除所有冷却
DECLARE_METHOD_0(bool, CoolManager, ClearAllCooling);

/// method: bool UpdateCoolItem(long lCoolType, float seconds)
/// \brief  手动更新冷却时间
/// \param lCoolType 冷却类型
/// \param seconds 更新时长
DECLARE_METHOD_2(bool, CoolManager, UpdateCoolItem, long, float);
#endif

CoolManager::CoolManager()
{
	m_nAnimationTime = 0;
	m_vCommonCool.resize(1);
	m_i64TimeCount = Port_GetTickCount();
}

CoolManager::~CoolManager()
{
#ifdef USE_MAP
	std::map<int, CoolItem*>::iterator it = m_MapCoolItems.begin();
	for (; it != m_MapCoolItems.end(); ++it)
	{
		CoolItem* pItem = it->second;
		if (pItem != NULL)
		{
			CORE_DELETE(pItem);
		}
	}
	m_MapCoolItems.clear();
#else
	TPodHashMap<int, CoolItem*, TPodTraits<int>, TCoreAlloc>::iterator it = m_MapCoolItems.Begin();
	TPodHashMap<int, CoolItem*, TPodTraits<int>, TCoreAlloc>::iterator end = m_MapCoolItems.End();

	for (; it != end; ++it)
	{
		CORE_DELETE(it.GetData());
	}
#endif
}

// 初始化（可创建其他附属实体）
bool CoolManager::Init(const IVarList& args)
{
	//m_MapCoolItems.clear();
	//m_vUnUsed.clear();

	GetCore()->AddExecute(this);
	// 设置成单件实体
	//GetCore()->SetSingle(this);

	return true;
}

// 用指定符号分割字符串
void ext_split_string(IVarList& result, const char *str, const char ch)
{
	if (0 == ch)
	{
		return;
	}

	const size_t SIZE1 = ::strlen(str) + 1;

	TAutoMem<char, 256> auto_buf(SIZE1);

	char* buf = auto_buf.GetBuffer();

	::CopyString(buf, SIZE1, str);

	char* s = NULL;
	char* p;

	for (p = buf; *p; ++p)
	{
		if (*p == ch)
		{
			*p = 0;

			if (s)
			{
				result.AddString(s);

				s = NULL;
			}
			else
			{
				result.AddString(p);
			}
		}
		else if (NULL == s)
		{
			s = p;
		}
	}

	if (s)
	{
		result.AddString(s);
	}
	else if (p != buf)
	{
		result.AddString(p);
	}
}

//字符串类型的整数类型转换为整型
inline int convert_int(const char* szint, int default_int)
{
	if (szint == NULL)
		return default_int;

	return ::atoi(szint);
}

// 加载配置文件
bool CoolManager::LoadResource(const char* szPath)
{
	return true;
}

// 设置各类公共冷却的大分类区间
bool CoolManager::SetCommonCoolRange(int lPublicTeam, 
									 int begin_cool_type, int end_cool_type)
{
	if (lPublicTeam >= (int)m_vCommonCool.size())
	{
		m_vCommonCool.resize(lPublicTeam + 1);
	}

	CommonCoolItem &item = m_vCommonCool[lPublicTeam];

	item.m_CoolCategory = lPublicTeam;
	item.begin_cool_index = begin_cool_type;
	item.end_cool_index = end_cool_type;

	return true;
}

// 设置公共冷却的免疫区间
bool CoolManager::SetCommonCoolNoEffectRange(int lPublicTeam, 
											 int begin_cool_type, int end_cool_type)
{
	if (lPublicTeam > -1 && lPublicTeam < (int)m_vCommonCool.size())
	{
		m_vCommonCool[lPublicTeam].no_effect_begin = begin_cool_type;
		m_vCommonCool[lPublicTeam].no_effect_end = end_cool_type;
		return true;
	}
	else
	{
		m_vCommonCool.resize(m_vCommonCool.size() + 1);

		CommonCoolItem & item = m_vCommonCool.back();

		item.m_CoolCategory = lPublicTeam;
		item.begin_cool_index = begin_cool_type;
		item.end_cool_index = end_cool_type;
	}

	return true;
}

// 设置各类公共冷却的大分类区间
bool CoolManager::SetCommonTeamCoolRange(int lPublicTeam, 
										 int begin_cool_type, int end_cool_type)
{
	for (size_t i = 0; i < m_vTeamCommonCool.size(); ++i)
	{
		if (m_vTeamCommonCool[i].m_CoolCategory == lPublicTeam)
		{
			m_vTeamCommonCool[i].begin_cool_index = begin_cool_type;
			m_vTeamCommonCool[i].end_cool_index = end_cool_type;
			return true;
		}
	}

	m_vTeamCommonCool.resize(m_vTeamCommonCool.size() + 1);

	CommonCoolItem & item = m_vTeamCommonCool.back();

	item.m_CoolCategory = lPublicTeam;
	item.begin_cool_index = begin_cool_type;
	item.end_cool_index = end_cool_type;

	return true;
}

// 设置公共冷却的免疫区间
bool CoolManager::SetCommonTeamCoolNoEffectRange(
	int lPublicTeam, int begin_cool_type, int end_cool_type)
{
	for (size_t i = 0; i < m_vTeamCommonCool.size(); ++i)
	{
		if (m_vTeamCommonCool[i].m_CoolCategory == lPublicTeam)
		{
			m_vTeamCommonCool[i].no_effect_begin = begin_cool_type;
			m_vTeamCommonCool[i].no_effect_end = end_cool_type;
			return true;
		}
	}

	m_vTeamCommonCool.resize(m_vTeamCommonCool.size() + 1);

	CommonCoolItem & item = m_vTeamCommonCool.back();

	item.m_CoolCategory = lPublicTeam;
	item.no_effect_begin = begin_cool_type;
	item.no_effect_end = end_cool_type;

	return true;
}
// 关闭（可删除其他引用的附属实体，而资源释放应在析构函数中进行）
bool CoolManager::Shut()
{
	GetCore()->RemoveExecute(this);

	return true;
}

// 每帧调用（如果已添加到运行队列）
void CoolManager::Execute(float seconds)
{
	unsigned int old_time_count = m_i64TimeCount;

	m_i64TimeCount = Port_GetTickCount();
	seconds = (m_i64TimeCount - old_time_count) / 1000.0f;

	size_t c = 0;

	for (c = 0; c < m_vCommonCool.size(); ++c)
	{
		if (m_vCommonCool[c].IsCooling())
		{
			m_vCommonCool[c].UpdateCool(seconds);
		}
	}

	for (c = 0; c < m_vTeamCommonCool.size(); ++c)
	{
		if (m_vTeamCommonCool[c].IsCooling())
		{
			m_vTeamCommonCool[c].UpdateCool(seconds);
		}
	}


#ifdef USE_MAP
	std::map<int, CoolItem*>::iterator it = m_MapCoolItems.begin();

	for (; it != m_MapCoolItems.end(); ++it)
	{
		if (it->second != NULL && it->second->IsCooling())
		{
			it->second->UpdateCool(seconds);
		}
	}
#else
	TPodHashMap<int, CoolItem*, TPodTraits<int>, TCoreAlloc>::iterator it = m_MapCoolItems.Begin();
	TPodHashMap<int, CoolItem*, TPodTraits<int>, TCoreAlloc>::iterator end = m_MapCoolItems.End();

	for (; it != end; ++it)
	{
		CoolItem* pItem = it.GetData();

		if (pItem->IsCooling())
		{
			pItem->UpdateCool(seconds);
		}
	}
#endif
}

// 手动更新冷却时间
bool CoolManager::UpdateCoolItem(long lCoolType, float seconds)
{
#ifdef USE_MAP
	std::map<int, CoolItem*>::iterator itFind = m_MapCoolItems.find(lCoolType);
	if (itFind != m_MapCoolItems.end())
	{
		CoolItem* item = itFind->second;
		if (item != NULL)
		{
			item->UpdateCool(seconds);
		}
		
	}
#else
	CoolItem* pCoolItem;
	if (m_MapCoolItems.GetData(lCoolType, pCoolItem))
	{
		pCoolItem->UpdateCool(seconds);
		return true;
	}
#endif

	return false;
}

bool CoolManager::StartACool(int nCoolCategory, int TotalTime, int nCoolTeamIndex, int nBeginTime)
{
	// 1.启动如果是公共冷却，则记录组ID
	if (nCoolCategory < (int)m_vCommonCool.size())
	{
		if (m_vCommonCool[nCoolCategory].StartCool(nCoolCategory, nBeginTime, TotalTime))
		{
			m_vCommonCool[nCoolCategory].no_effect_index = nCoolTeamIndex;
		}

		return true;
	}

	// 2.组公共冷却
	for (size_t i = 0; i < m_vTeamCommonCool.size(); ++i)
	{
		if (m_vTeamCommonCool[i].m_CoolCategory == nCoolCategory)
		{
			if (m_vTeamCommonCool[i].StartCool(nCoolCategory, nBeginTime, TotalTime))
			{
				m_vTeamCommonCool[i].no_effect_index = nCoolTeamIndex;
			}
			return true;
		}
	}

	// 2.是普通冷却，启动, 组冷却单独启动的
#ifdef USE_MAP
	std::map<int, CoolItem*>::iterator it = m_MapCoolItems.find(nCoolCategory);
#else
	CoolItem* pCoolItem;
#endif

#ifdef USE_MAP
	if (it != m_MapCoolItems.end())
#else
	if (m_MapCoolItems.GetData(nCoolCategory, pCoolItem))
#endif
	{
#ifdef USE_MAP
		if (it->second != NULL)
		{
			it->second->StartCool(nCoolCategory, nBeginTime, TotalTime);
		}
		
#else
		pCoolItem->StartCool(nCoolCategory, nBeginTime, TotalTime);
#endif
	}

	return true;
}

//
bool CoolManager::IsCooling(int nCoolCategory, int nCoolTeam)
{
	// 1公共冷却是否正在进行
	for (size_t i = 0 ; i < m_vCommonCool.size(); ++i)
	{
		if (nCoolCategory >= m_vCommonCool[i].begin_cool_index && 
			nCoolCategory < m_vCommonCool[i].end_cool_index)
		{
			// 1. 公共冷却是否正在冷却
			if (m_vCommonCool[i].IsCooling())
			{
				return true;
			}
		}
	}

	// 2.自身是否正在冷却

#ifdef USE_MAP
	std::map<int, CoolItem*>::iterator it = m_MapCoolItems.find(nCoolCategory);
#else
	CoolItem* pCoolItem;
#endif

#ifdef USE_MAP
	if (it != m_MapCoolItems.end())
#else
	if (m_MapCoolItems.GetData(nCoolCategory, pCoolItem))
#endif
	{
#ifdef USE_MAP
		CoolItem* self_item = it->second;


		// 自身如果在冷却，则返回在冷却
		if (self_item != NULL && self_item->IsCooling())
#else
		if (pCoolItem->IsCooling())
#endif
		{
			return true;
		}
	}

#ifdef USE_MAP
	// 3.所属分组是否正在冷却
	it = m_MapCoolItems.find(nCoolTeam);

	if (it != m_MapCoolItems.end())
#else
	if (m_MapCoolItems.GetData(nCoolTeam, pCoolItem))
#endif
	{
#ifdef USE_MAP
		CoolItem* team_item = it->second;

		if (team_item != NULL && team_item->IsCooling())
#else
		if (pCoolItem->IsCooling())
#endif
		{
#ifdef USE_MAP
			// 是否是特殊的不受组冷却影响的冷却ID
			if (m_vSpecialCategory.find(nCoolCategory) != m_vSpecialCategory.end())
#else
			if (m_vSpecialCategory.Exists(nCoolCategory))
#endif
			{
				// 特殊不冷却
			}
			else
			{
				return true;
			}
		}
	}

	// 4.组公共冷却是否正在进行
	for (size_t i = 0 ; i < m_vTeamCommonCool.size(); ++i)
	{
		if (nCoolTeam >= m_vTeamCommonCool[i].begin_cool_index && 
			nCoolTeam < m_vTeamCommonCool[i].end_cool_index)
		{
			if (m_vTeamCommonCool[i].IsCooling())
			{
				if (m_vTeamCommonCool[i].no_effect_index == nCoolTeam)
				{
					// 发动着，不受影响
					continue;
				}
				else if (nCoolTeam >= m_vTeamCommonCool[i].no_effect_begin && 
					nCoolTeam < m_vTeamCommonCool[i].no_effect_end)
				{
					// 对组永远免疫
					continue;
				}
#ifdef USE_MAP
				else if (m_vSpecialTeam.find(nCoolTeam) != m_vSpecialTeam.end())
#else
				else if (m_vSpecialTeam.Exists(nCoolTeam))
#endif
				{
					// 暂时免疫
					continue;
				}
#ifdef USE_MAP
				else if (m_vSpecialCategory.find(nCoolCategory) != m_vSpecialCategory.end())
#else
				else if (m_vSpecialCategory.Exists(nCoolCategory))
#endif
				{
					// 技能免疫
					continue;
				}

				return true;
			}
		}
	}

	return false;
}

// 清除传入ID的冷却
bool CoolManager::ClearCooling(int nCoolCategory)
{
	// 1公共冷却
	if (nCoolCategory < (int)m_vCommonCool.size())
	{
		m_vCommonCool[nCoolCategory].ClearCooling();
		return true;
	}

	// 2.组公共冷却
	for (size_t i = 0 ; i < m_vTeamCommonCool.size(); ++i)
	{
		if (m_vTeamCommonCool[i].m_CoolCategory == nCoolCategory)
		{
			// 发动着，不受影响
			m_vTeamCommonCool[i].ClearCooling();
			return true;
		}  
	}

	// 3.其他冷却
#ifdef USE_MAP
	std::map<int, CoolItem*>::iterator it = m_MapCoolItems.find(nCoolCategory);
#else
	CoolItem* pCoolItem;
#endif

#ifdef USE_MAP
	if (it != m_MapCoolItems.end())
#else
	if (m_MapCoolItems.GetData(nCoolCategory, pCoolItem))
#endif
	{
#ifdef USE_MAP
		CoolItem* self_item = it->second;

		// 自身如果在冷却，则返回在冷却
		if (self_item != NULL)
		{
			self_item->ClearCooling();
		}
		
#else
		pCoolItem->ClearCooling();
#endif
		return true;
	}

	return false;
}

// 清除所有冷却
bool CoolManager::ClearAllCooling()
{
	// 1公共冷却
	size_t count = m_vCommonCool.size();

	for (size_t nCoolCategory = 0; nCoolCategory < count; ++nCoolCategory)
	{
		m_vCommonCool[nCoolCategory].ClearCooling();
	}

	// 2.组公共冷却
	for (size_t i = 0 ; i < m_vTeamCommonCool.size(); ++i)
	{
		// 发动着，不受影响
		m_vTeamCommonCool[i].ClearCooling();
	}

	// 3.其他冷却
#ifdef USE_MAP
	std::map<int, CoolItem*>::iterator it = m_MapCoolItems.begin();
#else
	TPodHashMap<int, CoolItem*, TPodTraits<int>, TCoreAlloc>::iterator it = m_MapCoolItems.Begin();
#endif

#ifdef USE_MAP
	while (it != m_MapCoolItems.end())
#else
	while (it != m_MapCoolItems.End())
#endif
	{
#ifdef USE_MAP
		CoolItem* self_item = it->second;
#else
		CoolItem* pCoolItem = it.GetData();
#endif

#ifdef USE_MAP
		// 自身如果在冷却，则返回在冷却
		if (self_item != NULL)
		{
			self_item->ClearCooling();
		}
		
#else
		pCoolItem->ClearCooling();
#endif
		++it;
	}

	return true;
}

// 添加一种类型的冷确，每种类型仅创建一次;
bool CoolManager::AddCool(int CoolCategory)
{
#ifdef USE_MAP
	std::map<int, CoolItem*>::iterator it = m_MapCoolItems.find(CoolCategory);
	if (it != m_MapCoolItems.end())
#else
	if (m_MapCoolItems.Exists(CoolCategory))
#endif
	{
		return true;
	}

	CoolItem* newItem = CORE_NEW(CoolItem);

	newItem->m_CoolCategory = CoolCategory;
	//newItem.m_CoolTeamIndex = CoolTeamIndex;
	newItem->m_nWidth = m_vCommonCool[0].m_nWidth;
	newItem->m_nHeight = m_vCommonCool[0].m_nHeight;

#ifdef USE_MAP
	m_MapCoolItems.insert(std::make_pair(CoolCategory, newItem));
#else
	m_MapCoolItems.Add(CoolCategory, newItem);
#endif


	return true;
}

void CoolManager::DelCool(int CoolCategory)
{
#ifdef USE_MAP
	std::map<int, CoolItem*>::iterator it = m_MapCoolItems.find(CoolCategory);
	if (it != m_MapCoolItems.end())
#else
	if (m_MapCoolItems.Exists(CoolCategory))
#endif
	{
#ifdef USE_MAP
		CoolItem* pItem = it->second;
		if (pItem != NULL)
		{
			CORE_DELETE(pItem);
		}
		
		m_MapCoolItems.erase(it);
#else
		m_MapCoolItems.Remove(CoolCategory);
#endif
	}
}

// 设置默认冷确的大小
bool CoolManager::SetDefCoolSize(int width, int height)
{
	m_vCommonCool[0].SetCoolSize(width, height);
	return true;
}

// 设置动画时间毫秒
void CoolManager::SetAnimationTime(int time)
{
	if (time != m_nAnimationTime)
	{
		m_nAnimationTime = time;
		CoolItem::s_nAnimationTime = time;
	}
}

// 设置动画管理器实体
void CoolManager::SetAniMng(PERSISTID id)
{
	m_ImageAmngID = id;
}

//
PERSISTID CoolManager::GetAniMng()
{
	return m_ImageAmngID;
}

//
bool CoolManager::GetCoolItem(int nCoolCategory, CoolItem** pItem)
{
#ifdef USE_MAP
	std::map<int, CoolItem*>::iterator it = m_MapCoolItems.find(nCoolCategory);

 	if (it != m_MapCoolItems.end())
	{
		*pItem = it->second;
		return true;
	}

	return false;
#else
	return m_MapCoolItems.GetData(nCoolCategory, pItem);
#endif
}

//
void CoolManager::DrawCooling(int lCoolCategory, 
							  int lCoolTeamIndex, IPainter* pPainter, long x, 
							  long y, unsigned long dwColor, const char* pAnimation, bool round)
{
	DrawCooling(lCoolCategory, lCoolTeamIndex, pPainter, x, y, 
		-1, -1, dwColor, pAnimation, round);
}

// 根据指定宽高绘制冷确
void CoolManager::DrawCooling(int lCoolCategory, 
							  int lCoolTeamIndex, IPainter* pPainter, 
							  long x, long y, long width, long height,
							  unsigned long dwColor, const char* pAnimation, bool round)
{
	CoolItem* pCoolItem = NULL;

	//获取需要描画的CD最长的CoolItem
	CoolItem* pSelfCoolItem = NULL;
	CoolItem* pTeamCoolItem = NULL;

	bool bCoolSelf = false;
	bool bCoolTeam = false;
	GetCoolItem(lCoolCategory, &pSelfCoolItem);
	GetCoolItem(lCoolTeamIndex, &pTeamCoolItem);
	if( pSelfCoolItem && pTeamCoolItem )
	{
		if( pSelfCoolItem->GetCoolTime() > pTeamCoolItem->GetCoolTime() )
		{
			bCoolSelf = true;
		}
		else
		{
			bCoolTeam = true;
		}
	}
	else if( pSelfCoolItem )
	{
		bCoolSelf = true;
	}
	else if( pTeamCoolItem )
	{
		bCoolTeam = true;
	}

	// 1. 自身是否正在冷却
	if (GetCoolItem(lCoolCategory, &pCoolItem) && bCoolSelf)
	{
		if (pCoolItem->IsCooling())
		{
			ImageAnimationManager * pAniManger = (ImageAnimationManager * )GetCore()->GetEntity(m_ImageAmngID);

			if (width < 0)
			{
				if (round)
				{
					pCoolItem->DrawCoolingRound(pPainter, x, y, width, height, dwColor);
				}
				else
				{
					pCoolItem->DrawCooling(pPainter, x, y,dwColor, pAniManger, pAnimation);
				}
			}
			else
			{
				if (round)
				{
					pCoolItem->DrawCoolingRound(pPainter, x, y, width, height, dwColor);
				}
				else
				{
					pCoolItem->DrawCooling(pPainter, x, y, width, height, dwColor, pAniManger, pAnimation);
				}
			}
			return;
		}
	}

	// 2. 组冷却是否正在进行
	if (GetCoolItem(lCoolTeamIndex, &pCoolItem) && bCoolTeam)
	{
		if (pCoolItem->IsCooling())
		{
			// 是否是暂时不受组冷却影响的冷却
#ifdef USE_MAP
			if (m_vSpecialCategory.find(lCoolCategory) != m_vSpecialCategory.end())
#else
			if (m_vSpecialCategory.Exists(lCoolCategory))
#endif
			{
				// 不受影响
			}
			else
			{
				ImageAnimationManager * pAniManger = (ImageAnimationManager * )GetCore()->GetEntity(m_ImageAmngID);

				if (width < 0)
				{
					if (round)
					{
						pCoolItem->DrawCoolingRound(pPainter, x, y, width, height, dwColor);
					}
					else
					{
						pCoolItem->DrawCooling(pPainter, x, y,dwColor, pAniManger, pAnimation);
					}
				}
				else
				{
					if (round)
					{
						pCoolItem->DrawCoolingRound(pPainter, x, y, width, height, dwColor);
					}
					else
					{
						pCoolItem->DrawCooling(pPainter, x, y, width, height, dwColor, pAniManger, pAnimation);
					}
				}
				return;
			}
		}
	}

	// 3. 最后公共冷却在进行,则进行公共冷却
	// 如果不需要冷却,冷却类型可配成负数
	size_t c;
	// 3.1 组公共冷却
	for (c = 0; c < m_vTeamCommonCool.size(); ++c)
	{
		if (lCoolTeamIndex >= m_vTeamCommonCool[c].begin_cool_index && 
			lCoolTeamIndex < m_vTeamCommonCool[c].end_cool_index)
		{

			if (m_vTeamCommonCool[c].no_effect_index != lCoolTeamIndex)
			{
				if (m_vTeamCommonCool[c].IsCooling())
				{
					// 组公共冷却正在进行
					if (lCoolTeamIndex >= m_vTeamCommonCool[c].no_effect_begin && 
						lCoolTeamIndex < m_vTeamCommonCool[c].no_effect_end)
					{
						// 免疫
					}
#ifdef USE_MAP
					else if (m_vSpecialTeam.find(lCoolTeamIndex) != m_vSpecialTeam.end())
#else
					else if (m_vSpecialTeam.Exists(lCoolTeamIndex))
#endif
					{
						// 特殊组，不受公共传递影响
					}
#ifdef USE_MAP
					else if (m_vSpecialCategory.find(lCoolCategory) != m_vSpecialCategory.end())
#else
					else if (m_vSpecialCategory.Exists(lCoolCategory))
#endif
					{
						// 特殊技能，不冷却
					}
					else
					{
						ImageAnimationManager * pAniManger = (ImageAnimationManager * )GetCore()->GetEntity(m_ImageAmngID);

						if (width < 0)
						{
							if (round)
							{
								m_vTeamCommonCool[c].DrawCoolingRound(pPainter, x, y, width, height, dwColor);
							}
							else
							{
								m_vTeamCommonCool[c].DrawCooling(pPainter, x, y, dwColor, pAniManger, pAnimation);
							}
						}
						else
						{
							if (round)
							{
								m_vTeamCommonCool[c].DrawCoolingRound(pPainter, x, y, width, height, dwColor);
							}
							else
							{
								m_vTeamCommonCool[c].DrawCooling(pPainter, x, y, width, height, dwColor, pAniManger, pAnimation);
							}
						}
						return;
					}
				}
			}
		}
	}

	// 3.2 本身公共冷却
	for (c = 0; c < m_vCommonCool.size(); ++c)
	{
		// 检查是否属性此类公共冷却范围
		if (lCoolCategory >= m_vCommonCool[c].begin_cool_index && 
			lCoolCategory < m_vCommonCool[c].end_cool_index)
		{
			// 是此类公共冷却范围，如果是当前冷却大分类，则不受影响,如果不是同类，则受到附加规则附加
			if (m_vCommonCool[c].no_effect_index != lCoolCategory)
			{
				if (m_vCommonCool[c].IsCooling())
				{
					ImageAnimationManager * pAniManger = (ImageAnimationManager * )GetCore()->GetEntity(m_ImageAmngID);

					if (width < 0)
					{
						if (round)
						{
							m_vCommonCool[c].DrawCoolingRound(pPainter, x, y, width, height, dwColor);
						}
						else
						{
							m_vCommonCool[c].DrawCooling(pPainter, x, y,dwColor, pAniManger, pAnimation);
						}
					}
					else
					{
						if (round)
						{
							m_vCommonCool[c].DrawCoolingRound(pPainter, x, y, width, height, dwColor);
						}
						else
						{
							m_vCommonCool[c].DrawCooling(pPainter, x, y, width, height, dwColor, pAniManger, pAnimation);
						}
					}

					return;
				}
			}

			break;
		}
	}
}

// 根据指定冷却的图绘制冷却
void CoolManager::DrawCooling(int lCoolCategory, int lCoolTeamIndex, CImagePlayer* pImagePlayer, 
    long x, long y, long width, long height)
{
    CoolItem* pCoolItem = NULL;
    CoolItem* pSelfCoolItem = NULL;
    CoolItem* pTeamCoolItem = NULL;

    bool bCoolSelf = false;
    bool bCoolTeam = false;
    GetCoolItem(lCoolCategory, &pSelfCoolItem);
    GetCoolItem(lCoolTeamIndex, &pTeamCoolItem);
    if( pSelfCoolItem && pTeamCoolItem )
    {
        if( pSelfCoolItem->GetCoolTime() > pTeamCoolItem->GetCoolTime() )
        {
            bCoolSelf = true;
        }
        else
        {
            bCoolTeam = true;
        }
    }
    else if( pSelfCoolItem )
    {
        bCoolSelf = true;
    }
    else if( pTeamCoolItem )
    {
        bCoolTeam = true;
    }

    if (GetCoolItem(lCoolCategory, &pCoolItem) && bCoolSelf)
    {
        if (pCoolItem->IsCooling())
        {
            pCoolItem->DrawCooling(pImagePlayer, x, y, width, height);
        }
    }
}