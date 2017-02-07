//--------------------------------------------------------------------
// �ļ���:		chunk_vb.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��8��9��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _CHUNK_VB_H
#define _CHUNK_VB_H

#include "../public/macros.h"


#include "../public/i_interface.h"
#include "../visual/i_render.h"
#include "../visual/i_resource.h"

// ���ηֿ�Ķ��㻺��

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
	
	// ����
	bool Create(IRender* pRender, size_t vertex_count, const void* pdata);
	// �ͷ�
	void Destroy();

	// ����
	void SetLevel(int value) { m_nLevel = value; }
	int GetLevel() const { return m_nLevel; }
	
	// ����
	//void SetNext(CChunkVB* value) { m_pNext = value; }
	//CChunkVB* GetNext() const { return m_pNext; }
	
	// ��ö�������
	size_t GetVertexCount() const { return m_nVertexCount; }
	// ��ö��㻺��
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
