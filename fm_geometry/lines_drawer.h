//--------------------------------------------------------------------
// 文件名:		lines_drawer.h
// 内  容:		
// 说  明:		
// 创建日期:	2014年03月05日
// 创建人:		叶峰
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _lines_drawer_H
#define _lines_drawer_H

#include "../public/core_type.h"
#include "../visual/shader_list.h"
#include "../visual/i_lines_drawer.h"
#include "../visual/i_render.h"
#include "../visual/i_context.h"
#include "../visual/i_scene_view.h"

#define MAX_LINES_NUM 1024

class LinesDrawer : public ILinesDrawer
{
public:
	LinesDrawer();
	virtual ~LinesDrawer();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	// 绘制
	virtual void Realize();

    // 设置当前场景视图
    void SetSceneView(ISceneView* pSceneView);

    // 获得当前场景视图
    ISceneView* GetSceneView();

	// 是否可见
	void SetVisible(bool value);
	bool GetVisible() const;

	// 是否使用深度测试
	void SetZEnable(bool value);
	bool GetZEnable() const;

    void DrawLine(const LinePoint& ptStart, const LinePoint& ptEnd, int nFrames = 1);

    void DrawLines(const LinePoint* listPoints, unsigned int numLines, int nFrames = 1);

    void DrawLine(const FmVec3& ptStart, const FmVec3& ptEnd, unsigned int color, int nFrames = 1);

    void DrawLines(const FmVec3* listPoints, unsigned int numLines, unsigned int color, int nFrames = 1);

    // 渲染函数
    static void RenderLines(void* pdata);

    void Clear();

private:
    IRender*		m_pRender;
    IShaderProgram*	m_pShaderProgram;
    IDynamicVB*		m_pVB;
    ISceneView*     m_pSceneView;

    fm_int          m_hMatWVP;

    LinePoint       m_listLinePoints[MAX_LINES_NUM*2];
    int             m_listLineFrames[MAX_LINES_NUM];
    unsigned int    m_linesNum;

	bool m_bVisible;
	bool m_bZEnable;
};

#endif // _GEOMETRY_H
