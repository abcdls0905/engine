
#include "stopstate.h"
#include "util/util_action.h"
#include "../fm_world/actor.h"

StopState::StopState()
{
  RegStateFunc(PLAYER_INPUT_ENTRY,			StopState::StateStopEnter);
  RegStateFunc(PLAYER_INPUT_LEAVE,			StopState::StateStopLeave);
  RegStateFunc(PLAYER_INPUT_TIMER,			StopState::StateStopTimer);
  RegStateFunc(PLAYER_INPUT_BE_MOVING,  StopState::StateStopBeMoving);
  RegStateFunc(PLAYER_INPUT_LOCATION,		StopState::StateStopLocation);
}

bool StopState::StateStopEnter(ITerrain* pTerrain, IVisBase* pRole, int inPutType, const IVarList& args)
{
  Actor* pActor = (Actor*)pRole;
  if (pActor)
  {
    CVarList result;
    pActor->ClearBlendAction();
    pActor->BlendAction(CVarList()<< "pos_0h_stand" << true << false, result);
  }
  return true;
}

bool StopState::StateStopLeave(ITerrain* pTerrain, IVisBase* pRole, int inPutType, const IVarList& args)
{
  return true;
}

bool StopState::StateStopTimer(ITerrain* pTerrain, IVisBase* pRole, int inPutType, const IVarList& args)
{
  return true;
}

bool StopState::StateStopBeMoving(ITerrain* pTerrain, IVisBase* pRole, int inPutType, const IVarList& args)
{
  return true;
}

bool StopState::StateStopLocation(ITerrain* pTerrain, IVisBase* pRole, int inPutType, const IVarList& args)
{
  return true;
}

