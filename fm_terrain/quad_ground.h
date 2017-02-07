//--------------------------------------------------------------------
// �ļ���:		quad_ground.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��1��16��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _QUAD_GROUND_H
#define _QUAD_GROUND_H

#include "../public/macros.h"
#include "chunk_style.h"

// ���������μ���

class Terrain;
class CTerrainChunk;
class CQuadNode;

class CQuadGround
{
public:
	CQuadGround();
	~CQuadGround();

	// ��ʼ��
	bool Init(Terrain* pTerrain, size_t scale, const FmVec3& v_min, 
		const FmVec3& v_max, size_t zone, size_t chunk, size_t index);

	// �Ƿ�������ײ
	bool IsTraceMasked(unsigned int trace_mask);
	// ��ײ������ϸ��Ϣ
	bool TraceDetail(const FmVec3& src, const FmVec3& dir,
		trace_info_t& result);
	// ��ײ�����Ƿ����赲
	bool TraceHitted(const FmVec3& src, const FmVec3& dir);
	// ���巶Χ��ײ
	bool TraceSphere(const FmVec3& center, float radius);
	// ��ѡ
	bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context);

private:
	CQuadGround(const CQuadGround&);
	CQuadGround& operator=(const CQuadGround&);

public:
	FmVec3 m_vMin;
	FmVec3 m_vMax;
	size_t m_nScale;	// �߶�
	Terrain* m_pTerrain;
	size_t m_nZone;		// ��������
	size_t m_nChunk;	// �ֿ�����
	size_t m_nIndex;	// �ڷֿ��е�����
	CQuadNode* m_pNode;
};

#endif // _QUAD_GROUND_H
