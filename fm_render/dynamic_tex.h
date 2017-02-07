//--------------------------------------------------------------------
// 文件名:		dynamic_tex.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年8月11日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _DYNAMIC_TEX_H
#define _DYNAMIC_TEX_H



#include "../visual/i_resource.h"
#include "../public/core_mem.h"
// 动态变化的贴图
#include "render.h"
class Render;

class CDynamicTex: public IDynamicTex
{
	int m_PixelFormat;
	int m_InternalPixelFormat;
	int m_size;
	GLenum m_BufferType;
#ifdef PROCESS_LOST_DEVICE
	void* m_pdata;
	void Blit(void* dst_data, int dx, int dy, const void* src_data, int src_width, int src_height);
#endif
public:
	CDynamicTex(Render* pRender);
	virtual ~CDynamicTex();

	// 释放
	virtual void Release();

	// 创建
	bool Create(unsigned int width, unsigned int height, unsigned int levels, 
		TEX_FORMAT format);

	// 宽度
	unsigned int GetWidth() const { return m_nWidth; }
	// 高度
	unsigned int GetHeight() const { return m_nHeight; }
	// 格式
	TEX_FORMAT GetFormat() const { return m_nFormat; }

	// 设置资源索引
	void SetIndex(int value) { m_nIndex = value; }

		// 获取贴图采样器操作
	ITextureSampler* GetTextureSampler() const 
	{
		return m_pTexSampler;
	}

	bool Update(int level, unsigned int offset_x, unsigned int offset_y, unsigned int width, 
		unsigned int height, const void* pdata);
private:
	CDynamicTex();
	CDynamicTex(const CDynamicTex&);
	CDynamicTex& operator=(const CDynamicTex&);
	
	// 销毁
	virtual void Destroy();

#ifdef PROCESS_LOST_DEVICE
	// 设备就绪时的处理
	virtual bool Restore();
	// 设备丢失时的处理
	virtual bool Invalidate();
#endif

private:
	Render* m_pRender;
	ITextureSampler* m_pTexSampler;
	int m_nIndex;
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	TEX_FORMAT m_nFormat;
};

#endif // _DYNAMIC_TEX_H
