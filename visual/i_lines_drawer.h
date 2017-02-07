
//--------------------------------------------------------------------
// 文件名:		i_lines_drawer.h
// 内  容:		画线的实体
// 说  明:		用于在测试时,任意地方添加画线的逻辑
// 创建日期:	2014年03月05日
// 创建人:		叶峰
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _i_lines_drawer_H
#define _i_lines_drawer_H

// --------------------------------------------------------------------------------------

#include "i_vis_base.h"

// --------------------------------------------------------------------------------------

class ISceneView;

class ILinesDrawer: public IVisBase
{
public:
    struct LinePoint
    {
        FmVec3 vPos;
        unsigned int diffuse;
    };

    // 当前场景视图(默认初始化时，从IRender中获取)
    virtual void SetSceneView(ISceneView* pSceneView) = 0;
    virtual ISceneView* GetSceneView() = 0;

	// 是否使用深度测试
	virtual void SetZEnable(bool value) = 0;
	virtual bool GetZEnable() const = 0;

	// 是否可见
	virtual void SetVisible(bool value) = 0;
	virtual bool GetVisible() const = 0;

    virtual void DrawLine(const LinePoint& ptStart, const LinePoint& ptEnd, int nFrames = 1) = 0;

    virtual void DrawLines(const LinePoint* listPoints, unsigned int numLines, int nFrames = 1) = 0;

    virtual void DrawLine(const FmVec3& ptStart, const FmVec3& ptEnd, unsigned int color, int nFrames = 1) = 0;

    virtual void DrawLines(const FmVec3* listPoints, unsigned int numLines, unsigned int color, int nFrames = 1) = 0;
};

// --------------------------------------------------------------------------------------

#endif