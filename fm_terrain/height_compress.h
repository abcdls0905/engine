//--------------------------------------------------------------------
// 文件名:		height_compress.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年1月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _HEIGHT_COMPRESS_H
#define _HEIGHT_COMPRESS_H

#include "../public/macros.h"

// 高度信息压缩

class CHeightCompress
{
private:
	// 节点类型
	enum 
	{
		NODE_BRANCH,			// 带有子节点
		NODE_ORIGINAL,			// 未压缩的值
		NODE_COMPRESS_EQUAL,	// 等值压缩
		NODE_COMPRESS_DELTA8,	// 8位差值压缩
		NODE_COMPRESS_DELTA16,	// 16位差值压缩
	};

	// 节点数据
	struct node_data_t
	{
		int nType;
		size_t nScale;
		size_t nRow;
		size_t nCol;
		node_data_t* pChildren[4];
	};

public:
	CHeightCompress();
	~CHeightCompress();

	// 压缩
	bool Compress(size_t scale, const float* pdata, size_t size);
	// 解压缩
	bool Decompress(size_t scale, const unsigned char* pdata, size_t size);

	// 获得结果数据
	const unsigned char* GetResultData() const { return m_pResultData; }
	size_t GetResultSize() const { return m_nResultSize; }

private:
	CHeightCompress(const CHeightCompress&);
	CHeightCompress& operator=(const CHeightCompress&);

	void ReleaseAll();

	// 删除节点
	void DeleteNode(node_data_t* node);
	// 设置节点
	node_data_t* CreateNode(size_t scale, size_t row, size_t col,
		const float* pdata, size_t size);
	// 压缩节点
	size_t CompressNode(node_data_t* node, const float* pdata, size_t size);
	// 数据解压缩
	bool DecompressData(size_t scale, size_t row, size_t col,
		const unsigned char* pdata, size_t size, size_t& pos);

private:
	node_data_t* m_pRoot;
	size_t m_nScale;
	unsigned char* m_pResultData;
	size_t m_nResultMemSize;
	size_t m_nResultSize;
};

#endif // _HEIGHT_COMPRESS_H
