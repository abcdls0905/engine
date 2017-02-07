//--------------------------------------------------------------------
// 文件名:		color_tex.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年11月6日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _COLOR_TEX_H
#define _COLOR_TEX_H


#include "../visual/i_resource.h"


// 模板贴图
class Render;

class CColorTex:public IColorTex
{
public:
	CColorTex(Render* pRender, unsigned int width, unsigned int height, TEX_FORMAT format, int type);
	virtual ~CColorTex();

	// 释放
	virtual void Release();

	// 创建
	bool Create();

	// 宽度
	virtual unsigned int GetWidth() const { return m_nWidth; }
	// 高度
	virtual unsigned int GetHeight() const { return m_nHeight; }
	// 格式
	virtual TEX_FORMAT GetFormat() const { return m_nFormat; }

	// 设置资源索引
	void SetIndex(int value) { m_nIndex = value; }

private:
	CColorTex();
	CColorTex(const CColorTex&);
	CColorTex& operator=(const CColorTex&);
	
	// 销毁
	virtual void Destroy();

private:
	Render* m_pRender;
	int m_nIndex;
	int m_nType;
	bool m_bDeviceSize;
	double m_dWidthRatio;
	double m_dHeightRatio;
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	TEX_FORMAT m_nFormat;
	bool m_bAutoGenMipmap;
};


#endif