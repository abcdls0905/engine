//--------------------------------------------------------------------
// �ļ���:		DynamicWalkLoader.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��3��11��
// ������:		����ΰ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _DYNAMICWALKLOADER_H
#define _DYNAMICWALKLOADER_H

#include "dynamic_walkable.h"
#include "../visual/i_res_loader.h"

// ��̬������Ϣ������

class IBlockReader;

class CDynamicWalkLoader : public IResLoader
{
public:
	static CDynamicWalkLoader* NewInstance();

	static IBlockReader* CreateBlockReader(int block_id, void* context);
	
	struct walk_all_t
	{
		unsigned char* pFloorHeights[MAX_FLOOR_NUM];
		size_t nFloorHeightsSize[MAX_FLOOR_NUM];
		unsigned char* pSpaceHeights[MAX_FLOOR_NUM];
		size_t nSpaceHeightsSize[MAX_FLOOR_NUM];
		unsigned char* pFloorMarkers[MAX_FLOOR_NUM];
		size_t nFloorMarkersSize[MAX_FLOOR_NUM];
		size_t nFloorCount;
		float fAngleY;
		float fHeight;
		float fRadius;
		float fBoxSizeX;
		float fBoxSizeZ;
	};

public:
	CDynamicWalkLoader();
	virtual ~CDynamicWalkLoader();

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
	void SetOwner(CDynamicWalkable* pOwner);
	// �߶��ļ���
	void SetFileName(const char* file_name);

	// ����������Ϣ�ļ�
	static bool SaveBlockWalk(const char* file_name, CDynamicWalkable* pWalk);

private:
	CDynamicWalkLoader(const CDynamicWalkLoader&);
	CDynamicWalkLoader& operator=(const CDynamicWalkLoader&);

	// ����������Ϣ
	bool LoadBlockWalk(const char* file_name);

private:
	CDynamicWalkable* m_pOwner;
	TFastStr<char, 16, TCharTraits<char>, TCoreAlloc> m_strFileName;
	size_t m_nRows;
	size_t m_nCols;
	walk_all_t m_Walk;

};

#endif // _DYNAMICWALKLOADER_H
