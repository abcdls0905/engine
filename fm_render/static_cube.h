//--------------------------------------------------------------------
// �ļ���:		static_cube.h
// ��  ��:		
// ˵  ��:		
// ��������:	2012��9��26��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _STATIC_CUBE_H
#define _STATIC_CUBE_H



#include "../visual/i_resource.h"

// ��̬��������ͼ

class Render;

class CStaticCube: public IStaticCube
{
public:
	CStaticCube(Render* pRender);
	virtual ~CStaticCube();

	// �ͷ�
	virtual void Release();

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


private:
	CStaticCube();
	CStaticCube(const CStaticCube&);
	CStaticCube& operator=(const CStaticCube&);
	
	// ����
	virtual void Destroy();

private:
	Render* m_pRender;
	ITextureSampler* m_pTexSampler;
	int m_nIndex;
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	unsigned int m_nLevels;
	TEX_FORMAT m_nFormat;
};

#endif // _STATIC_CUBE_H
