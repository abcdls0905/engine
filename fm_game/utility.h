#pragma once

#include "../public/persistid.h"
#include "stage/stage.h"

class IActor;
class IGameObject;
class IControl;

extern bool terrain_load(const char* pstrfile, bool can_load_skybox = true);

extern void actor_motion(IActor* actor, float x, float y, float z, float speed);

extern IActor* actor_find(const PERSISTID& key);

extern void actor_set(const PERSISTID& key, const PERSISTID& actor_id);

extern void actor_remove(const PERSISTID& key);

extern void actor_setrole(const PERSISTID& key, const PERSISTID& actor_id);

extern void actor_clear();

extern IActor* actor_getrole();

extern PERSISTID gui_loadform(const char* file);

extern void gui_center(IControl* control);

extern void gui_showmbox(const wchar_t* text);

extern void stage_reg(int stage, StageBase* logic);

extern void stage_reg(int stage, const char* entity_name);

extern void stage_switch(int stage);

extern int stage_current();

extern IActor* role_composite_create(IGameObject* pGameObject);