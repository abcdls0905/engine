#pragma once

#include "../public/module.h"

enum {
	STAGE_NONE = -1,
	STAGE_LOGIN, //��½�߼�
	STAGE_MAIN,  //��Ϸ�߼�
	STAGE_SWITCHSCENE,  //��Ϸ�߼�
	STAGE_CLIENT, //���ͻ��˵���
	STAGE_TESTMODEL, //����ģ�Ͳ���
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

