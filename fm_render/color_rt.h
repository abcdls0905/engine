//--------------------------------------------------------------------
// �ļ���:		color_rt.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��6��24��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _COLOR_RT_H
#define _COLOR_RT_H

#include "../visual/i_resource.h"

// ��ɫ��ȾĿ��

class Render;

class CColorRT: public IColorRT
{
public:
	CColorRT(Render* pRender, unsigned int width, unsigned int height, 
		TEX_FORMAT format, bool auto_gen_mipmap, int type);
	virtual ~CColorRT();

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
	// �Ƿ��Զ����ɲ㼶
	virtual bool GetAutoGenMipmap() const { return m_bAutoGenMipmap; }

	// ������Դ����
	void SetIndex(int value) { m_nIndex = value; }

	// ��ȡ��ͼ����������
	ITextureSampler* GetTextureSampler() const 
	{
		return m_pTexSampler;
	}


private:
	CColorRT();
	CColorRT(const CColorRT&);
	CColorRT& operator=(const CColorRT&);
	
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
	bool m_bAutoGenMipmap;
};

#endif // _COLOR_RT_H
