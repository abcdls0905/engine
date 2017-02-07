//--------------------------------------------------------------------
// 文件名:		CLightMapConfig.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年12月31日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _LIGHTMAPCONFIG_H
#define _LIGHTMAPCONFIG_H

#include "../utils/string_pod.h"
#include "terrain.h"


// 光贴图配置

class CLightMapConfig
{
public:
	enum CONFIG_TYPE
	{
		CY_MODEL = 0x1,
		CY_ZONE = 0x2,
	};

	// 缩放值
	struct scale_info_t
	{
		TArrayPod<FmVec3, 1, TCoreAlloc> vScales;
	};

	typedef TStringPod<char, scale_info_t*, TStringTraits<char>, TCoreAlloc> 
		CONFIG_CONTAINER;

public:
	CLightMapConfig();
	~CLightMapConfig();

	// 获取模型配置
	size_t GetModelScaleCount(const char* name);
	bool GetModelScale(const char* name, size_t mat_index, float& x, float& y, 
		float& z);
	// 获取地形配置
	bool GetZoneScale(const char* name, float& x, float& y, float& z);
	// 清空
	bool Clear();
	// 加载
	bool Load();
	// 设置地形
	void SetTerrain(Terrain* terrain) { m_pTerrain = terrain; }

private:
	bool LoadConfig(CONFIG_TYPE type, CONFIG_CONTAINER& configs);

private:
	Terrain* m_pTerrain;
	CONFIG_CONTAINER m_ModelConfigs;
	CONFIG_CONTAINER m_ZoneConfigs;
	IRender* m_pRender;
};

#endif // _LIGHTMAPCONFIG_H
