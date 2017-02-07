//--------------------------------------------------------------------
// 文件名:		dynamic_ib.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年6月24日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "dynamic_ib.h"
#include "render.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"

 
#include "../public/portable.h"
 

#define performance_time Port_GetPerformanceTime

CDynamicIB::CDynamicIB(Render* pRender,const char* pstrIdent)
{
	Assert(pRender != NULL);
	
	m_pRender = pRender;
	m_nIndex = -1;
	m_nFormat = 0;
	m_nSize = 0;
	m_nBuffer = 0;
	m_pstrIdent = pstrIdent;
}

CDynamicIB::~CDynamicIB()
{
	if (m_nBuffer)
	{
		glDeleteBuffers(1, &m_nBuffer);
	}
}

void CDynamicIB::Destroy()
{
	CORE_DELETE(this);
}

void CDynamicIB::Release()
{
	m_pRender->ReleaseResource(m_nIndex);
}

bool CDynamicIB::Create(int format, unsigned int size)
{
#ifdef PERFORMANCE_DEBUG
	double dTime = performance_time();
#endif

	Assert(0 == m_nBuffer);
	
	glGenBuffers(1, &m_nBuffer);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nBuffer);
	IRenderDrawOp* pRenderDrawOp = m_pRender->GetRenderDrawOp();

	pRenderDrawOp->SetIB( m_nBuffer );
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dIBUploadGPUTime += performance_time() - dTime;
	m_pRender->GetPerformance()->dLoadIBData +=m_nSize/1024.0f;
#endif

	int err = glGetError();
	
	if (err != GL_NO_ERROR)
	{
		char info[256];
		SafeSprintf(info, sizeof(info), 
			"(CDynamicIB::Create)[DxLog][err:%d]glBufferData failed, "
			"size:%d", err, m_nSize);
		CORE_TRACE(info);

		if (err == GL_OUT_OF_MEMORY)
		{
			// 显存不足
			m_pRender->RaiseDeviceError(err,
				"(CDynamicIB::Create)glBufferData failed");
		}
		
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		pRenderDrawOp->SetIB(0);
		return false;
	}

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	pRenderDrawOp->SetIB(0);

	m_nFormat = format;
	m_nSize = size;

	return true;
}

// 设备就绪时的处理
bool CDynamicIB::Restore()
{
	return true;
}

// 设备丢失时的处理
bool CDynamicIB::Invalidate()
{
	if(m_nBuffer)
	{
		::glDeleteBuffers(1, &m_nBuffer);
		m_nBuffer = 0;
	}
	return true;	
}