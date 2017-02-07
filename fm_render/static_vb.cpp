//--------------------------------------------------------------------
// 文件名:		static_vb.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年6月25日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "static_vb.h"
#include "render.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../visual/vis_utils.h"

#include "../public/portable.h"
 
#define performance_time Port_GetPerformanceTime

CStaticVB::CStaticVB(Render* pRender)
{
	Assert(pRender != NULL);
	
	m_pRender = pRender;
	m_nIndex = -1;
	m_nSize = 0;
	m_nBuffer = 0;
#ifdef PROCESS_LOST_DEVICE
	m_pData = 0;
#endif
}

CStaticVB::~CStaticVB()
{
	if (m_nBuffer)
	{
		glDeleteBuffers(1, &m_nBuffer);
		m_pRender->GetMemPerformance()->nUsedVBData -= m_nSize;
	}

#ifdef PROCESS_LOST_DEVICE
	if(m_pData)
	{
		CORE_FREE(m_pData, m_nSize);
	}
#endif
}

void CStaticVB::Destroy()
{
	CORE_DELETE(this);
}

void CStaticVB::Release()
{
	m_pRender->ReleaseResource(m_nIndex);
}

bool CStaticVB::Create(const void* pdata, unsigned int size)
{
	Assert(0 == m_nBuffer);
	m_nSize = size;
	
#ifdef PROCESS_LOST_DEVICE
	if(m_pData)
	{
		CORE_FREE(m_pData, m_nSize);
	}

	m_pData = CORE_ALLOC(sizeof(char*)*size);
	memcpy(m_pData, pdata, size);
	return Restore();
#else

	void* pData = CORE_ALLOC(sizeof(char*)*size);
	memcpy(pData, pdata, size);

	#ifdef PERFORMANCE_DEBUG
		double dTime = performance_time();
	#endif

	IRenderDrawOp* pRenderDrawOp = m_pRender->GetRenderDrawOp();
	
	glGenBuffers(1, &m_nBuffer);
	//glBindBuffer(GL_ARRAY_BUFFER, m_nBuffer);
	pRenderDrawOp->SetVB( m_nBuffer );

	glBufferData(GL_ARRAY_BUFFER, m_nSize, pData, GL_STATIC_DRAW);

	#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->dVBUploadGPUTime += performance_time() - dTime;
		m_pRender->GetPerformance()->dLoadVBData +=m_nSize/1024.0f;
	#endif

	CORE_FREE(pData, m_nSize);

	if(TestErr("CStaticVB::Create failed!"))
	{
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		pRenderDrawOp->SetVB( 0 );
		return false;
	}

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	pRenderDrawOp->SetVB( 0 );
	m_pRender->GetMemPerformance()->nUsedVBData += m_nSize;
	return true;
#endif
		
	
}

#ifdef PROCESS_LOST_DEVICE
bool CStaticVB::Restore()
{
#ifdef PERFORMANCE_DEBUG
	double dTime = performance_time();
#endif

	IRenderDrawOp* pRenderDrawOp = m_pRender->GetRenderDrawOp();
	
	glGenBuffers(1, &m_nBuffer);
	//glBindBuffer(GL_ARRAY_BUFFER, m_nBuffer);
	pRenderDrawOp->SetVB( m_nBuffer );

	glBufferData(GL_ARRAY_BUFFER, m_nSize, m_pData, GL_STATIC_DRAW);

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dVBUploadGPUTime += performance_time() - dTime;
	m_pRender->GetPerformance()->dLoadVBData +=m_nSize/1024.0f;
#endif

	if(TestErr("CStaticVB::Create failed!"))
	{
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		pRenderDrawOp->SetVB( 0 );
		return false;
	}

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	pRenderDrawOp->SetVB( 0 );
	return true;
}

bool CStaticVB::Invalidate()
{

	if(this->m_nBuffer)
	{
		::glDeleteBuffers(1, &m_nBuffer);
		m_nBuffer = 0;
	}
	return true;
}
#endif