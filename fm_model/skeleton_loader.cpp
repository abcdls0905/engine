//--------------------------------------------------------------------
// 文件名:		skeleton_loader.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2010年7月12日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "skeleton_loader.h"
#include "skeleton.h"
#include "../visual/i_render.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"

// CSkeletonLoader

CSkeletonLoader* CSkeletonLoader::NewInstance()
{
	return CORE_NEW(CSkeletonLoader);
}

CSkeletonLoader::CSkeletonLoader()
{
	m_pOwner = NULL;
	m_pSkeleton = NULL;
}

CSkeletonLoader::~CSkeletonLoader()
{
	if (m_pSkeleton)
	{
		unload_skeleton(m_pSkeleton);
	}
}

void CSkeletonLoader::Release()
{
	if (this->DecRefs() == 0)
	{
		CORE_DELETE(this);
	}
}

void CSkeletonLoader::SetOwner(CSkeleton* pOwner)
{
	m_pOwner = pOwner;
}

void CSkeletonLoader::SetFileName(const char* file_name)
{
	m_strFileName = file_name;
}

const char* CSkeletonLoader::GetName()
{
	return m_strFileName.c_str();
}

bool CSkeletonLoader::Load(bool async)
{
	m_pSkeleton = load_skeleton(m_strFileName.c_str());

	if (NULL == m_pSkeleton)
	{
		return false;
	}
	
	return true;
}

bool CSkeletonLoader::Create(bool async)
{
	if (!m_pOwner->Build(m_pSkeleton))
	{
		return false;
	}

	// 数据所有权已经转移
	m_pSkeleton = NULL;

	return true;
}

bool CSkeletonLoader::EndCreate(int category, bool succeed)
{
	return m_pOwner->EndCreate(category, succeed);
}
