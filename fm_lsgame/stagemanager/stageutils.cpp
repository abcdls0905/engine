

#include "stageutils.h"

#include "../visual/i_Scene.h"
#include "../visual/i_terrain.h"
#include "../visual/i_camera.h"
#include "../visual/i_render.h"

#include "../util/terrain_util.h"

StageUtils::StageUtils(void)
{

}

StageUtils::~StageUtils(void)
{

}

void StageUtils::LoadLoginScene()
{
	char szScenePath[256];
	sprintf(szScenePath,"%s%s%s/", g_core->GetResourcePath(), "map/ter/", LOGIN_SCENE_NAME);

	g_render->SetContext(g_scene->GetContext());
	terrain_load(szScenePath);
}
