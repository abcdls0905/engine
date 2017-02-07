#pragma once

#include "../stage/stage.h"

class StageTestModel : public StageBase
{
public:
	StageTestModel(void);
	~StageTestModel(void);

	void OnEnterStage();
	void OnLeaveStage();
};
