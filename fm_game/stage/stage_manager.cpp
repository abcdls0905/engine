#include "stage_manager.h"
#include "utility.h"
#include "global.h"
#include "stage_login.h"
#include "stage_main.h"
#include "stage_client.h"
#include "../stage/stage.h"



StageManager::StageManager()
:m_stage(STAGE_NONE)
{
	static StageLogin stage_login;
	static StageMain stage_main;
	static StageClient stage_client;

	m_stagelogic[STAGE_LOGIN] = &stage_login;
	m_stagelogic[STAGE_MAIN] = &stage_main;
	m_stagelogic[STAGE_CLIENT] = &stage_client;
}

void StageManager::RegStage(int stage, StageBase* logic)
{
	m_stagelogic[stage] = logic;
}

void StageManager::RegStage(int stage, const char* entity_name)
{
	IEntity* pEntity = g_core->CreateEntity(entity_name);
	RegStage(stage, (StageBase*)pEntity);
}

void StageManager::SwitchStage(int stage)
{
	if(stage == m_stage)
		return;
	if(stage == STAGE_NONE)
	{
		m_stage = stage;
		return;
	}
	if(m_stage != STAGE_NONE)
		m_stagelogic[m_stage]->OnLeaveStage();
	m_stage = stage;
	m_stagelogic[stage]->OnEnterStage();
}

int StageManager::GetStage()
{
	return m_stage;
}

StageManager g_stage_manager;
void stage_reg(int stage, StageBase* logic)
{
	g_stage_manager.RegStage(stage, logic);
}

void stage_reg(int stage, const char* entity_name)
{
	g_stage_manager.RegStage(stage, entity_name);
}

void stage_switch(int stage)
{
	g_stage_manager.SwitchStage(stage);
}

int stage_current()
{
	return g_stage_manager.GetStage();
}