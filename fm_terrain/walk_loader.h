//--------------------------------------------------------------------
// �ļ���:		walk_loader.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��8��8��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _WALK_LOADER_H
#define _WALK_LOADER_H

#include "terrain_walkable.h"

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"
#include "../visual/i_res_loader.h"
#include "../public/i_interface.h"

// ������Ϣ������

class CTerrainZone;
class IBlockReader;

class CWalkLoader: public IResLoader
{
public:
	static CWalkLoader* NewInstance();

	static IBlockReader* CreateBlockReader(int block_id, void* context);

	struct walk_all_t
	{
		unsigned char* pWaterHeight;
		size_t nWaterHeightSize;
		unsigned char* pWalkMarker;
		size_t nWalkMarkerSize;
		unsigned char* pFloorHeights[MAX_FLOOR_NUM];
		size_t nFloorHeightsSize[MAX_FLOOR_NUM];
		unsigned char* pSpaceHeights[MAX_FLOOR_NUM];
		size_t nSpaceHeightsSize[MAX_FLOOR_NUM];
		unsigned char* pFloorMarkers[MAX_FLOOR_NUM];
		size_t nFloorMarkersSize[MAX_FLOOR_NUM];
		//unsigned char* pStandMarkers[MAX_FLOOR_NUM];
		//size_t nStandMarkersSize[MAX_FLOOR_NUM];
		//unsigned char* pWallMarkers[MAX_FLOOR_NUM];
		//size_t nWallMarkersSize[MAX_FLOOR_NUM];
		size_t nFloorCount;
		//size_t nWallCount;
	};
	
public:
	CWalkLoader();
	virtual ~CWalkLoader();

	// �ͷ�
	virtual void Release();
	// �������
	virtual const char* GetName();
	// ����
	virtual bool Load(bool async);
	// ����
	virtual bool Create(bool async);
	// �����첽���� 
	virtual bool EndCreate(int category, bool succeed);
	
	// ��Դ����
	void SetOwner(CTerrainZone* pOwner);
	// �߶��ļ���
	void SetFileName(const char* file_name);
	
	// ����������Ϣ�ļ�
	static bool SaveBlockWalk(const char* file_name, CTerrainZone* pZone);
	
private:
	CWalkLoader(const CWalkLoader&);
	CWalkLoader& operator=(const CWalkLoader&);
	
	// ����������Ϣ
	bool LoadBlockWalk(const char* file_name);

private:
	CTerrainZone* m_pOwner;
	core_string m_strFileName;
	size_t m_nRows;
	size_t m_nCols;
	walk_all_t m_Walk;
};

#endif // _WALK_LOADER_H
