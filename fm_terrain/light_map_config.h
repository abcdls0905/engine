//--------------------------------------------------------------------
// �ļ���:		CLightMapConfig.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��12��31��
// ������:		
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _LIGHTMAPCONFIG_H
#define _LIGHTMAPCONFIG_H

#include "../utils/string_pod.h"
#include "terrain.h"


// ����ͼ����

class CLightMapConfig
{
public:
	enum CONFIG_TYPE
	{
		CY_MODEL = 0x1,
		CY_ZONE = 0x2,
	};

	// ����ֵ
	struct scale_info_t
	{
		TArrayPod<FmVec3, 1, TCoreAlloc> vScales;
	};

	typedef TStringPod<char, scale_info_t*, TStringTraits<char>, TCoreAlloc> 
		CONFIG_CONTAINER;

public:
	CLightMapConfig();
	~CLightMapConfig();

	// ��ȡģ������
	size_t GetModelScaleCount(const char* name);
	bool GetModelScale(const char* name, size_t mat_index, float& x, float& y, 
		float& z);
	// ��ȡ��������
	bool GetZoneScale(const char* name, float& x, float& y, float& z);
	// ���
	bool Clear();
	// ����
	bool Load();
	// ���õ���
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
