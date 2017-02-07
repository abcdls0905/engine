//--------------------------------------------------------------------
// �ļ���:		walk_height_map.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��1��13��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _WALK_HEIGHT_MAP_H
#define _WALK_HEIGHT_MAP_H

#include "../public/macros.h"
#include "../utils/array_pod.h"

// CWalkHeightMap
// ���߸߶�ͼ

class CWalkHeightMap
{
public:
	CWalkHeightMap();
	~CWalkHeightMap();

	// ���ԭʼ����
	float* GetData() const { return m_pValues; }
	size_t GetSize() const { return m_nRows * m_nCols; }

	// ���ѹ���������
	unsigned char* GetCompressData() const { return m_pCompressData; }
	size_t GetCompressSize() const { return m_nCompressSize; }

	// ����Ϊѹ������
	bool BuildCompress(size_t rows, size_t cols, unsigned char* pdata, 
		size_t size);

	// ����
	bool Create(size_t rows, size_t cols, float* pValues);

	// ���ø߶�
    bool SetValue(size_t row, size_t col, float value)
    {
        Assert(row < m_nRows);
        Assert(col < m_nCols);
        Assert(m_pValues != NULL);

        m_pValues[row * m_nCols + col] = value;

        return true;
    }

    // ���߶�����Ϊͳһֵ
    void ResetValue();

	// ��ø߶�ֵ
	float GetValue(size_t row, size_t col);

private:
	CWalkHeightMap(const CWalkHeightMap&);
	CWalkHeightMap& operator=(const CWalkHeightMap&);

private:
	size_t m_nRows;
	size_t m_nCols;
	float* m_pValues;

	unsigned char* m_pCompressData;
	size_t m_nCompressSize;
};

#endif // _WALK_HEIGHT_MAP_H
