//--------------------------------------------------------------------
// �ļ���:		image_access.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��4��3��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _IMAGE_ACCESS_H
#define _IMAGE_ACCESS_H

#include "../public/macros.h"
#include "../visual/i_image_access.h"

// ͼ���ļ���ȡ

class Render;

class CImageAccess:public IImageAccess
{
public:
	// ����
	static IImageAccess* NewInstance(Render* pRender);

public:
	CImageAccess(Render* pRender);
	~CImageAccess();

	// �ͷ�
	void Release();

	// ͼƬ���
	unsigned int GetWidth();
	// ͼƬ�߶�
	unsigned int GetHeight();
	// ÿ�����ص�λ��
	unsigned int GetBitPerPixel();
	// ͼƬ����
	unsigned char* GetData();

	// ���ø�ʽ
	bool SetFormat(unsigned int width, unsigned int height, 
		unsigned int bit_per_pixel);
	// ����ͼƬ����
	bool SetData(const void* pdata, size_t size);

	// ת����32λARGB��ʽ
	bool ConvertToARGB();
	// ת����24λRGB��ʽ
	bool ConvertToRGB();
	// ת����8λ�Ҷȸ�ʽ
	bool ConvertToGray8();
	
	// ���ļ�����
	bool LoadFile(const char* file_name);
	// ���浽�ļ�
	bool SaveFile(const char* file_name);
	
private:
	CImageAccess(const CImageAccess&);
	CImageAccess& operator=(const CImageAccess&);

	// �ͷ���Դ
	void ReleaseAll();

	// ����ͼ���ļ�
	bool LoadTga(const char* file_name);
	bool LoadBmp(const char* file_name);
	bool LoadDDS(const char* file_name);
	// ����ͼ���ļ�
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
