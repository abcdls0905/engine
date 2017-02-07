//--------------------------------------------------------------------
// �ļ���:		actor_node.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��2��3��
// ������:		�ݾ���
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _ACTOR_NODE_H
#define _ACTOR_NODE_H

#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../visual/i_model_system.h"
#include "../utils/array_pod.h"
#include "../visual/i_vis_base.h"

// �����ģ�ͽڵ�

class Actor;

class CActorNode
{
public:
	CActorNode();
	~CActorNode();

	// ��ð�Χ��
	bool GetBoundingBox(FmVec3& v_min, FmVec3& v_max);

	// �ͷ����������ӽڵ�
	void ReleaseAll();

	// �������ģ�Ͷ���ָ��
	void SetActor(Actor* value) { m_pActor = value; }

	// ��ýڵ�����
	IVisBase* GetVisBase() const
	{
		return (IVisBase*)g_pCore->GetEntity(m_VisualID);
	}

	void GetWorldMatrix(FmMat4& mat) const;
	
	// ����
	void SetName(const char* value);
	const char* GetName() const { return m_strName.c_str(); }

	// ������
	void SetPoint(const char* value);
	const char* GetPoint() const { return m_strPoint.c_str(); }

	// ��������
	void SetVisualID(const PERSISTID& value);
	PERSISTID GetVisualID() const { return m_VisualID; }

	// ��ȡ��ǰģ���Ƿ������ģ����ģ��
	bool ExistMainModel();
	// ָ����ǰģ�ͽ�����ģ����ģ��������ײ���
	void SetTraceMainModel(bool value);
	// ��ȡ��ǰģ���Ƿ������ģ����ģ��������ײ���
	bool GetTraceMainModel();

	// �����Ƿ���ʾ��Χ��
	void SetShowBoundBox(bool value);

	// ���ò�ӰID
	void SetBrokenShadowID(const PERSISTID& value);
	PERSISTID GetBrokenShadowID() const { return m_BrokenShadow; }

	// ȡ�ô��ڿ��Ӷ���Ľڵ��б�
	void GetVisualNodeList(IVarList& result);

	// ��ÿɼ����� 
	void GetVisualList(IVarList& result);

	// ���ö���������
	bool SetActionPlayer(IActionPlayer* ani_player);

	// ���׽ڵ�
	void SetParentNode(CActorNode* pNode);
	CActorNode* GetParentNode() const { return m_pParentNode; }

	// ����ӽڵ�
	CActorNode* AddChild(const char* name, const char* point, 
		const PERSISTID& id);
	// ����ӽڵ�
	bool ClearChild();
	// ɾ���ӽڵ�
	bool DeleteChild(const char* pName);
	// ����ӽڵ�����
	size_t GetChildCount() const;
	// ����ӽڵ�ʵ�������
	size_t GetVisBaseCount() const;
	// ���ָ�������ӽڵ�
	CActorNode* GetChildByIndex(size_t index) const;
	// ���ָ�����ֵĽڵ�
	CActorNode* GetNode(const char* name);
	
	void Update(float seconds);
	void Realize();
	void RealizeShadowMap();

	// ���¶���λ��
	void UpdateMatrix(IVisBase* pVisBase);
	// ���½ڵ�(�������ӽڵ�)
	void UpdateNode();

	// ���������Ƿ�ɹ�
	bool GetLoaded();
	// �Ƿ���سɹ�
	bool GetLoadFinish();
    // �Ƿ���Ч
    bool IsValid();

	// ����λ��
	bool SetLocalPosition(float x, float y, float z);
	FmVec3 GetLocalPosition();

	// ���ӷ���
	bool SetLocalAngle(float x, float y, float z);
	FmVec3 GetLocalAngle();

	// ��������ϵ��
	bool SetLocalScale(float x, float y, float z); 
	FmVec3 GetLocalScale();

	// ������ˮ��
	bool SetUnderWater(bool value);
	// ����������ٶ�����
	bool SetWriteVelocity(int value);
	// ������ɫ
	bool SetColor(unsigned int value);
	// ���ø���ɫ
	bool SetParentColor(unsigned int value);
	// ����ϸ�ڵȼ�
	bool SetLevelOfDetail(float value);

	// ��ײ������ϸ��Ϣ
	bool TraceDetail(const FmVec3& src, const FmVec3& dst,
		trace_info_t& result);
	// ��ײ�����Ƿ����赲
	bool TraceHitted(const FmVec3& src, const FmVec3& dst);
	// ���巶Χ��ײ
	bool TraceSphere(const FmVec3& center, float radius);
	// ��������ײ
    bool TraceTriangle(const FmVec3& v0, const FmVec3& v1, const FmVec3& v2);
	// ��ѡ
	bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context);

	// �������Ӷ������Ƿ񱣳ְ󶨷���
	void SetAngleKeep(bool value);
	bool GetAngleKeep() const { return m_bAngleKeep; }

	// �����Ƿ��ǲ�����ײ�ͼ��õ�ʵ��
	void SetIsEsseObj(bool value);

	// �����ӽڵ��Ƿ�ɼ�
	void SetNodeVisible(const char* name, bool value);
	bool GetNodeVisible(const char* name);

	// �����Ƿ��ǲ�����ײ�ͼ��õ�ʵ��
	void SetEsseObj(const char* name, bool value);
	bool GetEsseObj(const char* name);

private:
	CActorNode(const CActorNode&);
	CActorNode& operator=(const CActorNode&);

	// ���ָ�����ֵĽڵ�
	CActorNode* GetNode(const char* name, const unsigned int hash);

private:
	Actor* m_pActor;
	core_string m_strName;
	unsigned int m_nHash;
	core_string m_strPoint;
	PERSISTID m_VisualID;
	PERSISTID m_BrokenShadow;
	CActorNode* m_pParentNode;
	FmMat4 m_LocalMatrix;
	FmMat4 m_mtxWorld;
	TArrayPod<CActorNode*, 8, TCoreAlloc> m_Children;
	bool m_bAngleKeep;
	bool m_bEsseObj;
	bool m_bNodeVisible;
	unsigned int m_nColor;
};

#endif // _ACTOR_NODE_H
