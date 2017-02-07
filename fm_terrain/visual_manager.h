//--------------------------------------------------------------------
// �ļ���:		visual_manager.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��9��25��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TVisualManagerAlloc& src) {}
};

// �����������

class Terrain;
class CTerrainZone;
class CZoneManager;

class CVisualManager
{
private:
	// ����״̬
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
	
	// �Ƿ����пɼ�����ȫ���������
	bool GetLoadFinish();
	// ���ؼ��ذٷֱ�
	float GetLoadPercent();
	// �������������
	size_t GetVisualCount() const;
	
	// ��ʼ��
	bool Init();
	// �ر�
	bool Shut();

	// ��ʱִ��
	void CheckLoading();
	// ���Ҫ���صĵ���������Ϣ
	bool AddLoadVisual(int zone_index, visual_info_t* pInfo);

private:
	CVisualManager(const CVisualManager&);
	CVisualManager& operator=(const CVisualManager&);

	// ���������Ƴ�
	bool RemoveByIndex(size_t index);

	// ����ģ��
	IVisBase* LoadModel(visual_info_t* visual, CTerrainZone* pZone, 
		const char* tex_paths);
	// ��������
	IVisBase* LoadParticle(visual_info_t* visual);
	// ���صƹ�
	IVisBase* LoadLight(visual_info_t* visual);
	// ��������
	IVisBase* LoadSound(visual_info_t* visual);
	// ���������
	IVisBase* LoadActor(visual_info_t* visual);
	// ��������
	IVisBase* LoadDecal(visual_info_t* visual);
	// ���ش�����
	IVisBase* LoadTrigger(visual_info_t* visual);
	// ������Чģ��
	IVisBase* LoadEffectModel(visual_info_t* visual);

	// �����ص�������
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
