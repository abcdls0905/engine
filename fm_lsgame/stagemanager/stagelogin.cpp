
#include "StageLogin.h"
#include "stagemanager.h"
#include "../fm_gui/form.h"
#include "../source_path_define.h"
#include "../formlogic/form_login.h"

//
StageLogin::StageLogin(StageManager* pStageManager)
{
	m_pStageManager = pStageManager;
	m_iPort = 2001;
}


//
StageLogin::~StageLogin(void)
{
}

//
void StageLogin::OnEntry()
{
	_loadScene();
}

//
void StageLogin::OnLeave()
{
}

//
bool StageLogin::OnNetMessage(const std::string& strMsgName, const IVarList& params)
{
	if (strMsgName == "OnLoginSucceed")	
	{
		// ��½�ɹ�
		m_pStageManager->SwitchStage(STAGE_ROLES);
	}
	else if(strMsgName == "OnEntryScene")	
	{
		// �˽׶��յ�EntryScene��˵����ɫ�Ѿ��ڳ�������
		m_pStageManager->SwitchStage(STAGE_MAIN);
	}
	return true;
}

//
bool StageLogin::OnCommand(const std::string& strCmd, const IVarList& params)
{	
	if (strCmd == "LoadFinish") 
	{
		// ����load����
		_loadFinish();
	}

	return true;
}

bool StageLogin::OnInputMessage(UINT msg, UINT param1, UINT param2)
{
	return true;
}


//���س���
bool StageLogin::_loadScene()
{

	StageUtils::LoadLoginScene();
	
	FormLogin::Instance()->Show();

	return true;
}

//
void StageLogin::_loadFinish()
{
	
}
