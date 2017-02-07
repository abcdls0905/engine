//--------------------------------------------------------------------
// 文件名:		WalkIDMap.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年3月21日
// 创建人:		沈奇伟
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _WALKIDMAP_H
#define _WALKIDMAP_H

#include "../public/macros.h"
#include "../public/persistid.h"

// CWalkIDMap
// 行走ID图

class CWalkIDMap
{
public:
	CWalkIDMap();
	~CWalkIDMap();

	// 获得原始数据
	PERSISTID* GetData() const { return m_pValues; }
	size_t GetSize() const { return m_nRows * m_nCols; }

	// 创建
	bool Create(size_t rows, size_t cols, PERSISTID* pValues);
	// 设置ID
	bool SetValue(size_t row, size_t col, const PERSISTID& value);
	// 获得ID值
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
