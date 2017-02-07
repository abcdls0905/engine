#pragma once

#include "../public/i_entity.h"

class UI3DTest : public IEntity
{
public:
	// 初始化（可创建其他附属实体）
	virtual bool Init(const IVarList& args);

	// 关闭（可删除其他引用的附属实体，而资源释放应在析构函数中进行）
	virtual bool Shut();

	// 窗口消息处理
	virtual bool MsgProc(unsigned int msg, size_t param1, size_t param2, 
		int& result);

	virtual void Execute(float seconds) ;

};