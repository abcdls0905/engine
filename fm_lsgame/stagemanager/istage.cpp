#include "IStage.h"

#include "StageManager.h"

//
IStage::IStage()
{
	m_pStageManager = NULL;
}

//
IStage::~IStage(void)
{
}

//
bool IStage::Init(const IVarList& args)
{
	return true;
}

//
bool IStage::Shut()
{
	return true;
}

//
bool IStage::OnNetMessage(const std::string& msg, const IVarList& params)
{
	return true;
}

//
bool IStage::OnInputMessage(UINT msg, size_t param1, size_t param2)
{
	return true;
}

//
bool IStage::OnCommand(const std::string& cmd, const IVarList& params)
{
	return true;
}
