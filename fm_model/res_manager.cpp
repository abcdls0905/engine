//--------------------------------------------------------------------
// 文件名:		res_manager.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年5月4日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "res_manager.h"
#include "res_model.h"
#include "skeleton.h"
#include "action_set.h"
#include "model_system.h"
#include "../visual/i_render.h"
#include "../visual/i_resource.h"
#include "../public/core_mem.h"
#include "../public/core_file.h"
#include "../public/portable.h"

DECLARE_ENTITY(CResManagerUpdate, 0, IEntity);

// CResManager

CResManager::CResManager()
{
	m_pModelSystem = NULL;
}

CResManager::~CResManager()
{
	// 必须先释放动作集合
	for (action_set_container_t::iterator it1 = m_ActionSets.Begin(); 
		it1 != m_ActionSets.End(); ++it1)
	{
		CActionSet* p = it1.GetData();

		p->Destroy();
	}

	for (skeleton_container_t::iterator it2 = m_Skeletons.Begin(); 
		it2 != m_Skeletons.End(); ++it2)
	{
		CSkeleton* p = it2.GetData();

		p->Destroy();
	}

	for (model_container_t::iterator it3 = m_ResModels.Begin(); 
		it3 != m_ResModels.End(); ++it3)
	{
		CResModel* p = it3.GetData();

		p->Destroy();
	}
}

bool CResManager::Initialize(ModelSystem* pModelSystem)
{
	Assert(pModelSystem != NULL);
	
	m_pModelSystem = pModelSystem;
	
	return true;
}

bool CResManager::ReleaseAllTextures()
{
	for (model_container_t::iterator it = m_ResModels.Begin(); 
		it != m_ResModels.End(); ++it)
	{
		CResModel* p = it.GetData();

		p->ReleaseTextures();
	}

	return true;
}

CResModel* CResManager::CreateResModel(const char* name, 
	const char* tex_paths, bool async)
{
	CResModel* p;
	
	if (m_ResModels.GetData(name, p))
	{
		p->IncRefs();
		return p;
	}
	
	CResModel* pResModel = CResModel::NewInstance();
	
	pResModel->SetManager(this);
	pResModel->SetName(name);
	pResModel->SetTexPaths(tex_paths);
	
	if (async && pResModel->CanAsync())
	{
		// 创建异步加载的相关内容
		if (!pResModel->BeginCreate())
		{
			pResModel->Destroy();
			return NULL;
		}
		
		// 请求线程加载
		if (!m_pModelSystem->GetRender()->RequestLoadResource( 
			CResModel::RES_CATEGORY_MODEL, pResModel->GetLoader()))
		{
			pResModel->Destroy();
			return NULL;
		}
		
		// 设置正在加载状态
		pResModel->SetState(CResModel::STATE_LOADING);
	}
	else
	{
		// 同步加载
		if (!pResModel->Create())
		{
			pResModel->Destroy();
			return NULL;
		}
		
		// 设置资源就绪
		pResModel->SetState(CResModel::STATE_READY);
	}
	
	m_ResModels.Add(name, pResModel);
	
	return pResModel;
}

bool CResManager::ReleaseResModel(CResModel* pResModel)
{
	Assert(pResModel != NULL);

    pResModel->DecRefs();

	if (0 == pResModel->GetRefs())
	{
		pResModel->time = 0.0;
	}
	
	return true;
}

CSkeleton* CResManager::GetSkeleton(const char* name, skeleton_t* pSkeleton)
{
	CSkeleton* p;

	if (m_Skeletons.GetData(name, p))
	{
		p->Build(pSkeleton);
		p->EndCreate(0, true);
		p->IncRefs();
		return p;
	}

	p = CSkeleton::NewInstance();
	p->SetManager(this);
	p->SetName(name);
	p->Build(pSkeleton);
	// 设置资源就绪
	p->SetState(CSkeleton::STATE_READY);

	m_Skeletons.Add(name, p);

	return p;
}

CSkeleton* CResManager::CreateSkeleton(const char* name, bool async)
{
	CSkeleton* p;

	if (m_Skeletons.GetData(name, p))
	{
		p->IncRefs();
		return p;
	}

	CSkeleton* pSkeleton = CSkeleton::NewInstance();
	
	pSkeleton->SetManager(this);
	pSkeleton->SetName(name);

	if (async && pSkeleton->CanAsync())
	{
		// 创建异步加载的相关内容
		if (!pSkeleton->BeginCreate())
		{
			pSkeleton->Destroy();
			return NULL;
		}

		// 请求线程加载
		if (!m_pModelSystem->GetRender()->RequestLoadResource(
			0, pSkeleton->GetLoader()))
		{
			pSkeleton->Destroy();
			return NULL;
		}

		// 设置正在加载状态
		pSkeleton->SetState(CSkeleton::STATE_LOADING);
	}
	else
	{
		// 同步加载
		if (!pSkeleton->Create())
		{
			pSkeleton->Destroy();
			return NULL;
		}

		// 设置资源就绪
		pSkeleton->SetState(CSkeleton::STATE_READY);
	}

	m_Skeletons.Add(name, pSkeleton);

	return pSkeleton;
}

bool CResManager::ReleaseSkeleton(CSkeleton* pSkeleton)
{
	Assert(pSkeleton != NULL);

    pSkeleton->DecRefs();

	if (0 == pSkeleton->GetRefs())
	{
		pSkeleton->time = 0;
	}

	return true;
}

CActionSet* CResManager::CreateActionSet(const char* name, 
	const char* append_path, bool async)
{
	CActionSet* p;

	if (m_ActionSets.GetData(name, p))
	{
		p->IncRefs();
		return p;
	}

	CActionSet* pActionSet = CActionSet::NewInstance();
	
	pActionSet->SetManager(this);
	pActionSet->SetName(name);
	pActionSet->SetAppendPath(append_path);

	if (async && pActionSet->CanAsync())
	{
		// 创建异步加载的相关内容
		if (!pActionSet->BeginCreate())
		{
			pActionSet->Destroy();
			return NULL;
		}

		// 请求线程加载
		if (!m_pModelSystem->GetRender()->RequestLoadResource(
			0, pActionSet->GetLoader()))
		{
			pActionSet->Destroy();
			return NULL;
		}

		// 设置正在加载状态
		pActionSet->SetState(CActionSet::STATE_LOADING);
	}
	else
	{
		// 同步加载
		if (!pActionSet->Create())
		{
			pActionSet->Destroy();
			return NULL;
		}

		// 设置资源就绪
		pActionSet->SetState(CActionSet::STATE_READY);
	}

	m_ActionSets.Add(name, pActionSet);

	return pActionSet;
}

bool CResManager::ReleaseActionSet(CActionSet* pActionSet)
{
	Assert(pActionSet != NULL);

    pActionSet->DecRefs();

	if (0 == pActionSet->GetRefs())
	{
        pActionSet->time = 0.0f;
	}

	return true;
}

int CResManager::CollectUnuseAction(int seconds)
{
	unsigned int ms = seconds * 1000;
	unsigned int cur_tick = Port_GetTickCount();
	action_set_container_t::iterator it = m_ActionSets.Begin();
	action_set_container_t::iterator end = m_ActionSets.End();
	int count = 0;

	for (; it != end; ++it)
	{
		count += it.GetData()->CollectUnuseAction(cur_tick, ms);
	}

	return count;
}

bool CResManager::DumpAll(const char* file_name)
{
	Assert(file_name != NULL);

	FILE* fp = core_file::fopen(file_name, "wb");

	if (NULL == fp)
	{
		return false;
	}

	// 模型
	int count = 0;

	for (model_container_t::iterator it1 = m_ResModels.Begin(); 
		it1 != m_ResModels.End(); ++it1)
	{
		CResModel* p = it1.GetData();

		fprintf(fp, "geometry: %s refs: %d state: %d\r\n", 
			p->GetName(), p->GetRefs(), p->GetState());
		++count;
	}

	fprintf(fp, "\r\ntotal %d models\r\n\r\n", count);

	// 骨骼动画
	count = 0;

	for (skeleton_container_t::iterator it3 = m_Skeletons.Begin(); 
		it3 != m_Skeletons.End(); ++it3)
	{
		CSkeleton* p = it3.GetData();

		core_file::fprintf(fp, "skeleton: %s refs: %d state: %d\r\n", 
			p->GetName(), p->GetRefs(), p->GetState());
		++count;
	}

	core_file::fprintf(fp, "\r\ntotal %d animates\r\n\r\n", count);

	// 动作集
	count = 0;

	for (action_set_container_t::iterator it4 = m_ActionSets.Begin(); 
		it4 != m_ActionSets.End(); ++it4)
	{
		CActionSet* p = it4.GetData();

		core_file::fprintf(fp, "action set: %s refs: %d state: %d\r\n", 
			p->GetName(), p->GetRefs(), p->GetState());
		++count;
	}

	core_file::fprintf(fp, "\r\ntotal %d action sets\r\n\r\n", count);
	core_file::fclose(fp);

	return true;
}

// 初始化（可创建其他附属实体）
bool CResManager::Init(const IVarList& args)
{
//	g_pCore->AddExecute(this);
	return true;
}

// 关闭（可删除其他引用的附属实体，而资源释放应在析构函数中进行）
bool CResManager::Shut()
{
	return true;
}

#define IDLE_TIME 300.0f

void CResManager::Execute(float seconds)
{
	for (model_container_t::iterator it1 = m_ResModels.Begin(); 
		it1 != m_ResModels.End(); ++it1)
	{
		CResModel* pResModel = it1.GetData();
		
		if((pResModel->GetRefs() < 1) && (pResModel->time += seconds) > IDLE_TIME)
		{
			m_ResModels.Remove(pResModel->GetName());
			pResModel->Destroy();
			break;
		}
	}

	for (skeleton_container_t::iterator it3 = m_Skeletons.Begin(); 
		it3 != m_Skeletons.End(); ++it3)
	{
		CSkeleton* p = it3.GetData();

		if((p->GetRefs() < 1) && (p->time += seconds) > IDLE_TIME)
		{
			m_Skeletons.Remove(p->GetName());
			p->Destroy();
			break;
		}
	}

	for (action_set_container_t::iterator it4 = m_ActionSets.Begin(); 
		it4 != m_ActionSets.End(); ++it4)
	{
		CActionSet* p = it4.GetData();

		if((p->GetRefs() < 1) && (p->time += seconds) > IDLE_TIME)
		{
			m_ActionSets.Remove(p->GetName());
			p->Destroy();
			break;
		}
	}
}

bool CResManagerUpdate::Init(const IVarList& args)
{
	g_pCore->AddExecute(this);
	return true;
}

void CResManagerUpdate::Execute(float seconds)
{
	res_manager->Execute(seconds);
};