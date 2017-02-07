//--------------------------------------------------------------------
// 文件名:		stencil_tex.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年11月6日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _STENCIL_TEX_H
#define _STENCIL_TEX_H



#include "../visual/i_resource.h"


// 模板贴图
class Render;
class CStencilTex: public IStencilTex
{
public:
	CStencilTex(Render* pRender, unsigned int width, unsigned int height, TEX_FORMAT format,int type);

	virtual ~CStencilTex();

	// 宽度
	virtual unsigned int GetWidth() const { return m_nWidth; }
	// 高度
	virtual unsigned int GetHeight() const { return m_nHeight; }
	// 格式
	virtual TEX_FORMAT GetFormat() const { return m_nFormat; }

	// 释放
	virtual void Release();

	// 创建
	bool Create();

	// 设置资源索引
	void SetIndex(int value) { m_nIndex = value; }

private:
	CStencilTex();
	CStencilTex(const CStencilTex&);
	CStencilTex& operator=(const CStencilTex&);
	
	// 销毁
	virtual void Destroy();
	// 设备就绪时的处理
	virtual bool Restore();
	// 设备丢失时的处理
	virtual bool Invalidate();


private:
	Render* m_pRender;
	TEX_FORMAT m_nFormat;
    unsigned int m_nWidth;
    unsigned int m_nHeight;
	int m_nType;
	bool m_bDeviceSize;
	double m_dWidthRatio;
	double m_dHeightRatio;
	int m_nIndex;
};


#endif