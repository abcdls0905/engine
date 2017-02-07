
#pragma once

#include <vector>
#include "../objbinder/i_bind_entity.h"
#include "../public/i_entity.h"
#include "../visual/i_vis_base.h"
#include <string>

// 清除类型
enum ACTION_CLEAR_TYPE
{
	// 不清除
	eClearNone,
	// 清除全身动作
	eClearState,
	// 清除半身动作
	eClearHalf,
	// 清除一次性动作
	eClearOnce,
	// 清楚一次性动作，忽略拒绝动作
	eClearOnceIgnore,
	// 清除全部
	eClearAll,
};

// 动作是否可被打断
enum ACTION_INTERRUPT_TYPE
{
	// 可被打断:当为半身动作时,如果下一动作也为半身动作,则打断当前半身动作,做下一个半身动作
	//      当为全身动作时,如果下一动作为非半身动作,则打断当前半身动作,做下一个全身动作
	eInterrupt,

	// 动作发生变化时被打断(静止落地变向前落地等)
	eChangeInterrupt,

    // 只被eClearAll的动作打断
    eNoInterrupt,

	// 不被打断,且拒绝其他动作请求,大于eClearAll优先级
	eRefuseAction,
};

//当前动作队列是否可加入其他动作
enum ACTION_REFUSE_TYPE
{
	eRefuseNone,
	eRefuseAll,
};

//队列中的动作状态
enum ActionState
{
	ACTION_STATE_WAIT,
	ACTION_STATE_PLAY,
};

enum PLAYER_ACTION_TYPE
{
	ePlayAll,
	ePlayNormal,
	ePlayHalfAction,
};

//动作信息
struct ActionInfo 
{
	char ActionName[256];               //动作名
	char FullActionName[256];	        //全身动作名
	unsigned char loop;					//是否循环
	unsigned char state;				//动作当前状态
	ACTION_CLEAR_TYPE clear;	        //是否清除之前的所有融合
	ACTION_INTERRUPT_TYPE bInterrupt;   //是否可打断（仅用于一次性动作）
};

const int QUEUE_COUNT_MAX = 50; //动作队列阀值

class CActionQueue : public IBindEntity
{
public:
  CActionQueue();
  ~CActionQueue();
  virtual void SetRole(const PERSISTID & id){ m_visID = id; }
  virtual bool Init(const IVarList& args);
  virtual bool Shut();
private:
  std::vector<ActionInfo*>	m_vecActionQueue;      // 动作队列
  std::vector<ActionInfo*>	m_vecFreeActionQueue;      // 空闲动作队列
  std::string         m_strLoopActionName;           // 循环动作名称
  bool                m_bPlayLoopAction;	           // 是否播放循环动作
  ACTION_REFUSE_TYPE  m_eRefuseAction;	           // 是否拒绝动作
  PERSISTID           m_visID;
};