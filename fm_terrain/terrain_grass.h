//--------------------------------------------------------------------
// �ļ���:		terrain_grass.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��12��4��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TERRAIN_GRASS_H
#define _TERRAIN_GRASS_H

#include "../public/module.h"


#include "../visual/i_render.h"
#include "terrain_chunk.h"
// ���滨��

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

	// ����
	bool Load();
	
	// ��Ⱦ
	void Realize();

	// ������ͼ�Ƿ�������
	bool GetLoadFinish();

	// ����
	bool FindIndex(const char* name, size_t& index) const;
	bool Find(const char* name) const;

	// �������
	size_t GetCount() const { return m_nGrassCount; }

	// ���ָ����������
	GrassType* GetByIndex(size_t index) const
	{
		Assert(index < m_nGrassCount);

		return m_GrassTypes[index];
	}
	
	// ����
	GrassType* Create(const char* name);
	// ɾ��
	bool DeleteByIndex(size_t index);
	// ���
	void Clear();

private:
	CTerrainGrass(const CTerrainGrass&);
	CTerrainGrass& operator=(const CTerrainGrass&);

	void ReleaseAll();

private:
	Terrain* m_pTerrain;
	GrassType* m_GrassTypes[MAX_GRASS_NUM];
	size_t m_nGrassCount;
	TArrayPod<CTerrainChunk::chunk_grass_cb_per_render_t, 1> m_gassrender;//��¼�βݵ���Ⱦ״̬
};

#endif // _TERRAIN_GRASS_H
