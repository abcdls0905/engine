#pragma once

#include "../stage/stage.h"

class StageManager 
{
	StageBase* m_stagelogic[STAGE_MAX];
	int m_stage;
public:
	StageManager();
	void RegStage(int stage, StageBase* stagebase);
	void RegStage(int stage, const char* entity_name);
	void SwitchStage(int stage);
	int GetStage();
};