
#include "scenemanager.h"

#include "../public/module.h"
#include "../public/var.h"
#include "../public/var_list.h"
#include "../visual/i_vis_base.h"
#include "../visual/i_terrain.h"
#include "../visual/i_scene.h"
#include "../visual/i_camera.h"
#include "global.h"
#include "helper.h"
#include "util/macro_util.h"
#include "statemachine/statemachine.h"

DECLARE_ENTITY(SceneObjManager, 0, IEntity);
SceneObjManager::SceneObjManager()
{
}

SceneObjManager::~SceneObjManager()
{
  CORE_REMOVE_EXECUTE(this);
}

bool SceneObjManager::Init(const IVarList& args)
{
  CORE_ADD_EXECUTE(this);
  return true;
}

bool SceneObjManager::Shut()
{
  CORE_REMOVE_EXECUTE(this);
  return true;
}

PERSISTID& SceneObjManager::GetPlayerId()
{
  return player;
}

void SceneObjManager::SetPlayer(const PERSISTID& id)
{
  player = id;
}

void SceneObjManager::Execute(float seconds)
{
  StateMachine* pStateMachine = CHelper::GetStateMachine();
  if (pStateMachine)
  {
    IVisBase* pRole = CHelper::GetPlayer();
    if (pRole)
      pStateMachine->InPutEvent(g_terrain, pRole, PLAYER_INPUT_TIMER, CVarList() << seconds);
  }
  return;
}
