//--------------------------------------------------------------------
// 文件名:		chunk_vb.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _CHUNK_VB_H
#define _CHUNK_VB_H

#include "../public/macros.h"


#include "../public/i_interface.h"
#include "../visual/i_render.h"
#include "../visual/i_resource.h"

// 地形分块的顶点缓冲

class CChunkVB
{
public:
	struct terrain_vertex_t
	{
		float x;
		float y;
		float z;
		FmVec4 diffuse_normal;
	};

public:
	CChunkVB();
	~CChunkVB();
	
	// 创建
	bool Create(IRender* pRender, size_t vertex_count, const void* pdata);
	// 释放
	void Destroy();

	// 级别
	void SetLevel(int value) { m_nLevel = value; }
	int GetLevel() const { return m_nLevel; }
	
	// 链表
	//void SetNext(CChunkVB* value) { m_pNext = value; }
	//CChunkVB* GetNext() const { return m_pNext; }
	
	// 获得顶点数量
	size_t GetVertexCount() const { return m_nVertexCount; }
	// 获得顶点缓冲
	IStaticVB* GetVertexBuf() const { return m_pVertexBuf; }
	
private:
	CChunkVB(const CChunkVB&);
	CChunkVB& operator=(const CChunkVB&);
	
private:
	size_t m_nVertexCount;
	IStaticVB* m_pVertexBuf;
	int m_nLevel;
	//CChunkVB* m_pNext;
};

#endif // _CHUNK_VB_H
