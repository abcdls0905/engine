//--------------------------------------------------------------------
// 文件名:		action_control.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年8月7日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _ACTION_CONTROL_H
#define _ACTION_CONTROL_H

#include "../public/module.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"

// 动作控制

class IActionPlayer;

class ActionControl: public IEntity
{
private:
	// 状态节点类型
	enum
	{
		NODE_TYPE_DEFAULT,	// 按权值混合
		NODE_TYPE_BLEND,	// 选择两个动作混合
		NODE_TYPE_SELECT,	// 选择一个动作
		NODE_TYPE_QUEUE,	// 序列播放
	};

	// 动作同步方式
	enum
	{
		SYNC_MODE_DEFAULT,	// 缺省方式
		SYNC_MODE_FOOTSTEP, // 按照步伐同步
		SYNC_MODE_STARTEND, // 按照动作起始和结束事件同步
	};
	
	// 最大状态子节点数
	enum { MAX_NODE_CHILD_NUM = 8 };
	// 最大同时混合动作数量
	enum { MAX_BLEND_ACTION_NUM = 16 };

	// 状态结束的内部命令索引
	enum { END_COMMAND_INDEX = 0x10000 };
	
	// 控制变量
	struct parameter_t
	{
		core_string strName;
		unsigned int nHash;
		float fDefaultValue;
	};

	// 控制命令
	struct command_t
	{
		core_string strName;
		unsigned int nHash;
	};

	// 状态转换条件
	struct convert_t
	{
		int nCommand;
		int nToState;
		float fBlendTime;
	};
	
	// 状态节点
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
	
	// 状态
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
	
	// 动作控制信息
	struct control_info_t
	{
		int nRefs;
		core_string strName;
		TArrayPod<state_t*, 1, TCoreAlloc> states;
		TArrayPod<parameter_t*, 1, TCoreAlloc> parameters;
		TArrayPod<command_t*, 1, TCoreAlloc> commands;

		control_info_t();
		~control_info_t();
		// 释放
		void Release();
		// 增加引用计数
		void IncRefs();
		// 删除状态数据
		static void DeleteState(state_t* pState);
	};

	// 动作混合信息
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
	
	// 设置动作播放器接口
	void SetActionPlayer(IActionPlayer* value);
	
	// 加载动作数据文件
	bool LoadAllAction();
	// 所有动作数据文件是否加载完成
	bool GetActionLoadFinish() const;

	// 名称
	void SetName(const char* value);
	const char* GetName() const;
	
	// 添加控制变量
	bool AddParameter(const char* param, float def_value);
	// 删除控制变量
	bool RemoveParameter(const char* param);
	// 获得控制变量列表
	void GetParameterList(const IVarList& args, IVarList& result) const;

	// 添加控制命令
	bool AddCommand(const char* command);
	// 删除控制命令
	bool RemoveCommand(const char* command);
	// 获得控制命令列表
	void GetCommandList(const IVarList& args, IVarList& result) const;

	// 添加状态
	//bool AddState(const char* state, const char* next_state, bool loop, 
	//	float def_speed, bool can_break, float duration);
	bool AddState(const char* state, bool loop, float def_speed, bool can_break, 
		float duration);
	// 删除状态
	bool RemoveState(const char* state);
	// 获得状态列表
	void GetStateList(const IVarList& args, IVarList& result) const;
	// 非持续状态的下一个状态
	//bool SetStateNext(const char* state, const char* next_state);
	//const char* GetStateNext(const char* state) const;
	// 状态是否持续
	bool SetStateLoop(const char* state, bool loop);
	bool GetStateLoop(const char* state) const;
	// 状态缺省动作播放速度
	bool SetStateDefaultSpeed(const char* state, float def_speed);
	float GetStateDefaultSpeed(const char* state) const;
	// 状态动作是否可被打断
	bool SetStateCanBreak(const char* state, bool can_break);
	bool GetStateCanBreak(const char* state) const;
	// 状态持续时间
	bool SetStateDuration(const char* state, float duration);
	float GetStateDuration(const char* state) const;

	// 添加状态节点
	bool AddStateNode(const char* state, const char* node_name, 
		const char* parent_name, const char* node_type, const char* action, 
		const char* parameter, const char* sync_mode);
	// 删除状态节点
	bool RemoveStateNode(const char* state, const char* node_name);
	// 获得状态节点列表
	void GetStateNodeList(const IVarList& args, IVarList& result) const;
	// 状态节点父节点名
	const char* GetStateNodeParent(const char* state, 
		const char* node_name) const;
	// 状态节点类型
	bool SetStateNodeType(const char* state, const char* node_name, 
		const char* node_type);
	const char* GetStateNodeType(const char* state, 
		const char* node_name) const;
	// 状态节点动作
	bool SetStateNodeAction(const char* state, const char* node_name, 
		const char* action);
	const char* GetStateNodeAction(const char* state, 
		const char* node_name) const;
	// 状态节点控制变量名
	bool SetStateNodeParameter(const char* state, const char* node_name, 
		const char* parameter);
	const char* GetStateNodeParameter(const char* state, 
		const char* node_name) const;
	// 状态节点动作同步方式
	bool SetStateNodeSyncMode(const char* state, const char* node_name, 
		const char* sync_mode);
	const char* GetStateNodeSyncMode(const char* state, 
		const char* node_name) const;

	// 添加状态转换规则
	bool AddStateConvert(const char* state, const char* command,
		const char* to_state, float blend_time);
	// 删除状态转换规则
	bool RemoveStateConvert(const char* state, const char* command);
	// 获得状态转换规则列表
	void GetStateConvertList(const IVarList& args, IVarList& result) const;
	// 状态转换目标状态
	bool SetStateConvertToState(const char* state, const char* command,
		const char* to_state);
	const char* GetStateConvertToState(const char* state, 
		const char* command) const;
	// 状态转换动作混合时间
	bool SetStateConvertBlendTime(const char* state, const char* command,
		float blend_time);
	float GetStateConvertBlendTime(const char* state, 
		const char* command) const;

	// 动作名前缀
	void SetActionPrefix(const char* value);
	const char* GetActionPrefix() const;
		
	// 设置当前状态
	bool SetState(const char* state);
	// 获得当前状态
	const char* GetState() const;

	// 设置整体播放速度
	bool SetGlobalSpeed(float value);
	// 获得整体播放速度
	float GetGlobalSpeed();

	// 设置状态动作播放速度
	bool SetStateSpeed(const char* state, float value);
	// 获得状态动作播放速度
	float GetStateSpeed(const char* state);
	
	// 设置控制变量值
	bool SetParameterValue(const char* param, float value);
	// 获得控制变量值
	float GetParameterValue(const char* param);

	// 获得当前的脚步索引
	int GetCurrentFoot();

	// 获得混合动作数量
	int GetBlendCount();
	// 获得混合动作名
	const char* GetBlendAction(int index);

	// 发出状态控制命令
	bool EmitCommand(const char* command);

	// 更新
	void Update(float seconds);

private:
	// 删除状态节点
	void DeleteStateNode(state_t* pState, int node_index);

	// 获得控制参数索引
	int GetParameterIndex(const char* param) const;
	// 获得控制命令索引
	int GetCommandIndex(const char* command) const;
	// 获得状态索引
	int GetStateIndex(const char* state) const;
	// 获得状态节点索引
	int GetStateNodeIndex(state_t* pState, const char* node_name) const;
	// 获得状态转换条件索引
	int GetStateConvertIndex(state_t* pState, int command_index) const;

	// 获得状态节点
	state_node_t* GetStateNode(const char* state, const char* node_name) const;
	// 获得当前状态
	state_t* GetCurrentState() const;
	// 获得状态的根节点索引
	int GetStateRootNodeIndex(state_t* pState) const;
	// 获得当前的状态播放速度
	float* GetSpeedCurrentValues();
	// 获得控制参数当前值数组
	float* GetParamCurrentValues();

	// 获得完整的动作名
	const char* GetNodeAction(state_node_t* pNode, char* buffer, size_t size);

	// 选择需要混合的动作
	int SelectActionBlend(state_t* pState, int node_index, float weight,
		blend_info_t* blend_info, int blend_num);
	// 应用动作混合
	bool ApplyActionBlend(state_t* pState, blend_info_t* pBlendInfo, 
		float blend_time);

	// 获得序列播放的下一个状态节点索引
	int GetQueueNextNode(state_t* pState, int node_index, bool loop);
	// 获得状态节点的同步模式
	int GetNodeSyncMode(state_t* pState, int node_index);
	// 获得起始结束帧信息
	bool GetStartEndFrame(int action_index, int& start_frame, int& frame_num);
	// 获得下一个脚步帧信息
	bool GetNextFootFrame(int action_index, int foot_flag, int current_foot, 
		int& foot_index, int& start_frame, int& frame_num);
	// 获得当前播放的一轮动作是否全部结束
	bool GetBlendAllEnded();

	// 动作混合前的预处理
	bool PreprocessBlend(state_t* pState);

	// 更新混合信息的结束时间
	bool UpdateEndTime(blend_info_t* pBlendInfo);

	// 获得状态节点在混合信息的索引
	int GetBlendInfoIndex(int node_index, blend_info_t* blend_info, 
		int blend_num);

	// 切换状态
	bool SwitchState(int new_state_index, float blend_time);
	// 进入状态
	bool StateInputEntry(state_t* pState, float blend_time);
	// 离开状态
	bool StateInputLeave(state_t* pState, float blend_time);
	// 状态定时控制
	bool StateInputTimer(state_t* pState, float seconds);
	// 整体播放速度发生变化
	bool StateInputGlobalSpeedChange(state_t* pState);
	// 动作播放速度发生变化
	bool StateInputSpeedChange(state_t* pState, float new_speed, 
		float old_speed);
	// 控制变量发生变化
	bool StateInputParamChange(state_t* pState, int param_index, 
		float old_param);
	// 输入命令
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
