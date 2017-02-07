//--------------------------------------------------------------------
// 文件名:		action_control.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2010年8月7日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "action_control.h"
#include "../visual/i_action_player.h"
#include "../public/inlines.h"

/// \file action_control.cpp
/// \brief 动作控制

/// entity: ActionControl
/// \brief 动作控制实体
DECLARE_ENTITY(ActionControl, 0, IEntity);

/// property: string Name
/// \brief 名称
DECLARE_PROPERTY(const char*, ActionControl, Name, GetName, SetName);
/// property: string State
/// \brief 当前状态
DECLARE_PROPERTY(const char*, ActionControl, State, GetState, SetState);
/// property: string ActionPrefix
/// \brief 动作名前缀
DECLARE_PROPERTY(const char*, ActionControl, ActionPrefix, GetActionPrefix, 
	SetActionPrefix);

	/*
/// method: bool LoadAllAction()
// \brief 加载所有相关动作的数据文件
DECLARE_METHOD_0(bool, ActionControl, LoadAllAction);
// method: bool GetActionLoadFinish()
/// \brief 所有动作数据文件是否加载完成
DECLARE_METHOD_0(bool, ActionControl, GetActionLoadFinish);

/// method: bool AddParameter(string param, float value)
/// \brief 添加控制变量
/// \param param 参数名
/// \param value 参数缺省值
DECLARE_METHOD_2(bool, ActionControl, AddParameter, const char*, float);
/// method: bool RemoveParameter(string param)
/// \brief 删除控制变量
/// \param param 参数名
DECLARE_METHOD_1(bool, ActionControl, RemoveParameter, const char*);
/// method: [table] GetParameterList()
/// \brief 获得控制变量列表
DECLARE_METHOD_T(ActionControl, GetParameterList);

/// method: bool AddCommand(string command)
/// \brief 添加控制命令
/// \param command 命令
DECLARE_METHOD_1(bool, ActionControl, AddCommand, const char*);
/// method: bool RemoveCommand(string command)
/// \brief 删除控制命令
/// \param command 命令
DECLARE_METHOD_1(bool, ActionControl, RemoveCommand, const char*);
/// method: [table] GetCommandList()
/// \brief 获得控制命令列表
DECLARE_METHOD_T(ActionControl, GetCommandList);

/// method: bool AddState(string state, bool loop, float def_speed, 
///		bool can_break, float duration)
/// \brief 添加状态
/// \param state 状态名
/// \param loop 状态是否持续
/// \param def_speed 缺省动作播放速度
/// \param can_break 状态动作是否可以被打断
/// \param duration 状态持续时间（小于等于0表示无明确限制
DECLARE_METHOD_5(bool, ActionControl, AddState, const char*, bool, float, 
	bool, float);
/// method: bool RemoveState(string state)
/// \brief 删除状态
/// \param state 状态名
DECLARE_METHOD_1(bool, ActionControl, RemoveState, const char*);
/// method: [table] GetStateList()
/// \brief 获得状态名列表
DECLARE_METHOD_T(ActionControl, GetStateList);
/// method: bool SetStateNext(string state, string next_state)
/// \brief 设置后续状态
/// \param state 状态名
/// \param next_state 后续状态
//DECLARE_METHOD_2(bool, ActionControl, SetStateNext, const char*, const char*);
/// method: string GetStateNext(string state)
/// \brief 获得后续状态
/// \param state 状态名
//DECLARE_METHOD_1(const char*, ActionControl, GetStateNext, const char*);
/// method: bool SetStateLoop(string state, bool loop)
/// \brief 设置状态是否持续
/// \param state 状态名
/// \param loop 是否循环
DECLARE_METHOD_2(bool, ActionControl, SetStateLoop, const char*, bool);
/// method: bool GetStateLoop(string state)
/// \brief 获得状态是否持续
/// \param state 状态名
DECLARE_METHOD_1(bool, ActionControl, GetStateLoop, const char*);
/// method: bool SetStateDefaultSpeed(string state, float def_speed)
/// \brief 设置状态缺省动作播放速度
/// \param state 状态名
/// \param def_speed 缺省动作播放速度
DECLARE_METHOD_2(bool, ActionControl, SetStateDefaultSpeed, const char*, 
	float);
/// method: float GetStateDefaultSpeed(string state)
/// \brief 获得状态缺省动作播放速度
/// \param state 状态名
DECLARE_METHOD_1(float, ActionControl, GetStateDefaultSpeed, const char*);
/// method: bool SetStateCanBreak(string state, bool can_break)
/// \brief 设置状态动作是否可以被打断
/// \param state 状态名
/// \param can_break 是否可打断
DECLARE_METHOD_2(bool, ActionControl, SetStateCanBreak, const char*, bool);
/// method: bool GetStateCanBreak(string state)
/// \brief 获得状态动作是否可以被打断
/// \param state 状态名
DECLARE_METHOD_1(bool, ActionControl, GetStateCanBreak, const char*);
/// method: bool SetStateDuration(string state, float def_speed)
/// \brief 设置状态持续时间
/// \param state 状态名
/// \param duration 状态持续时间（小于等于0表示无明确限制
DECLARE_METHOD_2(bool, ActionControl, SetStateDuration, const char*, float);
/// method: float GetStateDuration(string state)
/// \brief 获得状态持续时间
/// \param state 状态名
DECLARE_METHOD_1(float, ActionControl, GetStateDuration, const char*);

/// method: bool AddStateNode(string state, string node_name, string parent_name,
///		string node_type, string action, string parameter, string sync_mode)
/// \brief 添加状态节点
/// \param state 状态名
/// \param node_name 节点名
/// \param parent_name 父节点名
/// \param node_type 节点类型["default", "blend", "select", "queue"]
/// \param action 动作名
/// \param parameter 控制变量名
/// \param sync_mode 动作同步方式["default", "footstep", "startend"]
DECLARE_METHOD_7(bool, ActionControl, AddStateNode, const char*, const char*,
	const char*, const char*, const char*, const char*, const char*);
/// method: bool RemoveStateNode(string state, string node_name)
/// \brief 删除状态节点
/// \param state 状态名
/// \param node_name 节点名
DECLARE_METHOD_2(bool, ActionControl, RemoveStateNode, const char*, 
	const char*);
/// method: [table] GetStateNodeList(string state)
/// \brief 获得状态节点名列表
/// \param state 状态名
DECLARE_METHOD_T(ActionControl, GetStateNodeList);
/// method: string GetStateNodeParent(string state, string node_name)
/// \brief 获得状态节点父节点名
/// \param state 状态名
/// \param node_name 节点名
DECLARE_METHOD_2(const char*, ActionControl, GetStateNodeParent, const char*, 
	const char*);
/// method: bool SetStateNodeType(string state, string node_name, string node_type)
/// \brief 设置状态节点类型
/// \param state 状态名
/// \param node_name 节点名
/// \param node_type 节点类型["default", "blend", "select", "queue"]
DECLARE_METHOD_3(bool, ActionControl, SetStateNodeType, const char*, 
	const char*, const char*);
/// method: string GetStateNodeType(string state, string node_name)
/// \brief 获得状态节点类型
/// \param state 状态名
/// \param node_name 节点名
DECLARE_METHOD_2(const char*, ActionControl, GetStateNodeType, const char*, 
	const char*);
/// method: bool SetStateNodeAction(string state, string node_name, string action)
/// \brief 设置状态节点动作
/// \param state 状态名
/// \param node_name 节点名
/// \param action 动作名
DECLARE_METHOD_3(bool, ActionControl, SetStateNodeAction, const char*, 
	const char*, const char*);
/// method: string GetStateNodeAction(string state, string node_name)
/// \brief 获得状态节点动作
/// \param state 状态名
/// \param node_name 节点名
DECLARE_METHOD_2(const char*, ActionControl, GetStateNodeAction, const char*, 
	const char*);
/// method: bool SetStateNodeParameter(string state, string node_name, string parameter)
/// \brief 设置状态节点控制变量名
/// \param state 状态名
/// \param node_name 节点名
/// \param parameter 控制变量名
DECLARE_METHOD_3(bool, ActionControl, SetStateNodeParameter, const char*, 
	const char*, const char*);
/// method: string GetStateNodeParameter(string state, string node_name)
/// \brief 获得状态节点控制变量名
/// \param state 状态名
/// \param node_name 节点名
DECLARE_METHOD_2(const char*, ActionControl, GetStateNodeParameter, 
	const char*, const char*);
/// method: bool SetStateNodeSyncMode(string state, string node_name, string sync_mode)
/// \brief 设置状态节点动作同步方式
/// \param state 状态名
/// \param node_name 节点名
/// \param sync_mode 动作同步方式["default", "footstep", "startend"]
DECLARE_METHOD_3(bool, ActionControl, SetStateNodeSyncMode, const char*, 
	const char*, const char*);
/// method: string GetStateNodeSyncMode(string state, string node_name)
/// \brief 获得状态节点动作同步方式
/// \param state 状态名
/// \param node_name 节点名
DECLARE_METHOD_2(const char*, ActionControl, GetStateNodeSyncMode, const char*, 
	const char*);

/// method: bool AddStateConvert(string state, string command, string to_state, float blend_time)
/// \brief 添加状态转换规则
/// \param state 状态名
/// \param command 控制命令
/// \param to_state 目标状态
/// \param blend_time 动作融合时间
DECLARE_METHOD_4(bool, ActionControl, AddStateConvert, const char*, 
	const char*, const char*, float);
/// method: bool RemoveStateConvert(string state, string command)
/// \brief 删除状态转换规则
/// \param state 状态名
/// \param command 控制命令
DECLARE_METHOD_2(bool, ActionControl, RemoveStateConvert, const char*, 
	const char*);
/// method: [table] GetStateConvertList(string state)
/// \brief 获得状态转换规则列表
/// \param state 状态名
DECLARE_METHOD_T(ActionControl, GetStateConvertList);
/// method: bool SetStateConvertToState(string state, string command, string to_state)
/// \brief 设置控制命令对应的目标状态
/// \param state 状态名
/// \param command 控制命令
/// \param to_state 目标状态
DECLARE_METHOD_3(bool, ActionControl, SetStateConvertToState, const char*,
	const char*, const char*);
/// method: string GetStateConvertToState(string state, string command)
/// \brief 获得控制命令对应的目标状态
/// \param state 状态名
/// \param command 控制命令
DECLARE_METHOD_2(const char*, ActionControl, GetStateConvertToState, 
	const char*, const char*);
/// method: bool SetStateConvertBlendTime(string state, string command, string to_state)
/// \brief 设置控制命令对应的动作融合时间
/// \param state 状态名
/// \param command 控制命令
/// \param blend_time 动作融合时间
DECLARE_METHOD_3(bool, ActionControl, SetStateConvertBlendTime, const char*,
	const char*, float);
/// method: float GetStateConvertBlendTime(string state, string command)
/// \brief 获得控制命令对应的动作融合时间
/// \param state 状态名
/// \param command 控制命令
DECLARE_METHOD_2(float, ActionControl, GetStateConvertBlendTime, 
	const char*, const char*);

/// method: bool SetGlobalSpeed(float speed)
/// \brief 设置整体播放速度
/// \param speed 动作播放速度
DECLARE_METHOD_1(bool, ActionControl, SetGlobalSpeed, float);
/// method: float GetGlobalSpeed()
/// \brief 获得整体速度
DECLARE_METHOD_0(float, ActionControl, GetGlobalSpeed);

/// method: bool SetStateSpeed(string state, float speed)
/// \brief 设置状态动作播放速度
/// \param state 状态名
/// \param speed 动作播放速度
DECLARE_METHOD_2(bool, ActionControl, SetStateSpeed, const char*, float);
/// method: float GetStateSpeed(string state)
/// \brief 获得状态动作播放速度
/// \param state 状态名
DECLARE_METHOD_1(float, ActionControl, GetStateSpeed, const char*);

/// method: bool SetParameterValue(string param, float value)
/// \brief 设置控制变量值
/// \param param 参数名
/// \param value 参数值
DECLARE_METHOD_2(bool, ActionControl, SetParameterValue, const char*, float);
/// method: float GetParameterValue(string param)
/// \brief 获得控制变量值
/// \param param 参数名
DECLARE_METHOD_1(float, ActionControl, GetParameterValue, const char*);

/// method: int GetCurrentFoot()
/// \brief 获得当前的脚步索引
DECLARE_METHOD_0(int, ActionControl, GetCurrentFoot);

/// method: int GetBlendCount()
// \brief 获得当前混合动作数量
DECLARE_METHOD_0(int, ActionControl, GetBlendCount);
/// method: string GetBlendAction(int index)
/// \brief 获得指定索引的混合动作名
/// \param index 混合索引
DECLARE_METHOD_1(const char*, ActionControl, GetBlendAction, int);

/// method: bool EmitCommand(string command)
/// \brief 发出状态控制命令
/// \param command 命令
DECLARE_METHOD_1(bool, ActionControl, EmitCommand, const char*);*/

ActionControl::ActionControl()
{
	m_pActionPlayer = NULL;
	m_pControlInfo = NULL;
	m_nCurrentState = -1;
	m_nFootFlag = -1;
	m_fFootPercent = 0.0F;
	m_fStateTimer = 0.0F;
	m_bStateDeferred = false;
	m_nStateCount = 0;
	m_pStateSpeedValues = NULL;
	m_nParameterCount = 0;
	m_pParameterValues = NULL;
	m_nBlendCount = 0;
	memset(m_BlendInfo, 0, sizeof(m_BlendInfo));
}

ActionControl::~ActionControl()
{
	SAFE_RELEASE(m_pControlInfo);

	if (m_pStateSpeedValues)
	{
		CORE_FREE(m_pStateSpeedValues, sizeof(float) * m_nStateCount);
	}

	if (m_pParameterValues)
	{
		CORE_FREE(m_pParameterValues, sizeof(float) * m_nParameterCount);
	}
}

bool ActionControl::Init(const IVarList& args)
{
	ActionControl* p = (ActionControl*)GetCore()->GetEntity(args.ObjectVal(0));

	if ((p != NULL) 
		&& p->GetEntInfo()->IsKindOf("ActionControl"))
	{
		// 复用动作控制信息
		m_pControlInfo = p->m_pControlInfo;
		m_pControlInfo->IncRefs();
	}
	else
	{
		m_pControlInfo = CORE_NEW(control_info_t);
	}
	
	return true;
}

bool ActionControl::Shut()
{
	return true;
}

void ActionControl::SetActionPlayer(IActionPlayer* value)
{
	Assert(value != NULL);
	
	m_pActionPlayer = value;
}

bool ActionControl::LoadAllAction()
{
	if (NULL == m_pActionPlayer)
	{
		return false;
	}
	
	size_t state_num = m_pControlInfo->states.size();

	for (size_t i = 0; i < state_num; ++i)
	{
		state_t* pState = m_pControlInfo->states[i];

		if (NULL == pState)
		{
			continue;
		}

		size_t node_num = pState->nodes.size();

		for (size_t n = 0; n < node_num; ++n)
		{
			state_node_t* pNode = pState->nodes[n];

			if (NULL == pNode)
			{
				continue;
			}

			if (pNode->strAction.empty())
			{
				continue;
			}

			char buffer[128];

			const char* action_name = GetNodeAction(pNode, buffer, 
				sizeof(buffer));
			
			int action_index = m_pActionPlayer->GetActionIndex(action_name);

			if (action_index < 0)
			{
				continue;
			}

			m_pActionPlayer->LoadAction(action_index);
		}
	}
	
	return true;
}

bool ActionControl::GetActionLoadFinish() const
{
	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	return m_pActionPlayer->IsLoadComplete();
}

void ActionControl::SetActionPrefix(const char* value)
{
	Assert(value != NULL);

	m_strActionPrefix = value;
}

const char* ActionControl::GetActionPrefix() const
{
	return m_strActionPrefix.c_str();
}

bool ActionControl::SetState(const char* state)
{
	Assert(state != NULL);
	
	int state_index = -1;
	
	if (state[0] != 0)
	{
		state_index = GetStateIndex(state);

		if (state_index < 0)
		{
			return false;
		}
	}

	SwitchState(state_index, -1.0F);

	return true;
}

const char* ActionControl::GetState() const
{
	if (m_nCurrentState < 0)
	{
		return "";
	}

	state_t* pState = m_pControlInfo->states[m_nCurrentState];

	if (NULL == pState)
	{
		return "";
	}

	return pState->strName.c_str();
}

float* ActionControl::GetSpeedCurrentValues()
{
	size_t state_num = m_pControlInfo->states.size();

	if (m_nStateCount != state_num)
	{
		float* values = (float*)CORE_ALLOC(sizeof(float) * state_num);

		for (size_t i = 0; i < state_num; ++i)
		{
			state_t* p = m_pControlInfo->states[i];

			if (p)
			{
				values[i] = p->fDefaultSpeed;
			}
			else
			{
				values[i] = 1.0F;
			}
		}

		if (m_pStateSpeedValues)
		{
			CORE_FREE(m_pStateSpeedValues, sizeof(float) * m_nStateCount);
		}

		m_pStateSpeedValues = values;
		m_nStateCount = state_num;
	}

	return m_pStateSpeedValues;
}

bool ActionControl::SetGlobalSpeed(float value)
{
	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	float old_speed = m_pActionPlayer->GetSpeed();

	if (FloatEqual(value, old_speed))
	{
		return true;
	}

	if (!m_pActionPlayer->SetSpeed(value))
	{
		return false;
	}

	state_t* pState = GetCurrentState();

	if (pState)
	{
		StateInputGlobalSpeedChange(pState);
	}
	
	return true;
}

float ActionControl::GetGlobalSpeed()
{
	if (NULL == m_pActionPlayer)
	{
		return 1.0F;
	}

	return m_pActionPlayer->GetSpeed();
}

bool ActionControl::SetStateSpeed(const char* state, float value)
{
	Assert(state != NULL);

	if ((value < 0.01F) || (value > 100.0F))
	{
		return false;
	}
	
	int state_index = GetStateIndex(state);

	if (state_index < 0)
	{
		return false;
	}

	float* speed_values = GetSpeedCurrentValues();

	float old_speed = speed_values[state_index];

	if (FloatEqual(old_speed, value))
	{
		// 没有变化
		return true;
	}

	speed_values[state_index] = value;

	if (state_index == m_nCurrentState)
	{
		state_t* pState = GetCurrentState();
		
		// 动作播放速度改变的事件
		StateInputSpeedChange(pState, value, old_speed);
	}
	
	return true;
}

float ActionControl::GetStateSpeed(const char* state)
{
	Assert(state != NULL);

	int state_index = GetStateIndex(state);

	if (state_index < 0)
	{
		return 0.0F;
	}

	float* speed_values = GetSpeedCurrentValues();

	return speed_values[state_index];
}

float* ActionControl::GetParamCurrentValues()
{
	size_t param_num = m_pControlInfo->parameters.size();

	if (m_nParameterCount != param_num)
	{
		float* values = (float*)CORE_ALLOC(sizeof(float) * param_num);

		for (size_t i = 0; i < param_num; ++i)
		{
			parameter_t* p = m_pControlInfo->parameters[i];
			
			if (p)
			{
				values[i] = p->fDefaultValue;
			}
			else
			{
				values[i] = 0.0F;
			}
		}

		if (m_pParameterValues)
		{
			CORE_FREE(m_pParameterValues, sizeof(float) * m_nParameterCount);
		}

		m_pParameterValues = values;
		m_nParameterCount = param_num;
	}

	return m_pParameterValues;
}

bool ActionControl::SetParameterValue(const char* param, float value)
{
	Assert(param != NULL);

	int index = GetParameterIndex(param);

	if (index < 0)
	{
		return false;
	}

	float* param_values = GetParamCurrentValues();

	float old_param = param_values[index];

	if (FloatEqual(old_param, value))
	{
		return true;
	}

	param_values[index] = value;

	state_t* pState = GetCurrentState();

	if (pState)
	{
		// 控制变量发生改变的事件
		StateInputParamChange(pState, index, old_param);
	}

	return true;
}

float ActionControl::GetParameterValue(const char* param)
{
	Assert(param != NULL);

	int index = GetParameterIndex(param);

	if (index < 0)
	{
		return 0.0F;
	}

	float* param_values = GetParamCurrentValues();

	return param_values[index];
}

// 获得当前帧的脚步索引和百分比
static bool get_foot_percent(IActionPlayer* pActionPlayer, int action_index, 
	int& foot_index, float& percent)
{
	int footstep_num = pActionPlayer->GetActionFootstepCount(action_index);

	if (0 == footstep_num)
	{
		return false;
	}

	float cur_frame = pActionPlayer->GetCurrentFrame(action_index);

	if (cur_frame < 3)
	{
		return false;
	}

	for (int i = 0; i < footstep_num; ++i)
	{
		int frame = pActionPlayer->GetActionFootstepFrame(action_index, i);

		if (frame < 3)
		{
			continue;
		}

		int next_foot = i + 1;

		if (next_foot == footstep_num)
		{
			next_foot = 0;
		}

		int next_frame = pActionPlayer->GetActionFootstepFrame(action_index,
			next_foot);

		if (next_frame < 3)
		{
			int next_foot = i + 1;

			if (next_foot == footstep_num)
			{
				next_foot = 0;
			}

			next_frame = pActionPlayer->GetActionFootstepFrame(action_index,
				next_foot);

			if (next_frame < 3)
			{
				return false;
			}
		}

		if (next_frame <= frame)
		{
			int frame_num = pActionPlayer->GetFrameCount(action_index);

			next_frame += frame_num;
		}

		if ((cur_frame >= float(frame)) && (cur_frame <= float(next_frame)))
		{
			foot_index = i;
			percent = (cur_frame - float(frame)) / float(next_frame - frame);
			return true;
		}
	}

	return false;
}

int ActionControl::GetCurrentFoot()
{
	for (int i = 0; i < m_nBlendCount; ++i)
	{
		if (m_BlendInfo[i].nStartFoot >= 0)
		{
			int foot_index;
			float percent;

			if (get_foot_percent(m_pActionPlayer, m_BlendInfo[i].nActionIndex,
				foot_index, percent))
			{
				return foot_index;
			}
		}
	}
	
	return -1;
}

int ActionControl::GetBlendCount()
{
	return m_nBlendCount;
}

const char* ActionControl::GetBlendAction(int index)
{
	if ((index < 0) || (index >= m_nBlendCount))
	{
		return "";
	}

	int action_index = m_BlendInfo[index].nActionIndex;

	if ((action_index < 0) 
		|| (action_index >= m_pActionPlayer->GetActionCount()))
	{
		return "";
	}

	return m_pActionPlayer->GetActionName(action_index);
}

bool ActionControl::EmitCommand(const char* command)
{
	Assert(command != NULL);

	int command_index = GetCommandIndex(command);

	if (command_index < 0)
	{
		// 未定义的命令
		return false;
	}

	state_t* pState = GetCurrentState();

	if (NULL == pState)
	{
		return false;
	}

	StateInputCommand(pState, command_index);

	return true;
}

void ActionControl::Update(float seconds)
{
	state_t* pState = GetCurrentState();
	
	if (pState)
	{
		StateInputTimer(pState, seconds);
	}
}

ActionControl::state_t* ActionControl::GetCurrentState() const
{
	if (size_t(m_nCurrentState) >= m_pControlInfo->states.size())
	{
		return NULL;
	}

	return m_pControlInfo->states[m_nCurrentState];
}

int ActionControl::GetStateRootNodeIndex(state_t* pState) const
{
	Assert(pState != NULL);

	size_t node_num = pState->nodes.size();

	for (size_t i = 0; i < node_num; ++i)
	{
		state_node_t* pNode = pState->nodes[i];

		if (NULL == pNode)
		{
			continue;
		}

		if (pNode->nParentIndex < 0)
		{
			return int(i);
		}
	}

	return -1;
}

const char* ActionControl::GetNodeAction(state_node_t* pNode, char* buffer, 
	size_t size)
{
	if (pNode->strAction.empty())
	{
		return pNode->strAction.c_str();
	}

	if (m_strActionPrefix.empty())
	{
		return pNode->strAction.c_str();
	}

	SafeSprintf(buffer, size, "%s%s", m_strActionPrefix.c_str(),
		pNode->strAction.c_str());

	return buffer;
}

int ActionControl::SelectActionBlend(state_t* pState, int node_index,
	float weight, blend_info_t* blend_info, int blend_num)
{
	if (blend_num <= 0)
	{
		// 达到混合动作数量上限
		return 0;
	}

	if (weight < 0.001F)
	{
		// 混合权值太小
		return 0;
	}
	
	state_node_t* pNode = pState->nodes[node_index];

	if (NULL == pNode)
	{
		return 0;
	}

	// 控制变量的当前值
	int param_index = pNode->nParamIndex;
	float param_val = 0.0F;

	if (param_index >= 0)
	{
		float* param_values = GetParamCurrentValues();

		param_val = param_values[param_index];
		
		if (param_val < 0.0F)
		{
			param_val = 0.0F;
		}

		if (param_val > 1.0F)
		{
			param_val = 1.0F;
		}
	}

	if (!pNode->strAction.empty())
	{
		char buffer[128];
		const char* action_name = GetNodeAction(pNode, buffer, 
			sizeof(buffer));
		int action_index = m_pActionPlayer->GetActionIndex(action_name);

		if (action_index < 0)
		{
			return 0;
		}

		if (param_index >= 0)
		{
			// 用控制变量调节混合权重
			weight *= param_val;
		}

		// 动作的有效帧数
		int frame_num = m_pActionPlayer->GetFrameCount(action_index);

		blend_info[0].nNodeIndex = node_index;
		blend_info[0].nSyncMode = GetNodeSyncMode(pState, node_index);
		blend_info[0].nActionIndex = action_index;
		blend_info[0].nStartFoot = -1;
		blend_info[0].nCurrentFoot = -1;
		blend_info[0].nStartFrame = 0;
		blend_info[0].nFrameCount = 0;
		blend_info[0].nTotalFrame = frame_num;
		blend_info[0].fStartPercent = 0.0F;
		blend_info[0].fWeight = weight;
		blend_info[0].fSpeed = 1.0F;
		blend_info[0].fEndTime = 0.0F;
		blend_info[0].bFootEnded = false;
		blend_info[0].bLoop = pState->bLoop || (pState->fDuration > 0.0F);
		blend_info[0].bBlended = false;

		return 1;
	}

	int count = 0;
	int child_num = pNode->nChildCount;

	for (int i = 0; i < child_num; ++i)
	{
		float child_weight = weight;

		if (param_index >= 0)
		{
			int node_type = pNode->nType;

			if (node_type == NODE_TYPE_BLEND)
			{
				// 从几个动作里选择两个动作进行混合
				if (child_num > 1)
				{
					float factor = param_val * float(child_num - 1);
					int selector = int(factor);

					if (selector < 0)
					{
						selector = 0;
					}

					if (selector > (child_num - 2))
					{
						selector = child_num - 2;
					}

					if (i == selector)
					{
						child_weight *= float(selector + 1) - factor;
					}
					else if (i == (selector + 1))
					{
						child_weight *= factor - float(selector);
					}
					else
					{
						child_weight = 0.0F;
					}
				}
			}
			else if (node_type == NODE_TYPE_SELECT)
			{
				// 从几个动作里选择一个动作
				int selector = int(param_val * float(child_num));

				if (selector < 0)
				{
					selector = 0;
				}
				
				if (selector >= child_num)
				{
					selector = child_num - 1;
				}

				if (i != selector)
				{
					child_weight = 0.0F;
				}
			}
			else if (node_type == NODE_TYPE_QUEUE)
			{
				// 序列播放
				if (i != 0)
				{
					// 先播放第一个动作
					child_weight = 0.0F;
				}
			}
		}
		else
		{
			// 序列播放
			if (i != 0)
			{
				// 先播放第一个动作
				child_weight = 0.0F;
			}
		}

		if (child_weight < 0.001F)
		{
			continue;
		}
		
		int num = SelectActionBlend(pState, pNode->nChildIndex[i], 
			child_weight, blend_info, blend_num);

		blend_info += num;
		blend_num -= num;
		count += num;
	}

	return count;
}

bool ActionControl::ApplyActionBlend(state_t* pState, 
	blend_info_t* pBlendInfo, float blend_time)
{
	Assert(pState != NULL);
	Assert(pBlendInfo != NULL);

	int action_index = pBlendInfo->nActionIndex;
	int start_frame = pBlendInfo->nStartFrame;
	int frame_num = pBlendInfo->nFrameCount;
	float percent = pBlendInfo->fStartPercent;
	float weight = pBlendInfo->fWeight;
	float speed = pBlendInfo->fSpeed;
	bool loop = pBlendInfo->bLoop;

	m_pActionPlayer->BlendAction(action_index, loop, false, true, weight, 
		"",  "");

	if (blend_time >= 0.0F)
	{
		m_pActionPlayer->SetBlendActionEnterTime(action_index, blend_time);
	}

	float old_speed = m_pActionPlayer->GetActionSpeed(action_index);
	
	if (!FloatEqual(speed, old_speed))
	{
		// 设置播放速度
		m_pActionPlayer->SetBlendActionSpeed(action_index, speed);
	}

	// 预估的动作结束时间
	float global_speed = m_pActionPlayer->GetSpeed();
	float fps = m_pActionPlayer->GetInternalFPS(action_index);
	float t = float(frame_num) / (fps * speed * global_speed);

	if (percent > 0.0F)
	{
		// 与其他动作在相同的百分比位置开始播放
		int total_frame = m_pActionPlayer->GetFrameCount(action_index);
		float frame = start_frame + frame_num * percent;

		if (frame >= float(total_frame + 3))
		{
			frame = frame - total_frame;
		}

		m_pActionPlayer->SetCurrentFrame(action_index, frame);

		t *= (1.0F - percent);
	}
	else if (start_frame > 3)
	{
		// 从指定的起始帧开始播放
		m_pActionPlayer->SetCurrentFrame(action_index, float(start_frame));
	}

	pBlendInfo->fEndTime = m_fStateTimer + t;
	pBlendInfo->bBlended = true;

	return true;
}

int ActionControl::GetQueueNextNode(state_t* pState, int node_index, 
	bool loop)
{
	Assert(pState != NULL);

	state_node_t* pNode = pState->nodes[node_index];

	if (NULL == pNode)
	{
		return -1;
	}

	int parent_index = pNode->nParentIndex;

	if (parent_index < 0)
	{
		return -1;
	}

	state_node_t* pParentNode = pState->nodes[parent_index];

	if (NULL == pParentNode)
	{
		return -1;
	}

	if (pParentNode->nType != NODE_TYPE_QUEUE)
	{
		return -1;
	}

	int child_num = pParentNode->nChildCount;

	Assert(child_num > 0);

	for (int i = 0; i < child_num; ++i)
	{
		if (pParentNode->nChildIndex[i] == node_index)
		{
			int next_child = i + 1;

			if (next_child == child_num)
			{
				if (!loop)
				{
					return -1;
				}

				next_child = 0;
			}

			return pParentNode->nChildIndex[next_child];
		}
	}

	return -1;
}

int ActionControl::GetNodeSyncMode(state_t* pState, int node_index)
{
	Assert(pState != NULL);

	state_node_t* pNode = pState->nodes[node_index];
	
	int sync_mode = pNode->nSyncMode;

	if (0 == sync_mode)
	{
		int parent_index = pNode->nParentIndex;

		if (parent_index >= 0)
		{
			state_node_t* pParentNode = pState->nodes[parent_index];

			if (pParentNode)
			{
				// 使用父节点的动作同步模式
				sync_mode = pParentNode->nSyncMode;
			}
		}
	}

	return sync_mode;
}

bool ActionControl::GetStartEndFrame(int action_index, int& start_frame, 
	int& frame_num)
{
	// 此动作的有效播放帧数
	int total_frames = m_pActionPlayer->GetFrameCount(action_index);

	if (total_frames > 0)
	{
		start_frame = m_pActionPlayer->GetActionStartFarme(action_index);

		int end_frame = m_pActionPlayer->GetActionEndFarme(action_index);

		if ((start_frame < 3) || (end_frame < 3))
		{
			start_frame = 3;
			frame_num = total_frames;
			return true;
		}

		if (end_frame > (3 + total_frames))
		{
			end_frame = 3 + total_frames;
		}

		if (end_frame < start_frame)
		{
			end_frame = start_frame;
		}

		frame_num = end_frame - start_frame;
	}
	else
	{
		start_frame = 3;
		frame_num = 0;
	}

	return true;
}

bool ActionControl::GetNextFootFrame(int action_index, int foot_flag, 
	int current_foot, int& foot_index, int& start_frame, int& frame_num)
{
	int footstep_num = m_pActionPlayer->GetActionFootstepCount(action_index);

	if (0 == footstep_num)
	{
		return false;
	}

	for (int i = 0; i < footstep_num; ++i)
	{
		// 当前脚步的下一个
		int foot = (current_foot + 1 + i) % footstep_num;
		
		int frame = m_pActionPlayer->GetActionFootstepFrame(action_index, 
			foot);

		if (frame < 3)
		{
			continue;
		}

		if (foot_flag >= 0)
		{
			// 是否与当前期望的脚步匹配
			if ((foot & 0x1) != foot_flag)
			{
				continue;
			}
		}

		// 下一个脚步的索引
		int next_foot = foot + 1;

		if (next_foot == footstep_num)
		{
			next_foot = 0;
		}

		if (next_foot == foot)
		{
			return false;
		}

		// 下一个脚步的帧
		int next_frame = m_pActionPlayer->GetActionFootstepFrame(action_index, 
			next_foot);

		if (next_frame == frame)
		{
			return false;
		}

		if (next_frame < 3)
		{
			next_foot = next_foot + 1;
			
			if (next_foot == footstep_num)
			{
				next_foot = 0;
			}
			
			if (next_foot == foot)
			{
				return false;
			}

			next_frame = m_pActionPlayer->GetActionFootstepFrame(action_index, 
				next_foot);

			if (next_frame < 3)
			{
				return false;
			}

			if (next_frame == frame)
			{
				return false;
			}
		}

		foot_index = foot;
		start_frame = frame;

		if (next_frame > start_frame)
		{
			frame_num = next_frame - start_frame;
		}
		else
		{
			int total_frames = m_pActionPlayer->GetFrameCount(action_index);

			frame_num = total_frames - start_frame + next_frame;

			if (frame_num < 1)
			{
				frame_num = 1;
			}
		}

		return true;
	}
	
	return false;
}

bool ActionControl::GetBlendAllEnded()
{
	for (int i = 0; i < m_nBlendCount; ++i)
	{
		if (!m_BlendInfo[i].bBlended)
		{
			continue;
		}

		if (m_fStateTimer < m_BlendInfo[i].fEndTime)
		{
			return false;
		}
	}

	return true;
}

bool ActionControl::PreprocessBlend(state_t* pState)
{
	float* speed_values = GetSpeedCurrentValues();
	float state_speed = speed_values[m_nCurrentState];
	//bool state_loop = pState->bLoop;
	int first_foot = -1;
	float total_weight = 0.0F;

	for (int k = 0; k < m_nBlendCount; ++k)
	{
		int action_index = m_BlendInfo[k].nActionIndex;
		int sync_mode = m_BlendInfo[k].nSyncMode;

		if (sync_mode == SYNC_MODE_FOOTSTEP)
		{
			int foot_index;
			int start_frame;
			int frame_num;

			if (GetNextFootFrame(action_index, m_nFootFlag, -1, 
				foot_index, start_frame, frame_num))
			{
				if (first_foot < 0)
				{
					first_foot = foot_index;
					
					if (m_nFootFlag < 0)
					{
						m_nFootFlag = first_foot & 0x1;
						m_fFootPercent = 0.0F;
					}
				}

				m_BlendInfo[k].nStartFoot = foot_index;
				m_BlendInfo[k].nCurrentFoot = foot_index;
				m_BlendInfo[k].nStartFrame = start_frame;
				m_BlendInfo[k].nFrameCount = frame_num;
				m_BlendInfo[k].fStartPercent = m_fFootPercent;
			}
			else
			{
				frame_num = m_pActionPlayer->GetFrameCount(action_index);
				
				m_BlendInfo[k].nStartFrame = 3;
				m_BlendInfo[k].nFrameCount = frame_num;
			}
		}
		else
		{
			int start_frame;
			int frame_num;

			GetStartEndFrame(action_index, start_frame, frame_num);

			m_BlendInfo[k].nStartFrame = start_frame;
			m_BlendInfo[k].nFrameCount = frame_num;
		}

		m_BlendInfo[k].fSpeed = state_speed;
		//m_BlendInfo[k].bLoop = state_loop;

		total_weight += m_BlendInfo[k].fWeight;
	}

	if (m_nBlendCount >= 2)
	{
		// 计算平均播放帧数量
		float avg_frame_num = 0.0F;

		for (int i = 0; i < m_nBlendCount; ++i)
		{
			float weight = m_BlendInfo[i].fWeight / total_weight;

			//avg_frame_num += float(m_BlendInfo[i].nFrameCount) * weight;
			avg_frame_num += float(m_BlendInfo[i].nTotalFrame) * weight;
		}

		if (avg_frame_num >= 1.0F)
		{
			for (int j = 0; j < m_nBlendCount; ++j)
			{
				// 调节动作的播放速度以使脚步一致
				//m_BlendInfo[j].fSpeed *= float(m_BlendInfo[j].nFrameCount) 
				//	/ avg_frame_num;
				m_BlendInfo[j].fSpeed *= float(m_BlendInfo[j].nTotalFrame) 
					/ avg_frame_num;
			}
		}
	}

	if (first_foot >= 0)
	{
		// 变换左右脚标志
		m_nFootFlag = (m_nFootFlag + 1) & 0x1;
		m_fFootPercent = 0.0F;
	}
	else
	{
		// 不再使用脚步同步时复位脚步序列
		m_nFootFlag = -1;
		m_fFootPercent = 0.0F;
	}

	return true;
}

bool ActionControl::UpdateEndTime(blend_info_t* pBlendInfo)
{
	Assert(pBlendInfo != NULL);

	int action_index = pBlendInfo->nActionIndex;
	float cur_frame = m_pActionPlayer->GetCurrentFrame(action_index);
	
	if (cur_frame < 0)
	{
		// 动作已经播放结束
		pBlendInfo->fEndTime = m_fStateTimer;
		return true;
	}
	
	float start_frame = float(pBlendInfo->nStartFrame);
	float frame_num = float(m_pActionPlayer->GetFrameCount(action_index));
	float fps = m_pActionPlayer->GetInternalFPS(action_index);
	float global_speed = m_pActionPlayer->GetSpeed();

	// 已经播放的帧数
	float play_frames;

	if (cur_frame >= start_frame)
	{
		play_frames = cur_frame - start_frame;
	}
	else
	{
		play_frames = cur_frame + frame_num - start_frame;
	}

	// 还剩下多少帧未播放
	float frames = float(pBlendInfo->nFrameCount) - play_frames;

	if (frames < 0.0F)
	{
		frames = 0.0F;
	}

	float t = frames / (fps * pBlendInfo->fSpeed * global_speed);

	pBlendInfo->fEndTime = m_fStateTimer + t;

	return true;
}

bool ActionControl::SwitchState(int new_state_index, float blend_time)
{
	m_bStateDeferred = false;
	
	state_t* pOldState = GetCurrentState();
	
	if (pOldState)
	{
		if (!pOldState->bCanBreak && !GetBlendAllEnded())
		{
			// 等待本状态的一轮动作播完再播放新状态的动作
			m_bStateDeferred = true;
		}

		StateInputLeave(pOldState, blend_time);
	}
	
	m_nCurrentState = new_state_index;
	
	if (!m_bStateDeferred)
	{
		m_fStateTimer = 0.0F;
	}
	
	if (new_state_index >= 0)
	{
		state_t* pNewState = m_pControlInfo->states[new_state_index];
		
		StateInputEntry(pNewState, blend_time);
	}
	else
	{
		// 状态置为空时复位脚步序列
		m_nFootFlag = -1;
		m_fFootPercent = 0.0F;
	}
	
	return true;
} 

bool ActionControl::StateInputEntry(state_t* pState, float blend_time)
{
	Assert(pState != NULL);

	if (m_bStateDeferred)
	{
		// 延迟播放新的动作
		return true;
	}
	
	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	int root_node_index = GetStateRootNodeIndex(pState);

	if (root_node_index < 0)
	{
		return false;
	}

	m_nBlendCount = SelectActionBlend(pState, root_node_index, 1.0F, 
		m_BlendInfo, MAX_BLEND_ACTION_NUM);

	if (0 == m_nBlendCount)
	{
		return false;
	}

	PreprocessBlend(pState);

	for (int i = 0; i < m_nBlendCount; ++i)
	{
		if (!m_BlendInfo[i].bBlended)
		{
			ApplyActionBlend(pState, &m_BlendInfo[i], blend_time);
		}
	}

	return true;
}

bool ActionControl::StateInputLeave(state_t* pState, float blend_time)
{
	Assert(pState != NULL);

	if (m_bStateDeferred)
	{
		// 延迟停止当前的动作
		return false;
	}
	
	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	for (int i = 0; i < m_nBlendCount; ++i)
	{
		if (!m_BlendInfo[i].bBlended)
		{
			continue;
		}
		
		int action_index = m_BlendInfo[i].nActionIndex;

		if (m_BlendInfo[i].nStartFoot >= 0)
		{
			// 获得当前脚步帧的播放比例
			int foot_index;
			float percent;

			if (get_foot_percent(m_pActionPlayer, action_index, foot_index, 
				percent))
			{
				// 记录脚步信息以便于下一个状态平滑过渡
				m_nFootFlag = foot_index & 0x1;
				m_fFootPercent = percent;
			}
		}

		if (blend_time >= 0)
		{
			m_pActionPlayer->SetBlendActionLeaveTime(action_index, blend_time);
		}

		m_pActionPlayer->UnblendAction(action_index);
	}

	m_nBlendCount = 0;

	return true;
}

bool ActionControl::StateInputTimer(state_t* pState, float seconds)
{
	Assert(pState != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	m_fStateTimer += seconds;

	if (m_bStateDeferred)
	{
		// 所有前一状态的动作是否结束
		if (GetBlendAllEnded())
		{
			// 清除前一个状态的所有动作
			for (int i = 0; i < m_nBlendCount; ++i)
			{
				if (m_BlendInfo[i].bBlended)
				{
					int action_index = m_BlendInfo[i].nActionIndex;

					m_pActionPlayer->UnblendAction(action_index);
				}
			}

			m_nBlendCount = 0;

			// 复位状态计时
			m_fStateTimer = 0.0F;
			m_bStateDeferred = false;

			// 融入新的动作
			StateInputEntry(pState, -1.0F);
		}

		return true;
	}

	// 是否某个脚步已经结束
	bool any_foot_ended = false;

	for (int i = 0; i < m_nBlendCount; ++i)
	{
		if (!m_BlendInfo[i].bBlended)
		{
			continue;
		}

		if (m_BlendInfo[i].nStartFoot < 0)
		{
			continue;
		}

		if (m_fStateTimer >= m_BlendInfo[i].fEndTime)
		{
			// 脚步结束
			any_foot_ended = true;
			break;
		}
	}

	if (any_foot_ended)
	{
		//float total_weight = 0.0F;

		for (int k = 0; k < m_nBlendCount; ++k)
		{
			if (!m_BlendInfo[k].bBlended)
			{
				continue;
			}
			
			if (m_BlendInfo[k].nStartFoot < 0)
			{
				continue;
			}

			int action_index = m_BlendInfo[k].nActionIndex;
			int current_foot = m_BlendInfo[k].nCurrentFoot;

			int foot_index;
			int start_frame;
			int frame_num;

			// 获得下一脚步的帧信息
			if (GetNextFootFrame(action_index, m_nFootFlag, current_foot, 
				foot_index, start_frame, frame_num))
			{
				if (foot_index == m_BlendInfo[k].nStartFoot)
				{
					m_BlendInfo[k].bFootEnded = true;
				}

				/*
				// 比较当前播放到的帧和脚步起始帧
				int frame_num = m_pActionPlayer->GetFrameCount(action_index);
				float cur_frame = m_pActionPlayer->GetCurrentFrame(
					action_index);

				float delta = cur_frame - float(start_frame);

				if (delta > (frame_num * 0.5F))
				{
					delta -= float(frame_num);
				}

				if (delta < -(frame_num * 0.5F))
				{
					delta += float(frame_num);
				}

				if ((delta > 3.0F) || (delta < -3.0F))
				{
					// 修正过大的偏差
					m_pActionPlayer->SetCurrentFrame(action_index, 
						float(start_frame));
				}
				*/

				m_BlendInfo[k].nCurrentFoot = foot_index;
				m_BlendInfo[k].nStartFrame = start_frame;
				m_BlendInfo[k].nFrameCount = frame_num;
				
				float global_speed = m_pActionPlayer->GetSpeed();
				float speed = m_BlendInfo[k].fSpeed;
				float fps = m_pActionPlayer->GetInternalFPS(action_index);
				float t = float(frame_num) / (fps * speed * global_speed);

				// 预估的下一脚步结束时间（加到上个脚步的结束时间以避免累积误差）
				m_BlendInfo[k].fEndTime += t;
			}
			else
			{
				// 结束脚步
				m_BlendInfo[k].bFootEnded = true;
			}

			//total_weight += m_BlendInfo[k].fWeight;
		}
		
		// 不再进行速度调整，因为可能有时间差的问题
		/*
		if (m_nBlendCount >= 2)
		{
			// 计算平均播放帧数量
			float avg_frame_num = 0.0F;

			for (int i = 0; i < m_nBlendCount; ++i)
			{
				if (!m_BlendInfo[i].bBlended)
				{
					continue;
				}

				if (m_BlendInfo[i].nStartFoot < 0)
				{
					continue;
				}

				float weight = m_BlendInfo[i].fWeight / total_weight;

				avg_frame_num += float(m_BlendInfo[i].nFrameCount) * weight;
			}

			if (avg_frame_num >= 1.0F)
			{
				float* speed_values = GetSpeedCurrentValues();
				float state_speed = speed_values[m_nCurrentState];

				for (int j = 0; j < m_nBlendCount; ++j)
				{
					if (!m_BlendInfo[j].bBlended)
					{
						continue;
					}

					if (m_BlendInfo[j].nStartFoot < 0)
					{
						continue;
					}

					// 调节动作的播放速度以使脚步一致
					m_BlendInfo[j].fSpeed = state_speed;
					m_BlendInfo[j].fSpeed *= float(m_BlendInfo[j].nFrameCount) 
						/ avg_frame_num;

					m_pActionPlayer->SetBlendActionSpeed(
						m_BlendInfo[j].nActionIndex, m_BlendInfo[j].fSpeed);
					
					UpdateEndTime(&m_BlendInfo[j]);
				}
			}
		}
		*/

		// 变换左右脚标志
		m_nFootFlag = (m_nFootFlag + 1) & 0x1;
		m_fFootPercent = 0.0F;
	}

	// 是否所有的动作都已经结束
	bool all_ended = true;

	for (int j = 0; j < m_nBlendCount; ++j)
	{
		if (!m_BlendInfo[j].bBlended)
		{
			continue;
		}

		if (m_BlendInfo[j].nStartFoot < 0)
		{
			if (m_fStateTimer < m_BlendInfo[j].fEndTime)
			{
				all_ended = false;
			}
		}
		else
		{
			if (!m_BlendInfo[j].bFootEnded)
			{
				all_ended = false;
			}
		}
	}

	if (all_ended)
	{
		for (int k = 0; k < m_nBlendCount; ++k)
		{
			if (!m_BlendInfo[k].bBlended)
			{
				continue;
			}

			// 获得序列播放的下一个动作
			int next_node_index = GetQueueNextNode(pState, 
				m_BlendInfo[k].nNodeIndex, pState->bLoop);

			if (next_node_index >= 0)
			{
				state_node_t* pNode = pState->nodes[next_node_index];

				char buffer[128];

				const char* action_name = GetNodeAction(pNode, buffer, 
					sizeof(buffer));

				int action_index = m_pActionPlayer->GetActionIndex(
					action_name);

				if (action_index >= 0)
				{
					m_pActionPlayer->UnblendAction(
						m_BlendInfo[k].nActionIndex);

					int frame_num = m_pActionPlayer->GetFrameCount(
						action_index);

					// 混合权值保持不变
					m_BlendInfo[k].nNodeIndex = next_node_index;
					m_BlendInfo[k].nSyncMode = GetNodeSyncMode(pState, 
						next_node_index);
					m_BlendInfo[k].nActionIndex = action_index;
					m_BlendInfo[k].nStartFoot = -1;
					m_BlendInfo[k].nCurrentFoot = -1;
					m_BlendInfo[k].nStartFrame = 0;
					m_BlendInfo[k].nFrameCount = 0;
					m_BlendInfo[k].nTotalFrame = frame_num;
					m_BlendInfo[k].fStartPercent = 0.0F;
					m_BlendInfo[k].fSpeed = 1.0F;
					m_BlendInfo[k].fEndTime = 0.0F;
					m_BlendInfo[k].bFootEnded = false;
					m_BlendInfo[k].bLoop = false;
					m_BlendInfo[k].bBlended = false;

					all_ended = false;

					continue;
				}
			}

			//if (!pState->bLoop)
			if (!m_BlendInfo[k].bLoop)
			{
				m_pActionPlayer->UnblendAction(m_BlendInfo[k].nActionIndex);
				m_BlendInfo[k].bBlended = false;
			}
		}

		if (!all_ended)
		{
			// 开始播放下一轮的动作
			PreprocessBlend(pState);

			for (int n = 0; n < m_nBlendCount; ++n)
			{
				if (!m_BlendInfo[n].bBlended)
				{
					ApplyActionBlend(pState, &m_BlendInfo[n], -1.0F);
				}
			}
		}
	}

	bool finished = false;

	if (pState->fDuration > 0.0F)
	{
		// 是否到达限定持续时间
		if (m_fStateTimer >= pState->fDuration)
		{
			finished = true;
		}
	}
	else
	{
		if (all_ended && (!pState->bLoop)) 
		{
			finished = true;
		}
	}

	if (finished)
	{
		// 状态时限到，结束状态
		StateInputCommand(pState, END_COMMAND_INDEX);
		//int next_state_index = GetStateIndex(pState->strNextState.c_str());

		//if (next_state_index >= 0)
		//{
		//	// 切换到设定的下一个状态
		//	SwitchState(next_state_index);
		//}
	}

	return true;
}

bool ActionControl::StateInputGlobalSpeedChange(state_t* pState)
{
	Assert(pState != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	for (int i = 0; i < m_nBlendCount; ++i)
	{
		if (m_BlendInfo[i].bBlended)
		{
			UpdateEndTime(&m_BlendInfo[i]);
		}
	}

	return true;
}

bool ActionControl::StateInputSpeedChange(state_t* pState, float new_speed, 
	float old_speed)
{
	Assert(pState != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	float factor = new_speed / old_speed;

	for (int i = 0; i < m_nBlendCount; ++i)
	{
		m_BlendInfo[i].fSpeed *= factor;

		if (m_BlendInfo[i].bBlended)
		{
			m_pActionPlayer->SetBlendActionSpeed(m_BlendInfo[i].nActionIndex, 
				m_BlendInfo[i].fSpeed);

			// 重新计算动作结束时间
			UpdateEndTime(&m_BlendInfo[i]);
		}
	}

	return true;
}

int ActionControl::GetBlendInfoIndex(int node_index, blend_info_t* blend_info, 
	int blend_num)
{
	Assert(blend_info != NULL);
	
	for (int i = 0; i < blend_num; ++i)
	{
		if (blend_info[i].nNodeIndex == node_index)
		{
			return i;
		}
	}

	return -1;
}

// 获得动作已经播放的百分比
static float get_play_percent(IActionPlayer* pActionPlayer, int action_index, 
	int start_frame, int frame_num)
{
	float cur_frame = pActionPlayer->GetCurrentFrame(action_index);

	if (cur_frame < 3)
	{
		return 0.0F;
	}

	if (cur_frame < float(start_frame))
	{
		cur_frame += pActionPlayer->GetFrameCount(action_index);
	}

	//int end_frame = start_frame + frame_num;

	//if (cur_frame > float(end_frame))
	//{
	//	return 1.0F;
	//}

	return (cur_frame - float(start_frame)) / float(frame_num);
}

bool ActionControl::StateInputParamChange(state_t* pState, int param_index, 
	float old_param)
{
	Assert(pState != NULL);

	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	// 当前状态是否使用此参数
	bool use_this_param = false;

	size_t node_num = pState->nodes.size();

	for (size_t n = 0; n < node_num; ++n)
	{
		state_node_t* pNode = pState->nodes[n];

		if (NULL == pNode)
		{
			continue;
		}

		if (pNode->nParamIndex == param_index)
		{
			use_this_param = true;
			break;
		}
	}

	if (!use_this_param)
	{
		// 与当前状态无关
		return true;
	}

	int root_node_index = GetStateRootNodeIndex(pState);

	if (root_node_index < 0)
	{
		return false;
	}

	blend_info_t new_blend_info[MAX_BLEND_ACTION_NUM];

	int new_blend_num = SelectActionBlend(pState, root_node_index, 1.0F, 
		new_blend_info, MAX_BLEND_ACTION_NUM);

	// 混合权值是否有变化
	bool weight_changed = false;
	// 当前动作播放的百分比
	float percent = 0.0F;

	for (int i = 0; i < m_nBlendCount; ++i)
	{
		if (!m_BlendInfo[i].bBlended)
		{
			continue;
		}

		percent = get_play_percent(m_pActionPlayer, 
			m_BlendInfo[i].nActionIndex, m_BlendInfo[i].nStartFrame, 
			m_BlendInfo[i].nFrameCount);

		int blend_index = GetBlendInfoIndex(m_BlendInfo[i].nNodeIndex,
			new_blend_info, new_blend_num);

		if (blend_index < 0)
		{
			// 此动作不需要再播放
			m_pActionPlayer->UnblendAction(m_BlendInfo[i].nActionIndex);

			// 删除混合信息 
			memmove(m_BlendInfo + i, m_BlendInfo + i + 1,
				sizeof(blend_info_t) * (m_nBlendCount - i - 1));
			--m_nBlendCount;
			--i;
			continue;
		}

		// 新的混合权值
		float new_weight = new_blend_info[blend_index].fWeight;

		if (!FloatEqual(m_BlendInfo[i].fWeight, new_weight))
		{
			// 改变动作的权值
			m_BlendInfo[i].fWeight = new_weight;

			m_pActionPlayer->SetBlendActionWeight(
				m_BlendInfo[i].nActionIndex, new_weight);

			weight_changed = true;
		}
	}

	float* speed_values = GetSpeedCurrentValues();
	float state_speed = speed_values[m_nCurrentState];

	for (int k = 0; k < new_blend_num; ++k)
	{
		int blend_index = GetBlendInfoIndex(new_blend_info[k].nNodeIndex,
			m_BlendInfo, m_nBlendCount);

		if (blend_index >= 0)
		{
			// 已经播放的动作
			continue;
		}

		if (m_nBlendCount >= MAX_BLEND_ACTION_NUM)
		{
			// 太多动作无法播放
			continue;
		}

		int index = m_nBlendCount;

		// 添加新的混合信息
		memcpy(&m_BlendInfo[index], &new_blend_info[k], sizeof(blend_info_t));
		++m_nBlendCount;

		int action_index = m_BlendInfo[index].nActionIndex;
		int sync_mode = m_BlendInfo[index].nSyncMode;

		if (sync_mode == SYNC_MODE_FOOTSTEP)
		{
			// 使用上一个脚步的标志
			int foot_flag = (m_nFootFlag + 1) & 0x1;
			
			int foot_index;
			int start_frame;
			int frame_num;

			if (GetNextFootFrame(action_index, foot_flag, -1, 
				foot_index, start_frame, frame_num))
			{
				m_BlendInfo[index].nStartFoot = foot_index;
				m_BlendInfo[index].nCurrentFoot = foot_index;
				m_BlendInfo[index].nStartFrame = start_frame;
				m_BlendInfo[index].nFrameCount = frame_num;
			}
			else
			{
				frame_num = m_pActionPlayer->GetFrameCount(action_index);
				
				m_BlendInfo[index].nStartFrame = 3;
				m_BlendInfo[index].nFrameCount = frame_num;
			}
		}
		else
		{
			int start_frame;
			int frame_num;

			GetStartEndFrame(action_index, start_frame, frame_num);

			m_BlendInfo[index].nStartFrame = start_frame;
			m_BlendInfo[index].nFrameCount = frame_num;
		}

		m_BlendInfo[index].fSpeed = state_speed;
		//m_BlendInfo[index].bLoop = pState->bLoop;
		// 与其他动作在相同的百分比位置开始播放
		m_BlendInfo[index].fStartPercent = percent;

		ApplyActionBlend(pState, &m_BlendInfo[index], -1.0F);

		weight_changed = true;
	}

	// 无论动作的多少，都需要重置播放速度
	if (weight_changed)
	{
		float total_weight = 0.0F;

		for (int k = 0; k < m_nBlendCount; ++k)
		{
			if (!m_BlendInfo[k].bBlended)
			{
				continue;
			}
			
			total_weight += m_BlendInfo[k].fWeight;
		}
		
		// 计算平均播放帧数量
		float avg_frame_num = 0.0F;

		for (int i = 0; i < m_nBlendCount; ++i)
		{
			if (!m_BlendInfo[i].bBlended)
			{
				continue;
			}

			float weight = m_BlendInfo[i].fWeight / total_weight;

			avg_frame_num += float(m_BlendInfo[i].nTotalFrame) * weight;
			//avg_frame_num += float(m_BlendInfo[i].nFrameCount) * weight;
		}

		if (avg_frame_num >= 1.0F)
		{
			for (int j = 0; j < m_nBlendCount; ++j)
			{
				if (!m_BlendInfo[j].bBlended)
				{
					continue;
				}

				// 调节动作的播放速度以使脚步一致
				m_BlendInfo[j].fSpeed = state_speed;
				//m_BlendInfo[j].fSpeed *= float(m_BlendInfo[j].nFrameCount) 
				//	/ avg_frame_num;
				m_BlendInfo[j].fSpeed *= float(m_BlendInfo[j].nTotalFrame) 
					/ avg_frame_num;

				m_pActionPlayer->SetBlendActionSpeed(
					m_BlendInfo[j].nActionIndex, m_BlendInfo[j].fSpeed);

				UpdateEndTime(&m_BlendInfo[j]);
			}
		}
	}
	
	return true;
}

bool ActionControl::StateInputCommand(state_t* pState, int command_index)
{
	Assert(pState != NULL);
	
	int index = GetStateConvertIndex(pState, command_index);

	if (index < 0)
	{
		return false;
	}

	int to_state_index = pState->converts[index].nToState;

	if (NULL == m_pControlInfo->states[to_state_index])
	{
		return false;
	}

	SwitchState(to_state_index, pState->converts[index].fBlendTime);

	return true;
}

