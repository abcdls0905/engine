//--------------------------------------------------------------------
// �ļ���:		tex_light_map.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��8��9��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TEX_LIGHT_MAP_H
#define _TEX_LIGHT_MAP_H

#include "../public/macros.h"


#include "../public/i_interface.h"
#include "../visual/i_render.h"
#include "../visual/i_resource.h"

// ���������ͼ

class CTexLightMap
{
public:
	CTexLightMap();
	~CTexLightMap();

	// ���
	void SetRows(size_t value) { m_nRows = value; }
	size_t GetRows() const { return m_nRows; }

	// �߶�
	void SetCols(size_t value) { m_nCols = value; }
	size_t GetCols() const { return m_nCols; }

	// ��������
	size_t GetSize() const { return m_nRows * m_nCols; }

	// ��ͼ��Դ
	void SetTexture(IRender* pRender, ITexture* value);

	ITexture* GetTexture();

	// ������ͼ
	bool CreateTex(IRender* pRender);
	// �ͷ���ͼ
	void ReleaseTex();

	// ʹ����ͼ�ļ�����������ͼ
	bool BuildTex(IRender* pRender, ITexture* pTex);
	
	// �����ɫ���ݵ���ͼ
	bool WriteColor32(const unsigned int* colors, size_t rows, size_t cols);
	// ���Ϊ��ͬ����ֵ
	bool WriteValue(unsigned int value, size_t rows, size_t cols);

	// ������ͼ���ļ�
	bool SaveBitmap(const char* file);
	// ���浽ѹ����DDS�ļ�
	bool SaveDDS(IRender* pRender, const char* file, bool save_occlusion);

private:
	CTexLightMap(const CTexLightMap&);
	CTexLightMap& operator=(const CTexLightMap&);

private:
	size_t m_nRows;
	size_t m_nCols;
	ITexture* m_pTexture;
};

#endif // _TEX_LIGHT_MAP_H
 
