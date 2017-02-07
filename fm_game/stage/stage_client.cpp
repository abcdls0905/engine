#include "global.h"
#include "utility.h"
#include "stage_client.h"
#include "../public/module.h"
#include "../visual/i_camera.h"
#include "../visual/i_scene.h"
#include "../visual/i_model_system.h"
#include "../visual/i_particle_manager.h"
#include "../visual/i_particle.h"
#include "../visual/i_terrain.h"
#include "../visual/i_model.h"
#include "../fm_gui/gui_input.h"


DECLARE_ENTITY(StageClient, 0, IEntity);

void StageClient::OnEnterStage()
{
	char mbzTemp[512];
	//arena
//	sprintf(mbzTemp,"%s%s%s\\", g_core->GetResourcePath(), "map\\ter\\", "login02");
	sprintf(mbzTemp,"%s%s%s\\", g_core->GetResourcePath(), "map\\ter\\", "login2d");
	terrain_load(mbzTemp);

	ICamera* pCamera = (ICamera*) g_core->GetEntity(g_scene->GetCameraID());
	pCamera->SetPosition(212, 190, 356);
	pCamera->SetAngle(0.218163, 10.513f, 0.00000000f);
	//pCamera->SetPosition(255.84540f, 25.122454f,-85.781944f);
	//pCamera->SetAngle(0.087266505f, -4.6976604f, 0.00000000f);
	/*
	ParticleManager* pParticleMan = (ParticleManager*)g_core->LookupEntity("ParticleManager");
	//´´½¨Á£×Ó
	PERSISTID particle = pParticleMan->CreateFromIni("map\\ini\\particles_mdl.ini" , "changjing_006");
	Particle* pParticle = (Particle*) g_core->GetEntity(particle);
	pParticle->SetPosition(279.0f,23.247999f,-84.583f);
	//g_scene->AddObject(pParticle->GetID(), 100);
	g_terrain->AddVisual("ffdds", particle);*/
	int n = 3;
	
	g_core->AddMsgProc(this, WM_LBUTTONDOWN);
}

void StageClient::OnLeaveStage()
{
}

bool StageClient::MsgProc(unsigned int msg, size_t param1, size_t param2, 
	int& result)
{
	switch(msg)
	{
	case WM_LBUTTONDOWN:
		{
			long x = LOWORD(param2);
			long y = HIWORD(param2);
			CVarList result;
			PERSISTID pick = g_terrain->PickVisual(x, y, 300);
			if(!pick.IsNull())
			{
				
				if(g_core->GetEntity(pick)->GetEntInfo()->IsKindOf("EffectModel"))
				{
					int n = 3;
				}
				else if(g_core->GetEntity(pick)->GetEntInfo()->IsKindOf("Model"))
				{
					((IModel*)g_core->GetEntity(pick))->SetShowBoundBox(true);
					((IModel*)g_core->GetEntity(pick))->SetVisible(false);
				}

				
			}
		}
		break;
	}
	return false;
}