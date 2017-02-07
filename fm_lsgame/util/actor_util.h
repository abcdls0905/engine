//--------------------------------------------------------------------
// 文件名:		actor_util.h
// 内  容:		模型的工具接口
// 说  明:		
// 创建日期:	2014年1月20日
// 创建人:		张志明
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#ifndef ACTOR2_UTIL_H_
#define ACTOR2_UTIL_H_

#include "../visual/i_vis_base.h"
#include <map>

// 获得角色主体
//IVisBase* get_actor_role(IVisBase* pRole);

// 显示影子
//void show_player_shadow(IVisBase* pRole);

// 不显示影子
//void hide_player_shadow(IVisBase* pRole);

bool is_actor_createfinish(IVisBase* pRole);


class IActor;

extern void actor_motion(IActor* actor, float x, float y, float z, float speed);

extern void actor_stop(IActor* actor);

extern IActor* actor_find(const PERSISTID& key);

extern void actor_set(const PERSISTID& key, const PERSISTID& actor_id);

extern void actor_remove(const PERSISTID& key);

extern void actor_setrole(const PERSISTID& key, const PERSISTID& actor_id);

extern void actor_clear();

extern IActor* actor_getrole();

extern IActor* role_composite_create(IGameObject* pGameObject);

extern IActor* simple_role_composite_create_expose(int iRoleIndex);
extern void simple_role_composite_add_born_equipment(IVisBase* pActor2);
extern void simple_role_composite_refresh_part(IVisBase* pActor2, const std::string& prop_name, const std::wstring& prop_value);

IActor* simple_npc_create_from_ini(const std::string& npc_config);

#endif