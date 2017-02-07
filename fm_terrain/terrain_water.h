//--------------------------------------------------------------------
// �ļ���:		terrain_water.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��11��30��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TERRAIN_WATER_H
#define _TERRAIN_WATER_H

#include "../public/module.h"


#include "../visual/i_render.h"

// ����ˮ��

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

	// ����
	bool Load();
	// ж��
	bool Unload();
	
	// ��Ⱦ
	void Realize();

	// ������ͼ�Ƿ�������
	bool GetLoadFinish();

	// ����
	bool FindIndex(const char* name, size_t& index) const;
	bool Find(const char* name) const;
	// �������
	size_t GetCount() const;
	// ���ָ������ˮ��
	SeaWater* GetByIndex(size_t index) const;
	
	// ����
	SeaWater* Create(const char* name);
	// ɾ��
	bool DeleteByIndex(size_t index);
	// ���
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

