//--------------------------------------------------------------------
// 文件名:		shader_program.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年3月26日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "shader_program.h"
#include "render.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"

#include "../public/portable.h"
#define performance_time Port_GetPerformanceTime

inline char** newstrings(const char** s, unsigned int len)
{
	char** p = (char**)malloc(sizeof(char*)*len);
	for(int i = 0; i < len; i++)
	{
		p[i] = strdup(s[i]);
	}
	return p;
}

CShaderProgram::CShaderProgram(Render* pRender):m_vs(NULL),m_ps(NULL)
{
	Assert(pRender != NULL);

	m_pRender = pRender;
	m_nIndex = -1;
	m_nProgram = 0;
	m_pshaderparamop = NULL;


}

CShaderProgram::~CShaderProgram()
{
	if (m_nProgram)
	{
		glDeleteProgram(m_nProgram);
	}

	if(m_pshaderparamop)
	{
		delete m_pshaderparamop;
		m_pshaderparamop = NULL;
	}

	for(int i = 0; i < m_element_num; i++)
	{
		free(m_element_names[i]);
	}
	free(m_element_names);
}

IShaderParamOp* CShaderProgram::GetParamOp() const
{
	return m_pshaderparamop;
}

IRenderStateOp* CShaderProgram::GetRenderStateOp() const
{
	return m_pRender->GetRenderStateOp();
}

IRenderDrawOp*  CShaderProgram::GetRenderDrawOp() const
{
	return m_pRender->GetRenderDrawOp();
}

//渲染状态记录器
IRenderStateRecordOp* CShaderProgram::GetRenderStateRecordOp() const
{
	return m_pRender->GetRenderStateRecordOp();
}

//使用改shader
void CShaderProgram::UsedShader()
{
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderShaderSumNum++;
#endif

	if( !m_pRender->GetUsedShader() || (m_nProgram != ((CShaderProgram*)m_pRender->GetUsedShader())->m_nProgram ) )
	{
		m_pRender->UsedShader(this);
		glUseProgram(m_nProgram);
#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->nRenderShaderUsedNum++;
#endif
	}
}

void CShaderProgram::Destroy()
{
	CORE_DELETE(this);
}

void CShaderProgram::Release()
{
	m_pRender->ReleaseResource(m_nIndex);
}

bool CShaderProgram::Create(IVertexShader* vs, IPixelShader* ps, const char* element_names[], 
							unsigned int element_num)
{
	/*
#ifdef _DEBUG
	CORE_TRACE("Build Shader ...");
	CORE_TRACE(vs->GetFileName());
	if( strcmp( vs->GetDefine() ,"") != 0)
		CORE_TRACE(vs->GetDefine());
	CORE_TRACE(ps->GetFileName());
	if( strcmp( ps->GetDefine() ,"") != 0)
		CORE_TRACE(ps->GetDefine());
#endif
	*/
	m_vs = vs;
	m_ps = ps;
	m_element_names = newstrings(element_names, element_num);
	m_element_num = element_num;
	return Create(this->m_vs->GetShader(), this->m_ps->GetShader(),(const char**)m_element_names, this->m_element_num);
}

bool CShaderProgram::Create(fm_uint vs, fm_uint ps, const char* element_names[],
							unsigned int element_num)
{
	Assert(vs != 0);
	Assert(ps != 0);

	if (m_nProgram)
	{
		glDeleteProgram(m_nProgram);
		m_nProgram = 0;
	}

#ifdef PERFORMANCE_DEBUG
	double dTimes = performance_time();
#endif	

	m_nProgram = glCreateProgram();

	glAttachShader(m_nProgram, vs);
	glAttachShader(m_nProgram, ps);

	for (unsigned int i = 0; i < element_num; ++i)
	{
		glBindAttribLocation(m_nProgram, i, element_names[i]);
	}

	glLinkProgram(m_nProgram);

	fm_int link_status;

	glGetProgramiv(m_nProgram, GL_LINK_STATUS, &link_status);

	if (link_status != GL_TRUE)
	{
		int i32InfoLogLength, i32CharsWritten;
		glGetProgramiv(m_nProgram, GL_INFO_LOG_LENGTH, &i32InfoLogLength);
		char* pszInfoLog = new char[i32InfoLogLength];
		glGetProgramInfoLog(m_nProgram, i32InfoLogLength, &i32CharsWritten, pszInfoLog);
		char* pszMsg = new char[i32InfoLogLength+256];
		strcpy(pszMsg, "(CShaderProgram::Create)link program failed:");
		strcat(pszMsg, pszInfoLog);
		CORE_TRACE(pszMsg);
		delete [] pszMsg;
		delete [] pszInfoLog;

		CORE_TRACE("glLinkProgram Failed!");
#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->dBuildShaderTime += performance_time() - dTimes;
#endif	
		return false;
	}
	if(m_pshaderparamop)
	{
		//m_pshaderparamop->SetProgram(m_nProgram);
		delete m_pshaderparamop;
	}
	//else
	m_pshaderparamop = NEW CShaderParamOp(m_nProgram,m_pRender);
//	CORE_TRACE("Build OK!");

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dBuildShaderTime += performance_time() - dTimes;
#endif	

	return true;
}

IVertexShader* CShaderProgram::GetVertexShader() const
{
	return m_vs;
}

IPixelShader*  CShaderProgram::GetPixelShader() const
{
	return m_ps;
}

void CShaderProgram::Print()
{

	CORE_TRACE_EX("[VS:%s PS:%s]",m_vs->GetFileName(),m_ps->GetFileName());
	m_pshaderparamop->Print();
}


#ifdef PROCESS_LOST_DEVICE

bool CShaderProgram::Restore()
{
	Create(m_vs->GetShader(), m_ps->GetShader(), (const char**)m_element_names, this->m_element_num);
	return true;
}

// 设备丢失时的处理
bool CShaderProgram::Invalidate()
{
	if (m_nProgram)
	{
		glDeleteProgram(m_nProgram);
		m_nProgram = 0;
	}

	return true;
}
#endif