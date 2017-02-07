//--------------------------------------------------------------------
// 文件名:		int_manager.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年2月8日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "int_manager.h"
#include "core.h"
#include "../public/inlines.h"
#include "../public/portable.h"
#include "../public/i_interface.h"
#include "../public/i_int_creator.h"

// CIntManager

#define performance_time Port_GetPerformanceTime


CIntManager::CIntManager(CCore* pCore)
{
	Assert(pCore != NULL);

	m_pCore = pCore;
	m_nOrderCount = 0;
}

CIntManager::~CIntManager()
{
	ReleaseAll();
}

int CIntManager::SortCompare(const void* elem1, const void* elem2)
{
	int_creator_t* p1 = (int_creator_t*)elem1;
	int_creator_t* p2 = (int_creator_t*)elem2;

	if (p1->nOrder < p2->nOrder)
	{
		return 1;
	}

	if (p1->nOrder > p2->nOrder)
	{
		return -1;
	}
	
	return 0;
}

void CIntManager::ReleaseAll()
{
	if (! m_IntCreators.empty())
	{
		// 按引用顺序进行排序
		qsort(&m_IntCreators[0], m_IntCreators.size(), sizeof(int_creator_t),
			SortCompare);
		
		// 调用功能接口的关闭函数
		for (size_t k = 0; k < m_IntCreators.size(); ++k)
		{
			if (m_IntCreators[k].pInterface != NULL)
			{
				Assert(m_IntCreators[k].nRefCount > 0);

				m_IntCreators[k].pInterface->Shut();
			}
		}

		// 删除功能接口
		for (size_t i = 0; i < m_IntCreators.size(); ++i)
		{
			if (m_IntCreators[i].pInterface != NULL)
			{
				Assert(m_IntCreators[i].nRefCount > 0);

				m_IntCreators[i].pCreator->Destroy(
					m_IntCreators[i].pInterface);
			}
		}
	
		m_IntCreators.clear();
	}
}

bool CIntManager::AddCreator(IIntCreator* pCreator)
{
	Assert(pCreator != NULL);
	
	if (Find(pCreator->GetName()))
	{
		m_pCore->SaveLog("(CIntManager::AddCreator)interface name repeat");
		m_pCore->SaveLog(pCreator->GetName());
	}

	m_IntCreators.push_back(int_creator_t());

	int_creator_t& data = m_IntCreators.back();
	
	data.pCreator = pCreator;
	data.nHash = GetHashValueCase(pCreator->GetName());
	data.nRefCount = 0;
	data.nOrder = 0;
	data.pInterface = NULL;

	return true;
}

bool CIntManager::Find(const char* name) const
{
	size_t index;

	return FindIndex(name, index);
}

bool CIntManager::FindIndex(const char* name, size_t& index) const
{
	Assert(name != NULL);
	
	unsigned int hash = GetHashValueCase(name);
	const size_t int_creator_num = m_IntCreators.size();
	
	for (size_t i = 0; i < int_creator_num; ++i)
	{
		if ((m_IntCreators[i].nHash == hash)
			&& (strcmp(m_IntCreators[i].pCreator->GetName(), name) == 0))
		{
			index = i;
			return true;
		}
	}
	
	return false;
}

size_t CIntManager::GetCount() const
{
	return m_IntCreators.size();
}

const char* CIntManager::GetName(size_t index) const
{
	Assert(index < m_IntCreators.size());
	
	return m_IntCreators[index].pCreator->GetName();
}

IIntCreator* CIntManager::GetCreator(size_t index) const
{
	Assert(index < m_IntCreators.size());

	return m_IntCreators[index].pCreator;
}

IInterface* CIntManager::GetInterface(size_t index) const
{
	Assert(index < m_IntCreators.size());
	
	return m_IntCreators[index].pInterface;
}

int CIntManager::GetRefCount(size_t index) const
{
	Assert(index < m_IntCreators.size());
	
	return m_IntCreators[index].nRefCount;
}

IInterface* CIntManager::GetReference(const char* name)
{
	size_t index;
	
	if (!FindIndex(name, index))
	{
		return NULL;
	}
	
	if (m_IntCreators[index].nRefCount == 0)
	{
		Assert(m_IntCreators[index].pInterface == NULL);
		
		IIntCreator* pCreator = m_IntCreators[index].pCreator;
		IInterface* pInterface = pCreator->Create();
		
		pInterface->SetCore(m_pCore);
		pInterface->SetCreator(pCreator);
		
		m_IntCreators[index].pInterface = pInterface;
		m_IntCreators[index].nRefCount = 1;
		
		double dTime = performance_time();
		pInterface->Init();
		m_pCore->GetPerformance()->dInitFuncTime += performance_time() - dTime;

		m_IntCreators[index].nOrder = ++m_nOrderCount;
		
		if (pInterface->NeedExecPerFrame())
		{
			AddFrameExecute(pInterface);
		}

		return pInterface;
	}
	else
	{
		m_IntCreators[index].nRefCount++;
		
		return m_IntCreators[index].pInterface;
	}
}

void CIntManager::ReleaseReference(IInterface* pInterface)
{
	Assert(pInterface != NULL);
	
	if (m_pCore->GetQuit())
	{
		// 系统退出时不能删除功能接口
		return;
	}
	
	const char* name = pInterface->GetCreator()->GetName();
	size_t index;
	
	if (!FindIndex(name, index))
	{
		Assert(0);
		return;
	}
	
	Assert(m_IntCreators[index].nRefCount > 0);
	Assert(m_IntCreators[index].pInterface == pInterface);
	
	if (--m_IntCreators[index].nRefCount == 0)
	{
		if (pInterface->NeedExecPerFrame())
		{
			RemoveFrameExecute(pInterface);
		}
		double dTime = performance_time();
		pInterface->Shut();
		m_pCore->GetPerformance()->dShutFuncTime += performance_time() - dTime;
		m_IntCreators[index].pCreator->Destroy(pInterface);
		m_IntCreators[index].pInterface = NULL;
	}
}

bool CIntManager::AddFrameExecute(IInterface* pInterface)
{
	Assert(pInterface != NULL);

	m_FrameExecute.push_back(pInterface);

	return true;
}

bool CIntManager::RemoveFrameExecute(IInterface* pInterface)
{
	Assert(pInterface != NULL);

	const size_t size = m_FrameExecute.size();

	for (size_t i = 0; i < size; ++i)
	{
		if (m_FrameExecute[i] == pInterface)
		{
			m_FrameExecute.remove(i);
			return true;
		}
	}

	return false;
}

void CIntManager::ExecuteFrameBegin()
{
	const size_t size = m_FrameExecute.size();

	if (0 == size)
	{
		return;
	}

	double dTime = performance_time();

	IInterface** p = &m_FrameExecute[0];

	for (size_t i = 0; i < size; ++i)
	{
		p[i]->ExecFrameBegin();
	} 

	m_pCore->GetPerformance()->dExecFrameBeginFuncTime += performance_time() - dTime;
 
}

void CIntManager::ExecuteFrameEnd()
{
	const size_t size = m_FrameExecute.size();

	if (0 == size)
	{
		return;
	}

	double dTime = performance_time();

	IInterface** p = &m_FrameExecute[0];

	for (size_t i = 0; i < size; ++i)
	{
		p[i]->ExecFrameEnd();
	}

	m_pCore->GetPerformance()->dExecFrameEndFuncTime += performance_time() - dTime;
}

bool CIntManager::Dump(const char* file_name)
{
	FILE* fp = Port_FileOpen(file_name, "wb");
	
	if (NULL == fp)
	{
		return false;
	}
	
	const size_t size = m_IntCreators.size();
	
	for (size_t i = 0; i < size; ++i)
	{
		int_creator_t* p = &m_IntCreators[i];
		
		fprintf(fp, "Interface: %s, Refs: %d, Order: %d\r\n", 
			p->pCreator->GetName(), p->nRefCount, p->nOrder);
	}
	
	fprintf(fp, "\r\ntotal %d interfaces\r\n", size);
	fclose(fp);
	
	return true;
}
