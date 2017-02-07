//--------------------------------------------------------------------
// 文件名:		height_compress.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年1月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "height_compress.h"
#include "../public/core_mem.h"

#define HEIGHT_EQUAL(v1, v2) ((v1 <= (v2 + 0.0001F)) && (v1 >= (v2 - 0.0001F)))

// CHeightCompress

CHeightCompress::CHeightCompress()
{
	m_pRoot = NULL;
	m_nScale = 0;
	m_pResultData = NULL;
	m_nResultMemSize = 0;
	m_nResultSize = 0;
}

CHeightCompress::~CHeightCompress()
{
	ReleaseAll();
}

void CHeightCompress::ReleaseAll()
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

	m_nResultSize = 0;
}

void CHeightCompress::DeleteNode(node_data_t* node)
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

bool CHeightCompress::Compress(size_t scale, const float* pdata, size_t size)
{
	Assert((scale * scale) == size);
	Assert(pdata != NULL);
	
	ReleaseAll();
	
	m_nScale = scale;
	m_pRoot = CreateNode(scale, 0, 0, pdata, size);

	// 足够大的缓冲区
	size_t mem_size = size * sizeof(float) * 2;
	unsigned char* p = (unsigned char*)CORE_ALLOC(mem_size);

	memset(p, 0, mem_size);
	m_pResultData = p;
	m_nResultMemSize = mem_size;
	m_nResultSize = 0;
	
	return (CompressNode(m_pRoot, pdata, size) == m_nResultSize);
}

CHeightCompress::node_data_t* CHeightCompress::CreateNode(
	size_t scale, size_t row, size_t col, const float* pdata, size_t size)
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
			float value = pdata[(row + r) * m_nScale + col + c];

			if (!HEIGHT_EQUAL(value, first_value))
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
		node->nType = NODE_COMPRESS_EQUAL;
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

size_t CHeightCompress::CompressNode(node_data_t* node, const float* pdata, 
	size_t size)
{
	Assert(node != NULL);

	size_t count = 0;

	if (node->nType == NODE_COMPRESS_EQUAL)
	{
		// 只记录一个高度值
		m_pResultData[m_nResultSize] = node->nType;
		m_nResultSize++;
		count++;

		size_t index = node->nRow * m_nScale + node->nCol;

		*(float*)(m_pResultData + m_nResultSize) = pdata[index];
		m_nResultSize += sizeof(float);
		count += sizeof(float);
	}
	else if (node->nType == NODE_ORIGINAL)
	{
		Assert(node->nScale < 16);

		// 最低点和最高点
		float min_value = pdata[node->nRow * m_nScale + node->nCol];
		float max_value = pdata[node->nRow * m_nScale + node->nCol];
		
		for (size_t r = 0; r < node->nScale; ++r)
		{
			for (size_t c = 0; c < node->nScale; ++c)
			{
				size_t index = (node->nRow + r) * m_nScale + node->nCol + c;

				if (pdata[index] < min_value)
				{
					min_value = pdata[index];
				}

				if (pdata[index] > max_value)
				{
					max_value = pdata[index];
				}
			}
		}

		if ((max_value - min_value) < 65.0F)
		{
			// 高度差小于65米
			unsigned int max_delta = 
				(unsigned int)((max_value - min_value) * 1000.0F);

			if (max_delta < 256)
			{
				node->nType = NODE_COMPRESS_DELTA8;
			}
			else
			{
				node->nType = NODE_COMPRESS_DELTA16;
			}
		}
		
		m_pResultData[m_nResultSize] = 
			(unsigned char)((node->nScale << 4) + node->nType);
		m_nResultSize++;
		count++;

		if (node->nType == NODE_COMPRESS_DELTA8)
		{
			*(float*)(m_pResultData + m_nResultSize) = min_value;
			m_nResultSize += sizeof(float);
			count += sizeof(float);
			
			for (size_t r = 0; r < node->nScale; ++r)
			{
				for (size_t c = 0; c < node->nScale; ++c)
				{
					size_t index = 
						(node->nRow + r) * m_nScale + node->nCol + c;
					unsigned int delta = 
						(unsigned int)((pdata[index] - min_value) * 1000.0F);

					*(unsigned char*)(m_pResultData + m_nResultSize) = 
						(unsigned char)(delta);
					m_nResultSize += 1;
					count += 1;
				}
			}
		}
		else if (node->nType == NODE_COMPRESS_DELTA16)
		{
			*(float*)(m_pResultData + m_nResultSize) = min_value;
			m_nResultSize += sizeof(float);
			count += sizeof(float);

			for (size_t r = 0; r < node->nScale; ++r)
			{
				for (size_t c = 0; c < node->nScale; ++c)
				{
					size_t index = 
						(node->nRow + r) * m_nScale + node->nCol + c;
					unsigned int delta = 
						(unsigned int)((pdata[index] - min_value) * 1000.0F);

					*(unsigned short*)(m_pResultData + m_nResultSize) = 
						(unsigned short)(delta);
					m_nResultSize += 2;
					count += 2;
				}
			}
		}
		else 
		{
			for (size_t r = 0; r < node->nScale; ++r)
			{
				for (size_t c = 0; c < node->nScale; ++c)
				{
					size_t index = 
						(node->nRow + r) * m_nScale + node->nCol + c;

					*(float*)(m_pResultData + m_nResultSize) = pdata[index];
					m_nResultSize += sizeof(float);
					count += sizeof(float);
				}
			}
		}
	}
	else
	{
		m_pResultData[m_nResultSize] = node->nType;
		m_nResultSize++;
		count++;
		count += CompressNode(node->pChildren[0], pdata, size);
		count += CompressNode(node->pChildren[1], pdata, size);
		count += CompressNode(node->pChildren[2], pdata, size);
		count += CompressNode(node->pChildren[3], pdata, size);
	}

	return count;
}

bool CHeightCompress::Decompress(size_t scale, const unsigned char* pdata, 
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
	
	return DecompressData(scale, 0, 0, pdata, size, pos);
}

bool CHeightCompress::DecompressData(size_t scale, size_t row, size_t col,
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
	
	if (type == NODE_COMPRESS_EQUAL)
	{
		pos++;

		if ((pos + sizeof(float)) > size)
		{
			return false;
		}

        float value ;
        memcpy(&value,(pdata + pos),sizeof(float));

		pos += sizeof(float);

		for (size_t r = 0; r < scale; ++r)
		{
			for (size_t c = 0; c < scale; ++c)
			{
				size_t index = (row + r) * m_nScale + col + c;

				result[index] = value;
			}
		}
	}
	else if (type == NODE_COMPRESS_DELTA8)
	{
		pos++;

		if ((pos + sizeof(float)) > size)
		{
			return false;
		}

		float value ;
		//float value = *(float*)(pdata + pos);
         memcpy(&value,(pdata + pos),sizeof(float));

		pos += sizeof(float);

		size_t delta_size = scale * scale;

		if ((pos + delta_size) > size)
		{
			return false;
		}

		for (size_t r = 0; r < scale; ++r)
		{
			for (size_t c = 0; c < scale; ++c)
			{
				unsigned int delta = *(unsigned char*)(pdata + pos);

				pos += 1;
				
				size_t index = (row + r) * m_nScale + col + c;

				result[index] = value + float(delta) * 0.001F;
			}
		}
	}
	else if (type == NODE_COMPRESS_DELTA16)
	{
		pos++;

		if ((pos + sizeof(float)) > size)
		{
			return false;
		}

		//float value = *(float*)(pdata + pos);
		float value ;
         memcpy(&value,(pdata + pos),sizeof(float));

		pos += sizeof(float);

		size_t delta_size = scale * scale * 2;

		if ((pos + delta_size) > size)
		{
			return false;
		}

		for (size_t r = 0; r < scale; ++r)
		{
			for (size_t c = 0; c < scale; ++c)
			{
				unsigned int delta = *(unsigned short*)(pdata + pos);

				pos += 2;

				size_t index = (row + r) * m_nScale + col + c;

				result[index] = value + float(delta) * 0.001F;;
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

		size_t origin_size = scale * scale * sizeof(float);

		if ((pos + origin_size) > size)
		{
			return false;
		}

		for (size_t r = 0; r < scale; ++r)
		{
			for (size_t c = 0; c < scale; ++c)
			{
				size_t index = (row + r) * m_nScale + col + c;

				//result[index] = *(float*)(pdata + pos);
				memcpy(&result[index] ,(pdata + pos),sizeof(float));//解决XCODE的内存对齐问题
				pos += sizeof(float);
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

