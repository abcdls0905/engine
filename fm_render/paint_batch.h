//--------------------------------------------------------------------
// 文件名:		paint_batch.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年7月14日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

//#define _COMPRESSED_VERTEX

#ifndef _PAINT_BATCH_H
#define _PAINT_BATCH_H

#include "../public/macros.h"
#include "../public/core_mem.h"


#include "../visual/i_resource.h"
#include "../visual/shader_list.h"
#include "../utils/array_pod.h"

class TPaintBatchAlloc
{
public:
	TPaintBatchAlloc() {}
	~TPaintBatchAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TPaintBatchAlloc& src) {}
};

// 平面渲染批次管理

class Render;
class IVisBase;
class batch_t;
class ISceneView;
class IScene;
class CPaintBatchs
{
	IFrameRT* m_pFrameRT;
	size_t m_debug_batch_num; //用来调试
public:
	static CPaintBatchs* NewInstance(Render* pRender);

	struct viewport_t
	{
		int nLeft;
		int nTop;
		int nWidth;
		int nHeight;
	};
	
	struct vertex_line_t
	{
		float x;
		float y;
		float z;
		float w;
		unsigned int color;     
	};

	struct vertex_pict_t
	{
#ifdef _COMPRESSED_VERTEX
		short x;
		short y;
		short z;
		short w;
		unsigned int color;     
		short tu;
		short tv;
#else
		float x;
		float y;
		float z;
		float w;
		unsigned int color;     
		float tu;
		float tv;
#endif
	};

	struct vertex_point_t
	{
		float x;
		float y;
		float z;
		float w;
		float pointsize;
		unsigned int color;
	};

private:
	enum GUI_VS_SHIFT_ENUM
	{
		GUI_VS_USE_POINT,
		GUI_VS_USE_PICTURE,
		GUI_VS_USE_ROTATE,
		GUI_VS_USE_DEPTH,
		GUI_VS_MAX,
	};

	enum GUI_PS_SHIFT_ENUM
	{
		GUI_PS_USE_PICTURE,
		GUI_PS_USE_PICTUREA8,
		GUI_PS_USE_FADE,
		GUI_PS_USE_BLACKWHITE,
		GUI_PS_USE_OUTLINE,
		GUI_PS_MAX,
	};

	enum
	{
		USE_DEPTH = 1 << 0,
		USE_BLEND = 1 << 1,
		USE_TEXTURE = 1 << 2,
		USE_BLACKWHITE = 1 << 3,
		USE_ROTATE = 1 << 3,
        USE_TEXTUREA8 = 1 << 4,
		USE_FADE = 1 << 5,
		USE_OUTLINE = 1 << 6,
		USE_OUTLINE_FADE = 1 << 7,
	};
	
	struct batch_t
	{
		unsigned int nFlags;
		IRenderDrawOp::DRAW_MODE nPrimitiveType;
		int nPrimitiveCount;
		int nStartVertex;
		int nVertexCount;
		int nBlendAlpha;
		int nBlendMode;
		IShaderTex* pTexture;
		FmMat4 mtxRotate;
		int nViewLeft;
		int nViewTop;
		int nViewWidth;
		int nViewHeight;
		float fDepthZ;
		FmVec3 nOutlineColor;
		IVertexShader* pVertexShader;
		IPixelShader* pPixelShader;
		IScene* pScene;
		ISceneView* pSceneView;
		IFrameRT* pFrameRT;
	};
	
	void SetConstValue(struct GuiShaderData& shaderData, CPaintBatchs::batch_t* pBatch);

public:
	CPaintBatchs(Render* pRender);
	~CPaintBatchs();

	// 释放
	void Release();

	// 初始化
	bool Initialize();
	
	// 添加一个渲染批次并获得顶点内存
	void* AddBatch(float depth_z, float depth_w, bool use_blend, 
		bool use_texture, bool use_black_white, bool use_texture_a8, 
		int blend_alpha, int blend_mode, IRenderDrawOp::DRAW_MODE primitive_type, 
		int primitive_count, IShaderTex* pTexture, IScene* pScene, 
		const FmMat4* pRotate, const FmVec3* pOutColor = 0, bool outline_fade = false);
		
	// 清空批次数据
	void ClearData();
	// 渲染
	bool RenderFlat();

	// 设置输出窗口范围
	bool SetViewport(int x1, int y1, int x2, int y2, bool is_in_parent);
	// 恢复前面的输出窗口范围
	bool ResetViewport();

	// 当前视口
	void GetCurrentViewport(int& left, int& top, int& width, int& height)
	{
		left = m_CurViewport.nLeft;
		top = m_CurViewport.nTop;
		width = m_CurViewport.nWidth;
		height = m_CurViewport.nHeight;
	}

	// 设置是否缩放
	void SetScaleEnable(bool value) { m_bScaleEnable = value; }

	// 清除一些渲染过程中使用的临时资源
	void ClearRenderResource();

	void SetFrameRT(IFrameRT* pFrameRT) { m_pFrameRT = pFrameRT; }

	IFrameRT* GetFrameRT() { return m_pFrameRT; }

	size_t GetBatchSize() { return m_Batchs.size(); }

	//用来调试
	void SetDebugBatchNum(size_t num) { m_debug_batch_num = num; }
private:
	CPaintBatchs();
	CPaintBatchs(const CPaintBatchs&);
	CPaintBatchs& operator=(const CPaintBatchs&);

	// 设置平面绘制状态
	void SetPaintState();

	// 获得界面着色器
	IVertexShader* GetGuiLineVS(bool use_rotate, bool use_depth)
	{
		size_t vs_flags = (size_t(use_rotate) << GUI_VS_USE_ROTATE) 
			+ (size_t(use_depth) << GUI_VS_USE_DEPTH);

		return m_GuiVSList.GetShader(vs_flags);
	}

	IVertexShader* GetGuiPictVS(bool use_rotate, bool use_depth)
	{
		size_t vs_flags = (size_t(use_rotate) << GUI_VS_USE_ROTATE) 
			+ (size_t(use_depth) << GUI_VS_USE_DEPTH)
			+ (0x1 << GUI_VS_USE_PICTURE);

		return m_GuiVSList.GetShader(vs_flags);
	}

	IVertexShader* GetGuiBWPictVS(bool use_rotate, bool use_depth)
	{
		size_t vs_flags = (size_t(use_rotate) << GUI_VS_USE_ROTATE) 
			+ (size_t(use_depth) << GUI_VS_USE_DEPTH)
			+ (0x1 << GUI_VS_USE_PICTURE);

		return m_GuiVSList.GetShader(vs_flags);
	}

	IVertexShader* GetGuiA8PictVS(bool use_rotate, bool use_depth)
	{
		size_t vs_flags = (size_t(use_rotate) << GUI_VS_USE_ROTATE) 
			+ (size_t(use_depth) << GUI_VS_USE_DEPTH)
			+ (0x1 << GUI_VS_USE_PICTURE);

		return m_GuiVSList.GetShader(vs_flags);
	}

	IVertexShader* GetGuiPointVS(bool use_rotate, bool use_depth)
	{
		size_t vs_flags = (size_t(use_rotate) << GUI_VS_USE_ROTATE) 
			+ (size_t(use_depth) << GUI_VS_USE_DEPTH)
			+ (0x1 << GUI_VS_USE_POINT);

		return m_GuiVSList.GetShader(vs_flags);
	}

	IVertexShader* GetGuiVS(bool use_point, bool use_picture, bool use_rotate, bool use_depth)
	{
		size_t vs_flags =  0;
		vs_flags |= size_t(use_point) << GUI_VS_USE_POINT;
		vs_flags |= size_t(use_picture) << GUI_VS_USE_PICTURE;
		vs_flags |= size_t(use_rotate) << GUI_VS_USE_ROTATE;
		vs_flags |= size_t(use_depth) << GUI_VS_USE_DEPTH;
		return m_GuiVSList.GetShader(vs_flags);
	}

	IPixelShader* GetGuiLinePS(bool use_fade)
	{
		size_t ps_flags = (size_t(use_fade) << GUI_PS_USE_FADE);

		return m_GuiPSList.GetShader(ps_flags);
	}

	IPixelShader* GetGuiPictPS(bool use_fade)
	{
		size_t ps_flags = (size_t(use_fade) << GUI_PS_USE_FADE)
			+ (0x1 << GUI_PS_USE_PICTURE);

		return m_GuiPSList.GetShader(ps_flags);
	}

	IPixelShader* GetGuiBWPictPS(bool use_fade)
	{
		size_t ps_flags = (size_t(use_fade) << GUI_PS_USE_FADE)
			+ (0x1 << GUI_PS_USE_PICTURE)
			+ (0x1 << GUI_PS_USE_BLACKWHITE);

		return m_GuiPSList.GetShader(ps_flags);
	}

	IPixelShader* GetGuiA8PictPS(bool use_fade)
	{
		size_t ps_flags = (size_t(use_fade) << GUI_PS_USE_FADE)
			+ (0x1 << GUI_PS_USE_PICTURE)
			+ (0x1 << GUI_PS_USE_PICTUREA8);

		return m_GuiPSList.GetShader(ps_flags);
	}

	IPixelShader* GetGuiPointPS(bool use_fade)
	{
		size_t ps_flags = (size_t(use_fade) << GUI_PS_USE_FADE);

		return m_GuiPSList.GetShader(ps_flags);
	}

	IPixelShader* GetGuiPS(bool use_picture, bool use_a8, bool use_fade, bool use_blackwhite, bool use_outline)
	{
		size_t ps_flags =  0;
		ps_flags |= size_t(use_picture) << GUI_PS_USE_PICTURE;
		ps_flags |= size_t(use_a8) << GUI_PS_USE_PICTUREA8;
		ps_flags |= size_t(use_fade) << GUI_PS_USE_FADE;
		ps_flags |= size_t(use_blackwhite) << GUI_PS_USE_BLACKWHITE;
		ps_flags |= size_t(use_outline) << GUI_PS_USE_OUTLINE;
		return m_GuiPSList.GetShader(ps_flags);
	}

private:
	Render* m_pRender;
	bool m_bScaleEnable;
	size_t m_nBatchUsed;
	TArrayPod<batch_t*, 256, TPaintBatchAlloc> m_BatchCache;
	TArrayPod<batch_t*, 256, TPaintBatchAlloc> m_Batchs;
	TArrayPod<viewport_t, 8, TPaintBatchAlloc> m_ViewportStack;
	viewport_t m_CurViewport;
	TShaderList<IVertexShader, GUI_VS_MAX> m_GuiVSList;
	TShaderList<IPixelShader, GUI_PS_MAX> m_GuiPSList;
	TArrayPod<vertex_line_t, 1, TPaintBatchAlloc> m_VertexLine;
	TArrayPod<vertex_pict_t, 1, TPaintBatchAlloc> m_VertexPict;
	TArrayPod<vertex_point_t, 1, TPaintBatchAlloc> m_VertexPoint;
	size_t m_nLineVBSize;
	size_t m_nPictVBSize;
	size_t m_nPointVBSize;
	IDynamicVB* m_pLineVB;
	IDynamicVB* m_pPictVB;
	IDynamicVB* m_pPointVB;
};

#endif // _PAINT_BATCH_H
