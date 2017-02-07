//--------------------------------------------------------------------
// �ļ���:		depth_rt.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��6��24��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _DEPTH_RT_H
#define _DEPTH_RT_H


#include "../visual/i_resource.h"

// �����ȾĿ��

class Render;

class CDepthRT: public IDepthRT
{
public:
	CDepthRT(Render* pRender, unsigned int width, unsigned int height, 
		TEX_FORMAT format, int type);
	virtual ~CDepthRT();

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

	// ��ȡ��ͼ����������
	ITextureSampler* GetTextureSampler() const 
	{
		return m_pTexSampler;
	}
	
private:
	CDepthRT();
	CDepthRT(const CDepthRT&);
	CDepthRT& operator=(const CDepthRT&);
	
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
