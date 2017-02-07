//--------------------------------------------------------------------
// 文件名:		chunk_style.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "chunk_style.h"
#include "../visual/i_render.h"
#include "../visual/i_resource.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"

// CChunkStyle

CChunkStyle::CChunkStyle()
{
	m_nTriangleNum = 0;
	m_pIndices = NULL;
	m_pIBuffer = NULL;
}

CChunkStyle::~CChunkStyle()
{
	if (m_pIndices)
	{
		CORE_FREE(m_pIndices, sizeof(unsigned short) * m_nTriangleNum * 3);
	}

	ReleaseIndexBuffer();

	for (size_t i = 0; i < m_Collides.size(); ++i)
	{
		CORE_DELETE(m_Collides[i]);
	}
}

// 矩形是否相交
static inline bool rect_intersect(int left1, int top1, int right1, int bottom1,
	int left2, int top2, int right2, int bottom2)
{
	//return (right1 >= left2) && (right2 >= left1) 
	//	&& (bottom1 >= top2) && (bottom2 >= top1);
	return (right1 > left2) && (right2 > left1) 
		&& (bottom1 > top2) && (bottom2 > top1);
}

#define INDEX_VALUE(row, col) ((unsigned int)(((row) << 16) + (col)))
#define INDEX_INVALID 0xFFFFFFFF

bool CChunkStyle::Generate(size_t scale, int level, int type, 
	int collide_scale)
{
	Assert(scale > 0);
	Assert(m_pIndices == NULL);

	// 相邻的格子是否需要连接
	int left = (type >> 3) & 0x1;
	int top = (type >> 2) & 0x1;
	int right = (type >> 1) & 0x1;
	int bottom = (type >> 0) & 0x1;

	// 每边的三角形数量
	size_t size = scale >> level;
	// 总的三角形数量
	size_t triangle_num = size * size * 2;
	// 生成索引
	TArrayPod<unsigned int, 256, TChunkStyleAlloc> temp_indices;

	temp_indices.resize(triangle_num * 3);

	unsigned int* index = &temp_indices[0];
	size_t half_size = size / 2;
	// 每边的顶点数量
	size_t size1 = size + 1;

	for (size_t r = 0; r < size; ++r)
	{
		for (size_t c = 0; c < size; ++c)
		{
			if ((c < half_size) ^ (r < half_size))
			{
				// t1
				*index++ = INDEX_VALUE(r, c);
				*index++ = INDEX_VALUE(r + 1, c);
				*index++ = INDEX_VALUE(r, c + 1);
				// t2
				*index++ = INDEX_VALUE(r + 1, c);
				*index++ = INDEX_VALUE(r + 1, c + 1);
				*index++ = INDEX_VALUE(r, c + 1);
			}
			else
			{
				// t1
				*index++ = INDEX_VALUE(r, c);
				*index++ = INDEX_VALUE(r + 1, c);
				*index++ = INDEX_VALUE(r + 1, c + 1);
				// t2
				*index++ = INDEX_VALUE(r, c);
				*index++ = INDEX_VALUE(r + 1, c + 1);
				*index++ = INDEX_VALUE(r, c + 1);
			}
		}
	}

	if (left == 1)
	{
		index = &temp_indices[0];

		for (size_t r = 0; r < size; ++r)
		{
			for (size_t c = 0; c < size; ++c)
			{
				if (c == 0)
				{
					if (r < half_size)
					{
						if ((r & 0x1) == 0)
						{
							// 删除一个三角形
							*index++ = INDEX_INVALID;	
							*index++ = INDEX_INVALID;	
							*index++ = INDEX_INVALID;	
							triangle_num--;
							// 不改变
							index += 3;
						}
						else
						{
							// 改变三角形
							*index++ = INDEX_VALUE(r - 1, c);
							*index++ = INDEX_VALUE(r + 1, c);
							*index++ = INDEX_VALUE(r, c + 1);
							// 改变三角形
							*index++ = INDEX_VALUE(r + 1, c);
							*index++ = INDEX_VALUE(r + 1, c + 1);
							*index++ = INDEX_VALUE(r, c + 1);
						}
					}
					else
					{
						if ((r & 0x1) == 0)
						{
							// 改变三角形
							*index++ = INDEX_VALUE(r, c);
							*index++ = INDEX_VALUE(r + 2, c);
							*index++ = INDEX_VALUE(r + 1, c + 1);
							// 改变三角形
							*index++ = INDEX_VALUE(r, c);
							*index++ = INDEX_VALUE(r + 1, c + 1);
							*index++ = INDEX_VALUE(r, c + 1);
						}
						else
						{
							// 删除一个三角形
							*index++ = INDEX_INVALID;	
							*index++ = INDEX_INVALID;	
							*index++ = INDEX_INVALID;	
							triangle_num--;
							// 不改变
							index += 3;
						}
					}
				}
				else
				{
					// 不改变
					index += 6;
				}
			}
		}
	}
	
	if (top == 1)
	{
		index = &temp_indices[0];

		for (size_t r = 0; r < size; ++r)
		{
			for (size_t c = 0; c < size; ++c)
			{
				if (r == 0)
				{
					if (c < half_size)
					{
						if ((c & 0x1) == 0)
						{
							// 不改变
							index += 3;
							// 删除一个三角形
							*index++ = INDEX_INVALID;	
							*index++ = INDEX_INVALID;	
							*index++ = INDEX_INVALID;	
							triangle_num--;
						}
						else
						{
							// 改变三角形
							*index++ = INDEX_VALUE(r, c - 1);
							*index++ = INDEX_VALUE(r + 1, c);
							*index++ = INDEX_VALUE(r, c + 1);
							// 改变三角形
							*index++ = INDEX_VALUE(r + 1, c);
							*index++ = INDEX_VALUE(r + 1, c + 1);
							*index++ = INDEX_VALUE(r, c + 1);
						}
					}
					else
					{
						if ((c & 0x1) == 0)
						{
							// 改变三角形
							*index++ = INDEX_VALUE(r, c);
							*index++ = INDEX_VALUE(r + 1, c);
							*index++ = INDEX_VALUE(r + 1, c + 1);
							// 改变三角形
							*index++ = INDEX_VALUE(r, c);
							*index++ = INDEX_VALUE(r + 1, c + 1);
							*index++ = INDEX_VALUE(r, c + 2);
						}
						else
						{
							// 删除一个三角形
							*index++ = INDEX_INVALID;	
							*index++ = INDEX_INVALID;	
							*index++ = INDEX_INVALID;	
							triangle_num--;
							// 不改变
							index += 3;
						}
					}
				}
				else
				{
					// 不改变
					index += 6;
				}
			}
		}
	}

	if (right == 1)
	{
		index = &temp_indices[0];

		for (size_t r = 0; r < size; ++r)
		{
			for (size_t c = 0; c < size; ++c)
			{
				if (c == (size - 1))
				{
					if (r < half_size)
					{
						if ((r & 0x1) == 0)
						{
							// 不改变
							index += 3;
							// 删除一个三角形
							*index++ = INDEX_INVALID;	
							*index++ = INDEX_INVALID;	
							*index++ = INDEX_INVALID;	
							triangle_num--;
						}
						else
						{
							// 改变三角形
							*index++ = INDEX_VALUE(r, c);
							*index++ = INDEX_VALUE(r + 1, c);
							*index++ = INDEX_VALUE(r + 1, c + 1);
							// 改变三角形
							*index++ = INDEX_VALUE(r, c);
							*index++ = INDEX_VALUE(r + 1, c + 1);
							*index++ = INDEX_VALUE(r - 1, c + 1);
						}
					}
					else
					{
						if ((r & 0x1) == 0)
						{
							// 改变三角形
							*index++ = INDEX_VALUE(r, c);
							*index++ = INDEX_VALUE(r + 1, c);
							*index++ = INDEX_VALUE(r, c + 1);
							// 改变三角形
							*index++ = INDEX_VALUE(r + 1, c);
							*index++ = INDEX_VALUE(r + 2, c + 1);
							*index++ = INDEX_VALUE(r, c + 1);
						}
						else
						{
							// 不改变
							index += 3;
							// 删除一个三角形
							*index++ = INDEX_INVALID;	
							*index++ = INDEX_INVALID;	
							*index++ = INDEX_INVALID;	
							triangle_num--;
						}
					}
				}
				else
				{
					// 不改变
					index += 6;
				}
			}
		}
	}

	if (bottom == 1)
	{
		index = &temp_indices[0];

		for (size_t r = 0; r < size; ++r)
		{
			for (size_t c = 0; c < size; ++c)
			{
				if (r == (size - 1))
				{
					if (c < half_size)
					{
						if ((c & 0x1) == 0)
						{
							// 不改变
							index += 3;
							// 删除一个三角形
							*index++ = INDEX_INVALID;	
							*index++ = INDEX_INVALID;	
							*index++ = INDEX_INVALID;	
							triangle_num--;
						}
						else
						{
							// 改变三角形
							*index++ = INDEX_VALUE(r, c);
							*index++ = INDEX_VALUE(r + 1, c - 1);
							*index++ = INDEX_VALUE(r + 1, c + 1);
							// 改变三角形
							*index++ = INDEX_VALUE(r, c);
							*index++ = INDEX_VALUE(r + 1, c + 1);
							*index++ = INDEX_VALUE(r, c + 1);
						}
					}
					else
					{
						if ((c & 0x1) == 0)
						{
							// 改变三角形
							*index++ = INDEX_VALUE(r, c);
							*index++ = INDEX_VALUE(r + 1, c);
							*index++ = INDEX_VALUE(r, c + 1);
							// 改变三角形
							*index++ = INDEX_VALUE(r, c + 1);
							*index++ = INDEX_VALUE(r + 1, c);
							*index++ = INDEX_VALUE(r + 1, c + 2);
						}
						else
						{
							// 删除一个三角形
							*index++ = INDEX_INVALID;	
							*index++ = INDEX_INVALID;	
							*index++ = INDEX_INVALID;	
							triangle_num--;
							// 不改变
							index += 3;
						}
					}
				}
				else
				{
					// 不改变
					index += 6;
				}
			}
		}
	}

	unsigned short* indices = (unsigned short*)CORE_ALLOC(
		sizeof(unsigned short) * triangle_num * 3);
	unsigned short* p = indices;

	for (size_t i = 0; i < temp_indices.size(); ++i)
	{
		if (temp_indices[i] != INDEX_INVALID)
		{
			unsigned int row = (temp_indices[i] >> 16) & 0xFFFF;
			unsigned int col = temp_indices[i] & 0xFFFF;
			
			*p++ = (unsigned short)(row * size1 + col);
		}
	}
	
	m_nTriangleNum = triangle_num;
	m_pIndices = indices;

	// 生成碰撞区块
	size_t collide_size = scale / collide_scale;

	for (size_t cr = 0; cr < collide_size; ++cr)
	{
		for (size_t cc = 0; cc < collide_size; ++cc)
		{
			collide_info_t* ci = CORE_NEW(collide_info_t);

			ci->nLeft = (int)cc * collide_scale;
			ci->nTop = (int)cr * collide_scale;
			ci->nRight = ci->nLeft + collide_scale;
			ci->nBottom = ci->nTop + collide_scale;

			m_Collides.push_back(ci);
		}
	}

	// 将三角形添加到碰撞区块中
	for (size_t k = 0; k < temp_indices.size(); k += 3)
	{
		if (temp_indices[k] == INDEX_INVALID)
		{
			continue;
		}

		triangle_info_t ti;
		
		ti.nVertexRow1 = (temp_indices[k] >> 16) & 0xFFFF;
		ti.nVertexCol1 = temp_indices[k] & 0xFFFF;
		ti.nVertexRow2 = (temp_indices[k + 1] >> 16) & 0xFFFF;
		ti.nVertexCol2 = temp_indices[k + 1] & 0xFFFF;
		ti.nVertexRow3 = (temp_indices[k + 2] >> 16) & 0xFFFF;
		ti.nVertexCol3 = temp_indices[k + 2]& 0xFFFF;
		ti.nVertexRow1 <<= level;
		ti.nVertexCol1 <<= level;
		ti.nVertexRow2 <<= level;
		ti.nVertexCol2 <<= level;
		ti.nVertexRow3 <<= level;
		ti.nVertexCol3 <<= level;

		int left1 = ti.nVertexCol1;

		if (ti.nVertexCol2 < left1)
		{
			left1 = ti.nVertexCol2;
		}

		if (ti.nVertexCol3 < left1)
		{
			left1 = ti.nVertexCol3;
		}

		int top1 = ti.nVertexRow1;

		if (ti.nVertexRow2 < top1)
		{
			top1 = ti.nVertexRow2;
		}

		if (ti.nVertexRow3 < top1)
		{
			top1 = ti.nVertexRow3;
		}

		int right1 = ti.nVertexCol1;

		if (ti.nVertexCol2 > right1)
		{
			right1 = ti.nVertexCol2;
		}

		if (ti.nVertexCol3 > right1)
		{
			right1 = ti.nVertexCol3;
		}

		int bottom1 = ti.nVertexRow1;
		
		if (ti.nVertexRow2 > bottom1)
		{
			bottom1 = ti.nVertexRow2;
		}

		if (ti.nVertexRow3 > bottom1)
		{
			bottom1 = ti.nVertexRow3;
		}

		for (size_t c = 0; c < m_Collides.size(); ++c)
		{
			collide_info_t* ci = m_Collides[c];
			
			if (rect_intersect(left1, top1, right1, bottom1, 
				ci->nLeft, ci->nTop, ci->nRight, ci->nBottom))
			{
				ti.nLeft = left1;
				ti.nTop = top1;
				ti.nRight = right1;
				ti.nBottom = bottom1;
				
				ci->triangles.push_back(ti);
			}
		}
	}

	return true;
}

bool CChunkStyle::CreateIndexBuffer(IRender* pRender)
{
	Assert(pRender != NULL);
	Assert(m_pIBuffer == NULL);

	IStaticIB* pIBuffer = pRender->CreateStaticIB(m_pIndices,
		(unsigned int)m_nTriangleNum * 3 * sizeof(unsigned short));

	if (NULL == pIBuffer)
	{
		CORE_TRACE("(CChunkStyle::CreateIndexBuffer)create failed");
		return false;
	}

	m_pIBuffer = pIBuffer;

	return true;
}

void CChunkStyle::ReleaseIndexBuffer()
{
	SAFE_RELEASE(m_pIBuffer);
}
