//--------------------------------------------------------------------
// �ļ���:		ent_factory.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2007��2��1��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "ent_factory.h"
#include "ent_manager.h"
#include "ent_info.h"
#include "hook_manager.h"
#include "core.h"
#include "memory_pool.h"
#include "../public/var_table.h"
#include "../public/var.h"
#include "../public/portable.h"
#include "../public/i_ent_creator.h"

extern bool g_bClosing;
// �ű�ʹ�õ��ڴ��
extern CMemoryPool* g_pMemoryPool;


#define performance_time Port_GetPerformanceTime

// �ڴ���ڴ������

class CEntFactoryAlloc
{
public:
	CEntFactoryAlloc() {}
	~CEntFactoryAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return g_pMemoryPool->Alloc(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { g_pMemoryPool->Free(ptr, size); }
	// ����
	void Swap(CEntFactoryAlloc& src) {}
};

typedef TVarTable<CEntFactoryAlloc> pool_var_table_t;

// CEntFactory

CEntFactory::CEntFactory(CCore* pCore, CEntManager* pEntManager, 
	CHookManager* pHookManager)
{
	Assert(pCore != NULL);
	Assert(pEntManager != NULL);
	Assert(pHookManager != NULL);
	
	m_pCore = pCore;
	m_pEntManager = pEntManager;
	m_pHookManager = pHookManager;
	m_nSerial = 0;
	m_nCount = 0;
	m_nUsedSize = 1;	// ��ʹ��λ��0
	m_nBoundSize = 1;
	m_Entities.resize(2, entity_t());
}

CEntFactory::~CEntFactory()
{
	ReleaseAll();
}

void CEntFactory::ReleaseAll()
{
	int size = (int)m_Entities.size();

	// �������Ա�֤���ɾ����ʵ��
	for (int i = size - 1; i >= 0; --i)
	{
		if (m_Entities[i].pEntity[0] != NULL)
		{
			// ������Shut����
			m_Entities[i].pEntity[0]->SetDeleted(true);
			Destroy(m_Entities[i].pEntity[0]);
		}
	}

	m_Entities.clear();
	m_nUsedSize = 1;	// ��ʹ��λ��0
	m_nBoundSize = 1;
	m_Entities.resize(2, entity_t());
	m_FreeList.clear();
	m_nCount = 0;
	ClearDeleted();
}

unsigned int CEntFactory::GenSerial()
{
	if (++m_nSerial == 0)
	{
		return ++m_nSerial & 0x7FFFFFFF;
	}
	else
	{
		return m_nSerial & 0x7FFFFFFF;
	}
}

IEntity* CEntFactory::Create(const char* name, const IVarList& args)
{
	Assert(name != NULL);

	CEntInfo* pEntInfo = m_pEntManager->GetEntInfo(name);

	if (NULL == pEntInfo)
	{
		m_pCore->SaveLog("(CEntFactory::Create)class %s not found", name);
		return NULL;
	}

	if (pEntInfo->GetCreator()->IsAbstract())
	{
		// �Ǵ������
		m_pCore->SaveLog("(CEntFactory::Create)class %s is abstract", name);
		return NULL;
	}

	IEntity* pEntity = pEntInfo->GetCreator()->Create();

	if (NULL == pEntity)
	{
		return NULL;
	}
	
	size_t index;

	if (m_FreeList.empty())
	{
		if (m_nUsedSize == m_Entities.size())
		{
			// ��֤�����ĳ�����2��N�η�
			m_Entities.resize(m_nUsedSize * 2, entity_t());
			m_nBoundSize = m_Entities.size() - 1;
		}

		Assert(m_nUsedSize < m_Entities.size());

		index = m_nUsedSize;
		m_Entities[index].pEntity[0] = pEntity;
		m_nUsedSize++;
	}
	else
	{
		index = m_FreeList.back();
		m_FreeList.pop_back();
		m_Entities[index].pEntity[0] = pEntity;
	}
	
	++m_nCount;

	unsigned int serial = GenSerial();

	pEntity->SetID(PERSISTID((unsigned int)index, serial));
	pEntity->SetCore(m_pCore);
	pEntity->SetEntInfo(pEntInfo);

	IVarTable* pCustoms;

	if (g_pMemoryPool)
	{
		pCustoms = (pool_var_table_t*)g_pMemoryPool->Alloc(
			sizeof(pool_var_table_t));
		new (pCustoms) pool_var_table_t;
	}
	else
	{
		pCustoms = NEW TVarTable<>;
	}

	pEntity->SetCustoms(pCustoms);
	m_Entities[index].EntityId = pEntity->GetID();

	double dTime = performance_time();

	if (!pEntity->Init(args))
	{
		m_pCore->GetPerformance()->dInitFuncTime += performance_time() - dTime;
		// ��ʼ��ʧ��
		m_pCore->SaveLog("(CEntFactory::Create)class %s init failed", name);
		Destroy(pEntity);
		return NULL;
	}
	m_pCore->GetPerformance()->dInitFuncTime += performance_time() - dTime;

	return pEntity;
}

void CEntFactory::Destroy(IEntity* pEntity)
{
	Assert(pEntity != NULL);

	if (!pEntity->GetDeleted())
	{
		Delete(pEntity);
	}
	
	IVarTable* pCustoms = pEntity->GetCustoms();
	IEntCreator* pCreator = pEntity->GetEntInfo()->GetCreator();

	pCreator->Destroy(pEntity);
	
	if (g_pMemoryPool)
	{
		((pool_var_table_t*)pCustoms)->~pool_var_table_t();
		g_pMemoryPool->Free(pCustoms, sizeof(pool_var_table_t));
	}
	else
	{
		delete pCustoms;
	}
}

void CEntFactory::Delete(IEntity* pEntity)
{
	Assert(pEntity != NULL);
	Assert(!pEntity->GetDeleted());

	size_t index = pEntity->GetID().nIdent;

	Assert(index < m_Entities.size());
	Assert(m_Entities[index].pEntity[0] == pEntity);
	
	double dTime = performance_time();
	pEntity->Shut();
	m_pCore->GetPerformance()->dShutFuncTime += performance_time() - dTime;

	pEntity->SetDeleted(true);
	m_pEntManager->RemoveFrameExecute(pEntity);

	// �����δɾ������Ϣ����
	if (pEntity->GetMsgHooks() > 0)
	{
		m_pHookManager->RemoveAll(pEntity);
	}

	m_Entities[index].pEntity[0] = NULL;
	m_Entities[index].EntityId = PERSISTID();
	m_FreeList.push_back(index);
	--m_nCount;
}

void CEntFactory::SetDeleted(IEntity* pEntity)
{
	Assert(pEntity != NULL);

	if (g_bClosing)
	{
		// ��������ϵͳ�ر�ʱɾ��ʵ��
		Assert(0);
		abort();
	}

	Delete(pEntity);
	m_Deletes.push_back(pEntity);
}

bool CEntFactory::AddMsgProc(IEntity* pEntity, unsigned int msg, int prior, 
	bool instant)
{
	Assert(pEntity != NULL);

	if (!m_pHookManager->AddHook(pEntity, msg, prior, instant))
	{
		return false;
	}

	pEntity->IncMsgHooks();

	return true;
}

bool CEntFactory::RemoveMsgProc(IEntity* pEntity, unsigned int msg)
{
	Assert(pEntity != NULL);

	if (!m_pHookManager->RemoveHook(pEntity, msg))
	{
		return false;
	}

	pEntity->DecMsgHooks();

	return true;
}

size_t CEntFactory::GetAll(IVarList& result) const
{
	result.Clear();
	
	const size_t entity_size = m_Entities.size();
	
	for (size_t i = 0; i < entity_size; ++i)
	{
		IEntity* pEntity = m_Entities[i].pEntity[0];

		if (NULL == pEntity)
		{
			continue;
		}
		
		result.AddObject(pEntity->GetID());
	}
	
	return result.GetCount();
}

IEntity* CEntFactory::Find(const char* name) const
{
	Assert(name != NULL);

	const size_t entity_size = m_Entities.size();
	
	for (size_t i = 0; i < entity_size; ++i)
	{
		IEntity* pEntity = m_Entities[i].pEntity[0];

		if (NULL == pEntity)
		{
			continue;
		}

		IEntInfo* pEntInfo = pEntity->GetEntInfo();

		if (strcmp(pEntInfo->GetEntityName(), name) == 0)
		{
			return pEntity;
		}
	}
	
	return NULL;
}

size_t CEntFactory::FindMore(const char* name, IVarList& result) const
{
	Assert(name != NULL);
	
	result.Clear();

	const size_t entity_size = m_Entities.size();
	
	for (size_t i = 0; i < entity_size; ++i)
	{
		IEntity* pEntity = m_Entities[i].pEntity[0];

		if (NULL == pEntity)
		{
			continue;
		}
		
		IEntInfo* pEntInfo = pEntity->GetEntInfo();
		
		if (strcmp(pEntInfo->GetEntityName(), name) == 0)
		{
			result.AddObject(pEntity->GetID());
		}
	}
	
	return result.GetCount();
}

void CEntFactory::ClearDeleted()
{
	if (m_Deletes.empty())
	{
		return;
	}

	// ɾ��ʵ��ʱ���ܸı�����
	for (size_t i = 0; i < m_Deletes.size(); ++i)
	{
		Destroy(m_Deletes[i]);
	}

	m_Deletes.clear();
}

bool CEntFactory::Dump(const char* file_name)
{
	Assert(file_name != NULL);
	
	FILE* fp = Port_FileOpen(file_name, "wb");
	
	if (NULL == fp)
	{
		return false;
	}
	
	size_t count = 0;
	const size_t entity_size = m_Entities.size();
	
	for (size_t i = 0; i < entity_size; ++i)
	{
		IEntity* p = m_Entities[i].pEntity[0];
		
		if (NULL == p)
		{
			continue;
		}
		
		fprintf(fp, "entity:%s, class id:%d, id:%u-%u", 
			p->GetEntInfo()->GetEntityName(), 
			p->GetEntInfo()->GetEntityClassID(),
			p->GetID().nIdent, p->GetID().nSerial);

		// �ڲ���������
		CVar prop;

		if (m_pCore->GetProperty(p, "Name", prop))
		{
			fprintf(fp, ", property Name:%s", prop.StringVal());
		}

		// �Զ�����������
		IVar* pName = p->GetCustoms()->GetValue("name");

		if (pName)
		{
			fprintf(fp, ", custom name:%s", pName->StringVal());
		}

		fprintf(fp, "\r\n");
		count++;
	}

	fprintf(fp, "\r\ntotal %d entities\r\n", count);
	fclose(fp);
	
	return true;
}
