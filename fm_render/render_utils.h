//--------------------------------------------------------------------
// �ļ���:		render_utils.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��9��12��
// ������:		�ſ�ʤ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _RENDER_UTILS_H
#define _RENDER_UTILS_H
#include "../public/inlines.h"
#include "../public/core_log.h"
#include "render_header.h"

// gl es �����⺯��
inline bool TestErr(const char* ident)
{
  return false;
	int err = glGetError();	
	if(err == GL_NO_ERROR)
		return false;
	if(ident)
	CORE_TRACE(ident);
	switch(err)
	{
	case GL_INVALID_ENUM:
		CORE_TRACE("Err: GL_INVALID_ENUM ");
		break;
	case GL_INVALID_VALUE:
		CORE_TRACE("Err: GL_INVALID_VALUE ");
		break;
	case GL_INVALID_OPERATION:
		CORE_TRACE("Err: GL_INVALID_OPERATION ");
		break;
	case GL_OUT_OF_MEMORY:
		CORE_TRACE("Err: GL_OUT_OF_MEMORY ");
		break;
	default:
		CORE_TRACE("Err: UnKnown ");
		break;
	}
	return true;
}

// FrameBuffer������
inline bool TestFrameErr(const char* ident)
{
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status == GL_FRAMEBUFFER_COMPLETE)
	{
		return false;
	}
	CORE_TRACE(ident);
	switch(status)
	{
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		CORE_TRACE("Err: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT ");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		CORE_TRACE("Err: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT ");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
		CORE_TRACE("Err: GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS ");
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		CORE_TRACE("Err: GL_FRAMEBUFFER_UNSUPPORTED ");
		break;
	default:
		CORE_TRACE("Err: CheckFramebufferStatus UnKnown ");
		break;
	}
    return true;
}

#endif
