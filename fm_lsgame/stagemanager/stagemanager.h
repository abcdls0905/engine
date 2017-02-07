
#pragma once
#include "StageUtils.h"

class IStage;

class StageManager : public IEntity
{
public:
	typedef std::map<std::string, IStage*> StageMap;

public:
	StageManager(void);
	~StageManager(void);

	bool Init(const IVarList& args);
	bool Shut();
	void Execute(float seconds);
	bool MsgProc(unsigned int msg, size_t param1, size_t param2, int& result);

	std::string CurrentStage();	// 当前stage名称
	std::string OldStage();		// 之前stage名称

	// stage跳转过程（先离开当前stage，再进入新stage）
	bool SwitchStage(const std::string& stageName);

	// 收到网络消息，会分发给当前stage
	bool OnNetMessage(const std::string& msg, const IVarList& params);

	// 收到其他模块调用消息，会分发给当前stage
	void OnCommand(const std::string& cmd, const IVarList& params);

	// 收到输入消息，会分发给当天stage
	bool OnInputMessage(UINT msg, size_t param1, size_t param2);

private:
	void _registerAllStages();
	void _unregisterAllStages();

	void _registerInputMessage();
	void _unregisterInputMessage();

	IStage* _getStage(const std::string& stageName);

	bool _handleNetMessage(const std::string& msgName, const IVarList& params);
	bool _handleCommand(const std::string& msgName, const IVarList& params);
	bool _handleInputMessage(UINT key, UINT param1, UINT param2);

private:
	StageMap m_mapStage;
	std::string m_strCurrentStage;
	std::string m_strOldStage;
};
