
#pragma once

#include "../../visual/i_vis_base.h"
#include "../../visual/i_game_client.h"
#include "../../visual/i_terrain.h"


// 获得下一个位置的有效高度
bool get_next_height(ITerrain* pTerrain, IVisBase* pRole, float dx, float dz, float step, float& floorY, int& floorIndex);


//根据地形状态，修正摇杆寻路的位置和方向
bool check_terrain_walk_condition_for_joystick(ITerrain* pTerrain, IVisBase* pRole, float fDestX, float fDestZ, float& fFinalMoveAngle, 
                                               float& fFinalDistance, int& nFinalFloor, float& fFinalX, float& fFinalY, float& fFinalZ);


// 考虑地形障碍，获得下一个位置的可行走层和高度，bAllowHeightDiff表示可通行地块之间允许高度差
bool get_next_floor(ITerrain* pTerrain, IVisBase* pRole, float dx, float dz,
                    int& res_floor_index, float& res_x, float& res_y, float& res_z, float fAllowHeightDiff = 1.2f);


// 获得与当前高度最接近的层高度
float get_cur_floor_height(ITerrain* pTerrain, float x, float y, float z, int* outfloor = NULL);