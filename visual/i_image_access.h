#ifndef _I_IMAGE_ACCESS_H_
#define _I_IMAGE_ACCESS_H_

#include "../public/macros.h"


class IImageAccess
{
public:
	// �ͷ�
	virtual void Release() = 0;

	// ͼƬ���
	virtual unsigned int GetWidth() = 0;
	// ͼƬ�߶�
	virtual unsigned int GetHeight() = 0;
	// ÿ�����ص�λ��
	virtual unsigned int GetBitPerPixel() = 0;
	// ͼƬ����
	virtual unsigned char* GetData() = 0;

	// ���ø�ʽ
	virtual bool SetFormat(unsigned int width, unsigned int height, 
		unsigned int bit_per_pixel) = 0;
	// ����ͼƬ����
	virtual bool SetData(const void* pdata, size_t size) = 0;

	// ת����32λARGB��ʽ
	virtual bool ConvertToARGB() = 0;
	// ת����24λRGB��ʽ
	virtual bool ConvertToRGB() = 0;
	// ת����8λ�Ҷȸ�ʽ
	virtual bool ConvertToGray8() = 0;
	
	// ���ļ�����
	virtual bool LoadFile(const char* file_name) = 0;
	// ���浽�ļ�
	virtual bool SaveFile(const char* file_name) = 0;	
};

#endif