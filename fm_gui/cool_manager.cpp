//--------------------------------------------------------------------
// �ļ���:		CoolManager.h
// ��  ��:		��ȴ������
// ˵  ��:		
// ��������:	2008��8��27��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
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
	// ��ֵ����
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
	// ��ȷ��ֹͣ�򷵻�
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
	// ��ȷ��ֹͣ�򷵻�
	if (!m_IsCooling)
	{
		return;
	}

	if (width != m_nWidth || height != m_nHeight)
	{
		SetCoolSize(width, height);
	}

	// �ѽ��붯����ʾʱ��
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
		// 0-1/8 ��һ�������α仯
		m_vTrangles[0].point[0].x = (long)(0 + widthd2 
			+ tan(m_CoolCount * COOL_PI * 2 / m_CoolTime) * heightd2);
	}
	else if (m_CoolCount * 8  <= m_CoolTime * 3)
	{
		//1/8-3/8 �ڶ��������α仯
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
		//3/8-5/8 �����������α仯
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
		//5/8-7/8 ���ĸ������α仯
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
		//7/8-8/8 ����������α仯
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
		// ��ʱ��ʱ�����
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
/// \brief �б��ʵ��
DECLARE_ENTITY(CoolManager, 0, IEntity);

/// property: bool Transparent
/// \brief �Ƿ�͸������Ӧ��Ϣ
DECLARE_PROPERTY(PERSISTID, CoolManager, AnimationMng, GetAniMng, SetAniMng);

/// method: int AnimationTime
/// \brief  ����ʱ��
/// \param millionseconds ����
DECLARE_PROPERTY(int, CoolManager, AnimationTime, GetAnimationTime, SetAnimationTime);

#if 0	//DECLARE_METHOD
/// method: bool LoadResource(string path)
/// \brief  ��ȡ��ȴ��������
/// \param path �����ļ���
DECLARE_METHOD_1(bool, CoolManager, LoadResource, const char*);

/// method: bool SetCommonCoolRange(int lPublicCoolCategory, int begin_cool_type, int end_cool_type);
/// \brief ���ø��๫����ȴ�Ĵ��������
/// \param lPublicCoolCategory ������ȴ����, ���ֵ���ܴ���10,Լ��
/// \param begin_team_type ��ȴ����ʼ���
/// \param end_team_type ��ȴ��������
DECLARE_METHOD_3(bool, CoolManager, SetCommonCoolRange, int, int, int);

/// method: bool SetCommonCoolNoEffectRange(int lPublicCoolCategory, int begin_cool_type, int end_cool_type);
/// \brief ���ø��๫����ȴ�Ĵ��������
/// \param lPublicCoolCategory ������ȴ����, ���ֵ���ܴ���10,Լ��
/// \param begin_cool_type ��ȴ����ʼ���
/// \param end_cool_type ��ȴ��������
DECLARE_METHOD_3(bool, CoolManager, SetCommonCoolNoEffectRange, int, int, int);

/// method: bool SetCommonTeamCoolRange(int lPublicCoolCategory, int begin_team_type, int end_team_type);
/// \brief ���ù�������ȴ�Ĵ��������
/// \param lPublicCoolCategory ������ȴ����, ���ֵ���ܴ���10,Լ��
/// \param begin_team_type ��ȴ����ʼ���
/// \param end_team_type ��ȴ��������
DECLARE_METHOD_3(bool, CoolManager, SetCommonTeamCoolRange, int, int, int);

/// method: bool SetCommonTeamCoolNoEffectRange(int lPublicCoolCategory, int begin_cool_type, int end_cool_type);
/// \brief ���ø��๫����ȴ�Ĵ��������
/// \param lPublicCoolCategory ������ȴ����, ���ֵ���ܴ���10,Լ��
/// \param begin_cool_type ��ȴ����ʼ���
/// \param end_cool_type ��ȴ��������
DECLARE_METHOD_3(bool, CoolManager, SetCommonTeamCoolNoEffectRange, int, int, int);

/// method: bool StartACool(int nCoolCategory, int TotalTime, int nCoolTeamIndex, int nBeginTime)
/// \brief  ����ָ���еĹ�������
/// \param lCoolCategory ��ȴ����
/// \param TotalTime ��ȴ�ּ�ʱ��
/// \param lCoolTeamIndex ��ȴ����
/// \param nBeginTime ��ȴ��ʼʱ��
DECLARE_METHOD_4(bool, CoolManager, StartACool, int, int, int, int);

/// method: bool StartACool(const int width, const int height)
/// \brief  ����Ĭ�ϵ���ȷ��С
/// \param width ��
/// \param height ��
DECLARE_METHOD_2(bool, CoolManager, SetDefCoolSize, int, int);

/// method: bool IsCooling(const int nCoolCategory, int nCoolTeam)
/// \brief  ����Ƿ�����׼ȷ
/// \param lCoolCategory ��ȴ����
DECLARE_METHOD_2(bool, CoolManager, IsCooling, int, int);

/// method: bool ClearCooling(const int nCoolCategory)
/// \brief  �������ID����ȴ
/// \param lCoolCategory ��ȴ����ID/����ȴID
DECLARE_METHOD_1(bool, CoolManager, ClearCooling, int);

/// method: bool ClearAllCooling()
/// \brief  ���������ȴ
DECLARE_METHOD_0(bool, CoolManager, ClearAllCooling);

/// method: bool UpdateCoolItem(long lCoolType, float seconds)
/// \brief  �ֶ�������ȴʱ��
/// \param lCoolType ��ȴ����
/// \param seconds ����ʱ��
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

// ��ʼ�����ɴ�����������ʵ�壩
bool CoolManager::Init(const IVarList& args)
{
	//m_MapCoolItems.clear();
	//m_vUnUsed.clear();

	GetCore()->AddExecute(this);
	// ���óɵ���ʵ��
	//GetCore()->SetSingle(this);

	return true;
}

// ��ָ�����ŷָ��ַ���
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

//�ַ������͵���������ת��Ϊ����
inline int convert_int(const char* szint, int default_int)
{
	if (szint == NULL)
		return default_int;

	return ::atoi(szint);
}

// ���������ļ�
bool CoolManager::LoadResource(const char* szPath)
{
	return true;
}

// ���ø��๫����ȴ�Ĵ��������
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

// ���ù�����ȴ����������
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

// ���ø��๫����ȴ�Ĵ��������
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

// ���ù�����ȴ����������
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
// �رգ���ɾ���������õĸ���ʵ�壬����Դ�ͷ�Ӧ�����������н��У�
bool CoolManager::Shut()
{
	GetCore()->RemoveExecute(this);

	return true;
}

// ÿ֡���ã��������ӵ����ж��У�
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

// �ֶ�������ȴʱ��
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
	// 1.��������ǹ�����ȴ�����¼��ID
	if (nCoolCategory < (int)m_vCommonCool.size())
	{
		if (m_vCommonCool[nCoolCategory].StartCool(nCoolCategory, nBeginTime, TotalTime))
		{
			m_vCommonCool[nCoolCategory].no_effect_index = nCoolTeamIndex;
		}

		return true;
	}

	// 2.�鹫����ȴ
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

	// 2.����ͨ��ȴ������, ����ȴ����������
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
	// 1������ȴ�Ƿ����ڽ���
	for (size_t i = 0 ; i < m_vCommonCool.size(); ++i)
	{
		if (nCoolCategory >= m_vCommonCool[i].begin_cool_index && 
			nCoolCategory < m_vCommonCool[i].end_cool_index)
		{
			// 1. ������ȴ�Ƿ�������ȴ
			if (m_vCommonCool[i].IsCooling())
			{
				return true;
			}
		}
	}

	// 2.�����Ƿ�������ȴ

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


		// �����������ȴ���򷵻�����ȴ
		if (self_item != NULL && self_item->IsCooling())
#else
		if (pCoolItem->IsCooling())
#endif
		{
			return true;
		}
	}

#ifdef USE_MAP
	// 3.���������Ƿ�������ȴ
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
			// �Ƿ�������Ĳ�������ȴӰ�����ȴID
			if (m_vSpecialCategory.find(nCoolCategory) != m_vSpecialCategory.end())
#else
			if (m_vSpecialCategory.Exists(nCoolCategory))
#endif
			{
				// ���ⲻ��ȴ
			}
			else
			{
				return true;
			}
		}
	}

	// 4.�鹫����ȴ�Ƿ����ڽ���
	for (size_t i = 0 ; i < m_vTeamCommonCool.size(); ++i)
	{
		if (nCoolTeam >= m_vTeamCommonCool[i].begin_cool_index && 
			nCoolTeam < m_vTeamCommonCool[i].end_cool_index)
		{
			if (m_vTeamCommonCool[i].IsCooling())
			{
				if (m_vTeamCommonCool[i].no_effect_index == nCoolTeam)
				{
					// �����ţ�����Ӱ��
					continue;
				}
				else if (nCoolTeam >= m_vTeamCommonCool[i].no_effect_begin && 
					nCoolTeam < m_vTeamCommonCool[i].no_effect_end)
				{
					// ������Զ����
					continue;
				}
#ifdef USE_MAP
				else if (m_vSpecialTeam.find(nCoolTeam) != m_vSpecialTeam.end())
#else
				else if (m_vSpecialTeam.Exists(nCoolTeam))
#endif
				{
					// ��ʱ����
					continue;
				}
#ifdef USE_MAP
				else if (m_vSpecialCategory.find(nCoolCategory) != m_vSpecialCategory.end())
#else
				else if (m_vSpecialCategory.Exists(nCoolCategory))
#endif
				{
					// ��������
					continue;
				}

				return true;
			}
		}
	}

	return false;
}

// �������ID����ȴ
bool CoolManager::ClearCooling(int nCoolCategory)
{
	// 1������ȴ
	if (nCoolCategory < (int)m_vCommonCool.size())
	{
		m_vCommonCool[nCoolCategory].ClearCooling();
		return true;
	}

	// 2.�鹫����ȴ
	for (size_t i = 0 ; i < m_vTeamCommonCool.size(); ++i)
	{
		if (m_vTeamCommonCool[i].m_CoolCategory == nCoolCategory)
		{
			// �����ţ�����Ӱ��
			m_vTeamCommonCool[i].ClearCooling();
			return true;
		}  
	}

	// 3.������ȴ
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

		// �����������ȴ���򷵻�����ȴ
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

// ���������ȴ
bool CoolManager::ClearAllCooling()
{
	// 1������ȴ
	size_t count = m_vCommonCool.size();

	for (size_t nCoolCategory = 0; nCoolCategory < count; ++nCoolCategory)
	{
		m_vCommonCool[nCoolCategory].ClearCooling();
	}

	// 2.�鹫����ȴ
	for (size_t i = 0 ; i < m_vTeamCommonCool.size(); ++i)
	{
		// �����ţ�����Ӱ��
		m_vTeamCommonCool[i].ClearCooling();
	}

	// 3.������ȴ
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
		// �����������ȴ���򷵻�����ȴ
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

// ���һ�����͵���ȷ��ÿ�����ͽ�����һ��;
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

// ����Ĭ����ȷ�Ĵ�С
bool CoolManager::SetDefCoolSize(int width, int height)
{
	m_vCommonCool[0].SetCoolSize(width, height);
	return true;
}

// ���ö���ʱ�����
void CoolManager::SetAnimationTime(int time)
{
	if (time != m_nAnimationTime)
	{
		m_nAnimationTime = time;
		CoolItem::s_nAnimationTime = time;
	}
}

// ���ö���������ʵ��
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

// ����ָ����߻�����ȷ
void CoolManager::DrawCooling(int lCoolCategory, 
							  int lCoolTeamIndex, IPainter* pPainter, 
							  long x, long y, long width, long height,
							  unsigned long dwColor, const char* pAnimation, bool round)
{
	CoolItem* pCoolItem = NULL;

	//��ȡ��Ҫ�軭��CD���CoolItem
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

	// 1. �����Ƿ�������ȴ
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

	// 2. ����ȴ�Ƿ����ڽ���
	if (GetCoolItem(lCoolTeamIndex, &pCoolItem) && bCoolTeam)
	{
		if (pCoolItem->IsCooling())
		{
			// �Ƿ�����ʱ��������ȴӰ�����ȴ
#ifdef USE_MAP
			if (m_vSpecialCategory.find(lCoolCategory) != m_vSpecialCategory.end())
#else
			if (m_vSpecialCategory.Exists(lCoolCategory))
#endif
			{
				// ����Ӱ��
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

	// 3. ��󹫹���ȴ�ڽ���,����й�����ȴ
	// �������Ҫ��ȴ,��ȴ���Ϳ���ɸ���
	size_t c;
	// 3.1 �鹫����ȴ
	for (c = 0; c < m_vTeamCommonCool.size(); ++c)
	{
		if (lCoolTeamIndex >= m_vTeamCommonCool[c].begin_cool_index && 
			lCoolTeamIndex < m_vTeamCommonCool[c].end_cool_index)
		{

			if (m_vTeamCommonCool[c].no_effect_index != lCoolTeamIndex)
			{
				if (m_vTeamCommonCool[c].IsCooling())
				{
					// �鹫����ȴ���ڽ���
					if (lCoolTeamIndex >= m_vTeamCommonCool[c].no_effect_begin && 
						lCoolTeamIndex < m_vTeamCommonCool[c].no_effect_end)
					{
						// ����
					}
#ifdef USE_MAP
					else if (m_vSpecialTeam.find(lCoolTeamIndex) != m_vSpecialTeam.end())
#else
					else if (m_vSpecialTeam.Exists(lCoolTeamIndex))
#endif
					{
						// �����飬���ܹ�������Ӱ��
					}
#ifdef USE_MAP
					else if (m_vSpecialCategory.find(lCoolCategory) != m_vSpecialCategory.end())
#else
					else if (m_vSpecialCategory.Exists(lCoolCategory))
#endif
					{
						// ���⼼�ܣ�����ȴ
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

	// 3.2 ��������ȴ
	for (c = 0; c < m_vCommonCool.size(); ++c)
	{
		// ����Ƿ����Դ��๫����ȴ��Χ
		if (lCoolCategory >= m_vCommonCool[c].begin_cool_index && 
			lCoolCategory < m_vCommonCool[c].end_cool_index)
		{
			// �Ǵ��๫����ȴ��Χ������ǵ�ǰ��ȴ����࣬����Ӱ��,�������ͬ�࣬���ܵ����ӹ��򸽼�
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

// ����ָ����ȴ��ͼ������ȴ
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