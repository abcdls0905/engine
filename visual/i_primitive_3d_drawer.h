
/****************************************************************

  File name   :  i_primitive_3d_drawer.h
  Author      :  Ҷ��
  Version     :  1.0a
  Create Date :  2013/03/12   
  Description :  ��ά��ͼ����Ⱦ�ӿ�

*****************************************************************/

#ifndef __i_primitive_3d_drawer_H_
#define __i_primitive_3d_drawer_H_

// INCLUDES -----------------------------------------------------------------------------

#include "../public/i_interface.h"
#include "../math/fm_vector.h"
#include "../math/fm_matrix.h"
#include "../math/fm_quaternion.h"

// --------------------------------------------------------------------------------------

class ISceneView;

// --------------------------------------------------------------------------------------

// �����᷽��
enum YeAxis3DType
{
	YE_AXIS_INVALID       = 0,

	YE_AXIS_3D_POSITIVE   = 0,
	YE_AXIS_3D_NEGATIVE   = 1,

	YE_AXIS_3D_X          = 2,
	YE_AXIS_3D_Y          = 4,
	YE_AXIS_3D_Z          = 8,

	YE_AXIS_3D_PLANE_XOY  = YE_AXIS_3D_X | YE_AXIS_3D_Y,
	YE_AXIS_3D_PLANE_XOZ  = YE_AXIS_3D_X | YE_AXIS_3D_Z,
	YE_AXIS_3D_PLANE_YOZ  = YE_AXIS_3D_Y | YE_AXIS_3D_Z,

	YE_AXIS_3D_POSITIVE_X = YE_AXIS_3D_X | YE_AXIS_3D_POSITIVE,
	YE_AXIS_3D_NEGATIVE_X = YE_AXIS_3D_X | YE_AXIS_3D_NEGATIVE,
	YE_AXIS_3D_POSITIVE_Y = YE_AXIS_3D_Y | YE_AXIS_3D_POSITIVE,
	YE_AXIS_3D_NEGATIVE_Y = YE_AXIS_3D_Y | YE_AXIS_3D_NEGATIVE,
	YE_AXIS_3D_POSITIVE_Z = YE_AXIS_3D_Z | YE_AXIS_3D_POSITIVE,
	YE_AXIS_3D_NEGATIVE_Z = YE_AXIS_3D_Z | YE_AXIS_3D_NEGATIVE, 
};

// --------------------------------------------------------------------------------------

class IPrimitive3DDrawer : public IInterface
{
public:
    // ���õ�ǰ������ͼ
    virtual void            SetSceneView(ISceneView* pSceneView) = 0;

    // ��õ�ǰ������ͼ
    virtual ISceneView*     GetSceneView() = 0;

    virtual bool            IsReady() const = 0;

    virtual bool            SetSlices(unsigned int slices) = 0;

    // �������
    virtual void            SetMtxWorld(const FmMat4& mtxWorld) = 0;
    virtual void            SetMtxWorldIdentity() = 0;
    virtual const FmMat4&   GetMtxWorld() const = 0;

    // �Ƿ���ģʽ
    virtual void            SetWireMode(bool value) = 0;
    virtual bool            GetWireMode() const = 0;

    // �Ƿ����޳�
    virtual void            SetCullFace(bool value) = 0;
    virtual bool            GetCullFace() const = 0;

    // �Ƿ�ʹ����Ȳ���
    virtual void            SetZEnable(bool value) = 0;
    virtual bool            GetZEnable() const = 0;


    /*************����ͼ��*************/

    // ��
    virtual void            DrawVertex(
        const FmVec3&		vPos, 
        unsigned int        color
        ) = 0;

    // ��
    virtual void            DrawLine(
        const FmVec3&       vStart, 
        const FmVec3&       vEnd, 
        unsigned int        color
        ) = 0;

    virtual void            DrawLine(
        const FmVec3&       vStart, 
        const FmVec3&       vEnd, 
        unsigned int        colStart, 
        unsigned int        colEnd
        ) = 0;

    // �߼�
    virtual void            DrawLineList(
        const FmVec3*       pVertices, 
        const unsigned int* pColors,
        unsigned int        numPoints,
        float               depth = 0.0f
        ) = 0;

    virtual void            DrawLineList(
        const FmVec3*       pVertices, 
        unsigned int        color,
        unsigned int        numPoints,
        float               depth = 0.0f
        ) = 0;

    // �ߴ�
    virtual void            DrawLineStrip(
        const FmVec3*       pVertices, 
        const unsigned int* pColors,
        unsigned int        numPoints,
        float               depth = 0.0f
        ) = 0;

    virtual void            DrawLineStrip(
        const FmVec3*       pVertices, 
        unsigned int        color,
        unsigned int        numPoints,
        float               depth = 0.0f
        ) = 0;

    //��Ȧ
    virtual void            DrawLineLoop(
        const FmVec3*       pVertices, 
        const unsigned int* pColors,
        unsigned int        numPoints,
        float               depth = 0.0f
        ) = 0;

    virtual void            DrawLineLoop(
        const FmVec3*       pVertices, 
        unsigned int        color,
        unsigned int        numPoints,
        float               depth = 0.0f
        ) = 0;

    // ������
    virtual void            DrawTriangle(
        const FmVec3&       v1, 
        const FmVec3&       v2,  
        const FmVec3&       v3, 
        unsigned int        color
        ) = 0;

    virtual void            DrawTriangle(
        const FmVec3&       v1, 
        const FmVec3&       v2,  
        const FmVec3&       v3, 
        unsigned int        color1, 
        unsigned int        color2,
        unsigned int        color3
        ) = 0;

    // �Ľ���
    virtual void            DrawQuadrangle(
        const FmVec3&       v1, 
        const FmVec3&       v2,  
        const FmVec3&       v3,  
        const FmVec3&       v4, 
        unsigned int        color
        ) = 0;

    virtual void            DrawQuadrangle(
        const FmVec3&       v1, 
        const FmVec3&       v2,  
        const FmVec3&       v3,  
        const FmVec3&       v4, 
        unsigned int        color1, 
        unsigned int        color2,
        unsigned int        color3,
        unsigned int        color4
        ) = 0;

    /*************��ͼ��*************/

    // ƽ��
    virtual void            DrawPlane(
        const FmVec3&       vCenter, 
        float               fWidth,
        float               fLength,
        unsigned int        color,
        YeAxis3DType        axisPlane = YE_AXIS_3D_PLANE_XOZ
        ) = 0;

    // ƽ��
    virtual void            DrawPlane(
        const FmPlane&      plane, 
        const FmVec3&       vMin, 
        const FmVec3&       vMax, 
        unsigned int        color
        ) = 0;

    // Բ��
    virtual void            DrawCircleRound(
        const FmVec3&       vCenter, 
        float               fHeight,
        float               fRadius,
        unsigned int        colCenter,
        unsigned int        colOuter,
        YeAxis3DType        axisPlane = YE_AXIS_3D_PLANE_XOZ
        ) = 0;

    // ��Բ��
    virtual void            DrawEllipseRound(
        const FmVec3&       vCenter, 
        float               fHeight,
        float               fRadiusW,
        float               fRadiusH,
        unsigned int        colCenter,
        unsigned int        colOuter,
        YeAxis3DType        axisPlane = YE_AXIS_3D_PLANE_XOZ
        ) = 0;

    // ׶��
    virtual void            DrawCone(
        const FmVec3&       vCenter, 
        float               fHeight,
        float               fRadius,
        unsigned int        colTop,
        unsigned int        colBottom,
        YeAxis3DType        axisPlane = YE_AXIS_3D_PLANE_XOZ
        ) = 0;

    // ������
    virtual void            DrawPyramid(
        const FmVec3&       vCenter, 
        float               fWidth,
        float               fLength,
        float               fHeight,
        unsigned int        colTop,
        unsigned int        colBottom,
        YeAxis3DType        axisPlane = YE_AXIS_3D_PLANE_XOZ
        ) = 0;

    // ��
    virtual void            DrawLoop(
        const FmVec3&       vPosition, 
        float               fHeight,
        float               fInRadius,
        float               fOutRadius,
        unsigned int        colIn,
        unsigned int        colOut,
        YeAxis3DType        axisPlane = YE_AXIS_3D_PLANE_XOZ
        ) = 0;

    // ��
    virtual void            DrawCylinder(
        const FmVec3&       vPosition, 
        float               fHeight,
        float               fTopRadius,
        float               fBottomRadius,
        unsigned int        colTop,
        unsigned int        colBottom,
        YeAxis3DType        axisPlane = YE_AXIS_3D_PLANE_XOZ
        ) = 0;

    // ������
    virtual void            DrawBox(
        const FmVec3&       vMin,
        const FmVec3&       vMax,
        unsigned int        color
        ) = 0;

    // ��Χǽ��
    virtual void            DrawBindingWall(
        const FmVec3&       vMin,
        const FmVec3&       vMax,
        float               fWallHeight,
        unsigned int        colTop,
        unsigned int        colBottom
        ) = 0;

    // ��
    virtual void            DrawSphere(
        const FmVec3&       vCenter,
        float               fRadius,
        unsigned int        color
        ) = 0;

    // ����
    virtual void            DrawHemisphere(
        const FmVec3&       vCenter,
        float               fRadius,
        unsigned int        color,
        YeAxis3DType        axisDir = YE_AXIS_3D_POSITIVE_Y
        ) = 0;

    // ������
    virtual void            DrawCapsule(
        const FmVec3&       vCenter,
        float               fRadius,
        float               fHeight,
        unsigned int        color,
        YeAxis3DType        axisPlane = YE_AXIS_3D_PLANE_XOZ
        ) = 0;

    /*************��ͼ��*************/

    // �ı���
    virtual void            DrawQuadWire(
        const FmVec3&       vCenter,
        float               fSize,
        unsigned int        color,
        YeAxis3DType        axisPlane = YE_AXIS_3D_PLANE_XOZ
        ) = 0;

    // ԲȦ
    virtual void            DrawCircleWire(
        const FmVec3&       vCenter,
        float               fRadius,
        unsigned int        color,
        YeAxis3DType        axisPlane = YE_AXIS_3D_PLANE_XOZ
        ) = 0;

    // ��ԲȦ
    virtual void            DrawEllipseWire(
        const FmVec3&       vCenter,
        float               fRadiusW,
        float               fRadiusH,
        unsigned int        color,
        YeAxis3DType        axisPlane = YE_AXIS_3D_PLANE_XOZ
        ) = 0;

    // ��Χ��
    virtual void            DrawBindingBox(
        const FmVec3&       vMin,
        const FmVec3&       vMax,
        unsigned int        color
        ) = 0;

    // ÿ�����������Ǳߵİ�Χ��
    virtual void            DrawSurroundBox(
        const FmVec3&       vMin,
        const FmVec3&       vMax,
        unsigned int        color1,
        unsigned int        color2
        ) = 0;

    // ��Χ��Ȧ(Բ)
    virtual void            DrawSurroundCircle(
        const FmVec3&       vCenter,
        float               fRadius,
        unsigned int        colorXOZ,
        unsigned int        colorYOZ,
        unsigned int        colorXOY
        ) = 0;

    // ��Χ��Ȧ(��Բ)
    virtual void            DrawSurroundEllipse(
        const FmVec3&       vMin,
        const FmVec3&       vMax,
        unsigned int        colorXOZ,
        unsigned int        colorYOZ,
        unsigned int        colorXOY
        ) = 0;

    /*************����ͼ��*************/

    // ������
    virtual void            DrawTetrahedron(
        const FmVec3*       pVertices,
        unsigned int        faceColor,
        unsigned int        wireColor
        ) = 0;

    // ������
    virtual void            DrawHexahedron(
        const FmVec3*       pVertices,
        unsigned int        faceColor,
        unsigned int        wireColor
        ) = 0;   

    // �ύ��Ⱦ
    virtual void            Flush() = 0;
};

// --------------------------------------------------------------------------------------

#endif