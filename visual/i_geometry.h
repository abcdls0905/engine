
//--------------------------------------------------------------------
// �ļ���:		i_geometry.h
// ��  ��:		
// ˵  ��:		��һЩ��ͼ�ε���Ⱦʵ��ӿ�
// ��������:	2013��11��25��
// ������:		Ҷ��
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _I_GEOMETRY_H
#define _I_GEOMETRY_H

// --------------------------------------------------------------------------------------

#include "i_vis_base.h"

// --------------------------------------------------------------------------------------

// ��ͼ�ε�����
enum YeGraphType
{
	YE_GRAPH_INVALID        = 0,
	YE_GRAPH_PLANE          = 1,    // ƽ��
	YE_GRAPH_BOX            = 2,    // ������
	YE_GRAPH_SPHERE         = 3,    // ��
	YE_GRAPH_CONE           = 4,    // Բ׶
	YE_GRAPH_CYLINDER       = 5,    // ��̨
	YE_GRAPH_CAPSULE        = 6,    // ������
	YE_GRAPH_PYRAMID        = 7,    // ������
	YE_GRAPH_ROUND          = 8,    // Բ��
	YE_GRAPH_RING           = 9,    // Բ��
	YE_GRAPH_PIPE           = 10,   // Բ��
	YE_GRAPH_WEDGE          = 11,   // Ш����
	YE_GRAPH_FAN            = 12,   // ������
	YE_GRAPH_ARC            = 13,   // ����
	YE_GRAPH_GEARWHEEL      = 14,   // ����
	YE_GRAPH_STAR           = 15,   // ����
	YE_GRAPH_SPIRE          = 16,   // ������
	YE_GRAPH_STAIRS         = 17,   // ¥��
	YE_GRAPH_SPIRAL_STAIRS  = 18,   // ����¥��
	YE_GRAPH_HEMISPHERE     = 19,   // ������
	YE_GRAPH_DRUM           = 20,   // ��״��
};

// ģ�͵���ʼλ��
enum YeOriginPose                         
{
	YE_ORIGIN_POSE_TOP      = 0,
	YE_ORIGIN_POSE_CENTER   = 1,
	YE_ORIGIN_POSE_BOTTOM   = 2
};

// --------------------------------------------------------------------------------------

// ͼ������,��������һ��Geometry
struct YsGraphDesc
{
	YeGraphType         graphType;
	union              
	{                  
		float           radius;
		float           extentX;
		float           width;
	};                 
	union              
	{                  
		float           assistRadius;
		float           extentZ;
		float           degree;
		float           length;
		float           depth;
	};                 
	union              
	{                  
		float           height;
		float           posY;
		float           extentY;
	};                 
	unsigned int        slices;         // �Σ�X-0-Z���ϵķָ�
	unsigned int        stacks;         // �㣬Y���ϵķָ�

	YeOriginPose        originPose;     // ԭ��λ��

	YsGraphDesc()      
	{                  
		graphType       = YE_GRAPH_INVALID;
		slices          = 16;
		stacks          = 16;
		originPose      = YE_ORIGIN_POSE_CENTER;
		extentX         = 1.0f;
		extentZ         = 1.0f;
		extentY         = 1.0f;
	}
};

/*
    ��ͬ��ͼ������YeGraphType,ʹ�ò�ͬ�Ĳ���
    YE_GRAPH_PLANE          X�����:width          Z����:length          Y�߶�λ��:posY
    YE_GRAPH_BOX            X�᷶Χ:extentX        Y�᷶Χ:extentY         Z�᷶Χ:extentZ
    YE_GRAPH_SPHERE         �뾶:radius
    YE_GRAPH_DRUM           ����뾶:radius        ����뾶:assistRadius
    YE_GRAPH_HEMISPHERE     �뾶:radius
    YE_GRAPH_CONE           ׶�װ뾶:radius        ��:height
    YE_GRAPH_CYLINDER       ����뾶:radius        ����뾶:assistRadius   ��:height
    YE_GRAPH_CAPSULE        �뾶:radius            ��:height
    YE_GRAPH_PYRAMID        X�����:width          Z����:length           ��:height
    YE_GRAPH_ROUND          �뾶:radius            Y�߶�λ��:posY
    YE_GRAPH_RING           ��뾶:radius          �ڰ뾶:assistRadius
    YE_GRAPH_PIPE           ��뾶:radius          �ڰ뾶:assistRadius
    YE_GRAPH_WEDGE          X�����:width          Z����:length           ��:height
    YE_GRAPH_FAN            �뾶:radius            �Ƕ�:degree              ��:height
    YE_GRAPH_ARC            �뾶:radius            �Ƕ�:degree              ��:height
    YE_GRAPH_GEARWHEEL      ��뾶:radius          �ڰ뾶:assistRadius      ��:height
    YE_GRAPH_STAR           ��뾶:radius          �ڰ뾶:assistRadius      ��:height
    YE_GRAPH_SPIRE          ��뾶:radius          �ڰ뾶:assistRadius      ��:height
    YE_GRAPH_STAIRS         X�����:width          Z����:length           ��:height
    YE_GRAPH_SPIRAL_STAIRS  ��뾶:radius          �ڰ뾶:assistRadius      ��:height
*/

// --------------------------------------------------------------------------------------

class ISceneView;

class IGeometry: public IVisBase
{
public:
    // ��ǰ������ͼ(Ĭ�ϳ�ʼ��ʱ����IRender�л�ȡ)
    virtual void SetSceneView(ISceneView* pSceneView) = 0;
    virtual ISceneView* GetSceneView() = 0;

    // ��Ⱦ
    virtual void Draw(const FmMat4& mtxWorld) = 0;

	// λ��
	virtual bool SetPosition(float x, float y, float z) = 0;
	virtual FmVec3 GetPosition() const = 0;

	// ����
	virtual bool SetAngle(float x, float y, float z) = 0;
	virtual FmVec3 GetAngle() const = 0;

	// ����
	virtual bool SetScale(float x, float y, float z) = 0;
	virtual FmVec3 GetScale() const = 0;

	// �任����
	virtual bool SetTransform(const FmMat4& mtx) = 0;
	virtual const FmMat4& GetTransform() const = 0;

	// ��ɫ
	virtual void SetColor(unsigned int value) = 0;
	virtual unsigned int GetColor() const = 0;

	// �Ƿ�ɼ�(������ÿ֡�Ƿ��Զ���Ⱦ��Ĭ��Ϊfalse)
	virtual void SetVisible(bool value) = 0;
	virtual bool GetVisible() const = 0;

	// �Ƿ���ģʽ
	virtual void SetLineMode(bool value) = 0;
	virtual bool GetLineMode() const = 0;

	// �Ƿ�ʹ����Ȳ���
	virtual void SetZEnable(bool value) = 0;
	virtual bool GetZEnable() const = 0;

	// ����
	virtual bool Build(const YsGraphDesc& graphDesc) = 0;
	virtual void Unbuild() = 0;

	// �Ƿ���Ч
	virtual bool IsReady() const = 0;
};

// --------------------------------------------------------------------------------------

#endif