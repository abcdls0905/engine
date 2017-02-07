//--------------------------------------------------------------------
// �ļ���:		action_control.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2010��8��7��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "action_control.h"
#include "../visual/i_action_player.h"
#include "../public/inlines.h"

/// \file action_control.cpp
/// \brief ��������

/// entity: ActionControl
/// \brief ��������ʵ��
DECLARE_ENTITY(ActionControl, 0, IEntity);

/// property: string Name
/// \brief ����
DECLARE_PROPERTY(const char*, ActionControl, Name, GetName, SetName);
/// property: string State
/// \brief ��ǰ״̬
DECLARE_PROPERTY(const char*, ActionControl, State, GetState, SetState);
/// property: string ActionPrefix
/// \brief ������ǰ׺
DECLARE_PROPERTY(const char*, ActionControl, ActionPrefix, GetActionPrefix, 
	SetActionPrefix);

	/*
/// method: bool LoadAllAction()
// \brief ����������ض����������ļ�
DECLARE_METHOD_0(bool, ActionControl, LoadAllAction);
// method: bool GetActionLoadFinish()
/// \brief ���ж��������ļ��Ƿ�������
DECLARE_METHOD_0(bool, ActionControl, GetActionLoadFinish);

/// method: bool AddParameter(string param, float value)
/// \brief ��ӿ��Ʊ���
/// \param param ������
/// \param value ����ȱʡֵ
DECLARE_METHOD_2(bool, ActionControl, AddParameter, const char*, float);
/// method: bool RemoveParameter(string param)
/// \brief ɾ�����Ʊ���
/// \param param ������
DECLARE_METHOD_1(bool, ActionControl, RemoveParameter, const char*);
/// method: [table] GetParameterList()
/// \brief ��ÿ��Ʊ����б�
DECLARE_METHOD_T(ActionControl, GetParameterList);

/// method: bool AddCommand(string command)
/// \brief ��ӿ�������
/// \param command ����
DECLARE_METHOD_1(bool, ActionControl, AddCommand, const char*);
/// method: bool RemoveCommand(string command)
/// \brief ɾ����������
/// \param command ����
DECLARE_METHOD_1(bool, ActionControl, RemoveCommand, const char*);
/// method: [table] GetCommandList()
/// \brief ��ÿ��������б�
DECLARE_METHOD_T(ActionControl, GetCommandList);

/// method: bool AddState(string state, bool loop, float def_speed, 
///		bool can_break, float duration)
/// \brief ���״̬
/// \param state ״̬��
/// \param loop ״̬�Ƿ����
/// \param def_speed ȱʡ���������ٶ�
/// \param can_break ״̬�����Ƿ���Ա����
/// \param duration ״̬����ʱ�䣨С�ڵ���0��ʾ����ȷ����
DECLARE_METHOD_5(bool, ActionControl, AddState, const char*, bool, float, 
	bool, float);
/// method: bool RemoveState(string state)
/// \brief ɾ��״̬
/// \param state ״̬��
DECLARE_METHOD_1(bool, ActionControl, RemoveState, const char*);
/// method: [table] GetStateList()
/// \brief ���״̬���б�
DECLARE_METHOD_T(ActionControl, GetStateList);
/// method: bool SetStateNext(string state, string next_state)
/// \brief ���ú���״̬
/// \param state ״̬��
/// \param next_state ����״̬
//DECLARE_METHOD_2(bool, ActionControl, SetStateNext, const char*, const char*);
/// method: string GetStateNext(string state)
/// \brief ��ú���״̬
/// \param state ״̬��
//DECLARE_METHOD_1(const char*, ActionControl, GetStateNext, const char*);
/// method: bool SetStateLoop(string state, bool loop)
/// \brief ����״̬�Ƿ����
/// \param state ״̬��
/// \param loop �Ƿ�ѭ��
DECLARE_METHOD_2(bool, ActionControl, SetStateLoop, const char*, bool);
/// method: bool GetStateLoop(string state)
/// \brief ���״̬�Ƿ����
/// \param state ״̬��
DECLARE_METHOD_1(bool, ActionControl, GetStateLoop, const char*);
/// method: bool SetStateDefaultSpeed(string state, float def_speed)
/// \brief ����״̬ȱʡ���������ٶ�
/// \param state ״̬��
/// \param def_speed ȱʡ���������ٶ�
DECLARE_METHOD_2(bool, ActionControl, SetStateDefaultSpeed, const char*, 
	float);
/// method: float GetStateDefaultSpeed(string state)
/// \brief ���״̬ȱʡ���������ٶ�
/// \param state ״̬��
DECLARE_METHOD_1(float, ActionControl, GetStateDefaultSpeed, const char*);
/// method: bool SetStateCanBreak(string state, bool can_break)
/// \brief ����״̬�����Ƿ���Ա����
/// \param state ״̬��
/// \param can_break �Ƿ�ɴ��
DECLARE_METHOD_2(bool, ActionControl, SetStateCanBreak, const char*, bool);
/// method: bool GetStateCanBreak(string state)
/// \brief ���״̬�����Ƿ���Ա����
/// \param state ״̬��
DECLARE_METHOD_1(bool, ActionControl, GetStateCanBreak, const char*);
/// method: bool SetStateDuration(string state, float def_speed)
/// \brief ����״̬����ʱ��
/// \param state ״̬��
/// \param duration ״̬����ʱ�䣨С�ڵ���0��ʾ����ȷ����
DECLARE_METHOD_2(bool, ActionControl, SetStateDuration, const char*, float);
/// method: float GetStateDuration(string state)
/// \brief ���״̬����ʱ��
/// \param state ״̬��
DECLARE_METHOD_1(float, ActionControl, GetStateDuration, const char*);

/// method: bool AddStateNode(string state, string node_name, string parent_name,
///		string node_type, string action, string parameter, string sync_mode)
/// \brief ���״̬�ڵ�
/// \param state ״̬��
/// \param node_name �ڵ���
/// \param parent_name ���ڵ���
/// \param node_type �ڵ�����["default", "blend", "select", "queue"]
/// \param action ������
/// \param parameter ���Ʊ�����
/// \param sync_mode ����ͬ����ʽ["default", "footstep", "startend"]
DECLARE_METHOD_7(bool, ActionControl, AddStateNode, const char*, const char*,
	const char*, const char*, const char*, const char*, const char*);
/// method: bool RemoveStateNode(string state, string node_name)
/// \brief ɾ��״̬�ڵ�
/// \param state ״̬��
/// \param node_name �ڵ���
DECLARE_METHOD_2(bool, ActionControl, RemoveStateNode, const char*, 
	const char*);
/// method: [table] GetStateNodeList(string state)
/// \brief ���״̬�ڵ����б�
/// \param state ״̬��
DECLARE_METHOD_T(ActionControl, GetStateNodeList);
/// method: string GetStateNodeParent(string state, string node_name)
/// \brief ���״̬�ڵ㸸�ڵ���
/// \param state ״̬��
/// \param node_name �ڵ���
DECLARE_METHOD_2(const char*, ActionControl, GetStateNodeParent, const char*, 
	const char*);
/// method: bool SetStateNodeType(string state, string node_name, string node_type)
/// \brief ����״̬�ڵ�����
/// \param state ״̬��
/// \param node_name �ڵ���
/// \param node_type �ڵ�����["default", "blend", "select", "queue"]
DECLARE_METHOD_3(bool, ActionControl, SetStateNodeType, const char*, 
	const char*, const char*);
/// method: string GetStateNodeType(string state, string node_name)
/// \brief ���״̬�ڵ�����
/// \param state ״̬��
/// \param node_name �ڵ���
DECLARE_METHOD_2(const char*, ActionControl, GetStateNodeType, const char*, 
	const char*);
/// method: bool SetStateNodeAction(string state, string node_name, string action)
/// \brief ����״̬�ڵ㶯��
/// \param state ״̬��
/// \param node_name �ڵ���
/// \param action ������
DECLARE_METHOD_3(bool, ActionControl, SetStateNodeAction, const char*, 
	const char*, const char*);
/// method: string GetStateNodeAction(string state, string node_name)
/// \brief ���״̬�ڵ㶯��
/// \param state ״̬��
/// \param node_name �ڵ���
DECLARE_METHOD_2(const char*, ActionControl, GetStateNodeAction, const char*, 
	const char*);
/// method: bool SetStateNodeParameter(string state, string node_name, string parameter)
/// \brief ����״̬�ڵ���Ʊ�����
/// \param state ״̬��
/// \param node_name �ڵ���
/// \param parameter ���Ʊ�����
DECLARE_METHOD_3(bool, ActionControl, SetStateNodeParameter, const char*, 
	const char*, const char*);
/// method: string GetStateNodeParameter(string state, string node_name)
/// \brief ���״̬�ڵ���Ʊ�����
/// \param state ״̬��
/// \param node_name �ڵ���
DECLARE_METHOD_2(const char*, ActionControl, GetStateNodeParameter, 
	const char*, const char*);
/// method: bool SetStateNodeSyncMode(string state, string node_name, string sync_mode)
/// \brief ����״̬�ڵ㶯��ͬ����ʽ
/// \param state ״̬��
/// \param node_name �ڵ���
/// \param sync_mode ����ͬ����ʽ["default", "footstep", "startend"]
DECLARE_METHOD_3(bool, ActionControl, SetStateNodeSyncMode, const char*, 
	const char*, const char*);
/// method: string GetStateNodeSyncMode(string state, string node_name)
/// \brief ���״̬�ڵ㶯��ͬ����ʽ
/// \param state ״̬��
/// \param node_name �ڵ���
DECLARE_METHOD_2(const char*, ActionControl, GetStateNodeSyncMode, const char*, 
	const char*);

/// method: bool AddStateConvert(string state, string command, string to_state, float blend_time)
/// \brief ���״̬ת������
/// \param state ״̬��
/// \param command ��������
/// \param to_state Ŀ��״̬
/// \param blend_time �����ں�ʱ��
DECLARE_METHOD_4(bool, ActionControl, AddStateConvert, const char*, 
	const char*, const char*, float);
/// method: bool RemoveStateConvert(string state, string command)
/// \brief ɾ��״̬ת������
/// \param state ״̬��
/// \param command ��������
DECLARE_METHOD_2(bool, ActionControl, RemoveStateConvert, const char*, 
	const char*);
/// method: [table] GetStateConvertList(string state)
/// \brief ���״̬ת�������б�
/// \param state ״̬��
DECLARE_METHOD_T(ActionControl, GetStateConvertList);
/// method: bool SetStateConvertToState(string state, string command, string to_state)
/// \brief ���ÿ��������Ӧ��Ŀ��״̬
/// \param state ״̬��
/// \param command ��������
/// \param to_state Ŀ��״̬
DECLARE_METHOD_3(bool, ActionControl, SetStateConvertToState, const char*,
	const char*, const char*);
/// method: string GetStateConvertToState(string state, string command)
/// \brief ��ÿ��������Ӧ��Ŀ��״̬
/// \param state ״̬��
/// \param command ��������
DECLARE_METHOD_2(const char*, ActionControl, GetStateConvertToState, 
	const char*, const char*);
/// method: bool SetStateConvertBlendTime(string state, string command, string to_state)
/// \brief ���ÿ��������Ӧ�Ķ����ں�ʱ��
/// \param state ״̬��
/// \param command ��������
/// \param blend_time �����ں�ʱ��
DECLARE_METHOD_3(bool, ActionControl, SetStateConvertBlendTime, const char*,
	const char*, float);
/// method: float GetStateConvertBlendTime(string state, string command)
/// \brief ��ÿ��������Ӧ�Ķ����ں�ʱ��
/// \param state ״̬��
/// \param command ��������
DECLARE_METHOD_2(float, ActionControl, GetStateConvertBlendTime, 
	const char*, const char*);

/// method: bool SetGlobalSpeed(float speed)
/// \brief �������岥���ٶ�
/// \param speed ���������ٶ�
DECLARE_METHOD_1(bool, ActionControl, SetGlobalSpeed, float);
/// method: float GetGlobalSpeed()
/// \brief ��������ٶ�
DECLARE_METHOD_0(float, ActionControl, GetGlobalSpeed);

/// method: bool SetStateSpeed(string state, float speed)
/// \brief ����״̬���������ٶ�
/// \param state ״̬��
/// \param speed ���������ٶ�
DECLARE_METHOD_2(bool, ActionControl, SetStateSpeed, const char*, float);
/// method: float GetStateSpeed(string state)
/// \brief ���״̬���������ٶ�
/// \param state ״̬��
DECLARE_METHOD_1(float, ActionControl, GetStateSpeed, const char*);

/// method: bool SetParameterValue(string param, float value)
/// \brief ���ÿ��Ʊ���ֵ
/// \param param ������
/// \param value ����ֵ
DECLARE_METHOD_2(bool, ActionControl, SetParameterValue, const char*, float);
/// method: float GetParameterValue(string param)
/// \brief ��ÿ��Ʊ���ֵ
/// \param param ������
DECLARE_METHOD_1(float, ActionControl, GetParameterValue, const char*);

/// method: int GetCurrentFoot()
/// \brief ��õ�ǰ�ĽŲ�����
DECLARE_METHOD_0(int, ActionControl, GetCurrentFoot);

/// method: int GetBlendCount()
// \brief ��õ�ǰ��϶�������
DECLARE_METHOD_0(int, ActionControl, GetBlendCount);
/// method: string GetBlendAction(int index)
/// \brief ���ָ�������Ļ�϶�����
/// \param index �������
DECLARE_METHOD_1(const char*, ActionControl, GetBlendAction, int);

/// method: bool EmitCommand(string command)
/// \brief ����״̬��������
/// \param command ����
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
		// ���ö���������Ϣ
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
		// û�б仯
		return true;
	}

	speed_values[state_index] = value;

	if (state_index == m_nCurrentState)
	{
		state_t* pState = GetCurrentState();
		
		// ���������ٶȸı���¼�
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
		// ���Ʊ��������ı���¼�
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

// ��õ�ǰ֡�ĽŲ������Ͱٷֱ�
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
		// δ���������
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
		// �ﵽ��϶�����������
		return 0;
	}

	if (weight < 0.001F)
	{
		// ���Ȩֵ̫С
		return 0;
	}
	
	state_node_t* pNode = pState->nodes[node_index];

	if (NULL == pNode)
	{
		return 0;
	}

	// ���Ʊ����ĵ�ǰֵ
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
			// �ÿ��Ʊ������ڻ��Ȩ��
			weight *= param_val;
		}

		// ��������Ч֡��
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
				// �Ӽ���������ѡ�������������л��
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
				// �Ӽ���������ѡ��һ������
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
				// ���в���
				if (i != 0)
				{
					// �Ȳ��ŵ�һ������
					child_weight = 0.0F;
				}
			}
		}
		else
		{
			// ���в���
			if (i != 0)
			{
				// �Ȳ��ŵ�һ������
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
		// ���ò����ٶ�
		m_pActionPlayer->SetBlendActionSpeed(action_index, speed);
	}

	// Ԥ���Ķ�������ʱ��
	float global_speed = m_pActionPlayer->GetSpeed();
	float fps = m_pActionPlayer->GetInternalFPS(action_index);
	float t = float(frame_num) / (fps * speed * global_speed);

	if (percent > 0.0F)
	{
		// ��������������ͬ�İٷֱ�λ�ÿ�ʼ����
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
		// ��ָ������ʼ֡��ʼ����
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
				// ʹ�ø��ڵ�Ķ���ͬ��ģʽ
				sync_mode = pParentNode->nSyncMode;
			}
		}
	}

	return sync_mode;
}

bool ActionControl::GetStartEndFrame(int action_index, int& start_frame, 
	int& frame_num)
{
	// �˶�������Ч����֡��
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
		// ��ǰ�Ų�����һ��
		int foot = (current_foot + 1 + i) % footstep_num;
		
		int frame = m_pActionPlayer->GetActionFootstepFrame(action_index, 
			foot);

		if (frame < 3)
		{
			continue;
		}

		if (foot_flag >= 0)
		{
			// �Ƿ��뵱ǰ�����ĽŲ�ƥ��
			if ((foot & 0x1) != foot_flag)
			{
				continue;
			}
		}

		// ��һ���Ų�������
		int next_foot = foot + 1;

		if (next_foot == footstep_num)
		{
			next_foot = 0;
		}

		if (next_foot == foot)
		{
			return false;
		}

		// ��һ���Ų���֡
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
		// ����ƽ������֡����
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
				// ���ڶ����Ĳ����ٶ���ʹ�Ų�һ��
				//m_BlendInfo[j].fSpeed *= float(m_BlendInfo[j].nFrameCount) 
				//	/ avg_frame_num;
				m_BlendInfo[j].fSpeed *= float(m_BlendInfo[j].nTotalFrame) 
					/ avg_frame_num;
			}
		}
	}

	if (first_foot >= 0)
	{
		// �任���ҽű�־
		m_nFootFlag = (m_nFootFlag + 1) & 0x1;
		m_fFootPercent = 0.0F;
	}
	else
	{
		// ����ʹ�ýŲ�ͬ��ʱ��λ�Ų�����
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
		// �����Ѿ����Ž���
		pBlendInfo->fEndTime = m_fStateTimer;
		return true;
	}
	
	float start_frame = float(pBlendInfo->nStartFrame);
	float frame_num = float(m_pActionPlayer->GetFrameCount(action_index));
	float fps = m_pActionPlayer->GetInternalFPS(action_index);
	float global_speed = m_pActionPlayer->GetSpeed();

	// �Ѿ����ŵ�֡��
	float play_frames;

	if (cur_frame >= start_frame)
	{
		play_frames = cur_frame - start_frame;
	}
	else
	{
		play_frames = cur_frame + frame_num - start_frame;
	}

	// ��ʣ�¶���֡δ����
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
			// �ȴ���״̬��һ�ֶ��������ٲ�����״̬�Ķ���
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
		// ״̬��Ϊ��ʱ��λ�Ų�����
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
		// �ӳٲ����µĶ���
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
		// �ӳ�ֹͣ��ǰ�Ķ���
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
			// ��õ�ǰ�Ų�֡�Ĳ��ű���
			int foot_index;
			float percent;

			if (get_foot_percent(m_pActionPlayer, action_index, foot_index, 
				percent))
			{
				// ��¼�Ų���Ϣ�Ա�����һ��״̬ƽ������
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
		// ����ǰһ״̬�Ķ����Ƿ����
		if (GetBlendAllEnded())
		{
			// ���ǰһ��״̬�����ж���
			for (int i = 0; i < m_nBlendCount; ++i)
			{
				if (m_BlendInfo[i].bBlended)
				{
					int action_index = m_BlendInfo[i].nActionIndex;

					m_pActionPlayer->UnblendAction(action_index);
				}
			}

			m_nBlendCount = 0;

			// ��λ״̬��ʱ
			m_fStateTimer = 0.0F;
			m_bStateDeferred = false;

			// �����µĶ���
			StateInputEntry(pState, -1.0F);
		}

		return true;
	}

	// �Ƿ�ĳ���Ų��Ѿ�����
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
			// �Ų�����
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

			// �����һ�Ų���֡��Ϣ
			if (GetNextFootFrame(action_index, m_nFootFlag, current_foot, 
				foot_index, start_frame, frame_num))
			{
				if (foot_index == m_BlendInfo[k].nStartFoot)
				{
					m_BlendInfo[k].bFootEnded = true;
				}

				/*
				// �Ƚϵ�ǰ���ŵ���֡�ͽŲ���ʼ֡
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
					// ���������ƫ��
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

				// Ԥ������һ�Ų�����ʱ�䣨�ӵ��ϸ��Ų��Ľ���ʱ���Ա����ۻ���
				m_BlendInfo[k].fEndTime += t;
			}
			else
			{
				// �����Ų�
				m_BlendInfo[k].bFootEnded = true;
			}

			//total_weight += m_BlendInfo[k].fWeight;
		}
		
		// ���ٽ����ٶȵ�������Ϊ������ʱ��������
		/*
		if (m_nBlendCount >= 2)
		{
			// ����ƽ������֡����
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

					// ���ڶ����Ĳ����ٶ���ʹ�Ų�һ��
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

		// �任���ҽű�־
		m_nFootFlag = (m_nFootFlag + 1) & 0x1;
		m_fFootPercent = 0.0F;
	}

	// �Ƿ����еĶ������Ѿ�����
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

			// ������в��ŵ���һ������
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

					// ���Ȩֵ���ֲ���
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
			// ��ʼ������һ�ֵĶ���
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
		// �Ƿ񵽴��޶�����ʱ��
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
		// ״̬ʱ�޵�������״̬
		StateInputCommand(pState, END_COMMAND_INDEX);
		//int next_state_index = GetStateIndex(pState->strNextState.c_str());

		//if (next_state_index >= 0)
		//{
		//	// �л����趨����һ��״̬
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

			// ���¼��㶯������ʱ��
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

// ��ö����Ѿ����ŵİٷֱ�
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

	// ��ǰ״̬�Ƿ�ʹ�ô˲���
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
		// �뵱ǰ״̬�޹�
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

	// ���Ȩֵ�Ƿ��б仯
	bool weight_changed = false;
	// ��ǰ�������ŵİٷֱ�
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
			// �˶�������Ҫ�ٲ���
			m_pActionPlayer->UnblendAction(m_BlendInfo[i].nActionIndex);

			// ɾ�������Ϣ 
			memmove(m_BlendInfo + i, m_BlendInfo + i + 1,
				sizeof(blend_info_t) * (m_nBlendCount - i - 1));
			--m_nBlendCount;
			--i;
			continue;
		}

		// �µĻ��Ȩֵ
		float new_weight = new_blend_info[blend_index].fWeight;

		if (!FloatEqual(m_BlendInfo[i].fWeight, new_weight))
		{
			// �ı䶯����Ȩֵ
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
			// �Ѿ����ŵĶ���
			continue;
		}

		if (m_nBlendCount >= MAX_BLEND_ACTION_NUM)
		{
			// ̫�ද���޷�����
			continue;
		}

		int index = m_nBlendCount;

		// ����µĻ����Ϣ
		memcpy(&m_BlendInfo[index], &new_blend_info[k], sizeof(blend_info_t));
		++m_nBlendCount;

		int action_index = m_BlendInfo[index].nActionIndex;
		int sync_mode = m_BlendInfo[index].nSyncMode;

		if (sync_mode == SYNC_MODE_FOOTSTEP)
		{
			// ʹ����һ���Ų��ı�־
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
		// ��������������ͬ�İٷֱ�λ�ÿ�ʼ����
		m_BlendInfo[index].fStartPercent = percent;

		ApplyActionBlend(pState, &m_BlendInfo[index], -1.0F);

		weight_changed = true;
	}

	// ���۶����Ķ��٣�����Ҫ���ò����ٶ�
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
		
		// ����ƽ������֡����
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

				// ���ڶ����Ĳ����ٶ���ʹ�Ų�һ��
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

