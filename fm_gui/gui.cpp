//--------------------------------------------------------------------
// 文件名:		gui.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "gui.h"
#include "gui_input.h"
#include "i_control.h"
#include "label.h"
#include "form.h"
#include "font.h"
#include "caret.h"
#include "cursor.h"
#include "designer.h"
#include "design_info.h"
#include "../visual/i_gui_hook.h"
#include "../public/auto_mem.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/inlines.h"
#include "../public/converts.h"
#include "../public/portable.h"
#include "text.h"
#include "../visual/i_scene.h"

/// \file gui.cpp
/// \brief 界面

/// entity: Gui
/// \brief 界面实体
DECLARE_ENTITY(Gui, 0, IEntity);

/// readonly: string PainterName
/// \brief 界面渲染器名称
DECLARE_PROPERTY_GET(const char*, Gui, PainterName, GetPainterName);
/// readonly: int Width
/// \brief 主窗口宽度
DECLARE_PROPERTY_GET(int, Gui, Width, GetWidth);
/// readonly: int Height
/// \brief 主窗口高度
DECLARE_PROPERTY_GET(int, Gui, Height, GetHeight);
/// readonly: int DeviceWidth
/// \brief 主窗口宽度
DECLARE_PROPERTY_GET(int, Gui, DeviceWidth, GetDeviceWidth);
/// readonly: int DeviceHeight
/// \brief 主窗口高度
DECLARE_PROPERTY_GET(int, Gui, DeviceHeight, GetDeviceHeight);
/// readonly: object Desktop
/// \brief 主窗口表单对象
DECLARE_PROPERTY_GET(PERSISTID, Gui, Desktop, GetDesktopID);
/// readonly: object ModalForm
/// \brief 当前模式窗口对象
DECLARE_PROPERTY_GET(PERSISTID, Gui, ModalForm, GetModalFormID);
/// readonly: object Loader
/// \brief 资源加载器对象
DECLARE_PROPERTY_GET(PERSISTID, Gui, Loader, GetLoader);
/// readonly: object Designer
/// \brief 资源编辑器对象
DECLARE_PROPERTY_GET(PERSISTID, Gui, Designer, GetDesigner);
/// readonly: float FPS
/// \brief 帧数
DECLARE_PROPERTY_GET(float, Gui, FPS, GetFPS);

/// readonly: float AverageFPS
/// \brief 平均帧数
DECLARE_PROPERTY_GET(float, Gui, AverageFPS, GetAverageFPS);

/// readonly: object Capture
/// \brief 当前捕获鼠标的控件
DECLARE_PROPERTY_GET(PERSISTID, Gui, Capture, GetCaptureID);

/// property: string DefaultPath
/// \brief 缺省资源路径
DECLARE_PROPERTY(const char*, Gui, DefaultPath, GetDefaultPath, SetDefaultPath);
/// property: bool Visible
/// \brief 是否显示控件
DECLARE_PROPERTY(bool, Gui, Visible, GetVisible, SetVisible);
/// property: bool ShowFPS
/// \brief 是否显示帧数
DECLARE_PROPERTY(bool, Gui, ShowFPS, GetShowFPS, SetShowFPS);
/// property: bool ScaleEnable
/// \brief 是否开启界面缩放
DECLARE_PROPERTY(bool, Gui, ScaleEnable, GetScaleEnable, SetScaleEnable);

/// property: bool WordChangeLine
/// \brief 是否整词换行处理
DECLARE_PROPERTY(bool, Gui, WordChangeLine, GetWordChangeLine, 
SetWordChangeLine);
/// property: float ScaleRatio
/// \brief 界面放缩比率（0.5到2.0）
DECLARE_PROPERTY(float, Gui, ScaleRatio, GetScaleRatio, SetScaleRatio);
/// property: bool ShowRenderInfo
/// \brief 是否显示渲染器信息
DECLARE_PROPERTY(bool, Gui, ShowRenderInfo, GetShowRenderInfo, 
SetShowRenderInfo);

/// property: float HintDelay
/// \brief 鼠标停止多少秒后显示浮动提示信息
DECLARE_PROPERTY(float, Gui, HintDelay, GetHintDelay, SetHintDelay);
/// property: object Focused
/// \brief 当前输入焦点对象
DECLARE_PROPERTY(PERSISTID, Gui, Focused, GetFocused, SetFocused);

/// property: string Cursor
/// \brief 鼠标外观资源名
DECLARE_PROPERTY(const char*, Gui, Cursor, GetCursorName, SetCursorName);

/// property: bool AbandomMouseMove
/// \brief 忽略鼠标移动消息
DECLARE_PROPERTY(bool, Gui, AbandomMouseMove, GetAbandomMouseMove, SetAbandomMouseMove);

/// method: bool HasKeyMsg()
/// \brief 输出所有保存界面图片的贴图
DECLARE_PROPERTY_GET(bool, Gui, GuiHasKeyMsg, GuiHasKeyMsg);

/// event: int on_open(object self)
/// \brief 主窗口打开时的回调
/// \param self 本控件
DECLARE_EVENT(on_open);
/// event: int on_close(object self)
/// \brief 主窗口关闭时的回调
/// \param self 本控件
/// \return 返回0表示取消关闭
DECLARE_EVENT(on_close);
/// event: int on_size(object self, int width, int height)
/// \brief 主窗口尺寸改变时的回调
/// \param width 新的宽度
/// \param height 新的高度
DECLARE_EVENT(on_size);
/// event: int on_func_key(object self, string key)
/// \brief 按下调试功能键时的回调（Ctrl+F1-F12）
/// \param self 本控件
/// \param key 键名（"F1"-"F12"）
DECLARE_EVENT(on_func_key);
/// event: int on_design_enter(object self, object control)
/// \brief: 设计状态下选择某控件时的回调
/// \param self 本控件
/// \param control 被选择控件
DECLARE_EVENT(on_design_enter);
/// event: int on_design_click(object self, object control, int x, int y)
/// \brief: 设计状态下点击被选择控件时的回调
/// \param self 本控件
/// \param control 被选择控件
/// \param x X坐标
/// \param y Y坐标
DECLARE_EVENT(on_design_click);
/// event: int on_active(object self)
/// \brief: 游戏被击活
DECLARE_EVENT(on_active);
/// event: int on_unactive(object self)
/// \brief: 游戏被最小化
DECLARE_EVENT(on_unactive);

/// event: int on_whatever_key_down(object self, key, shift, ctrl)
/// \brief: 按键回调，无视输入和焦点处理,都会回调,一般在此处理一些特殊按键，
DECLARE_EVENT(on_whatever_key_down);

/// event: int on_show_hint(object self, wstring hint_text, string hint_type, int x, int y)
/// \brief 显示浮动提示时的回调
/// \param self 本控件
/// \param hint_text 浮动提示的文本
/// \param hint_type 浮动提示的类型
/// \param x 窗口内位置x
/// \param y 窗口内位置y
DECLARE_EVENT(on_show_hint);

/// event: int on_hide_hint(object self, string hint_type, int x, int y)
/// \brief 显示浮动提示时的回调
/// \param self 本控件
/// \param hint_type 浮动提示的类型
/// \param x 窗口内位置x
/// \param y 窗口内位置y
DECLARE_EVENT(on_hide_hint);


#include "../public/core_file.h"
//#include "../utils/fm_mem_log.h"
#include "../utils/fm_log.h"

FILE* Gui::s_fp_GuiLogPerf = NULL;

void Gui::WorkerProc(void* pParameter)
{
	Gui* pthis = (Gui*)pParameter;
	char* plog = fm_logread();
	if(plog == NULL)
	{
		pthis->m_pThreadWaiter->Wait(10);
		return ;
	}

	if(s_fp_GuiLogPerf)
	{
		//CORE_TRACE(plog);
		core_file::fwrite(plog,sizeof(performance_info_t),1,s_fp_GuiLogPerf);
	}
}


// Gui

Gui::Gui()
{
	m_CurCP = CP_ACP;
	m_pPainter = NULL;
	m_pGuiHook = NULL;
	m_pDesktop = NULL;
	m_pLoader = NULL;
	m_pDesigner = NULL;

	m_nTickCount = Port_GetTickCount();
	m_nFrameCount = 0;
	m_bAbandomMouseMove = false;
	m_fTimeCount = 0.0F;
	m_fFPS = 0.0F;
	m_nCountFPS = 0;
	m_fAverageFPS = 0.0F;

	m_bVisible = true;
	m_bShowFPS = true;
	m_bShowRenderInfo = false;
	m_bWordChangeLine = false;
	m_fHintDelay = 0.8F;
	m_fHintCount = 0.0F;

	m_bFocusCanInput = false;
	//m_bLastInputOpenState = false;
	m_Scenes.clear();

	m_pThread = NULL;
	m_pThreadWaiter = NULL;
	m_bLogPerf = false;

	m_dEffectModelDrawCallMaxNumPreFrame = 0.0;
	m_dEffectParticleDrawCallMaxNumPreFrame = 0.0;
	m_dGuiDrawCallMaxNumPreFrame = 0.0;
	m_dMaxValTimeCount = 0.0f;
}

Gui::~Gui()
{
	ClearSound();

	des_info_container_t::iterator it = m_DesInfo.Begin();
	des_info_container_t::iterator end = m_DesInfo.End();

	for (; it != end; ++it)
	{
		CORE_DELETE((CDesignInfo*)it.GetData());
	}

	SAFE_RELEASE(m_pGuiHook);
}
//#define WM_MOUSEWHEEL                   0x020A
bool Gui::Init(const IVarList& args)
{
	// 创建时可以传入渲染器的名称
	m_strPainterName = args.StringVal(0);

	if (m_strPainterName.empty())
	{
		// 否则用缺省的渲染器名
		m_strPainterName = "Painter";
	}

	// 只允许存在一个实例
	IEntity* pEntity = GetCore()->LookupEntity("Gui");

	if (pEntity != this)
	{
		CORE_TRACE("(Gui::Init)gui already exists");
		return false;
	}

	m_pPainter = (IPainter*)GetCore()->GetInterface(GetPainterName());

	if (NULL == m_pPainter)
	{
		CORE_TRACE("(Gui::Init)no painter");
		CORE_TRACE(GetPainterName());
		return false;
	}

	// 超过的字体不参于共享处理

	// 添加消息处理
	/*
	GetCore()->AddMsgProcInstant(this, WM_SETCURSOR);
	GetCore()->AddMsgProc(this, WM_SYSKEYDOWN);
	GetCore()->AddMsgProc(this, WM_KEYDOWN);
	GetCore()->AddMsgProc(this, WM_KEYUP);
	GetCore()->AddMsgProc(this, WM_CHAR);
	GetCore()->AddMsgProc(this, WM_INPUTLANGCHANGE);
	GetCore()->AddMsgProcInstant(this, WM_IME_COMPOSITION);
	GetCore()->AddMsgProc(this, WM_KILLFOCUS);
	GetCore()->AddMsgProc(this, WM_LBUTTONDOWN);
	GetCore()->AddMsgProc(this, WM_LBUTTONUP);
	GetCore()->AddMsgProc(this, WM_LBUTTONDBLCLK);
	GetCore()->AddMsgProc(this, WM_RBUTTONDOWN);
	GetCore()->AddMsgProc(this, WM_RBUTTONUP);
	GetCore()->AddMsgProc(this, WM_RBUTTONDBLCLK);
	GetCore()->AddMsgProc(this, WM_MOUSEMOVE);
	GetCore()->AddMsgProc(this, WM_MOUSEWHEEL);
	GetCore()->AddMsgProc(this, WM_MOUSEHOVER);
	GetCore()->AddMsgProc(this, WM_SIZE);
	GetCore()->AddMsgProc(this, WM_SIZING);
	GetCore()->AddMsgProc(this, WM_MOVE);
	GetCore()->AddMsgProc(this, WM_MOVING);
	GetCore()->AddMsgProc(this, WM_CLOSE);
	GetCore()->AddMsgProc(this, WM_ACTIVATE);
	GetCore()->AddMsgProc(this, WM_IME_NOTIFY);
	GetCore()->AddMsgProc(this, WM_IME_SETCONTEXT);*/

	//鼠标相关
	GetCore()->AddMsgProc(this, WM_LBUTTONDOWN);
	GetCore()->AddMsgProc(this, WM_LBUTTONUP);
	GetCore()->AddMsgProc(this, WM_LBUTTONDBLCLK);
	GetCore()->AddMsgProc(this, WM_RBUTTONDOWN);
	GetCore()->AddMsgProc(this, WM_RBUTTONUP);
	GetCore()->AddMsgProc(this, WM_RBUTTONDBLCLK);
	GetCore()->AddMsgProc(this, WM_MOUSEMOVE);
	//	GetCore()->AddMsgProc(this, WM_MOUSEWHEEL);
	//GetCore()->AddMsgProc(this, WM_MOUSEHOVER);

	//输入相关
	GetCore()->AddMsgProc(this, WM_KEYDOWN);
	GetCore()->AddMsgProc(this, WM_KEYUP);
	GetCore()->AddMsgProc(this, WM_CHAR);


	// 窗口加载器
	m_pLoader = (Designer*)GetCore()->CreateEntity("Designer");

	if (m_pLoader != NULL)
	{
		m_pLoader->SetGui(this);
		m_pLoader->SetPainter(m_pPainter);
		m_pLoader->SetDesignMode(false);
	}

	// 创建系统光标
	InnerCreateCursor("Default", "IDC_ARROW");
	InnerCreateCursor("WIN_IBEAM", "IDC_IBEAM");
	InnerCreateCursor("WIN_SIZEALL", "IDC_SIZEALL");
	InnerCreateCursor("WIN_SIZENS", "IDC_SIZENS");
	InnerCreateCursor("WIN_SIZEWE", "IDC_SIZEWE");
	InnerCreateCursor("WIN_SIZENESW", "IDC_SIZENESW");
	InnerCreateCursor("WIN_SIZENWSE", "IDC_SIZENWSE");
	InnerCreateCursor("WIN_WAIT", "IDC_WAIT");
	InnerCreateCursor("WIN_HELP", "IDC_HELP");

	//// 创建缺省字体
	AddDefaultFont();

	// 创建缺省输入光标
	Caret* pCaret = (Caret*)GetCore()->CreateEntityArgs("Caret", 
		CVarList() << GetPainterName());

	pCaret->SetName("Default");
	pCaret->SetFixWidth(2);

	AddCaret(pCaret->GetID());

	// 创建缺省浮动提示框
	Label* pLabel = (Label*)GetCore()->CreateEntityArgs("Label", 
		CVarList() << GetPainterName() << GetID());

	pLabel->SetName("Default");
	pLabel->SetSolid(true);
	pLabel->SetNoFrame(false);
	pLabel->SetAutoSize(true);
	pLabel->SetBackColor(CGuiConst::DEF_HINT_COLOR);
	pLabel->SetAlignString("Center");

	AddHint(pLabel->GetID());

	// 创建文本管理器
	pEntity = GetCore()->CreateEntity("Text");
	if (pEntity)
	{
		m_TextManager = pEntity->GetID();
	}

	// 获取当前输入法IME设置
	//LoadCurIMESetting();

	// POINT cur_pos;

	//GetCursorPos(&cur_pos);
	//ScreenToClient((HWND)GetCore()->GetWindowHandle(), &cur_pos);

	//m_nMouseClientX = cur_pos.x;
	//m_nMouseClientY = cur_pos.y;


	//
	const IVar& varImgMrg = GetCore()->GetGlobalValue("ImageAnimationManager");
	if ( !GetCore()->GetEntity(varImgMrg.ObjectVal()) )
	{
		GetCore()->SetGlobalValue("ImageAnimationManager", 
			CVar(VTYPE_OBJECT, GetCore()->CreateEntity("ImageAnimationManager")->GetID()));
	}

	// 创建动画管理器
	pEntity = GetCore()->CreateEntityArgs("AnimationManger", CVarList() << GetID());
	if (pEntity)
	{
		m_AnimMngID = pEntity->GetID();
	}

	const IVar& varHypStylMrg = GetCore()->GetGlobalValue("HyperLinkStyleManager");
	if ( !GetCore()->GetEntity(varHypStylMrg.ObjectVal()) )
	{
		GetCore()->SetGlobalValue("HyperLinkStyleManager", 
			CVar(VTYPE_OBJECT, GetCore()->CreateEntity("HyperLinkStyleManager")->GetID()));
	}



	// 创建冷却管理器
	const IVar& varCoolMrg = GetCore()->GetGlobalValue("CoolManager");
	if ( !GetCore()->GetEntity(varCoolMrg.ObjectVal()) )
	{
		GetCore()->SetGlobalValue("CoolManager", 
			CVar(VTYPE_OBJECT, GetCore()->CreateEntity("CoolManager")->GetID()));
	}



	return true;
}

bool Gui::Shut()
{
	// 添加消息处理
	//	GetCore()->RemoveMsgProc(this, WM_SETCURSOR);
	//	GetCore()->RemoveMsgProc(this, WM_SYSKEYDOWN);
	//	GetCore()->RemoveMsgProc(this, WM_KEYDOWN);
	//	GetCore()->RemoveMsgProc(this, WM_KEYUP);
	//	GetCore()->RemoveMsgProc(this, WM_CHAR);
	//  GetCore()->RemoveMsgProc(this, WM_INPUTLANGCHANGE);
	//    GetCore()->RemoveMsgProc(this, WM_IME_COMPOSITION);
	//	GetCore()->RemoveMsgProc(this, WM_KILLFOCUS);
	//	GetCore()->RemoveMsgProc(this, WM_LBUTTONDOWN);
	//	GetCore()->RemoveMsgProc(this, WM_LBUTTONUP);
	//	GetCore()->RemoveMsgProc(this, WM_LBUTTONDBLCLK);
	//	GetCore()->RemoveMsgProc(this, WM_RBUTTONDOWN);
	//	GetCore()->RemoveMsgProc(this, WM_RBUTTONUP);
	//	GetCore()->RemoveMsgProc(this, WM_RBUTTONDBLCLK);
	//	GetCore()->RemoveMsgProc(this, WM_MOUSEMOVE);
	//	GetCore()->RemoveMsgProc(this, WM_MOUSEWHEEL);
	//	GetCore()->RemoveMsgProc(this, WM_MOUSEHOVER);
	//	GetCore()->RemoveMsgProc(this, WM_SIZE);
	//	GetCore()->RemoveMsgProc(this, WM_SIZING);
	//	GetCore()->RemoveMsgProc(this, WM_MOVE);
	//	GetCore()->RemoveMsgProc(this, WM_MOVING);
	//	GetCore()->RemoveMsgProc(this, WM_CLOSE);
	//	GetCore()->RemoveMsgProc(this, WM_ACTIVATE);
	//	GetCore()->RemoveMsgProc(this, WM_IME_NOTIFY);
	//	GetCore()->RemoveMsgProc(this, WM_IME_SETCONTEXT);

	ClearFont();
	ClearCursor();
	ClearCaret();
	ClearHint();
	ClearContext();

	//GetCore()->DeleteEntity(m_HyperLinkMngID);
	GetCore()->DeleteEntity(m_AnimMngID);
	//GetCore()->DeleteEntity(m_CoolManagerID);
	//GetCore()->DeleteEntity(m_GameHand);
	GetCore()->DeleteEntity(m_TextManager);

	SAFE_RELEASE(m_pDesktop);
	SAFE_RELEASE(m_pLoader);
	SAFE_RELEASE(m_pDesigner);
	SAFE_RELEASE(m_pPainter);

	return true;
}

Caret* Gui::InnerGetCaret() const
{
	return (Caret*)GetCore()->GetEntity(m_CaretID);
}

Cursor* Gui::InnerGetCursor() const
{
	return (Cursor*)GetCore()->GetEntity(m_CursorID);
}

//GameHand*Gui::InnerGetGameHand() const
//{
//	return (GameHand*)GetCore()->GetEntity(m_GameHand);
//}

bool Gui::InnerCreateCursor(const char* name, const char* image)
{
	Assert(name != NULL);
	Assert(image != NULL);

	Cursor* pCursor = (Cursor*)GetCore()->CreateEntityArgs(
		"Cursor", CVarList() << GetPainterName());

	if (NULL == pCursor)
	{
		return false;
	}

	pCursor->SetName(name);
	pCursor->SetImage(image);

	return AddCursor(pCursor->GetID());
}

void Gui::Update(float seconds)
{
	if (NULL == m_pPainter)
	{
		return;
	}

	// 刷新输入光标
	Caret* pCaret = InnerGetCaret();

	if (pCaret)
	{
		pCaret->Flash(seconds);
	}

	// 刷新鼠标（动画）
	Cursor* pCursor = InnerGetCursor();

	if (pCursor)
	{
		pCursor->Flash(seconds);
	}

	// 刷新浮动提示
	if (m_fHintDelay > 0.0F)
	{
		// 浮动提示计时
		m_fHintCount += seconds;

		if (m_fHintCount >= m_fHintDelay)
		{
			m_fHintCount = 0.0F;

			point_t pos;

			if (this->GetCursorCoord(pos))
			{
				// 显示浮动提示
				//OnShowHint(pos.x, pos.y);
			}
		}
	}
	else
	{
		point_t pos;

		if (this->GetCursorCoord(pos))
		{
			// 显示浮动提示
			//OnShowHint(pos.x, pos.y);
		}
	}

	//
	//AnimationManger* pAnimManager = InnerGetAnimationManger();
	//if (pAnimManager)
	//{
	//	pAnimManager->Update(seconds);
	//}

	// 更新动画图片
	//if (m_Images.GetCount() > 0)
	//{
	//	IMAGE_CONTAINER::iterator it = m_Images.Begin();
	//	IMAGE_CONTAINER::iterator end = m_Images.End();

	//	for (; it != end; ++it)
	//	{
	//		it.GetData()->ExecuteFlip(seconds);
	//	}
	//}


	//------------------ add by zhangbj -----------------------
	// 更新场景
	if (!m_Scenes.empty())
	{
		//std::vector<PERSISTID>::iterator it = m_Scenes.begin();
		size_t k = 0;

		//while (it != m_Scenes.end())
		while (k < m_Scenes.size())
		{
			//IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(*it);
			IScene* pVisBase = (IScene*)GetCore()->GetEntity(m_Scenes[k]);

			if (NULL == pVisBase)
			{
				//it = m_Scenes.erase(it);
				m_Scenes.remove(k);
			}
			else
			{
				pVisBase->Update(seconds);
				++k;
				//++it;
			}
		}
	}// zhangbj end 
}

void Gui::Realize()
{
	if (NULL == m_pPainter)
	{
		return;
	}

	m_pPainter->BeginPaint();

		static performance_info_t s_performance;
	static core_performance_info_t s_core_performance;

	if (m_bVisible)
	{
		// 画所有的背景控件
		for (size_t k = 0; k < m_BackControls.size(); ++k)
		{
			IControl* pControl = GetControl(m_BackControls[k]);

			if (pControl)
			{
				pControl->Paint(0.0F);
			}
		}

		// 画所有的模式窗口
		for (size_t i = 0; i < m_ModalFormStack.size(); ++i)
		{
			IControl* pControl = GetControl(m_ModalFormStack[i]);

			if (pControl)
			{
				pControl->Paint(0.0F);
			}
		}
	}

	performance_info_t* pPerf = m_pPainter->GetPerformance();
	pPerf->nGuiDrawCallNum += m_pPainter->GetGuiBatchSize();

	// 刷新显示帧数
	unsigned int cur_tick = Port_GetTickCount();
	float frame_seconds = float(cur_tick - m_nTickCount) * 0.001F;

	m_nTickCount = cur_tick;
	m_nFrameCount++;
	m_fTimeCount += frame_seconds;


  //ls test
	// 显示帧数
	if (m_bShowFPS)
	{
		wchar_t frames[32];

		SafeSwprintf(frames, sizeof(frames), L"FPS:%.2f", m_fFPS);
		this->SetFont("Default");

		int w = m_pPainter->GetTextWidth(frames);

		m_pPainter->WriteText(GetWidth() - (w + 10), 10, 0xFFFF0000, frames);
	}

	// 显示渲染信息
	if (m_bShowRenderInfo)
	{

		this->SetFont("Default");

		int pos_x = 10;
		int pos_y = 30;
		wchar_t info[128];

		SafeSwprintf(info, sizeof(info), 
			L"InitFunc:%2.4f, ShutFunc:%2.4f, ExecFunc:%2.4f, MsgProc:%2.4f, FrameBegin:%2.4f, FrameEnd:%2.4f ",
			s_core_performance.dInitFuncTime, s_core_performance.dShutFuncTime,
			s_core_performance.dExecuteFuncTime, s_core_performance.dMsgProcFuncTime,
			s_core_performance.dExecFrameBeginFuncTime, s_core_performance.dExecFrameEndFuncTime );
		m_pPainter->WriteText(pos_x, pos_y, 0xFFFF0000, info);
		pos_y += 20;


		SafeSwprintf(info, sizeof(info), 
			L"Render:%f, Present:%f, Update:%f, Realize:%f",
			s_performance.dRenderTime, s_performance.dPresentTime,
			s_performance.dUpdateTime, s_performance.dRealizeTime);
		m_pPainter->WriteText(pos_x, pos_y, 0xFFFF0000, info);
		pos_y += 20;

		SafeSwprintf(info, sizeof(info), 
			L"Realize Gui:%f, ShadowMap:%f, Reflection:%f, PostProcess:%f, Sound:%f",
			s_performance.dGuiRealizeTime, 
			s_performance.dShadowMapRealizeTime,
			s_performance.dReflectionRealizeTime, 
			s_performance.dPostProcessRealizeTime,
			s_performance.dSoundRealizeTime);
		m_pPainter->WriteText(pos_x, pos_y, 0xFFFF0000, info);
		pos_y += 20;

		SafeSwprintf(info, sizeof(info), 
			L"Realize Ground:%f, Visual:%f, Grass:%f, Water:%f",
			s_performance.dGroundRealizeTime, 
			s_performance.dVisualRealizeTime, 
			s_performance.dGrassRealizeTime, 
			s_performance.dWaterRealizeTime);
		m_pPainter->WriteText(pos_x, pos_y, 0xFFFF0000, info);
		pos_y += 20;

		SafeSwprintf(info, sizeof(info), 
			L"Realize Model:%f, Actor:%f, Particle:%f, Shadow:%f",
			s_performance.dModelRealizeTime, 
			s_performance.dActorRealizeTime,
			s_performance.dParticleRealizeTime, 
			s_performance.dShadowRealizeTime);
		m_pPainter->WriteText(pos_x, pos_y, 0xFFFF0000, info);
		pos_y += 20;

		SafeSwprintf(info, sizeof(info), 
			L"Refresh Terrain:%f, Particle:%f, Shadow:%f, Skeleton:%f",
			s_performance.dTerrainRefreshTime, 
			s_performance.dParticleRefreshTime,
			s_performance.dShadowRefreshTime,
			s_performance.dSkeletonBlendTime);
		m_pPainter->WriteText(pos_x, pos_y, 0xFFFF0000, info);
		pos_y += 20;

		SafeSwprintf(info, sizeof(info), 
			L"RayTrace:%f, SphereTrace:%f, Cull:%f, DrawGui:%f, BuildShader:%f",
			s_performance.dRayTraceTime, s_performance.dSphereTraceTime,
			s_performance.dCullTime,s_performance.dDrawGuiTime,s_performance.dBuildShaderTime);
		m_pPainter->WriteText(pos_x, pos_y, 0xFFFF0000, info);
		pos_y += 20;


		SafeSwprintf(info, sizeof(info), 
			L"DrawEarlyZBatchs:%2.4f, DrawSolidBatchs:%2.4f, DrawGroundBatchs:%2.4f, DrawAlphaTestSoliBatchs:%2.4f, DrawGrassBatchs:%2.4f",
			s_performance.dDrawEarlyZBatchsTime,s_performance.dDrawSolidBatchsTime,s_performance.dDrawGroundBatchsTime,s_performance.dDrawSolidAlphaTestBatchsTime,s_performance.dDrawGrassBatchsTime );
		m_pPainter->WriteText(pos_x, pos_y, 0xFFFFFF00, info);
		pos_y += 20;

		SafeSwprintf(info, sizeof(info), 
			L"DrawSkyBatchs:%2.4f, DrawWaterBatchs:%2.4f, DrawBlendBatchs:%2.4f, DrawTopmostBatchs:%2.4f, DrawPostBatchs:%2.4f",
			s_performance.dDrawSkyBatchsTime,s_performance.dDrawWaterBatchsTime,s_performance.dDrawBlendBatchsTime, s_performance.dDrawTopmostBatchsTime,
			s_performance.dDrawPostBatchsTime);
		m_pPainter->WriteText(pos_x, pos_y, 0xFFFFFF00, info);
		pos_y += 20;

		SafeSwprintf(info, sizeof(info), 
			L"EarlyZBatchsNum:%d, SolidBatchsNum:%d, GroundBatchsNum:%d, SolidAlphaTestNum:%d, GrassBatchsNum:%d",
			s_performance.nDrawEarlyZBatchsNum,s_performance.nDrawSolidBatchsNum, s_performance.nDrawGroundBatchsNum, s_performance.nDrawSolidAlphaTestBatchsNum,
			s_performance.nDrawGrassBatchsNum);
		m_pPainter->WriteText(pos_x, pos_y, 0xFFFFFF00, info);
		pos_y += 20;

		SafeSwprintf(info, sizeof(info), 
			L"SkyBatchsNum:%d, WaterBatchsNum:%d, BlendBatchsNum:%d, TopmostBatchsNum:%d, PostBatchsNum:%d",
			s_performance.nDrawSkyBatchsNum,s_performance.nDrawWaterBatchsNum,s_performance.nDrawBlendBatchsNum, s_performance.nDrawTopmostBatchsNum,
			s_performance.nDrawPostBatchsNum);
		m_pPainter->WriteText(pos_x, pos_y, 0xFFFFFF00, info);
		pos_y += 20;


		SafeSwprintf(info, sizeof(info), 
			L"nGpuUniformDataUsedNum:%d, nGpuUniformDataSumNum:%d",
			s_performance.nGpuUniformDataUsedNum, s_performance.nGpuUniformDataSumNum);
		m_pPainter->WriteText(pos_x, pos_y, 0xFFFFFF00, info);
		pos_y += 20;

		SafeSwprintf(info, sizeof(info), 
			L"dTexUploadGPUTime:%2.4f, dVBUploadGPUTime:%2.4f, dIBUploadGPUTime:%2.4f, dConstUploadGPUTime:%2.4f, dTexBuildGPUTime:%2.4f ",
			s_performance.dTexUploadGPUTime,s_performance.dVBUploadGPUTime,s_performance.dIBUploadGPUTime,s_performance.dConstUploadGPUTime, s_performance.dTexBuildGPUTime);
		m_pPainter->WriteText(pos_x, pos_y, 0xFFFFFF00, info);
		pos_y += 20;

		SafeSwprintf(info, sizeof(info), 
			L"dCopyTempDepthRTTime:%f, dCopyTempColorRTTime:%f, nRenderStateUsedNum:%d, nRenderStateSumNum:%d ",
			s_performance.dCopyTempDepthRTTime, s_performance.dCopyTempColorRTTime,s_performance.nRenderStateUsedNum,s_performance.nRenderStateSumNum);
		m_pPainter->WriteText(pos_x, pos_y, 0xFFFFFF00, info);
		pos_y += 20;

		SafeSwprintf(info, sizeof(info), 
			L"nRenderShaderUsedNum:%d, nRenderShaderSumNum:%d, nRenderBlendUsedNum:%d, nRenderBlendSumNum:%d ",
			s_performance.nRenderShaderUsedNum, s_performance.nRenderShaderSumNum,s_performance.nRenderBlendUsedNum,s_performance.nRenderBlendSumNum);
		m_pPainter->WriteText(pos_x, pos_y, 0xFFFFFF00, info);
		pos_y += 20;

		SafeSwprintf(info, sizeof(info), 
			L"nRenderUsedVBNum:%d,  nRenderSumVBNum:%d, nRenderUsedIBNum:%d, nRenderSumIBNum:%d ",
			s_performance.nRenderUsedVBNum, s_performance. nRenderSumVBNum,s_performance.nRenderUsedIBNum,s_performance.nRenderSumIBNum);
		m_pPainter->WriteText(pos_x, pos_y, 0xFFFFFF00, info);
		pos_y += 20;

		SafeSwprintf(info, sizeof(info), 
			L"ShaderConstData:%4.4fKB, dUsedTextureData:%4.4fKB, dLoadFileData:%4.4fKB, dLoadTextureData:%4.4fKB",
			s_performance.dShaderConstData,s_performance.dUsedTextureData,s_performance.dLoadFileData,s_performance.dLoadTextureData);
		m_pPainter->WriteText(pos_x, pos_y, 0xFFFFFFFF, info);
		pos_y += 20;

		SafeSwprintf(info, sizeof(info), 
			L"nDrawPointNum:%d, nDrawLineStripNum:%d, nDrawLineLoopNum:%d",
			s_performance.nDrawPointNum, s_performance.nDrawLineStripNum,s_performance.nDrawLineLoopNum);
		m_pPainter->WriteText(pos_x, pos_y, 0xFF0000FF, info);
		pos_y += 20;

		SafeSwprintf(info, sizeof(info), 
			L"nDrawLineNum:%d, nDrawTriangleNum:%d, nDrawTriangleStripNum:%d, nDrawTriangleFanNum:%d",
			s_performance.nDrawLineNum,s_performance.nDrawTriangleNum,s_performance.nDrawTriangleStripNum,s_performance.nDrawTriangleFanNum);
		m_pPainter->WriteText(pos_x, pos_y, 0xFF0000FF, info);
		pos_y += 20;

		SafeSwprintf(info, sizeof(info), 
			L"nRenderDrawIndexNum:%d, nRenderDrawArrayNum:%d, fPerDrawCallNum:%8.0f, fPerDrawTriangleNum:%8.0f",
			s_performance.nRenderDrawIndexNum,s_performance.nRenderDrawArrayNum,m_dPerDrawCallNum,m_dPerDrawTriangleNum);
		m_pPainter->WriteText(pos_x, pos_y, 0xFF0000FF, info);
		pos_y += 20;

		SafeSwprintf(info, sizeof(info), 
			L"nCollisionShapeNum:%d, nRigidbodyNum:%d, nConstraintNum:%d, nRagdollNum:%d, nPhysicsTime:%f",
			s_performance.nCollisionShapeNum,s_performance.nRigidbodyNum,s_performance.nConstraintNum,s_performance.nRagdollNum,s_performance.nPhysicsTime);
		m_pPainter->WriteText(pos_x, pos_y, 0xFF00FFFF, info);
		pos_y += 20;

		SafeSwprintf(info, sizeof(info), 
			L"EffectParticleDC: Count = %d, Avg = %8.0f, max = %8.0f, EffectModelDC: Count = %d, Avg = %8.0f, max = %8.0f",
			s_performance.nEffectParticleDrawCallNum,s_performance.dAvgEffectParticleDrawCallNumPerFrame,m_dEffectParticleDrawCallMaxNumPreFrame,
			s_performance.nEffectModelDrawCallNum,s_performance.dAvgEffectModelDrawCallNumPerFrame,m_dEffectModelDrawCallMaxNumPreFrame);
		m_pPainter->WriteText(pos_x, pos_y, 0xFF00FF00, info);
		pos_y += 20;

		SafeSwprintf(info, sizeof(info), 
			L"GuiDC: Count = %d, Avg = %8.0f, max = %8.0f",
			s_performance.nGuiDrawCallNum,s_performance.dAvgGuiDrawCallNumPerFrame, m_dGuiDrawCallMaxNumPreFrame);
		m_pPainter->WriteText(pos_x, pos_y, 0xFF0055FF, info);
		pos_y += 20;

		/*
		// 显示每帧时间
		static float s_frame_times[300];
		static int s_frame_count = 0;
		static int s_points[1024];

		if (s_frame_count == (sizeof(s_frame_times) / sizeof(float)))
		{
		memmove(s_frame_times, s_frame_times + 1, 
		(s_frame_count - 1) * sizeof(float));
		--s_frame_count;
		}

		// 这是经过平滑处理后的帧时间
		float frame_time = (float)GetCore()->GetFrameSeconds();

		s_frame_times[s_frame_count++] = frame_time;

		for (int k = 0; k < s_frame_count; ++k)
		{
		s_points[k * 2] = 100 + k;
		s_points[k * 2 + 1] = FM_CLAMP(100,int(s_frame_times[k] * 300),300);
		}

		m_pPainter->DrawLinkedLines(s_points, s_frame_count, 0xFFFFFFFF);
		*/
	}

	m_dPerDrawCallNum = (s_performance.nRenderDrawIndexNum + s_performance.nRenderDrawArrayNum) /  m_fFPS;
	m_dPerDrawTriangleNum = (s_performance.nDrawTriangleNum /3 + FM_MAX((s_performance.nDrawTriangleStripNum-2),0) + FM_MAX((s_performance.nDrawTriangleFanNum - 1),0)) / m_fFPS;

	s_performance.dFPSTime = m_fFPS;
	s_performance.dPerDrawCallNum = m_dPerDrawCallNum;
	s_performance.dPerDrawTriangleNum = m_dPerDrawTriangleNum;

	s_performance.dAvgEffectModelDrawCallNumPerFrame = s_performance.nEffectModelDrawCallNum / m_fFPS;
	s_performance.dAvgEffectParticleDrawCallNumPerFrame = s_performance.nEffectParticleDrawCallNum / m_fFPS;
	s_performance.dAvgGuiDrawCallNumPerFrame = s_performance.nGuiDrawCallNum / m_fFPS;

	if (s_performance.dAvgEffectModelDrawCallNumPerFrame > m_dEffectModelDrawCallMaxNumPreFrame)
	{
		m_dEffectModelDrawCallMaxNumPreFrame = s_performance.dAvgEffectModelDrawCallNumPerFrame;
	}

	if (s_performance.dAvgEffectParticleDrawCallNumPerFrame > m_dEffectParticleDrawCallMaxNumPreFrame)
	{
		m_dEffectParticleDrawCallMaxNumPreFrame = s_performance.dAvgEffectParticleDrawCallNumPerFrame;
	}

	if (s_performance.dAvgGuiDrawCallNumPerFrame > m_dGuiDrawCallMaxNumPreFrame)
	{
		m_dGuiDrawCallMaxNumPreFrame = s_performance.dAvgGuiDrawCallNumPerFrame;
	}

	m_dMaxValTimeCount += frame_seconds;

	if (m_dMaxValTimeCount >= 10.0f)
	{
		m_dMaxValTimeCount = 0.0f;

		m_dEffectModelDrawCallMaxNumPreFrame = 0.0;
		m_dEffectParticleDrawCallMaxNumPreFrame = 0.0;
		m_dGuiDrawCallMaxNumPreFrame = 0.0;
	}

	if (m_fTimeCount >= 1.0F)
	{
		m_fFPS = float(m_nFrameCount) / m_fTimeCount;
		m_nFrameCount = 0;
		m_fTimeCount = 0.0F;

		// 先打印出来
		fm_tracelog((const char*)&s_performance,LOG_RENDER);
        fm_tracelog((const char*)&s_core_performance,LOG_CORE);

		s_performance = *pPerf;
		memset(pPerf, 0, sizeof(performance_info_t));

		core_performance_info_t* pCorePerf = GetCore()->GetPerformance();
		s_core_performance = *pCorePerf;
		memset(pCorePerf, 0, sizeof(core_performance_info_t));

		m_nCountFPS++;
		m_fAverageFPS = (m_fFPS - m_fAverageFPS) / m_nCountFPS + m_fAverageFPS;
	}

	m_pPainter->EndPaint();
}

void Gui::SetDefaultPath(const char* value)
{
	m_pPainter->SetDefaultPath(value);
}

const char* Gui::GetDefaultPath() const
{
	return m_pPainter->GetDefaultPath();
}

//void* Gui::GetWinHandle() const
//{
//	return GetCore()->GetWindowHandle();
//}

const char* Gui::GetPainterName() const
{
	return m_strPainterName.c_str();
}

PERSISTID Gui::GetDesktopID() const
{
	if (NULL == m_pDesktop)
	{
		return PERSISTID();
	}

	return m_pDesktop->GetID();
}

IControl* Gui::GetDesktopForm() const
{
	return m_pDesktop;
}

bool Gui::CreateDesktopForm()
{
	if (m_pDesktop != NULL)
	{
		CORE_TRACE("(Gui:CreateDesktopForm)desktop created");
		return false;
	}

	Form* form = (Form*)CreateControl("Form");

	if (NULL == form)
	{
		return false;
	}

	form->SetIsDesktop(true);
	// 主窗口缺省为透明状态
	form->SetTransparent(true);
	form->SetWidth(GetWidth());
	form->SetHeight(GetHeight());

	m_pDesktop = form;

	return true;
}

PERSISTID Gui::CreateSelector(const PERSISTID& parent_id)
{
	IControl* selector = CreateControl("Selector");

	if (NULL == selector)
	{
		return PERSISTID();
	}

	IControl* parent = GetControl(parent_id);

	if (parent != NULL)
	{
		selector->SetParentControl(parent);
	}

	return selector->GetID();
}

PERSISTID Gui::CreateImageList()
{
	IEntity* pEntity = GetCore()->CreateEntityArgs("ImageList", 
		CVarList() << GetPainterName() << GetID());

	if (NULL == pEntity)
	{
		return PERSISTID();
	}

	return pEntity->GetID();
}

IControl* Gui::CreateControl(const char* name)
{
	IEntity* pEntity = GetCore()->CreateEntityArgs(name, 
		CVarList() << GetPainterName() << GetID());

	if (NULL == pEntity)
	{
		return NULL;
	}

	if (!pEntity->GetEntInfo()->IsKindOf("IControl"))
	{
		pEntity->Release();
		return NULL;
	}

	IControl* pControl = (IControl*)pEntity;

	return pControl;
}

bool Gui::DeleteControl(IControl* pControl)
{
	Assert(pControl != NULL);

	IControl* parent = pControl->GetParentControl();

	if (parent != NULL)
	{
		parent->RemoveChild(pControl);
	}

	pControl->Release();

	return true;
}

PERSISTID Gui::Create(const char* name)
{
	IControl* pControl = CreateControl(name);

	if (NULL == pControl)
	{
		return PERSISTID();
	}

	return pControl->GetID();
}

bool Gui::Delete(const PERSISTID& id)
{
	IControl* pControl = GetControl(id);

	if (NULL == pControl)
	{
		return false;
	}

	return DeleteControl(pControl);
}

void Gui::SetFocusControl(IControl* val)
{
	//ChangeFocus(val, GetControl(m_FocusID));
}

IControl* Gui::GetFocusControl() const
{
	return GetControl(m_FocusID);
}

void Gui::SetFocused(const PERSISTID& id)
{
	IControl* pControl = GetControl(id);

	SetFocusControl(pControl);
}

PERSISTID Gui::GetFocused() const
{
	IControl* pControl = GetFocusControl();

	if (NULL == pControl)
	{
		return PERSISTID();
	}

	return pControl->GetID();
}

bool Gui::AddBackControl(const PERSISTID& id)
{
	IControl* pControl = GetControl(id);

	if (NULL == pControl)
	{
		return false;
	}

	m_BackControls.push_back(id);

	return true;
}

bool Gui::RemoveBackControl(const PERSISTID& id)
{
	size_t back_control_num = m_BackControls.size();

	for (size_t i = 0; i < back_control_num; ++i)
	{
		if (m_BackControls[i] == id)
		{
			m_BackControls.remove(i);
			return true;
		}
	}

	return false;
}

bool Gui::AddModalForm(IControl* pControl)
{
	Assert(pControl != NULL);

	bool bExist = false;

	// 如果以前存在则先删除, 再添加到最后
	for (size_t k = 0; k < m_ModalFormStack.size(); ++k)
	{
		if (m_ModalFormStack[k] == pControl->GetID())
		{
			m_ModalFormStack.remove(k);
			bExist = true;
			break;
		}
	}

	m_ModalFormStack.push_back(pControl->GetID());

	SetFocusControl(NULL);

	// 如果以前不存在，则调用进入
	if (!bExist)
	{
		pControl->OnEnter(PERSISTID());
	}

	return true;
}

bool Gui::RemoveModalForm(IControl* pControl)
{
	Assert(pControl != NULL);

	for (size_t k = 0; k < m_ModalFormStack.size(); ++k)
	{
		if (m_ModalFormStack[k] == pControl->GetID())
		{
			pControl->OnLeave(PERSISTID());
			m_ModalFormStack.remove(k);
			return true;
		}
	}

	return false;
}

IControl* Gui::GetModalForm() const
{
	if (m_ModalFormStack.empty())
	{
		return NULL;
	}
	else
	{
		return GetControl(m_ModalFormStack.back());
	}
}

PERSISTID Gui::GetModalFormID() const
{
	if (m_ModalFormStack.empty())
	{
		return PERSISTID();
	}
	else
	{
		return m_ModalFormStack.back();
	}
}

PERSISTID Gui::GetLoader() const
{
	if (m_pLoader == NULL)
	{
		return PERSISTID();
	}
	else
	{
		return m_pLoader->GetID();
	}
}

PERSISTID Gui::GetDesigner()
{
	if (m_pDesigner == NULL)
	{
		m_pDesigner = (Designer*)GetCore()->CreateEntity("Designer");

		if (m_pDesigner != NULL)
		{
			m_pDesigner->SetGui(this);
			m_pDesigner->SetPainter(m_pPainter);
			m_pDesigner->SetDesignMode(true);

			return m_pDesigner->GetID();
		}
		else
		{
			return PERSISTID();
		}
	}
	else
	{
		return m_pDesigner->GetID();
	}
}

IControl* Gui::GetControl(const PERSISTID& id) const
{
	IEntity* pEntity = GetCore()->GetEntity(id);

	if (NULL == pEntity)
	{
		return NULL;
	}

	if (!pEntity->GetEntInfo()->IsKindOf("IControl"))
	{
		return NULL;
	}

	return (IControl*)pEntity;
}

float Gui::GetFPS() const
{
	return m_fFPS;
}

float Gui::GetAverageFPS() const
{
	return m_fAverageFPS;
}

double Gui::GetPerDrawCallNum() const
{
	return m_dPerDrawCallNum;
}

double Gui::GetPerDrawTriangleNum() const
{
	return m_dPerDrawTriangleNum;
}

int Gui::GetWidth() const
{
	return m_pPainter->GetRenderWidth();
}

int Gui::GetHeight() const
{
	return m_pPainter->GetRenderHeight();
}

int Gui::GetDeviceWidth() const
{
	return m_pPainter->GetDeviceWidth();
}

int Gui::GetDeviceHeight() const
{
	return m_pPainter->GetDeviceHeight();
}


PERSISTID Gui::GetCaptureID() const
{
	return m_CaptureID;
}

PERSISTID Gui::GetClickedID(size_t nTouchId) const
{
#ifdef _WIN32
    nTouchId = 0;   //WIN32下不支持多点触摸
#endif

    PERSISTID clickedID;
    if (m_ClickedCotrols.GetData(nTouchId, clickedID))
    {
        return clickedID;
    }

	return PERSISTID();
}

void Gui::AddClickedID(size_t nTouchId, const PERSISTID& clickedId)
{
#ifdef _WIN32
    nTouchId = 0;   //WIN32下不支持多点触摸
#endif

    m_ClickedCotrols.Set(nTouchId, clickedId);
}

void Gui::RemoveClickedID(size_t nTouchId)
{
#ifdef _WIN32
    nTouchId = 0;   //WIN32下不支持多点触摸
#endif
    m_ClickedCotrols.Remove(nTouchId);
}

//PERSISTID Gui::GetCoolManagerID() const
//{
//	return m_CoolManagerID;
//}

//PERSISTID Gui::GetGameHandID() const
//{
//	return m_GameHand;
//}

//
PERSISTID Gui::GetTextManagerID() const
{
	return m_TextManager;
}

//PERSISTID Gui::GetHyperLinkManagerID() const
//{
//    return m_HyperLinkMngID;
//}

//
PERSISTID Gui::GetAnimationManagerIDNew() const
{
	return m_AnimMngID;
}

//
ImageAnimationManager* Gui::GetAnimationManager() const
{
	const IVar& varImgMrg = GetCore()->GetGlobalValue("ImageAnimationManager");
	return (ImageAnimationManager*)GetCore()->GetEntity(varImgMrg.ObjectVal());
}

//
void Gui::SetVisible(bool value)
{
	m_bVisible = value;
}

bool Gui::GetVisible() const
{
	return m_bVisible;
}

void Gui::SetShowFPS(bool value)
{
	m_bShowFPS = value;
}

bool Gui::GetShowFPS() const
{
	return m_bShowFPS;
}

void Gui::SetScaleEnable(bool value)
{
	m_pPainter->SetScaleEnable(value);
}

bool Gui::GetScaleEnable() const
{
	return m_pPainter->GetScaleEnable();
}

void Gui::SetScaleRatio(float value)
{
	m_pPainter->SetScaleRatio(value);
}

float Gui::GetScaleRatio() const
{
	return m_pPainter->GetScaleRatio();
}

// 性能监控器
void Gui::SetLogPerf(bool bValue)
{
	if(m_bLogPerf!=bValue)
	{
		if(bValue)
		{
			m_bLogPerf = EnableLogPerf();
			m_bLogPerf = bValue;
		}
		else
		{
			DiscardLogPerf();
			m_bLogPerf = bValue;
		}    
	}
}

bool Gui::GetLogPerf() const
{
	return m_bLogPerf;
}

bool Gui::EnableLogPerf()
{
#ifndef FX_SYSTEM_WINDOWS
	s_fp_GuiLogPerf = core_file::fopen("fmlog.log", "wb");
#endif

#ifndef FX_SYSTEM_LINUX
	s_fp_GuiLogPerf = core_file::fopen("/sdcard/fmlog_perf_gui.log", "wb");
#endif

	if(s_fp_GuiLogPerf)
	{
		//先写入大小
		size_t flag = 'logg';//log
		core_file::fwrite(&flag,sizeof(size_t),1,s_fp_GuiLogPerf);
		size_t size =  sizeof(performance_info_t);
		core_file::fwrite(&size,sizeof(size_t),1,s_fp_GuiLogPerf);
	}

	if( s_fp_GuiLogPerf == NULL )
		return false;

	m_pThread = (CThread*)CORE_ALLOC(sizeof(CThread));
	new (m_pThread) CThread(WorkerProc, NULL, NULL, this, -1, 0);
	m_pThreadWaiter = CORE_NEW(CThreadWaiter);

	m_pThread->Start();
	return true;
}

void Gui::DiscardLogPerf()
{
	if(m_pThread)
		m_pThread->Stop();

	CORE_DELETE(m_pThread);
	CORE_DELETE(m_pThreadWaiter);

	if(s_fp_GuiLogPerf)
	{
		core_file::fclose(s_fp_GuiLogPerf);
		s_fp_GuiLogPerf = NULL;
	}
}

bool Gui::ConvertText(const wchar_t* src, result_wstring& dst)
{
	if (WideStrEmpty(src))
	{
		return false;
	}

	Text* pTextManager = (Text*)GetCore()->GetEntity(m_TextManager);

	if (NULL == pTextManager)
	{
		return false;
	}

	size_t size = Port_GetWideStrToUTF8Len(src);
	TAutoMem<char, 256, TCoreAlloc> auto_buf(size);
	char* szIDName = auto_buf.GetBuffer();

	Port_WideStrToUTF8(src, szIDName, size*sizeof(wchar_t));

	//std::string szIDName = StringUtil::WideStrAsString(src);

	//if (szIDName.size() > 2)
	if (strlen(szIDName) > 2)
	{
		//szIDName = szIDName.substr(1, szIDName.size() - 1);

		//long id = pTextManager->GetTextID(szIDName.c_str());
		long id = pTextManager->GetTextID(szIDName + 1);

		if (!pTextManager->IsVaildID(id))
		{
			return false;
		}

		dst = pTextManager->GetText(id).c_str();

		return true;
	}

	return false;
}

void Gui::SetAbandomMouseMove(bool value)
{
	m_bAbandomMouseMove = value;
}

bool Gui::GetAbandomMouseMove() const
{
	return m_bAbandomMouseMove;
}

void Gui::SetShowRenderInfo(bool value)
{
	m_bShowRenderInfo = value;
}

bool Gui::GetShowRenderInfo() const
{
	return m_bShowRenderInfo;
}

void Gui::SetHintDelay(float value)
{
	m_fHintDelay = value;
}

float Gui::GetHintDelay() const
{
	return m_fHintDelay;
}

/*
void Gui::SetSoftCursor(bool value)
{
if (m_pPainter->GetSoftCursor() != value)
{
m_pPainter->SetSoftCursor(value);

if (value)
{
// 显示WINDOWS鼠标
ShowCursor(FALSE);
}
else
{
// 显示WINDOWS鼠标
ShowCursor(TRUE);
}
}
}

bool Gui::GetSoftCursor() const
{
return m_pPainter->GetSoftCursor();
}
*/

// 是否进行整单词换行处理
void Gui::SetWordChangeLine(bool value)
{
	m_bWordChangeLine = value;
}

bool Gui::GetWordChangeLine() const
{
	return m_bWordChangeLine;
}

/*
// 颜色
bool Gui::FindColor(const char* name) const
{
return m_Colors.Exists(name);
}

bool Gui::AddColor(const char* name, const char* color)
{
if (m_Colors.Exists(name))
{
m_Colors.Remove(name);
}

int value = GuiUtil_ColorToInt(color);

return m_Colors.Add(name, value);
}

int Gui::GetColor(const char* name) const
{
int value;

if (!m_Colors.GetData(name, value))
{
return GuiUtil_ColorToInt(name);
}

return value;
}

void Gui::ClearColor()
{
m_Colors.Clear();
}

// 图元

bool Gui::FindImage(const char* name) const
{
return m_Images.Exists(name);
}

bool Gui::AddImage(const char* name, const char* tex, int left, int top, 
int right, int bottom)
{
CImageData* p = CORE_NEW(CImageData);

p->m_strTex = tex;
p->m_nLeft = left;
p->m_nTop = top;
p->m_nRight = right;
p->m_nBottom = bottom;

CImageData* pOldImage;

if (m_Images.GetData(name, pOldImage))
{
m_Images.Remove(name);
CORE_DELETE(pOldImage);
}

return m_Images.Add(name, p);
}

bool Gui::GetImage(const char* name, core_string& tex, int& left, int& top, 
int& right, int& bottom) const
{
CImageData* p;

if (!m_Images.GetData(name, p))
{
return GuiUtil_ConvertImage(name, tex, left, top, right, bottom);
}

tex = p->m_strTex.c_str();
left = p->m_nLeft;
top = p->m_nTop;
right = p->m_nRight;
bottom = p->m_nBottom;

return true;
}

bool Gui::AddImageData(const char* name, const CImageData& data)
{
CImageData* p = CORE_NEW(CImageData);

p->Copy(data);

CImageData* pOldImage;

if (m_Images.GetData(name, pOldImage))
{
m_Images.Remove(name);
CORE_DELETE(pOldImage);
}

return m_Images.Add(name, p);
}

bool Gui::GetImageData(const char* name, CImageData& data) const
{
CImageData* p;

if (!m_Images.GetData(name, p))
{
data.DeleteFlip();

return GuiUtil_ConvertImage(name, data.m_strTex, 
data.m_nLeft, data.m_nTop, data.m_nRight, data.m_nBottom);
}

data.Copy(*p);

return true;
}

void Gui::ClearImage()
{
//IMAGE_CONTAINER::iterator it = m_Images.Begin();
//IMAGE_CONTAINER::iterator end = m_Images.End();

//for (; it != end; ++it)
//{
//	CORE_DELETE(it.GetData());
//}

//m_Images.Clear();
}
*/

// 声音

bool Gui::FindSound(const char* name) const
{
	return m_Sounds.Exists(name);
}

bool Gui::AddSound(const char* name, const char* sound_file)
{
	Assert(sound_file != NULL);

	sound_data_t* pOldSound;

	if (m_Sounds.GetData(name, pOldSound))
	{
		m_Sounds.Remove(name);
		CORE_DELETE(pOldSound);
	}

	sound_data_t* p = CORE_NEW(sound_data_t);

	p->strSoundFile = sound_file;

	return m_Sounds.Add(name, p);
}

bool Gui::GetSound(const char* name, sound_data_t& data) const
{
	sound_data_t* p;

	if (!m_Sounds.GetData(name, p))
	{
		return false;
	}

	data = *p;

	return true;
}

void Gui::ClearSound()
{
	sound_container_t::iterator it = m_Sounds.Begin();
	sound_container_t::iterator end = m_Sounds.End();

	for (; it != end; ++it)
	{
		CORE_DELETE(it.GetData());
	}

	m_Sounds.Clear();
}

bool Gui::PlayingSound(const char* name)
{
	if (StringEmpty(name))
	{
		return false;
	}

	sound_data_t* p;

	if (!m_Sounds.GetData(name, p))
	{
		CORE_TRACE_EX("(Gui::PlayingSound)sound:%s not found", name);
		return false;
	}

	event_send(GetID(),"on_play_sound",CVarList() << name << p->strSoundFile.c_str());

	/*if (m_pGuiHook)
	{
	int res = m_pGuiHook->OnEvent(this, "on_play_sound", 
	CVarList() << name << p->strSoundFile.c_str());

	return (res != 0);
	}*/



	//	int res = GuiUtil_RunCallback(this, "on_play_sound", 
	//		CVarList() << name << p->strSoundFile.c_str());
	//	return (res != 0);
	return true;
}

const char* Gui::GetSoundFile(const char* name) const
{
	if (StringEmpty(name))
	{
		return "";
	}

	sound_data_t* p;

	if (!m_Sounds.GetData(name, p))
	{
		CORE_TRACE("(Gui::PlayingSound)sound not found");
		CORE_TRACE(name);
		return "";
	}

	return p->strSoundFile.c_str();
}

void Gui::ClearFont()
{
	for (size_t i = 0; i < m_Fonts.size(); ++i)
	{
		m_Fonts[i]->Release();
	}

	m_Fonts.clear();
	m_FontIndex.Clear();
	m_MapReferFont.Clear();
}

PERSISTID Gui::NewFont()
{
	Font* font = (Font*)GetCore()->CreateEntityArgs("Font", 
		CVarList() << GetPainterName());

	if (NULL == font)
	{
		return PERSISTID();
	}

	return font->GetID();
}

bool Gui::FindFontIndex(const char* name, size_t& index) const
{
	return m_FontIndex.GetData(name, index);
}

bool Gui::FindFont(const char* name) const
{
	return m_FontIndex.Exists(name);
}

bool Gui::AddDefaultFont()
{
	// 创建缺省字体
	char font_file[256];
	const char *pDefPath = m_pPainter->GetDefaultPath();
	if(strlen(pDefPath)==0)
	{
		SafeSprintf(font_file, sizeof(font_file), "font/FZCY_GBK.TTF");
	}
	else
	{
		SafeSprintf(font_file, sizeof(font_file), "%sfont/FZCY_GBK.TTF", pDefPath);
	}

	Font* pFont = (Font*)GetCore()->CreateEntityArgs("Font", 
		CVarList() << GetPainterName());

	pFont->SetName("Default");
	pFont->SetFontFile(font_file);
	//pFont->SetSysFont(font_name);
	//pFont->SetHeight(16);
	//pFont->SetVarExtent(true);
	//pFont->SetItalic(true);
	//pFont->SetWeight(1);
	//pFont->SetUnderline(true);
	//pFont->SetSmooth(true);

	return AddFont(pFont->GetID());
}

bool Gui::AddFont(const PERSISTID& id)
{
	Font* font = (Font*)GetCore()->GetEntity(id);

	if (NULL == font)
	{
		CORE_TRACE("(Gui::AddFont)entity not exists");
		return false;
	}

	if (!font->GetEntInfo()->IsKindOf("Font"))
	{
		CORE_TRACE("(Gui::AddFont)class not match");
		return false;
	}

	if (!font->Create())
	{
		CORE_TRACE("(Gui::AddFont)create failed");
		CORE_TRACE(font->GetName());
		return false;
	}

	size_t index;

	if (m_FontIndex.GetData(font->GetName(), index))
	{
		// 替换掉原来的字体
		m_Fonts[index]->Release();
		m_Fonts[index] = font;
		return true;
	}

	m_FontIndex.Add(font->GetName(), m_Fonts.size());
	m_Fonts.push_back(font);

	return true;
}

bool Gui::AddReferFont(const char* name, const char* refer_name)
{
	// 如果此字体已存在，则返回
	if (m_FontIndex.Exists(name))
	{
		// 已存在于实字体中
		return true;
	}

	if (m_MapReferFont.Exists(name))
	{
		// 已存在于引用字体中
		return true;
	}

	// 被引用的字体也是引用字体,则取其实字体，并添加引用
	const char* data = m_MapReferFont.GetData(refer_name);

	if (data != NULL)
	{
		m_MapReferFont.Add(name, data);
		return true;
	}

	// 被引用的字体是实字体，则直接添加引用
	if (m_FontIndex.Exists(refer_name))
	{
		m_MapReferFont.Add(name, refer_name);
		return true;
	}

	return false;
}

bool Gui::SetFont(const char* name, unsigned int shadow_color)
{
	Assert(name != NULL);

	if (NULL == m_pPainter)
	{
		return false;
	}

	// 如果此字体是实字体，则直接设置
	if (m_FontIndex.Exists(name))
	{
		// 已存在于实字体中
		m_pPainter->SetFont(name);
		m_pPainter->SetFontShadowColor(name, shadow_color);
		return true;
	}

	// 如果此字体是引用字体，则检查其实字体，存在则设置
	const char* real_font = m_MapReferFont.GetData(name);

	if (real_font != NULL)
	{
		if (m_FontIndex.Exists(real_font))
		{
			// 已存在于实字体中
			m_pPainter->SetFont(real_font);
			m_pPainter->SetFontShadowColor(real_font, shadow_color);
			return true;
		}
	}

	if (strcmp(name, "Default") == 0)
	{
		return false;
	}

	return SetFont("Default");
}

PERSISTID Gui::GetFont(const char* name) const
{
	size_t index;

	if (!m_FontIndex.GetData(name, index))
	{
		return PERSISTID();
	}

	return m_Fonts[index]->GetID();
}

bool Gui::DumpAllFontTex()
{
	return m_pPainter->DumpAllFontTex();
}

// 鼠标

void Gui::ClearCursor()
{
	for (size_t i = 0; i < m_Cursors.size(); ++i)
	{
		m_Cursors[i]->Release();
	}

	m_Cursors.clear();
}

PERSISTID Gui::NewCursor()
{
	Cursor* cursor = (Cursor*)GetCore()->CreateEntityArgs("Cursor", 
		CVarList() << GetPainterName());

	if (NULL == cursor)
	{
		return PERSISTID();
	}

	return cursor->GetID();
}

bool Gui::FindCursorIndex(const char* name, size_t& index) const
{
	Assert(name != NULL);

	unsigned int hash = GetHashValueCase(name);

	for (size_t i = 0; i < m_Cursors.size(); i++)
	{
		if ((m_Cursors[i]->GetHash() == hash)
			&& (strcmp(m_Cursors[i]->GetName(), name) == 0))
		{
			index = i;
			return true;
		}
	}

	return false;
}

bool Gui::FindCursor(const char* name) const
{
	size_t index;

	return FindCursorIndex(name, index);
}

bool Gui::AddCursor(const PERSISTID& id)
{
	Cursor* cursor = (Cursor*)GetCore()->GetEntity(id);

	if (NULL == cursor)
	{
		CORE_TRACE("(Gui::AddCursor)entity not exists");
		return false;
	}

	if (!cursor->GetEntInfo()->IsKindOf("Cursor"))
	{
		CORE_TRACE("(Gui::AddCursor)class not match");
		return false;
	}

	size_t index;

	if (FindCursorIndex(cursor->GetName(), index))
	{
		m_Cursors[index]->Release();
		m_Cursors[index] = cursor;
		return true;
	}

	m_Cursors.push_back(cursor);

	return true;
}

PERSISTID Gui::GetCursor(const char* name) const
{
	size_t index;

	if (!FindCursorIndex(name, index))
	{
		return PERSISTID();
	}

	return m_Cursors[index]->GetID();
}

bool Gui::SetCursorPos(int x, int y)
{
#ifdef _WIN32
	return ::SetCursorPos(x, y) == TRUE;
#else
	return false;
#endif
	//return m_pPainter->SetCursorPos(x, y);
}

bool Gui::SetCurrentCursor(const char* name)
{
	if (StringEmpty(name))
	{
		name = "Default";
	}

	// 原鼠标
	Cursor* pCursor = InnerGetCursor();

	if (pCursor != NULL)
	{
		if (strcmp(pCursor->GetName(), name) == 0)
		{
			return true;
		}
	}

	//GameHand* pGameHand = InnerGetGameHand();

	//if (pGameHand)
	//{
	//	if (!pGameHand->IsEmpty())
	//	{
	//		return false;
	//	}
	//}

	size_t index;

	if (!FindCursorIndex(name, index))
	{
		if (!FindCursorIndex("Default", index))
		{
			return false;
		}
	}

	Cursor* cursor = m_Cursors[index];

	if (!cursor->Apply())
	{
		return false;
	}

	// 隐藏原鼠标
	if (pCursor != NULL)
	{
		pCursor->Hide();
	}

	// 显示当前鼠标
	cursor->Show();

	//cursor->Reset();

	m_CursorID = cursor->GetID();

	return true;
}

void Gui::SetCursorName(const char* value)
{
	Assert(value != NULL);

	m_strCursorName = value;

	SetCurrentCursor(m_strCursorName.c_str());
}

const char* Gui::GetCursorName() const
{
	return m_strCursorName.c_str();
}

// 输入光标

void Gui::ClearCaret()
{
	for (size_t i = 0; i < m_Carets.size(); ++i)
	{
		m_Carets[i]->Release();
	}

	m_Carets.clear();
}

PERSISTID Gui::NewCaret()
{
	Caret* caret = (Caret*)GetCore()->CreateEntityArgs("Caret", 
		CVarList() << GetPainterName());

	if (NULL == caret)
	{
		return PERSISTID();
	}

	return caret->GetID();
}

bool Gui::FindCaretIndex(const char* name, size_t& index) const
{
	Assert(name != NULL);

	unsigned int hash = GetHashValueCase(name);

	for (size_t i = 0; i < m_Carets.size(); ++i)
	{
		if ((m_Carets[i]->GetHash() == hash)
			&& (strcmp(m_Carets[i]->GetName(), name) == 0))
		{
			index = i;
			return true;
		}
	}

	return false;
}

bool Gui::FindCaret(const char* name) const
{
	size_t index;

	return FindCaretIndex(name, index);
}

bool Gui::AddCaret(const PERSISTID& id)
{
	Caret* caret = (Caret*)GetCore()->GetEntity(id);

	if (NULL == caret)
	{
		CORE_TRACE("(Gui::AddCaret)entity not exists");
		return false;
	}

	if (!caret->GetEntInfo()->IsKindOf("Caret"))
	{
		CORE_TRACE("(Gui::AddCaret)class not match");
		return false;
	}

	size_t index;

	if (FindCaretIndex(caret->GetName(), index))
	{
		m_Carets[index]->Release();
		m_Carets[index] = caret;
		return true;
	}

	m_Carets.push_back(caret);

	return true;
}

PERSISTID Gui::GetCaret(const char* name) const
{
	size_t index;

	if (!FindCaretIndex(name, index))
	{
		return PERSISTID();
	}

	return m_Carets[index]->GetID();
}

// 浮动提示

void Gui::ClearHint()
{
	for (size_t i = 0; i < m_Hints.size(); ++i)
	{
		m_Hints[i]->Release();
	}

	m_Hints.clear();
}

PERSISTID Gui::NewHint(const char* control_name)
{
	Assert(control_name != NULL);

	IControl* pControl = (IControl*)GetCore()->CreateEntityArgs(
		control_name, CVarList() << GetPainterName());

	if (NULL == pControl)
	{
		return PERSISTID();
	}

	if (!pControl->GetEntInfo()->IsKindOf("IControl"))
	{
		CORE_TRACE("(Gui::NewHint)class not match");
		pControl->Release();
		return PERSISTID();
	}

	return pControl->GetID();
}

bool Gui::FindHintIndex(const char* name, size_t& index) const
{
	Assert(name != NULL);

	unsigned int hash = GetHashValueCase(name);

	for (size_t i = 0; i < m_Hints.size(); i++)
	{
		if ((m_Hints[i]->GetHash() == hash)
			&& (strcmp(m_Hints[i]->GetName(), name) == 0))
		{
			index = i;
			return true;
		}
	}

	return false;
}

bool Gui::FindHint(const char* name) const
{
	size_t index;

	return FindHintIndex(name, index);
}

bool Gui::ShowHint(const PERSISTID& id) const
{
	IControl* pControl = GetControl(id);

	if (pControl == NULL)
	{
		return false;
	}

	// 当前的模式窗口
	IControl* pModalForm = GetModalForm();

	if (NULL == pModalForm)
	{
		return false;
	}

	pModalForm->AddTopLayer(pControl->GetID());

	return true;
}

bool Gui::AddHint(const PERSISTID& id)
{
	IControl* pControl = (IControl*)GetCore()->GetEntity(id);

	if (NULL == pControl)
	{
		CORE_TRACE("(Gui::AddHint)entity not exists");
		return false;
	}

	if (!pControl->GetEntInfo()->IsKindOf("IControl"))
	{
		CORE_TRACE("(Gui::AddHint)class not match");
		return false;
	}

	size_t index;

	if (FindHintIndex(pControl->GetName(), index))
	{
		m_Hints[index]->Release();
		m_Hints[index] = pControl;
		return true;
	}

	m_Hints.push_back(pControl);

	return true;
}

PERSISTID Gui::GetHint(const char* name) const
{
	size_t index;

	if (!FindHintIndex(name, index))
	{
		return PERSISTID();
	}

	return m_Hints[index]->GetID();
}

// 上下文菜单

void Gui::ClearContext()
{
	for (size_t i = 0; i < m_Contexts.size(); ++i)
	{
		m_Contexts[i]->Release();
	}

	m_Contexts.clear();
}

PERSISTID Gui::NewContext(const char* control_name)
{
	Assert(control_name != NULL);

	IControl* pControl = (IControl*)GetCore()->CreateEntityArgs(
		control_name, CVarList() << GetPainterName());

	if (NULL == pControl)
	{
		return PERSISTID();
	}

	if (!pControl->GetEntInfo()->IsKindOf("IControl"))
	{
		CORE_TRACE("(Gui::NewContext)class not match");
		pControl->Release();
		return PERSISTID();
	}

	return pControl->GetID();
}

bool Gui::FindContextIndex(const char* name, size_t& index) const
{
	Assert(name != NULL);

	unsigned int hash = GetHashValueCase(name);

	for (size_t i = 0; i < m_Contexts.size(); i++)
	{
		if ((m_Contexts[i]->GetHash() == hash)
			&& (strcmp(m_Contexts[i]->GetName(), name) == 0))
		{
			index = i;
			return true;
		}
	}

	return false;
}

bool Gui::FindContext(const char* name) const
{
	size_t index;

	return FindContextIndex(name, index);
}

bool Gui::AddContext(const PERSISTID& id)
{
	IControl* pControl = (IControl*)GetCore()->GetEntity(id);

	if (NULL == pControl)
	{
		CORE_TRACE("(Gui::AddContext)entity not exists");
		return false;
	}

	if (!pControl->GetEntInfo()->IsKindOf("IControl"))
	{
		CORE_TRACE("(Gui::AddContext)class not match");
		return false;
	}

	size_t index;

	if (FindContextIndex(pControl->GetName(), index))
	{
		m_Contexts[index]->Release();
		m_Contexts[index] = pControl;
		return true;
	}

	m_Contexts.push_back(pControl);

	return true;
}

bool Gui::TrackPopupMenu(const PERSISTID& id, int x, int y)
{
	IControl* pMenu = (IControl*)GetCore()->GetEntity(id);

	if (NULL == pMenu)
	{
		CORE_TRACE("(Gui::TrackPopupMenu)entity not exists");
		return false;
	}

	if (!pMenu->GetEntInfo()->IsKindOf("Menu"))
	{
		CORE_TRACE("(Gui::TrackPopupMenu)class not a menu");
		return false;
	}

	return ShowPopupMenu(pMenu, x, y);
}

PERSISTID Gui::GetContext(const char* name) const
{
	size_t index;

	if (!FindContextIndex(name, index))
	{
		return PERSISTID();
	}

	return m_Contexts[index]->GetID();
}

// 设计信息

IDesignInfo* Gui::GetDesignInfo(IControl* pControl)
{
	Assert(pControl != NULL);

	const char* name = pControl->GetEntInfo()->GetEntityName();
	IDesignInfo* des_info;

	if (!m_DesInfo.GetData(name, des_info))
	{	
		des_info = CORE_NEW(CDesignInfo);
		pControl->PrepareDesignInfo(des_info);
		m_DesInfo.Add(name, des_info);
	}

	return des_info;
}

void Gui::GetCursorClientPos(const IVarList& args, IVarList& result)
{
	result << m_nMouseClientX << m_nMouseClientY;
}

// 获取当前鼠标相对时界面宽高的位置
void Gui::GetCursorPosition(const IVarList& args, IVarList& result)
{
	point_t pos;

	if (GetCursorCoord(pos))
	{
		result << pos.x << pos.y;
	}
}

// 获取当前鼠标相对时D3D设备宽高的位置
void Gui::GetCursor3DPosition(const IVarList& args, IVarList& result)
{
	point_t pos;

	if (GetCursor3DCoord(pos))
	{
		result << pos.x << pos.y;
	}
}

bool Gui::GetCursorCoord(point_t& pos)
{
	//if (GetCursorPos(&pos))
	//{
	//    ScreenToClient((HWND)GetWinHandle(), &pos);
	pos.x = m_nMouseClientX;
	pos.y = m_nMouseClientY;

	/*if (pos.x != m_nMouseX || pos.y != m_nMouseY)
	{
	//return true;
	}*/

	if (m_pPainter->GetNeedScaleX())
	{
		pos.x = int(pos.x * m_pPainter->GetScaleFactorX());
	}

	if (m_pPainter->GetNeedScaleY())
	{
		pos.y = int(pos.y * m_pPainter->GetScaleFactorY());
	}

	return true;
	//}

	//return false;
}

bool Gui::GetCursor3DCoord(point_t& pos)
{
	pos.x = m_nMouseClientX;
	pos.y = m_nMouseClientY;

	int width = m_pPainter->GetWinWidth();
	int height = m_pPainter->GetWinHeight();

	if (width == 0 || height == 0)
	{
		pos.x = pos.y = 0;
		return true;
	}

	pos.x = int(pos.x * m_pPainter->GetDeviceWidth() / width);
	pos.y = int(pos.y * m_pPainter->GetDeviceHeight() / height);

	return true;
}

//void Gui::GetCursorPositionInChildControl(const IVarList& args, 
//										  IVarList& result)
//{
//	point_t pos;
//
//	if (this->GetCursorCoordInChildControl(pos))
//	{
//		result.AddInt(pos.x);
//		result.AddInt(pos.y);
//	}
//}
//
//bool Gui::GetCursorCoordInChildControl(point_t& pos)
//{
//	//if (GetCursorPos(&pos))
//	//{
//	//ScreenToClient((HWND)GetWinHandle(), &pos);
//	pos.x = m_nMouseClientX;
//	pos.y = m_nMouseClientY;
//
//	if (m_pPainter->GetNeedScaleX())
//	{
//		pos.x = int(pos.x * m_pPainter->GetScaleFactorX());
//	}
//
//	if (m_pPainter->GetNeedScaleY())
//	{
//		pos.y = int(pos.y * m_pPainter->GetScaleFactorY());
//	}
//
//	// 直接通过ID获取当前控件
//	IControl* pControl = (IControl*)GetCore()->GetEntity(m_ClickedID);
//
//	if (pControl == NULL)
//	{
//		int x = int(pos.x);
//		int y = int(pos.y);
//
//		// 当前的模式窗口
//		IControl* pModalForm = GetModalForm();
//
//		if (NULL == pModalForm)
//		{
//			return false;
//		}
//
//		if (!pModalForm->InRectangle(x, y))
//		{
//			return true;
//		}
//
//		IControl* control = pModalForm->GetInWhat(x, y);
//
//		if (control != NULL)
//		{
//			IControl* master = control;
//
//			// 非附属控件
//			while (master->GetDelegateControl() != NULL)
//			{
//				master = master->GetDelegateControl();
//			}
//		}
//
//		while (control != NULL)
//		{
//			// 控件是否可以被选择
//			if (control->GetCanSelect())
//			{
//				break;
//			}
//
//			control = control->GetParentControl();
//		}
//
//		if (control != NULL)
//		{
//			IControl* can_focus = control;
//
//			if (!can_focus->GetCanFocus())
//			{
//				// 可以获得输入焦点的控件
//				while (can_focus->GetDelegateControl() != NULL)
//				{
//					can_focus = can_focus->GetDelegateControl();
//
//					if (can_focus->GetCanFocus())
//					{
//						break;
//					}
//				}
//			}
//
//			pControl = control;
//		}
//	}
//
//	if (pControl)
//	{
//		int x = pControl->GetAbsLeft();
//		int y = pControl->GetAbsTop();
//
//		pos.x -= x;
//		pos.y -= y;
//	}
//
//	return true;
//	//}
//
//	//return false;
//}

//bool Gui::ClearRenderInfo()
//{
//	m_RenderInfo.Clear();
//
//	return true;
//}

//void Gui::AddRenderInfo(const IVarList& args, IVarList& result)
//{
//	for (size_t i = 0; i < args.GetCount(); ++i)
//	{
//		if (VTYPE_STRING == args.GetType(i))
//		{
//			m_RenderInfo.AddName(args.StringVal(i));
//		}
//	}
//
//	result.AddInt((int)m_RenderInfo.GetCount());
//}

//void Gui::GetPrimitiveInfo(const IVarList& args, IVarList& result)
//{
//	result.Clear();
//
//	const dx_primitive_info_t& pi = m_pPainter->GetPrimitiveInfo();
//
//	// DP DIP
//	result.AddInt(pi.nDrawPrimitiveTimes);
//	result.AddInt(pi.nDrawIndexedPrimitiveTimes);
//	// PL LL LS TL TS
//	result.AddInt(pi.nPrimitivePointListCount);
//	result.AddInt(pi.nPrimitiveLineListCount);
//	result.AddInt(pi.nPrimitiveLineStripCount);
//	result.AddInt(pi.nPrimitiveTriangleListCount);
//	result.AddInt(pi.nPrimitiveTriangleStripCount);
//}

bool Gui::GuiHasKeyMsg()
{
	IControl* pControl = GetFocusControl();

	if (NULL == pControl)
	{
		return false;
	}

	if (pControl == m_pDesktop)
	{
		return false;
	}

	IEntInfo* pEntInfo = pControl->GetEntInfo();

	if (pEntInfo->IsKindOf("RichEdit") ||
		pEntInfo->IsKindOf("RichInputBox") ||
		pEntInfo->IsKindOf("Edit"))
	{
		return true;
	}

	return false;
}

bool Gui::ClearCapture()
{
	// 为了避免控件被删除后GUI产生的BUG,在控件删除时立即清空
	// 不然会在其它地方没用到,导致崩溃
	m_CaptureID = PERSISTID();

	return true;
}

void Gui::GetAllFormList(const IVarList& args, IVarList& result)
{
	size_t count = g_pCore->LookupEntityMore("Form", result);
}

void Gui::SetGuiHook(IGuiHook* value)
{
	SAFE_RELEASE(m_pGuiHook);
	m_pGuiHook = value;
}

// 嵌入3D场景对象 add by zhangbj
bool Gui::FindScene(const PERSISTID& id)
{
	for (size_t k = 0; k < m_Scenes.size(); ++k)
	{
		if (m_Scenes[k] == id)
		{
			return true;
		}
	}

	return false;
}

//
bool Gui::AddScene(const PERSISTID& id)
{
	if (FindScene(id))
	{
		return false;
	}

	m_Scenes.push_back(id);

	return true;
}

//
bool Gui::RemoveScene(const PERSISTID& id)
{
	for (size_t k = 0; k < m_Scenes.size(); ++k)
	{
		if (m_Scenes[k] == id)
		{
			m_Scenes.remove(k);
			return true;
		}
	}

	return false;
}

//
AnimationManger* Gui::InnerGetAnimationManger() const
{
	return (AnimationManger*)GetCore()->GetEntity(m_AnimMngID);
}