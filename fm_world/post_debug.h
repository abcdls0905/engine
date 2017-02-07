//--------------------------------------------------------------------
// 文件名:		post_debug.h
// 内  容:		
// 说  明:		
// 创建日期:	 
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------



#ifndef _POST_DEBUG_H_
#define _POST_DEBUG_H_


#include "../public/module.h"
#include "../public/core_type.h"


#include "../visual/i_render.h"
#include "../visual/i_vis_base.h"
/*
这里主要用来处理调试使用的RT相关的输出信息
可以输出Color和Depth相关的数据信息
目前只能Color和Depth不能同时输出
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

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
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


	// 调试渲染相关数据
	struct post_debug_cb_t
	{
        FmVec2 vec2scale;
		FmVec4 vec4DepthParam;
		FmMat4 mat4InvProj;
	};

	// 渲染相关的句柄数据
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

	//这里传入设置数据的RT
	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void Update(float seconds);
	virtual void Realize();
	//设置屏幕相对位置
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
	这里为了方便忽视像素的概念
    采用window的窗口坐标系原点和方向。其中包括
    但是。数值范围(0.0-1.0)
	*/

	//根据位置创建一个输出位置
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