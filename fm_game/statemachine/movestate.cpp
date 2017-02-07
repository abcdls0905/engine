
#include "movestate.h"
#include "util/util_action.h"
#include "../fm_world/actor.h"
#include "helper.h"
#include "custom_prop_define.h"
#include "statemachine/util_state.h"


MoveState::MoveState()
{
  RegStateFunc(PLAYER_INPUT_ENTRY,		MoveState::StateMotionEnter);
  RegStateFunc(PLAYER_INPUT_TIMER,		MoveState::StateMotionTimer);
  RegStateFunc(PLAYER_INPUT_BEGIN_MOVE_TO, MoveState::StateMotionBeginMoveTo);
  RegStateFunc(PLAYER_INPUT_END_MOVE,		MoveState::StateMotionEndMove);
  RegStateFunc(PLAYER_INPUT_BE_MOVING,	MoveState::StateMotionBeMoving);
  RegStateFunc(PLAYER_INPUT_LOCATION,		MoveState::StateMotionLocation);
}

bool MoveState::StateMotionEnter(ITerrain* pTerrain, IVisBase* pRole, int inPutType, const IVarList& args)
{
  Actor* pActor = (Actor*)pRole;
  if (pActor)
  {
    CVarList result;
    pActor->BlendAction(CVarList()<< "pe_run" << true << false, result);
  }
  return true;
}

bool MoveState::StateMotionTimer(ITerrain* pTerrain, IVisBase* pRole, int inPutType, const IVarList& args)
{
  float seconds = args.FloatVal(0);
  player_in_motion_move(seconds, pTerrain, pRole);
  return true;
}

bool MoveState::StateMotionBeginMoveTo(ITerrain* pTerrain, IVisBase* pRole, int inPutType, const IVarList& args)
{
  return true;
}

bool MoveState::StateMotionEndMove(ITerrain* pTerrain, IVisBase* pRole, int inPutType, const IVarList& args)
{
  return true;
}

bool MoveState::StateMotionBeMoving(ITerrain* pTerrain, IVisBase* pRole, int inPutType, const IVarList& args)
{
  return true;
}

bool MoveState::StateMotionLocation(ITerrain* pTerrain, IVisBase* pRole, int inPutType, const IVarList& args)
{
  return true;
}
