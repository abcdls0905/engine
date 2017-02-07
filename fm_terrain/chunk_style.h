//--------------------------------------------------------------------
// �ļ���:		chunk_style.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��8��9��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TChunkStyleAlloc& src) {}
};

// ���ηֿ����ʽ

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

	// �������������
	size_t GetTriangleCount() const { return m_nTriangleNum; }
	// �������������
	unsigned short* GetIndices() const { return m_pIndices; }
	// �����������
	IStaticIB* GetIndexBuf() const { return m_pIBuffer; }
	
	// ������ʽ
	bool Generate(size_t scale, int level, int type, int collide_scale);

	// ������������
	bool CreateIndexBuffer(IRender* pRender);
	// �ͷ���������
	void ReleaseIndexBuffer();

	// ���ָ���������ײ��Ϣ
	collide_info_t* GetCollideInfo(size_t index)
	{
		Assert(index < m_Collides.size());

		return m_Collides[index];
	}

private:
	CChunkStyle(const CChunkStyle&);
	CChunkStyle& operator=(const CChunkStyle&);

private:
	size_t m_nTriangleNum;			// ����������
	unsigned short* m_pIndices;		// ������������
	IStaticIB* m_pIBuffer;		// ������������
	TArrayPod<collide_info_t*, 1, TChunkStyleAlloc> m_Collides;
};

#endif // _CHUNK_STYLE_H
