#include "ui3d.h"
#include "ui3d_helper.h"
#include "../visual/i_render.h"

#define _USE_TO_UI
extern IRender* g_render;
UI3D::~UI3D()
{
	delete(m_mesh);
	delete(m_animation);
	delete(m_animationback);
}

bool UI3D::Init(const IVarList& args)
{
	
	const char* forms[] = {
		"res/skin/form_stage_login/form_login.xml",
		"res/skin/form_common/form_loading.xml",
		"res/skin/form_common/form_before_loading.xml",
		"res/skin/form_stage_create/form_create_role_mobile.xml",
		"res/skin/form_stage_main/form_bag/form_bag.xml",
		"res/skin/form_stage_roles/form_roles_mobile.xml",
		0
	};

	const char* tex[] = {
		"tex/01.png",
		"tex/02.png",
		"tex/03.png",
		"tex/04.png",
		"tex/05.png",
		"tex/06.png"
	};

	

 //	m_renderui.Create();
	////for(int i = 0; forms[i] != 0; i++)
	//{
	////	m_renderui.LoadUI(forms[i]);
	//}
	
	g_render = (IRender*)g_pCore->GetInterface("Render");
	m_ui3d_core = (IUI3DCore*)g_pCore->GetInterface("ui3d_core");

	for(int i = 0; forms[i] != 0; i++)
	{
		m_ui3d_core->AddForm(forms[i]);
	}

	m_mesh = m_ui3d_core->CreateMesh();
	m_animation = m_ui3d_core->CreateAnimation();
	m_animationback = m_ui3d_core->CreateAnimation();
int num = 6;

#ifdef _USE_TO_UI
	BuildCylinder(m_mesh, 6, 1.0f, 0.75, FVF_POS|FVF_TEX0, 1.0, 1.0f);
#else
	BuildCylinder(m_mesh, num, 0.71f, 1.0f, FVF_POS|FVF_TEX0, 0.927734375, -0.64453125);
#endif
	for(int i = 0; i < num; i++)
	{
#ifdef _USE_TO_UI
		ITexture* pTexture = m_ui3d_core->GetFormTexture(i);
#else
		ITexture* pTexture = g_render->CreateTexture(tex[i], true);
		
#endif
		m_mesh->GetNode(i)->SetTex0(pTexture);
	}

	g_pCore->AddMsgProc(this, WM_LBUTTONDOWN);
	g_pCore->AddMsgProc(this, WM_RBUTTONDOWN);
	g_pCore->AddMsgProc(this, WM_KEYDOWN);
	
	g_pCore->AddMsgProc(this, WM_MOUSEMOVE);
	g_pCore->AddMsgProc(this, WM_LBUTTONUP);

	g_pCore->AddExecute(this);

	m_animation->Create(30, 0.2);
	m_animation->CreateFrameGroup("1");
	m_animation->CreateFrameGroup("2");
	m_animation->CreateFrameGroup("3");

	m_animationback->Create(30, 0.2);
	m_animationback->CreateFrameGroup("1");
	m_animationback->CreateFrameGroup("1");
	m_animationback->CreateFrameGroup("3");
	
//	m_animation.AddAction(new ui3d::RotateAction(this, FmVec3(0.0, 3.14*2.0/6.0, 0.0)), 2);
	FmVec3 s(0.1f,0.1f,0.1f);
	float offsetx = -1.0f;

	for(int i = 0; i < 1; i++)
	{
		for(int i = 0; i< num ; i++)
		{
			FmVec3 pos = m_mesh->GetNode(i)->GetPosition();
			FmVec3 angle = m_mesh->GetNode(i)->GetAngle();
			FmVec3 scale = m_mesh->GetNode(i)->GetScale();

			//m_animation.AddAction(new ui3d::RotateAction(m_mesh.GetNode(i), FmVec3(0, 3.1415926*2/6, 0.0)), 1, 0, "2");
			m_animation->AddAction(new PositionAction(m_mesh->GetNode(i), FmVec3(offsetx,0,0), false), 1, 0, "1");
			m_animation->AddAction(new RotateAction(m_mesh->GetNode(i), FmVec3(0,0,0), false), 1, 0, "1");
			m_animation->AddAction(new ScaleAction(m_mesh->GetNode(i), FmVec3(0.3,0.3,0.3), false), 2, 0, "3");

			m_animationback->AddAction(new PositionAction(m_mesh->GetNode(i), pos, false), 1, 0, "1");
			m_animationback->AddAction(new RotateAction(m_mesh->GetNode(i), angle, false), 1, 0, "1");
			m_animationback->AddAction(new ScaleAction(m_mesh->GetNode(i), scale, false), 2, 0, "3");
			offsetx += 0.4;
		}


	//	m_animation.AddAction(new ui3d::ScaleAction(this, -s), 1, 0, "1");
	}
	
	
	return true; 
}

bool UI3D::Shut()
{
	return true;
}

void UI3D::Realize()
{
	m_mesh->Draw();
}

bool UI3D::MsgProc(unsigned int msg, size_t param1, size_t param2, 
		int& result)
{
	switch(msg)
	{
	case WM_RBUTTONDOWN:
		m_animation->Play();
		break;
	case WM_LBUTTONDOWN:
		{
			m_animationback->Play();
			long x = LOWORD(param2);
			long y = HIWORD(param2);
			FmVec2 uv;
			int node_index;
			if(m_mesh->Pick(GetContext(), x, y, uv, node_index) && m_ui3d_core->GetFormCount() > node_index)
			{
				const PERSISTID& pid = m_ui3d_core->GetForm(node_index);
				IControl* pControl = (IControl*)g_pCore->GetEntity(pid);
				int gui_x = uv.x * pControl->GetWidth();
				int gui_y = (1-uv.y) * pControl->GetHeight();
				
				Gui* pGui = (Gui*)g_pCore->LookupEntity("Gui");
				
			
				pGui->MsgLButtonDownToControl(pControl, param1, MAKELONG(gui_x, gui_y), result);
			}
		}
		break;
	case WM_LBUTTONUP:
		{
			long x = LOWORD(param2);
			long y = HIWORD(param2);
			FmVec2 uv;
			int node_index;
			if(m_mesh->Pick(GetContext(), x, y, uv, node_index) && m_ui3d_core->GetFormCount() > node_index)
			{
				IControl* pControl = (IControl*)g_pCore->GetEntity(m_ui3d_core->GetForm(node_index));
				int gui_x = uv.x * pControl->GetWidth();
				int gui_y = (1-uv.y) * pControl->GetHeight();
				
				Gui* pGui = (Gui*)g_pCore->LookupEntity("Gui");
				
			
				pGui->MsgLButtonUpToControl(pControl, param1, MAKELONG(gui_x, gui_y), result);
			}
		}
		break;
	case WM_MOUSEMOVE:
		{
			long x = LOWORD(param2);
			long y = HIWORD(param2);
			FmVec2 uv;
			int node_index;
			int count = m_ui3d_core->GetFormCount();
			if(m_mesh->Pick(GetContext(), x, y, uv, node_index) && count > node_index)
			{
				if(m_ui3d_core->GetFormCount() > node_index)
				{
					IControl* pControl = (IControl*)g_pCore->GetEntity(m_ui3d_core->GetForm(node_index));
					int gui_x = uv.x * pControl->GetWidth();
					int gui_y = (1-uv.y) * pControl->GetHeight();
					Gui* pGui = (Gui*)g_pCore->LookupEntity("Gui");
					pGui->MsgMouseMoveToControl(pControl, param1, MAKELONG(gui_x, gui_y), result);
				}
			}
		}
		break;
	case WM_KEYDOWN:
		{
			for(size_t i = 0; i < m_ui3d_core->GetFormCount(); i++)
			{
				IControl* pControl = (IControl*)g_pCore->GetEntity(m_ui3d_core->GetForm(i));
				Gui* pGui = (Gui*)g_pCore->LookupEntity("Gui");
				if(pGui->MsgKeyDownToControl(pControl, param1, param2, result))
					break;
			}
		}
		break;
	}
	
	return false;
}

void UI3D::Execute(float seconds)
{
	m_animation->Update(seconds);
	m_animationback->Update(seconds);
}

DECLARE_ENTITY(UI3D, 0, IVisBase);


