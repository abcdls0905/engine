//--------------------------------------------------------------------
// �ļ���:		walk_flag_map.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��9��22��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _WALK_FLAG_MAP_H
#define _WALK_FLAG_MAP_H

#include "../public/macros.h"

// ���߱��ͼ

class CWalkFlagMap
{
public:
	CWalkFlagMap();
	~CWalkFlagMap();

	// ���ԭʼ����
	unsigned char* GetData() const { return m_pValues; }
	size_t GetSize() const { return m_nRows * m_nCols; }

	// ���ѹ���������
	unsigned char* GetCompressData() const { return m_pCompressData; }
	size_t GetCompressSize() const { return m_nCompressSize; }

	// ����Ϊѹ������
	bool BuildCompress(size_t rows, size_t cols, unsigned char* pdata, 
		size_t size);

	// ����
	bool Create(size_t rows, size_t cols, const unsigned char* pValues);
	// ���ñ��ֵ
	bool SetValue(size_t row, size_t col, unsigned int value);
	// ��ñ��ֵ
	unsigned int GetValue(size_t row, size_t col);
	// ��ñ��ֵ
	unsigned int GetDynamicValue(size_t row, size_t col);
	// �Ƿ���ȫ�հ�
	bool GetIsNull() const;

private:
	CWalkFlagMap(const CWalkFlagMap&);
	CWalkFlagMap& operator=(const CWalkFlagMap&);

	void ReleaseAll();

private:
	size_t m_nRows;
	size_t m_nCols;
	unsigned char* m_pValues;
	unsigned char* m_pCompressData;
	size_t m_nCompressSize;
};

#endif // _WALK_FLAG_MAP_H
