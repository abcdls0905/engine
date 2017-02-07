//--------------------------------------------------------------------
// 文件名:		dynamic_vb.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年6月24日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "dynamic_vb.h"
#include "render.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../visual/vis_utils.h"

#include "../public/portable.h"
 

#define performance_time Port_GetPerformanceTime

CDynamicVB::CDynamicVB(Render* pRender,const char* pstrIdent)
{
	Assert(pRender != NULL);
	
	m_pRender = pRender;
	m_nIndex = -1;
	m_nSize = 0;
	m_nBuffer = 0;
    m_pstrIdent = pstrIdent;
}

CDynamicVB::~CDynamicVB()
{
	if (m_nBuffer)
	{
		glDeleteBuffers(1, &m_nBuffer);
	}
}

void CDynamicVB::Destroy()
{
	CORE_DELETE(this);
}

void CDynamicVB::Release()
{
	m_pRender->ReleaseResource(m_nIndex);
}

bool CDynamicVB::Create(unsigned int size)
{
#ifdef PERFORMANCE_DEBUG
	double dTime = performance_time();
#endif

	Assert(0 == m_nBuffer);
	IRenderDrawOp* pRenderDrawOp = m_pRender->GetRenderDrawOp();
	glGenBuffers(1, &m_nBuffer);
	//glBindBuffer(GL_ARRAY_BUFFER, m_nBuffer);
	pRenderDrawOp->SetVB( m_nBuffer );

	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dVBUploadGPUTime += performance_time() - dTime;
	m_pRender->GetPerformance()->dLoadVBData +=m_nSize/1024.0f;
#endif

	if(::TestErr("CDynamicVB::Create"))
	{
		if(this->GetIdent())
		{
			CORE_TRACE_EX("CDynamicVB IdentName:%s",GetIdent());
		}
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		CORE_TRACE_EX("CDynamicVB Size:%d",size);
		pRenderDrawOp->SetVB( 0 );
		return false;
	}

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	pRenderDrawOp->SetVB( 0 );
	
	m_nSize = size;

	return true;
}

	// 返回创建VB的时候的标识串
const char* CDynamicVB::GetIdent()
{
	return m_pstrIdent;
}

// 设备就绪时的处理
bool CDynamicVB::Restore()
{
	return true;
}

// 设备丢失时的处理
bool CDynamicVB::Invalidate()
{
	if(m_nBuffer)
	{
		::glDeleteBuffers(1, &m_nBuffer);
		m_nBuffer = 0;
	}
	return true;	
}