
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
  //�����¼�
  bool InPutEvent(ITerrain* pTerrain, IVisBase* pObj, int inPutType, const IVarList& args, const char* szFrom = "");

  // �л�����״̬
  bool StateSwitch(ITerrain* pTerrain, IVisBase* pObj, int iStateIndex, const char* szFrom = "");
private:
  IState* m_vecPlayerStateInfo[STATE_INDEX_MAX];
};