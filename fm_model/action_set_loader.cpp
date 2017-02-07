//--------------------------------------------------------------------
// 文件名:		action_set_loader.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年7月13日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "action_set_loader.h"
#include "action_set.h"
#include "action_data.h"
#include "location.h"
#include "../visual/i_render.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/inlines.h"

extern IRender* g_pRender;

// CActionSetLoader

CActionSetLoader* CActionSetLoader::NewInstance()
{
	return CORE_NEW(CActionSetLoader);
}

CActionSetLoader::CActionSetLoader()
{
	m_pOwner = NULL;
	m_pActionSet = NULL;
	m_pMainSkeleton = NULL;
}

CActionSetLoader::~CActionSetLoader()
{
	if (m_pMainSkeleton)
	{
		unload_skeleton(m_pMainSkeleton);
	}

	if (m_pActionSet)
	{
		unload_action_set(m_pActionSet);
	}
}

void CActionSetLoader::Release()
{
	if (this->DecRefs() == 0)
	{
		CORE_DELETE(this);
	}
}

void CActionSetLoader::SetOwner(CActionSet* pOwner)
{
	m_pOwner = pOwner;
}

void CActionSetLoader::SetFileName(const char* file_name)
{
	m_strFileName = file_name;
}

void CActionSetLoader::SetAppendPath(const char* append_path)
{
	Assert(append_path != NULL);

	m_strAppendPath = append_path;
}

const char* CActionSetLoader::GetName()
{
	return m_strFileName.c_str();
}

bool CActionSetLoader::Load(bool async)
{
	// 加载动作集配置文件
	m_pActionSet = load_action_set(m_strFileName.c_str(), 
		m_strAppendPath.c_str());

	if (NULL == m_pActionSet)
	{
		CORE_TRACE("CActionSetLoader::Load)load actionset failed");
		CORE_TRACE(m_strFileName.c_str());
		return false;
	}

	if (NULL == m_pActionSet->pszMainSkeletonName)
	{
		CORE_TRACE("CActionSetLoader::Load)no main skeleton");
		CORE_TRACE(m_strFileName.c_str());
		return false;
	}

	// 加载主骨骼数据
	char fname[256];

	SafeSprintf(fname, sizeof(fname), "%s%s", 
		g_pRender->GetPathPrefix(m_pActionSet->pszMainSkeletonName),
		m_pActionSet->pszMainSkeletonName);

	m_pMainSkeleton = load_skeleton(fname);

	if (NULL == m_pMainSkeleton)
	{
		CORE_TRACE("CActionSetLoader::Load)load main skeleton failed");
		CORE_TRACE(fname);
		CORE_TRACE(m_strFileName.c_str());
		return false;
	}
	
	if (m_pMainSkeleton->nRootNodeCount == 0)
	{
		CORE_TRACE("CActionSetLoader::Load)main skeleton data error");
		CORE_TRACE(fname);
		CORE_TRACE(m_strFileName.c_str());
		return false;
	}
	
	// 获得根骨骼的初始位置
	location_t location;

	location.position.x = 0.0F;
	location.position.y = 0.0F;
	location.position.z = 0.0F;
	
	for (int i = 0; i < (int)m_pMainSkeleton->nRootNodeCount; ++i)
	{
		if (FXNODE_TYPE_BONE == m_pMainSkeleton->RootNodes[i].nType)
		{
			location = m_pMainSkeleton->pBoneNodeList[0]->pLocation[0];
			break;
		}
	}

	m_pActionSet->vRootPoint = location.position;

	return true;
}

bool CActionSetLoader::Create(bool async)
{
	if (!m_pOwner->Build(m_pActionSet, m_pMainSkeleton))
	{
		return false;
	}

	// 数据所有权已经转移
	m_pActionSet = NULL;
	m_pMainSkeleton = NULL;

	return true;
}

bool CActionSetLoader::EndCreate(int category, bool succeed)
{
	return m_pOwner->EndCreate(category, succeed);
}

