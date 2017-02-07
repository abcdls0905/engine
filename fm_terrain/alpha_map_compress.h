//--------------------------------------------------------------------
// 文件名:		alpha_map_compress.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年1月11日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _ALPHA_MAP_COMPRESS_H
#define _ALPHA_MAP_COMPRESS_H

#include "../public/macros.h"

// 贴图混合信息压缩

class CAlphaMapCompress
{
private:
	// 节点类型
	enum 
	{
		NODE_BRANCH,	// 带有子节点
		NODE_ORIGINAL,	// 未压缩的值
		NODE_COMPRESS,	// 压缩后的值
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
	CAlphaMapCompress();
	~CAlphaMapCompress();

	// 压缩
	bool Compress(size_t scale, const unsigned char* pdata, size_t size);
	// 解压缩
	bool Decompress(size_t scale, const unsigned char* pdata, size_t size);

	// 获得结果数据
	const unsigned char* GetResultData() const { return m_pResultData; }
	size_t GetResultSize() const { return m_nResultSize; }

private:
	CAlphaMapCompress(const CAlphaMapCompress&);
	CAlphaMapCompress& operator=(const CAlphaMapCompress&);

	void ReleaseAll();

	// 删除节点
	void DeleteNode(node_data_t* node);
	// 设置节点
	node_data_t* CreateNode(size_t scale, size_t row, size_t col,
		const unsigned char* pdata, size_t size);
	// 压缩节点
	bool CompressNode(node_data_t* node, const unsigned char* pdata, 
		size_t size);
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

#endif // _ALPHA_MAP_COMPRESS_H

