
#include "game.h"
#include "helper.h"
#include "../interface/global_prop_define.h"
#include "../visual/i_scene.h"
#include "../visual/i_world.h"
#include "../visual/i_camera.h"
#include "../fm_gui/gui.h"
#include "../fm_gui/designer.h"
#include "../fm_gui/form.h"
#include "camera/free_camera.h"

#include "global.h"

#include "../public/module.h"
#include "stagemanager/stagemanager.h"

#include "formlogic/guimanager.h"

#ifdef FX_SYSTEM_WINDOWS
#endif

DECLARE_ENTITY(Game, 0, IEntity);

Game::Game()
{

}

Game::~Game()
{

}

bool Game::Init(const IVarList& args)
{
	int iDesginWidth = args.IntVal(0);
	int iDesginHeight = args.IntVal(1);
	float fScale = args.FloatVal(2);
	g_core = GetCore();

	g_world = (IWorld*)GetCore()->LookupEntity("World");
	g_render = (IRender*)GetCore()->GetInterface("Render");

	Gui* pGui = (Gui*)GetCore()->CreateEntity("Gui");
	if (pGui == NULL)
	{
		//TraceLog(LOG_ERROR, "[Game::Init] Create gui fail");
		return false;
	}
	g_world->SetMainGuiID(pGui->GetID());

	pGui->SetShowRenderInfo(false);
	pGui->SetShowFPS(true);

	//
	Designer* pDesigneder = (Designer*)GetCore()->GetEntity(pGui->GetDesigner());
	if (pDesigneder != NULL)
	{
		pDesigneder->LoadResource(GetCore()->GetResourcePath(), "skin/resource.xml");
		g_designeder = pDesigneder;
	}

	pGui->CreateDesktopForm();

	g_gui = pGui;
	g_gui->SetScaleEnable(fScale != 1.0f);
	g_gui->SetScaleRatio(fScale);


	Form* pDesktopForm = (Form*)pGui->GetDesktopForm();
	if (pDesktopForm != NULL)
	{
		//pDesktopForm->SetBackImage("gui/common/window/bg.jpg");
// 		pDesktopForm->ShowModal();
// 
// 		g_designeder->SetDesignMode(false);
// 		pDesktopForm->SetLeft(0);
// 		pDesktopForm->SetTop(0);
// 		pDesktopForm->SetWidth(pGui->GetWidth());
// 		pDesktopForm->SetHeight(pGui->GetHeight());
	}


	//逻辑界面管理器
	GuiManager*pGuiManager = (GuiManager*)CreateEnt("GuiManager", GLOBAL_PROP_ENT_GuiManager);
	if (pGuiManager != NULL)
	{
		pGuiManager->InitConfig();
	}
	

	g_scene = (IScene*)g_core->CreateEntity("Scene");
	g_world->SetMainSceneID(g_scene->GetID());

	CreateEnt();

	StageManager* pStageManager = (StageManager*)CHelper::GetGlobalEntity(GLOBAL_PROP_ENT_stage_manager);
	if (pStageManager != NULL)
	{
		pStageManager->SwitchStage(STAGE_LOGIN);
	}


	ICamera* pCamera = (ICamera*) g_core->GetEntity(g_scene->GetCameraID());

	pCamera->SetAngleSpeed(-0.21544901f,11.042315f,0.0f);

	FreeCamera* pC = (FreeCamera*)g_core->CreateEntity("FreeCamera");
	pC->SetBindCamera(g_scene->GetCameraID());
	pC->SetAllowControl(true);
	pCamera->SetPosition(-18.583885, 20.963966, 9.031261);
	pCamera->SetAngle(0.0f, FM_PIf,0.0f);

#ifdef FX_SYSTEM_WINDOWS
#endif
	//光源管理器
	PERSISTID light_man_id = g_scene->Create("LightManager");
	g_scene->AddObject(light_man_id,3);

	return true;
}

bool Game::Shut()
{
	DeleteEnt();
	return true;
}

void Game::CreateEnt()
{
	CreateEnt("StageManager", GLOBAL_PROP_ENT_stage_manager);
}

IEntity* Game::CreateEnt(std::string strEntName, std::string strGlobalName/*=""*/)
{
	std::map<std::string, size_t>::iterator itFind = m_mapNameToIndex.find(strEntName.c_str());
	if (itFind != m_mapNameToIndex.end())
	{
		//TraceLog(LOG_ERROR, "[Game::CreateEnt] Ent %s is have", strEntName.c_str());
		return NULL;
	}

	//
	IEntity* pEnt = CHelper::CreateEntity(strEntName.c_str(), strGlobalName);
	if (pEnt == NULL)
	{
		//TraceLog(LOG_ERROR, "[Game::CreateEnt] Create Ent %s Fail", strEntName.c_str());
		return NULL;
	}

	//
	m_mapNameToIndex[strEntName] = m_vecEnt.size();
	m_vecEnt.push_back(pEnt->GetID());

	return pEnt;
}

void Game::DeleteEnt()
{
	for (size_t i=0; i<m_vecEnt.size(); i++)
	{
		PERSISTID id = m_vecEnt[i];
		IEntity* pEnity=GetCore()->GetEntity(id);
		if (pEnity)
		{
			pEnity->Release();
		}
	}
}
