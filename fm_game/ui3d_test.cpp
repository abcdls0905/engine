#include "../public/module.h"
#include "ui3d_test.h"
#include "../visual/i_scene.h"
#include "../visual/i_camera.h"
#include "../visual/i_vis_base.h"
#include "../visual/i_render.h"
#include "../fm_gui/gui.h"
#include "../fm_gui/form.h"
#include "../fm_gui/designer.h"
#include "../visual/i_actor.h"
#include "../visual/i_ui3d.h"


extern IRender* g_render;

bool UI3DTest::Init(const IVarList& args)
{
	IScene* pScene = (IScene*)g_pCore->LookupEntity("Scene");
	ICamera* pCamera = (ICamera*)g_pCore->GetEntity(pScene->GetCameraID());
	pCamera->SetPosition(0, 0, 0);
	pCamera->SetAngle(0,0,0);

	IVisBase* pVisBase = (IVisBase*)g_pCore->CreateEntity("UI3D");
	pVisBase->SetContext(g_render->GetContext());
	pScene->AddObject(pVisBase->GetID(), 0);
	IUI3DCore* pcore = (IUI3DCore*)g_pCore->GetInterface("ui3d_core");
	pcore->AddToScene(pScene);
	

	pVisBase->SetPosition(0, 0, 1.2);
	pVisBase->SetScale(0.5, 0.5, 0.5);

	IActor* pActor = (IActor*)g_pCore->CreateEntity("Actor");
	pActor->SetContext(pScene->GetContext());
	pActor->CreateFromIni("obj/char/face/composite.ini");
	pActor->SetPosition(0, 0, 1.2);
	pActor->SetAngle(0, 3.1415925, 0);
	pActor->SetScale(1.5, 1.5, 1.5);

	g_pCore->AddExecute(this);
	return true;
}

bool UI3DTest::Shut()
{
	return true;
}

bool UI3DTest::MsgProc(unsigned int msg, size_t param1, size_t param2, 
		int& result)
{
	return false;
}

void UI3DTest::Execute(float seconds)
{
	//IPainter* pRender = (IPainter*)g_pCore->GetInterface("Painter");
	//pRender->DrawRect(0, 0, 200, 200, 0xffff0000);
}
// 窗口消息处理

DECLARE_ENTITY(UI3DTest, 0, IEntity);



