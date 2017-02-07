//--------------------------------------------------------------------
// 文件名:		walk_height_compress.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年1月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "walk_height_compress.h"
#include "../public/core_mem.h"
#include <float.h>

// CWalkHeightCompress

CWalkHeightCompress::CWalkHeightCompress()
{
	m_pRoot = NULL;
	m_nScale = 0;
	m_pResultData = NULL;
	m_nResultMemSize = 0;
	m_nResultSize = 0;
	m_pMiddleData = NULL;
	m_nMiddleDataSize = 0;
	m_nMinHeight = 0;
	m_nMulFactor = 0;
}

CWalkHeightCompress::~CWalkHeightCompress()
{
	ReleaseAll();
}

void CWalkHeightCompress::ReleaseAll()
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
	}

	m_nResultSize = 0;

	if (m_pMiddleData)
	{
		CORE_FREE(m_pMiddleData, m_nMiddleDataSize);
		m_pMiddleData = NULL;
	}
}

void CWalkHeightCompress::DeleteNode(node_data_t* node)
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

#define FLOAT_EQUAL(v1, v2) \
	((v1 <= (v2 + FLT_EPSILON)) && (v1 >= (v2 - FLT_EPSILON)))

bool CWalkHeightCompress::Compress(size_t scale, const float* pdata, 
	size_t size)
{
	Assert((scale * scale) == size);
	Assert(pdata != NULL);
	
	ReleaseAll();

	// 获得最高点和最低点
	float high = -1e8;
	float low = 1e8;

	for (size_t i = 0; i < size; ++i)
	{
		float val = pdata[i];
		
		if (val > (OUT_SCENE_HEIGHT - 1.0F))
		{
			// 排除特殊的高度值
			continue;
		}
		
		if (val > high)
		{
			high = val;
		}

		if (val < low)
		{
			low = val;
		}
	}

	// 求出最低高度毫米值和乘法因子
	m_nMinHeight = 0;
	m_nMulFactor = 1;

	if (high >= low)
	{
		m_nMinHeight = int((low - 0.001F) * 1000.0F);

		float delta = high - low + 0.001F;

		if (delta > 65.0F)
		{
			m_nMulFactor = int((delta + 64.999F) / 65.0F);
		}
	}

	// 将高度转换为中间整数值
	m_pMiddleData = (int*)CORE_ALLOC(sizeof(int) * size);
	m_nMiddleDataSize = sizeof(int) * size;

	for (size_t k = 0; k < size; ++k)
	{
		float val = pdata[k];

		if (val > 1e7)
		{
			m_pMiddleData[k] = 0xFFFF;
		}
		else if (FLOAT_EQUAL(val, OUT_SCENE_HEIGHT))
		{
			// 用特殊值表示特定高度
			m_pMiddleData[k] = 0xFFFE;
		}
		else
		{
			m_pMiddleData[k] = int((val - low + 0.001F) * 1000.0F)
				/ m_nMulFactor;
		}
	}
	
	m_nScale = scale;
	m_pRoot = CreateNode(scale, 0, 0, m_pMiddleData, size);

	// 足够大的缓冲区
	size_t mem_size = size * sizeof(float) * 2;
	unsigned char* p = (unsigned char*)CORE_ALLOC(mem_size);

	memset(p, 0, mem_size);
	m_pResultData = p;
	m_nResultMemSize = mem_size;
	m_nResultSize = 0;
	
	*(int*)(m_pResultData + m_nResultSize) = m_nMinHeight;
	m_nResultSize += sizeof(int);
	*(int*)(m_pResultData + m_nResultSize) = m_nMulFactor;
	m_nResultSize += sizeof(int);

	return CompressNode(m_pRoot, m_pMiddleData, size) == (m_nResultSize - 8);
}

CWalkHeightCompress::node_data_t* CWalkHeightCompress::CreateNode(
	size_t scale, size_t row, size_t col, const int* pdata, size_t size)
{
	node_data_t* node = (node_data_t*)CORE_ALLOC(sizeof(node_data_t));

	memset(node, 0, sizeof(node_data_t));
	node->nScale = scale;
	node->nRow = row;
	node->nCol = col;

	// 测试所有值是否全部相等
	int first_value = pdata[row * m_nScale + col];
	bool all_equal = true;

	for (size_t r = 0; r < scale; ++r)
	{
		for (size_t c = 0; c < scale; ++c)
		{
			int value = pdata[(row + r) * m_nScale + col + c];

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

size_t CWalkHeightCompress::CompressNode(node_data_t* node, const int* pdata, 
	size_t size)
{
	Assert(node != NULL);

	size_t count = 0;

	if (node->nType == NODE_COMPRESS)
	{
		// 只记录一个高度值
		m_pResultData[m_nResultSize] = node->nType;
		m_nResultSize++;
		count++;

		size_t index = node->nRow * m_nScale + node->nCol;

		*(unsigned short*)(m_pResultData + m_nResultSize) = 
			(unsigned int)(pdata[index]);

		m_nResultSize += sizeof(unsigned short);
		count += sizeof(unsigned short);
	}
	else if (node->nType == NODE_ORIGINAL)
	{
		Assert(node->nScale < 16);
		
		m_pResultData[m_nResultSize] = 
			(unsigned char)((node->nScale << 4) + node->nType);
		m_nResultSize++;
		count++;

		for (size_t r = 0; r < node->nScale; ++r)
		{
			for (size_t c = 0; c < node->nScale; ++c)
			{
				size_t index = (node->nRow + r) * m_nScale + node->nCol + c;

				*(unsigned short*)(m_pResultData + m_nResultSize) = 
					(unsigned int)(pdata[index]);

				m_nResultSize += sizeof(unsigned short);
				count += sizeof(unsigned short);
			}
		}
	}
	else
	{
		size_t pos = m_nResultSize;
		
		m_nResultSize += 1 + 4 * 4;

		size_t s0 = CompressNode(node->pChildren[0], pdata, size);
		size_t s1 = CompressNode(node->pChildren[1], pdata, size);
		size_t s2 = CompressNode(node->pChildren[2], pdata, size);
		size_t s3 = CompressNode(node->pChildren[3], pdata, size);

		size_t children_size = s0 + s1 + s2 + s3;

		// 子节点的最大长度
		size_t size_max = s0;

		if (s1 > size_max)
		{
			size_max = s1;
		}

		if (s2 > size_max)
		{
			size_max = s2;
		}
		
		if (s3 > size_max)
		{
			size_max = s3;
		}

		// 记录子节点长度需要的字节数
		size_t size_bytes = 1;

		if (size_max > 0xFFFF)
		{
			size_bytes = 4;
		}
		else if (size_max > 0xFF)
		{
			size_bytes = 2;
		}

		m_pResultData[pos++] = 
			(unsigned char)((size_bytes << 4) + node->nType);
		count += 1;

		if (size_bytes == 4)
		{
			*(unsigned int*)(m_pResultData + pos) = (unsigned int)(s0);
			pos += sizeof(unsigned int);
			*(unsigned int*)(m_pResultData + pos) = (unsigned int)(s1);
			pos += sizeof(unsigned int);
			*(unsigned int*)(m_pResultData + pos) = (unsigned int)(s2);
			pos += sizeof(unsigned int);
			*(unsigned int*)(m_pResultData + pos) = (unsigned int)(s3);
			pos += sizeof(unsigned int);

			count += 4 * sizeof(unsigned int);
		}
		else if (size_bytes == 2)
		{
			*(unsigned short*)(m_pResultData + pos) = (unsigned short)(s0);
			pos += sizeof(unsigned short);
			*(unsigned short*)(m_pResultData + pos) = (unsigned short)(s1);
			pos += sizeof(unsigned short);
			*(unsigned short*)(m_pResultData + pos) = (unsigned short)(s2);
			pos += sizeof(unsigned short);
			*(unsigned short*)(m_pResultData + pos) = (unsigned short)(s3);
			pos += sizeof(unsigned short);

			count += 4 * sizeof(unsigned short);

			// 结果数据向前移动
			memmove(m_pResultData + m_nResultSize - children_size - 8,
				m_pResultData + m_nResultSize - children_size, children_size);
			
			m_nResultSize -= 8;
		}
		else
		{
			m_pResultData[pos++] = (unsigned char)(s0);
			m_pResultData[pos++] = (unsigned char)(s1);
			m_pResultData[pos++] = (unsigned char)(s2);
			m_pResultData[pos++] = (unsigned char)(s3);

			count += 4;
			
			// 结果数据向前移动
			memmove(m_pResultData + m_nResultSize - children_size - 12,
				m_pResultData + m_nResultSize - children_size, children_size);

			m_nResultSize -= 12;
		}

		count += children_size;
	}

	return count;
}

bool CWalkHeightCompress::Decompress(size_t scale, const unsigned char* pdata, 
	size_t size)
{
	Assert(pdata != NULL);

	ReleaseAll();
	
	size_t data_size = scale * scale * sizeof(float);
	unsigned char* p = (unsigned char*)CORE_ALLOC(data_size);

	memset(p, 0, data_size);
	m_pResultData = p;
	m_nResultMemSize = data_size;
	m_nResultSize = data_size;
	m_nScale = scale;
	
	size_t pos = 0;

	if ((pos + 8) > size)
	{
		return false;
	}

	m_nMinHeight = *(int*)(pdata + pos);
	pos += sizeof(int);
	m_nMulFactor = *(int*)(pdata + pos);
	pos += sizeof(int);
	
	return DecompressData(scale, 0, 0, pdata, size, pos);
}

bool CWalkHeightCompress::DecompressData(size_t scale, size_t row, size_t col,
	const unsigned char* pdata, size_t size, size_t& pos)
{
	if (0 == scale)
	{
		return false;
	}

	if (pos >= size)
	{
		return false;
	}

	float* result = (float*)m_pResultData;
	unsigned int type = pdata[pos] & 0xF;

	if (type == NODE_COMPRESS)
	{
		pos++;

		if ((pos + sizeof(unsigned short)) > size)
		{
			return false;
		}

		unsigned int v = *(unsigned short*)(pdata + pos);

		float value;

		if (v > 0xFFFD)
		{
			if (v == 0xFFFF)
			{
				value = 1e8;
			}
			else
			{
				value = OUT_SCENE_HEIGHT;
			}
		}
		else
		{
			value = ((int)(v * (unsigned int)(m_nMulFactor)) + m_nMinHeight)
				* 0.001F;
		}

		pos += sizeof(unsigned short);

		for (size_t r = 0; r < scale; ++r)
		{
			for (size_t c = 0; c < scale; ++c)
			{
				size_t index = (row + r) * m_nScale + col + c;

				result[index] = value;
			}
		}
	}
	else if (type == NODE_ORIGINAL)
	{
		size_t node_scale = (pdata[pos] >> 4) & 0xF;

		if (node_scale != scale)
		{
			return false;
		}

		pos++;

		size_t origin_size = scale * scale * sizeof(unsigned short);

		if ((pos + origin_size) > size)
		{
			return false;
		}

		for (size_t r = 0; r < scale; ++r)
		{
			for (size_t c = 0; c < scale; ++c)
			{
				size_t index = (row + r) * m_nScale + col + c;

				unsigned int v = *(unsigned short*)(pdata + pos);

				if (v > 0xFFFD)
				{
					if (v == 0xFFFF)
					{
						result[index] = 1e8;
					}
					else
					{
						result[index] = OUT_SCENE_HEIGHT;
					}
				}
				else
				{
					result[index] = (int(v * (unsigned int)(m_nMulFactor)) 
						+ m_nMinHeight) * 0.001F;
				}

				pos += sizeof(unsigned short);
			}
		}
	}
	else if (type == NODE_BRANCH)
	{
		size_t size_bytes = (pdata[pos] >> 4) & 0xF;

		pos++;

		if (1 == size_bytes)
		{
			if ((pos + 4) > size)
			{
				return false;
			}

			pos += 4;
		}
		else if (2 == size_bytes)
		{
			if ((pos + 8) > size)
			{
				return false;
			}

			pos += 8;
		}
		else if (4 == size_bytes)
		{
			if ((pos + 16) > size)
			{
				return false;
			}

			pos += 16;
		}
		else
		{
			return false;
		}

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

		if (!DecompressData(hscale, row + hscale, col + hscale, pdata, size, 
			pos))
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

bool CWalkHeightCompress::GetHeight(size_t scale, size_t row, size_t col,
	const unsigned char* pdata, size_t size, float& value)
{
	Assert(pdata != NULL);
	Assert(row < scale);
	Assert(col < scale);

	size_t pos = 0;
	
	if ((pos + 8) > size)
	{
		return false;
	}

	int min_height = *(int*)(pdata + pos);
	pos += sizeof(int);
	int mul_factor = *(int*)(pdata + pos);
	pos += sizeof(int);

	return GetHeightData(scale, row, col, pdata, size, min_height, 
		mul_factor, pos, value);
}

bool CWalkHeightCompress::GetHeightData(size_t scale, size_t row, size_t col, 
	const unsigned char* pdata, size_t size, int min_height, int mul_factor, 
	size_t& pos, float& value)
{
	Assert(row < scale);
	Assert(col < scale);

	if (pos >= size)
	{
		return false;
	}

	unsigned int type = pdata[pos] & 0xF;

	if (type == NODE_COMPRESS)
	{
		pos++;

		if ((pos + sizeof(unsigned short)) > size)
		{
			return false;
		}

		unsigned int v = *(unsigned short*)(pdata + pos);

		if (v > 0xFFFD)
		{
			if (v == 0xFFFF)
			{
				value = 1e8;
			}
			else
			{
				value = OUT_SCENE_HEIGHT;
			}
		}
		else
		{
			value = (int(v * (unsigned int)(mul_factor)) + min_height)
				* 0.001F;
		}
	}
	else if (type == NODE_ORIGINAL)
	{
		size_t node_scale = (pdata[pos] >> 4) & 0xF;

		if (node_scale != scale)
		{
			return false;
		}

		pos++;

		size_t origin_size = scale * scale * sizeof(unsigned short);

		if ((pos + origin_size) > size)
		{
			return false;
		}

		unsigned short* values = (unsigned short*)(pdata + pos);
		unsigned int v = values[row * scale + col];

		if (v > 0xFFFD)
		{
			if (v == 0xFFFF)
			{
				value = 1e8;
			}
			else
			{
				value = OUT_SCENE_HEIGHT;
			}
		}
		else
		{
			value = ((int)(v * (unsigned int)(mul_factor)) + min_height)
				* 0.001F;
		}
	}
	else if (type == NODE_BRANCH)
	{
		size_t size_bytes = (pdata[pos] >> 4) & 0xF;

		pos++;

		size_t s0;
		size_t s1;
		size_t s2;
		size_t s3;

		if (1 == size_bytes)
		{
			if ((pos + 4) > size)
			{
				return false;
			}

			s0 = *(unsigned char*)(pdata + pos);
			pos++;
			s1 = *(unsigned char*)(pdata + pos);
			pos++;
			s2 = *(unsigned char*)(pdata + pos);
			pos++;
			s3 = *(unsigned char*)(pdata + pos);
			pos++;
		}
		else if (2 == size_bytes)
		{
			if ((pos + 8) > size)
			{
				return false;
			}

			s0 = *(unsigned short*)(pdata + pos);
			pos += 2;
			s1 = *(unsigned short*)(pdata + pos);
			pos += 2;
			s2 = *(unsigned short*)(pdata + pos);
			pos += 2;
			s3 = *(unsigned short*)(pdata + pos);
			pos += 2;
		}
		else if (4 == size_bytes)
		{
			if ((pos + 16) > size)
			{
				return false;
			}

			s0 = *(unsigned int*)(pdata + pos);
			pos += 4;
			s1 = *(unsigned int*)(pdata + pos);
			pos += 4;
			s2 = *(unsigned int*)(pdata + pos);
			pos += 4;
			s3 = *(unsigned int*)(pdata + pos);
			pos += 4;
		}
		else
		{
			return false;
		}

		size_t hscale = scale / 2;

		if (0 == hscale)
		{
			return false;
		}

		if (row < hscale)
		{
			if (col < hscale)
			{
				return GetHeightData(hscale, row, col, pdata, size, 
					min_height, mul_factor, pos, value);
			}
			else
			{
				pos += s0;

				return GetHeightData(hscale, row, col - hscale, pdata, size, 
					min_height, mul_factor, pos, value);
			}
		}
		else
		{
			if (col < hscale)
			{
				pos += s0 + s1;
				
				return GetHeightData(hscale, row - hscale, col, pdata, size, 
					min_height, mul_factor, pos, value);
			}
			else
			{
				pos += s0 + s1 + s2;

				return GetHeightData(hscale, row - hscale, col - hscale, 
					pdata, size, min_height, mul_factor, pos, value);
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}
