//--------------------------------------------------------------------
// �ļ���:		skeleton.h
// ��  ��:		
// ˵  ��:		
// ��������:	2010��7��12��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SKELETON_H
#define _SKELETON_H

#include "../public/macros.h"
#include "../public/core_type.h"

#include "../visual/i_res_base.h"
#include "../visual/i_model_system.h"
#include "skeleton_data.h"

// ��������

class IRender;
class IResLoader;
class CResManager;

class CSkeleton: public IResBase
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
		RES_CATEGORY_SKELETON,	// ģ������
	};
	
	int time;
public:
	static CSkeleton* NewInstance();

public:
	CSkeleton();
	virtual ~CSkeleton();

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

	// ��ù�����������
	skeleton_t* GetSkeletonData() const { return m_pSkeleton; }
	
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
	bool Build(skeleton_t* pSkeleton);

	// ���ù�����
	void SetManager(CResManager* value) { m_pManager = value; }
	
private:
	CSkeleton(const CSkeleton&);
	CSkeleton& operator=(const CSkeleton&);

private:
	CResManager* m_pManager;
	IResLoader* m_pLoader;
	core_string m_strName;
	int m_nState;
	skeleton_t* m_pSkeleton;
};

#endif // _SKELETON_H
