//--------------------------------------------------------------------
// 文件名:		i_actor_callee.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年2月4日
// 创建人:		惠军超
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _VISUAL_I_ACTOR_CALLEE_H
#define _VISUAL_I_ACTOR_CALLEE_H

#include "../public/module.h"
#include "../visual/i_vis_base.h"

// 组合模型动作事件接口

class IActorCallee: public IEntity
{
public:
	// 虚函数的缺省实现是为了可以定义IActorCallee基类
	virtual bool Init(const IVarList& args) { return false; }
	virtual bool Shut() { return false; }

	// 动作事件
	virtual bool OnActionEvent(IVisBase* actor, const char* action_name,
		const char* event_name, int frame, const char* info) { return 0; }
};

#endif // _VISUAL_I_ACTOR_CALLEE_H
