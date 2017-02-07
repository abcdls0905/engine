//--------------------------------------------------------------------
// �ļ���:		grass_type.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��12��4��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TTerrainGrassShaderAlloc& src) {}
};


// ��������
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
		float fPosiX;		// λ��
		float fPosiZ;
		float fWidth;		// �ߴ�
		float fHeight;
		float fDirX;		// ����
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
		float height;		// ��������ڵײ��ĸ߶�
		float angle;		// ����Ƕ�
	};



public:
	GrassType();
	virtual ~GrassType();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	// ������Դ
	bool Load();
	
	// ��Ⱦ
	void Realize(TArrayPod<CTerrainChunk::chunk_grass_cb_per_render_t, 1>& gassrender);

	void SetTerrain(Terrain* value);

	// ��������
	void SetName(const char* value) { m_strName = value; }
	const char* GetName() const { return m_strName.c_str(); }

	// ���ֵĹ�ϣֵ
	void SetHash(unsigned int value) { m_nHash = value; }
	unsigned int GetHash() const { return m_nHash; }

	// ��������ֵ
	void SetIndex(size_t value) { m_nIndex = value; }

	// ��ͼ�ļ���
	void SetTexture(const char* value) { m_strTexture = value; }
	const char* GetTexture() const { return m_strTexture.c_str(); }

	// ��ͼ�ߴ�
	void SetTexScale(int value) { m_nTexScale = value; }
	int GetTexScale() const { return m_nTexScale; }

	// ��ȷ������ͼ�ָ�����
	void SetFrameWidth(int value) { m_nFrameWidth = value; }
	int GetFrameWidth() const { return m_nFrameWidth; }
	
	// �߶ȷ������ͼ�ָ�����
	void SetFrameHeight(int value) { m_nFrameHeight = value; }
	int GetFrameHeight() const { return m_nFrameHeight; }

	// ��С���
	void SetMinWidth(float value) { m_fMinWidth = value; }
	float GetMinWidth() const { return m_fMinWidth; }
	
	// �����
	void SetMaxWidth(float value) { m_fMaxWidth = value; }
	float GetMaxWidth() const { return m_fMaxWidth; }
	
	// ��С�߶�
	void SetMinHeight(float value) { m_fMinHeight = value; }
	float GetMinHeight() const { return m_fMinHeight; }
	
	// ���߶�
	void SetMaxHeight(float value) { m_fMaxHeight = value; }
	float GetMaxHeight() const { return m_fMaxHeight; }
	
	// ��С��б
	void SetMinPitch(float value) { m_fMinPitch = value; }
	float GetMinPitch() const { return m_fMinPitch; }
	
	// �����б
	void SetMaxPitch(float value) { m_fMaxPitch = value; }
	float GetMaxPitch() const { return m_fMaxPitch; }

	// ��Χ��������
	void SetScaleFactor(float value) { m_fScaleFactor = value; }
	float GetScaleFactor() const { return m_fScaleFactor; }

	// ǿ��ˢ��
	void SetMustRefresh(bool value) { m_bMustRefresh = value; }
	bool GetMustRefresh() const { return m_bMustRefresh; }

	// �����ͼָ��
	ITexture* GetTex() const { return m_pTexture; }

	// ����֮
	void Activate()
	{
		m_bActive = true;

		if (NULL == m_pTexture)
		{
			Load();
		}
	}

	// �Ƿ񼤻�
	bool GetActive() const { return m_bActive; }

private:
	GrassType(const GrassType&);
	GrassType& operator=(const GrassType&);

	// ���ɵ��ηֿ��ڵĻ���
	IStaticVB* Generate(CTerrainChunk* pChunk, size_t& count);
	// ������Ⱦǰ��׼��
	void PrepareRealize();
	// �ӳ���ɫ
	void DrawGBuffer(TArrayPod<CTerrainChunk::chunk_grass_cb_per_render_t, 1>& gassrender);
	// ���ó�������
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
