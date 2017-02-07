//--------------------------------------------------------------------
// �ļ���:		quad_collide.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��1��16��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _QUAD_COLLIDE_H
#define _QUAD_COLLIDE_H

#include "../public/macros.h"


#include "../public/i_interface.h"
#include "../visual/i_render.h"

// ������ײ

class CQuadVisual;
class CQuadTree;
class CQuadNode;
class Terrain;
class CZoneManager;

class CQuadCollide
{
public:
	CQuadCollide();
	~CQuadCollide();

	CQuadTree* GetQuadTree() const { return m_pQuadTree; }

	void SetTerrain(Terrain* pTerrain);

	// ��ʼ��
	bool Init();

	// ��ײ�����ϸ��Ϣ
	bool TraceDetail(const FmVec3& src, const FmVec3& dst,
		trace_info_t& result);
	bool TraceAll(const FmVec3& src, const FmVec3& dst,
		trace_info_t& result);
	// ��ײ�����Ƿ����赲
	bool TraceHitted(const FmVec3& src, const FmVec3& dst);
	// ���巶Χ��ײ
	bool TraceSphere(const FmVec3& center, float radius);
	// ��ѡ
	bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context);

	// ѹ��
	bool Compress();

private:
	CQuadCollide(const CQuadCollide&);
	CQuadCollide& operator=(const CQuadCollide&);

	bool TraceNode(CQuadNode* node, const FmVec3& src, 
		const FmVec3& dst, const FmVec3& dir, trace_info_t* result);
	bool TraceLeaf(CQuadNode* node, const FmVec3& src, 
		const FmVec3& dst, const FmVec3& dir, trace_info_t* result);
	bool TraceSphereNode(CQuadNode* node, const FmVec3& center, 
		float radius, unsigned int trace_mask);
	bool CullNode(CQuadNode* node, const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context);

private:
	Terrain* m_pTerrain;
	CQuadTree* m_pQuadTree;
	bool m_bFound;
	float m_fDistance;
	CQuadVisual* m_pProcessed;
};

#endif // _QUAD_COLLIDE_H

