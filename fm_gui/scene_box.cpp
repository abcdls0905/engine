//--------------------------------------------------------------------
// 文件名:		SceneBox.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月31日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "../visual/i_vis_base.h"
#include "../visual/i_painter.h"
#include "../visual/i_scene.h"
#include "scene_box.h"
/// \file SceneBox.cpp
/// \brief 嵌入3D场景控件

/// entity: SceneBox
/// \brief 嵌入3D场景控件实体
/// \see 继承自IControl
DECLARE_ENTITY(SceneBox, 0, IControl);

/// readonly: int OldX
/// \brief 上一次的鼠标位置X
DECLARE_PROPERTY_GET(int, SceneBox, OldX, GetOldX);
/// readonly: int OldY
/// \brief 上一次的鼠标位置Y
DECLARE_PROPERTY_GET(int, SceneBox, OldY, GetOldY);

/// property: object Scene
/// \brief 3D场景对象
DECLARE_PROPERTY(PERSISTID, SceneBox, Scene, GetSceneID, SetSceneID);
///// property: bool Transparent
///// \brief 是否透明不响应消息
//DECLARE_PROPERTY(bool, SceneBox, Transparent, GetTransparent, SetTransparent);

/// event: int on_rightclick_down(object self, x, y)
/// \brief 鼠标右键按下时触发
/// \param self本控件
DECLARE_EVENT(on_rightclick_down);
/// event: int on_rightclick_up(object self, x, y)
/// \brief 鼠标右键弹起时触发
/// \param self本控件
DECLARE_EVENT(on_rightclick_up);

// SceneBox

SceneBox::SceneBox()
{
	m_nOldX = 0;
	m_nOldY = 0;
	
	SetDesignSize(100, 80);
	SetCanFocus(true);
	SetTransparentDefault(false);
}

SceneBox::~SceneBox()
{
}

bool SceneBox::Init(const IVarList& args)
{
	if (!IControl::Init(args))
	{
		return false;
	}
	
	return true;
}

bool SceneBox::Shut()
{
	// 删除嵌入的场景
	GetCore()->DeleteEntity(m_SceneID);
	
	return IControl::Shut();
}

void SceneBox::PrepareDesignInfo(IDesignInfo* di)
{
	IControl::PrepareDesignInfo(di);

	di->RemovePropInfo("Text");
	di->RemovePropInfo("Font");
  //  di->AddPropInfo("Transparent", IDesignInfo::TYPE_BOOL, true, true, 
		//&CVar(IDesignInfo::TYPE_BOOL, false));

	di->AddEventInfo("on_rightclick_down");
	di->AddEventInfo("on_rightclick_up");
}

void SceneBox::Paint(float seconds)
{
	if (!GetVisible())
	{
		return;
	}
	
    RefreshInnerAbsPos();

	int x1 = InnerGetAbsLeft();
	int y1 = InnerGetAbsTop();
	int x2 = x1 + GetWidth();
	int y2 = y1 + GetHeight();

	if (HasBackImage())
	{
		DrawBackImage(seconds, x1, y1, x2, y2);
	}
	else
	{
		GetPainter()->FillRect(x1, y1, x2, y2, GetBackColor());
	}

	IScene* pScene = (IScene*)GetCore()->GetEntity(m_SceneID);

	if (pScene)
	{
		//int old_color = pScene->GetColor();
		int ui_alpha = GetPainter()->GetBlendAlpha();

		//pScene->SetColor((ui_alpha << 24) | (pScene->GetColor() & 0x00ffffff));

		if (GetPainter()->SetViewport(x1, y1, x2, y2))
		{
			GetPainter()->DrawScene(pScene);
			GetPainter()->ResetViewport();
		}

		//pScene->SetColor(old_color);
	}
	else
	{
		if (GetPainter()->SetViewport(x1, y1, x2, y2))
		{
			GetPainter()->DrawScene(pScene);
			GetPainter()->ResetViewport();
		}
	}

	if (!GetNoFrame())
	{
		GetPainter()->DrawRect(x1, y1, x2, y2, GetLineColor());
	}
}

void SceneBox::SetSceneID(const PERSISTID& id)
{
	IScene* pVisBase = (IScene*)GetCore()->GetEntity(id);

	if (NULL == pVisBase)
	{
		return;
	}

	if (!pVisBase->GetEntInfo()->IsKindOf("Scene"))
	{
		return;
	}

	if (!m_SceneID.IsNull())
	{
		GetGui()->RemoveScene(m_SceneID);
	}

	m_SceneID = id;

	// 添加到预计算（Prepare）队列
	GetGui()->AddScene(id);
}

PERSISTID SceneBox::GetSceneID() const
{
	return m_SceneID;
}

//void SceneBox::SetTransparent(bool bTrans)
//{
//    m_bTransparent = bTrans;
//}
//
//bool SceneBox::GetTransparent()
//{
//    return m_bTransparent;
//}

bool SceneBox::TestTransparent(int x, int y)
{
	if (GetTransparent())
	{
		// 完全透明
		return true;
	}

	return IControl::TestTransparent(x, y);
}

int SceneBox::GetOldX() const
{
	return m_nOldX;
}

int SceneBox::GetOldY() const
{
	return m_nOldY;
}

int SceneBox::GetRealDeviceX(int x)
{
	int device_width = GetPainter()->GetDeviceWidth();
	int render_width = GetPainter()->GetRenderWidth();

	if (device_width != render_width)
	{
		float scale_x = float(device_width) / float(render_width);
		float x1 = float(x) * scale_x;

		return int(x1);
	}
	
	return x;
}

int SceneBox::GetRealDeviceY(int y)
{
	int device_height = GetPainter()->GetDeviceHeight();
	int render_height = GetPainter()->GetRenderHeight();

	if (device_height != render_height)
	{
		float scale_y = float(device_height) / float(render_height);
		float y1 = float(y) * scale_y;

		return int(y1);
	}

	return y;
}

int SceneBox::OnLButtonDown(int x, int y)
{ 
	if (!GetEnabled())
	{
		return 0;
	}
	
	int click_x = GetRealDeviceX(x) - GetRealDeviceX(GetAbsLeft());
	int click_y = GetRealDeviceY(y) - GetRealDeviceY(GetAbsTop());

	GuiUtil_RunCallback(this, "on_leftclick_down", 
		CVarList() << click_x << click_y);

	GuiUtil_RunCallback(this, "on_click", 
		CVarList() << click_x << click_y);

	m_nOldX = click_x;
	m_nOldY = click_y;

	return 1; 
}

int SceneBox::OnLButtonUp(int x, int y)
{
	if (!GetEnabled())
	{
		return 0;
	}

	int click_x = GetRealDeviceX(x) - GetRealDeviceX(GetAbsLeft());
	int click_y = GetRealDeviceY(y) - GetRealDeviceY(GetAbsTop());

	GuiUtil_RunCallback(this, "on_leftclick_up", 
		CVarList() << click_x << click_y);

	m_nOldX = click_x;
	m_nOldY = click_y;

	return 1; 
}

int SceneBox::OnLButtonDoubleClick(int x, int y)
{
	if (!GetEnabled())
	{
		return 0;
	}
	
	int click_x = GetRealDeviceX(x) - GetRealDeviceX(GetAbsLeft());
	int click_y = GetRealDeviceY(y) - GetRealDeviceY(GetAbsTop());

	GuiUtil_RunCallback(this, "on_double_click", 
		CVarList() << click_x << click_y << CGuiConst::MOUSE_LEFT);
	
	return 1; 
}

int SceneBox::OnRButtonDown(int x, int y)
{ 
	if (!GetEnabled())
	{
		return 0;
	}

	int click_x = GetRealDeviceX(x) - GetRealDeviceX(GetAbsLeft());
	int click_y = GetRealDeviceY(y) - GetRealDeviceY(GetAbsTop());

	GuiUtil_RunCallback(this, "on_rightclick_down", 
		CVarList() << click_x << click_y);

	m_nOldX = click_x;
	m_nOldY = click_y;

	return 1; 
}

int SceneBox::OnRButtonUp(int x, int y)
{ 
	if (!GetEnabled())
	{
		return 0;
	}

	IControl::OnRButtonUp(x, y);

	int click_x = x - InnerGetAbsLeft();
	int click_y = y - InnerGetAbsTop();


	Assert((click_x >= 0) && (click_x < GetWidth()));
	Assert((click_y >= 0) && (click_y < GetHeight()));

	GuiUtil_RunCallback(this, "on_rightclick_up", 
		CVarList() << click_x << click_y);

	return 1;
}

int SceneBox::OnDragMove(int x, int y)
{
	int click_x = GetRealDeviceX(x) - GetRealDeviceX(GetAbsLeft());
	int click_y = GetRealDeviceY(y) - GetRealDeviceY(GetAbsTop());
	
	if (click_x < 0)
	{
		return 0;
	}

	int width = GetRealDeviceX(GetWidth());

	if (click_x >= width)
	{
		return 0;
	}

	if (click_y < 0)
	{
		return 0;
	}

	int height = GetRealDeviceY(GetHeight());

	if (click_y >= height)
	{
		return 0;
	}

	GuiUtil_RunCallback(this, "on_drag_move", 
		CVarList() << click_x << click_y << CGuiConst::MOUSE_LEFT);
	
	m_nOldX = click_x;
	m_nOldY = click_y;
	
	return 1;
}

int SceneBox::OnDragLeave(int x, int y) 
{ 
	int click_x = GetRealDeviceX(x) - GetRealDeviceX(GetAbsLeft());
	int click_y = GetRealDeviceY(y) - GetRealDeviceY(GetAbsTop());
	
	GuiUtil_RunCallback(this, "on_drag_leave", 
		CVarList() << click_x << click_y << CGuiConst::MOUSE_LEFT);
	
	m_nOldX = click_x;
	m_nOldY = click_y;
	
	return 1;
}

int SceneBox::OnMouseMove(int x, int y)
{
	int click_x = GetRealDeviceX(x) - GetRealDeviceX(GetAbsLeft());
	int click_y = GetRealDeviceY(y) - GetRealDeviceY(GetAbsTop());

	if ((m_nOldX == click_x) && (m_nOldY == click_y))
	{
		return 0;
	}
	
	int res = GuiUtil_RunCallback(this, "on_mouse_move",
		CVarList() << click_x << click_y);
	
	m_nOldX = click_x;
	m_nOldY = click_y;
	
	return res;
}

