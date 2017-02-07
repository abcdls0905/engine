//--------------------------------------------------------------------
// 文件名:		terrain_grass.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年12月4日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TERRAIN_GRASS_H
#define _TERRAIN_GRASS_H

#include "../public/module.h"


#include "../visual/i_render.h"
#include "terrain_chunk.h"
// 地面花草

class Terrain;
class GrassType;

class CTerrainGrass
{
private:
	enum { MAX_GRASS_NUM = 256 };

public:
	CTerrainGrass();
	~CTerrainGrass();

	void SetTerrain(Terrain* value) { m_pTerrain = value; }

	// 加载
	bool Load();
	
	// 渲染
	void Realize();

	// 所有贴图是否加载完成
	bool GetLoadFinish();

	// 查找
	bool FindIndex(const char* name, size_t& index) const;
	bool Find(const char* name) const;

	// 获得数量
	size_t GetCount() const { return m_nGrassCount; }

	// 获得指定索引花草
	GrassType* GetByIndex(size_t index) const
	{
		Assert(index < m_nGrassCount);

		return m_GrassTypes[index];
	}
	
	// 创建
	GrassType* Create(const char* name);
	// 删除
	bool DeleteByIndex(size_t index);
	// 清空
	void Clear();

private:
	CTerrainGrass(const CTerrainGrass&);
	CTerrainGrass& operator=(const CTerrainGrass&);

	void ReleaseAll();

private:
	Terrain* m_pTerrain;
	GrassType* m_GrassTypes[MAX_GRASS_NUM];
	size_t m_nGrassCount;
	TArrayPod<CTerrainChunk::chunk_grass_cb_per_render_t, 1> m_gassrender;//记录次草的渲染状态
};

#endif // _TERRAIN_GRASS_H
