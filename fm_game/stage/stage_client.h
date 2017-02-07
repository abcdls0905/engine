#pragma once

#include "../stage/stage.h"

class StageClient : public StageBase
{
	PERSISTID m_select;
public:
	void OnEnterStage();
	void OnLeaveStage();
	bool MsgProc(unsigned int msg, size_t param1, size_t param2, 
		int& result);
};