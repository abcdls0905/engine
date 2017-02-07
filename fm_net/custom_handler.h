//--------------------------------------------------------------------
// �ļ���:		custom_handler.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��7��24��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _CUSTOM_HANDLER_H
#define _CUSTOM_HANDLER_H

#include "../public/module.h"

// �Զ�����Ϣ������

class CustomHandler: public IEntity
{
public:
	CustomHandler();
	virtual ~CustomHandler();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// ������Ϣ
	bool Process(const IVarList& msg);
};

#endif // _CUSTOM_HANDLER_H

