//--------------------------------------------------------------------
// 文件名:		ent_manager.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年2月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include <new>
#include "ent_manager.h"
#include "ent_info.h"
#include "ent_info_list.h"
#include "core.h"
#include "memory_pool.h"
#include "../public/var_list.h"
#include "../public/auto_mem.h"
#include "../public/portable.h"
#include "../public/i_entity.h"
#include "../public/i_ent_creator.h"

#define performance_time Port_GetPerformanceTime

extern CMemoryPool* g_pMemoryPool;

// CEntManager

CEntManager::CEntManager(CCore* pCore)
{
	Assert(pCore != NULL);

	m_pCore = pCore;
	m_bModified = true;

	if (g_pMemoryPool)
	{
		m_pEntInfoList = (CEntInfoList*)g_pMemoryPool->Alloc(
			sizeof(CEntInfoList));
		new (m_pEntInfoList) CEntInfoList;
	}
	else
	{
		m_pEntInfoList = NEW CEntInfoList;
	}
}

CEntManager::~CEntManager()
{
	for (size_t i = 0; i < m_pEntInfoList->GetCount(); ++i)
	{
		CEntInfo* pEntInfo = m_pEntInfoList->GetByIndex(i);

		if (g_pMemoryPool)
		{
			pEntInfo->~CEntInfo();
			g_pMemoryPool->Free(pEntInfo, sizeof(CEntInfo));
		}
		else
		{
			delete pEntInfo;
		}
	}

	if (g_pMemoryPool)
	{
		m_pEntInfoList->~CEntInfoList();
		g_pMemoryPool->Free(m_pEntInfoList, sizeof(CEntInfoList));
	}
	else
	{
		delete m_pEntInfoList;
	}
}

bool CEntManager::AddCreator(IEntCreator* pCreator)
{
	Assert(pCreator != NULL);

	if (m_pEntInfoList->Get(pCreator->GetName()) != NULL)
	{
		m_pCore->SaveLog("(CEntManager::AddCreator)entity name repeat");
		m_pCore->SaveLog(pCreator->GetName());
	}
	
	CEntInfo* pEntInfo;
	
	if (g_pMemoryPool)
	{
		pEntInfo = (CEntInfo*)g_pMemoryPool->Alloc(sizeof(CEntInfo));
		new (pEntInfo) CEntInfo;
	}
	else
	{
		pEntInfo = NEW CEntInfo;
	}

	pEntInfo->SetCreator(pCreator);
	pEntInfo->SetParentName(pCreator->GetParent());
	pEntInfo->SetEntityName(pCreator->GetName());
	pEntInfo->SetEntityClassID(pCreator->GetClassID());
	pEntInfo->AddPropertyLink(pCreator->GetPropertyLink());

	m_pEntInfoList->Add(pEntInfo);

	return true;
}

void CEntManager::SetAllParentInfo()
{
	const size_t ent_info_num = m_pEntInfoList->GetCount();
	
	for (size_t i = 0; i < ent_info_num; ++i)
	{
		CEntInfo* pEntInfo = m_pEntInfoList->GetByIndex(i);
		const char* parent_name = pEntInfo->GetParentName();
		
		if (strcmp(parent_name, "IEntity") == 0)
		{
			continue;
		}
		
		CEntInfo* pParentInfo = GetEntInfo(parent_name);

		if (NULL == pParentInfo)
		{
			// 在更大的范围内查找父类
			pParentInfo = GetEntInfo(parent_name);
		}
		
		if (pParentInfo != NULL)
		{
			pEntInfo->SetParent(pParentInfo);
		}
		else
		{
			// 父类不存在
			m_pCore->SaveLog(
				"(CEntManager::SetAllParentInfo)parent not exists");
			m_pCore->SaveLog(parent_name);
			m_pCore->SaveLog(pEntInfo->GetEntityName());
		}
	}
}

size_t CEntManager::GetCount() const
{
	return m_pEntInfoList->GetCount();
}

CEntInfo* CEntManager::GetEntInfoByIndex(size_t index) const
{
	return m_pEntInfoList->GetByIndex(index);
}

CEntInfo* CEntManager::GetEntInfo(const char* name) const
{
	return m_pEntInfoList->Get(name);
}

bool CEntManager::AddFrameExecute(IEntity* pEntity)
{
	Assert(pEntity != NULL);
	
	m_FrameExecute.push_back(pEntity);
	m_bModified = true;

	return true;
}

bool CEntManager::RemoveFrameExecute(IEntity* pEntity)
{
	Assert(pEntity != NULL);
	
	const size_t size = m_FrameExecute.size();

	for (size_t i = 0; i < size; ++i)
	{
		if (m_FrameExecute[i] == pEntity)
		{
			m_FrameExecute.remove(i);
			m_bModified = true;
			return true;
		}
	}
	
	return false;
}

void CEntManager::ExecuteFrame(float seconds)
{
	double dTime = performance_time();

	if (m_bModified)
	{
		// 更新运行队列
		size_t new_size = m_FrameExecute.size();

		m_CurrentExecute.resize(new_size, NULL);

		if (new_size > 0)
		{
			memcpy(&m_CurrentExecute[0], &m_FrameExecute[0], 
				sizeof(IEntity*) * new_size);
		}

		m_bModified = false;
	}

	size_t size = m_CurrentExecute.size();
	IEntity** pEntities = m_CurrentExecute.data();

	for (size_t i = 0; i < size; ++i)
	{
		pEntities[i]->Execute(seconds);
	}

	m_pCore->m_CorePerformance.dExecuteFuncTime += performance_time() - dTime;
}

bool CEntManager::Dump(const char* file_name)
{
	FILE* fp = Port_FileOpen(file_name, "wb");
	
	if (NULL == fp)
	{
		return false;
	}
	
	const size_t ent_info_num = m_pEntInfoList->GetCount();
	
	for (size_t i = 0; i < ent_info_num; ++i)
	{
		CEntInfo* pEntInfo = m_pEntInfoList->GetByIndex(i);
		
		fprintf(fp, 
			"Entity: %s, Parent: %s, Property: %d\r\n", 
			pEntInfo->GetEntityName(), pEntInfo->GetParentName(), 
			pEntInfo->GetPropertyCount());

		CVarList res;
		size_t prop_count = pEntInfo->GetPropertyList(res);

		for (size_t k = 0; k < prop_count; ++k)
		{
			fprintf(fp, "\tProperty: %s\r\n", res.StringVal(k));
		}
	}
	
	fprintf(fp, "\r\ntotal %d entity informations\r\n", ent_info_num);
	fclose(fp);
	
	return true;
}
