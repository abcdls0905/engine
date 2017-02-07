
#include "../visual/i_vis_base.h"
#include "../visual/i_terrain.h"
#include "../public/i_var_list.h"
#include "../helper.h"
#include "movestate.h"
#include "stopstate.h"
#include "statemachine.h"
#include "custom_prop_define.h"


DECLARE_ENTITY(StateMachine, 0, IEntity);

static const char* GetStateName(int iStateIndex)
{
  switch(iStateIndex)
  {
  case STATE_STATIC_INDEX:
    return "static";
  case STATE_MOTION_INDEX:
    return "motion";
  case STATE_BE_MOTION_INDEX:
    return "be_motion";
  case STATE_PATH_FINDING_INDEX:
    return "path_finding";
  case STATE_BE_STOP_INDEX:
    return "be_stop";
  case STATE_LOCKED_INDEX:
    return "locked";
  case STATE_DEAD_INDEX:
    return "dead";
  default:
    return "";
  }
}

StateMachine::StateMachine()
{
  m_vecPlayerStateInfo[STATE_MOTION_INDEX] = CORE_NEW(MoveState);
  m_vecPlayerStateInfo[STATE_BE_STOP_INDEX] = CORE_NEW(StopState);
}

StateMachine::~StateMachine()
{
  for (int i = STATE_INDEX_NULL + 1; i < STATE_INDEX_MAX; ++i)
  {
    IState* pState = m_vecPlayerStateInfo[i];
    if (pState)
    {
      CORE_DELETE(pState);
      m_vecPlayerStateInfo[i] = NULL;
    }
  }
}

bool StateMachine::InPutEvent(ITerrain* pTerrain, IVisBase* pObj, int inPutType, const IVarList& args, const char* szFrom /*= ""*/)
{
  if (inPutType != PLAYER_INPUT_LOGIC)
  {
    int iStateIndex = CHelper::QueryCustomInt(pObj, CUSTOM_PROP_INT_STATE_INDEX);
    if (iStateIndex > STATE_INDEX_NULL && iStateIndex < STATE_INDEX_MAX)
    {
      IState* pState = m_vecPlayerStateInfo[iStateIndex];
      if (pState != NULL)
      {
        pState->InPutEvent(pTerrain, pObj, (PlayerInput)inPutType, args, szFrom);
      }
    }
  }
  return true;
}

bool StateMachine::StateSwitch(ITerrain* pTerrain, IVisBase* pObj, int iStateIndex, const char* szFrom /*= ""*/)
{
  int iOldStateIndex = CHelper::QueryCustomInt(pObj, CUSTOM_PROP_INT_STATE_INDEX);
  if (iStateIndex == iOldStateIndex)
  {
    return false;
  }

  // 离开老状态
  InPutEvent(pTerrain, pObj, PLAYER_INPUT_LEAVE, CVarList() << iOldStateIndex, szFrom);

  CHelper::SetCustomInt(pObj, CUSTOM_PROP_INT_STATE_INDEX, iStateIndex);
  CHelper::SetCustomString(pObj, CUSTOM_PROP_STRING_STATE, GetStateName(iStateIndex));

  // 进入新状态
  InPutEvent(pTerrain, pObj, PLAYER_INPUT_ENTRY, CVarList() << iStateIndex << iOldStateIndex, szFrom);
  return true;
}
