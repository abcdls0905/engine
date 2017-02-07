//--------------------------------------------------------------------
// �ļ���:		depthstencil_rt.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��11��27��
// ������:		�ſ�ʤ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _DEPTHSTENCIL_RT_H
#define _DEPTHSTENCIL_RT_H


#include "../visual/i_resource.h"

// ���ģ����ȾĿ��

class Render;

class CDepthStencilRT: public IDepthStencilRT
{
public:
	CDepthStencilRT(Render* pRender, unsigned int width, unsigned int height, 
		TEX_FORMAT format, int type);
	virtual ~CDepthStencilRT();

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

	// ��ȡ��ͼ����������
	ITextureSampler* GetTextureSampler() const 
	{
		return m_pTexSampler;
	}
	

	// ������Դ����
	void SetIndex(int value) { m_nIndex = value; }
private:
	CDepthStencilRT();
	CDepthStencilRT(const CDepthStencilRT&);
	CDepthStencilRT& operator=(const CDepthStencilRT&);
	
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
