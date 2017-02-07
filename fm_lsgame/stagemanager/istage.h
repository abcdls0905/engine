
#pragma once
#include "StageUtils.h"

#include "../fm_gui/gui_input.h"


class StageManager;

class IStage : public IEntity
{
public:
	IStage();
	virtual ~IStage(void);

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

public:
	//
	virtual void OnEntry() = 0;

	//
	virtual void OnLeave() = 0;

	// ��stage�յ�������Ϣ
	virtual bool OnNetMessage(const std::string& msg, const IVarList& params);

	// ��stage�յ�����ģ��ĵ�����Ϣ
	virtual bool OnCommand(const std::string& cmd, const IVarList& params);

	// ��stage�յ�������Ϣ
	virtual bool OnInputMessage(UINT msg, size_t param1, size_t param2);

protected:
	StageManager* m_pStageManager;
};
