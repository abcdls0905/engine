//--------------------------------------------------------------------
// �ļ���:		CoolManager.h
// ��  ��:		��ȴ������
// ˵  ��:		
// ��������:	2008��8��27��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef Cool_h_2008_8_27
#define Cool_h_2008_8_27

#include "../public/core_mem.h"
#include "../utils/pod_hash_map.h"
#include "../public/i_entity.h"
#include "gui_utils.h"
//#include "../visual/ipainter.h"
//#include <windows.h>
#include <vector>
#include <map>

// ��ֵ����������ȴ��ʾʱ��,
// ʹ����ڿ�����ȴ������������100msʱ���ܳ��У�
// ��߸��ܶ�
#define COOL_ADD_SHOWTIME 100
#define COOL_PI 3.1415926f 

#define USE_MAP

class CoolManager;
class ImageAnimationManager;
class CImagePlayer;

// CoolItem

class CoolItem
{
private:
	friend class CoolManager;
	friend class ImageGrid;

	struct Trangle
	{
		point_t point[3];
	};

public:
	CoolItem()
	{
		m_CoolCategory = 0;
		//m_CoolTeamIndex = 0;
		m_IsCooling = false;
		m_CoolCount = 0;
		m_CoolTime = 1;
		m_nWidth = 32;
		m_nHeight = 32;
	}

	void SetCoolSize(int width, int height)
	{
		if (m_nWidth != width || m_nHeight != height)
		{
			m_nWidth = width;
			m_nHeight = height;
			RestoreCoolTrangles();
		}
	}

	void SetCoolCategory(long lCoolCategory)
	{
		m_CoolCategory = lCoolCategory;
	}

	long GetCoolCategory() { return m_CoolCategory; }

	long GetCoolTime(){return m_CoolTime;}

	void Clear();

	bool IsCooling() 
	{ 
		return m_IsCooling || m_CoolCount + COOL_ADD_SHOWTIME < m_CoolTime; 
	}

	void ClearCooling()
	{
		m_CoolCount = 0;
		m_CoolTime = 1;
		m_IsCooling = false;
	}
protected:
	bool StartCool(long CoolCategory, long BeginTime, long CoolTime);
	void UpdateCool(float fliptime);
	void DrawCooling(IPainter* pPainter, long x, long y, unsigned long dwColor, 
		ImageAnimationManager * pImageAnimation, const char* pAnimation);
	void DrawCooling(IPainter* pPainter, long x, long y, 
		long width, long height, unsigned long dwColor, 
		ImageAnimationManager * pImageAnimation, const char* pAnimation);
	void DrawCoolingRound(IPainter* pPainter, long x, long y, 
		long width, long height, unsigned long dwColor);
	void RestoreCoolTrangles();

    void DrawCooling(CImagePlayer* pImagePlayer, long x, long y, long width, long height);

private:
	// ��ǰ�Ƿ�������ȴ
	bool m_IsCooling;
	// ��ȴ����
	long m_CoolCategory;
	// ��ȴ�����
	//long m_CoolTeamIndex;
	// ��ȴ����,�Ӿ���ͼƬ���
	int m_nWidth;
	int m_nHeight;
	// ��ȴ��ʱ
	long m_CoolCount;
	// ����ȴʱ��
	long m_CoolTime;

	Trangle m_vTrangles[5];

public:

	static long s_nAnimationTime;
};

class CommonCoolItem : public CoolItem
{
public:
	CommonCoolItem():begin_cool_index(0),
		end_cool_index(0),no_effect_index(0),
		no_effect_begin(0),no_effect_end(0)
	{}
	int begin_cool_index;	// ������ȴ��������ȴ��ʼ�����
	int end_cool_index;		// ������ȴ��������ȴ���������
	int no_effect_begin;    // ���������Ӱ��������������
	int no_effect_end;      // ���������Ӱ��������������
	int no_effect_index;    // �˹�����ȴ�����ķ����,�ɱ��
};

// CoolManager

class CoolManager: public IEntity
{
public:
	CoolManager();
	virtual ~CoolManager();

	// ��ʼ�����ɴ�����������ʵ�壩
	virtual bool Init(const IVarList& args);
	// �رգ���ɾ���������õĸ���ʵ�壬����Դ�ͷ�Ӧ�����������н��У�
	virtual bool Shut();

	// ÿ֡���ã��������ӵ����ж��У�
	virtual void Execute(float seconds);

	bool LoadResource(const char* szPath);

	// ���ø��๫����ȴ�Ĵ��������
	bool SetCommonCoolRange(int lPublicTeam, int begin_cool_type, int end_cool_type);

	// ���ù�����ȴ����������
	bool SetCommonCoolNoEffectRange(int lPublicTeam, int begin_cool_type, int end_cool_type);

	// ���ù�������ȴ�Ĵ��������
	bool SetCommonTeamCoolRange(int lPublicTeam, int begin_cool_type, int end_cool_type);

	// ���ù�������ȴ����������
	bool SetCommonTeamCoolNoEffectRange(int lPublicTeam, int begin_cool_type, int end_cool_type);

	// ������ȴС���࣬����������ȴ
	bool StartACool(int nCoolCategory, int nCoolDownTime, int nCoolTeamIndex, int nBeginTime);

	// ��鵱ǰ�����Ƿ�����ȴ����ʱС������ܲ�����ȴ�����������ȴ������贫������
	bool IsCooling(int CoolCategory, int CoolTeam);

	// �������ID����ȴ
	bool ClearCooling(int CoolCategory);

	// ���������ȴ
	bool ClearAllCooling();

	// ������ȴС���� ���һ�����͵���ȴ��ÿ�����ͽ�����һ��;
	bool AddCool(int CoolCategory);

	// ������ȴС���� ɾ��һ����ȴ
	void DelCool(int CoolCategory);

	// ����Ĭ����ȴ�Ĵ�С
	bool SetDefCoolSize(int width, int height);

	// ���ö���ʱ�����
	void SetAnimationTime(int time);
	int GetAnimationTime(){return m_nAnimationTime;};
	// ���ö���������ʵ��
	void SetAniMng(PERSISTID id);
	PERSISTID GetAniMng();

	// ���õ���׼ȷ��С
	bool GetCoolItem(int nCoolCategory, CoolItem** pItem);

	// ����Ĭ��׼ȷ��С������ȷ
	void DrawCooling(int lCoolCategory, int lCoolTeamIndex, IPainter* pPainter, long x, long y, 
		unsigned long dwColor = 0x50000000, const char* pAnimation = NULL, bool round = false);

	// ����ָ����߻�����ȷ
	void DrawCooling(int lCoolCategory, int lCoolTeamIndex, IPainter* pPainter, long x, long y, 
		long width, long height,
		unsigned long dwColor = 0x50000000, const char* pAnimation = NULL, bool round = false);

    // ����ָ����ȴ��ͼ������ȴ
    void DrawCooling(int lCoolCategory, int lCoolTeamIndex, CImagePlayer* pImagePlayer, 
        long x, long y, long width, long height);

	// �ֶ�������ȴʱ��
	bool UpdateCoolItem(long lCoolType, float seconds);
private:
	// ���������ж���ȴ���������ԣ�
	// �Ӷ��ж�start_cool_team����ȴʱcur_cool_team�Ƿ�����ȴ
	bool IsCoolingBySetting(const int start_cool_team, const int cur_cool_team);

	// ������ȴ
	std::vector<CommonCoolItem> m_vCommonCool;

	// �鹫����ȴ
	std::vector<CommonCoolItem> m_vTeamCommonCool;

	// ����б�ֻ����Ӳ������,
#ifdef USE_MAP
	std::map<int, CoolItem*> m_MapCoolItems;
#else
	TPodHashMap<int, CoolItem*, TPodTraits<int>, TCoreAlloc> m_MapCoolItems;
#endif

	// �ڽ��涯̬����ʱ�Ὣ��ʹ�õļӵ���������У����ʱ�����ȿ���δʹ�õ������Ǵ����µ�
	std::vector<size_t> m_vUnUsed;

	// ��ȷ���Ŷ�����ʱ��
	int m_nAnimationTime;

	// ����Ĳ�������ȴӰ�����ȴID
#ifdef USE_MAP
	std::map<int, int> m_vSpecialCategory;
	std::map<size_t, bool> m_vSpecialTeam;
#else
	TPodHashMap<int, int, TPodTraits<int>, TCoreAlloc> m_vSpecialCategory;
	TPodHashMap<size_t, bool, TPodTraits<size_t>, TCoreAlloc> m_vSpecialTeam;
#endif
	//
	PERSISTID m_ImageAmngID;

	unsigned int m_i64TimeCount;
};

#endif //Cool_h_2008_8_27

