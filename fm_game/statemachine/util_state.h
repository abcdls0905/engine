
#pragma once

#include "../../public/Macros.h"
#include "../../public/Module.h"
#include "../../public/var_list.h"
#include "../../public/fast_str.h"
#include "../../public/core_log.h"
#include "../../visual/i_vis_base.h"
#include "../../visual/i_terrain.h"
#include <string>

//��ʼҡ��Ѱ·
bool player_begin_move(IVisBase* pRole, float fMoveAngle);

bool player_stop_move(IVisBase* pRole);

//��ʼת��Ŀ��
bool rotate_to_angle(IVisBase* pVisRole, float fDestAngle, bool bSmoothRotate, bool bSynLocalServer = false);

bool player_in_motion_move(float seconds, ITerrain* pTerrain, IVisBase* pRole);

//�ڵ�������ǰ�ƶ��������Ǹ߶�(����ң�п����ƶ�)
void move_to_custom_forward_pos(IVisBase* pRole, float seconds, float fMoveAngle, float& fNewX, float& fNewZ, float& fDistance);


// ����Ŀ��߶�ʹ��ƽ����̨��
bool check_footstep_height_condition(ITerrain* pTerrain, IVisBase* pRole, float fDestX, float fDestZ, 
                                     float fDistance, float& fFinalY, int& nFinalFloor);

// �ض�λ����,������floor_index
bool relocate_visual(ITerrain* pTerrain, IVisBase* pRole, float new_x, float new_y, float new_z, int nFloorIndex, bool bSynLocalServer);

// �ض�λ����
bool relocate_visual(ITerrain* pTerrain, IVisBase* pRole, float new_x, float new_y, float new_z, bool bSynLocalServer);