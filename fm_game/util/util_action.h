
#pragma once

#include "../../visual/i_vis_base.h"
#include "../action/action_queue.h"
#include <string>

// 播放循环动作
bool play_state(IVisBase* pRole, const char* action, ACTION_CLEAR_TYPE clear = eClearNone, 
                ACTION_INTERRUPT_TYPE bInterrupt = eInterrupt);