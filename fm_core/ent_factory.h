//--------------------------------------------------------------------
// �ļ���:		ent_factory.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��2��1��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _ENT_FACTORY_H
#define _ENT_FACTORY_H

#include "../public/macros.h"
#include "../public/persistid.h"
#include "../public/i_var_list.h"
#include "../public/i_entity.h"
#include "../utils/array_pod.h"

// ʵ�幤��

class CCore;
class CHookManager;
class CEntManager;
class IEntCreator;

class CEntFactory
{
private:
	struct entity_t
	{
		PERSISTID EntityId;
		IEntity* pEntity[2];

		entity_t()
		{
			pEntity[0] = NULL;
			pEntity[1] = NULL;
		}
	};

public:
	explicit CEntFactory(CCore* pCore, CEntManager* pEntManager, 
		CHookManager* pHookManager);
	~CEntFactory();

	// ����ʵ��
	IEntity* Create(const char* name, const IVarList& args);
	// ����ʵ��
	void Destroy(IEntity* pEntity);
	// ��ʵ����Ϊɾ��
	void SetDeleted(IEntity* pEntity);
	
	// ��Ӵ�����Ϣ����
	bool AddMsgProc(IEntity* pEntity, unsigned int msg, int prior, 
		bool instant);
	// �Ƴ�������Ϣ����
	bool RemoveMsgProc(IEntity* pEntity, unsigned int msg);

	// ���ݶ���ID���ʵ��
	IEntity* Get(const PERSISTID& id) const
	{
		// ʡȥ�߽��ж�
		size_t index = id.nIdent & m_nBoundSize;
		bool not_equal = m_Entities[index].EntityId != id;
		return m_Entities[index].pEntity[size_t(not_equal)];
	}

	// �������ʵ��
	size_t GetAll(IVarList& result) const;
	// ���ʵ������
	size_t GetCount() const { return m_nCount; }
	// �������ַ��ϵ�ʵ��
	IEntity* Find(const char* name) const;
	size_t FindMore(const char* name, IVarList& result) const;
	// ���ٱ��Ϊɾ����ʵ��
	void ClearDeleted();
	// ȫ��ɾ��
	void ReleaseAll();
	// ��������ʵ����Ϣ���ļ�
	bool Dump(const char* file_name);
	
private:
	CEntFactory();
	CEntFactory(const CEntFactory&);
	CEntFactory& operator=(const CEntFactory&);

	// ɾ��ʵ��
	void Delete(IEntity* pEntity);
	// �������к�
	unsigned int GenSerial();

private:
	CCore* m_pCore;
	CEntManager* m_pEntManager;
	CHookManager* m_pHookManager;
	TArrayPod<entity_t, 1> m_Entities;
	TArrayPod<unsigned int, 1> m_FreeList;
	TArrayPod<IEntity*, 1> m_Deletes;
	unsigned int m_nSerial;
	size_t m_nCount;
	size_t m_nUsedSize;
	size_t m_nBoundSize;
};

#endif // _ENT_FACTORY_H
