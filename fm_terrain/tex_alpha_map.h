//--------------------------------------------------------------------
// �ļ���:		tex_alpha_map.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��8��9��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TEX_ALPHA_MAP_H
#define _TEX_ALPHA_MAP_H

#include "../public/macros.h"



// ���ڵر���ͼ�ںϵ�ALPHA��ͼ����

class CTexAlphaMap
{
public:
	CTexAlphaMap();
	~CTexAlphaMap();

	// �ߴ�
	void SetScale(size_t value) { m_nScale = value; }
	// ALPHA���ݵ��ֽ���
	size_t GetSize() const { return m_nScale * m_nScale; }

	// ����ALPHA���ݣ���ȫ��Ϊ0
	void CreateAlpha();

	// ����Alphaֵ
	bool SetAlpha(size_t row, size_t col, size_t value);
	// ���Alphaֵ
	size_t GetAlpha(size_t row, size_t col) const;

	// �Ƿ�Ϊ�հ�
	bool IsEmpty() const;

	// ���ALPHA���ݵ���ͼ
	bool WriteMapData(const unsigned char* data, size_t size);

	// ���ALPHA����
	unsigned char* GetMapData() const { return m_pAlpha; }
	
private:
	CTexAlphaMap(const CTexAlphaMap&);
	CTexAlphaMap& operator=(const CTexAlphaMap&);

	// ���ALPHA����
	void ReleaseAlpha();

private:
	size_t m_nScale;
	unsigned char* m_pAlpha;
	size_t m_nSize;
};

#endif // _TEX_ALPHA_MAP_H

