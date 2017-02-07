#pragma once

#include "../public/module.h"

enum {
	STAGE_NONE = -1,
	STAGE_LOGIN, //登陆逻辑
	STAGE_MAIN,  //游戏逻辑
	STAGE_SWITCHSCENE,  //游戏逻辑
	STAGE_CLIENT, //纯客户端调试
	STAGE_TESTMODEL, //用来模型测试
	STAGE_MAX,
};

class StageBase : public IEntity
{
public:
	virtual void OnEnterStage() = 0;
	virtual void OnLeaveStage() = 0;
	bool Init(const IVarList& args){ return true; }
	bool Shut(){ return true; }
};

