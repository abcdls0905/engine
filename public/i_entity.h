//--------------------------------------------------------------------
// �ļ���:		i_entity.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��1��31��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _PUBLIC_I_ENTITY_H
#define _PUBLIC_I_ENTITY_H

#include "macros.h"
#include "persistid.h"
#include "i_core.h"

// ʵ��ӿ�

class IVarTable;
class IEntInfo;

class IEntity
{
public:
	// �ڲ�������֤�������Ч�ԣ����ɵ���
	static void __validate_parent_exists() {}

public:
	IEntity()
	{
		m_pCore = NULL;
		m_pCustoms = NULL;
		m_pEntInfo = NULL;
		m_nMsgHooks = 0;
		m_bDeleted = false;
	}

	virtual ~IEntity() = 0;

	// ��ʼ�����ɴ�����������ʵ�壩
	virtual bool Init(const IVarList& args) = 0;
	// �رգ���ɾ���������õĸ���ʵ�壬����Դ�ͷ�Ӧ�����������н��У�
	virtual bool Shut() = 0;
	// ÿִ֡���߼����������ӵ����ж��У�
	virtual void Execute(float seconds) {}
	// ������Ϣ����
	virtual bool MsgProc(unsigned int msg, size_t param1, size_t param2, 
		int& result) { return false; }

	// �ͷ�����
	virtual void Release()
	{
		m_pCore->DeleteEntity(GetID());
	}
	
	// ��ú��Ľӿ�
	ICore* GetCore() const
	{
		return m_pCore;
	}

	// ����Զ������Ա�
	IVarTable* GetCustoms() const
	{
		return m_pCustoms;
	}

	// ���ʵ������Ϣ
	IEntInfo* GetEntInfo() const
	{
		return m_pEntInfo;
	}

	// ��ö���ID
	const PERSISTID& GetID() const
	{
		return m_PersistID;
	}

	// �Ƿ���ɾ��
	bool GetDeleted() const
	{
		return m_bDeleted;
	}

private:
	IEntity(const IEntity&);
	IEntity& operator=(const IEntity&);
	
	void SetCore(ICore* value)
	{
		m_pCore = value;
	}

	void SetCustoms(IVarTable* value)
	{
		m_pCustoms = value;
	}
	
	void SetEntInfo(IEntInfo* value)
	{
		m_pEntInfo = value;
	}

	void SetID(const PERSISTID& value)
	{
		m_PersistID = value;
	}

	void SetDeleted(bool value)
	{
		m_bDeleted = value;
	}

	void IncMsgHooks()
	{
		++m_nMsgHooks;
	}

	void DecMsgHooks()
	{
		--m_nMsgHooks;
	}

	int GetMsgHooks() const
	{
		return m_nMsgHooks;
	}

private:
	ICore* m_pCore;
	IVarTable* m_pCustoms;
	IEntInfo* m_pEntInfo;
	int m_nMsgHooks;
	bool m_bDeleted;
	PERSISTID m_PersistID;

	friend class CEntFactory;
};

inline IEntity::~IEntity() {}

#endif // _PUBLIC_I_ENTITY_H
