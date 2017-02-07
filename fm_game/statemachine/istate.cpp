
#include "istate.h"

bool IState::InPutEvent(ITerrain* pTerrain, IVisBase* pRole, PlayerInput inPutType, const IVarList& args, const char* szFrom)
{
  if (inPutType > PLAYER_INPUT_NULL && inPutType < PLAYER_INPUT_MAX)
  {
    OBJ_INPUT_FUNC pFunc = m_vecInputFunc[inPutType].pFunc;
    if (pFunc)
    {
      return pFunc(pTerrain, pRole, inPutType, args);
    }
  }
  return false;
}

void IState::RegStateFunc(PlayerInput inPutType, OBJ_INPUT_FUNC pFunc)
{
  if (inPutType > PLAYER_INPUT_NULL && inPutType < PLAYER_INPUT_MAX)
  {
    m_vecInputFunc[inPutType].pFunc = pFunc;
  }
}
