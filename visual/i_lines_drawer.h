
//--------------------------------------------------------------------
// �ļ���:		i_lines_drawer.h
// ��  ��:		���ߵ�ʵ��
// ˵  ��:		�����ڲ���ʱ,����ط���ӻ��ߵ��߼�
// ��������:	2014��03��05��
// ������:		Ҷ��
// ��Ȩ����:	������ţ�������޹�˾
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

    // ��ǰ������ͼ(Ĭ�ϳ�ʼ��ʱ����IRender�л�ȡ)
    virtual void SetSceneView(ISceneView* pSceneView) = 0;
    virtual ISceneView* GetSceneView() = 0;

	// �Ƿ�ʹ����Ȳ���
	virtual void SetZEnable(bool value) = 0;
	virtual bool GetZEnable() const = 0;

	// �Ƿ�ɼ�
	virtual void SetVisible(bool value) = 0;
	virtual bool GetVisible() const = 0;

    virtual void DrawLine(const LinePoint& ptStart, const LinePoint& ptEnd, int nFrames = 1) = 0;

    virtual void DrawLines(const LinePoint* listPoints, unsigned int numLines, int nFrames = 1) = 0;

    virtual void DrawLine(const FmVec3& ptStart, const FmVec3& ptEnd, unsigned int color, int nFrames = 1) = 0;

    virtual void DrawLines(const FmVec3* listPoints, unsigned int numLines, unsigned int color, int nFrames = 1) = 0;
};

// --------------------------------------------------------------------------------------

#endif