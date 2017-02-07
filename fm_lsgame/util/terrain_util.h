
#pragma once

#include "../../visual/i_vis_base.h"
#include "../../visual/i_terrain.h"


extern bool terrain_load(const char* pstrfile, bool can_load_skybox = false);
extern void terrain_unload();
