//--------------------------------------------------------------------
// 文件名:		DynamicWalkLoader.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年3月11日
// 创建人:		沈奇伟
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _DYNAMICWALKLOADER_H
#define _DYNAMICWALKLOADER_H

#include "dynamic_walkable.h"
#include "../visual/i_res_loader.h"

// 动态行走信息加载器

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

	// 释放
	virtual void Release();
	// 获得名字
	virtual const char* GetName();
	// 加载
	virtual bool Load(bool async);
	// 创建
	virtual bool Create(bool async);
	// 结束异步创建 
	virtual bool EndCreate(int category, bool succeed);

	// 资源主体
	void SetOwner(CDynamicWalkable* pOwner);
	// 高度文件名
	void SetFileName(const char* file_name);

	// 保存行走信息文件
	static bool SaveBlockWalk(const char* file_name, CDynamicWalkable* pWalk);

private:
	CDynamicWalkLoader(const CDynamicWalkLoader&);
	CDynamicWalkLoader& operator=(const CDynamicWalkLoader&);

	// 加载行走信息
	bool LoadBlockWalk(const char* file_name);

private:
	CDynamicWalkable* m_pOwner;
	TFastStr<char, 16, TCharTraits<char>, TCoreAlloc> m_strFileName;
	size_t m_nRows;
	size_t m_nCols;
	walk_all_t m_Walk;

};

#endif // _DYNAMICWALKLOADER_H
