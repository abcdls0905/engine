
#pragma once

#include "obj_state_define.h"
#include "istate.h"


class  StopState: public IState
{
public:
  StopState();

  //此状态下对输入的处理
  static bool StateStopEnter(ITerrain* pTerrain, IVisBase* pRole, int inPutType, const IVarList& args);
  static bool StateStopLeave(ITerrain* pTerrain, IVisBase* pRole, int inPutType, const IVarList& args);
  static bool StateStopTimer(ITerrain* pTerrain, IVisBase* pRole, int inPutType, const IVarList& args);
  static bool StateStopBeMoving(ITerrain* pTerrain, IVisBase* pRole, int inPutType, const IVarList& args);
  static bool StateStopLocation(ITerrain* pTerrain, IVisBase* pRole, int inPutType, const IVarList& args);
};