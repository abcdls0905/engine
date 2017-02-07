//--------------------------------------------------------------------
// 文件名:		CLightMapConfig.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年12月31日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "light_map_config.h"
#include "../public/i_ini_file.h"

CLightMapConfig::CLightMapConfig()
{
	m_pRender = NULL;
}

CLightMapConfig::~CLightMapConfig()
{
	Clear();
}

size_t CLightMapConfig::GetModelScaleCount(const char* name)
{
	scale_info_t* info;

	if (!m_ModelConfigs.GetData(name, info))
	{
		return 0;
	}

	return info->vScales.size();
}

bool CLightMapConfig::GetModelScale(const char* name, size_t mat_index, float& x, 
									float& y, float& z)
{
	scale_info_t* info;

	if (!m_ModelConfigs.GetData(name, info) || mat_index >= info->vScales.size())
	{
		x = 1.0F;
		y = 1.0F;
		z = 1.0F;

		return true;
	}

	x = info->vScales[mat_index].x;
	y = info->vScales[mat_index].y;
	z = info->vScales[mat_index].z;

	return true;
}

bool CLightMapConfig::GetZoneScale(const char* name, float& x, float& y, float& z)
{
	scale_info_t* info;

	if (!m_ZoneConfigs.GetData(name, info) || info->vScales.size() < 1)
	{
		x = 1.0F;
		y = 1.0F;
		z = 1.0F;

		return true;
	}

	x = info->vScales[0].x;
	y = info->vScales[0].y;
	z = info->vScales[0].z;

	return true;
}

bool CLightMapConfig::Clear()
{
	CONFIG_CONTAINER::iterator it = m_ModelConfigs.Begin();
	CONFIG_CONTAINER::iterator end_it = m_ModelConfigs.End();

	for (; it != end_it; ++it)
	{
		scale_info_t* info = it.GetData();

		CORE_DELETE(info);
	}

	m_ModelConfigs.Clear();
	it = m_ZoneConfigs.Begin();
	end_it = m_ZoneConfigs.End();

	for (; it != end_it; ++it)
	{
		scale_info_t* info = it.GetData();

		CORE_DELETE(info);
	}

	m_ZoneConfigs.Clear();

	return true;
}

bool CLightMapConfig::LoadConfig(CONFIG_TYPE type, CONFIG_CONTAINER& configs)
{
	char config[260];

	if (type == CY_MODEL)
	{
		SafeSprintf(config, sizeof(config), "%s%s\\lightmap_config.ini", 
			m_pTerrain->GetFilePath(), m_pTerrain->GetModelLightPath());
	}
	else
	{
		SafeSprintf(config, sizeof(config), "%s%s\\lightmap_config.ini", 
			m_pTerrain->GetFilePath(), m_pTerrain->GetZoneLightPath());
	}

	IIniFile* pIni = m_pTerrain->GetCore()->CreateIniFile(config);

	pIni->LoadFromFile();

	size_t sect_count = pIni->GetSectionCount();

	for (size_t i = 0; i < sect_count; ++i)
	{
		const char* sect_name = pIni->GetSectionByIndex(i);
		size_t item_count = pIni->GetItemCount(sect_name);

		scale_info_t* info;

		if (configs.GetData(sect_name, info))
		{
			continue;
		}

		info = (scale_info_t*)CORE_NEW(scale_info_t);

		for (size_t j = 0; j < item_count; ++j)
		{
			const char* item_key = pIni->GetSectionItemKey(i, j);
			int mat_index = atoi(item_key);
			const char* item_value = pIni->GetSectionItemValue(i, j);
			float x = (float)atof(item_value);
			const char* split = strchr(item_value, ',');

			if (NULL == split)
			{
				continue;
			}

			float y = (float)atof(split + 1);

			split = strchr(split + 1, ',');

			if (NULL == split)
			{
				continue;
			}

			float z = (float)atof(split + 1);

			if (mat_index >= (int)info->vScales.size())
			{
				info->vScales.resize(mat_index + 1, FmVec3(1.0F, 1.0F, 1.0F));
			}

			info->vScales[mat_index] = FmVec3(x, y, z);
		}

		configs.Set(sect_name, info);
	}

	pIni->Release();

	return true;
}

bool CLightMapConfig::Load()
{
	LoadConfig(CY_MODEL, m_ModelConfigs);
	LoadConfig(CY_ZONE, m_ZoneConfigs);

	return true;
}
