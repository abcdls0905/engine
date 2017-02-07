//--------------------------------------------------------------------
// �ļ���:		actor_util.h
// ��  ��:		ģ�͵Ĺ��߽ӿ�
// ˵  ��:		
// ��������:	2014��1��20��
// ������:		��־��
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
#ifndef ACTOR2_UTIL_H_
#define ACTOR2_UTIL_H_

#include "../visual/i_vis_base.h"
#include <map>

// ��ý�ɫ����
//IVisBase* get_actor_role(IVisBase* pRole);

// ��ʾӰ��
//void show_player_shadow(IVisBase* pRole);

// ����ʾӰ��
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