#pragma once

#include "../stage/stage.h"

class StageSwitchScene : public StageBase
{
	bool m_sceneload;
	PERSISTID m_form_id;
public:
	StageSwitchScene(void);
	~StageSwitchScene(void);

	void OnEnterStage();
	void OnLeaveStage();
	void Execute(float secontds);
};
