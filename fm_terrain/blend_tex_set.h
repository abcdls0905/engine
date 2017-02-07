//--------------------------------------------------------------------
// �ļ���:		blend_tex_set.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��7��3��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _BLEND_TEX_SET_H
#define _BLEND_TEX_SET_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../utils/array_pod.h"

//#include "../visual/i_dx_render.h"

// ������ͼ����

class CBlendTex;
class Terrain;

class CBlendTexSet
{
public:
	CBlendTexSet();
	~CBlendTexSet();

	void SetTerrain(Terrain* value) { m_pTerrain = value; }
	
	// �ͷ�������ͼ
	void ReleaseAll();

	// ����
	bool Find(const char* name) const;
	bool FindIndex(const char* name, size_t* index) const;
	// ���
	bool Add(const char* name, const char* path);

	// �������
	size_t GetCount() const { return m_Textures.size(); }

	// ���ָ�������Ļ����ͼ
	CBlendTex* GetByIndex(size_t index) const
	{
		Assert(index < m_Textures.size());

		return m_Textures[index];
	}

	// ��ͼ�Ƿ�ȫ����ɼ���
	bool GetLoadFinish();

private:
	CBlendTexSet(const CBlendTexSet&);
	CBlendTexSet& operator=(const CBlendTexSet&);

private:
	Terrain* m_pTerrain;
	TArrayPod<CBlendTex*, 1, TCoreAlloc> m_Textures;
};

#endif // _BLEND_TEX_SET_H
