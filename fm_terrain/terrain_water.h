//--------------------------------------------------------------------
// 文件名:		terrain_water.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年11月30日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TERRAIN_WATER_H
#define _TERRAIN_WATER_H

#include "../public/module.h"


#include "../visual/i_render.h"

// 地形水面

class Terrain;
class SeaWater;

class CTerrainWater
{
private:
	enum { MAX_WATER_NUM = 256 };

public:
	CTerrainWater();
	~CTerrainWater();

	void SetTerrain(Terrain* value) { m_pTerrain = value; }

	// 加载
	bool Load();
	// 卸载
	bool Unload();
	
	// 渲染
	void Realize();

	// 所有贴图是否加载完成
	bool GetLoadFinish();

	// 查找
	bool FindIndex(const char* name, size_t& index) const;
	bool Find(const char* name) const;
	// 获得数量
	size_t GetCount() const;
	// 获得指定索引水面
	SeaWater* GetByIndex(size_t index) const;
	
	// 创建
	SeaWater* Create(const char* name);
	// 删除
	bool DeleteByIndex(size_t index);
	// 清空
	void Clear();

private:
	CTerrainWater(const CTerrainWater&);
	CTerrainWater& operator=(const CTerrainWater&);

	void ReleaseAll();

private:
	Terrain* m_pTerrain;
	SeaWater* m_Waters[MAX_WATER_NUM];
	size_t m_nWaterCount;
};

#endif // _TERRAIN_WATER_H

