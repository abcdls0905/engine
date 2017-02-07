//--------------------------------------------------------------------
// 文件名:		depthstencil_tex.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年11月6日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _DEPTHSTENCIL_TEX_H
#define _DEPTHSTENCIL_TEX_H


#include "../visual/i_resource.h"

// 深度模板渲染目标

class Render;

class CDepthStencilTex: public IDepthStencilTex
{
public:
	CDepthStencilTex(Render* pRender, unsigned int width, unsigned int height, 
		TEX_FORMAT format, int type);
	virtual ~CDepthStencilTex();

	// 释放
	virtual void Release();

	// 创建
	bool Create();

	// 宽度
	unsigned int GetWidth() const { return m_nWidth; }
	// 高度
	unsigned int GetHeight() const { return m_nHeight; }
	// 格式
	TEX_FORMAT GetFormat() const { return m_nFormat; }

	// 设置资源索引
	void SetIndex(int value) { m_nIndex = value; }
private:
	CDepthStencilTex();
	CDepthStencilTex(const CDepthStencilTex&);
	CDepthStencilTex& operator=(const CDepthStencilTex&);
	
	// 销毁
	virtual void Destroy();

private:
	Render* m_pRender;
	ITextureSampler* m_pTexSampler;
	int m_nIndex;
	int m_nType;
	bool m_bDeviceSize;
	double m_dWidthRatio;
	double m_dHeightRatio;
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	TEX_FORMAT m_nFormat;
};

#endif // _DEPTH_RT_H
