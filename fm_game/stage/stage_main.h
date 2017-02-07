#pragma once
#include "../stage/stage.h"

#include "../formlogic/form_gmcommand.h"

class StageMain : public StageBase
{
	form_gmcommand m_form_gmcccommand;
	bool m_isshow_gmcommand;
	void TouchScreen(int x, int y);
public:
	StageMain();
	void OnEnterStage();
	void OnLeaveStage();
	bool MsgProc(unsigned int msg, size_t param1, size_t param2, 
		int& result);
};