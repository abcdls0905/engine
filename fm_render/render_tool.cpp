//--------------------------------------------------------------------
// 文件名:		render_tool.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2012年11月8日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "render_tool.h"
#include "render.h"
#include "color_rt.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../public/auto_mem.h"

CRenderTool* CRenderTool::NewInstance(Render* pRender)
{
	CRenderTool* p = (CRenderTool*)CORE_ALLOC(sizeof(CRenderTool));
	
	new (p) CRenderTool(pRender);
	
	return p;
}

CRenderTool::CRenderTool(Render* pRender)
{
	Assert(pRender != NULL);
	
	m_pRender = pRender;
	m_pPostRT0 = NULL;
	m_pPostRT1 = NULL;
	m_pConeVB = NULL;
	m_pConeIB = NULL;
	m_nConeVertexNum = 0;
	m_nConeTriangleNum = 0;
	m_pSphereVB = NULL;
	m_pSphereIB = NULL;
	m_nSphereVertexNum = 0;
	m_nSphereTriangleNum = 0;
	m_pFineSphereVB = NULL;
	m_pFineSphereIB = NULL;
	m_nFineSphereVertexNum = 0;
	m_nFineSphereTriangleNum = 0;
	m_pScreenQuadVB = NULL;
	m_pScreenQuadIB = NULL;
	m_pCubeVB = NULL;
	m_pCubeIB = NULL;
	m_pHalfCubeVB = NULL;
	m_pHalfCubeIB = NULL;
	m_pPyramidVB = NULL;
	m_pPyramidNXIB = NULL;
	m_pPyramidPXIB = NULL;
	m_pPyramidNZIB = NULL;
	m_pPyramidPZIB = NULL;
	m_pPyramidNYIB = NULL;
	m_pPyramidPYIB = NULL;
	m_pBoundBoxVB = NULL;
	m_pBoundBoxIB = NULL;
	m_pDesignLineVB = NULL;
	m_pPictureVB = NULL;
	m_pDesignLineVS = NULL;
	m_pDesignLinePS = NULL;
	m_pPictureVS = NULL;
	m_pPicturePS = NULL;
	m_pPictureAlphaPS = NULL;
	m_pPictureNormalPS = NULL;
	m_pPictureLengthPS = NULL;
	m_pPictureGammaPS = NULL;
	m_pCopyTexVS = NULL;
	m_pCopyTexPS = NULL;
	m_pCopyTexGammaPS = NULL;
	m_pCopyTexLinearPS = NULL;
	m_pCopyDepthVS = NULL;
	m_pCopyDepthPS = NULL;
	m_pCopyMinDepthVS = NULL;
	m_pCopyMinDepthPS = NULL;
	m_pRevertDepthVS = NULL;
	m_pRevertDepthPS = NULL;
	m_pBoundBoxVS = NULL;
	m_pBoundBoxPS = NULL;
}

CRenderTool::~CRenderTool()
{
	SAFE_RELEASE(m_pPostRT0);
	SAFE_RELEASE(m_pPostRT1);

	for (size_t k = 0; k < m_BlackWhiteTexs.size(); ++k)
	{
		m_BlackWhiteTexs[k].pTexture->Release();
	}
}

void CRenderTool::Release()
{
	CORE_DELETE(this);
}

bool CRenderTool::Initialize()
{
	CreateCommonResource();
	CreateGeometryResource();
	
	return true;
}

IDynamicVB* CRenderTool::CreateDesignLineVB(unsigned int need_size)
{
	if (NULL == m_pDesignLineVB)
	{
		m_pDesignLineVB = m_pRender->CreateDynamicVB(need_size,"CRenderTool::CreateDesignLineVB NULL");
	}
	else if (m_pDesignLineVB->GetSize() < need_size)
	{
		m_pDesignLineVB->Release();
		m_pDesignLineVB = m_pRender->CreateDynamicVB(need_size),"CRenderTool::CreateDesignLineVB Size";
	}
	
	return m_pDesignLineVB;
}

IDynamicVB* CRenderTool::CreateBoundBoxVB(unsigned int size)
{
	SAFE_RELEASE(m_pBoundBoxVB);
	m_pBoundBoxVB = m_pRender->CreateDynamicVB(size,"CRenderTool::CreateBoundBoxVB");

	return m_pBoundBoxVB;
}

IDynamicIB* CRenderTool::CreateBoundBoxIB(unsigned int size)
{
	SAFE_RELEASE(m_pBoundBoxIB);
	m_pBoundBoxIB = m_pRender->CreateDynamicIB(size);

	return m_pBoundBoxIB;
}

bool CRenderTool::CreateCommonResource()
{
	m_pDesignLineVS = m_pRender->LoadVertexShader("design_line.hlsl", 
		"vs_main_design_line", "");
	m_pDesignLinePS = m_pRender->LoadPixelShader("design_line.hlsl", 
		"ps_main_design_line", "");
	m_pPictureVS = m_pRender->LoadVertexShader("picture.hlsl", 
		"vs_main_picture", "");
	m_pPicturePS = m_pRender->LoadPixelShader("picture.hlsl", 
		"ps_main_picture", "");
	m_pPictureAlphaPS = m_pRender->LoadPixelShader("picture.hlsl", 
		"ps_main_picture", "SHOW_ALPHA=1;");
	m_pPictureNormalPS = m_pRender->LoadPixelShader("picture.hlsl", 
		"ps_main_picture", "SHOW_NORMAL=1;");
	m_pPictureLengthPS = m_pRender->LoadPixelShader("picture.hlsl", 
		"ps_main_picture", "SHOW_LENGTH=1;");
	m_pPictureGammaPS = m_pRender->LoadPixelShader("picture.hlsl", 
		"ps_main_picture", "SHOW_GAMMA=1;");
	m_pCopyTexVS = m_pRender->LoadVertexShader("copy_tex.hlsl", 
		"vs_main_copy_tex", "");
	m_pCopyTexPS = m_pRender->LoadPixelShader("copy_tex.hlsl", 
		"ps_main_copy_tex", "");
	m_pCopyTexGammaPS = m_pRender->LoadPixelShader("copy_tex.hlsl", 
		"ps_main_copy_tex", "GAMMA=1;");
	m_pCopyTexLinearPS = m_pRender->LoadPixelShader("copy_tex.hlsl", 
		"ps_main_copy_tex", "LINEAR=1;");
	m_pCopyDepthVS = m_pRender->LoadVertexShader("copy_depth.hlsl", 
		"vs_main_copy_depth", "");
	m_pCopyDepthPS = m_pRender->LoadPixelShader("copy_depth.hlsl", 
		"ps_main_copy_depth", "");
	m_pCopyMinDepthVS = m_pRender->LoadVertexShader("copy_min_depth.hlsl", 
		"vs_main_copy_min_depth", "");
	m_pCopyMinDepthPS = m_pRender->LoadPixelShader("copy_min_depth.hlsl", 
		"ps_main_copy_min_depth", "");
	m_pRevertDepthVS = m_pRender->LoadVertexShader("revert_depth.hlsl", 
		"vs_main_revert_depth", "");
	m_pRevertDepthPS = m_pRender->LoadPixelShader("revert_depth.hlsl", 
		"ps_main_revert_depth", "");
	m_pBoundBoxVS = m_pRender->LoadVertexShader("bound_box.hlsl", 
		"vs_main_bound_box", "");
	m_pBoundBoxPS = m_pRender->LoadPixelShader("bound_box.hlsl", 
		"ps_main_bound_box", "");

	// 顶点缓冲
	m_pPictureVB = m_pRender->CreateDynamicVB(
		(sizeof(FmVec3) + sizeof(FmVec2)) * 6,"CRenderTool::CreateCommonResource");

	return true;
}

void CRenderTool::CreateGeometryResource()
{
	// 屏幕矩形
	SAFE_RELEASE(m_pScreenQuadVB);
	SAFE_RELEASE(m_pScreenQuadIB);

	struct screen_quad_vertex_t
	{
		FmVec4	position;
	};

	screen_quad_vertex_t pScreenQuadVB[4] = 
	{
		FmVec4(-1.0F, -1.0F, +0.0F, +1.0F),
		FmVec4(-1.0F, +1.0F, +0.0F, +1.0F),
		FmVec4(+1.0F, +1.0F, +0.0F, +1.0F),
		FmVec4(+1.0F, -1.0F, +0.0F, +1.0F),
	};

	unsigned short pScreenQuadIB[6] = { 0, 1, 2, 0, 2, 3 };

	m_pScreenQuadVB = m_pRender->CreateStaticVB(pScreenQuadVB, 
		sizeof(pScreenQuadVB));
	m_pScreenQuadIB = m_pRender->CreateStaticIB(pScreenQuadIB, 
		sizeof(pScreenQuadIB));

	// 球体网格
	SAFE_RELEASE(m_pSphereVB);
	SAFE_RELEASE(m_pSphereIB);

	int ring_num = 4;
	int section_num = 8;
	// Generate space for the required triangles and vertices.
	int triangle_num = (ring_num + 1) * section_num * 2;
	int index_num  = triangle_num * 3;
	int vertex_num = (ring_num + 1) * section_num + 2;

	m_nSphereVertexNum = vertex_num;
	m_nSphereTriangleNum = triangle_num;

	FmVec4* pVertices = (FmVec4*)CORE_ALLOC(
		vertex_num * sizeof(FmVec4));
	unsigned short* pIndices = (unsigned short*)CORE_ALLOC(
		triangle_num * 3 * sizeof(unsigned short));

	//if (m_pSphereVB && m_pSphereIB)
	{
		float radius = 1.0F;
		float sx = 1.0F;
		float sy = 1.0F;
		float sz = 1.0F;

		// Generate vertices at the top and bottom points.
		pVertices[0] = FmVec4(0.0F, +sy * radius, 0.0F, 1.0F);
		pVertices[vertex_num - 1] = FmVec4(0.0F, -sy * radius, 0.0F, 1.0F);

		// Generate vertex points for rings
		float dtheta = (FLOAT_PI / (ring_num + 2)); // Angle between each ring
		float dphi = (2 * FLOAT_PI / (section_num)); // Angle between each section
		float theta  = dtheta;
		int n = 1; // vertex being generated, begins at 1 to skip top point
		int i;
		int j;

		for (i = 0; i < (ring_num + 1); i++)
		{
			float y = radius * (float)cos(theta); // y is the same for each ring
			float rsintheta = radius * (float)sin(theta);
			float phi = 0.0F;

			for (j = 0; j < section_num; j++)
			{
				float x = rsintheta * (float)sin(phi);
				float z = rsintheta * (float)cos(phi);

				pVertices[n] = FmVec4(sx * x, sy * y, sz * z, 1.0F);
				phi += dphi;
				++n;
			}

			theta += dtheta;
		}

		// Generate triangles for top and bottom caps.
		for (i = 0; i < section_num; i++)
		{
			pIndices[3 * i + 0] = 0;
			pIndices[3 * i + 1] = i + 1;
			pIndices[3 * i + 2] = 1 + ((i + 1) % section_num);
			pIndices[3 * (triangle_num - section_num + i) + 0] = 
				(unsigned short)(vertex_num - 1);
			pIndices[3 * (triangle_num - section_num + i) + 1] = 
				(unsigned short)(vertex_num - 2 - i);
			pIndices[3 * (triangle_num - section_num + i) + 2] = 
				(unsigned short)(vertex_num - 2 - ((1 + i) % section_num));
		}

		// Generate triangles for the rings
		int m = 1; // first vertex in current ring,begins at 1 to skip top point

		n = section_num; // triangle being generated, skip the top cap 

		for (i = 0; i < ring_num; i++ )
		{
			for (j = 0; j < section_num; j++)
			{
				pIndices[3 * n + 0] = m + j;
				pIndices[3 * n + 1] = m + section_num + j;
				pIndices[3 * n + 2] = m + section_num + ((j + 1) % section_num);
				pIndices[3 * (n + 1) + 0] = pIndices[3 * n + 0];
				pIndices[3 * (n + 1) + 1] = pIndices[3 * n + 2];
				pIndices[3 * (n + 1) + 2] = m + ((j + 1) % section_num);
				n += 2;
			}

			m += section_num;
		}
	}

	m_pSphereVB = m_pRender->CreateStaticVB(pVertices, 
		vertex_num * sizeof(FmVec4));
	m_pSphereIB = m_pRender->CreateStaticIB(pIndices, 
		triangle_num * 3 * sizeof(unsigned short));
	CORE_FREE(pVertices, vertex_num * sizeof(FmVec4));
	CORE_FREE(pIndices, triangle_num * 3 * sizeof(unsigned short));

	// 精细球体网格
	SAFE_RELEASE(m_pFineSphereVB);
	SAFE_RELEASE(m_pFineSphereIB);

	ring_num = 16;
	section_num = 32;
	// Generate space for the required triangles and vertices.
	triangle_num = (ring_num + 1) * section_num * 2;
	index_num  = triangle_num * 3;
	vertex_num = (ring_num + 1) * section_num + 2;

	m_nFineSphereVertexNum = vertex_num;
	m_nFineSphereTriangleNum = triangle_num;

	pVertices = (FmVec4*)CORE_ALLOC(
		vertex_num * sizeof(FmVec4));
	pIndices = (unsigned short*)CORE_ALLOC(
		triangle_num * 3 * sizeof(unsigned short));

	//if (m_pFineSphereVB && m_pFineSphereIB)
	{
		float radius = 1.0F;
		float sx = 1.0F;
		float sy = 1.0F;
		float sz = 1.0F;

		// Generate vertices at the top and bottom points.
		pVertices[0] = FmVec4(0.0F, +sy * radius, 0.0F, 1.0F);
		pVertices[vertex_num - 1] = FmVec4(0.0F, -sy * radius, 0.0F, 1.0F);

		// Generate vertex points for rings
		float dtheta = (FLOAT_PI / (ring_num + 2)); // Angle between each ring
		float dphi = (2 * FLOAT_PI / (section_num)); // Angle between each section
		float theta  = dtheta;
		int n = 1; // vertex being generated, begins at 1 to skip top point
		int i;
		int j;

		for (i = 0; i < (ring_num + 1); i++)
		{
			float y = radius * (float)cos(theta); // y is the same for each ring
			float rsintheta = radius * (float)sin(theta);
			float phi = 0.0F;

			for (j = 0; j < section_num; j++)
			{
				float x = rsintheta * (float)sin(phi);
				float z = rsintheta * (float)cos(phi);

				pVertices[n] = FmVec4(sx * x, sy * y, sz * z, 1.0F);
				phi += dphi;
				++n;
			}

			theta += dtheta;
		}

		// Generate triangles for top and bottom caps.
		for (i = 0; i < section_num; i++)
		{
			pIndices[3 * i + 0] = 0;
			pIndices[3 * i + 1] = i + 1;
			pIndices[3 * i + 2] = 1 + ((i + 1) % section_num);
			pIndices[3 * (triangle_num - section_num + i) + 0] = 
				(unsigned short)(vertex_num - 1);
			pIndices[3 * (triangle_num - section_num + i) + 1] = 
				(unsigned short)(vertex_num - 2 - i);
			pIndices[3 * (triangle_num - section_num + i) + 2] = 
				(unsigned short)(vertex_num - 2 - ((1 + i) % section_num));
		}

		// Generate triangles for the rings
		int m = 1; // first vertex in current ring,begins at 1 to skip top point

		n = section_num; // triangle being generated, skip the top cap 

		for (i = 0; i < ring_num; i++ )
		{
			for (j = 0; j < section_num; j++)
			{
				pIndices[3 * n + 0] = m + j;
				pIndices[3 * n + 1] = m + section_num + j;
				pIndices[3 * n + 2] = m + section_num + ((j + 1) % section_num);
				pIndices[3 * (n + 1) + 0] = pIndices[3 * n + 0];
				pIndices[3 * (n + 1) + 1] = pIndices[3 * n + 2];
				pIndices[3 * (n + 1) + 2] = m + ((j + 1) % section_num);
				n += 2;
			}

			m += section_num;
		}
	}

	m_pFineSphereVB = m_pRender->CreateStaticVB(pVertices, 
		vertex_num * sizeof(FmVec4));
	m_pFineSphereIB = m_pRender->CreateStaticIB(pIndices, 
		triangle_num * 3 * sizeof(unsigned short));
	CORE_FREE(pVertices, vertex_num * sizeof(FmVec4));
	CORE_FREE(pIndices, triangle_num * 3 * sizeof(unsigned short));

	// 圆锥体网格
	SAFE_RELEASE(m_pConeVB);
	SAFE_RELEASE(m_pConeIB);

	int cone_section_num = 16;
	int cone_vertex_num = cone_section_num + 2;
	int cone_triangle_num = cone_section_num * 2;

	m_nConeVertexNum = cone_vertex_num;
	m_nConeTriangleNum = cone_triangle_num;

	pVertices = (FmVec4*)CORE_ALLOC(
		cone_vertex_num * sizeof(FmVec4));
	pIndices = (unsigned short*)CORE_ALLOC(
		cone_triangle_num * 3 * sizeof(unsigned short));

	//if (m_pConeVB && m_pConeIB)
	{
		pVertices[0] = FmVec4(0.0F, 0.0F, -1.0F, 0.0F);
		pVertices[cone_vertex_num - 1] = FmVec4(0.0F, 0.0F, 0.0F, 1.0F);

		float cone_theta = 0.0F;
		int i;
		int k = 0;

		for (i = 0; i < cone_section_num; ++i)
		{
			pVertices[i + 1].x = 1.0F * cosf(cone_theta);
			pVertices[i + 1].y = 1.0F * sinf(cone_theta);
			pVertices[i + 1].z = 0.0F;
			pVertices[i + 1].w = 1.0F;
			cone_theta += (FLOAT_PI * 2.0F) / cone_section_num;
		}

		for (i = 0; i < cone_section_num; ++i)
		{
			pIndices[k++] = 0;
			pIndices[k++] = ((i + 1) % cone_section_num) + 1;
			pIndices[k++] = i + 1;
		}

		for (i = 0; i < cone_section_num; ++i)
		{
			pIndices[k++] = cone_vertex_num - 1;
			pIndices[k++] = i + 1;
			pIndices[k++] = ((i + 1) % cone_section_num) + 1;
		}
	}

	m_pConeVB = m_pRender->CreateStaticVB(pVertices, 
		cone_vertex_num * sizeof(FmVec4));
	m_pConeIB = m_pRender->CreateStaticIB(pIndices, 
		cone_triangle_num * 3 * sizeof(unsigned short));
	CORE_FREE(pVertices, cone_vertex_num * sizeof(FmVec4));
	CORE_FREE(pIndices, cone_triangle_num * 3 * sizeof(unsigned short));

	// 立方体网格
	SAFE_RELEASE(m_pCubeVB);
	SAFE_RELEASE(m_pCubeIB);

	FmVec4 cube_vb[8] = 
	{
		FmVec4(-1.0F, +1.0F, +1.0F, +1.0F),
		FmVec4(+1.0F, +1.0F, +1.0F, +1.0F),
		FmVec4(-1.0F, -1.0F, +1.0F, +1.0F),
		FmVec4(+1.0F, -1.0F, +1.0F, +1.0F),
		FmVec4(-1.0F, +1.0F, -1.0F, +1.0F),
		FmVec4(+1.0F, +1.0F, -1.0F, +1.0F),
		FmVec4(-1.0F, -1.0F, -1.0F, +1.0F),
		FmVec4(+1.0F, -1.0F, -1.0F, +1.0F),
	};

	unsigned short cube_ib[12 * 3] = 
	{
		0, 2, 1,
		1, 2, 3,
		0, 1, 5,
		0, 5, 4,
		0, 6, 2,
		0, 4, 6,
		1, 3, 7,
		1, 7, 5,
		2, 6, 7,
		2, 7, 3,
		4, 5, 6,
		5, 7, 6,
	};

	m_pCubeVB = m_pRender->CreateStaticVB(cube_vb, sizeof(cube_vb));
	m_pCubeIB = m_pRender->CreateStaticIB(cube_ib, sizeof(cube_ib));

	// 半立方体网格
	SAFE_RELEASE(m_pHalfCubeVB);
	SAFE_RELEASE(m_pHalfCubeIB);

	FmVec4 half_cube_vb[8] = 
	{
		FmVec4(-1.0F, +1.0F, 1.0F, +1.0F),
		FmVec4(+1.0F, +1.0F, 1.0F, +1.0F),
		FmVec4(-1.0F, -1.0F, 1.0F, +1.0F),
		FmVec4(+1.0F, -1.0F, 1.0F, +1.0F),
		FmVec4(-1.0F, +1.0F, 0.0F, +1.0F),
		FmVec4(+1.0F, +1.0F, 0.0F, +1.0F),
		FmVec4(-1.0F, -1.0F, 0.0F, +1.0F),
		FmVec4(+1.0F, -1.0F, 0.0F, +1.0F),
	};

	unsigned short half_cube_ib[12 * 3] = 
	{
		0, 2, 1,
		1, 2, 3,
		0, 1, 5,
		0, 5, 4,
		0, 6, 2,
		0, 4, 6,
		1, 3, 7,
		1, 7, 5,
		2, 6, 7,
		2, 7, 3,
		4, 5, 6,
		5, 7, 6,
	};

	m_pHalfCubeVB = m_pRender->CreateStaticVB(half_cube_vb, 
		sizeof(half_cube_vb));
	m_pHalfCubeIB = m_pRender->CreateStaticIB(half_cube_ib, 
		sizeof(half_cube_ib));
	
	// 金字塔网格
	SAFE_RELEASE(m_pPyramidVB);
	SAFE_RELEASE(m_pPyramidNXIB);
	SAFE_RELEASE(m_pPyramidPXIB);
	SAFE_RELEASE(m_pPyramidNZIB);
	SAFE_RELEASE(m_pPyramidPZIB);
	SAFE_RELEASE(m_pPyramidNYIB);
	SAFE_RELEASE(m_pPyramidPYIB);

	FmVec4 pyramid_vb[9] = 
	{
		FmVec4(+0.0F, +0.0F, +0.0F, +1.0F),
		FmVec4(-1.0F, +1.0F, +1.0F, +1.0F),
		FmVec4(+1.0F, +1.0F, +1.0F, +1.0F),
		FmVec4(-1.0F, -1.0F, +1.0F, +1.0F),
		FmVec4(+1.0F, -1.0F, +1.0F, +1.0F),
		FmVec4(-1.0F, +1.0F, -1.0F, +1.0F),
		FmVec4(+1.0F, +1.0F, -1.0F, +1.0F),
		FmVec4(-1.0F, -1.0F, -1.0F, +1.0F),
		FmVec4(+1.0F, -1.0F, -1.0F, +1.0F),
	};

	unsigned short pyramid_nx_ib[6 * 3] = 
	{
		0, 1, 3,
		0, 5, 1,
		0, 7, 5,
		0, 3, 7,
		1, 7, 3,
		1, 5, 7,
	};

	unsigned short pyramid_px_ib[6 * 3] = 
	{
		0, 4, 2,
		0, 2, 6,
		0, 6, 8,
		0, 8, 4,
		2, 4, 8,
		2, 8, 6,
	};

	unsigned short pyramid_nz_ib[6 * 3] = 
	{
		0, 6, 5,
		0, 5, 7,
		0, 7, 8,
		0, 8, 6,
		5, 6, 8,
		5, 8, 7,
	};

	unsigned short pyramid_pz_ib[6 * 3] = 
	{
		0, 1, 2,
		0, 3, 1,
		0, 4, 3,
		0, 2, 4,
		1, 4, 2,
		1, 3, 4,
	};

	unsigned short pyramid_ny_ib[6 * 3] = 
	{
		0, 3, 4,
		0, 7, 3,
		0, 8, 7,
		0, 4, 8,
		3, 8, 4,
		3, 7, 8,
	};

	unsigned short pyramid_py_ib[6 * 3] = 
	{
		0, 2, 1,
		0, 1, 5,
		0, 5, 6,
		0, 6, 2,
		1, 6, 2,
		1, 5, 6,
	};

	m_pPyramidVB = m_pRender->CreateStaticVB(pyramid_vb, sizeof(pyramid_vb));
	m_pPyramidNXIB = m_pRender->CreateStaticIB(pyramid_nx_ib, 
		sizeof(pyramid_nx_ib));
	m_pPyramidPXIB = m_pRender->CreateStaticIB(pyramid_px_ib, 
		sizeof(pyramid_px_ib));
	m_pPyramidNZIB = m_pRender->CreateStaticIB(pyramid_nz_ib, 
		sizeof(pyramid_nz_ib));
	m_pPyramidPZIB = m_pRender->CreateStaticIB(pyramid_pz_ib, 
		sizeof(pyramid_pz_ib));
	m_pPyramidNYIB = m_pRender->CreateStaticIB(pyramid_ny_ib, 
		sizeof(pyramid_ny_ib));
	m_pPyramidPYIB = m_pRender->CreateStaticIB(pyramid_py_ib, 
		sizeof(pyramid_pz_ib));
}

void CRenderTool::DrawScreenQuad()
{
//	ID3D11Buffer* pVBuffer = m_pScreenQuadVB->GetBuffer();
//	unsigned int stride = sizeof(FmVec4);
//	unsigned int offset = 0;
//
//	m_pDeviceContext->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
//	m_pDeviceContext->IASetIndexBuffer(m_pScreenQuadIB->GetBuffer(), 
//		DXGI_FORMAT_R16_UINT, 0);
//	m_pDeviceContext->IASetInputLayout(m_pShapeXYZWLayout->GetInputLayout());
//	DRAW_INDEXED_PRIMITIVE(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 6, 0, 0);
}

void CRenderTool::DrawSphere()
{
//	ID3D11Buffer* pVBuffer = m_pSphereVB->GetBuffer();
//	unsigned int stride = sizeof(FmVec4);
//	unsigned int offset = 0;
//
//	m_pDeviceContext->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
//	m_pDeviceContext->IASetIndexBuffer(m_pSphereIB->GetBuffer(), 
//		DXGI_FORMAT_R16_UINT, 0);
//	m_pDeviceContext->IASetInputLayout(m_pShapeXYZWLayout->GetInputLayout());
//	DRAW_INDEXED_PRIMITIVE(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 
//		m_nSphereTriangleNum * 3, 0, 0);
}

void CRenderTool::DrawFineSphere()
{
//	ID3D11Buffer* pVBuffer = m_pFineSphereVB->GetBuffer();
//	unsigned int stride = sizeof(FmVec4);
//	unsigned int offset = 0;
//
//	m_pDeviceContext->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
//	m_pDeviceContext->IASetIndexBuffer(m_pFineSphereIB->GetBuffer(), 
//		DXGI_FORMAT_R16_UINT, 0);
//	m_pDeviceContext->IASetInputLayout(m_pShapeXYZWLayout->GetInputLayout());
//	DRAW_INDEXED_PRIMITIVE(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
//		m_nFineSphereTriangleNum * 3, 0, 0);
}

void CRenderTool::DrawCone()
{
//	ID3D11Buffer* pVBuffer = m_pConeVB->GetBuffer();
//	unsigned int stride = sizeof(FmVec4);
//	unsigned int offset = 0;
//
//	m_pDeviceContext->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
//	m_pDeviceContext->IASetIndexBuffer(m_pConeIB->GetBuffer(), 
//		DXGI_FORMAT_R16_UINT, 0);
//	m_pDeviceContext->IASetInputLayout(m_pShapeXYZWLayout->GetInputLayout());
//	DRAW_INDEXED_PRIMITIVE(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
//		m_nConeTriangleNum * 3, 0, 0);
}

void CRenderTool::DrawCube()
{
//	ID3D11Buffer* pVBuffer = m_pCubeVB->GetBuffer();
//	unsigned int stride = sizeof(FmVec4);
//	unsigned int offset = 0;
//
//	m_pDeviceContext->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
//	m_pDeviceContext->IASetIndexBuffer(m_pCubeIB->GetBuffer(), 
//		DXGI_FORMAT_R16_UINT, 0);
//	m_pDeviceContext->IASetInputLayout(m_pShapeXYZWLayout->GetInputLayout());
//	DRAW_INDEXED_PRIMITIVE(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 36, 0, 0);
}

void CRenderTool::DrawHalfCube()
{
//	ID3D11Buffer* pVBuffer = m_pHalfCubeVB->GetBuffer();
//	unsigned int stride = sizeof(FmVec4);
//	unsigned int offset = 0;
//
//	m_pDeviceContext->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
//	m_pDeviceContext->IASetIndexBuffer(m_pHalfCubeIB->GetBuffer(), 
//		DXGI_FORMAT_R16_UINT, 0);
//	m_pDeviceContext->IASetInputLayout(m_pShapeXYZWLayout->GetInputLayout());
//	DRAW_INDEXED_PRIMITIVE(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 36, 0, 0);
}

void CRenderTool::DrawPyramidNX()
{
//	ID3D11Buffer* pVBuffer = m_pPyramidVB->GetBuffer();
//	unsigned int stride = sizeof(FmVec4);
//	unsigned int offset = 0;
//
//	m_pDeviceContext->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
//	m_pDeviceContext->IASetIndexBuffer(m_pPyramidNXIB->GetBuffer(), 
//		DXGI_FORMAT_R16_UINT, 0);
//	m_pDeviceContext->IASetInputLayout(m_pShapeXYZWLayout->GetInputLayout());
//	DRAW_INDEXED_PRIMITIVE(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 18, 0, 0);
}

void CRenderTool::DrawPyramidPX()
{
//	ID3D11Buffer* pVBuffer = m_pPyramidVB->GetBuffer();
//	unsigned int stride = sizeof(FmVec4);
//	unsigned int offset = 0;
//
//	m_pDeviceContext->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
//	m_pDeviceContext->IASetIndexBuffer(m_pPyramidPXIB->GetBuffer(), 
//		DXGI_FORMAT_R16_UINT, 0);
//	m_pDeviceContext->IASetInputLayout(m_pShapeXYZWLayout->GetInputLayout());
//	DRAW_INDEXED_PRIMITIVE(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 18, 0, 0);
}

void CRenderTool::DrawPyramidNZ()
{
//	ID3D11Buffer* pVBuffer = m_pPyramidVB->GetBuffer();
//	unsigned int stride = sizeof(FmVec4);
//	unsigned int offset = 0;
//
//	m_pDeviceContext->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
//	m_pDeviceContext->IASetIndexBuffer(m_pPyramidNZIB->GetBuffer(), 
//		DXGI_FORMAT_R16_UINT, 0);
//	m_pDeviceContext->IASetInputLayout(m_pShapeXYZWLayout->GetInputLayout());
//	DRAW_INDEXED_PRIMITIVE(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 18, 0, 0);
}

void CRenderTool::DrawPyramidPZ()
{
//	ID3D11Buffer* pVBuffer = m_pPyramidVB->GetBuffer();
//	unsigned int stride = sizeof(FmVec4);
//	unsigned int offset = 0;
//
//	m_pDeviceContext->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
//	m_pDeviceContext->IASetIndexBuffer(m_pPyramidPZIB->GetBuffer(), 
//		DXGI_FORMAT_R16_UINT, 0);
//	m_pDeviceContext->IASetInputLayout(m_pShapeXYZWLayout->GetInputLayout());
//	DRAW_INDEXED_PRIMITIVE(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 18, 0, 0);
}

void CRenderTool::DrawPyramidNY()
{
//	ID3D11Buffer* pVBuffer = m_pPyramidVB->GetBuffer();
//	unsigned int stride = sizeof(FmVec4);
//	unsigned int offset = 0;
//
//	m_pDeviceContext->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
//	m_pDeviceContext->IASetIndexBuffer(m_pPyramidNYIB->GetBuffer(), 
//		DXGI_FORMAT_R16_UINT, 0);
//	m_pDeviceContext->IASetInputLayout(m_pShapeXYZWLayout->GetInputLayout());
//	DRAW_INDEXED_PRIMITIVE(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 18, 0, 0);
}

void CRenderTool::DrawPyramidPY()
{
//	ID3D11Buffer* pVBuffer = m_pPyramidVB->GetBuffer();
//	unsigned int stride = sizeof(FmVec4);
//	unsigned int offset = 0;
//
//	m_pDeviceContext->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
//	m_pDeviceContext->IASetIndexBuffer(m_pPyramidPYIB->GetBuffer(), 
//		DXGI_FORMAT_R16_UINT, 0);
//	m_pDeviceContext->IASetInputLayout(m_pShapeXYZWLayout->GetInputLayout());
//	DRAW_INDEXED_PRIMITIVE(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 18, 0, 0);
}

CColorRT* CRenderTool::GetPostRT0(TEX_FORMAT format, bool auto_gen_mipmap)
{
	if (m_pPostRT0)
	{
		if ((m_pPostRT0->GetFormat() != format)
			|| (m_pPostRT0->GetAutoGenMipmap() != auto_gen_mipmap))
		{
			m_pPostRT0->Release();
			m_pPostRT0 = NULL;
		}
	}

	if (NULL == m_pPostRT0)
	{
		m_pPostRT0 = (CColorRT*)m_pRender->CreateColorRT(
			m_pRender->GetDeviceWidth(), m_pRender->GetDeviceHeight(), 
			format, RT_TYPE_DEVICE);
	}

	return m_pPostRT0;
}

CColorRT* CRenderTool::GetPostRT1(TEX_FORMAT format, bool auto_gen_mipmap)
{
	if (m_pPostRT1)
	{
		if ((m_pPostRT1->GetFormat() != format)
			|| (m_pPostRT1->GetAutoGenMipmap() != auto_gen_mipmap))
		{
			m_pPostRT1->Release();
			m_pPostRT1 = NULL;
		}
	}

	if (NULL == m_pPostRT1)
	{
		m_pPostRT1 = (CColorRT*)m_pRender->CreateColorRT(
			m_pRender->GetDeviceWidth(), m_pRender->GetDeviceHeight(), 
			format, RT_TYPE_DEVICE);
	}

	return m_pPostRT1;
}

IStaticTex* CRenderTool::GetBlackWhiteTex(IShaderTex* pShaderTex)
{
	return NULL;
	/*
//	if (NULL == pShaderView)
//	{
//		return NULL;
//	}
//
//	ID3D11Resource* pResource;
//	D3D11_RESOURCE_DIMENSION nResType;
//	
//	pShaderView->GetResource(&pResource);
//	pResource->GetType(&nResType);
//
//	if (nResType != D3D11_RESOURCE_DIMENSION_TEXTURE2D)
	{
		pResource->Release();
		return pShaderView;
	}

	ID3D11Texture2D* pTex2D = (ID3D11Texture2D*)pResource;
	D3D11_TEXTURE2D_DESC td;

	pTex2D->GetDesc(&td);
	pTex2D->Release();

	unsigned int width = td.Width;
	unsigned int height = td.Height;
	const size_t tex_num = m_BlackWhiteTexs.size();

	if (tex_num > 0)
	{
		bw_tex_t* texs = m_BlackWhiteTexs.data();

		for (size_t i = 0; i < tex_num; ++i)
		{
			if ((texs[i].nWidth == width) && (texs[i].nHeight == height))
			{
				return texs[i].pTexture->GetShaderView();
			}
		}
	}

	TAutoMem<unsigned char, 1, TCoreAlloc> auto_buf(width * height * 4);
	unsigned char* dst = auto_buf.GetBuffer();

	for (unsigned int y = 0; y < height; ++y)
	{
		unsigned int y_flag = (y / 16) & 0x1;
		unsigned int* data = (unsigned int*)dst;

		for (unsigned int x = 0; x < width; ++x)
		{
			unsigned int x_flag = (x / 16) & 0x1;

			if ((x_flag + y_flag) == 1)
			{
				data[x] = 0xFFFFFFFF;
			}
			else
			{
				data[x] = 0xFF000000;
			}
		}

		dst += width * 4;
	}

	IStaticTex* pTex = m_pRender->CreateStaticTex(width, height, 1,
		DXGI_FORMAT_B8G8R8A8_UNORM, auto_buf.GetBuffer(), width * height * 4);

	bw_tex_t data;

	data.nWidth = width;
	data.nHeight = height;
	data.pTexture = pTex;

	m_BlackWhiteTexs.push_back(data);

	return pTex->GetShaderView();
	*/
}

void CRenderTool::ClearRenderResource()
{
	SAFE_RELEASE(m_pBoundBoxVB);
	SAFE_RELEASE(m_pBoundBoxIB);
	SAFE_RELEASE(m_pPostRT0);
	SAFE_RELEASE(m_pPostRT1);
	
	for (size_t k = 0; k < m_BlackWhiteTexs.size(); ++k)
	{
		m_BlackWhiteTexs[k].pTexture->Release();
	}

	m_BlackWhiteTexs.clear();
}
