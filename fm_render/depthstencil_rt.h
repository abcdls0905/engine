//--------------------------------------------------------------------
// 文件名:		depthstencil_rt.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年11月27日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _DEPTHSTENCIL_RT_H
#define _DEPTHSTENCIL_RT_H


#include "../visual/i_resource.h"

// 深度模板渲染目标

class Render;

class CDepthStencilRT: public IDepthStencilRT
{
public:
	CDepthStencilRT(Render* pRender, unsigned int width, unsigned int height, 
		TEX_FORMAT format, int type);
	virtual ~CDepthStencilRT();

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

	// 获取贴图采样器操作
	ITextureSampler* GetTextureSampler() const 
	{
		return m_pTexSampler;
	}
	

	// 设置资源索引
	void SetIndex(int value) { m_nIndex = value; }
private:
	CDepthStencilRT();
	CDepthStencilRT(const CDepthStencilRT&);
	CDepthStencilRT& operator=(const CDepthStencilRT&);
	
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
