#include "ui3d_core.h"
#include "shader.h"
#include "../fm_gui/gui.h"
#include "mesh.h"
#include "animation.h"
#include "../visual/i_scene.h"
#include "defines.h"
#include "global.h"

DECLARE_FUNCTION_INTERFACE(ui3d_core);



bool ui3d_core::Init()
{
	g_fm_ui3d_gloabl.pRender = (IRender*)g_pCore->GetInterface("Render");
	m_renderui = (RenderUI*) g_pCore->CreateEntity("RenderUI");
	ui3d::Shader::Inst().Init();
	
	return true;
}

void ui3d_core::AddToScene(IScene* pScene)
{
	m_renderui->SetContext(pScene->GetContext());
	pScene->AddObject(m_renderui->GetID(), 0);
}

bool ui3d_core::Shut()
{
	ui3d::Shader::Inst().Destroy();
	return true;
}

IUI3DAnimation* ui3d_core::CreateAnimation()
{
	return new ui3d::Animation;
}

IUI3DMesh* ui3d_core::CreateMesh()
{
	return new ui3d::Mesh;
}

int ui3d_core::AddForm(const char* file, const PERSISTID & pid)
{
	Form* pForm = (Form*)g_pCore->GetEntity(pid);
	return m_renderui->LoadForm(file, pForm);
}

int ui3d_core::AddForm(const char* file)
{
	Gui* pGui = (Gui*)g_pCore->LookupEntity("Gui");
	Designer* pDesigner = (Designer*)g_pCore->GetEntity(pGui->GetDesigner());
	PERSISTID form_id = pDesigner->LoadForm(g_pCore->GetWorkPath(),file);
	return AddForm(file, form_id);
}

void ui3d_core::RemoveForm(const char* file)
{
	m_renderui->RemoveForm(file);
}

ITexture* ui3d_core::GetFormTexture(int i)
{
	return m_renderui->GetRenderTexture(i);
}

const PERSISTID& ui3d_core::GetForm(int i) const
{
	return m_renderui->GetUI(i);
};

size_t ui3d_core::GetFormCount()const
{
	return m_renderui->GetCount();
}