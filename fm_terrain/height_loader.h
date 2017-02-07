//--------------------------------------------------------------------
// 文件名:		height_loader.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _HEIGHT_LOADER_H
#define _HEIGHT_LOADER_H

#include "../public/macros.h"
#include "../public/core_type.h"
#include "../visual/fm_header.h"
#include "../visual/i_res_loader.h"
#include "name_list.h"

#define BLOCK_ID_HEIGHT_0 '0TGH'
#define BLOCK_ID_HEIGHT_2 '2TGH'
#define BLOCK_ID_CHUNK_0 '0KHC'
#define BLOCK_ID_WATER_0 '0RTW'

// 地面高度加载器

class CTerrainZone;
class IBlockReader;

class CHeightLoader: public IResLoader
{
public:
	static CHeightLoader* NewInstance();

	static IBlockReader* CreateBlockReader(int block_id, void* context);

	// 计算分块的LOD误差
	static void CalcChunkErrors(const float* height_values, size_t cols,
		size_t row_offset, size_t col_offset, size_t chunk_scale,
		size_t lod_levels, float* lod_errors);
	// 计算分块的包围盒
	static void CalcChunkBoundBox(const float* height_values, size_t cols,
		size_t row_offset, size_t col_offset, size_t chunk_scale, float left, 
		float top, float unit_size, FmVec3& v_min, FmVec3& v_max);
	static void CalcBlockMinMaxY(const float* height_values, size_t cols, 
		size_t row, size_t col, size_t scale, float& min_y, float& max_y);

	// 分块信息
	struct chunk_info_t
	{
		FmVec3 vMin;	// 包围盒
		FmVec3 vMax;
		float fErrors[8];	// 每个LOD级别的误差
	};
	
	struct height_all_t
	{
		float* pValues;
		unsigned int* pNormals;
	};
	
	struct chunk_all_t
	{
		chunk_info_t* pChunks;
	};

	struct water_all_t
	{
		size_t nWaterNum;
		CNameList WaterNames;
		unsigned char* pWaterIndex;
	};
	
private:
	// 地图镜象旋转
	enum MIRROR_TYPES
	{
		MIRROR_NONE,
		MIRROR_HORIZON,		// 水平镜像
		MIRROR_VERTICAL,	// 垂直镜像
		MIRROR_H_V,			// 水平和垂直镜像
		MIRROR_MAX,
	};
	
public:
	CHeightLoader();
	virtual ~CHeightLoader();

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
	void SetOwner(CTerrainZone* pOwner);
	// 高度文件名
	void SetFileName(const char* file_name);
	// 位图文件名
	void SetBitmap(const char* file_name, int mirror);
	// 压缩高度文件名
	void SetGroundFile(const char* file_name);
	// 水面类型设计数据文件
	void SetWaterFile(const char* file_name);
	// 设置碰撞地块的尺寸
	void SetCollideScale(size_t value);
	
	// 保存高度文件
	static bool SaveBlockHeight(const char* file_name, CTerrainZone* pZone);
	static bool SaveBlockHeightCompress(const char* file_name,
		CTerrainZone* pZone);
	// 保存水面类型设计数据
	static bool SaveBlockWater(const char* file_name, CTerrainZone* pZone);
	
private:
	CHeightLoader(const CHeightLoader&);
	CHeightLoader& operator=(const CHeightLoader&);
	
	// 从位图文件加载
	bool LoadBitmapFile(const char* file_name, int mirror);
	// 设置为初始高度
	bool LoadInitHeight();
	// 加载高度信息
	bool LoadBlockHeight(const char* file_name);
	// 加载水面类型设计数据
	bool LoadBlockWater(const char* file_name);

	// 计算地面的法线
	bool CalcNormals();
	// 计算所有分块的数据
	bool CalcChunkInfo();
	// 初始化所有分块的数据
	bool InitChunkInfo();

private:
	CTerrainZone* m_pOwner;
	core_string m_strFileName;
	core_string m_strBitmap;
	core_string m_strGroundFile;
	core_string m_strWaterFile;
	int m_nMirror;
	size_t m_nRows;
	size_t m_nCols;
	size_t m_nCollideScale;
	size_t m_nLodLevels;
	size_t m_nChunkScale;
	size_t m_nChunkRows;
	size_t m_nChunkCols;
	float m_fInitHeight;
	float m_fLeft;
	float m_fTop;
	float m_fUnitSize;
	float m_fMinY;
	float m_fMaxY;
	
	height_all_t m_Height;
	chunk_all_t m_Chunk;
	water_all_t m_Water;
	float* m_pBlockMinY;
	float* m_pBlockMaxY;
};

#endif // _HEIGHT_LOADER_H
