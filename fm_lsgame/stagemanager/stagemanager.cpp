

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

// 初始化
bool StageManager::Init(const IVarList& args)
{
	//CORE_ADD_EXECUTE(this);

	_registerAllStages();
	_registerInputMessage();

	return true;
}

// 关闭清楚
bool StageManager::Shut()
{
	//CORE_REMOVE_EXECUTE(this);

	_unregisterAllStages();
	_unregisterInputMessage();

	return true;
}

// 真循环
void StageManager::Execute(float seconds)
{

}

// 获取输入消息
bool StageManager::MsgProc(unsigned int msg, size_t param1, size_t param2, int& result)
{
	OnInputMessage(msg, param1, param2);

	return false;
}


// 跳转stage
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

// 当前的stage
std::string StageManager::CurrentStage()
{
	return m_strCurrentStage;
}

// 之前的stage
std::string StageManager::OldStage()
{
	return m_strOldStage;
}

// 分发网络消息
bool StageManager::OnNetMessage(const std::string& msg, const IVarList& params)
{
	// 分发之前的处理
	_handleNetMessage(msg, params);

	// 分发给当前stage
	IStage* pCurStage = _getStage(m_strCurrentStage);
	if (pCurStage)
	{
		return pCurStage->OnNetMessage(msg, params);
	}

	return true;
}

// 分发输入消息
bool StageManager::OnInputMessage(UINT msg, size_t param1, size_t param2)
{
	// 分发之前的处理
	_handleInputMessage(msg, param1, param2);

	// 分发给当前stage
	IStage* curStage = _getStage(m_strCurrentStage);
	if (curStage)
	{
		curStage->OnInputMessage(msg, param1, param2);
	}

	return true;
}

// 分发Command
void StageManager::OnCommand(const std::string& cmd, const IVarList& params)
{
	IStage* curStage = _getStage(m_strCurrentStage);
	if (curStage)
	{
		curStage->OnCommand(cmd, params);
	}
}

// 注册输入消息
void StageManager::_registerInputMessage()
{

}

// 去掉输入消息
void StageManager::_unregisterInputMessage()
{

}


// 注册所有stage
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

// 清除所有stage
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

// 根据stage名字获取stage
IStage* StageManager::_getStage(const std::string& stageName)
{
	if (m_mapStage.find(stageName) == m_mapStage.end())
	{
		return NULL;
	}

	return m_mapStage[stageName];
}

// 分发被各个stage之前，先处理一下
bool StageManager::_handleNetMessage(const std::string& strMsgName, const IVarList& params)
{
	return true;
}

//分发给stage之前先处理一下
bool StageManager::_handleInputMessage(UINT key, UINT param1, UINT param2)
{
	return true;
}

//分发给stage之前先处理一下
bool StageManager::_handleCommand(const std::string& msgName, const IVarList& params)
{
	return true;
}