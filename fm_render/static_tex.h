//--------------------------------------------------------------------
// �ļ���:		static_tex.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��12��10��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _STATIC_TEX_H
#define _STATIC_TEX_H



#include "../visual/i_resource.h"


// ��̬��ͼ

class Render;
class CTexture;
class CStaticTex: public IStaticTex
{
	void* m_pdata;
	int m_data_size;
	CTexture* m_pTexBuilder;
	void Build(unsigned int width, unsigned int height, 
	unsigned int levels, TEX_FORMAT format, const void* pdata,
	unsigned int data_size);
public:
	CStaticTex(Render* pRender);
	virtual ~CStaticTex();

	// �ͷ�
	virtual void Release();

	// �����ݴ���
	bool Create(unsigned int width, unsigned int height, 
		unsigned int levels, TEX_FORMAT format, const void* pdata, 
		unsigned int data_size);
	// ����ͼ��Դ����
	bool CreateFrom(fm_uint nTex);
	
	// ��ͼ���
	unsigned int GetWidth() const { return m_nWidth; }
	// ��ͼ�߶�
	unsigned int GetHeight() const { return m_nHeight; }
	// ��ͼ����
	unsigned int GetLevels() const { return m_nLevels; }
	// ��ͼ��ʽ
	TEX_FORMAT GetFormat() const { return m_nFormat; }
	
	// ������Դ����
	void SetIndex(int value) { m_nIndex = value; }

	// ��ȡ��ͼ����������
	ITextureSampler* GetTextureSampler() const 
	{
		return m_pTexSampler;
	}

	void SetTexBuilder(CTexture* pTexBuilder)
	{
		m_pTexBuilder = pTexBuilder;
	}
private:
	CStaticTex();
	CStaticTex(const CStaticTex&);
	CStaticTex& operator=(const CStaticTex&);
	
	virtual void Destroy();
	
	virtual bool Restore();

	virtual bool Invalidate();

private:
	Render* m_pRender;
	ITextureSampler* m_pTexSampler;
	int m_nIndex;
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	unsigned int m_nLevels;
	TEX_FORMAT m_nFormat;
};

#endif // _STATIC_TEX_H
