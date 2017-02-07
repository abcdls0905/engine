
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
	//�����½����
	bool _loadScene();	

	//��½�����������
	void _loadFinish();

private:
	std::string m_strAddress;
	int m_iPort;

	std::string m_strAccount;
	std::string m_strPassword;
};
