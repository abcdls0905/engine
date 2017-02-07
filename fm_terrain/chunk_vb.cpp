//--------------------------------------------------------------------
// 文件名:		chunk_vb.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "chunk_vb.h"

CChunkVB::CChunkVB()
{
	m_nVertexCount = 0;
	m_pVertexBuf = NULL;
	m_nLevel = 0;
	//m_pNext = NULL;
}

CChunkVB::~CChunkVB()
{
	SAFE_RELEASE(m_pVertexBuf);
}

bool CChunkVB::Create(IRender* pRender, size_t vertex_count, 
	const void* pdata) 
{
	Assert(pRender != NULL);
	Assert(pdata != NULL);
	Assert(NULL == m_pVertexBuf);

	m_pVertexBuf = pRender->CreateStaticVB(pdata, 
		(unsigned int)vertex_count * sizeof(terrain_vertex_t));

	if (NULL == m_pVertexBuf)
	{
		return false;
	}
	
	m_nVertexCount = vertex_count;
	
	return true;
}

void CChunkVB::Destroy()
{
	SAFE_RELEASE(m_pVertexBuf);
}
