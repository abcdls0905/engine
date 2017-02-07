
#pragma once

#include "../../visual/i_vis_base.h"
#include "../../visual/i_game_client.h"
#include "../../visual/i_terrain.h"


// �����һ��λ�õ���Ч�߶�
bool get_next_height(ITerrain* pTerrain, IVisBase* pRole, float dx, float dz, float step, float& floorY, int& floorIndex);


//���ݵ���״̬������ҡ��Ѱ·��λ�úͷ���
bool check_terrain_walk_condition_for_joystick(ITerrain* pTerrain, IVisBase* pRole, float fDestX, float fDestZ, float& fFinalMoveAngle, 
                                               float& fFinalDistance, int& nFinalFloor, float& fFinalX, float& fFinalY, float& fFinalZ);


// ���ǵ����ϰ��������һ��λ�õĿ����߲�͸߶ȣ�bAllowHeightDiff��ʾ��ͨ�еؿ�֮������߶Ȳ�
bool get_next_floor(ITerrain* pTerrain, IVisBase* pRole, float dx, float dz,
                    int& res_floor_index, float& res_x, float& res_y, float& res_z, float fAllowHeightDiff = 1.2f);


// ����뵱ǰ�߶���ӽ��Ĳ�߶�
float get_cur_floor_height(ITerrain* pTerrain, float x, float y, float z, int* outfloor = NULL);