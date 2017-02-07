//--------------------------------------------------------------------
// 文件名:		gui.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _GUI_H
#define _GUI_H

#include "../public/module.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "../utils/string_pod.h"
#include "../utils/string_alias.h"
#include "../utils/pod_hash_map.h"
#include "name_list.h"
#include "gui_utils.h"
#include "../visual/i_gui_hook.h"
#include "../visual/i_event_manager.h"
// 主界面

#include "../utils/lock_util.h"
#include "../utils/thread.h"

class IControl;
class IGuiHook;
class IPainter;
class IDesignInfo;
class Font;
class Caret;
class Cursor;
class Designer;
class ImageAnimationManager;
class AnimationManger;

class Gui: public IEntity
{
private:
	struct sound_data_t
	{
		core_string strSoundFile;
	};

	typedef TStringPod<char, sound_data_t*, TStringTraits<char>, 
		TCoreAlloc> sound_container_t;
	typedef TStringPod<char, IDesignInfo*, TStringTraits<char>, 
		TCoreAlloc> des_info_container_t;

	int RunCallback(const char* event, const IVarList& args)
	{
		return event_send(GetID(), event, args);
	}

	static void WorkerProc(void* pParameter);
public:
	Gui();
	virtual ~Gui();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	virtual bool MsgProc(unsigned int msg, size_t param1, size_t param2, 
		int& result);

	void Update(float seconds);
	void Realize();

	// 是否显示控件
	void SetVisible(bool value);
	bool GetVisible() const;

	// 缺省资源路径
	void SetDefaultPath(const char* value);
	const char* GetDefaultPath() const;

	// 获得绑定窗口的句柄
	//void* GetWinHandle() const;

	// 获得界面渲染器名称
	const char* GetPainterName() const;

	// 根据ID获得控件
	IControl* GetControl(const PERSISTID& id) const;

	// 添加背景控件
	bool AddBackControl(const PERSISTID& id);
	// 移除背景控件
	bool RemoveBackControl(const PERSISTID& id);

	// 添加模式窗口
	bool AddModalForm(IControl* pControl);
	// 移除模式窗口
	bool RemoveModalForm(IControl* pControl);
	// 获得当前模式窗口
	IControl* GetModalForm() const;
	// 获得当前模式窗口对象
	PERSISTID GetModalFormID() const;

	// 获得主窗口对象
	PERSISTID GetDesktopID() const;
	// 获得主窗口控件
	IControl* GetDesktopForm() const;
	// 创建主窗口
	bool CreateDesktopForm();
	// 创建选择器
	PERSISTID CreateSelector(const PERSISTID& parent_id);
	// 创建图元列表
	PERSISTID CreateImageList();

	// 创建控件
	IControl* CreateControl(const char* name);
	// 删除控件
	bool DeleteControl(IControl* pControl);

	// 检查控件是否正在显示
	bool IsControlVisible(IControl* pControl);

	// 创建
	PERSISTID Create(const char* name);
	// 删除
	bool Delete(const PERSISTID& id);

	// 键盘输入焦点
	void SetFocusControl(IControl* val);
	IControl* GetFocusControl() const;

	// 设置键盘输入焦点
	void SetFocused(const PERSISTID& id);
	// 获得键盘输入焦点
	PERSISTID GetFocused() const;

	// 获取所有Form类实体
	void GetAllFormList(const IVarList& args, IVarList& result);

	// 获得窗口加载器
	PERSISTID GetLoader() const;
	// 获得窗口设计器
	PERSISTID GetDesigner();
	// 获得当前帧数
	float GetFPS() const;

	// 获得当前帧数
	float GetAverageFPS() const;

	// 获得批处理次数
	double GetPerDrawCallNum() const;

	// 获得总三角面数
	double GetPerDrawTriangleNum() const;

	// 主窗口宽度
	int GetWidth() const;
	// 主窗口高度
	int GetHeight() const;
	// 获取设备宽度
	int GetDeviceWidth() const;
	// 获取设备高度
	int GetDeviceHeight() const;

	// 控制窗口产生鼠标持续按下消息
	//bool GenMouseHoverMessage(int delay_ms);

	// 声音资源
	bool FindSound(const char* name) const;
	bool AddSound(const char* name, const char* sound_file);
	bool GetSound(const char* name, sound_data_t& data) const;
	void ClearSound();
	bool PlayingSound(const char* name);
	const char* GetSoundFile(const char* name) const;

	// 字体
	void ClearFont();
	PERSISTID NewFont();
	bool FindFont(const char* name) const;
	bool AddDefaultFont();
	bool AddFont(const PERSISTID& id);
	bool AddReferFont(const char* name, const char* refer_name);
	PERSISTID GetFont(const char* name) const;
	bool SetFont(const char* name, unsigned int shadow_color = 0);
	// 鼠标
	void ClearCursor();
	PERSISTID NewCursor();
	bool FindCursor(const char* name) const;
	bool AddCursor(const PERSISTID& id);
	PERSISTID GetCursor(const char* name) const;
	// 设置鼠标位置
	bool SetCursorPos(int x, int y);
	// 设置当前鼠标
	bool SetCurrentCursor(const char* name);

	// 输入光标
	void ClearCaret();
	PERSISTID NewCaret();
	bool FindCaret(const char* name) const;
	bool AddCaret(const PERSISTID& id);
	PERSISTID GetCaret(const char* name) const;

	// 浮动提示
	void ClearHint();
	PERSISTID NewHint(const char* control_name);
	bool FindHint(const char* name) const;
	bool ShowHint(const PERSISTID& id) const;
	bool AddHint(const PERSISTID& id);
	PERSISTID GetHint(const char* name) const;

	// 上下文菜单
	void ClearContext();
	PERSISTID NewContext(const char* control_name);
	bool FindContext(const char* name) const;
	bool AddContext(const PERSISTID& id);
	PERSISTID GetContext(const char* name) const;

	// 显示弹出式菜单
	bool TrackPopupMenu(const PERSISTID& id, int x, int y);

	// 获得设计信息
	IDesignInfo* GetDesignInfo(IControl* pControl);

	// 获得当前捕获鼠标的控件
	PERSISTID GetCaptureID() const;

	// 维护当前被点击的控件，用于支持多点触摸
	PERSISTID GetClickedID(size_t nTouchId) const;
	void AddClickedID(size_t nTouchId, const PERSISTID& clickedId);
	void RemoveClickedID(size_t nTouchId);

	//PERSISTID GetCoolManagerID() const;

	PERSISTID GetTextManagerID() const;

	// 是否显示帧数
	void SetShowFPS(bool value);
	bool GetShowFPS() const;

	// 是否开启界面缩放
	void SetScaleEnable(bool value);
	bool GetScaleEnable() const;

	// 界面缩放比率
	void SetScaleRatio(float value);
	float GetScaleRatio() const;

	// 性能监控器
	void SetLogPerf(bool bValue);
	bool GetLogPerf() const;

	// 通过文本管理器转换文字
	bool ConvertText(const wchar_t* src, result_wstring& dst);

	// 是否显示渲染器信息
	void SetShowRenderInfo(bool value);
	bool GetShowRenderInfo() const;

	// 鼠标停止时显示浮动提示的延时时间
	void SetHintDelay(float value);
	float GetHintDelay() const;

	// 是否进行整单词换行处理
	void SetWordChangeLine(bool value);
	bool GetWordChangeLine() const;

	// 缺省鼠标样式
	void SetCursorName(const char* value);
	const char* GetCursorName() const;

	// 忽略鼠标移动
	void SetAbandomMouseMove(bool value);
	bool GetAbandomMouseMove() const;

	// 获得鼠标相对界面位置
	bool GetCursorCoord(point_t& pos);
	// 获得鼠标相对3D设备位置
	bool GetCursor3DCoord(point_t & pos);
	// 获取当前鼠标客户区位置
	void GetCursorClientPos(const IVarList& args, IVarList& result);
	// 获取当前鼠标相对时D3D设备宽高的位置
	void GetCursorPosition(const IVarList& args, IVarList& result);
	// 获取当前鼠标相对时D3D设备宽高的位置
	void GetCursor3DPosition(const IVarList& args, IVarList& result);
	//// 获得鼠标在子控件的位置
	//void GetCursorPositionInChildControl(const IVarList& args, 
	//	IVarList& result);
	//bool GetCursorCoordInChildControl(point_t& pos);
	// 输出所有字体贴图
	bool DumpAllFontTex();
	// 清理鼠标焦点
	bool ClearCapture();

	// 清空添加的显示信息
	//bool ClearRenderInfo();
	// 添加显示信息
	//void AddRenderInfo(const IVarList& args, IVarList& result);
	// 获得渲染多边形统计信息
	//void GetPrimitiveInfo(const IVarList& args, IVarList& result);

	unsigned int GetCodePage()
	{
		return m_CurCP;
	}

	// 是否界面占用按键输入
	bool GuiHasKeyMsg();

	// 设置界面回调
	void SetGuiHook(IGuiHook* value);


	// 获取动画管理器
	PERSISTID GetAnimationManagerIDNew() const;

	//获取动画管理器
	ImageAnimationManager* GetAnimationManager() const;


	// 嵌入3D场景对象 add by zhangbj 
	bool FindScene(const PERSISTID& id);
	bool AddScene(const PERSISTID& id);
	bool RemoveScene(const PERSISTID& id);
	// end

	//基于指定控件的消息
	bool MsgLButtonDownToControl(IControl* pControl, size_t param1, size_t param2, int& result);
	bool MsgMouseMoveToControl(IControl* pControl, size_t param1, size_t param2, int& result);
	bool MsgLButtonUpToControl(IControl* pControl, size_t param1, size_t param2, int& result);
	bool MsgKeyDownToControl(IControl* pControl, size_t param1, size_t param2, int& result);


private:
	bool FindFontIndex(const char* name, size_t& index) const;
	bool FindCaretIndex(const char* name, size_t& index) const;
	bool FindCursorIndex(const char* name, size_t& index) const;
	bool FindHintIndex(const char* name, size_t& index) const;
	bool FindContextIndex(const char* name, size_t& index) const;

	// 设计控件
	bool DesignControl(IControl* control, int x, int y);

	int OnDesignEntry(IControl* pControl);
	int OnDesignClick(IControl* pControl, int x, int y);
	int OnShowHint(int x, int y);
	int OnShowContext(IControl* pControl, int x, int y);

	// 获得鼠标高度
	int GetCursorHeight() const;
	// 显示控件浮动提示
	bool ShowControlHint(IControl* pControl, int x, int y);
	// 复位浮动提示状态
	bool ResetHintStatus(int x, int y);

	// 显示控件上下文菜单
	bool ShowControlContext(IControl* pControl, int x, int y);
	// 复位上下文菜单状态
	bool ResetContextStatus(int x, int y);
	// 复位主菜单状态
	bool ResetMainMenuStatus(IControl* capture);

	// 显示弹出式菜单
	bool ShowPopupMenu(IControl* pMenu, int x, int y);
	// 隐藏弹出式菜单
	bool HidePopupMenu();

	// 改变输入焦点控件
	bool ChangeFocus(IControl* get_control, IControl* lost_control);
	// 改变捕获鼠标控件
	bool ChangeCapture(IControl* get_control, IControl* lost_control);

	Caret* InnerGetCaret() const;
	Cursor* InnerGetCursor() const;
	//GameHand* InnerGetGameHand() const;

	//
	bool InnerCreateCursor(const char* name, const char* image);

	//
	AnimationManger* InnerGetAnimationManger() const;

	// 将输入Unicode字符发送到输入控件
	bool SendCharToControl(wchar_t ch);

	//基于GUI系统的消息
	bool MsgMouseMove(void* hwnd, size_t param1, size_t param2, int& result);

	bool MsgLButtonDown(void* hwnd, size_t param1, size_t param2, int& result);

	bool MsgLButtonUp(void* hwnd, size_t param1, size_t param2, int& result);

	bool MsgKeyDown(void* hwnd, size_t param1, size_t param2, int& result);

	bool MsgKeyUp(void* hwnd, size_t param1, size_t param2, int& result);

	bool MsgChar(void* hwnd, size_t param1, size_t param2, int& result);


	//   // 获取当前输入法IME设置
	//   bool LoadCurIMESetting();
	//	// 启用和禁用IME
	//	void EnableIME(HWND hwnd);
	//	void DisableIME(HWND hwnd);
	//	// 获取当前焦点控件是否是IME输入控件
	//	IControl * GetIMEInputControl(IControl* pControl);
	//	// 窗口消息处理
	//	bool MsgMouseMove(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgSetCursor(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgLButtonDown(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgLButtonUp(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgLButtonDblclk(void* hwnd, size_t param1, size_t param2, 
	//		int& result);
	//	bool MsgRButtonDown(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgRButtonUp(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgRButtonDblclk(void* hwnd, size_t param1, size_t param2, 
	//		int& result);
	//	bool MsgMouseWheel(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgMouseHover(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgSysKeyDown(void* hwnd, size_t param1, size_t param2, int& result);
	//    bool MsgInputLanguage(void* hwnd, size_t param1, size_t param2, 
	//        int& result);
	//    bool MsgIMECompositon(void* hwnd, size_t param1, size_t param2, 
	//       int& result);
	//	bool MsgKillFocus(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgSize(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgSizing(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgMove(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgMoving(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgClose(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgActivate(void* hwnd, size_t param1, size_t param2, int& result);

private:

	bool EnableLogPerf();
	void DiscardLogPerf();

	int m_nMouseClientX;
	int m_nMouseClientY;

	core_string m_strPainterName;
	core_string m_strCursorName;

	IPainter* m_pPainter;
	IGuiHook* m_pGuiHook;
	IControl* m_pDesktop;
	Designer* m_pLoader;
	Designer* m_pDesigner;

	bool m_bFocusCanInput;
	//bool m_bLastInputOpenState;
	PERSISTID m_FocusID;            // 输入焦点
	PERSISTID m_CaptureID;          // 鼠标焦点
	TPodHashMap<int, PERSISTID, TPodTraits<int>, TCoreAlloc> m_ClickedCotrols; // 按下鼠标左键选中对象，用于支持多点触摸

	unsigned int m_nTickCount;
	int m_nFrameCount;
	float m_fTimeCount;
	float m_fFPS;
	int m_nCountFPS;
	float m_fAverageFPS;
	double m_dPerDrawCallNum;
	double m_dPerDrawTriangleNum;

	double m_dEffectModelDrawCallMaxNumPreFrame;
	double m_dEffectParticleDrawCallMaxNumPreFrame;
	double m_dGuiDrawCallMaxNumPreFrame;
	double m_dMaxValTimeCount;

	bool m_bVisible;
	bool m_bShowFPS;
	bool m_bShowRenderInfo;
	bool m_bWordChangeLine;
	bool m_bAbandomMouseMove;
	//CNameList m_RenderInfo;

	bool m_bLogPerf;

	float m_fHintDelay;
	float m_fHintCount;

	//HKL m_hkl;                  // 当前键盘布局
	//DWORD m_ImeProp;            // 当前输入法属性
	//CHARSETINFO m_csInfo;       // 当前字符集信息
	unsigned int m_CurCP;       // 当前代码页

	PERSISTID m_HintID;
	PERSISTID m_ContextID;
	PERSISTID m_CursorID;
	PERSISTID m_CaretID;
	PERSISTID m_PopupMenuID;
	PERSISTID m_TextManager;
	PERSISTID m_AnimMngID;
	//PERSISTID m_CoolManagerID;

	sound_container_t m_Sounds;
	des_info_container_t m_DesInfo;

	TStringPod<char, int, TStringTraits<char>, TCoreAlloc> m_Colors;
	TStringPod<char, size_t, TStringTraits<char>, TCoreAlloc> m_FontIndex;

	TStringAlias<char, char, TStringTraits<char>, TStringTraits<char>, TCoreAlloc> m_MapReferFont;

	TArrayPod<Font*, 1, TCoreAlloc> m_Fonts;
	TArrayPod<PERSISTID, 1, TCoreAlloc> m_BackControls;
	TArrayPod<PERSISTID, 1, TCoreAlloc> m_ModalFormStack;
	TArrayPod<Cursor*, 1, TCoreAlloc> m_Cursors;
	TArrayPod<Caret*, 1, TCoreAlloc> m_Carets;
	TArrayPod<IControl*, 1, TCoreAlloc> m_Hints;
	TArrayPod<IControl*, 1, TCoreAlloc> m_Contexts;

	TArrayPod<PERSISTID, 1, TCoreAlloc> m_Scenes; // add by zhangbj

	CThread* m_pThread;
	CThreadWaiter* m_pThreadWaiter;
	static FILE* s_fp_GuiLogPerf;
	PERSISTID m_ClickedID;
};

#endif // _GUI_H
