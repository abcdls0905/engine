//--------------------------------------------------------------------
// 文件名:		walk_bit_map.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年1月13日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _WALK_BIT_MAP_H
#define _WALK_BIT_MAP_H

#include "../public/macros.h"

// 行走标记图

class CWalkBitMap
{
public:
	CWalkBitMap();
	~CWalkBitMap();

	// 获得原始数据
	unsigned char* GetData() const { return m_pValues; }
	size_t GetSize() const { return m_nRows * m_nCols; }

	// 获得压缩后的数据
	unsigned char* GetCompressData() const { return m_pCompressData; }
	size_t GetCompressSize() const { return m_nCompressSize; }

	// 设置为压缩数据
	bool BuildCompress(size_t rows, size_t cols, unsigned char* pdata, 
		size_t size);

	// 创建
	bool Create(size_t rows, size_t cols, const unsigned char* pValues);
	// 设置标记值
	bool SetValue(size_t row, size_t col, unsigned int value);
	// 获得标记值
	unsigned int GetValue(size_t row, size_t col);
	// 是否完全空白
	bool GetIsNull() const;

private:
	CWalkBitMap(const CWalkBitMap&);
	CWalkBitMap& operator=(const CWalkBitMap&);

	void ReleaseAll();

private:
	size_t m_nRows;
	size_t m_nCols;
	unsigned char* m_pValues;
	unsigned char* m_pCompressData;
	size_t m_nCompressSize;
};

#endif // _WALK_BIT_MAP_H

