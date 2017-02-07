
#include "obj_state_define.h"
#include "istate.h"

class MoveState : public IState
{
public:
  MoveState();

  static bool StateMotionEnter(ITerrain* pTerrain, IVisBase* pRole, int inPutType, const IVarList& args);
  static bool StateMotionTimer(ITerrain* pTerrain, IVisBase* pRole, int inPutType, const IVarList& args);
  static bool StateMotionBeginMoveTo(ITerrain* pTerrain, IVisBase* pRole, int inPutType, const IVarList& args);
  static bool StateMotionEndMove(ITerrain* pTerrain, IVisBase* pRole, int inPutType, const IVarList& args);
  static bool StateMotionBeMoving(ITerrain* pTerrain, IVisBase* pRole, int inPutType, const IVarList& args);
  static bool StateMotionLocation(ITerrain* pTerrain, IVisBase* pRole, int inPutType, const IVarList& args);
};