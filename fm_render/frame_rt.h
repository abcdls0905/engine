//--------------------------------------------------------------------
// 文件名:		frame.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年4月2日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _FRAME_RT_H
#define _FRAME_RT_H

#include "../visual/i_resource.h"

// 渲染帧对象

class Render;

/*
关于所有的状态
颜色绑定
color_rt 
color_tex 

深度绑定
depth_rt
depth_tex
depthstencil_tex

模板绑定
stencil_tex
depthstencil_tex


*/



class CFrameRT: public IFrameRT
{
public:

public:
	// 这里是以前的老的方式创建的
	CFrameRT(Render* pRender, IColorRT* pColorRT,IDepthRT* pDepthRT,IStencilTex* pStencilTex);

	// 这里是以前的老的方式创建的
	CFrameRT(Render* pRender, IColorRT* pColorRT,IDepthTex* pDepthTex,IStencilTex* pStencilTex);

    // 这里只要深度
	CFrameRT(Render* pRender, IColorTex* pColorRT,IDepthRT* pDepthRT,IStencilTex* pStencilTex);
    
	// 
	CFrameRT(Render* pRender, IColorTex* pColorRT,IDepthStencilTex* pDepthStencilTex);

    // 
	CFrameRT(Render* pRender, IColorRT* pColorRT,IDepthStencilTex* pDepthStencilTex);

	CFrameRT(Render* pRender, IColorRT* pColorRT,IDepthStencilRT* pDepthStencilRT);

	CFrameRT(Render* pRender, IColorRT* pColorRT);

	virtual ~CFrameRT();

	// 获取IColorRT
    virtual IColorRT* GetColorRT();

	// 获取IDepthRT
	virtual IDepthRT* GetDepthRT();

	// 获取绑定的IColorTex
	virtual IColorTex* GetColorTex();

	// 获取绑定的DepthTex
	virtual IDepthTex* GetDepthTex();

	// 获取绑定的模版
	virtual IStencilTex* GetStencilTex();

	// 获取绑定的深度模板类
	virtual IDepthStencilTex* GetDepthStencilTex();

	// 获取绑定的深度模板
	virtual IDepthStencilRT* GetDepthStencilRT();

	// 设置IColorRT
	virtual bool SetColorRT(IColorRT* pColorRT);

	// 设置IDepthRT
	virtual bool SetDepthRT(IDepthRT* pDepthRT);

	virtual bool SetColorTex(IColorTex* pColorTex);

	virtual bool SetDepthTex(IDepthTex* pDepthTex);

	virtual bool SetStencilTex(IStencilTex* pStencil);

	virtual bool SetDepthStencilTex(IDepthStencilTex* pDepthStencilTex);

	virtual bool SetDepthStencilRT(IDepthStencilRT* pDepthStencilRT);

	// 查询绑Frame当前的绑定的状态
	virtual IFrameRT::BIND_STATE GetColorState();
	virtual IFrameRT::BIND_STATE GetDepthState();
	virtual IFrameRT::BIND_STATE GetStencilState();

	// 是否绑定了DepthStencil
	virtual bool IsBindDepthStencil();

	// 释放
	virtual void Release();

	// 创建
	bool Create();

	// 使用当前FBO作为渲染对象
    void UsedFrameRT();

	// 设置资源索引
	void SetIndex(int value) { m_nIndex = value; }
	
private:
	CFrameRT();
	CFrameRT(const CFrameRT&);
	CFrameRT& operator=(const CFrameRT&);
	
	// 销毁
	virtual void Destroy();

	// 设置IColorRT
	bool inSetColorRT(IColorRT* pColorRT);

	// 设置IDepthRT
	bool inSetDepthRT(IDepthRT* pDepthRT);

	bool inSetColorTex(IColorTex* pColorTex);

	bool inSetDepthTex(IDepthTex* pDepthTex);

	bool inSetStencilTex(IStencilTex* pStencil);

	bool inSetDepthStencilTex(IDepthStencilTex* pDepthStencilTex);

	bool inSetDepthStencilRT(IDepthStencilRT* pDepthStencilRT);
private:
	Render* m_pRender;
	int m_nIndex;
	int m_nType;

	IFrameRT::BIND_STATE m_nBindColorState;
	IFrameRT::BIND_STATE m_nBindDepthState;
	IFrameRT::BIND_STATE m_nBindStencilState;

	bool   m_bIsBindDepthStencil;//是否绑定了深度模板格式

	//颜色
	union 
	{
		IColorRT* m_pColorRT;
		IColorTex* m_pColorTex;
	};

	//深度
	union
	{
		IDepthRT* m_pDepthRT;
		IDepthTex* m_pDepthTex;
	};

	//模板
	IStencilTex* m_pStencilTex;

	//深度模板
	union
	{
		IDepthStencilTex* m_pDepthStencilTex;
		IDepthStencilRT*  m_pDepthStencilRT;
	};
};

#endif // _DEPTH_RT_H

/*
CFrameRT 
是gles中的FBO的抽象


是一个渲染RT的管理容器

color_point
depth_point
stencil_point

*/