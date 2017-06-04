
#ifndef _STATIC_CUBE_H
#define _STATIC_CUBE_H



#include "../visual/i_resource.h"

// 静态立方体贴图

class Render;

class CStaticCube: public IStaticCube
{
public:
	CStaticCube(Render* pRender);
	virtual ~CStaticCube();

	// 释放
	virtual void Release();

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


private:
	CStaticCube();
	CStaticCube(const CStaticCube&);
	CStaticCube& operator=(const CStaticCube&);
	
	// 销毁
	virtual void Destroy();

private:
	Render* m_pRender;
	ITextureSampler* m_pTexSampler;
	int m_nIndex;
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	unsigned int m_nLevels;
	TEX_FORMAT m_nFormat;
};

#endif // _STATIC_CUBE_H
