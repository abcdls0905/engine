//--------------------------------------------------------------------
// �ļ���:		custom_handler.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2008��7��24��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "custom_handler.h"
#include "../public/inlines.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"

/// \file custom_handler.cpp
/// \brief �Զ�����Ϣ������

/// entity: CustomHandler
/// \brief �Զ�����Ϣ������ʵ��
DECLARE_ENTITY(CustomHandler, NULL, IEntity);

// CustomHandler

CustomHandler::CustomHandler()
{
}

CustomHandler::~CustomHandler()
{
}

bool CustomHandler::Init(const IVarList& args)
{
	return true;
}

bool CustomHandler::Shut()
{
	return true;
}

bool CustomHandler::Process(const IVarList& msg)
{
	int arg_num = msg.IntVal(0);

	if (0 == arg_num)
	{
		CORE_TRACE("(CustomHandler::Process)custom no argument");
		return false;
	}

	char event[128];

	if (msg.GetType(1) == VTYPE_INT)
	{
		int msg_type = msg.IntVal(1);

		SafeSprintf(event, sizeof(event), "on_%d", msg_type);
	}
	else if (msg.GetType(1) == VTYPE_STRING)
	{
		const char* msg_type = msg.StringVal(1);

		SafeSprintf(event, sizeof(event), "on_%s", msg_type);
	}
	else
	{
		CORE_TRACE("(CustomHandler::Process)custom msg type error");
		return false;
	}
	
	/*
	if (!GetCore()->ExecCallback(this, event, msg, NULL))
	{
		CORE_TRACE("(CustomHandler::Process)custom no handler");
		CORE_TRACE(event);
		return false;
	}
		*/
	return true;
}

