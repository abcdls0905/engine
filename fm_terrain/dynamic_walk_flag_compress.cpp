//--------------------------------------------------------------------
// 文件名:		DynamicWalkFlagCompress.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2013年3月26日
// 创建人:		沈奇伟
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "dynamic_walk_flag_compress.h"
#include "../public/core_mem.h"

// CDynamicWalkFlagCompress

CDynamicWalkFlagCompress::CDynamicWalkFlagCompress()
{
	m_pRoot = NULL;
	m_nScale = 0;
	m_pResultData = NULL;
	m_nResultMemSize = 0;
	m_nResultSize = 0;
}

CDynamicWalkFlagCompress::~CDynamicWalkFlagCompress()
{
	ReleaseAll();
}

void CDynamicWalkFlagCompress::ReleaseAll()
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
}

void CDynamicWalkFlagCompress::DeleteNode(node_data_t* node)
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

bool CDynamicWalkFlagCompress::Compress(size_t scale, const unsigned char* pdata, 
								 size_t size)
{
	Assert((scale * scale) == size);
	Assert(pdata != NULL);

	ReleaseAll();

	m_nScale = scale;
	m_pRoot = CreateNode(scale, 0, 0, pdata, size);

	// 足够大的缓冲区
	unsigned char* p = (unsigned char*)CORE_ALLOC(size * 2);

	memset(p, 0, size * 2);
	m_pResultData = p;
	m_nResultMemSize = size * 2;
	m_nResultSize = 0;

	return (CompressNode(m_pRoot, pdata, size) == m_nResultSize);
}


CDynamicWalkFlagCompress::node_data_t* CDynamicWalkFlagCompress::CreateNode(size_t scale, 
	size_t row, size_t col, const unsigned char* pdata, size_t size)
{
	node_data_t* node = (node_data_t*)CORE_ALLOC(sizeof(node_data_t));

	memset(node, 0, sizeof(node_data_t));
	node->nScale = scale;
	node->nRow = row;
	node->nCol = col;

	// 测试所有值是否全部相等
	float first_value = pdata[row * m_nScale + col];

	bool all_equal = true;

	for (size_t r = 0; r < scale; ++r)
	{
		for (size_t c = 0; c < scale; ++c)
		{
			unsigned char value = pdata[(row + r) * m_nScale + col + c];

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

size_t CDynamicWalkFlagCompress::CompressNode(node_data_t* node,
											  const unsigned char* pdata, size_t size)
{
	Assert(node != NULL);

	size_t count = 0;

	if (node->nType == NODE_COMPRESS)
	{
		// 只记录一个标记值
		size_t index = node->nRow * m_nScale + node->nCol;

		m_pResultData[m_nResultSize] = node->nType;
		m_nResultSize++;
		count++;

		m_pResultData[m_nResultSize] = pdata[index];
		m_nResultSize++;
		count++;
	}
	else if (node->nType == NODE_ORIGINAL)
	{
		Assert(node->nScale < 16);

		m_pResultData[m_nResultSize] = (node->nScale << 4) + node->nType;
		m_nResultSize++;
		count++;

		//size_t k = 0;
		unsigned char v = 0;

		for (size_t r = 0; r < node->nScale; ++r)
		{
			for (size_t c = 0; c < node->nScale; ++c)
			{
				size_t index = (node->nRow + r) * m_nScale + node->nCol + c;
				m_pResultData[m_nResultSize] = pdata[index];
				m_nResultSize++;
				count++;

				//if (++k == 2)
				//{
				//	m_pResultData[m_nResultSize] = v;
				//	m_nResultSize++;
				//	count++;
				//	
				//	k = 0;
				//	v = 0;
				//}
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

		m_pResultData[pos++] = (size_bytes << 4) + node->nType;

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

bool CDynamicWalkFlagCompress::Decompress(size_t scale,
	const unsigned char* pdata, size_t size)
{
	Assert(pdata != NULL);

	ReleaseAll();

	size_t data_size = scale * scale;
	unsigned char* p = (unsigned char*)CORE_ALLOC(data_size);

	memset(p, 0, data_size);
	m_pResultData = p;
	m_nResultMemSize = data_size;
	m_nResultSize = data_size;
	m_nScale = scale;

	size_t pos = 0;

	return DecompressData(scale, 0, 0, pdata, size, pos);
}

bool CDynamicWalkFlagCompress::DecompressData(size_t scale, size_t row,
	size_t col, const unsigned char* pdata, size_t size, size_t& pos)
{
	if (0 == scale)
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

		for (size_t r = 0; r < scale; ++r)
		{
			for (size_t c = 0; c < scale; ++c)
			{
				size_t index = (row + r) * m_nScale + col + c;

				m_pResultData[index] = value;
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

		size_t origin_size = scale * scale;

		if ((pos + origin_size) > size)
		{
			return false;
		}

		for (size_t r = 0; r < scale; ++r)
		{
			for (size_t c = 0; c < scale; ++c)
			{
				size_t index = (row + r) * m_nScale + col + c;

				m_pResultData[index] = pdata[pos];

				pos++;
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

bool CDynamicWalkFlagCompress::GetFlag(size_t scale, size_t row, size_t col,
	const unsigned char* pdata, size_t size, unsigned char& value)
{
	Assert(pdata != NULL);
	Assert(row < scale);
	Assert(col < scale);

	size_t pos = 0;

	return GetFlagData(scale, row, col, pdata, size, pos, value);
}

bool CDynamicWalkFlagCompress::GetFlagData(size_t scale, size_t row, size_t col,
										   const unsigned char* pdata, size_t size, size_t& pos, unsigned char& value)
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
		value = pdata[pos];
	}
	else if (type == NODE_ORIGINAL)
	{
		size_t node_scale = (pdata[pos] >> 4) & 0xF;

		if (node_scale != scale)
		{
			return false;
		}

		pos++;

		size_t origin_size = scale * scale;

		if ((pos + origin_size) > size)
		{
			return false;
		}

		//size_t index = row * scale + col;
		//size_t pos_byte = index >> 1;
		size_t pos_byte = row * scale + col;

		value = pdata[pos + pos_byte];
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
				return GetFlagData(hscale, row, col, pdata, size, pos, 
					value);
			}
			else
			{
				pos += s0;

				return GetFlagData(hscale, row, col - hscale, pdata, size, 
					pos, value);
			}
		}
		else
		{
			if (col < hscale)
			{
				pos += s0 + s1;

				return GetFlagData(hscale, row - hscale, col, pdata, size, 
					pos, value);
			}
			else
			{
				pos += s0 + s1 + s2;

				return GetFlagData(hscale, row - hscale, col - hscale, 
					pdata, size, pos, value);
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}
