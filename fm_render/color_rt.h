//--------------------------------------------------------------------
// 文件名:		color_rt.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年6月24日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _COLOR_RT_H
#define _COLOR_RT_H

#include "../visual/i_resource.h"

// 颜色渲染目标

class Render;

class CColorRT: public IColorRT
{
public:
	CColorRT(Render* pRender, unsigned int width, unsigned int height, 
		TEX_FORMAT format, bool auto_gen_mipmap, int type);
	virtual ~CColorRT();

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
	// 是否自动生成层级
	virtual bool GetAutoGenMipmap() const { return m_bAutoGenMipmap; }

	// 设置资源索引
	void SetIndex(int value) { m_nIndex = value; }

	// 获取贴图采样器操作
	ITextureSampler* GetTextureSampler() const 
	{
		return m_pTexSampler;
	}


private:
	CColorRT();
	CColorRT(const CColorRT&);
	CColorRT& operator=(const CColorRT&);
	
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
	bool m_bAutoGenMipmap;
};

#endif // _COLOR_RT_H
