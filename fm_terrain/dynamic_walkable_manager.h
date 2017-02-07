//--------------------------------------------------------------------
// �ļ���:		DynamicWalkableManager.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��3��1��
// ������:		����ΰ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _DYNAMICWALKABLEMANAGER_H
#define _DYNAMICWALKABLEMANAGER_H

#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../utils/array_pod.h"

class CDynamicWalkable;
class IVisBase;
class Terrain;

class CDynamicWalkableManager
{
private:
	typedef TFastStr<char, 16, TCharTraits<char>, TCoreAlloc> FileName;
public:
	CDynamicWalkableManager();
	~CDynamicWalkableManager();

	void SetTerrain(Terrain* pTerrain);

	// ��ȡģ����
	result_string GetVisualName(IVisBase* pVisBase) const;
	// ��ȡģ����չ��
	result_string GetVisualExtName(IVisBase* pVisBase) const;

	// ��Ӷ�̬���
	bool AddDynamicWalkable(IVisBase* pVisBase);
	// ��Ӷ�̬�����������)
	bool AddDynamicWalkable2(IVisBase* pVisBase);
	// ɾ����̬���
	bool DeleteDynamicWalkable(IVisBase* pVisBase);

	// ���ض�̬�����ײ
	bool LoadDynamicWalkable(IVisBase* pVisBase);
	// ж�ض�̬�����ײ
	bool UnloadDynamicWalkable(IVisBase* pVisBase);

	// ��ȡ�����������Ϣ
	CDynamicWalkable* GetDynamicWalkable(IVisBase* pVisBase) const;
private:
	Terrain* m_pTerrain;
	TArrayPod<CDynamicWalkable*, 1, TCoreAlloc> m_VisualWalkableArr;
};

#endif // _DYNAMICWALKABLEMANAGER_H
