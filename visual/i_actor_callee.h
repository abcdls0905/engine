//--------------------------------------------------------------------
// �ļ���:		i_actor_callee.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��2��4��
// ������:		�ݾ���
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _VISUAL_I_ACTOR_CALLEE_H
#define _VISUAL_I_ACTOR_CALLEE_H

#include "../public/module.h"
#include "../visual/i_vis_base.h"

// ���ģ�Ͷ����¼��ӿ�

class IActorCallee: public IEntity
{
public:
	// �麯����ȱʡʵ����Ϊ�˿��Զ���IActorCallee����
	virtual bool Init(const IVarList& args) { return false; }
	virtual bool Shut() { return false; }

	// �����¼�
	virtual bool OnActionEvent(IVisBase* actor, const char* action_name,
		const char* event_name, int frame, const char* info) { return 0; }
};

#endif // _VISUAL_I_ACTOR_CALLEE_H
