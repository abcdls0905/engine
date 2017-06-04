
#ifndef _STATIC_TEX_H
#define _STATIC_TEX_H



#include "../visual/i_resource.h"


// 静态贴图

class Render;
class CTexture;
class CStaticTex: public IStaticTex
{
	void* m_pdata;
	int m_data_size;
	CTexture* m_pTexBuilder;
	void Build(unsigned int width, unsigned int height, 
	unsigned int levels, TEX_FORMAT format, const void* pdata,
	unsigned int data_size);
public:
	CStaticTex(Render* pRender);
	virtual ~CStaticTex();

	// 释放
	virtual void Release();

	// 从数据创建
	bool Create(unsigned int width, unsigned int height, 
		unsigned int levels, TEX_FORMAT format, const void* pdata, 
		unsigned int data_size);
	// 从贴图资源创建
	bool CreateFrom(fm_uint nTex);
	
	// 贴图宽度
	unsigned int GetWidth() const { return m_nWidth; }
	// 贴图高度
	unsigned int GetHeight() const { return m_nHeight; }
	// 贴图层数
	unsigned int GetLevels() const { return m_nLevels; }
	// 贴图格式
	TEX_FORMAT GetFormat() const { return m_nFormat; }
	
	// 设置资源索引
	void SetIndex(int value) { m_nIndex = value; }

	// 获取贴图采样器操作
	ITextureSampler* GetTextureSampler() const 
	{
		return m_pTexSampler;
	}

	void SetTexBuilder(CTexture* pTexBuilder)
	{
		m_pTexBuilder = pTexBuilder;
	}
private:
	CStaticTex();
	CStaticTex(const CStaticTex&);
	CStaticTex& operator=(const CStaticTex&);
	
	virtual void Destroy();
	
	virtual bool Restore();

	virtual bool Invalidate();

private:
	Render* m_pRender;
	ITextureSampler* m_pTexSampler;
	int m_nIndex;
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	unsigned int m_nLevels;
	TEX_FORMAT m_nFormat;
};

#endif // _STATIC_TEX_H
