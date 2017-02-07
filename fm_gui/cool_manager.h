//--------------------------------------------------------------------
// 文件名:		CoolManager.h
// 内  容:		冷却管理器
// 说  明:		
// 创建日期:	2008年8月27日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
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

// 此值用于增加冷却显示时间,
// 使玩家在看到冷却即将结束还有100ms时就能出招，
// 提高感受度
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
	// 当前是否正在冷却
	bool m_IsCooling;
	// 冷却类型
	long m_CoolCategory;
	// 冷却大分类
	//long m_CoolTeamIndex;
	// 冷却区域,从具体图片获得
	int m_nWidth;
	int m_nHeight;
	// 冷却计时
	long m_CoolCount;
	// 总冷却时间
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
	int begin_cool_index;	// 公共冷却包含的冷却起始分类号
	int end_cool_index;		// 公共冷却包含的冷却结束分类号
	int no_effect_begin;    // 不受其分类影响的特殊分类区间
	int no_effect_end;      // 不受其分类影响的特殊分类区间
	int no_effect_index;    // 此公共冷却发动的分类号,可变的
};

// CoolManager

class CoolManager: public IEntity
{
public:
	CoolManager();
	virtual ~CoolManager();

	// 初始化（可创建其他附属实体）
	virtual bool Init(const IVarList& args);
	// 关闭（可删除其他引用的附属实体，而资源释放应在析构函数中进行）
	virtual bool Shut();

	// 每帧调用（如果已添加到运行队列）
	virtual void Execute(float seconds);

	bool LoadResource(const char* szPath);

	// 设置各类公共冷却的大分类区间
	bool SetCommonCoolRange(int lPublicTeam, int begin_cool_type, int end_cool_type);

	// 设置公共冷却的免疫区间
	bool SetCommonCoolNoEffectRange(int lPublicTeam, int begin_cool_type, int end_cool_type);

	// 设置公共组冷却的大分类区间
	bool SetCommonTeamCoolRange(int lPublicTeam, int begin_cool_type, int end_cool_type);

	// 设置公共组冷却组免疫区间
	bool SetCommonTeamCoolNoEffectRange(int lPublicTeam, int begin_cool_type, int end_cool_type);

	// 根据冷却小分类，单独启动冷却
	bool StartACool(int nCoolCategory, int nCoolDownTime, int nCoolTeamIndex, int nBeginTime);

	// 检查当前对像是否在冷却，这时小分类可能不在冷却但大分类在冷却，因此需传入大分类
	bool IsCooling(int CoolCategory, int CoolTeam);

	// 清除传入ID的冷却
	bool ClearCooling(int CoolCategory);

	// 清除所有冷却
	bool ClearAllCooling();

	// 根据冷却小分类 添加一种类型的冷却，每种类型仅创建一次;
	bool AddCool(int CoolCategory);

	// 根据冷却小分类 删除一个冷却
	void DelCool(int CoolCategory);

	// 设置默认冷却的大小
	bool SetDefCoolSize(int width, int height);

	// 设置动画时间毫秒
	void SetAnimationTime(int time);
	int GetAnimationTime(){return m_nAnimationTime;};
	// 设置动画管理器实体
	void SetAniMng(PERSISTID id);
	PERSISTID GetAniMng();

	// 设置单个准确大小
	bool GetCoolItem(int nCoolCategory, CoolItem** pItem);

	// 根据默认准确大小绘制冷确
	void DrawCooling(int lCoolCategory, int lCoolTeamIndex, IPainter* pPainter, long x, long y, 
		unsigned long dwColor = 0x50000000, const char* pAnimation = NULL, bool round = false);

	// 根据指定宽高绘制冷确
	void DrawCooling(int lCoolCategory, int lCoolTeamIndex, IPainter* pPainter, long x, long y, 
		long width, long height,
		unsigned long dwColor = 0x50000000, const char* pAnimation = NULL, bool round = false);

    // 根据指定冷却的图绘制冷却
    void DrawCooling(int lCoolCategory, int lCoolTeamIndex, CImagePlayer* pImagePlayer, 
        long x, long y, long width, long height);

	// 手动更新冷却时间
	bool UpdateCoolItem(long lCoolType, float seconds);
private:
	// 根据配置判定冷却分组的相关性，
	// 从而判定start_cool_team在冷却时cur_cool_team是否在冷却
	bool IsCoolingBySetting(const int start_cool_team, const int cur_cool_team);

	// 公共冷却
	std::vector<CommonCoolItem> m_vCommonCool;

	// 组公共冷却
	std::vector<CommonCoolItem> m_vTeamCommonCool;

	// 这个列表只会添加不会减少,
#ifdef USE_MAP
	std::map<int, CoolItem*> m_MapCoolItems;
#else
	TPodHashMap<int, CoolItem*, TPodTraits<int>, TCoreAlloc> m_MapCoolItems;
#endif

	// 在界面动态析构时会将不使用的加到这个容器中，添加时会首先考虑未使用的随后才是创建新的
	std::vector<size_t> m_vUnUsed;

	// 冷确最后放动画的时间
	int m_nAnimationTime;

	// 特殊的不受组冷却影响的冷却ID
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

