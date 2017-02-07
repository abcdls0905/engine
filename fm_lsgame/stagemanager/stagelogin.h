
#pragma once
#include "StageUtils.h"
#include "IStage.h"


class StageLogin : public IStage
{
public:
	StageLogin(StageManager* pStageManager);
	virtual ~StageLogin(void);

	virtual void OnEntry();
	virtual void OnLeave();

	virtual bool OnNetMessage(const std::string& strMsgName, const IVarList& params);

	virtual bool OnCommand(const std::string& strCmd, const IVarList& params);

	virtual bool OnInputMessage(UINT msg, UINT param1, UINT param2);
private:
	//载入登陆场景
	bool _loadScene();	

	//登陆场景载入结束
	void _loadFinish();

private:
	std::string m_strAddress;
	int m_iPort;

	std::string m_strAccount;
	std::string m_strPassword;
};
