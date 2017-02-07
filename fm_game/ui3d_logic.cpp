#include "ui3d_logic.h"

#include"../fm_gui/gui_input.h"

DECLARE_ENTITY(UI3DLogic, 0, IEntity)

UI3DLogic::UI3DLogic()
{
	m_mousefocus = -1;
}

bool UI3DLogic::Init(const IVarList& args)
{
	g_core->AddMsgProc(this, WM_LBUTTONDOWN);
	g_core->AddMsgProc(this, WM_LBUTTONUP);
	g_core->AddMsgProc(this, WM_MOUSEMOVE);

	IVisBase* pEntity =(IVisBase*) g_core->CreateEntity("UI3DX");
	pEntity->SetContext(g_scene->GetContext());
	pEntity->SetPosition(0,0,5);

	if(!g_scene->AddObject(pEntity->GetID(), 0xffffff))
	{
		Assert(0);
	}

	m_ui3d = (UI3DX*)pEntity;

	const char * default_textures[] = 
	{
		"gui/ui3d/01.png",
		"gui/ui3d/02.png",
		"gui/ui3d/03.png",
		"gui/ui3d/04.png",
		"gui/ui3d/05.png",
		"gui/ui3d/06.png",
		"gui/ui3d/07.png",
	};

	//参数分别是面数，面宽，面高，贴图例表
	m_ui3d->Build(6, 0.75, 1, default_textures);
	//这里可以控制位置,
	m_ui3d->SetPosition(0, 0, 1);

	m_ui3d->SetScale(0.5, 0.5, 0.5);

	g_core->AddExecute(this);

	ICamera* pCamera = (ICamera*)g_pCore->GetEntity(g_scene->GetCameraID());
	pCamera->SetPosition(0, 0, 0);
	pCamera->SetAngle(0,0,0);

	return true;
}

bool UI3DLogic::Shut()
{
	return true;
}

// 窗口消息处理
bool UI3DLogic::MsgProc(unsigned int msg, size_t param1, size_t param2, 
			 int& result) 
{

	int x = LOWORD(param2);
	int y = HIWORD(param2);

	switch(msg)
	{
	case WM_MOUSEMOVE:
		TouchMove(x, y);
		break;
	case WM_LBUTTONDOWN:
		TouchDown(x, y);
		break;
	case WM_LBUTTONUP:
		TouchUp(x, y);
		break;
	}

	return false; 
}

void UI3DLogic::OnTouchUp(int panel, int x, int y)
{
	char buf[256];
	sprintf(buf, "OnTouchUp %d, uv:%f, %f", panel, m_uv.x, m_uv.y);
	CORE_TRACE(buf);
}

void UI3DLogic::OnTouchDown(int panel, int x, int y)
{
	char buf[256];
	sprintf(buf, "OnTouchDown %d, uv:%f, %f", panel, m_uv.x, m_uv.y);
	CORE_TRACE(buf);
}

//离开页面时
void UI3DLogic::OnTouchOut(int panel, int x, int y)
{
	//鼠标焦点离开
	m_ui3d->SetAlpha(panel, 0.70f);
	char buf[256];
	sprintf(buf, "OnTouchOut %d, %d, %d", panel, x, y);
	CORE_TRACE(buf);
}

//触摸到页面时
void UI3DLogic::OnTouchIn(int panel, int x, int y)
{
	//鼠标焦点离开
	m_ui3d->SetAlpha(panel, 1.0f);
	char buf[256];
	sprintf(buf, "OnTouchIn %d, %d, %d", panel, x, y);
	CORE_TRACE(buf);
}

void UI3DLogic::TouchMove(int x, int y)
{
	int panel;

	if(!m_ui3d->Pick(x, y, panel, m_uv))
	{
		panel = -1;
	}

	if(panel != m_mousefocus)
	{
		if(m_mousefocus != -1)
		{
			OnTouchOut(m_mousefocus, x, y);
		}

		m_mousefocus = panel;

		if(m_mousefocus != -1)
		{
			OnTouchIn(m_mousefocus, x, y);
		}
	}
}

void UI3DLogic::TouchDown(int x, int y)
{
	if(m_mousefocus > -1)
	{
		OnTouchDown(m_mousefocus, x, y);
	}
}

void UI3DLogic::TouchUp(int x, int y)
{
	if(m_mousefocus > -1)
	{
		OnTouchUp(m_mousefocus, x, y);
	}
}

void UI3DLogic::Execute(float seconds)
{
	//RotateUI3D(seconds);
}

void UI3DLogic::RotateUI3D(float seconds)
{
	FmVec3 angle = m_ui3d->GetAngle();
	m_ui3d->SetAngle(angle.x, angle.y + seconds, angle.z);
}

void UI3DLogic::SetVisible(bool value)
{
	m_ui3d->SetVisible(value);
}

bool UI3DLogic::GetVisible(void)
{
	return m_ui3d->GetVisible();
}