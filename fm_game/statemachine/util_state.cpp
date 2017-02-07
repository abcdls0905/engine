
#include "../global.h"
#include "util_state.h"
#include "../helper.h"
#include "../tools/util_func.h"
#include "statemachine/statemachine.h"
#include "../visual/i_camera.h"
#include "../visual/i_scene.h"
#include "custom_prop_define.h"
#include "../util/util_terrain.h"

bool player_begin_move(IVisBase* pRole, float fMoveAngle)
{
  rotate_to_angle(pRole, fMoveAngle, true, true);
  if (pRole)
    CHelper::GetStateMachine()->StateSwitch(g_terrain, pRole, STATE_MOTION_INDEX, "");
  return true;
}


bool rotate_to_angle(IVisBase* pRole, float fDestAngle, bool bSmoothRotate, bool bSynLocalServer /*= false*/)
{
  if (!pRole)
    return false;
  ICamera* pCameraEnity = (ICamera*)CHelper::GetEntity(g_scene->GetCameraID());
  float fNewMoveAngle = util_normalize_angle(pCameraEnity->GetAngle().y + fDestAngle);
  CHelper::SetCustomFloat(pRole, STATE_JOYSTICK_ANGLE, fNewMoveAngle);
  fDestAngle = util_normalize_angle(fNewMoveAngle);
  CHelper::SetCustomFloat(pRole, STATE_FACE_ANGLE, fDestAngle);
  FmVec3 vAngle = pRole->GetAngle();
  pRole->SetAngle(vAngle.x, fDestAngle, vAngle.z);
  return true;
}

bool player_in_motion_move(float seconds, ITerrain* pTerrain, IVisBase* pRole)
{
  if(pTerrain == NULL || pRole == NULL)
    return false;

  int nFloorIndex = 0;
  float fDistance = 0.f;
  float fDestX = 0.f, fDestY = 0.f, fDestZ = 0.f;

  float fNewMoveAngle = CHelper::QueryCustomFloat(pRole, STATE_JOYSTICK_ANGLE);

  // 计算向前移动将到达的位置
  move_to_custom_forward_pos(pRole, seconds, fNewMoveAngle, fDestX, fDestZ, fDistance);

  // 检测当前地形是否可行走并调整目标位置
  if (!check_terrain_walk_condition_for_joystick(pTerrain, pRole, fDestX, fDestZ, fNewMoveAngle, 
    fDistance, nFloorIndex, fDestX, fDestY, fDestZ))
  {
    return true;
  }

  // 检测当前地形是否可行走并调整目标位置
  if (!check_terrain_walk_condition_for_joystick(pTerrain, pRole, fDestX, fDestZ, fNewMoveAngle, 
    fDistance, nFloorIndex, fDestX, fDestY, fDestZ))
  {
    return true;
  }

  //更新移动方向
  CHelper::SetCustomFloat(pRole, STATE_MOVE_ANGLE, fNewMoveAngle);

  // 调整目标高度使其平滑上台阶
  float fFloorY = 0.0;
  if (check_footstep_height_condition(pTerrain, pRole, fDestX, fDestZ, fDistance, fFloorY, nFloorIndex))
  {
    fDestY = fFloorY;
  }

  relocate_visual(pTerrain, pRole, fDestX, fDestY, fDestZ, nFloorIndex, false);

  return true;
}

void move_to_custom_forward_pos(IVisBase* pRole, float seconds, float fMoveAngle, float& fNewX, float& fNewZ, float& fDistance)
{
  float fMoveSpeed = CHelper::QueryCustomFloat(pRole, "move_speed");
  if (fMoveSpeed < 0.001f)
  {
    fMoveSpeed = 5.0f;
  }
  fDistance = fMoveSpeed * seconds;

  FmVec3 pos = pRole->GetPosition();
  fNewX = pos.x + fDistance * sin(fMoveAngle);
  fNewZ = pos.z + fDistance * cos(fMoveAngle);
}


bool check_footstep_height_condition(ITerrain* pTerrain, IVisBase* pRole, float fDestX, float fDestZ, float fDistance, float& fFinalY, int& nFinalFloor)
{
  FmVec3 pos = pRole->GetPosition();
  float fMoveAngle = CHelper::QueryCustomFloat(pRole, STATE_MOVE_ANGLE);
  float fFrontX = pos.x + sinf(fMoveAngle);
  float fFrontZ = pos.z + cosf(fMoveAngle);
  float fFrontY = pos.y;
  const float MAX_STEP = 4.0f;

  if (get_next_height(pTerrain, pRole, fFrontX, fFrontZ, MAX_STEP, fFrontY, nFinalFloor))
  {
    // 插值高度
    fFinalY = pos.y + (fFrontY - pos.y) * fDistance;
    return true;
  }
  return false;
}

bool relocate_visual(ITerrain* pTerrain, IVisBase* pRole, float new_x, float new_y, float new_z, int nFloorIndex, bool bSynLocalServer)
{
  CHelper::SetCustomInt(pRole, "floor_index", nFloorIndex);
  return relocate_visual(pTerrain, pRole, new_x, new_y, new_z, bSynLocalServer);
}

bool relocate_visual(ITerrain* pTerrain, IVisBase* pRole, float new_x, float new_y, float new_z, bool bSynLocalServer)
{
  if (pTerrain == NULL || pRole == NULL)
    return false;
  //设置位置
  pTerrain->RelocateVisBase(pRole, new_x, new_y, new_z);
  return true;
}

bool player_stop_move(IVisBase* pRole)
{
  if (pRole)
  {
    CHelper::GetStateMachine()->StateSwitch(g_terrain, pRole, STATE_BE_STOP_INDEX, "");
    return true;
  }
  return false;
}
