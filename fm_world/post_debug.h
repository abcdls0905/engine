//--------------------------------------------------------------------
// �ļ���:		post_debug.h
// ��  ��:		
// ˵  ��:		
// ��������:	 
// ������:		�ſ�ʤ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------



#ifndef _POST_DEBUG_H_
#define _POST_DEBUG_H_


#include "../public/module.h"
#include "../public/core_type.h"


#include "../visual/i_render.h"
#include "../visual/i_vis_base.h"
/*
������Ҫ�����������ʹ�õ�RT��ص������Ϣ
�������Color��Depth��ص�������Ϣ
Ŀǰֻ��Color��Depth����ͬʱ���
*/
#include "../visual/shader_list.h"
#include "../utils/array_pod.h"
#include "../utils/pod_hash_map.h"

class IRenderContext;
class ISceneView;

class TPostDebugShaderAlloc
{
public:
	TPostDebugShaderAlloc() {}
	~TPostDebugShaderAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TPostDebugShaderAlloc& src) {}
};



class CPostDebug: public IVisBase
{
public:
	struct post_debug_vertex_t
	{
		FmVec2 pos;
		FmVec2 uv;
	};

	enum FLAG_SHIFT_ENUM
	{
		FLAG_COLOR,
		FLAG_DEPTH,
		FLAG_TEXTURE,
		FLAG_DECODE_DEPTH,
		FLAG_MAX,
	};


	// ������Ⱦ�������
	struct post_debug_cb_t
	{
        FmVec2 vec2scale;
		FmVec4 vec4DepthParam;
		FmMat4 mat4InvProj;
	};

	// ��Ⱦ��صľ������
	struct post_debug_shaderhandle_t
	{
		IShaderProgram* pShader;
		fm_int vec2UVscaleHandle;
		fm_int vec4DepthParamHandle;
		fm_int mat4InvProjHandle;
		fm_int tex_ColorHandle;
		fm_int tex_DepthHandle;
	};



	CPostDebug();
	virtual ~CPostDebug();

	//���ﴫ���������ݵ�RT
	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void Update(float seconds);
	virtual void Realize();
	//������Ļ���λ��
	virtual void DrawAxisAlignedQuad(IShaderProgram* pShader);

	virtual float GetLeftUpX();
	virtual void  SetLeftUpX(float value);
	virtual float GetLeftUpY();
	virtual void  SetLeftUpY(float value) ;
    
	virtual float GetRightBottomX();
	virtual void  SetRightBottomX(float value);
	virtual float GetRightBottomY();
	virtual void  SetRightBottomY(float value);

private:
	static void RenderColorRT(void* pdata);
	static void RenderDepthRT(void* pdata);
	static void RenderTexture(void* pdata);
	static void RenderDepthStencilRT(void* pdata);
	static void RenderDecodeDepthRT(void* pdata);
 
	/*
	����Ϊ�˷���������صĸ���
    ����window�Ĵ�������ϵԭ��ͷ������а���
    ���ǡ���ֵ��Χ(0.0-1.0)
	*/

	//����λ�ô���һ�����λ��
	void UpDataVertByScreenPos(FmVec2 leftup_pos = FmVec2(0.0,0.0),FmVec2 rightbottom_pos = FmVec2(0.25,0.25));

	CPostDebug::post_debug_shaderhandle_t* GetShaderHandle(bool bDepth,bool bTexture,bool bColor,bool bDecode = false);

	void SetShaderConstValue(CPostDebug::post_debug_shaderhandle_t* pShaderHandle);

protected:
	IRender* m_pRender;
//	IShaderProgram* m_pShader;
//	IShaderProgram* m_pDepthShader;
	FmMat4          m_matProj;
	FmVec4          m_vec4DepthParam;
	IColorRT*       m_pTexColor;
	IDepthRT*       m_pTexDepth;
	IDepthStencilRT* m_pTexDepthStencil;
	IColorRT*       m_pTexDecodeColor;
	ITexture*       m_pTex;
	FmVec2          m_leftup;
	FmVec2          m_rightbottom;

	FmVec2          m_uvSacleColorRT;
	FmVec2          m_vsScaleDepthRT;
	post_debug_vertex_t m_vert[4];

	TShaderList<IVertexShader, FLAG_MAX> m_VSList;
	TShaderList<IPixelShader, FLAG_MAX>  m_PSList;

	TPodHashMap<unsigned int, CPostDebug::post_debug_shaderhandle_t*, TPodTraits<unsigned int>, TPostDebugShaderAlloc> m_ShaderMapHandle;
    post_debug_cb_t m_post_debug_cb;
};




#endif