
#pragma once

#include "../../public/Macros.h"
#include "../../public/Module.h"
#include "../../public/var_list.h"
#include "../../public/fast_str.h"
#include "../../public/core_log.h"
#include "../../visual/i_vis_base.h"
#include "../../visual/i_terrain.h"
#include <string>

//开始摇杆寻路
bool player_begin_move(IVisBase* pRole, float fMoveAngle);

bool player_stop_move(IVisBase* pRole);

//开始转向目标
bool rotate_to_angle(IVisBase* pVisRole, float fDestAngle, bool bSmoothRotate, bool bSynLocalServer = false);

bool player_in_motion_move(float seconds, ITerrain* pTerrain, IVisBase* pRole);

//在地面上往前移动，不考虑高度(用于遥感控制移动)
void move_to_custom_forward_pos(IVisBase* pRole, float seconds, float fMoveAngle, float& fNewX, float& fNewZ, float& fDistance);


// 调整目标高度使其平滑上台阶
bool check_footstep_height_condition(ITerrain* pTerrain, IVisBase* pRole, float fDestX, float fDestZ, 
                                     float fDistance, float& fFinalY, int& nFinalFloor);

// 重定位对象,并设置floor_index
bool relocate_visual(ITerrain* pTerrain, IVisBase* pRole, float new_x, float new_y, float new_z, int nFloorIndex, bool bSynLocalServer);

// 重定位对象
bool relocate_visual(ITerrain* pTerrain, IVisBase* pRole, float new_x, float new_y, float new_z, bool bSynLocalServer);