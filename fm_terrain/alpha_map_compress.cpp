//--------------------------------------------------------------------
// 文件名:		alpha_map_compress.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年1月11日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "alpha_map_compress.h"
#include "../public/core_mem.h"

// CAlphaMapCompress

CAlphaMapCompress::CAlphaMapCompress()
{
	m_pRoot = NULL;
	m_nScale = 0;
	m_pResultData = NULL;
	m_nResultMemSize = 0;
	m_nResultSize = 0;
}

CAlphaMapCompress::~CAlphaMapCompress()
{
	ReleaseAll();
}

void CAlphaMapCompress::ReleaseAll()
{
	if (m_pRoot)
	{
		DeleteNode(m_pRoot);
		m_pRoot = NULL;
	}

	if (m_pResultData)
	{
		CORE_FREE(m_pResultData, m_nResultMemSize);
		m_pResultData = NULL;
		m_nResultMemSize = 0;
	}
}

void CAlphaMapCompress::DeleteNode(node_data_t* node)
{
	Assert(node != NULL);

	for (size_t i = 0; i < 4; ++i)
	{
		if (node->pChildren[i])
		{
			DeleteNode(node->pChildren[i]);
		}
	}

	CORE_FREE(node, sizeof(node_data_t));
}

bool CAlphaMapCompress::Compress(size_t scale, const unsigned char* pdata, 
	size_t size)
{
	Assert((scale * scale / 2) == size);
	Assert(pdata != NULL);
	
	ReleaseAll();
	
	m_nScale = scale;
	m_pRoot = CreateNode(scale, 0, 0, pdata, size);

	unsigned char* p = (unsigned char*)CORE_ALLOC(size * 2);

	memset(p, 0, size * 2);
	m_pResultData = p;
	m_nResultMemSize = size * 2;
	m_nResultSize = 0;
	
	return CompressNode(m_pRoot, pdata, size);
}

CAlphaMapCompress::node_data_t* CAlphaMapCompress::CreateNode(size_t scale, 
	size_t row, size_t col, const unsigned char* pdata, size_t size)
{
	node_data_t* node = (node_data_t*)CORE_ALLOC(sizeof(node_data_t));

	memset(node, 0, sizeof(node_data_t));
	node->nScale = scale;
	node->nRow = row;
	node->nCol = col;

	// 测试所有值是否全部相等
	unsigned char first_value = pdata[(row * m_nScale + col) / 2] & 0xF;
	bool all_equal = true;

	for (size_t r = 0; r < scale; ++r)
	{
		for (size_t c = 0; c < scale; ++c)
		{
			size_t index = ((row + r) * m_nScale + col + c) / 2;
			unsigned char value;

			if ((c & 0x1) == 0)
			{
				value = pdata[index] & 0xF;
			}
			else
			{
				value = (pdata[index] >> 4) & 0xF;
			}

			if (value != first_value)
			{
				all_equal = false;
				break;
			}
		}

		if (!all_equal)
		{
			break;
		}
	}

	if (all_equal)
	{
		node->nType = NODE_COMPRESS;
	}
	else if (node->nScale == 4)
	{
		node->nType = NODE_ORIGINAL;
	}
	else
	{
		node->nType = NODE_BRANCH;

		size_t hscale = scale / 2;

		node->pChildren[0] = CreateNode(hscale, row, col, pdata, size);
		node->pChildren[1] = CreateNode(hscale, row, col + hscale, pdata, 
			size);
		node->pChildren[2] = CreateNode(hscale, row + hscale, col, pdata, 
			size);
		node->pChildren[3] = CreateNode(hscale, row + hscale, col + hscale,
			pdata, size);
	}

	return node;
}

bool CAlphaMapCompress::CompressNode(node_data_t* node, 
	const unsigned char* pdata, size_t size)
{
	Assert(node != NULL);

	if (node->nType == NODE_COMPRESS)
	{
		size_t index = (node->nRow * m_nScale + node->nCol) / 2;

		unsigned char value = pdata[index] & 0xF;

		m_pResultData[m_nResultSize++] = (value << 4) + node->nType;
	}
	else if (node->nType == NODE_ORIGINAL)
	{
		m_pResultData[m_nResultSize++] = node->nType;

		for (size_t r = 0; r < node->nScale; ++r)
		{
			for (size_t c = 0; c < node->nScale; ++c)
			{
				if ((c & 0x1) == 0)
				{
					size_t index = ((node->nRow + r) * m_nScale 
						+ node->nCol + c) / 2;

					m_pResultData[m_nResultSize++] = pdata[index];
				}
			}
		}
	}
	else
	{
		m_pResultData[m_nResultSize++] = node->nType;

		for (size_t i = 0; i < 4; ++i)
		{
			if (NULL == node->pChildren[i])
			{
				continue;
			}

			if (!CompressNode(node->pChildren[i], pdata, size))
			{
				return false;
			}
		}
	}

	return true;
}

bool CAlphaMapCompress::Decompress(size_t scale, const unsigned char* pdata, 
	size_t size)
{
	Assert(pdata != NULL);

	ReleaseAll();
	
	size_t data_size = scale * scale / 2;
	unsigned char* p = (unsigned char*)CORE_ALLOC(data_size);

	memset(p, 0, data_size);
	m_pResultData = p;
	m_nResultMemSize = data_size;
	m_nResultSize = data_size;
	m_nScale = scale;
	
	size_t pos = 0;
	
	return DecompressData(scale, 0, 0, pdata, size, pos);
}

bool CAlphaMapCompress::DecompressData(size_t scale, size_t row, size_t col,
	const unsigned char* pdata, size_t size, size_t& pos)
{
	if (scale < 4)
	{
		return false;
	}

	if (pos >= size)
	{
		return false;
	}

	unsigned int type = pdata[pos] & 0xF;

	if (type == NODE_COMPRESS)
	{
		unsigned char value = (pdata[pos] >> 4) & 0xF;

		pos++;

		value += (value << 4);

		for (size_t r = 0; r < scale; ++r)
		{
			for (size_t c = 0; c < scale; ++c)
			{
				if ((c & 0x1) == 0)
				{
					size_t index = ((row + r) * m_nScale + col + c) / 2;

					m_pResultData[index] = value;
				}
			}
		}
	}
	else if (type == NODE_ORIGINAL)
	{
		pos++;

		size_t origin_size = (scale * scale) / 2;

		if ((pos + origin_size) > size)
		{
			return false;
		}

		for (size_t r = 0; r < scale; ++r)
		{
			for (size_t c = 0; c < scale; ++c)
			{
				if ((c & 0x1) == 0)
				{
					size_t index = ((row + r) * m_nScale + col + c) / 2;

					m_pResultData[index] = pdata[pos++];
				}
			}
		}
	}
	else if (type == NODE_BRANCH)
	{
		pos++;

		size_t hscale = scale / 2;

		if (!DecompressData(hscale, row, col, pdata, size, pos))
		{
			return false;
		}

		if (!DecompressData(hscale, row, col + hscale, pdata, size, pos))
		{
			return false;
		}

		if (!DecompressData(hscale, row + hscale, col, pdata, size, pos))
		{
			return false;
		}

		if (!DecompressData(hscale, row + hscale, col + hscale, 
			pdata, size, pos))
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

