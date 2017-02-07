//--------------------------------------------------------------------
// �ļ���:		terrain_painter.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��8��9��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TERRAIN_PAINTER_H
#define _TERRAIN_PAINTER_H

#include "../public/macros.h"


#include "../public/i_interface.h"
#include "../visual/i_render.h"
#include "../visual/i_resource.h"
#include "../visual/shader_list.h"
#include "terrain_culling.h"
#include "../utils/array_pod.h"
#include "../utils/pod_hash_map.h"

// ������Ⱦ

class Terrain;
class CTerrainZone;
class CTerrainChunk;

class TTerrainPainterShaderAlloc
{
public:
	TTerrainPainterShaderAlloc() {}
	~TTerrainPainterShaderAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TTerrainPainterShaderAlloc& src) {}
};


class CTerrainPainter
{
private:
	enum { MAX_SHADOWMAP_NUM = 8 };

	enum FLAG_SHIFT_ENUM
	{
		FLAG_LIGHTMAP,
		FLAG_SHADOWEDMAP,
		FLAG_GRADUALLY,
		FLAG_FOGLINEAR,
		FLAG_NORMALMAP,
		FLAG_SPECULAR,
		FLAG_PRELIGHT,
		FLAG_SMOOTHCLIP,
		FLAG_FIXSHADOW,
		FLAG_FOGEXP,
		FLAG_SPHEREAMBIENT,
		FLAG_CLIPPLANE,
		FLAG_HEIGHT_FOG,
		FLAG_MAX,
	};

	enum GBUFFER_VS_SHIFT_NUM
	{
		GBUFFER_VS_NOMRALMAP,
		GBUFFER_VS_GRADUALLY,
		GBUFFER_VS_SMOOTHCLIP,
		GBUFFER_VS_MAX,
	};

	enum GBUFFER_DS_SHIFT_NUM
	{
		GBUFFER_DS_GRADUALLY,
		GBUFFER_DS_SMOOTHCLIP,
		GBUFFER_DS_MAX,
	};

	enum GBUFFER_PS_SHIFT_NUM
	{
		GBUFFER_PS_NOMRALMAP,
		GBUFFER_PS_GRADUALLY,
		GBUFFER_PS_SMOOTHCLIP,
		GBUFFER_PS_MAX,
	};

	enum DEPTH_VS_SHIFT_ENUM
	{
		DEPTH_VS_SMOOTHCLIP,
		DEPTH_VS_MAX,
	};

	enum DEPTH_PS_SHIFT_ENUM
	{
		DEPTH_PS_SMOOTHCLIP,
		DEPTH_PS_MAX,
	};

public:
	CTerrainPainter();
	~CTerrainPainter();

	void SetTerrain(Terrain* value) { m_pTerrain = value; }

	// ��õر����ʽ����
//	IDxInputLayout* GetInputLayout() const { return m_pInputLayout; }

	// ��ʼ��
	bool Initialize(IRender* pRender);

	// ���ر�
	void DrawChunks();
	// ����������
	void DrawVisuals();
	// ���򻯵ر�
	void DrawSimpleGround();
	
	// ��������ͼ�ļ򻯵ر�
	void DrawEnvMapSimpleGround(size_t index);
	// ��������ͼ�ĵ�������
	void DrawEnvMapChunks(size_t index);
	// ��������ͼ�ĵ�������
	void DrawEnvMapVisuals(size_t index);
	
	// ����Ӱ��ͼ�ĵر�
	void DrawShadowMapChunks();
	// ����Ӱ��ͼ�ĵ�������
	void DrawShadowMapVisuals();

	// ��ʾ������Ĳ����ṹ
	void DrawZoneQuadTree();
 

private:
	CTerrainPainter(const CTerrainPainter&);
	CTerrainPainter& operator=(const CTerrainPainter&);

	// ���¹�����������
	void UpdateChunkGroupCB(CTerrainChunk* chunk);
	// ����Ƹ�������
	void DrawDesignHelper();
	// ��ǰ�ؿ��ƽ��������С��ָ���ľ��룬��������ʾ���Ĵ�ĸ�������
	bool GetShowDesignHelper(const CTerrainChunk* chunk, 
		const float distance) const;
	// �ÿɱ�̹��߻��ر�
	void DrawChunksShader(render_zone_t* zones, 
		size_t zone_num, bool horizon_cull, bool simple, int level);
	void DrawChunksGBuffer(render_zone_t* zones, 
		size_t zone_num, bool horizon_cull, int level);
	// �������еĵر�
	void DrawInFog(render_zone_t* zones, size_t zone_num, bool horizon_cull);

    CTerrainChunk::chunk_cb_object_shader_handle_t* GetShaderHandle(size_t flagindex,size_t shader_flag,size_t blend_num);

private:
    // ��ʼ���Ƶĵر���
	static void DrawChunksShaderBatch(void* pdata);

	Terrain* m_pTerrain;
	IRender* m_pRender;
	TShaderOption<IVertexShader, 5, FLAG_MAX> m_VSList;
	TShaderOption<IPixelShader, 5, FLAG_MAX> m_PSList;
	TShaderOption<IVertexShader, 5, GBUFFER_VS_MAX> m_GBufferVSList;
	TShaderOption<IPixelShader, 5, GBUFFER_PS_MAX> m_GBufferPSList;
	TShaderList<IVertexShader, DEPTH_VS_MAX> m_DepthVSList;
	TShaderList<IPixelShader, DEPTH_PS_MAX> m_DepthPSList;
	IVertexShader* m_pGBufferTessVS;
	IVertexShader* m_pShadowMapVS;
	IVertexShader* m_pShadowMapTessVS;
	IPixelShader* m_pShadowMapPS;
	IVertexShader* m_pInFogVS;
	IPixelShader* m_pInFogPS;
	IVertexShader* m_pInFogDepthVS;
	IPixelShader* m_pInFogDepthPS;
	IVertexShader* m_pInFogGBufferVS;
	IPixelShader* m_pInFogGBufferPS;
	IDynamicVB* m_pInFogVB;
	IDynamicIB* m_pInFogIB;
	size_t m_nInFogVBSize;
	size_t m_nInFogIBSize;

	TPodHashMap<unsigned int, CTerrainChunk::chunk_cb_object_shader_handle_t*, TPodTraits<unsigned int>, TTerrainPainterShaderAlloc> m_ShaderMapHandle;

};

#endif // _TERRAIN_PAINTER_H
