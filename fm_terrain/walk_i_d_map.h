//--------------------------------------------------------------------
// �ļ���:		WalkIDMap.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��3��21��
// ������:		����ΰ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _WALKIDMAP_H
#define _WALKIDMAP_H

#include "../public/macros.h"
#include "../public/persistid.h"

// CWalkIDMap
// ����IDͼ

class CWalkIDMap
{
public:
	CWalkIDMap();
	~CWalkIDMap();

	// ���ԭʼ����
	PERSISTID* GetData() const { return m_pValues; }
	size_t GetSize() const { return m_nRows * m_nCols; }

	// ����
	bool Create(size_t rows, size_t cols, PERSISTID* pValues);
	// ����ID
	bool SetValue(size_t row, size_t col, const PERSISTID& value);
	// ���IDֵ
	PERSISTID GetValue(size_t row, size_t col);

private:
	CWalkIDMap(const CWalkIDMap&);
	CWalkIDMap& operator=(const CWalkIDMap&);

private:
	size_t m_nRows;
	size_t m_nCols;
	PERSISTID* m_pValues;
};

#endif // _WALKIDMAP_H
