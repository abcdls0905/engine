//--------------------------------------------------------------------
// 文件名:		Pyramid.h
// 内  容:		
// 说  明:		
// 创建日期:	2012年8月6日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#ifndef __PYRAMID_H__
#define __PYRAMID_H__

#include "../visual/i_vis_base.h"
#include "../visual/i_render.h"
#include <vector>

class ICamera;

// 视锥体

// 共11条线，分别是视锥的侧面的4条线、目标所在平面的4条线和指向目标的线,两条目标点的交叉线
#define PYRAMID_LINE_COUNT 11

class Pyramid
{
private:
	struct vertex_t 
	{
		FmVec3 pos;
		unsigned int color;
	};

public:
	Pyramid();
	virtual ~Pyramid() {}

	virtual bool Update();
	virtual void Realize(IRender* pRender);

	// 摄像机
	void SetCamera(const PERSISTID& value);
	PERSISTID GetCamera() const;

	// 是否显示视锥
	void SetShowPyramid(bool value) { m_bShowPyramid = value; }
	bool GetShowPyramid() const { return m_bShowPyramid; }

	// y方向视野角度
	void SetFovy(float value);
	float GetFovy() const { return m_fFovy; }
	
	// 视口宽高比
	void SetAspect(float value);
	float GetAspect() const { return m_fAspect; }

	/*设置焦距,在此只是改变绘制线框的长度。
	只对自由摄像机有效，目标摄像机的线框长度由跟目标的距离决定。*/
	void SetFocusDepth(float value);
	float GetFocusDepth() const {return m_fDepth;}

private:
	ICamera* GetCamera(const PERSISTID& id) const;
	// 更新顶点
	bool UpdateVertex();

private:
	PERSISTID	 m_camera;

	vertex_t	 m_vertexBuffer[PYRAMID_LINE_COUNT * 2];
	bool		 m_bShowPyramid;
	unsigned int m_uLineColor;

	float m_fFovy;
	float m_fAspect;
	float m_fDepth;
	FmMat4 m_mtxWorld;
};

#endif // __PYRAMID_H__
