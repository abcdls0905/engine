//--------------------------------------------------------------------
// 文件名:		static_ib.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年6月25日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "static_ib.h"
#include "render.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"

CStaticIB::CStaticIB(Render* pRender)
{
	Assert(pRender != NULL);
	
	m_pRender = pRender;
	m_nIndex = -1;
	m_nFormat = 0;
	m_nSize = 0;
	m_nBuffer = 0;
#ifdef PROCESS_LOST_DEVICE
	m_pData = 0;
#endif
}

CStaticIB::~CStaticIB()
{
	if (m_nBuffer)
	{
		glDeleteBuffers(1, &m_nBuffer);
		m_pRender->GetMemPerformance()->nUsedIBData += m_nSize;
	}

#ifdef PROCESS_LOST_DEVICE
	if(m_pData)
	{
		CORE_FREE(m_pData, m_nSize);
	}
#endif
}

void CStaticIB::Destroy()
{
	CORE_DELETE(this);
}

void CStaticIB::Release()
{
	m_pRender->ReleaseResource(m_nIndex);
}

bool CStaticIB::Create(int format, const void* pdata, unsigned int size)
{
	Assert(0 == m_nBuffer);

	m_nFormat = format;
	m_nSize = size;

#ifdef PROCESS_LOST_DEVICE
	if(m_pData)
	{
		CORE_FREE(m_pData, m_nSize);
	}

	m_pData = CORE_ALLOC(m_nSize);
	memcpy(m_pData, pdata, m_nSize);
	return Restore();
#else
	void *pData = CORE_ALLOC(m_nSize);
	memcpy(pData, pdata, m_nSize);
	
	IRenderDrawOp* pRenderDrawOp = m_pRender->GetRenderDrawOp();
	
	glGenBuffers(1, &m_nBuffer);
	pRenderDrawOp->SetIB( m_nBuffer );
	TestErr("CStaticIB restore pre");
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_nSize, pData, GL_STATIC_DRAW);

	CORE_FREE(pData, m_nSize);
	int err = glGetError();

	if (err != GL_NO_ERROR)
	{
		char info[256];
		SafeSprintf(info, sizeof(info), 
			"(CStaticIB::Create)[DxLog][err:%d]glBufferData failed, "
			"size:%d", err, m_nSize);
		CORE_TRACE(info);

		if (err == GL_OUT_OF_MEMORY)
		{
			// 显存不足
			m_pRender->RaiseDeviceError(err,
				"(CStaticIB::Create)glBufferData failed");
		}

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		pRenderDrawOp->SetIB( 0 );
		return false;
	}

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);;
	pRenderDrawOp->SetIB( 0 );
#endif
	m_pRender->GetMemPerformance()->nUsedIBData += m_nSize;
	return true;
	
}


#ifdef PROCESS_LOST_DEVICE
// 设备就绪时的处理
bool CStaticIB::Restore()
{
	IRenderDrawOp* pRenderDrawOp = m_pRender->GetRenderDrawOp();
	
	glGenBuffers(1, &m_nBuffer);
	pRenderDrawOp->SetIB( m_nBuffer );
	TestErr("CStaticIB restore pre");
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_nSize, m_pData, GL_STATIC_DRAW);

	int err = glGetError();

	if (err != GL_NO_ERROR)
	{
		char info[256];
		SafeSprintf(info, sizeof(info), 
			"(CStaticIB::Create)[DxLog][err:%d]glBufferData failed, "
			"size:%d", err, m_nSize);
		CORE_TRACE(info);

		if (err == GL_OUT_OF_MEMORY)
		{
			// 显存不足
			m_pRender->RaiseDeviceError(err,
				"(CStaticIB::Create)glBufferData failed");
		}

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		pRenderDrawOp->SetIB( 0 );
		return false;
	}

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);;
	pRenderDrawOp->SetIB( 0 );
	return true;
}

// 设备丢失时的处理
bool CStaticIB::Invalidate()
{
	if (m_nBuffer)
	{
		glDeleteBuffers(1, &m_nBuffer);
		m_nBuffer = 0;
	}
	return true;
}

#endif