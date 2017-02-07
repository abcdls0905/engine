
/****************************************************************

  File name   :  primitive_3d_drawer.h
  Author      :  叶峰
  Version     :  1.0a
  Create Date :  2013/03/12   
  Description :  三维简单图形渲染

*****************************************************************/

#ifndef __primitive_3d_drawer_H__
#define __primitive_3d_drawer_H__

// INCLUDES -----------------------------------------------------------------------------

#include "../visual/i_primitive_3d_drawer.h"
#include "../visual/i_render.h"

// --------------------------------------------------------------------------------------

#define MAX_BATCH_VERTICES_NUM 96       // 该数值应该为6的倍数,以保证线与面不断开

// --------------------------------------------------------------------------------------

class Primitive3DDrawer : public IPrimitive3DDrawer
{
public:
    Primitive3DDrawer();
    ~Primitive3DDrawer();

	bool					Init();

	bool					Shut();

    // 设置当前场景视图
    void                    SetSceneView(ISceneView* pSceneView);

    // 获得当前场景视图
    ISceneView*             GetSceneView();

    bool                    IsReady() const;

    bool                    SetSlices(unsigned int slices);

    // 世界矩阵
    void                    SetMtxWorld(const FmMat4& mtxWorld);
    void                    SetMtxWorldIdentity();
    const FmMat4&           GetMtxWorld() const;

    // 是否画线模式
    void                    SetWireMode(bool value);
    bool                    GetWireMode() const;

    // 是否背面剔除
    void                    SetCullFace(bool value);
    bool                    GetCullFace() const;

    // 是否使用深度测试
    void                    SetZEnable(bool value);
    bool                    GetZEnable() const;

    /*************基本图形*************/

    // 点
    void                    DrawVertex(
        const FmVec3&       vPos, 
        unsigned int        color
        );

    // 线
    void                    DrawLine(
        const FmVec3&       vStart, 
        const FmVec3&       vEnd, 
        unsigned int        color
        );

    void                    DrawLine(
        const FmVec3&       vStart, 
        const FmVec3&       vEnd, 
        unsigned int        colStart, 
        unsigned int        colEnd
        );

    // 线集
    void                    DrawLineList(
        const FmVec3*       pVertices, 
        const unsigned int* pColors,
        unsigned int        numPoints,
        float               depth = 0.0f
        );

    void                    DrawLineList(
        const FmVec3*       pVertices, 
        unsigned int        color,
        unsigned int        numPoints,
        float               depth = 0.0f
        );

    // 线带
    void                    DrawLineStrip(
        const FmVec3*       pVertices, 
        const unsigned int* pColors,
        unsigned int        numPoints,
        float               depth = 0.0f
        );

    void                    DrawLineStrip(
        const FmVec3*       pVertices, 
        unsigned int        color,
        unsigned int        numPoints,
        float               depth = 0.0f
        );

    //线圈
    void                    DrawLineLoop(
        const FmVec3*       pVertices, 
        const unsigned int* pColors,
        unsigned int        numPoints,
        float               depth = 0.0f
        );

    void                    DrawLineLoop(
        const FmVec3*       pVertices, 
        unsigned int        color,
        unsigned int        numPoints,
        float               depth = 0.0f
        );

    // 三角面
    void                    DrawTriangle(
        const FmVec3&       v1, 
        const FmVec3&       v2,  
        const FmVec3&       v3, 
        unsigned int        color
        );

    void                    DrawTriangle(
        const FmVec3&       v1, 
        const FmVec3&       v2,  
        const FmVec3&       v3, 
        unsigned int        color1, 
        unsigned int        color2,
        unsigned int        color3
        );

    // 四角面
    void                    DrawQuadrangle(
        const FmVec3&       v1, 
        const FmVec3&       v2,  
        const FmVec3&       v3,  
        const FmVec3&       v4, 
        unsigned int        color
        );

    void                    DrawQuadrangle(
        const FmVec3&       v1, 
        const FmVec3&       v2,  
        const FmVec3&       v3,  
        const FmVec3&       v4, 
        unsigned int        color1, 
        unsigned int        color2,
        unsigned int        color3,
        unsigned int        color4
        );

    /*************面图形*************/

    // 平面
    void                    DrawPlane(
        const FmVec3&       vCenter, 
        float               fWidth,
        float               fLength,
        unsigned int        color,
        YeAxis3DType        axisPlane = YE_AXIS_3D_PLANE_XOZ
        );

    // 平面
    void                    DrawPlane(
        const FmPlane&      plane, 
        const FmVec3&       vMin, 
        const FmVec3&       vMax, 
        unsigned int        color
        );

    // 圆面
    void                    DrawCircleRound(
        const FmVec3&       vCenter, 
        float               fHeight,
        float               fRadius,
        unsigned int        colCenter,
        unsigned int        colOuter,
        YeAxis3DType        axisPlane = YE_AXIS_3D_PLANE_XOZ
        );

    // 椭圆面
    void                    DrawEllipseRound(
        const FmVec3&       vCenter, 
        float               fHeight,
        float               fRadiusW,
        float               fRadiusH,
        unsigned int        colCenter,
        unsigned int        colOuter,
        YeAxis3DType        axisPlane = YE_AXIS_3D_PLANE_XOZ
        );

    // 锥体
    void                    DrawCone(
        const FmVec3&       vCenter, 
        float               fHeight,
        float               fRadius,
        unsigned int        colTop,
        unsigned int        colBottom,
        YeAxis3DType        axisPlane = YE_AXIS_3D_PLANE_XOZ
        );

    // 金字塔
    void                    DrawPyramid(
        const FmVec3&       vCenter, 
        float               fWidth,
        float               fLength,
        float               fHeight,
        unsigned int        colTop,
        unsigned int        colBottom,
        YeAxis3DType        axisPlane = YE_AXIS_3D_PLANE_XOZ
        );

    // 环
    void                    DrawLoop(
        const FmVec3&       vPosition, 
        float               fHeight,
        float               fInRadius,
        float               fOutRadius,
        unsigned int        colIn,
        unsigned int        colOut,
        YeAxis3DType        axisPlane = YE_AXIS_3D_PLANE_XOZ
        );

    // 柱
    void                    DrawCylinder(
        const FmVec3&       vPosition, 
        float               fHeight,
        float               fTopRadius,
        float               fBottomRadius,
        unsigned int        colTop,
        unsigned int        colBottom,
        YeAxis3DType        axisPlane = YE_AXIS_3D_PLANE_XOZ
        );

    // 长方形
    void                    DrawBox(
        const FmVec3&       vMin,
        const FmVec3&       vMax,
        unsigned int        color
        );

    // 包围墙面
    void                    DrawBindingWall(
        const FmVec3&       vMin,
        const FmVec3&       vMax,
        float               fWallHeight,
        unsigned int        colTop,
        unsigned int        colBottom
        );

    // 球
    void                    DrawSphere(
        const FmVec3&       vCenter,
        float               fRadius,
        unsigned int        color
        );

    // 半球
    void                    DrawHemisphere(
        const FmVec3&       vCenter,
        float               fRadius,
        unsigned int        color,
        YeAxis3DType        axisDir = YE_AXIS_3D_POSITIVE_Y
        );

    // 胶囊体
    void                    DrawCapsule(
        const FmVec3&       vCenter,
        float               fRadius,
        float               fHeight,
        unsigned int        color,
        YeAxis3DType        axisPlane = YE_AXIS_3D_PLANE_XOZ
        );

    /*************线图形*************/

    // 四边形
    void                    DrawQuadWire(
        const FmVec3&       vCenter,
        float               fSize,
        unsigned int        color,
        YeAxis3DType        axisPlane = YE_AXIS_3D_PLANE_XOZ
        );

    // 圆圈
    void                    DrawCircleWire(
        const FmVec3&       vCenter,
        float               fRadius,
        unsigned int        color,
        YeAxis3DType        axisPlane = YE_AXIS_3D_PLANE_XOZ
        );

    // 椭圆圈
    void                    DrawEllipseWire(
        const FmVec3&       vCenter,
        float               fRadiusW,
        float               fRadiusH,
        unsigned int        color,
        YeAxis3DType        axisPlane = YE_AXIS_3D_PLANE_XOZ
        );

    // 包围盒
    void                    DrawBindingBox(
        const FmVec3&       vMin,
        const FmVec3&       vMax,
        unsigned int        color
        );

    // 每个角上有三角边的包围盒
    void                    DrawSurroundBox(
        const FmVec3&       vMin,
        const FmVec3&       vMax,
        unsigned int        color1,
        unsigned int        color2
        );

    // 包围线圈(圆)
    void                    DrawSurroundCircle(
        const FmVec3&       vCenter,
        float               fRadius,
        unsigned int        colorXOZ,
        unsigned int        colorYOZ,
        unsigned int        colorXOY
        );

    // 包围线圈(椭圆)
    void                    DrawSurroundEllipse(
        const FmVec3&       vMin,
        const FmVec3&       vMax,
        unsigned int        colorXOZ,
        unsigned int        colorYOZ,
        unsigned int        colorXOY
        );

    /*************面线图形*************/

    // 四面体
    void                    DrawTetrahedron(
        const FmVec3*       pVertices,
        unsigned int        faceColor,
        unsigned int        wireColor
        );

    // 六面体
    void                    DrawHexahedron(
        const FmVec3*       pVertices,
        unsigned int        faceColor,
        unsigned int        wireColor
        );

    void                    Flush();

	static void				DrawBatchData(void* pdata);

private:
    void                    PushVertex(
        const FmVec3&       vertex, 
        unsigned int        color, 
		IRenderDrawOp::DRAW_MODE type
        );

    void                    PushVertices(
        const FmVec3*       pVertices, 
        unsigned int        numVertices,
		unsigned int        color, 
		IRenderDrawOp::DRAW_MODE type
        );

    void                    PushVertices(
        const FmVec3*       pVertices, 
        const unsigned int* pColors, 
		unsigned int        numVertices,
		IRenderDrawOp::DRAW_MODE type
        );

    void                    BuildInternal(unsigned int slices);

    void                    ClearInternal();

	// 带颜色的顶点
	struct PrimitiveVertex
	{
		float _x, _y, _z;
		unsigned int _color;

		PrimitiveVertex()
		{
			_x = 0.0f;
			_y = 0.0f; 
			_z = 0.0f;
			_color = 0xffffffff;
		}

		void Set(float x, float y, float z, unsigned int color)
		{
			_x = x;
			_y = y;
			_z = z;
			_color = color;
		}
	};

    // 批次渲染数据
    struct BatchData
    {
        unsigned int				nUsed;
		IRenderDrawOp::DRAW_MODE    drawType;
		PrimitiveVertex				verticesBuf[MAX_BATCH_VERTICES_NUM];
		FmMat4						mtxWVP;

        bool                        bWireMode;
        bool                        bZEnable;
        bool                        bCullFace;

        BatchData*					pNext;

        BatchData()
        {
            nUsed = 0;
			pNext = NULL;
        }
    };

	BatchData*              GetBatchData(IRenderDrawOp::DRAW_MODE drawMode, unsigned int verticesNum);

	void					FlushBatchData(IRenderDrawOp::DRAW_MODE drawMode);

    void                    UpdateBatchData(BatchData* pBatchData, IRenderDrawOp::DRAW_MODE drawMode);

private:
    static IRender*			s_pRender;
	static IShaderProgram*	s_pPrimitiveShader;
	static IDynamicVB*		s_pPrimitiveVB;

	static fm_int			s_hMatWVP;

    ISceneView*             m_pSceneView;

    unsigned int            m_nSlices;
    float*                  m_pSinList;  
    float*                  m_pCosList;
    FmVec3*					m_pVerticesBuffer;
    
	BatchData*				m_listBatchData;
	BatchData*				m_batchData[IRenderDrawOp::DRAW_TRIANGLES + 1];

    FmMat4                  m_mtxWorld;
    bool                    m_bWireMode;
    bool                    m_bZEnable;
    bool                    m_bCullFace;
};

// --------------------------------------------------------------------------------------

#endif