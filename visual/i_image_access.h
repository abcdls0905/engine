#ifndef _I_IMAGE_ACCESS_H_
#define _I_IMAGE_ACCESS_H_

#include "../public/macros.h"


class IImageAccess
{
public:
	// 释放
	virtual void Release() = 0;

	// 图片宽度
	virtual unsigned int GetWidth() = 0;
	// 图片高度
	virtual unsigned int GetHeight() = 0;
	// 每个像素的位数
	virtual unsigned int GetBitPerPixel() = 0;
	// 图片数据
	virtual unsigned char* GetData() = 0;

	// 设置格式
	virtual bool SetFormat(unsigned int width, unsigned int height, 
		unsigned int bit_per_pixel) = 0;
	// 设置图片数据
	virtual bool SetData(const void* pdata, size_t size) = 0;

	// 转换成32位ARGB格式
	virtual bool ConvertToARGB() = 0;
	// 转换成24位RGB格式
	virtual bool ConvertToRGB() = 0;
	// 转换成8位灰度格式
	virtual bool ConvertToGray8() = 0;
	
	// 从文件加载
	virtual bool LoadFile(const char* file_name) = 0;
	// 保存到文件
	virtual bool SaveFile(const char* file_name) = 0;	
};

#endif