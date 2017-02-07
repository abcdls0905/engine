//----------------------------------------------------------
// 文件名:		CustomPropDefine
// 内  容:		客户端自定义属性定义
// 说  明:		自定义属性太多了，放在这里统一定义并详细说明方便查看
//              ********为了便于区分客户端自定义属性全部用小写******
// 创建人:		chenhh
// 创建日期:    2013/06/18
// 版权所有:    苏州蜗牛电子有限公司
//----------------------------------------------------------

#ifndef CUSTOM_PROP_DEFINE_H_
#define CUSTOM_PROP_DEFINE_H_


#define  CUSTOM_PROP_STRING_STATE         "state"       //模型状态
#define  CUSTOM_PROP_INT_STATE_INDEX      "state_index"  //模型状态索引

#define  CUSTOM_PROP_OBJECT_TERRAIN      "terrain"   //场景的地形
#define  CUSTOM_PROP_OBJECT_SCENE        "scene"   //对象所属的场景
#define  CUSTOM_PROP_HIDE_NPC_BAR        "hide_npc_bar" //是否隐藏NPC头顶血条

#define  CUSTOM_PROP_STRING_CLIENT_IDENT   "client_ident"   //对象的客户端网络标示
#define  CUSTOM_PROP_NPC_SCRIPT                          "npc_script"  //客户端对象 设置npcscript
/*#define  CUSTOM_PROP_OBJECT_SERVE_RDEAD		"ServerDead" // */
#define  CUSTOM_PROP_OBJECT_CREATE_FINISH   "createfinish" // 是否创建完成 

//主角是否已经放置到场景
#define  CUSTOM_PROP_BOOL_MAIN_ROLE_ADD_TO_TERRAIN  "main_role_addto_terrain"

#define  CUSTOM_PROP_LAST_OBJECT  "lastobject"
//------------------------------< Action >----------------------------------------
#define  ACTION_ACTION_SET					"action_set"
#define  ACTION_OLD							"action_old"
#define  ACTION_DIEDOWN						"diedown"

//------------------------------< Skill >----------------------------------------
#define CUSTOM_PROP_SKILL_NO_CONSUME					"skill_no_consume"  //技能无消耗
#define CUSTOM_PROP_SKILL_ATTACK_SPEED_RATIO			"attack_speed_ratio"  //技能攻击速度比例
#define CUSTOM_PROP_SKILL_TARGET_COUNT					"target_count"		//目标数目
#define CUSTOM_PROP_SKILL_CAN_PAUSE						"can_pause"		//停顿
#define CUSTOM_PROP_EFFECT_PLAY_HURT_EFFECT				"play_hurt_effect"	//技能随机动作播放特效
#define CUSTOM_PROP_EFFECT_PLAY_HURTER					"play_hurter"		//播放受击动作对象
#define CUSTOM_PROP_SKILL_HURT_PLAYER					"hurt_player"		// 受伤的攻击者
#define CUSTOM_PROP_SKILL_DAMAGE_TYPE					"damage_type"		// 伤害类型
#define CUSTOM_PROP_CANTCOMBATACTION					"cant_combat_action"	// 是否不播放受击动作
#define CUSTOM_PROP_SKILL_DAMAGE_TYPE					"damage_type"		// 伤害类型
//------------------------------< State >----------------------------------------
#define STATE_MOVE_ANGLE                    "move_angle"
#define STATE_MOVE_DIST                     "move_distance"
#define STATE_FACE_ANGLE                    "face_angle"
#define STATE_LOCK_MOVE                     "lock_move"
#define STATE_LOCK_FACE                     "lock_face"
#define STATE_LOCK_SKILL                    "lock_skill"
#define STATE_CANT_USENORMAL        "notusenormal"
#define STATE_JOYSTICK_ANGLE                "joystick_angle"



#endif