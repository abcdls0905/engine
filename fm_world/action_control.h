//--------------------------------------------------------------------
// �ļ���:		action_control.h
// ��  ��:		
// ˵  ��:		
// ��������:	2010��8��7��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _ACTION_CONTROL_H
#define _ACTION_CONTROL_H

#include "../public/module.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"

// ��������

class IActionPlayer;

class ActionControl: public IEntity
{
private:
	// ״̬�ڵ�����
	enum
	{
		NODE_TYPE_DEFAULT,	// ��Ȩֵ���
		NODE_TYPE_BLEND,	// ѡ�������������
		NODE_TYPE_SELECT,	// ѡ��һ������
		NODE_TYPE_QUEUE,	// ���в���
	};

	// ����ͬ����ʽ
	enum
	{
		SYNC_MODE_DEFAULT,	// ȱʡ��ʽ
		SYNC_MODE_FOOTSTEP, // ���ղ���ͬ��
		SYNC_MODE_STARTEND, // ���ն�����ʼ�ͽ����¼�ͬ��
	};
	
	// ���״̬�ӽڵ���
	enum { MAX_NODE_CHILD_NUM = 8 };
	// ���ͬʱ��϶�������
	enum { MAX_BLEND_ACTION_NUM = 16 };

	// ״̬�������ڲ���������
	enum { END_COMMAND_INDEX = 0x10000 };
	
	// ���Ʊ���
	struct parameter_t
	{
		core_string strName;
		unsigned int nHash;
		float fDefaultValue;
	};

	// ��������
	struct command_t
	{
		core_string strName;
		unsigned int nHash;
	};

	// ״̬ת������
	struct convert_t
	{
		int nCommand;
		int nToState;
		float fBlendTime;
	};
	
	// ״̬�ڵ�
	struct state_node_t
	{
		core_string strName;
		core_string strAction;
		int nType;
		int nParamIndex;
		int nSyncMode;
		int nParentIndex;
		int nChildCount;
		int nChildIndex[8];
	};
	
	// ״̬
	struct state_t
	{
		core_string strName;
		unsigned int nHash;
		bool bLoop;
		bool bCanBreak;
		float fDefaultSpeed;
		float fDuration;
		TArrayPod<state_node_t*, 1, TCoreAlloc> nodes;
		TArrayPod<convert_t, 1, TCoreAlloc> converts;
	};
	
	// ����������Ϣ
	struct control_info_t
	{
		int nRefs;
		core_string strName;
		TArrayPod<state_t*, 1, TCoreAlloc> states;
		TArrayPod<parameter_t*, 1, TCoreAlloc> parameters;
		TArrayPod<command_t*, 1, TCoreAlloc> commands;

		control_info_t();
		~control_info_t();
		// �ͷ�
		void Release();
		// �������ü���
		void IncRefs();
		// ɾ��״̬����
		static void DeleteState(state_t* pState);
	};

	// ���������Ϣ
	struct blend_info_t
	{
		int nNodeIndex;
		int nSyncMode;
		int nActionIndex;
		int nStartFoot;
		int nCurrentFoot;
		int nStartFrame;
		int nFrameCount;
		int nTotalFrame;
		float fStartPercent;
		float fWeight;
		float fSpeed;
		float fEndTime;
		bool bFootEnded;
		bool bLoop;
		bool bBlended;
	};
	
public:
	ActionControl();
	virtual ~ActionControl();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	// ���ö����������ӿ�
	void SetActionPlayer(IActionPlayer* value);
	
	// ���ض��������ļ�
	bool LoadAllAction();
	// ���ж��������ļ��Ƿ�������
	bool GetActionLoadFinish() const;

	// ����
	void SetName(const char* value);
	const char* GetName() const;
	
	// ��ӿ��Ʊ���
	bool AddParameter(const char* param, float def_value);
	// ɾ�����Ʊ���
	bool RemoveParameter(const char* param);
	// ��ÿ��Ʊ����б�
	void GetParameterList(const IVarList& args, IVarList& result) const;

	// ��ӿ�������
	bool AddCommand(const char* command);
	// ɾ����������
	bool RemoveCommand(const char* command);
	// ��ÿ��������б�
	void GetCommandList(const IVarList& args, IVarList& result) const;

	// ���״̬
	//bool AddState(const char* state, const char* next_state, bool loop, 
	//	float def_speed, bool can_break, float duration);
	bool AddState(const char* state, bool loop, float def_speed, bool can_break, 
		float duration);
	// ɾ��״̬
	bool RemoveState(const char* state);
	// ���״̬�б�
	void GetStateList(const IVarList& args, IVarList& result) const;
	// �ǳ���״̬����һ��״̬
	//bool SetStateNext(const char* state, const char* next_state);
	//const char* GetStateNext(const char* state) const;
	// ״̬�Ƿ����
	bool SetStateLoop(const char* state, bool loop);
	bool GetStateLoop(const char* state) const;
	// ״̬ȱʡ���������ٶ�
	bool SetStateDefaultSpeed(const char* state, float def_speed);
	float GetStateDefaultSpeed(const char* state) const;
	// ״̬�����Ƿ�ɱ����
	bool SetStateCanBreak(const char* state, bool can_break);
	bool GetStateCanBreak(const char* state) const;
	// ״̬����ʱ��
	bool SetStateDuration(const char* state, float duration);
	float GetStateDuration(const char* state) const;

	// ���״̬�ڵ�
	bool AddStateNode(const char* state, const char* node_name, 
		const char* parent_name, const char* node_type, const char* action, 
		const char* parameter, const char* sync_mode);
	// ɾ��״̬�ڵ�
	bool RemoveStateNode(const char* state, const char* node_name);
	// ���״̬�ڵ��б�
	void GetStateNodeList(const IVarList& args, IVarList& result) const;
	// ״̬�ڵ㸸�ڵ���
	const char* GetStateNodeParent(const char* state, 
		const char* node_name) const;
	// ״̬�ڵ�����
	bool SetStateNodeType(const char* state, const char* node_name, 
		const char* node_type);
	const char* GetStateNodeType(const char* state, 
		const char* node_name) const;
	// ״̬�ڵ㶯��
	bool SetStateNodeAction(const char* state, const char* node_name, 
		const char* action);
	const char* GetStateNodeAction(const char* state, 
		const char* node_name) const;
	// ״̬�ڵ���Ʊ�����
	bool SetStateNodeParameter(const char* state, const char* node_name, 
		const char* parameter);
	const char* GetStateNodeParameter(const char* state, 
		const char* node_name) const;
	// ״̬�ڵ㶯��ͬ����ʽ
	bool SetStateNodeSyncMode(const char* state, const char* node_name, 
		const char* sync_mode);
	const char* GetStateNodeSyncMode(const char* state, 
		const char* node_name) const;

	// ���״̬ת������
	bool AddStateConvert(const char* state, const char* command,
		const char* to_state, float blend_time);
	// ɾ��״̬ת������
	bool RemoveStateConvert(const char* state, const char* command);
	// ���״̬ת�������б�
	void GetStateConvertList(const IVarList& args, IVarList& result) const;
	// ״̬ת��Ŀ��״̬
	bool SetStateConvertToState(const char* state, const char* command,
		const char* to_state);
	const char* GetStateConvertToState(const char* state, 
		const char* command) const;
	// ״̬ת���������ʱ��
	bool SetStateConvertBlendTime(const char* state, const char* command,
		float blend_time);
	float GetStateConvertBlendTime(const char* state, 
		const char* command) const;

	// ������ǰ׺
	void SetActionPrefix(const char* value);
	const char* GetActionPrefix() const;
		
	// ���õ�ǰ״̬
	bool SetState(const char* state);
	// ��õ�ǰ״̬
	const char* GetState() const;

	// �������岥���ٶ�
	bool SetGlobalSpeed(float value);
	// ������岥���ٶ�
	float GetGlobalSpeed();

	// ����״̬���������ٶ�
	bool SetStateSpeed(const char* state, float value);
	// ���״̬���������ٶ�
	float GetStateSpeed(const char* state);
	
	// ���ÿ��Ʊ���ֵ
	bool SetParameterValue(const char* param, float value);
	// ��ÿ��Ʊ���ֵ
	float GetParameterValue(const char* param);

	// ��õ�ǰ�ĽŲ�����
	int GetCurrentFoot();

	// ��û�϶�������
	int GetBlendCount();
	// ��û�϶�����
	const char* GetBlendAction(int index);

	// ����״̬��������
	bool EmitCommand(const char* command);

	// ����
	void Update(float seconds);

private:
	// ɾ��״̬�ڵ�
	void DeleteStateNode(state_t* pState, int node_index);

	// ��ÿ��Ʋ�������
	int GetParameterIndex(const char* param) const;
	// ��ÿ�����������
	int GetCommandIndex(const char* command) const;
	// ���״̬����
	int GetStateIndex(const char* state) const;
	// ���״̬�ڵ�����
	int GetStateNodeIndex(state_t* pState, const char* node_name) const;
	// ���״̬ת����������
	int GetStateConvertIndex(state_t* pState, int command_index) const;

	// ���״̬�ڵ�
	state_node_t* GetStateNode(const char* state, const char* node_name) const;
	// ��õ�ǰ״̬
	state_t* GetCurrentState() const;
	// ���״̬�ĸ��ڵ�����
	int GetStateRootNodeIndex(state_t* pState) const;
	// ��õ�ǰ��״̬�����ٶ�
	float* GetSpeedCurrentValues();
	// ��ÿ��Ʋ�����ǰֵ����
	float* GetParamCurrentValues();

	// ��������Ķ�����
	const char* GetNodeAction(state_node_t* pNode, char* buffer, size_t size);

	// ѡ����Ҫ��ϵĶ���
	int SelectActionBlend(state_t* pState, int node_index, float weight,
		blend_info_t* blend_info, int blend_num);
	// Ӧ�ö������
	bool ApplyActionBlend(state_t* pState, blend_info_t* pBlendInfo, 
		float blend_time);

	// ������в��ŵ���һ��״̬�ڵ�����
	int GetQueueNextNode(state_t* pState, int node_index, bool loop);
	// ���״̬�ڵ��ͬ��ģʽ
	int GetNodeSyncMode(state_t* pState, int node_index);
	// �����ʼ����֡��Ϣ
	bool GetStartEndFrame(int action_index, int& start_frame, int& frame_num);
	// �����һ���Ų�֡��Ϣ
	bool GetNextFootFrame(int action_index, int foot_flag, int current_foot, 
		int& foot_index, int& start_frame, int& frame_num);
	// ��õ�ǰ���ŵ�һ�ֶ����Ƿ�ȫ������
	bool GetBlendAllEnded();

	// �������ǰ��Ԥ����
	bool PreprocessBlend(state_t* pState);

	// ���»����Ϣ�Ľ���ʱ��
	bool UpdateEndTime(blend_info_t* pBlendInfo);

	// ���״̬�ڵ��ڻ����Ϣ������
	int GetBlendInfoIndex(int node_index, blend_info_t* blend_info, 
		int blend_num);

	// �л�״̬
	bool SwitchState(int new_state_index, float blend_time);
	// ����״̬
	bool StateInputEntry(state_t* pState, float blend_time);
	// �뿪״̬
	bool StateInputLeave(state_t* pState, float blend_time);
	// ״̬��ʱ����
	bool StateInputTimer(state_t* pState, float seconds);
	// ���岥���ٶȷ����仯
	bool StateInputGlobalSpeedChange(state_t* pState);
	// ���������ٶȷ����仯
	bool StateInputSpeedChange(state_t* pState, float new_speed, 
		float old_speed);
	// ���Ʊ��������仯
	bool StateInputParamChange(state_t* pState, int param_index, 
		float old_param);
	// ��������
	bool StateInputCommand(state_t* pState, int command_index);

private:
	IActionPlayer* m_pActionPlayer;
	control_info_t* m_pControlInfo;
	core_string m_strActionPrefix;
	int m_nCurrentState;
	int m_nFootFlag;
	float m_fFootPercent;
	float m_fStateTimer;
	bool m_bStateDeferred;
	size_t m_nStateCount;
	float* m_pStateSpeedValues;
	size_t m_nParameterCount;
	float* m_pParameterValues;
	int m_nBlendCount;
	blend_info_t m_BlendInfo[MAX_BLEND_ACTION_NUM];
};

#endif // _ACTION_CONTROL_H
