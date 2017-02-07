//--------------------------------------------------------------------
// �ļ���:		quad_visual.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��1��16��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _QUAD_VISUAL_H
#define _QUAD_VISUAL_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"


#include "../visual/fm_define.h"
#include "../utils/array_pod.h"

class TQuadVisualAlloc
{
public:
	TQuadVisualAlloc() {}
	~TQuadVisualAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TQuadVisualAlloc& src) {}
};

// ����������ײ

class Terrain;
class IVisBase;
class CQuadNode;

class CQuadVisual
{
public:
	CQuadVisual(Terrain* pTerrain, size_t scale, const FmVec4& v_min, 
		const FmVec4& v_max, IVisBase* pVisBase, const char* name, 
		unsigned int hash, bool is_role, float clip_radius);
	~CQuadVisual();

	// �Ƿ�������ײ
	bool IsTraceMasked(unsigned int trace_mask);
	// ��ײ����
	bool TraceDetail(const FmVec3& src, const FmVec3& dst,
		trace_info_t& result);
	// ��ײ�����Ƿ����赲
	bool TraceHitted(const FmVec3& src, const FmVec3& dst);
	// ���巶Χ��ײ
	bool TraceSphere(const FmVec3& center, float radius);
	// ��ѡ
	bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context);

	// ��¼�����ڵ�
	bool AddNode(CQuadNode* node);
	// �����¼�������ڵ�
	void RemoveNodes();

	// ��ÿɼ������
	const PERSISTID& GetVisualID() const { return m_VisualID; }
	// ����
	const char* GetName() const { return m_strName.c_str(); }
	// ��ϣֵ
	unsigned int GetHash() const { return m_nHash; }
	// �Ƿ�������
	bool GetIsRole() const { return m_bIsRole; }

	// ���þ���
	void SetClipRadius(float value) { m_fClipRadius = value; }
	float GetClipRadius() const { return m_fClipRadius; }
	
private:
	CQuadVisual();
	CQuadVisual(const CQuadVisual&);
	CQuadVisual& operator=(const CQuadVisual&);
		
public:
	FmVec4 m_vMin;
	FmVec4 m_vMax;
	size_t m_nScale;	// �߶�
	bool m_bUsed;
	CQuadVisual* m_pNext;

private:
	Terrain* m_pTerrain;
	core_string m_strName;
	unsigned int m_nHash;
	unsigned int m_nTraceMask;
	PERSISTID m_VisualID;
	bool m_bIsRole;
	float m_fClipRadius;
	TArrayPod<CQuadNode*, 4, TQuadVisualAlloc> m_Nodes;
};

#endif // _QUAD_VISUAL_H
