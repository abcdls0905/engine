//--------------------------------------------------------------------
// 文件名:		Client\Code\FxGameLogic\Define.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年8月2日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _DEFINE_H
#define _DEFINE_H

//
#define UNIT_SIZE 0.5f
// 非法高度
#define ERROR_HEIGHT 1e8

#define ROLE_HEIGHT_BOY 1.8f  //男角色高度
#define ROLE_HEIGHT_GIRL 1.7f //女角色高度
#define ROLE_HEIGHT 1.8f
#define ROLE_HALF_HEIGHT 0.9f
#define CAN_JUMP_IN_HEIGHT 2.5f
#define CAN_JUMP_HEIGHT 2.0f
#define CAN_MOTION_HEIGHT 2.0f          // 能够移动的层空间高度
#define TRACE_HEIGHT_FIX 0.001f         // 两次trace的误差范围
#define FLOOR_HEIGHT_FIX 0.1f           // 层最大误差
#define CAN_WALK_HEIGHT 1.0f
#define SWIM_DEEP 0.9f

// 必须同步目标点的距离
#define MUST_SYNC_DISTANCE 1000.0
// 无需同步目标点
#define NOT_SYNC_DISTANCE 0.0

// 位于水面标志
#define FLOOR_AT_WATER 200
// 位于空中标志
#define FLOOR_IN_AIR 300
// 位于水下标志
#define FLOOR_UNDER_WATER 400

// 非法高度
#define ERROR_HEIGHT 1e8
// 最小移动距离
#define MIN_DISTANCE 0.1f
#define MIN_FACE_CHANGE_DISTANCE 0.5f
// 最小碰撞距离
#define MIN_COLLIDE_RADIUS 0.5f


#endif 

