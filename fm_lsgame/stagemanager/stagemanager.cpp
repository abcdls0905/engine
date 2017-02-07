

#include "StageManager.h"
#include "IStage.h"
#include "stagelogin.h"

#include "../fm_gui/gui_input.h"

DECLARE_ENTITY(StageManager, 0, IEntity);

//
StageManager::StageManager(void) 
{
	m_strCurrentStage = "";
}

//
StageManager::~StageManager(void)
{
}

// ��ʼ��
bool StageManager::Init(const IVarList& args)
{
	//CORE_ADD_EXECUTE(this);

	_registerAllStages();
	_registerInputMessage();

	return true;
}

// �ر����
bool StageManager::Shut()
{
	//CORE_REMOVE_EXECUTE(this);

	_unregisterAllStages();
	_unregisterInputMessage();

	return true;
}

// ��ѭ��
void StageManager::Execute(float seconds)
{

}

// ��ȡ������Ϣ
bool StageManager::MsgProc(unsigned int msg, size_t param1, size_t param2, int& result)
{
	OnInputMessage(msg, param1, param2);

	return false;
}


// ��תstage
bool StageManager::SwitchStage(const std::string& strStageName)
{

		if (strStageName.empty()) 
		{
			return false;
		}

		if (!m_strCurrentStage.empty())
		{
			IStage* pCurStage = _getStage(m_strCurrentStage);
			if (!pCurStage) 
			{
				return false;
			}

			pCurStage->OnLeave();
		}

		//StageUtils::OnStageChanged(mCurrentStage, stageName);
		m_strOldStage = m_strCurrentStage;
		m_strCurrentStage = strStageName;

		//
		IStage* pNewStage = _getStage(strStageName);
		if (!pNewStage) 
		{
			return false;
		}

		//
		pNewStage->OnEntry();
		return true;
}

// ��ǰ��stage
std::string StageManager::CurrentStage()
{
	return m_strCurrentStage;
}

// ֮ǰ��stage
std::string StageManager::OldStage()
{
	return m_strOldStage;
}

// �ַ�������Ϣ
bool StageManager::OnNetMessage(const std::string& msg, const IVarList& params)
{
	// �ַ�֮ǰ�Ĵ���
	_handleNetMessage(msg, params);

	// �ַ�����ǰstage
	IStage* pCurStage = _getStage(m_strCurrentStage);
	if (pCurStage)
	{
		return pCurStage->OnNetMessage(msg, params);
	}

	return true;
}

// �ַ�������Ϣ
bool StageManager::OnInputMessage(UINT msg, size_t param1, size_t param2)
{
	// �ַ�֮ǰ�Ĵ���
	_handleInputMessage(msg, param1, param2);

	// �ַ�����ǰstage
	IStage* curStage = _getStage(m_strCurrentStage);
	if (curStage)
	{
		curStage->OnInputMessage(msg, param1, param2);
	}

	return true;
}

// �ַ�Command
void StageManager::OnCommand(const std::string& cmd, const IVarList& params)
{
	IStage* curStage = _getStage(m_strCurrentStage);
	if (curStage)
	{
		curStage->OnCommand(cmd, params);
	}
}

// ע��������Ϣ
void StageManager::_registerInputMessage()
{

}

// ȥ��������Ϣ
void StageManager::_unregisterInputMessage()
{

}


// ע������stage
void StageManager::_registerAllStages()
{
 	m_mapStage[STAGE_LOGIN] = new StageLogin(this);
// 	m_mapStage[STAGE_CREATE] = new StageCreate(this);
// 	m_mapStage[STAGE_ROLES] = new StageRoles(this);
// 	m_mapStage[STAGE_MAIN] = new StageMain(this);
// 
// 	m_mapStage[STAGE_GUIDE] = new StageGuide(this);
	//m_mapStage[STAGE_HOME] = new StageHome(this);
	//m_mapStage[STAGE_SHOP] = new StageShop(this);
	//m_mapStage[STAGE_RANK] = new StageRank(this);
	//m_mapStage[STAGE_TASK] = new StageTask(this);
	//m_mapStage[STAGE_ROOMS] = new StageRooms(this);
}

// �������stage
void StageManager::_unregisterAllStages()
{
	StageMap::iterator it = m_mapStage.begin();
	for (; it!=m_mapStage.end(); it++)
	{
		IStage* stage = it->second;
		delete stage;
	}

	m_mapStage.clear();
}

// ����stage���ֻ�ȡstage
IStage* StageManager::_getStage(const std::string& stageName)
{
	if (m_mapStage.find(stageName) == m_mapStage.end())
	{
		return NULL;
	}

	return m_mapStage[stageName];
}

// �ַ�������stage֮ǰ���ȴ���һ��
bool StageManager::_handleNetMessage(const std::string& strMsgName, const IVarList& params)
{
	return true;
}

//�ַ���stage֮ǰ�ȴ���һ��
bool StageManager::_handleInputMessage(UINT key, UINT param1, UINT param2)
{
	return true;
}

//�ַ���stage֮ǰ�ȴ���һ��
bool StageManager::_handleCommand(const std::string& msgName, const IVarList& params)
{
	return true;
}