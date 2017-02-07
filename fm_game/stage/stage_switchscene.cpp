#include "stage_switchscene.h"
#include "utility.h"
#include "global.h"
#include "../visual/i_terrain.h"
#include "../stage/stage.h"
#include "../fm_gui/gui.h"
#include "../fm_gui/form.h"
#include "../fm_gui/progress_bar.h"

DECLARE_ENTITY(StageSwitchScene, 0, IEntity);

StageSwitchScene::StageSwitchScene(void)
{
}

StageSwitchScene::~StageSwitchScene(void)
{
}

void StageSwitchScene::OnEnterStage()
{
	GetCore()->AddExecute(this);
	m_sceneload = false;
	Form* form_desktop = (Form*)g_gui->GetDesktopForm();
	m_form_id = gui_loadform("res/skin/form_common/form_loading.xml");
	Form* form = (Form*)g_core->GetEntity(m_form_id);
	int w = form->GetWidth();
	int h = form->GetHeight();
	form->SetWidth(g_gui->GetWidth());
	form->SetHeight(g_gui->GetHeight());
	
	IControl* lbl = form->FindChild("lbl_background");
	lbl->SetWidth(g_gui->GetWidth());
	lbl->SetHeight(g_gui->GetHeight());

	form->Show();
}

void StageSwitchScene::OnLeaveStage()
{
	GetCore()->RemoveExecute(this);
	Form* form = (Form*)g_core->GetEntity(m_form_id);
	form->Close();
	
}

void StageSwitchScene::Execute(float secontds)
{
	if(g_terrain)
	{
		Form* form = (Form*)g_core->GetEntity(m_form_id);
		IControl* pControl = form->FindChild("pbar_load");
		if(pControl)
		{
			((ProgressBar*)pControl)->SetMaximum(100);
			((ProgressBar*)pControl)->SetMinimum(0);
			((ProgressBar*)pControl)->SetCurValue(g_terrain->GetLoadPercent() * 100.0f);
		}
		if(m_sceneload != g_terrain->GetLoadFinish())
		{
			m_sceneload = true;
			stage_switch(STAGE_MAIN);
		}

	}
}