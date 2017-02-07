
//--------------------------------------------------------------------
// 文件名:		i_geometry.h
// 内  容:		
// 说  明:		对一些简单图形的渲染实体接口
// 创建日期:	2013年11月25日
// 创建人:		叶峰
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _I_GEOMETRY_H
#define _I_GEOMETRY_H

// --------------------------------------------------------------------------------------

#include "i_vis_base.h"

// --------------------------------------------------------------------------------------

// 简单图形的类型
enum YeGraphType
{
	YE_GRAPH_INVALID        = 0,
	YE_GRAPH_PLANE          = 1,    // 平面
	YE_GRAPH_BOX            = 2,    // 立方体
	YE_GRAPH_SPHERE         = 3,    // 球
	YE_GRAPH_CONE           = 4,    // 圆锥
	YE_GRAPH_CYLINDER       = 5,    // 柱台
	YE_GRAPH_CAPSULE        = 6,    // 胶囊体
	YE_GRAPH_PYRAMID        = 7,    // 金字塔
	YE_GRAPH_ROUND          = 8,    // 圆面
	YE_GRAPH_RING           = 9,    // 圆环
	YE_GRAPH_PIPE           = 10,   // 圆管
	YE_GRAPH_WEDGE          = 11,   // 楔形体
	YE_GRAPH_FAN            = 12,   // 扇形体
	YE_GRAPH_ARC            = 13,   // 弧面
	YE_GRAPH_GEARWHEEL      = 14,   // 齿轮
	YE_GRAPH_STAR           = 15,   // 星星
	YE_GRAPH_SPIRE          = 16,   // 螺旋面
	YE_GRAPH_STAIRS         = 17,   // 楼梯
	YE_GRAPH_SPIRAL_STAIRS  = 18,   // 螺旋楼梯
	YE_GRAPH_HEMISPHERE     = 19,   // 半球形
	YE_GRAPH_DRUM           = 20,   // 鼓状物
};

// 模型的起始位置
enum YeOriginPose                         
{
	YE_ORIGIN_POSE_TOP      = 0,
	YE_ORIGIN_POSE_CENTER   = 1,
	YE_ORIGIN_POSE_BOTTOM   = 2
};

// --------------------------------------------------------------------------------------

// 图形描述,用于生成一个Geometry
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
	unsigned int        slices;         // 段，X-0-Z面上的分割
	unsigned int        stacks;         // 层，Y轴上的分割

	YeOriginPose        originPose;     // 原点位置

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
    不同的图形类型YeGraphType,使用不同的参数
    YE_GRAPH_PLANE          X方向宽:width          Z方向长:length          Y高度位置:posY
    YE_GRAPH_BOX            X轴范围:extentX        Y轴范围:extentY         Z轴范围:extentZ
    YE_GRAPH_SPHERE         半径:radius
    YE_GRAPH_DRUM           鼓面半径:radius        鼓体半径:assistRadius
    YE_GRAPH_HEMISPHERE     半径:radius
    YE_GRAPH_CONE           锥底半径:radius        高:height
    YE_GRAPH_CYLINDER       上面半径:radius        底面半径:assistRadius   高:height
    YE_GRAPH_CAPSULE        半径:radius            高:height
    YE_GRAPH_PYRAMID        X方向宽:width          Z方向长:length           高:height
    YE_GRAPH_ROUND          半径:radius            Y高度位置:posY
    YE_GRAPH_RING           外半径:radius          内半径:assistRadius
    YE_GRAPH_PIPE           外半径:radius          内半径:assistRadius
    YE_GRAPH_WEDGE          X方向宽:width          Z方向长:length           高:height
    YE_GRAPH_FAN            半径:radius            角度:degree              高:height
    YE_GRAPH_ARC            半径:radius            角度:degree              高:height
    YE_GRAPH_GEARWHEEL      外半径:radius          内半径:assistRadius      高:height
    YE_GRAPH_STAR           外半径:radius          内半径:assistRadius      高:height
    YE_GRAPH_SPIRE          外半径:radius          内半径:assistRadius      高:height
    YE_GRAPH_STAIRS         X方向宽:width          Z方向长:length           高:height
    YE_GRAPH_SPIRAL_STAIRS  外半径:radius          内半径:assistRadius      高:height
*/

// --------------------------------------------------------------------------------------

class ISceneView;

class IGeometry: public IVisBase
{
public:
    // 当前场景视图(默认初始化时，从IRender中获取)
    virtual void SetSceneView(ISceneView* pSceneView) = 0;
    virtual ISceneView* GetSceneView() = 0;

    // 渲染
    virtual void Draw(const FmMat4& mtxWorld) = 0;

	// 位置
	virtual bool SetPosition(float x, float y, float z) = 0;
	virtual FmVec3 GetPosition() const = 0;

	// 方向
	virtual bool SetAngle(float x, float y, float z) = 0;
	virtual FmVec3 GetAngle() const = 0;

	// 缩放
	virtual bool SetScale(float x, float y, float z) = 0;
	virtual FmVec3 GetScale() const = 0;

	// 变换矩阵
	virtual bool SetTransform(const FmMat4& mtx) = 0;
	virtual const FmMat4& GetTransform() const = 0;

	// 颜色
	virtual void SetColor(unsigned int value) = 0;
	virtual unsigned int GetColor() const = 0;

	// 是否可见(即设置每帧是否自动渲染，默认为false)
	virtual void SetVisible(bool value) = 0;
	virtual bool GetVisible() const = 0;

	// 是否画线模式
	virtual void SetLineMode(bool value) = 0;
	virtual bool GetLineMode() const = 0;

	// 是否使用深度测试
	virtual void SetZEnable(bool value) = 0;
	virtual bool GetZEnable() const = 0;

	// 创建
	virtual bool Build(const YsGraphDesc& graphDesc) = 0;
	virtual void Unbuild() = 0;

	// 是否有效
	virtual bool IsReady() const = 0;
};

// --------------------------------------------------------------------------------------

#endif