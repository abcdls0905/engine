#pragma once

#include "../stage/stage.h"

#include "../formlogic/form_login.h"

class StageLogin : public StageBase
{
	form_login m_form_login;
public:
	void OnEnterStage();
	void OnLeaveStage();
};