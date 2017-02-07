//--------------------------------------------------------------------
// �ļ���:		lines_drawer.h
// ��  ��:		
// ˵  ��:		
// ��������:	2014��03��05��
// ������:		Ҷ��
// ��Ȩ����:	������ţ�������޹�˾
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
	
	// ����
	virtual void Realize();

    // ���õ�ǰ������ͼ
    void SetSceneView(ISceneView* pSceneView);

    // ��õ�ǰ������ͼ
    ISceneView* GetSceneView();

	// �Ƿ�ɼ�
	void SetVisible(bool value);
	bool GetVisible() const;

	// �Ƿ�ʹ����Ȳ���
	void SetZEnable(bool value);
	bool GetZEnable() const;

    void DrawLine(const LinePoint& ptStart, const LinePoint& ptEnd, int nFrames = 1);

    void DrawLines(const LinePoint* listPoints, unsigned int numLines, int nFrames = 1);

    void DrawLine(const FmVec3& ptStart, const FmVec3& ptEnd, unsigned int color, int nFrames = 1);

    void DrawLines(const FmVec3* listPoints, unsigned int numLines, unsigned int color, int nFrames = 1);

    // ��Ⱦ����
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
