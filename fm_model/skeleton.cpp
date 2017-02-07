//--------------------------------------------------------------------
// 文件名:		skeleton.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2010年7月12日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "skeleton.h"
#include "skeleton_data.h"
#include "skeleton_loader.h"
#include "res_manager.h"
#include "../visual/i_render.h"
#include "../visual/i_texture.h"
#include "../visual/vis_utils.h"
#include "../public/inlines.h"
#include "../public/core_mem.h"

extern IRender* g_pRender;

// CSkeleton

CSkeleton* CSkeleton::NewInstance()
{
	return CORE_NEW(CSkeleton);
}

CSkeleton::CSkeleton()
{
	m_pManager = NULL;
	m_pSkeleton = NULL;
	m_pLoader = NULL;
	m_nState = 0;
}

CSkeleton::~CSkeleton()
{
	SAFE_RELEASE(m_pLoader);

	if (m_pSkeleton)
	{
		unload_skeleton(m_pSkeleton);
	}
}

void CSkeleton::Destroy()
{
	CORE_DELETE(this);
}

void CSkeleton::Release()
{
	m_pManager->ReleaseSkeleton(this);
}

bool CSkeleton::Create()
{
	if (!BeginCreate())
	{
		return false;
	}
	
	if (!m_pLoader->Load(false))
	{
		EndCreate(RES_CATEGORY_SKELETON, false);
		return false;
	}
	
	if (!m_pLoader->Create(false))
	{
		EndCreate(RES_CATEGORY_SKELETON, false);
		return false;
	}
	
	if (!EndCreate(RES_CATEGORY_SKELETON, true))
	{
		return false;
	}
	
	return true;
}

bool CSkeleton::CanAsync()
{
	return true;
}

bool CSkeleton::BeginCreate()
{
	Assert(m_pLoader == NULL);
	
	char fname[256];

	SafeSprintf(fname, sizeof(fname), "%s%s", 
		g_pRender->GetPathPrefix(GetName()), GetName());

	CSkeletonLoader* loader = CSkeletonLoader::NewInstance();
	
	loader->SetOwner(this);
	loader->SetFileName(fname);
	
	m_pLoader = loader;

	return true;
}

bool CSkeleton::EndCreate(int category, bool succeed)
{
	if (category == RES_CATEGORY_SKELETON)
	{
		SAFE_RELEASE(m_pLoader);
	}
	
	if (succeed)
	{
		// 设置资源就绪
		SetState(STATE_READY);
	}
	else
	{
		SetState(STATE_FAILED);
	}
	
	return true;
}

IResLoader* CSkeleton::GetLoader()
{
	return m_pLoader;
}

bool CSkeleton::Build(skeleton_t* pSkeleton)
{
	Assert(pSkeleton != NULL);

	if (m_pSkeleton)
	{
		unload_skeleton(m_pSkeleton);
	}
	
	m_pSkeleton = pSkeleton;

	return true;
}
