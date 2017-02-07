//--------------------------------------------------------------------
// �ļ���:		sea_base.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��8��9��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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
// ˮ��

class Terrain;
//class CTerrainChunk;

class TTerrainSeaBaseShaderAlloc
{
public:
	TTerrainSeaBaseShaderAlloc() {}
	~TTerrainSeaBaseShaderAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
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
		WATER_FLAG_USE_INTZ,//�ƶ�Ŀǰû�������⣬����ֱ����z/w���з����
		WATER_FLAG_EASY,//����Ч��(�޶��㲨��Ч��)
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

	// ������Դ
	bool Load();
	// ж����Դ
	bool Unload();

	// ���ˮ������
	sea_data_t* GetSeaData() { return &m_SeaData; }

	// ��������߶�
	void SetBaseHeight(float value) { m_fBaseHeight = value; }
	float GetBaseHeight() const { return m_fBaseHeight; }

	// ��ǰ����߶�
	float GetCurrentHeight(float x, float y);

	// ���߶Ȳ�
	void SetMaxSeaHeight(float value) { m_fMaxSeaHeight = value; }
	float GetMaxSeaHeight() const { return m_fMaxSeaHeight; }

	// �Ƿ�ɼ�
	void SetVisible(bool value) { m_bVisible = value; }
	bool GetVisible() const { return m_bVisible; }

	// ����ֵ
	void SetIndex(size_t value) { m_nIndex = value; }
	size_t GetIndex() const { return m_nIndex; }

	// �����ͼָ��
	ITexture* GetWaterNormalMap1() const { return m_pWaterNormalMap1; }
	ITexture* GetWaterNormalMap2() const { return m_pWaterNormalMap2; }
	ITexture* GetWaterNormalMap3() const { return m_pWaterNormalMap3; }
	ITexture* GetWaterNormalMap4() const { return m_pWaterNormalMap4; }
	ITexture* GetWhiteWaterMap() const { return m_pWhiteWaterMap; }
	ITexture* GetCausticMap() const { return m_pCausticMap; }

	// ����������ͼ
	bool CreateNormalMap(const char* pszFileName1, const char* pszFileName2, 
		const char* pszFileName3, const char* pszFileName4);
	// ������ɫˮ����ͼ
	bool CreateWhiteWaterMap(const char* pszFileName);
	// ����ˮ�¿�ʴ��ͼ
	bool CreateCausticMap(const char* pszFileName);
	// ��������ʴ��ͼ
	bool CreateNoiseMap(const char* pszFileName);

	// ����������ͼ
	bool DumpReflectMap(const char* file_name);

private:
	CSeaBase(const CSeaBase&);
	CSeaBase& operator=(const CSeaBase&);

	void ReleaseAll();
	// ˢ����Ҫ��Ⱦ��ˮ���
	void RefreshBlocks();
	// ���ɻ���������ͼ
	void GenerateEnvMap();
	// ��Ⱦˮ�¿�ʴ
	void DrawCaustic();
	// ��ò��˵�ƽ���߶�
	float GetAverageWaveHeight();

	CTerrainChunk::chunk_water_shader_handle_t* GetShaderHandle(size_t flags);

	static void DrawChunksShaderWaterBatch(void* pdata);

private:
	Terrain* m_pTerrain;
	IRender* m_pRender;
	IRenderContext* m_pContext;
	ISceneView* m_pSeaSceneView;
	sea_data_t m_SeaData;
	float m_fBaseHeight;		// ��ƽ��߶�
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
	ITexture* m_pCurEnvMapTex;	// ��¼��ǰ�Ļ�����ͼָ��

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
