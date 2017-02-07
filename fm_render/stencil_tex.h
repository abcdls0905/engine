//--------------------------------------------------------------------
// �ļ���:		stencil_tex.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��11��6��
// ������:		�ſ�ʤ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _STENCIL_TEX_H
#define _STENCIL_TEX_H



#include "../visual/i_resource.h"


// ģ����ͼ
class Render;
class CStencilTex: public IStencilTex
{
public:
	CStencilTex(Render* pRender, unsigned int width, unsigned int height, TEX_FORMAT format,int type);

	virtual ~CStencilTex();

	// ���
	virtual unsigned int GetWidth() const { return m_nWidth; }
	// �߶�
	virtual unsigned int GetHeight() const { return m_nHeight; }
	// ��ʽ
	virtual TEX_FORMAT GetFormat() const { return m_nFormat; }

	// �ͷ�
	virtual void Release();

	// ����
	bool Create();

	// ������Դ����
	void SetIndex(int value) { m_nIndex = value; }

private:
	CStencilTex();
	CStencilTex(const CStencilTex&);
	CStencilTex& operator=(const CStencilTex&);
	
	// ����
	virtual void Destroy();
	// �豸����ʱ�Ĵ���
	virtual bool Restore();
	// �豸��ʧʱ�Ĵ���
	virtual bool Invalidate();


private:
	Render* m_pRender;
	TEX_FORMAT m_nFormat;
    unsigned int m_nWidth;
    unsigned int m_nHeight;
	int m_nType;
	bool m_bDeviceSize;
	double m_dWidthRatio;
	double m_dHeightRatio;
	int m_nIndex;
};


#endif