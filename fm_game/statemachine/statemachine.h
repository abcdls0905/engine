
#pragma once

#include "istate.h"
#include "obj_state_define.h"
#include <map>
#include <vector>

class ITerrain;
class IVisBase;
class IVarList;

class StateMachine: public IEntity
{
public:
  StateMachine();
  ~StateMachine();
  virtual bool Init(const IVarList& args){ return true; }
  virtual bool Shut(){ return true; }
public:
  //输入事件
  bool InPutEvent(ITerrain* pTerrain, IVisBase* pObj, int inPutType, const IVarList& args, const char* szFrom = "");

  // 切换对象状态
  bool StateSwitch(ITerrain* pTerrain, IVisBase* pObj, int iStateIndex, const char* szFrom = "");
private:
  IState* m_vecPlayerStateInfo[STATE_INDEX_MAX];
};