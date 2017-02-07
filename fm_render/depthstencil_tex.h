//--------------------------------------------------------------------
// �ļ���:		depthstencil_tex.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��11��6��
// ������:		�ſ�ʤ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _DEPTHSTENCIL_TEX_H
#define _DEPTHSTENCIL_TEX_H


#include "../visual/i_resource.h"

// ���ģ����ȾĿ��

class Render;

class CDepthStencilTex: public IDepthStencilTex
{
public:
	CDepthStencilTex(Render* pRender, unsigned int width, unsigned int height, 
		TEX_FORMAT format, int type);
	virtual ~CDepthStencilTex();

	// �ͷ�
	virtual void Release();

	// ����
	bool Create();

	// ���
	unsigned int GetWidth() const { return m_nWidth; }
	// �߶�
	unsigned int GetHeight() const { return m_nHeight; }
	// ��ʽ
	TEX_FORMAT GetFormat() const { return m_nFormat; }

	// ������Դ����
	void SetIndex(int value) { m_nIndex = value; }
private:
	CDepthStencilTex();
	CDepthStencilTex(const CDepthStencilTex&);
	CDepthStencilTex& operator=(const CDepthStencilTex&);
	
	// ����
	virtual void Destroy();

private:
	Render* m_pRender;
	ITextureSampler* m_pTexSampler;
	int m_nIndex;
	int m_nType;
	bool m_bDeviceSize;
	double m_dWidthRatio;
	double m_dHeightRatio;
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	TEX_FORMAT m_nFormat;
};

#endif // _DEPTH_RT_H
