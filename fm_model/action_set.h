//--------------------------------------------------------------------
// �ļ���:		action_set.h
// ��  ��:		
// ˵  ��:		
// ��������:	2010��7��13��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _ACTION_SET_H
#define _ACTION_SET_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../visual/i_res_base.h"
#include "skeleton_data.h"

// ������

class IRender;
class IResLoader;
class CResManager;
struct action_set_t;

class CActionSet: public IResBase
{
public:
	enum STATE_ENUM
	{
		STATE_INIT,		// ��ʼ״̬
		STATE_LOADING,	// ���ڼ���
		STATE_READY,	// ����
		STATE_FAILED,	// ����ʧ��
	};

	enum
	{
		RES_CATEGORY_ACTION_SET,	// ����������
	};
	
	long time;
public:
	static CActionSet* NewInstance();

public:
	CActionSet();
	virtual ~CActionSet();

	// �ͷ�
	virtual void Release();
	
	// ����
	void Destroy();

	// �Ƿ����
	bool IsReady()
	{
		return (m_nState == STATE_READY);
	}

	// �Ƿ���ؽ������ɹ���ʧ�ܣ�
	bool IsLoadComplete()
	{
		return (m_nState == STATE_READY) || (m_nState == STATE_FAILED);
	}

	// ��ö���������
	action_set_t* GetActionSet() const { return m_pActionSet; }
	
	// ��������
	void SetName(const char* value)
	{ 
		Assert(value != NULL);

		m_strName = value; 
	}

	// �������
	const char* GetName() const
	{
		return m_strName.c_str();
	}

	// ״̬
	void SetState(int value) { m_nState = value; }
	int GetState() const { return m_nState; }

	// ͬ������
	bool Create();
	// �Ƿ�����첽����
	bool CanAsync();
	// ��ʼ�첽���� 
	bool BeginCreate();
	// �����첽���� 
	bool EndCreate(int category, bool succeed);
	// ����첽������
	IResLoader* GetLoader();

	// ����
	bool Build(action_set_t* pActionSet, skeleton_t* pMainSkeleton);

	// �������ֻ�ö���������ֵ
	int GetActionIndex(const char* action_name);
	// ��Ӷ���������
	bool AddActionIndex(const char* action_name, int action_index);
	// �Ƴ�����������
	bool RemoveActionIndex(const char* action_name, int action_index);

	// �Ƴ����������ʹ��ʱ��
	bool RemoveActionLastUse(int action_index);
	// ���¶��������ʹ��ʱ��
	bool UpdateActionLastUse(int action_index);
	// ���ճ�ʱ��δʹ�õĶ�������
	int CollectUnuseAction(unsigned int cur_tick, unsigned int ms);

	// ���ù�����
	void SetManager(CResManager* value) { m_pManager = value; }
	// ��ù�����
	CResManager* GetManager() const { return m_pManager; }
	
	// ���ø�����Դ·����
	void SetAppendPath(const char* value) { m_strAppendPath = value; }
	const char* GetAppendPath() const { return m_strAppendPath.c_str(); }

private:
	CActionSet(const CActionSet&);
	CActionSet& operator=(const CActionSet&);

private:
	CResManager* m_pManager;
	IResLoader* m_pLoader;
	core_string m_strAppendPath;
	core_string m_strName;
	int m_nState;
	action_set_t* m_pActionSet;
};

#endif // _ACTION_SET_H
