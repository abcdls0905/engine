//--------------------------------------------------------------------
// �ļ���:		ImageData.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��14��	
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _IMAGEDATA_H
#define _IMAGEDATA_H

#include "../public/macros.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"

// ����ͼԪ����

class CImageFlip
{
public:
	struct sub_image_t
	{
		sub_image_t(){}
		float fSeconds;
		int nLeft;
		int nTop;
		int nRight;
		int nBottom;
	};
	
	typedef TArrayPod<sub_image_t, 1, TCoreAlloc> SUB_CONTAINER;

public:
	SUB_CONTAINER m_SubImages;
	float m_fCounter;
	int m_nCurrent;
};

// ����ͼԪ����

class CImageData
{
public:
	CImageData();
	~CImageData();

	// �Ƿ����ͼԪ����
	bool IsFlip() const { return (m_pFlip != NULL); }

	// ����
	void Copy(const CImageData& src);
	// ������������
	void CreateFlip();
	// ɾ����������
	void DeleteFlip();
	// ��λ����
	void ResetFlip();
	// ���Ŷ���
	void ExecuteFlip(float seconds);
	// ��õ�ǰͼԪλ��
	void GetCurrent(int& left, int& top, int& right, int& bottom);

private:
	CImageData(const CImageData&);
	CImageData& operator=(const CImageData&);

public:
	core_string m_strTex;
	int m_nLeft;
	int m_nTop;
	int m_nRight;
	int m_nBottom;
	CImageFlip* m_pFlip;
};

#endif // _IMAGEDATA_H

