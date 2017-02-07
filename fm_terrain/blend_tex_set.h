//--------------------------------------------------------------------
// 文件名:		blend_tex_set.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年7月3日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _BLEND_TEX_SET_H
#define _BLEND_TEX_SET_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../utils/array_pod.h"

//#include "../visual/i_dx_render.h"

// 地面贴图集合

class CBlendTex;
class Terrain;

class CBlendTexSet
{
public:
	CBlendTexSet();
	~CBlendTexSet();

	void SetTerrain(Terrain* value) { m_pTerrain = value; }
	
	// 释放所有贴图
	void ReleaseAll();

	// 查找
	bool Find(const char* name) const;
	bool FindIndex(const char* name, size_t* index) const;
	// 添加
	bool Add(const char* name, const char* path);

	// 获得数量
	size_t GetCount() const { return m_Textures.size(); }

	// 获得指定索引的混合贴图
	CBlendTex* GetByIndex(size_t index) const
	{
		Assert(index < m_Textures.size());

		return m_Textures[index];
	}

	// 贴图是否全部完成加载
	bool GetLoadFinish();

private:
	CBlendTexSet(const CBlendTexSet&);
	CBlendTexSet& operator=(const CBlendTexSet&);

private:
	Terrain* m_pTerrain;
	TArrayPod<CBlendTex*, 1, TCoreAlloc> m_Textures;
};

#endif // _BLEND_TEX_SET_H
