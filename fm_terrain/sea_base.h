//--------------------------------------------------------------------
// 文件名:		sea_base.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SEA_BASE_H
#define _SEA_BASE_H

#include "../public/macros.h"
#include "../public/i_interface.h"
#include "../visual/i_render.h"
#include "../visual/i_texture.h"
#include "../visual/shader_list.h"
#include "sea_data.h"

#include "../utils/array_pod.h"
#include "../utils/pod_hash_map.h"

#include "terrain_chunk.h"
// 水面

class Terrain;
//class CTerrainChunk;

class TTerrainSeaBaseShaderAlloc
{
public:
	TTerrainSeaBaseShaderAlloc() {}
	~TTerrainSeaBaseShaderAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TTerrainSeaBaseShaderAlloc& src) {}
};


class CSeaBase
{
private:
	enum { MAX_RENDER_BLOCK = 1024 };

	enum WATER_FLAG_SHIFT_NUM
	{
		WATER_FLAG_WAVELESS,
		WATER_FLAG_USE_REFLECTION,
		WATER_FLAG_USE_REFRACTION,
		WATER_FLAG_USE_DEPTHMAP,
		WATER_FLAG_SUN_REFLECTION,
		WATER_FLAG_BORDER,
		WATER_FLAG_UNDERWATER,
		WATER_FLAG_WHITEWATER,
		WATER_FLAG_FOGLINEAR,
		WATER_FLAG_FOGEXP,
		WATER_FLAG_USE_INTZ,//移动目前没有这玩意，都是直接拿z/w进行反算的
		WATER_FLAG_EASY,//简易效果(无顶点波动效果)
		WATER_FLAG_MAX,
	};

	enum CAUSTIC_PS_SHIFT_ENUM
	{
		CAUSTIC_PS_SHADOW,
		CAUSTIC_PS_MAX,
	};



public:
	CSeaBase();
	~CSeaBase();

	void SetTerrain(Terrain* value) { m_pTerrain = value; }
	void SetRender(IRender* value) { m_pRender = value; }

	void Realize();

	// 加载资源
	bool Load();
	// 卸载资源
	bool Unload();

	// 获得水面数据
	sea_data_t* GetSeaData() { return &m_SeaData; }

	// 海面基本高度
	void SetBaseHeight(float value) { m_fBaseHeight = value; }
	float GetBaseHeight() const { return m_fBaseHeight; }

	// 当前海面高度
	float GetCurrentHeight(float x, float y);

	// 最大高度差
	void SetMaxSeaHeight(float value) { m_fMaxSeaHeight = value; }
	float GetMaxSeaHeight() const { return m_fMaxSeaHeight; }

	// 是否可见
	void SetVisible(bool value) { m_bVisible = value; }
	bool GetVisible() const { return m_bVisible; }

	// 索引值
	void SetIndex(size_t value) { m_nIndex = value; }
	size_t GetIndex() const { return m_nIndex; }

	// 获得贴图指针
	ITexture* GetWaterNormalMap1() const { return m_pWaterNormalMap1; }
	ITexture* GetWaterNormalMap2() const { return m_pWaterNormalMap2; }
	ITexture* GetWaterNormalMap3() const { return m_pWaterNormalMap3; }
	ITexture* GetWaterNormalMap4() const { return m_pWaterNormalMap4; }
	ITexture* GetWhiteWaterMap() const { return m_pWhiteWaterMap; }
	ITexture* GetCausticMap() const { return m_pCausticMap; }

	// 创建法线贴图
	bool CreateNormalMap(const char* pszFileName1, const char* pszFileName2, 
		const char* pszFileName3, const char* pszFileName4);
	// 创建白色水域贴图
	bool CreateWhiteWaterMap(const char* pszFileName);
	// 创建水下刻蚀贴图
	bool CreateCausticMap(const char* pszFileName);
	// 创建噪声蚀贴图
	bool CreateNoiseMap(const char* pszFileName);

	// 导出反射贴图
	bool DumpReflectMap(const char* file_name);

private:
	CSeaBase(const CSeaBase&);
	CSeaBase& operator=(const CSeaBase&);

	void ReleaseAll();
	// 刷新需要渲染的水面块
	void RefreshBlocks();
	// 生成环境反射贴图
	void GenerateEnvMap();
	// 渲染水下刻蚀
	void DrawCaustic();
	// 获得波浪的平均高度
	float GetAverageWaveHeight();

	CTerrainChunk::chunk_water_shader_handle_t* GetShaderHandle(size_t flags);

	static void DrawChunksShaderWaterBatch(void* pdata);

private:
	Terrain* m_pTerrain;
	IRender* m_pRender;
	IRenderContext* m_pContext;
	ISceneView* m_pSeaSceneView;
	sea_data_t m_SeaData;
	float m_fBaseHeight;		// 海平面高度
	float m_fMaxSeaHeight;
	bool m_bVisible;
	bool m_bUnderWater;
	size_t m_nIndex;
	float m_fTimes;
	TShaderList<IVertexShader, WATER_FLAG_MAX> m_WaterVSList;
	TShaderList<IPixelShader, WATER_FLAG_MAX> m_WaterPSList;
	IVertexShader* m_pCausticVS;
	TShaderList<IPixelShader, CAUSTIC_PS_MAX> m_CausticPSList;
	float m_fNearCameraDistance;
	int m_nReflectNoFlush;
	ITexture* m_pCurEnvMapTex;	// 记录当前的环境贴图指针

	IColorRT* m_pSeaReflectionTex;
	IDepthRT* m_pSeaZBuffer;
    IDepthTex* m_pSeaZReflectionTex;

	ITexture* m_pWaterNormalMap1;
	ITexture* m_pWaterNormalMap2;
	ITexture* m_pWaterNormalMap3;
	ITexture* m_pWaterNormalMap4;
	ITexture* m_pWhiteWaterMap;
	ITexture* m_pCausticMap;
	ITexture* m_pNoiseMap;
	CTerrainChunk* m_RenderBlocks[MAX_RENDER_BLOCK];
	size_t m_nRenderBlockNum;

	bool   m_bEasyEffect;

	TPodHashMap<unsigned int, CTerrainChunk::chunk_water_shader_handle_t*, TPodTraits<unsigned int>,TTerrainSeaBaseShaderAlloc> m_ShaderMapHandle;
};

#endif // _SEA_BASE_H
