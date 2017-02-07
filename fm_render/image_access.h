//--------------------------------------------------------------------
// 文件名:		image_access.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年4月3日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _IMAGE_ACCESS_H
#define _IMAGE_ACCESS_H

#include "../public/macros.h"
#include "../visual/i_image_access.h"

// 图像文件存取

class Render;

class CImageAccess:public IImageAccess
{
public:
	// 创建
	static IImageAccess* NewInstance(Render* pRender);

public:
	CImageAccess(Render* pRender);
	~CImageAccess();

	// 释放
	void Release();

	// 图片宽度
	unsigned int GetWidth();
	// 图片高度
	unsigned int GetHeight();
	// 每个像素的位数
	unsigned int GetBitPerPixel();
	// 图片数据
	unsigned char* GetData();

	// 设置格式
	bool SetFormat(unsigned int width, unsigned int height, 
		unsigned int bit_per_pixel);
	// 设置图片数据
	bool SetData(const void* pdata, size_t size);

	// 转换成32位ARGB格式
	bool ConvertToARGB();
	// 转换成24位RGB格式
	bool ConvertToRGB();
	// 转换成8位灰度格式
	bool ConvertToGray8();
	
	// 从文件加载
	bool LoadFile(const char* file_name);
	// 保存到文件
	bool SaveFile(const char* file_name);
	
private:
	CImageAccess(const CImageAccess&);
	CImageAccess& operator=(const CImageAccess&);

	// 释放资源
	void ReleaseAll();

	// 加载图像文件
	bool LoadTga(const char* file_name);
	bool LoadBmp(const char* file_name);
	bool LoadDDS(const char* file_name);
	// 保存图像文件
	bool SaveTga(const char* file_name);
	bool SaveBmp(const char* file_name);

private:
	Render* m_pRender;
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	unsigned int m_nBitPerPixel;
	unsigned char* m_pData;
	unsigned int m_nSize;
};

#endif // _IMAGE_ACCESS_H
