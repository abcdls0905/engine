
#ifndef _OBJ_STATE_DEFINE_H
#define _OBJ_STATE_DEFINE_H


// 状态机输入类型定义
enum PlayerInput
{
    PLAYER_INPUT_NULL = 0,     //无效输入

	PLAYER_INPUT_ENTRY,        // 进入状态
	PLAYER_INPUT_LEAVE,        // 退出状态
	PLAYER_INPUT_TIMER,        // 定时器刷新
	PLAYER_INPUT_LOCATION,     // 服务器强制定位
    PLAYER_INPUT_BEGIN_MOVE,   // 开始移动(摇杆寻路)
	PLAYER_INPUT_BEGIN_MOVE_TO,// 开始移动到目标点位置
	PLAYER_INPUT_END_MOVE,     // 结束移动
	PLAYER_INPUT_BE_MOVING,    // 服务器控制对象移动
	PLAYER_INPUT_LOGIC,        // 服务器逻辑产生的输入

	PLAYER_INPUT_MAX,
};

// PLAYER_INPUT_LOGIC的子类型
enum LogicIndex
{
    LOGIC_UNKOWN = 0,
    LOGIC_DIE  = 9,
    LOGIC_RELIVE  = 10,
    LOGIC_LOCK  = 11,
    LOGIC_UNLOCK  = 12,
    LOGIC_END_TRACE  = 32,
    LOGIC_START_MOVE = 33,
    LOGIC_MAX,
};

// 状态名字对应的状态索引值的定义
enum ObjStateIndex
{
    STATE_INDEX_NULL  =  0,     //无效状态

	STATE_STATIC_INDEX,         //主玩家静止
    STATE_MOTION_INDEX,         //主玩家遥感移动
	STATE_PATH_FINDING_INDEX,   //主玩家寻路移动
	STATE_BE_STOP_INDEX,        //非主玩家静止
	STATE_BE_MOTION_INDEX,      //非主玩家移动
	STATE_LOCKED_INDEX,         //锁定
	STATE_DEAD_INDEX,           //死亡

	STATE_INDEX_MAX,
};

#endif 
