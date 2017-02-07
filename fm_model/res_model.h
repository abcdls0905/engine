//--------------------------------------------------------------------
// �ļ���:		res_model.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��5��4��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _RESMODEL_H
#define _RESMODEL_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"

#include "../visual/i_res_base.h"
#include "../visual/i_model_system.h"
#include "model_data.h"

// ģ��

class IRender;
class IResLoader;
class CResManager;

class CResModel: public IResBase
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
		RES_CATEGORY_MODEL,	// ģ������
	};
	
	float time;
public:
	static CResModel* NewInstance();

public:
	CResModel();
	virtual ~CResModel();

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

	// ���ģ��
	model_t* GetModelData() const { return m_pModel; }
	
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

	// ��ͼ·��
	void SetTexPaths(const char* value);
	const char* GetTexPaths() const;

	// ������ͼ�Ƿ�������
	bool GetTextureLoadComplete() const;
	// �ͷ�������ͼ
	void ReleaseTextures();

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
	bool Build(model_t* pModel);

	// ���ù�����
	void SetManager(CResManager* value) { m_pManager = value; }
	
private:
	CResModel(const CResModel&);
	CResModel& operator=(const CResModel&);

private:
	CResManager* m_pManager;
	core_string m_strTexPaths;
	core_string m_strName;
	int m_nState;
	IResLoader* m_pLoader;
	model_t* m_pModel;
};

#endif // _RESMODEL_H
