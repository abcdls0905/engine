//--------------------------------------------------------------------
// �ļ���:		color_tex.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��11��6��
// ������:		�ſ�ʤ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _COLOR_TEX_H
#define _COLOR_TEX_H


#include "../visual/i_resource.h"


// ģ����ͼ
class Render;

class CColorTex:public IColorTex
{
public:
	CColorTex(Render* pRender, unsigned int width, unsigned int height, TEX_FORMAT format, int type);
	virtual ~CColorTex();

	// �ͷ�
	virtual void Release();

	// ����
	bool Create();

	// ���
	virtual unsigned int GetWidth() const { return m_nWidth; }
	// �߶�
	virtual unsigned int GetHeight() const { return m_nHeight; }
	// ��ʽ
	virtual TEX_FORMAT GetFormat() const { return m_nFormat; }

	// ������Դ����
	void SetIndex(int value) { m_nIndex = value; }

private:
	CColorTex();
	CColorTex(const CColorTex&);
	CColorTex& operator=(const CColorTex&);
	
	// ����
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