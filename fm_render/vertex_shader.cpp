//--------------------------------------------------------------------
// �ļ���:		vertex_shader.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2012��9��26��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "vertex_shader.h"
#include "render.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"

#ifdef _SHADER_OPTIMIZER_
#include "shader_optimizer/src/glsl/glsl_optimizer.h"
#endif

#include "../visual/vis_utils.h"

#include "../public/portable.h"
#define performance_time Port_GetPerformanceTime

CVertexShader::CVertexShader(Render* pRender)
{
	Assert(pRender != NULL);
	
	m_pRender = pRender;
	m_nIndex = -1;
	m_nShader = 0;
}

CVertexShader::~CVertexShader()
{
	if (m_nShader)
	{
		glDeleteShader(m_nShader);
	}
}

void CVertexShader::Destroy()
{
	CORE_DELETE(this);
}

void CVertexShader::Release()
{
	m_pRender->ReleaseResource(m_nIndex);
}

bool CVertexShader::Create(const void* shader_data, unsigned int shader_size)
{
	Assert(shader_data != NULL);

	Invalidate();

	m_shader_data = shader_data;
	m_shader_size = shader_size;
	
	return Restore();
}

// �豸����ʱ�Ĵ���
bool CVertexShader::Restore()
{
#ifdef PERFORMANCE_DEBUG
	double dTimes = performance_time();
#endif	

	m_nShader = glCreateShader(GL_VERTEX_SHADER);

	const char* shader_code = (const char*)m_shader_data;
	fm_int shader_len = m_shader_size;


#ifdef _SHADER_OPTIMIZER_
#ifdef RENDER_ES_3_0
	glslopt_ctx* ctx = glslopt_initialize(kGlslTargetOpenGLES30);
#else
	glslopt_ctx* ctx = glslopt_initialize(kGlslTargetOpenGLES20);
#endif
	glslopt_shader* shader = glslopt_optimize (ctx,kGlslOptShaderVertex,shader_code,0);
	//����Ƿ�����Ż��ɹ�
	bool optimizeOk = glslopt_get_status(shader);
	if(optimizeOk)
	{
		const char* newSource = glslopt_get_output (shader);
		/*
		char buffer[200];
		int statsAlu, statsTex, statsFlow;
		glslopt_shader_get_stats (shader, &statsAlu, &statsTex, &statsFlow);
		int inputCount = glslopt_shader_get_input_count (shader);
		sprintf(buffer, "\nVS// inputs: %i, stats: %i alu %i tex %i flow\n", inputCount, statsAlu, statsTex, statsFlow);
        CORE_TRACE(buffer);
        */
		fm_int t_shader_len = strlen(newSource);
		glShaderSource(m_nShader, 1, &newSource, &t_shader_len);
		TestErr((char*)m_shader_data);
	}
	else
	{ 
		const char* errorLog = glslopt_get_log (shader);
        CORE_TRACE("optimize VS err!");
		CORE_TRACE(errorLog);

		glShaderSource(m_nShader, 1, &shader_code, &shader_len);

		TestErr((char*)m_shader_data);
	}

	glCompileShader(m_nShader);

	glslopt_shader_delete(shader);
	glslopt_cleanup(ctx);
#else
	glShaderSource(m_nShader, 1, &shader_code, &shader_len);

	TestErr((char*)m_shader_data);
	glCompileShader(m_nShader);
#endif


	fm_int bShaderCompiled;
    glGetShaderiv(m_nShader, GL_COMPILE_STATUS, &bShaderCompiled);

	//������״̬
	if(!bShaderCompiled)
	{
		// An error happened, first retrieve the length of the log message
		// ��ȡ������Ϣ����
		int i32InfoLogLength, i32CharsWritten;
		glGetShaderiv(m_nShader, GL_INFO_LOG_LENGTH, &i32InfoLogLength);

		// Allocate enough space for the message and retrieve it
		char* pszInfoLog = new char[i32InfoLogLength];
		// ��ȡ��������Ϣ
		glGetShaderInfoLog(m_nShader, i32InfoLogLength, &i32CharsWritten, pszInfoLog);

		// �������
		CORE_TRACE(pszInfoLog);
		delete[] pszInfoLog;

#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->dBuildShaderTime += performance_time() - dTimes;
#endif	

		return false;
	}
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dBuildShaderTime += performance_time() - dTimes;
#endif	

	return true;
}

// �豸��ʧʱ�Ĵ���
bool CVertexShader::Invalidate()
{
	if (m_nShader)
	{
		glDeleteShader(m_nShader);
		m_nShader = 0;
	}
	return true;
}