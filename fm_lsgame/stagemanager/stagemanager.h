
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

	std::string CurrentStage();	// ��ǰstage����
	std::string OldStage();		// ֮ǰstage����

	// stage��ת���̣����뿪��ǰstage���ٽ�����stage��
	bool SwitchStage(const std::string& stageName);

	// �յ�������Ϣ����ַ�����ǰstage
	bool OnNetMessage(const std::string& msg, const IVarList& params);

	// �յ�����ģ�������Ϣ����ַ�����ǰstage
	void OnCommand(const std::string& cmd, const IVarList& params);

	// �յ�������Ϣ����ַ�������stage
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
