//--------------------------------------------------------------------
// 文件名:		action_control_info.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2010年8月7日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "action_control.h"
#include "../public/inlines.h"

ActionControl::control_info_t::control_info_t()
{
	nRefs = 1;
}

ActionControl::control_info_t::~control_info_t()
{
	for (size_t i = 0; i < parameters.size(); ++i)
	{
		if (parameters[i])
		{
			CORE_DELETE(parameters[i]);
		}
	}

	for (size_t k = 0; k < commands.size(); ++k)
	{
		if (commands[k])
		{
			CORE_DELETE(commands[k]);
		}
	}

	for (size_t j = 0; j < states.size(); ++j)
	{
		if (states[j])
		{
			DeleteState(states[j]);
		}
	}
}

void ActionControl::control_info_t::Release()
{
	if (--nRefs == 0)
	{
		CORE_DELETE(this);
	}
}

void ActionControl::control_info_t::IncRefs()
{
	++nRefs;
}

void ActionControl::control_info_t::DeleteState(state_t* pState)
{
	Assert(pState != NULL);

	size_t node_num = pState->nodes.size();

	for (size_t i = 0; i < node_num; ++i)
	{
		state_node_t* pNode = pState->nodes[i];

		if (pNode)
		{
			CORE_DELETE(pNode);
		}
	}

	CORE_DELETE(pState);
}

void ActionControl::SetName(const char* value)
{
	Assert(value != NULL);

	m_pControlInfo->strName = value;
}

const char* ActionControl::GetName() const
{
	return m_pControlInfo->strName.c_str();
}

int ActionControl::GetParameterIndex(const char* param) const
{
	Assert(param != NULL);

	unsigned int hash = GetHashValueCase(param);
	size_t param_num = m_pControlInfo->parameters.size();

	for (size_t i = 0; i < param_num; ++i)
	{
		parameter_t* p = m_pControlInfo->parameters[i];

		if (NULL == p)
		{
			continue;
		}

		if (p->nHash == hash)
		{
			if (p->strName == param)
			{
				return int(i);
			}
		}
	}

	return -1;
}

bool ActionControl::AddParameter(const char* param, float def_value)
{
	Assert(param != NULL);

	if (GetParameterIndex(param) >= 0)
	{
		// 已存在
		return false;
	}

	int index = -1;
	size_t param_num = m_pControlInfo->parameters.size();

	for (size_t i = 0; i < param_num; ++i)
	{
		parameter_t* p = m_pControlInfo->parameters[i];

		if (NULL == p)
		{
			index = int(i);
			break;
		}
	}

	if (index < 0)
	{
		index = (int)param_num;
		m_pControlInfo->parameters.push_back(NULL);
	}

	parameter_t* p = CORE_NEW(parameter_t);

	p->strName = param;
	p->nHash = GetHashValueCase(param);
	p->fDefaultValue = def_value;

	m_pControlInfo->parameters[index] = p;

	return true;
}

bool ActionControl::RemoveParameter(const char* param)
{
	Assert(param != NULL);

	int index = GetParameterIndex(param);

	if (index < 0)
	{
		return false;
	}

	CORE_DELETE(m_pControlInfo->parameters[index]);
	m_pControlInfo->parameters[index] = NULL;

	return true;
}

void ActionControl::GetParameterList(const IVarList& args, 
	IVarList& result) const
{
	result.Clear();

	size_t param_num = m_pControlInfo->parameters.size();

	for (size_t i = 0; i < param_num; ++i)
	{
		parameter_t* p = m_pControlInfo->parameters[i];

		if (NULL == p)
		{
			continue;
		}

		result.AddString(p->strName.c_str());
	}
}

int ActionControl::GetCommandIndex(const char* command) const
{
	Assert(command != NULL);

	if (strcmp(command, "@end") == 0)
	{
		return END_COMMAND_INDEX;
	}
	
	unsigned int hash = GetHashValueCase(command);
	size_t command_num = m_pControlInfo->commands.size();

	for (size_t i = 0; i < command_num; ++i)
	{
		command_t* p = m_pControlInfo->commands[i];

		if (NULL == p)
		{
			continue;
		}

		if (p->nHash == hash)
		{
			if (p->strName == command)
			{
				return int(i);
			}
		}
	}

	return -1;
}

bool ActionControl::AddCommand(const char* command)
{
	Assert(command != NULL);

	if (GetCommandIndex(command) >= 0)
	{
		// 已存在
		return false;
	}

	int index = -1;

	size_t command_num = m_pControlInfo->commands.size();

	for (size_t i = 0; i < command_num; ++i)
	{
		command_t* p = m_pControlInfo->commands[i];

		if (NULL == p)
		{
			index = int(i);
			break;
		}
	}

	if (index < 0)
	{
		index = (int)command_num;
		m_pControlInfo->commands.push_back(NULL);
	}

	command_t* p = CORE_NEW(command_t);

	p->strName = command;
	p->nHash = GetHashValueCase(command);

	m_pControlInfo->commands[index] = p;

	return true;
}

bool ActionControl::RemoveCommand(const char* command)
{
	Assert(command != NULL);

	int index = GetCommandIndex(command);

	if (size_t(index) >= m_pControlInfo->commands.size())
	{
		return false;
	}

	CORE_DELETE(m_pControlInfo->commands[index]);
	m_pControlInfo->commands[index] = NULL;

	return true;
}

void ActionControl::GetCommandList(const IVarList& args, 
	IVarList& result) const
{
	result.Clear();

	size_t command_num = m_pControlInfo->commands.size();

	for (size_t i = 0; i < command_num; ++i)
	{
		command_t* p = m_pControlInfo->commands[i];

		if (NULL == p)
		{
			continue;
		}

		result.AddString(p->strName.c_str());
	}
}

int ActionControl::GetStateIndex(const char* state) const
{
	Assert(state != NULL);

	unsigned int hash = GetHashValueCase(state);
	size_t state_num = m_pControlInfo->states.size();

	for (size_t i = 0; i < state_num; ++i)
	{
		state_t* p = m_pControlInfo->states[i];

		if (NULL == p)
		{
			continue;
		}

		if (p->nHash == hash)
		{
			if (p->strName == state)
			{
				return int(i);
			}
		}
	}

	return -1;
}

bool ActionControl::AddState(const char* state,  //const char* next_state, 
	bool loop, float def_speed, bool can_break, float duration)
{
	Assert(state != NULL);
	//Assert(next_state != NULL);

	if ((def_speed < 0.01F) || (def_speed > 100.0F))
	{
		return false;
	}

	if (duration > 1024.0F)
	{
		return false;
	}
	
	if (GetStateIndex(state) >= 0)
	{
		// 已存在
		return false;
	}

	int index = -1;
	size_t state_num = m_pControlInfo->states.size();

	for (size_t i = 0; i < state_num; ++i)
	{
		state_t* p = m_pControlInfo->states[i];

		if (NULL == p)
		{
			index = int(i);
			break;
		}
	}

	if (index < 0)
	{
		index = (int)state_num;
		m_pControlInfo->states.push_back(NULL);
	}

	state_t* p = CORE_NEW(state_t);

	p->strName = state;
	p->nHash = GetHashValueCase(state);
	//p->strNextState = next_state;
	p->bLoop = loop;
	p->fDefaultSpeed = def_speed;
	p->bCanBreak = can_break;
	p->fDuration = duration;

	m_pControlInfo->states[index] = p;

	return true;
}

bool ActionControl::RemoveState(const char* state)
{
	Assert(state != NULL);

	int index = GetStateIndex(state);

	if (index < 0)
	{
		return false;
	}

	control_info_t::DeleteState(m_pControlInfo->states[index]);
	m_pControlInfo->states[index] = NULL;

	return true;
}

void ActionControl::GetStateList(const IVarList& args, IVarList& result) const
{
	result.Clear();

	size_t state_num = m_pControlInfo->states.size();

	for (size_t i = 0; i < state_num; ++i)
	{
		state_t* p = m_pControlInfo->states[i];

		if (NULL == p)
		{
			continue;
		}

		result.AddString(p->strName.c_str());
	}
}

/*
bool ActionControl::SetStateNext(const char* state, const char* next_state)
{
	Assert(state != NULL);
	Assert(next_state != NULL);

	int index = GetStateIndex(state);

	if (index < 0)
	{
		return false;
	}

	m_pControlInfo->states[index]->strNextState = next_state;

	return true;
}

const char* ActionControl::GetStateNext(const char* state) const
{
	Assert(state != NULL);
	
	int index = GetStateIndex(state);

	if (index < 0)
	{
		return "";
	}

	return m_pControlInfo->states[index]->strNextState.c_str();
}
*/

bool ActionControl::SetStateLoop(const char* state, bool loop)
{
	Assert(state != NULL);

	int index = GetStateIndex(state);

	if (index < 0)
	{
		return false;
	}

	m_pControlInfo->states[index]->bLoop = loop;

	return true;
}

bool ActionControl::GetStateLoop(const char* state) const
{
	Assert(state != NULL);

	int index = GetStateIndex(state);

	if (index < 0)
	{
		return false;
	}

	return m_pControlInfo->states[index]->bLoop;
}

bool ActionControl::SetStateDefaultSpeed(const char* state, float def_speed)
{
	Assert(state != NULL);

	if ((def_speed < 0.01F) || (def_speed > 100.0F))
	{
		return false;
	}

	int index = GetStateIndex(state);

	if (index < 0)
	{
		return false;
	}

	m_pControlInfo->states[index]->fDefaultSpeed = def_speed;

	return true;
}

float ActionControl::GetStateDefaultSpeed(const char* state) const
{
	Assert(state != NULL);

	int index = GetStateIndex(state);

	if (index < 0)
	{
		return 0.0F;
	}

	return m_pControlInfo->states[index]->fDefaultSpeed;
}

bool ActionControl::SetStateCanBreak(const char* state, bool can_break)
{
	Assert(state != NULL);

	int index = GetStateIndex(state);

	if (index < 0)
	{
		return false;
	}

	m_pControlInfo->states[index]->bCanBreak = can_break;

	return true;
}

bool ActionControl::GetStateCanBreak(const char* state) const
{
	Assert(state != NULL);

	int index = GetStateIndex(state);

	if (index < 0)
	{
		return false;
	}

	return m_pControlInfo->states[index]->bCanBreak;
}

bool ActionControl::SetStateDuration(const char* state, float duration)
{
	Assert(state != NULL);

	if (duration > 1024.0F)
	{
		return false;
	}

	int index = GetStateIndex(state);

	if (index < 0)
	{
		return false;
	}

	m_pControlInfo->states[index]->fDuration = duration;

	return true;
}

float ActionControl::GetStateDuration(const char* state) const
{
	Assert(state != NULL);

	int index = GetStateIndex(state);

	if (index < 0)
	{
		return 0.0F;
	}

	return m_pControlInfo->states[index]->fDuration;
}

int ActionControl::GetStateNodeIndex(state_t* pState, 
	const char* node_name) const
{
	Assert(pState != NULL);
	Assert(node_name != NULL);

	size_t node_num = pState->nodes.size();

	for (size_t i = 0; i < node_num; ++i)
	{
		state_node_t* p = pState->nodes[i];

		if (NULL == p)
		{
			continue;
		}

		if (p->strName == node_name)
		{
			return int(i);
		}
	}

	return -1;
}

ActionControl::state_node_t* ActionControl::GetStateNode(const char* state, 
	const char* node_name) const
{
	Assert(state != NULL);
	Assert(node_name != NULL);

	int state_index = GetStateIndex(state);

	if (state_index < 0)
	{
		return NULL;
	}

	state_t* pState = m_pControlInfo->states[state_index];
	int node_index = GetStateNodeIndex(pState, node_name);

	if (node_index < 0)
	{
		return NULL;
	}

	return pState->nodes[node_index];
}

// 状态节点类型名
static const char* s_strStateNodeTypes[] = { 
	"default", "blend", "select", "queue" };

// 获得状态节点类型索引
static int get_state_node_type_index(const char* node_type)
{
	Assert(node_type != NULL);
	
	int node_type_num = sizeof(s_strStateNodeTypes) / sizeof(const char*);
	
	for (int i = 0; i < node_type_num; ++i)
	{
		if (stricmp(s_strStateNodeTypes[i], node_type) == 0)
		{
			return i;
		}
	}

	return -1;
}

// 状态节点同步方式名
static const char* s_strStateNodeSyncModes[] = { 
	"default", "footstep", "startend" };

// 获得状态节点同步方式索引
static int get_state_node_sync_index(const char* sync_mode)
{
	Assert(sync_mode != NULL);

	int num = sizeof(s_strStateNodeSyncModes) / sizeof(const char*);

	for (int i = 0; i < num; ++i)
	{
		if (stricmp(s_strStateNodeSyncModes[i], sync_mode) == 0)
		{
			return i;
		}
	}

	return -1;
}

bool ActionControl::AddStateNode(const char* state, const char* node_name, 
	const char* parent_name, const char* node_type, const char* action, 
	const char* parameter, const char* sync_mode)
{
	Assert(state != NULL);
	Assert(node_name != NULL);
	Assert(parent_name != NULL);
	Assert(node_type != NULL);
	Assert(action != NULL);
	Assert(parameter != NULL);
	Assert(sync_mode != NULL);

	int state_index = GetStateIndex(state);

	if (state_index < 0)
	{
		return false;
	}

	state_t* pState = m_pControlInfo->states[state_index];

	if (GetStateNodeIndex(pState, node_name) >= 0)
	{
		// 已存在
		return false;
	}

	// 节点类型
	int node_type_index = 0;

	if (node_type[0] != 0)
	{
		node_type_index = get_state_node_type_index(node_type);

		if (node_type_index < 0)
		{
			return false;
		}
	}

	// 父节点索引
	int parent_index = -1;

	if (parent_name[0] != 0)
	{
		parent_index = GetStateNodeIndex(pState, parent_name);

		if (parent_index < 0)
		{
			// 父节点不存在
			return false;
		}
	}

	// 控制变量索引
	int param_index = -1;

	if (parameter[0] != 0)
	{
		param_index = GetParameterIndex(parameter);

		if (param_index < 0)
		{
			return false;
		}
	}

	// 同步方式索引
	int sync_mode_index = 0;

	if (sync_mode[0] != 0)
	{
		sync_mode_index = get_state_node_sync_index(sync_mode);

		if (sync_mode_index < 0)
		{
			return false;
		}
	}

	int index = -1;
	size_t node_num = pState->nodes.size();

	for (size_t i = 0; i < node_num; ++i)
	{
		if (NULL == pState->nodes[i])
		{
			index = int(i);
			break;
		}
	}

	if (index < 0)
	{
		index = int(node_num);
	}

	if (parent_index >= 0)
	{
		// 在父节点中添加子节点信息
		state_node_t* pParentNode = pState->nodes[parent_index];

		if (pParentNode->nChildCount >= MAX_NODE_CHILD_NUM)
		{
			return false;
		}

		pParentNode->nChildIndex[pParentNode->nChildCount] = index;
		pParentNode->nChildCount++;
	}

	if (index == int(node_num))
	{
		pState->nodes.push_back(NULL);
	}

	state_node_t* p = CORE_NEW(state_node_t);

	p->strName = node_name;
	p->strAction = action;
	p->nType = node_type_index;
	p->nParamIndex = param_index;
	p->nSyncMode = sync_mode_index;
	p->nParentIndex = parent_index;
	p->nChildCount = 0;
	memset(p->nChildIndex, 0, sizeof(p->nChildIndex));

	pState->nodes[index] = p;

	return true;
}

void ActionControl::DeleteStateNode(state_t* pState, int node_index)
{
	Assert(pState != NULL);
	Assert(size_t(node_index) < pState->nodes.size());

	state_node_t* pNode = pState->nodes[node_index];

	Assert(pNode != NULL);

	// 删除子节点
	for (int i = 0; i < pNode->nChildCount; ++i)
	{
		DeleteStateNode(pState, pNode->nChildIndex[i]);
	}

	CORE_DELETE(pNode);
	pState->nodes[node_index] = NULL;
}

bool ActionControl::RemoveStateNode(const char* state, const char* node_name)
{
	Assert(state != NULL);
	Assert(node_name != NULL);

	int state_index = GetStateIndex(state);

	if (state_index < 0)
	{
		return false;
	}

	state_t* pState = m_pControlInfo->states[state_index];
	int node_index = GetStateNodeIndex(pState, node_name);

	if (node_index < 0)
	{
		return false;
	}

	state_node_t* pNode = pState->nodes[node_index];

	if (pNode->nParentIndex >= 0)
	{
		// 在父节点的子对象列表中移除
		state_node_t* pParentNode = pState->nodes[pNode->nParentIndex];

		Assert(pParentNode != NULL);

		int child_num = pParentNode->nChildCount;
		int* children = pParentNode->nChildIndex;

		for (int i = 0; i < child_num; ++i)
		{
			if (children[i] == node_index)
			{
				memmove(children + i, children + i + 1, 
					(child_num - i - 1) * sizeof(int));
				pParentNode->nChildCount = child_num - 1;;
				break;
			}
		}
	}

	DeleteStateNode(pState, node_index);

	return true;
}

void ActionControl::GetStateNodeList(const IVarList& args, 
	IVarList& result) const
{
	result.Clear();

	const char* state = args.StringVal(0);
	int state_index = GetStateIndex(state);

	if (state_index < 0)
	{
		return;
	}

	state_t* pState = m_pControlInfo->states[state_index];
	size_t node_num = pState->nodes.size();

	for (size_t i = 0; i < node_num; ++i)
	{
		state_node_t* p = pState->nodes[i];

		if (NULL == p)
		{
			continue;
		}

		result.AddString(p->strName.c_str());
	}
}

const char* ActionControl::GetStateNodeParent(const char* state, 
	const char* node_name) const
{
	Assert(state != NULL);
	Assert(node_name != NULL);

	int state_index = GetStateIndex(state);

	if (state_index < 0)
	{
		return "";
	}

	state_t* pState = m_pControlInfo->states[state_index];
	int node_index = GetStateNodeIndex(pState, node_name);

	if (node_index < 0)
	{
		return "";
	}

	state_node_t* pNode = pState->nodes[node_index];

	if (NULL == pNode)
	{
		return "";
	}

	if (pNode->nParentIndex < 0)
	{
		return "";
	}

	state_node_t* pParentNode = pState->nodes[pNode->nParentIndex];

	if (NULL == pParentNode)
	{
		return "";
	}

	return pParentNode->strName.c_str();
}

bool ActionControl::SetStateNodeType(const char* state, const char* node_name, 
	const char* node_type)
{
	Assert(state != NULL);
	Assert(node_name != NULL);
	Assert(node_type != NULL);

	state_node_t* pNode = GetStateNode(state, node_name);

	if (NULL == pNode)
	{
		return false;
	}

	int node_type_index = get_state_node_type_index(node_type);

	if (node_type_index < 0)
	{
		return false;
	}

	pNode->nType = node_type_index;

	return true;
}

const char* ActionControl::GetStateNodeType(const char* state, 
	const char* node_name) const
{
	Assert(state != NULL);
	Assert(node_name != NULL);

	state_node_t* pNode = GetStateNode(state, node_name);

	if (NULL == pNode)
	{
		return "";
	}

	return s_strStateNodeTypes[pNode->nType];
}

bool ActionControl::SetStateNodeAction(const char* state, 
	const char* node_name, const char* action)
{
	Assert(state != NULL);
	Assert(node_name != NULL);
	Assert(action != NULL);

	state_node_t* pNode = GetStateNode(state, node_name);

	if (NULL == pNode)
	{
		return false;
	}

	pNode->strAction = action;

	return true;
}

const char* ActionControl::GetStateNodeAction(const char* state, 
	const char* node_name) const
{
	Assert(state != NULL);
	Assert(node_name != NULL);

	state_node_t* pNode = GetStateNode(state, node_name);

	if (NULL == pNode)
	{
		return "";
	}

	return pNode->strAction.c_str();
}

bool ActionControl::SetStateNodeParameter(const char* state, 
	const char* node_name, const char* parameter)
{
	Assert(state != NULL);
	Assert(node_name != NULL);
	Assert(parameter != NULL);

	state_node_t* pNode = GetStateNode(state, node_name);

	if (NULL == pNode)
	{
		return false;
	}

	int param_index = GetParameterIndex(parameter);

	if (param_index < 0)
	{
		return false;
	}

	pNode->nParamIndex = param_index;

	return true;
}

const char* ActionControl::GetStateNodeParameter(const char* state, 
	const char* node_name) const
{
	Assert(state != NULL);
	Assert(node_name != NULL);
	
	state_node_t* pNode = GetStateNode(state, node_name);

	if (NULL == pNode)
	{
		return "";
	}

	if (pNode->nParamIndex < 0)
	{
		return "";
	}

	parameter_t* pParam = m_pControlInfo->parameters[pNode->nParamIndex];

	if (NULL == pParam)
	{
		return "";
	}

	return pParam->strName.c_str();
}

bool ActionControl::SetStateNodeSyncMode(const char* state, 
	const char* node_name, const char* sync_mode)
{
	Assert(state != NULL);
	Assert(node_name != NULL);
	Assert(sync_mode != NULL);

	state_node_t* pNode = GetStateNode(state, node_name);

	if (NULL == pNode)
	{
		return false;
	}

	int sync_mode_index = get_state_node_sync_index(sync_mode);

	if (sync_mode_index < 0)
	{
		return false;
	}

	pNode->nSyncMode = sync_mode_index;

	return true;
}

const char* ActionControl::GetStateNodeSyncMode(const char* state, 
	const char* node_name) const
{
	Assert(state != NULL);
	Assert(node_name != NULL);

	state_node_t* pNode = GetStateNode(state, node_name);

	if (NULL == pNode)
	{
		return "";
	}

	return s_strStateNodeSyncModes[pNode->nSyncMode];
}

int ActionControl::GetStateConvertIndex(state_t* pState, 
	int command_index) const
{
	Assert(pState != NULL);

	size_t convert_num = pState->converts.size();

	for (size_t i = 0; i < convert_num; ++i)
	{
		if (pState->converts[i].nCommand == command_index)
		{
			return int(i);
		}
	}

	return -1;
}

bool ActionControl::AddStateConvert(const char* state, const char* command,
	const char* to_state, float blend_time)
{
	Assert(state != NULL);
	Assert(command != NULL);
	Assert(to_state != NULL);
	
	int state_index = GetStateIndex(state);

	if (state_index < 0)
	{
		return false;
	}

	int command_index = GetCommandIndex(command);

	if (command_index < 0)
	{
		return false;
	}

	int to_state_index = GetStateIndex(to_state);

	if (to_state_index < 0)
	{
		return false;
	}

	if (blend_time > 60.0F)
	{
		return false;
	}

	state_t* pState = m_pControlInfo->states[state_index];
	int index = GetStateConvertIndex(pState, command_index);

	if (index >= 0)
	{
		// 已存在
		return false;
	}

	convert_t data;

	data.nCommand = command_index;
	data.nToState = to_state_index;
	data.fBlendTime = blend_time;

	pState->converts.push_back(data);
	
	return true;
}

bool ActionControl::RemoveStateConvert(const char* state, const char* command)
{
	Assert(state != NULL);
	Assert(command != NULL);

	int state_index = GetStateIndex(state);

	if (state_index < 0)
	{
		return false;
	}

	int command_index = GetCommandIndex(command);

	if (command_index < 0)
	{
		return false;
	}

	state_t* pState = m_pControlInfo->states[state_index];
	int index = GetStateConvertIndex(pState, command_index);

	if (index < 0)
	{
		return false;
	}

	pState->converts.remove(index);

	return true;
}

void ActionControl::GetStateConvertList(const IVarList& args, 
	IVarList& result) const
{
	result.Clear();

	const char* state = args.StringVal(0);

	int index = GetStateIndex(state);
	
	if (index < 0)
	{
		return;
	}

	state_t* pState = m_pControlInfo->states[index];
	size_t convert_num = pState->converts.size();

	for (size_t i = 0; i < convert_num; ++i)
	{
		int command_index = pState->converts[i].nCommand;
		command_t* pCommand = m_pControlInfo->commands[command_index];

		if (pCommand)
		{
			result.AddString(pCommand->strName.c_str());
		}
		else
		{
			result.AddString("");
		}
	}
}

bool ActionControl::SetStateConvertToState(const char* state, 
	const char* command, const char* to_state)
{
	Assert(state != NULL);
	Assert(command != NULL);
	Assert(to_state != NULL);

	int state_index = GetStateIndex(state);

	if (state_index < 0)
	{
		return false;
	}

	int command_index = GetCommandIndex(command);

	if (command_index < 0)
	{
		return false;
	}

	int to_state_index = GetStateIndex(to_state);

	if (to_state_index < 0)
	{
		return false;
	}

	state_t* pState = m_pControlInfo->states[state_index];
	int index = GetStateConvertIndex(pState, command_index);

	if (index < 0)
	{
		return false;
	}

	pState->converts[index].nToState = to_state_index;

	return true;
}

const char* ActionControl::GetStateConvertToState(const char* state, 
	const char* command) const
{
	Assert(state != NULL);
	Assert(command != NULL);

	int state_index = GetStateIndex(state);

	if (state_index < 0)
	{
		return "";
	}

	int command_index = GetCommandIndex(command);

	if (command_index < 0)
	{
		return "";
	}

	state_t* pState = m_pControlInfo->states[state_index];
	int index = GetStateConvertIndex(pState, command_index);

	if (index < 0)
	{
		return "";
	}

	int to_state_index = pState->converts[index].nToState;

	state_t* pToState = m_pControlInfo->states[to_state_index];

	if (NULL == pToState)
	{
		return "";
	}

	return pToState->strName.c_str();
}

bool ActionControl::SetStateConvertBlendTime(const char* state, 
	const char* command, float blend_time)
{
	Assert(state != NULL);
	Assert(command != NULL);

	int state_index = GetStateIndex(state);

	if (state_index < 0)
	{
		return false;
	}

	int command_index = GetCommandIndex(command);

	if (command_index < 0)
	{
		return false;
	}

	if (blend_time > 60.0F)
	{
		return false;
	}
	
	state_t* pState = m_pControlInfo->states[state_index];
	int index = GetStateConvertIndex(pState, command_index);

	if (index < 0)
	{
		return false;
	}

	pState->converts[index].fBlendTime = blend_time;

	return true;
}

float ActionControl::GetStateConvertBlendTime(const char* state, 
	const char* command) const
{
	Assert(state != NULL);
	Assert(command != NULL);

	int state_index = GetStateIndex(state);

	if (state_index < 0)
	{
		return 0.0F;
	}

	int command_index = GetCommandIndex(command);

	if (command_index < 0)
	{
		return 0.0F;
	}

	state_t* pState = m_pControlInfo->states[state_index];
	int index = GetStateConvertIndex(pState, command_index);

	if (index < 0)
	{
		return 0.0F;
	}

	return pState->converts[index].fBlendTime;
}

