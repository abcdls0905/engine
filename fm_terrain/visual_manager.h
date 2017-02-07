//--------------------------------------------------------------------
// 文件名:		visual_manager.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年9月25日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _VISUAL_MANAGER_H
#define _VISUAL_MANAGER_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"


#include "../public/i_interface.h"
#include "../visual/i_render.h"
#include "../utils/tick_timer.h"
#include "../utils/string_pod.h"
#include "../utils/array_pod.h"
#include "../visual/i_vis_base.h"
#include "visual_info.h"

class TVisualManagerAlloc
{
public:
	TVisualManagerAlloc() {}
	~TVisualManagerAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TVisualManagerAlloc& src) {}
};

// 地面物体管理

class Terrain;
class CTerrainZone;
class CZoneManager;

class CVisualManager
{
private:
	// 加载状态
	enum LOAD_STATE_ENUM
	{
		LOADSTATE_BEGIN,
		LOADSTATE_LOADCACHE,
		LOADSTATE_LOADING,
		LOADSTATE_SUCCEED,
		LOADSTATE_FAILED,
	};

public:
	CVisualManager();
	~CVisualManager();

	void SetTerrain(Terrain* value);
	
	// 是否所有可见区域全部加载完成
	bool GetLoadFinish();
	// 返回加载百分比
	float GetLoadPercent();
	// 获得物体总数量
	size_t GetVisualCount() const;
	
	// 初始化
	bool Init();
	// 关闭
	bool Shut();

	// 定时执行
	void CheckLoading();
	// 添加要加载的地面物体信息
	bool AddLoadVisual(int zone_index, visual_info_t* pInfo);

private:
	CVisualManager(const CVisualManager&);
	CVisualManager& operator=(const CVisualManager&);

	// 根据索引移除
	bool RemoveByIndex(size_t index);

	// 加载模型
	IVisBase* LoadModel(visual_info_t* visual, CTerrainZone* pZone, 
		const char* tex_paths);
	// 加载粒子
	IVisBase* LoadParticle(visual_info_t* visual);
	// 加载灯光
	IVisBase* LoadLight(visual_info_t* visual);
	// 加载声音
	IVisBase* LoadSound(visual_info_t* visual);
	// 加载组合体
	IVisBase* LoadActor(visual_info_t* visual);
	// 加载贴花
	IVisBase* LoadDecal(visual_info_t* visual);
	// 加载触发器
	IVisBase* LoadTrigger(visual_info_t* visual);
	// 加载特效模型
	IVisBase* LoadEffectModel(visual_info_t* visual);

	// 检查加载地面物体
	bool CheckVisuals(float seconds);

private:
	Terrain* m_pTerrain;
	CZoneManager* m_pZoneManager;
	CTickTimer m_CheckTimer;
	TArrayPod<visual_info_t*, 1, TVisualManagerAlloc> m_Visuals;
	TArrayPod<size_t, 1, TVisualManagerAlloc> m_FreeList;
	TStringPod<char, size_t, TStringTraits<char>, 
		TVisualManagerAlloc> m_Indices;
	bool m_bNeedCheck;
	size_t m_nCheckIndex;
};

#endif // _VISUAL_MANAGER_H
