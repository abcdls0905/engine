//--------------------------------------------------------------------
// 文件名:		terrain_light.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TERRAIN_LIGHT_H
#define _TERRAIN_LIGHT_H

#include "../public/macros.h"
#include "../visual/i_texture.h"
#include "../math/fm_vector.h"

// 地形的光照信息

class Terrain;
class CTerrainZone;
class CTerrainHeight;
class CTexLightMap;
class IRender;

class CTerrainLight
{
public:
	CTerrainLight(Terrain* pTerrain, CTerrainZone* pZone, size_t rows, 
		size_t cols);
	~CTerrainLight();

	// 行列数
	size_t GetRows() const { return m_nRows; }
	size_t GetCols() const { return m_nCols; }

	// 获得混合颜色
	unsigned int* GetBlendColors();

	// 获得指定位置的光贴图亮度
	float GetDiffuse(float u, float v);

	// 是否为初始空白LightMap
	bool GetIsNull() const { return m_bIsNull; }

	// 设置地表混合颜色
	bool SetBlendColor(int row, int col, unsigned int color);
	// 获得地表混合颜色
	unsigned int GetBlendColor(int row, int col);

	// 生成空的光照数据
	bool BuildNull();

	// 创建区域的光照贴图
	CTexLightMap* CreateLightTex();
	
	// 获得区域的光照贴图
	CTexLightMap* GetLightTex() const { return m_pLightTex; }

	// 操作光照图缩放值
	void SetLightConfig(const FmVec3& value) { m_vHdrScale = value; }
	const FmVec3& GetLightConfig() const { return m_vHdrScale; }

	// 根据颜色值生成光贴图
	bool BuildTexByColors(size_t rows, size_t cols, 
		const unsigned int* colors);
	
	// 使用贴图文件数据生成贴图
	bool BuildTex(unsigned char* dds_data, size_t dds_size);
	bool BuildTex(ITexture* pTex);
	
	// 创建地表混合颜色
	bool BuildBlendColors(size_t rows, size_t cols, 
		const unsigned int* colors);

private:
	CTerrainLight();
	CTerrainLight(const CTerrainLight&);
	CTerrainLight& operator=(const CTerrainLight&);

	void ReleaseAll();
	// 释放区域的光照贴图
	void ReleaseLightTex();
	// 创建编辑模式的光贴图
	bool CreateDesignTex();

private:
	Terrain* m_pTerrain;
	CTerrainZone* m_pZone;
	IRender* m_pRender;
	size_t m_nRows;
	size_t m_nCols;
	unsigned int* m_pBlendColors;
	size_t m_nLightWidth;
	size_t m_nLightHeight;
	unsigned int* m_pLightColors;
	CTexLightMap* m_pLightTex;		// 光照贴图	
	FmVec3 m_vHdrScale;		// 光照图缩放
	ITexture* m_pDiffuseTex;
	bool m_bIsNull;
	bool m_bIsDesign;
};

#endif // _TERRAIN_LIGHT_H
