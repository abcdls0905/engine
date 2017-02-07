//--------------------------------------------------------------------
// 文件名:		grass_type.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年12月4日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _GRASS_TYPE_H
#define _GRASS_TYPE_H

#include "../public/module.h"
#include "../public/core_type.h"

#include "../visual/i_render.h"
#include "../visual/i_resource.h"
#include "../visual/shader_list.h"
#include "../utils/array_pod.h"
#include "../utils/pod_hash_map.h"
#include "terrain_chunk.h"

class TTerrainGrassShaderAlloc
{
public:
	TTerrainGrassShaderAlloc() {}
	~TTerrainGrassShaderAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TTerrainGrassShaderAlloc& src) {}
};


// 花草类型
class Terrain;
 
class GrassType: public IEntity
{
private:
	enum GRASS_FLAG_SHIFT_ENUM
	{
		GRASS_FLAG_FOGLINEAR,
		GRASS_FLAG_FOGEXP,
		GRASS_FLAG_SHADOWEDMAP,
		GRASS_FLAG_PRELIGHT,
		GRASS_FLAG_LIGHTMAP,
		GRASS_FLAG_EARLYZ,
		GRASS_FLAG_MAX,
	};


	struct grass_info_t
	{
		float fPosiX;		// 位置
		float fPosiZ;
		float fWidth;		// 尺寸
		float fHeight;
		float fDirX;		// 方向
		float fDirY;
		float fDirZ;
	};
public:	
	struct grass_vertex_t
	{
		float x;
		float y;
		float z;
		float tu;
		float tv;
		float height;		// 顶点相对于底部的高度
		float angle;		// 方向角度
	};



public:
	GrassType();
	virtual ~GrassType();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	// 加载资源
	bool Load();
	
	// 渲染
	void Realize(TArrayPod<CTerrainChunk::chunk_grass_cb_per_render_t, 1>& gassrender);

	void SetTerrain(Terrain* value);

	// 类型名字
	void SetName(const char* value) { m_strName = value; }
	const char* GetName() const { return m_strName.c_str(); }

	// 名字的哈希值
	void SetHash(unsigned int value) { m_nHash = value; }
	unsigned int GetHash() const { return m_nHash; }

	// 设置索引值
	void SetIndex(size_t value) { m_nIndex = value; }

	// 贴图文件名
	void SetTexture(const char* value) { m_strTexture = value; }
	const char* GetTexture() const { return m_strTexture.c_str(); }

	// 贴图尺寸
	void SetTexScale(int value) { m_nTexScale = value; }
	int GetTexScale() const { return m_nTexScale; }

	// 宽度方向的贴图分格数量
	void SetFrameWidth(int value) { m_nFrameWidth = value; }
	int GetFrameWidth() const { return m_nFrameWidth; }
	
	// 高度方向的贴图分格数量
	void SetFrameHeight(int value) { m_nFrameHeight = value; }
	int GetFrameHeight() const { return m_nFrameHeight; }

	// 最小宽度
	void SetMinWidth(float value) { m_fMinWidth = value; }
	float GetMinWidth() const { return m_fMinWidth; }
	
	// 最大宽度
	void SetMaxWidth(float value) { m_fMaxWidth = value; }
	float GetMaxWidth() const { return m_fMaxWidth; }
	
	// 最小高度
	void SetMinHeight(float value) { m_fMinHeight = value; }
	float GetMinHeight() const { return m_fMinHeight; }
	
	// 最大高度
	void SetMaxHeight(float value) { m_fMaxHeight = value; }
	float GetMaxHeight() const { return m_fMaxHeight; }
	
	// 最小倾斜
	void SetMinPitch(float value) { m_fMinPitch = value; }
	float GetMinPitch() const { return m_fMinPitch; }
	
	// 最大倾斜
	void SetMaxPitch(float value) { m_fMaxPitch = value; }
	float GetMaxPitch() const { return m_fMaxPitch; }

	// 范围乘数因子
	void SetScaleFactor(float value) { m_fScaleFactor = value; }
	float GetScaleFactor() const { return m_fScaleFactor; }

	// 强制刷新
	void SetMustRefresh(bool value) { m_bMustRefresh = value; }
	bool GetMustRefresh() const { return m_bMustRefresh; }

	// 获得贴图指针
	ITexture* GetTex() const { return m_pTexture; }

	// 激活之
	void Activate()
	{
		m_bActive = true;

		if (NULL == m_pTexture)
		{
			Load();
		}
	}

	// 是否激活
	bool GetActive() const { return m_bActive; }

private:
	GrassType(const GrassType&);
	GrassType& operator=(const GrassType&);

	// 生成地形分块内的花草
	IStaticVB* Generate(CTerrainChunk* pChunk, size_t& count);
	// 进行渲染前的准备
	void PrepareRealize();
	// 延迟着色
	void DrawGBuffer(TArrayPod<CTerrainChunk::chunk_grass_cb_per_render_t, 1>& gassrender);
	// 设置常量缓冲
	void FillGroupCB(CTerrainChunk* pChunk);

	static void RenderChunkGrass(void* pdata);
	static void RenderEarlyZChunkGrass(void* pdata);

	CTerrainChunk::grass_shader_handle_t* GetShaderHandle(bool fog_linear, bool fog_exp, bool dynamic_shadow,bool prelight, bool lightmap,bool bEarlyZ = false);
private:
	Terrain* m_pTerrain;
	IRender* m_pRender;
	core_string m_strTexture;
	core_string m_strName;
	unsigned int m_nHash;
	size_t m_nIndex;
	bool m_bActive;
	bool m_bMustRefresh;
	int m_nTexScale;
	int m_nFrameWidth;
	int m_nFrameHeight;
	float m_fMinWidth;
	float m_fMaxWidth;
	float m_fMinHeight;
	float m_fMaxHeight;
	float m_fMinPitch;
	float m_fMaxPitch;
	float m_fScaleFactor;
	float m_fWinForce;
	FmVec3 m_vWinDir;
	FmVec3 m_vPlayerPos;
	ITexture* m_pTexture;
	TShaderList<IVertexShader, GRASS_FLAG_MAX> m_VSList;
	TShaderList<IPixelShader, GRASS_FLAG_MAX> m_PSList;

	TPodHashMap<unsigned int, CTerrainChunk::grass_shader_handle_t*, TPodTraits<unsigned int>, TTerrainGrassShaderAlloc> m_ShaderHandleMap;

	static TArrayPod<CTerrainChunk::chunk_grass_cb_per_render_t, 1>* p_s_gassrender;
};

#endif // _GRASS_TYPE_H
