#include "../fm_gui/gui_input.h"
#include "../public/module.h"
#include "game.h"
#include "global.h"
#include "utility.h"
#include "../visual/i_scene.h"
#include "../visual/i_world.h"
#include "../visual/i_camera.h"
#include "../fm_gui/gui.h"
#include "../fm_gui/designer.h"
#include "../fm_gui/form.h"
#include "../visual/i_actor.h"
#include "camera/camera_control.h"
#include "../visual/i_context.h"
#include "../visual/i_render.h"
#include "../visual/i_game_sender.h"
#include "../public/core_log.h"
#include "game_control.h"
#include "scenemanager.h"
#include "helper.h"
#include "../interface/global_prop_define.h"
#include "statemachine/statemachine.h"

DECLARE_ENTITY(Game, 0, IEntity);

Game::~Game()
{
	stage_switch(STAGE_NONE);
}

bool Game::Init(const IVarList& args)
{
	stage_switch(-1);
	CORE_TRACE("Game Init");
	int ui_desgin_width = args.IntVal(0);
	int ui_desgin_height = args.IntVal(1);
	float scale = args.FloatVal(2);
	g_core = GetCore();
	g_world = (IWorld*)GetCore()->LookupEntity("World");
	g_render = (IRender*)GetCore()->GetInterface("Render");
	g_scene = (IScene*)g_core->CreateEntity("Scene");

	g_core->CreateEntity("GameObjMotion");

	//设置主场景实体
	g_world->SetMainSceneID(g_scene->GetID());
	
	g_core->CreateEntity("GameNet");

	stage_reg(STAGE_LOGIN, "StageLogin");	
	stage_reg(STAGE_MAIN, "StageMain");
	stage_reg(STAGE_SWITCHSCENE, "StageSwitchScene");
	stage_reg(STAGE_CLIENT, "StageClient");
	stage_reg(STAGE_TESTMODEL, "StageTestModel");

	PERSISTID light_man_id = g_scene->Create("LightManager");
	g_scene->AddObject(light_man_id,3);
	g_scene->GetContext()->SetUnique(g_core->GetEntity(light_man_id));

	GetCore()->CreateEntity("CameraControl");

	g_core->CreateEntity("RoleComposite");
	g_core->CreateEntity("FightNet");
	
	GetCore()->AddMsgProc(this, WM_LBUTTONDOWN);
	GetCore()->AddMsgProc(this, WM_KEYDOWN);
	Gui* pGui = (Gui*)GetCore()->CreateEntity("Gui");

//#define PERFORMANCE_DEBUG
#ifdef PERFORMANCE_DEBUG
	pGui->SetShowRenderInfo(true);
#endif

	g_world->SetMainGuiID(pGui->GetID());
	Designer* designeder = (Designer*)GetCore()->GetEntity(pGui->GetDesigner());
	designeder->LoadResource(GetCore()->GetWorkPath(), "res/skin/resource.xml");
	g_designeder = designeder;
	pGui->CreateDesktopForm();
	g_gui = pGui;
	g_gui->SetScaleEnable(scale != 1.0f);
	g_gui->SetScaleRatio(scale);
		
	Form* form = (Form*)pGui->GetDesktopForm();
	form->ShowModal();
	g_designeder->SetDesignMode(true);
	form->SetLeft(0);
	form->SetTop(0);
	ui_desgin_width = g_gui->GetWidth();
	ui_desgin_height = g_gui->GetHeight();
	form->SetWidth(ui_desgin_width);
	form->SetHeight(ui_desgin_height);
//	stage_switch(STAGE_LOGIN);
	//stage_switch(STAGE_TESTMODEL);
	
	//stage_switch(STAGE_CLIENT);
	CORE_TRACE("Game Init End");

	//开启RT调试模式
    PERSISTID post_debug_rt = g_scene->Create("CPostDebug");
    g_scene->AddObject(post_debug_rt,10000);

	g_scene->SetEnableRealizeTempColorRT(false);
  	g_scene->SetEnableRealizeTempDepthRT(false);
#ifdef TEST_RT
	PERSISTID pid = g_core->CreateEntity("TestRT")->GetID();
	if(!g_scene->AddObject(pid, 100))
	{
		Assert(0);
	}
#endif
#if defined TEST_3DUI
	//IEntity* pEntity = g_core->CreateEntity("UI3DTest");
#endif

	g_gamecontrol = (GameControl*)g_core->CreateEntity("GameControl");
  IEntity* sceneMgr = g_core->CreateEntity("SceneObjManager");
  CHelper::SetGlobalEntity(GLOBAL_SCENEMANAGER, sceneMgr);

  StateMachine* pStateMachine = (StateMachine*)CHelper::CreateEntity("StateMachine");
  if (pStateMachine)
  {
    CHelper::SetGlobalEntity(GLOBAL_PROP_ENT_StateMachine, pStateMachine);
  }

	stage_switch(STAGE_LOGIN);
	return true;
}

bool Game::Shut()
{
	return true;
}

// 窗口消息处理
bool Game::MsgProc(unsigned int msg, size_t param1, size_t param2, 
	int& result)
{

#if defined _WIN32 && defined PROCESS_LOST_DEVICE
	switch(msg)
	{
	case WM_KEYDOWN:
		{
			switch(param1)
			{
			case VK_F7:
				g_render->Invalidate();
				break;
			case VK_F9:
				g_render->Restore();
				break;
			}
		}
		break;
	}
#endif
	switch(msg)
	{
	case WM_LBUTTONDOWN:
#ifdef _WIN32
	//	g_render->ScreenShot("c:/work/screenshort/test.png");
#else
	//	g_render->ScreenShot("/sdcard/screenshort/test.png");
#endif
		break;
	}
	return false;
}