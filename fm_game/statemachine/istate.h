
#pragma once

#include "obj_state_define.h"
#include "../public/i_entity.h"
#include <map>

class ITerrain;
class IVisBase;
class IVarList;

typedef bool (* OBJ_INPUT_FUNC)(ITerrain* pTerrain, IVisBase* pRole, int inPutType, const IVarList& args);

struct ObjInPutInfo
{
  OBJ_INPUT_FUNC pFunc;
  ObjInPutInfo():pFunc(NULL){}
};

class IState
{
public:
  virtual bool InPutEvent(ITerrain* pTerrain, IVisBase* pRole, PlayerInput inPutType, const IVarList& args, const char* szFrom);
protected:
  void RegStateFunc(PlayerInput inputType, OBJ_INPUT_FUNC pFunc);
protected:
  ObjInPutInfo m_vecInputFunc[PLAYER_INPUT_MAX];
};