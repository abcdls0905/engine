//--------------------------------------------------------------------
// �ļ���:		BrokenShadow.h
// ��  ��:		
// ˵  ��:		
// ��������:	2011��9��13��
// ������:		
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _BROKEN_SHADOW_H
#define _BROKEN_SHADOW_H

#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../utils/array_pod.h"
#include "../visual/i_model_system.h"
#include "../visual/i_model_player.h"
#include "../visual/i_vis_base.h"

// ��ӰЧ��

class BrokenShadow: public IVisBase
{
private:
	struct child_t 
	{
		PERSISTID id;
		skt_pose_t* pose;
	};

	struct bs_info_t
	{
		FmMat4 mat;
		float alpha;
		TArrayPod<child_t, 1, TCoreAlloc> child;
	};

	enum ObiectType
	{
		OBJECT_TYPE_INVALID = 0,
		OBJECT_TYPE_MODEL = 1,
		OBJECT_TYPE_ACTOR = 2,
		OBJECT_TYPE_EFFECTMODEL = 3,
	};

public:
	BrokenShadow();
	virtual ~BrokenShadow();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual void Update(float seconds);
	virtual void Realize();

	// ������Ӱ�Ķ���
	void SetObject(const PERSISTID id);
	PERSISTID GetObjectID() const;

	// ����һ����Ӱ�ļ��ʱ��
	void SetInterval(float value);
	float GetInterval() const;

	// ��Ӱ��������
	void SetLifeTime(float value);
	float GetLifeTime() const;

	// ��Ӱ����
	void SetShadowCount(int value);
	int GetShadowCount();

	// ��ɫ
	void SetColorString(const char* value);
	result_string GetColorString() const;

	// �Ƿ񲥷Ž���
	bool GetComplete() const { return m_bComplete; }

private:
	void AddNode(TArrayPod<child_t, 1, TCoreAlloc>& info, IVisBase* pVisBase);
	void ClearData();
	void ClearData(size_t index);

	void ClearBSInfo()
	{
		for (size_t i = 0; i < m_vBSInfo.size(); ++i)
		{
			CORE_DELETE(m_vBSInfo[i]);
		}

		m_vBSInfo.clear();
	}

private:
	PERSISTID m_ObjectID;				// ������Ӱ�Ķ���
	ObiectType m_ObjectType;			// ��Ӱ��������
	TArrayPod<bs_info_t*, 1, TCoreAlloc> m_vBSInfo;	// ��Ӱ��Ϣ����
	float m_fInterval;					// ����һ�β�Ӱ��ʱ����
	float m_fLastCreateTime;			// ��һ�δ�����Ӱ������ʱ��
	float m_fLifeTime;					// ��Ӱ��������
	int m_nShadowCount;					// ��Ӱ��
	int m_nMaxShadowCount;				// ͬʱ���ڵ�����Ӱ��
	unsigned int m_nColor;				// ��ɫ
	bool m_bComplete;					// �Ƿ񲥷Ž���
};

#endif // _BROKEN_SHADOW_H
