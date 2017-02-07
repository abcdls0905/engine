//--------------------------------------------------------------------
// 文件名:		render_tool.h
// 内  容:		
// 说  明:		
// 创建日期:	2012年11月8日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _RENDER_TOOL_H
#define _RENDER_TOOL_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../visual/i_resource.h"


#include "../visual/fm_header.h"
#include "../visual/shader_list.h"
#include "../utils/array_pod.h"

// 渲染器辅助工具

class Render;
class CColorRT;

class CRenderTool
{
public:
	static CRenderTool* NewInstance(Render* pRender);

private:
	struct bw_tex_t
	{
		int nWidth;
		int nHeight;
		IStaticTex* pTexture;
	};

public:
	CRenderTool(Render* pRender);
	~CRenderTool();

	// 释放
	void Release();
	// 初始化
	bool Initialize();

	// 绘制一个屏幕大小的四边形
	void DrawScreenQuad();
	// 绘制球体
	void DrawSphere();
	// 绘制精细球体
	void DrawFineSphere();
	// 绘制圆锥体
	void DrawCone();
	// 绘制立方体
	void DrawCube();
	// 绘制半立方体
	void DrawHalfCube();
	// 绘制金字塔六分之一立方体
	void DrawPyramidNX();
	void DrawPyramidPX();
	void DrawPyramidNZ();
	void DrawPyramidPZ();
	void DrawPyramidNY();
	void DrawPyramidPY();

	// 获得后处理用的临时渲染目标
	CColorRT* GetPostRT0(TEX_FORMAT format, bool auto_gen_mipmap);
	CColorRT* GetPostRT1(TEX_FORMAT format, bool auto_gen_mipmap);
	// 获得黑白贴图
	IStaticTex* GetBlackWhiteTex(IShaderTex* pShaderTex);

	// 清除一些渲染过程中使用的临时资源
	void ClearRenderResource();

	IDynamicVB* GetDesignLineVB() const { return m_pDesignLineVB; }
	IVertexShader* GetDesignLineVS() const { return m_pDesignLineVS; }
	IPixelShader* GetDesignLinePS() const { return m_pDesignLinePS; }
	IDynamicVB* CreateDesignLineVB(unsigned int need_size);

	IDynamicVB* GetPictureVB() const { return m_pPictureVB; }
	IVertexShader* GetPictureVS() const { return m_pPictureVS; }
	IPixelShader* GetPicturePS() const { return m_pPicturePS; }
	IPixelShader* GetPictureAlphaPS() const { return m_pPictureAlphaPS; }
	IPixelShader* GetPictureNormalPS() const { return m_pPictureNormalPS; }
	IPixelShader* GetPictureLengthPS() const { return m_pPictureLengthPS; }
	IPixelShader* GetPictureGammaPS() const { return m_pPictureGammaPS; }

	IDynamicVB* CreateBoundBoxVB(unsigned int size);
	IDynamicIB* CreateBoundBoxIB(unsigned int size);
	IDynamicVB* GetBoundBoxVB() const { return m_pBoundBoxVB; }
	IDynamicIB* GetBoundBoxIB() const { return m_pBoundBoxIB; }
	IVertexShader* GetBoundBoxVS() const { return m_pBoundBoxVS; }
	IPixelShader* GetBoundBoxPS() const { return m_pBoundBoxPS; }

	IVertexShader* GetRevertDepthVS() const { return m_pRevertDepthVS; }
	IPixelShader* GetRevertDepthPS() const { return m_pRevertDepthPS; }
	IVertexShader* GetCopyMinDepthVS() const { return m_pCopyMinDepthVS; }
	IPixelShader* GetCopyMinDepthPS() const { return m_pCopyMinDepthPS; }
	IVertexShader* GetCopyDepthVS() const { return m_pCopyDepthVS; }
	IPixelShader* GetCopyDepthPS() const { return m_pCopyDepthPS; }
	IVertexShader* GetCopyTexVS() const { return m_pCopyTexVS; }
	IPixelShader* GetCopyTexPS() const { return m_pCopyTexPS; }
	IPixelShader* GetCopyTexGammaPS() const { return m_pCopyTexGammaPS; }
	IPixelShader* GetCopyTexLinearPS() const { return m_pCopyTexLinearPS; }

private:
	CRenderTool();
	CRenderTool(const CRenderTool&);
	CRenderTool& operator=(const CRenderTool&);
	
	// 加载一些内部使用的着色器
	bool CreateCommonResource();
	// 创建画简单模型所需的资源
	void CreateGeometryResource();

private:
	Render* m_pRender;
	CColorRT* m_pPostRT0;
	CColorRT* m_pPostRT1;
	size_t m_nMultSampleRTCount;
	TArrayPod<bw_tex_t, 1, TCoreAlloc> m_BlackWhiteTexs;

	IStaticVB* m_pConeVB;
	IStaticIB* m_pConeIB;
	int m_nConeVertexNum;
	int m_nConeTriangleNum;
	IStaticVB* m_pSphereVB;
	IStaticIB* m_pSphereIB;
	int m_nSphereVertexNum;
	int m_nSphereTriangleNum;
	IStaticVB* m_pFineSphereVB;
	IStaticIB* m_pFineSphereIB;
	int m_nFineSphereVertexNum;
	int m_nFineSphereTriangleNum;
	IStaticVB* m_pScreenQuadVB;
	IStaticIB* m_pScreenQuadIB;
	IStaticVB* m_pCubeVB;
	IStaticIB* m_pCubeIB;
	IStaticVB* m_pHalfCubeVB;
	IStaticIB* m_pHalfCubeIB;
	IStaticVB* m_pPyramidVB;
	IStaticIB* m_pPyramidNXIB;
	IStaticIB* m_pPyramidPXIB;
	IStaticIB* m_pPyramidNYIB;
	IStaticIB* m_pPyramidPYIB;
	IStaticIB* m_pPyramidNZIB;
	IStaticIB* m_pPyramidPZIB;
	IDynamicVB* m_pBoundBoxVB;
	IDynamicIB* m_pBoundBoxIB;
	IDynamicVB* m_pDesignLineVB;
	IDynamicVB* m_pPictureVB;
	IVertexShader* m_pDesignLineVS;
	IPixelShader* m_pDesignLinePS;
	IVertexShader* m_pPictureVS;
	IPixelShader* m_pPicturePS;
	IPixelShader* m_pPictureAlphaPS;
	IPixelShader* m_pPictureNormalPS;
	IPixelShader* m_pPictureLengthPS;
	IPixelShader* m_pPictureGammaPS;
	IVertexShader* m_pCopyTexVS;
	IPixelShader* m_pCopyTexPS;
	IPixelShader* m_pCopyTexGammaPS;
	IPixelShader* m_pCopyTexLinearPS;
	IVertexShader* m_pCopyDepthVS;
	IPixelShader* m_pCopyDepthPS;
	IVertexShader* m_pCopyMinDepthVS;
	IPixelShader* m_pCopyMinDepthPS;
	IVertexShader* m_pRevertDepthVS;
	IPixelShader* m_pRevertDepthPS;
	IVertexShader* m_pBoundBoxVS;
	IPixelShader* m_pBoundBoxPS;
};

#endif // _RENDER_TOOL_H
