//--------------------------------------------------------------------
// 文件名:		walk_height_map.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年1月13日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _WALK_HEIGHT_MAP_H
#define _WALK_HEIGHT_MAP_H

#include "../public/macros.h"
#include "../utils/array_pod.h"

// CWalkHeightMap
// 行走高度图

class CWalkHeightMap
{
public:
	CWalkHeightMap();
	~CWalkHeightMap();

	// 获得原始数据
	float* GetData() const { return m_pValues; }
	size_t GetSize() const { return m_nRows * m_nCols; }

	// 获得压缩后的数据
	unsigned char* GetCompressData() const { return m_pCompressData; }
	size_t GetCompressSize() const { return m_nCompressSize; }

	// 设置为压缩数据
	bool BuildCompress(size_t rows, size_t cols, unsigned char* pdata, 
		size_t size);

	// 创建
	bool Create(size_t rows, size_t cols, float* pValues);

	// 设置高度
    bool SetValue(size_t row, size_t col, float value)
    {
        Assert(row < m_nRows);
        Assert(col < m_nCols);
        Assert(m_pValues != NULL);

        m_pValues[row * m_nCols + col] = value;

        return true;
    }

    // 将高度设置为统一值
    void ResetValue();

	// 获得高度值
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
