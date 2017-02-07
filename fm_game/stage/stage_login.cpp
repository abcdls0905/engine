#include "stage_login.h"
#include "global.h"
#include "utility.h"
#include "../visual/i_camera.h"
#include "../visual/i_scene.h"
#include "../visual/i_terrain.h"
#include "../public/core_log.h"

DECLARE_ENTITY(StageLogin, 0, IEntity);

void StageLogin::OnEnterStage()
{
	char mbzTemp[256];
	//arena
	sprintf(mbzTemp,"%s%s%s/", g_core->GetResourcePath(), "map/ter/", "home_s3_1");
	terrain_load(mbzTemp);
	g_terrain->SetVisible(false);
	ICamera* pCamera = (ICamera*)g_core->GetEntity(g_scene->GetCameraID());
	pCamera->SetPosition(2.497921, 27.894909, -10.118525);
	pCamera->SetAngle(-0.116355, -1.230457, 0.000000);
	//pCamera->SetPosition(0, 0, -10);
	//pCamera->SetAngle(0,0,0);
	//pCamera->SetPosition(0,0,0);
	//pCamera->SetAngle(0,0,0);
	CORE_TRACE("StageLoigin enter");
	//m_form_login.Show();
}


void StageLogin::OnLeaveStage()
{
	m_form_login.Close();
}