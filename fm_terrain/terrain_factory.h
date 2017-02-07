//--------------------------------------------------------------------
// �ļ���:		terrain_factory.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��8��11��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TERRAIN_FACTORY_H
#define _TERRAIN_FACTORY_H

#include "../public/macros.h"
#include "../public/persistid.h"
#include "../public/core_mem.h"
#include "../utils/array_pod.h"

class TTerrainFactoryAlloc
{
public:
	TTerrainFactoryAlloc() {}
	~TTerrainFactoryAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TTerrainFactoryAlloc& src) {}
};

// ��Դ����

class Terrain;
class CTerrainZone;

class CTerrainFactory
{
public:
	CTerrainFactory();
	~CTerrainFactory();

	// ������Դ
	CTerrainZone* Create(Terrain* pTerrain, size_t index, size_t row, 
		size_t col);
	// ������Դ
	void Destroy(CTerrainZone* pResource);
	// ���ݶ���ID�����Դ
	CTerrainZone* Get(const PERSISTID& id) const;
	// ȫ��ɾ��
	void ReleaseAll();
	// �����Դ����
	size_t GetCount() const { return m_nCount; }
	
private:
	CTerrainFactory(const CTerrainFactory&);
	CTerrainFactory& operator=(const CTerrainFactory&);

	// �������к�
	unsigned int GenSerial();

private:
	TArrayPod<CTerrainZone*, 1, TTerrainFactoryAlloc> m_Resources;
	TArrayPod<unsigned int, 1, TTerrainFactoryAlloc> m_FreeList;
	unsigned int m_nSerial;
	size_t m_nCount;
};

#endif // _TERRAIN_FACTORY_H
