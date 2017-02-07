//--------------------------------------------------------------------
// 文件名:		chunk_style.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _CHUNK_STYLE_H
#define _CHUNK_STYLE_H

#include "../public/macros.h"
#include "../public/core_mem.h"


#include "../public/i_interface.h"
#include "../visual/i_resource.h"
#include "../utils/array_pod.h"

class TChunkStyleAlloc
{
public:
	TChunkStyleAlloc() {}
	~TChunkStyleAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TChunkStyleAlloc& src) {}
};

// 地形分块的样式

class IRender;
class IStaticIB;

class CChunkStyle
{
public:
	struct triangle_info_t
	{
		int nLeft;
		int nTop;
		int nRight;
		int	nBottom;
		int nVertexRow1;
		int nVertexCol1;
		int nVertexRow2;
		int nVertexCol2;
		int nVertexRow3;
		int nVertexCol3;
	};

	struct collide_info_t
	{
		int nLeft;
		int nTop;
		int nRight;
		int	nBottom;
		TArrayPod<triangle_info_t, 1, TChunkStyleAlloc> triangles;
	};

public:
	CChunkStyle();
	~CChunkStyle();

	// 获得三角形数量
	size_t GetTriangleCount() const { return m_nTriangleNum; }
	// 获得三角形索引
	unsigned short* GetIndices() const { return m_pIndices; }
	// 获得索引缓冲
	IStaticIB* GetIndexBuf() const { return m_pIBuffer; }
	
	// 生成样式
	bool Generate(size_t scale, int level, int type, int collide_scale);

	// 生成索引缓冲
	bool CreateIndexBuffer(IRender* pRender);
	// 释放索引缓冲
	void ReleaseIndexBuffer();

	// 获得指定区块的碰撞信息
	collide_info_t* GetCollideInfo(size_t index)
	{
		Assert(index < m_Collides.size());

		return m_Collides[index];
	}

private:
	CChunkStyle(const CChunkStyle&);
	CChunkStyle& operator=(const CChunkStyle&);

private:
	size_t m_nTriangleNum;			// 三角形数量
	unsigned short* m_pIndices;		// 顶点索引数据
	IStaticIB* m_pIBuffer;		// 顶点索引缓冲
	TArrayPod<collide_info_t*, 1, TChunkStyleAlloc> m_Collides;
};

#endif // _CHUNK_STYLE_H
