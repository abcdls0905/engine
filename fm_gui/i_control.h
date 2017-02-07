//--------------------------------------------------------------------
// 文件名:		i_control.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _I_CONTROL_H
#define _I_CONTROL_H

#include "../public/module.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../public/var.h"
#include "../public/var_list.h"
#include "../utils/array_pod.h"
#include "../utils/string_alias.h"
#include "../visual/i_painter.h"
#include "gui_const.h"
#include "gui_utils.h"
#include "color_data.h"
#include "image_player.h"
#include "i_design_info.h"
#include "gui.h"

#define DECLARE_EVENT(p)


// 界面控件基类

class IGuiHook;

class IControl: public IEntity
{
public:
	// 资源加载模式
	enum LOAD_MODE_ENUM 
	{
		LOADMODE_DEFAULT,	// 与父控件相同或同步加载
		LOADMODE_ASYNC,		// 异步加载
		LOADMODE_SYNC,		// 同步加载
	};

//private:
//	typedef TStringAlias<char, char, TStringTraits<char>,
//		TStringTraits<char>, TCoreAlloc> callback_container_t;

public:
	IControl();
	virtual ~IControl() = 0;

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
		
	// 绘制控件
	virtual void Paint(float seconds);
	
	// 设置设计器信息
	virtual void PrepareDesignInfo(IDesignInfo* di);

	// 是否加载完成
	virtual bool GetLoadFinish();
    
	bool GetLoadFinishValue() { return m_bLoadFinish; }
	
	// 标题
	virtual void SetText(const wchar_t* val);
	virtual const wchar_t* GetText() const;

    // 获取显示文本
	const wchar_t* GetShowText() const { return m_wsShowText.c_str(); }

    // 获取光标位置
    virtual point_t GetCaretPos();

	// 输入光标名称
	virtual void SetCaretName(const char* name) {}
	virtual const char* GetCaretName() const { return ""; }
	
    // 数据源配置
    void SetDataSource(const char* source);
    const char* GetDataSource() const;

	// 最小尺寸
	virtual int GetMinWidth() const { return 4; }
	virtual int GetMinHeight() const { return 4; }
	
	// 是否需要输入TAB键
	virtual bool GetAllowTab() const { return false; }
	// 是否需要输入回车键
	virtual bool GetAllowReturn() const { return false; }

	// 是否容器
	virtual bool IsContainer() const { return false; }
	// 用控件名查找
	virtual IControl* FindChild(const char* name) const { return NULL; }
	// 用控件名从本节点开始进行递归查找，包扩本节点
	virtual IControl* QueryControl(const char* name);
	// 添加子控件
	virtual bool AddChild(IControl* child) { return false; }
	// 在指定控件之后插入
	virtual bool InsertAfterChild(IControl* child, 
		IControl* after) { return false; }
	// 在指定控件之前插入
	virtual bool InsertBeforeChild(IControl* child, 
		IControl* before) { return false; }
	// 移除子控件
	virtual bool RemoveChild(IControl* child) { return false; }
    virtual bool RemoveChildNoEvent(IControl* child) { return false;}
	// 清空子控件
	virtual void ClearChild() {}
	// 删除所有子控件
	virtual void DeleteAllChild() {}
	// 将子控件放到最后（ZOrder）
	virtual bool SendToBack(IControl* child) { return false; }
	// 将子控件放到最前（ZOrder）
	virtual bool BringToFront(IControl* child) { return false; }
	// 是否子控件
	virtual bool IsChildControl(IControl* child) const { return false; }
	// 获得子控件数量
	virtual size_t GetChildCount() const { return 0; }
	// 获得指定位置的子控件
	virtual IControl* GetChildByIndex(size_t index) const { return NULL; }
	// 获得子控件ID列表
	virtual size_t GetChildList(IVarList& result) const { return 0; }
	// 需要显示在最上层的控件
	virtual bool FindTopLayer(const PERSISTID& id) { return false; }
	virtual bool AddTopLayer(const PERSISTID& id) { return false; }
	virtual bool RemoveTopLayer(const PERSISTID& id) { return false; }
    // 删除所有最上层的控件
    virtual void DeleteAllTopLayer() {}
	// 回调
	// 键按下
	virtual int OnKeyDown(unsigned int vk);
	// 键抬起
	virtual int OnKeyUp(unsigned int vk);
	// 鼠标左键按下
	virtual int OnLButtonDown(int x, int y);
	// 鼠标左键抬起
	virtual int OnLButtonUp(int x, int y);
	// 鼠标左键双击
	virtual int OnLButtonDoubleClick(int x, int y);
	// 鼠标左键持续按下
	virtual int OnLButtonHover(int x, int y);
	// 鼠标右键按下
	virtual int OnRButtonDown(int x, int y);
	// 鼠标右键抬起
	virtual int OnRButtonUp(int x, int y);
	// 鼠标右键双击
	virtual int OnRButtonDoubleClick(int x, int y);
	// 鼠标右键持续按下
	virtual int OnRButtonHover(int x, int y);
	// 鼠标滚轮
	virtual int OnMouseWheel(int x, int y, int delta);
	// 鼠标移动
	virtual int OnMouseMove(int x, int y);
	// 拖动
	virtual int OnDragMove(int x, int y);
	// 拖动放入
	virtual int OnDropIn(int x, int y);
	// 结束拖动
	virtual int OnDragLeave(int x, int y);
	// 输入字符
	virtual int OnChar(unsigned int ch);
	// 获得输入焦点
	virtual int OnGetFocus(const PERSISTID& lost_id);
	// 失去输入焦点
	virtual int OnLostFocus(const PERSISTID& get_id);
	// 添加子控件
	virtual int OnAdd(const PERSISTID& child_id);
	// 移除子控件
	virtual int OnRemove(const PERSISTID& child_id);
	// 进入父控件
	virtual int OnEnter(const PERSISTID& parent_id);
	// 离开父控件
	virtual int OnLeave(const PERSISTID& parent_id);
	// 获得鼠标焦点
	virtual int OnGetCapture(const PERSISTID& lost_id);
	// 失去鼠标焦点
	virtual int OnLostCapture(const PERSISTID& get_id);
	// 附属控件事件
	virtual int OnSuborEvent(IControl* subor, 
		int event, const IVarList& args);
	// 左上角坐标改变
	virtual int OnLeftChanged(int old);
	virtual int OnTopChanged(int old);
	// 尺寸改变
	virtual int OnWidthChanged(int old);
	virtual int OnHeightChanged(int old);
	// 设置浮动提示
	virtual int OnSetHint(IControl* hint, int x, int y);
	// 设置上下文菜单
	virtual int OnSetContext(IControl* context);
	// 设置鼠标形状
	virtual int OnSetCursor(int x, int y);
	// 获得坐标位于哪个控件中
	virtual IControl* OnGetInWhat(int x, int y);
    // 判断指定位置是否透明
    virtual bool TestTransparent(int x, int y);

	// 控件名
	void SetName(const char* val);
	const char* GetName() const { return m_strName.c_str(); }
	unsigned int GetHash() const { return m_nHash; }
	
	// 相对于父容器左边缘的距离
	void SetLeft(int val);
	int GetLeft() const { return m_nLeft; }

	// 相对于父容器上边缘的距离
	void SetTop(int val);
	int GetTop() const { return m_nTop; }
	
	// 控件宽度
	int GetWidth() const { return m_nWidth; }
	void SetWidth(int val);

	// 控件高度
	int GetHeight() const { return m_nHeight; }
	void SetHeight(int val);

	// 考虑锚定后的实际绝对坐标位置
	void SetAbsLeft(int val);
	int GetAbsLeft() const;
	void SetAbsTop(int val);
	int GetAbsTop() const;

	// 前景色,字体色
	void SetForeColor(unsigned int val);
	unsigned int GetForeColor() const { return m_ForeColor.GetValue(); }

	// 背景色
	void SetBackColor(unsigned int val);
	unsigned int GetBackColor() const { return m_BackColor.GetValue(); }

	// 线条色
	void SetLineColor(unsigned int val);
	unsigned int GetLineColor() const { return m_LineColor.GetValue(); }

	// 混色
	void SetBlendColor(unsigned int val);
	unsigned int GetBlendColor() const { return m_BlendColor.GetValue(); }

	// 字体阴影色/描边色
	void SetShadowColor(unsigned int val);
	unsigned int GetShadowColor() const { return m_ShadowColor.GetValue(); }

	// 允许/禁止
	void SetEnabled(bool val) { m_bEnabled = val; }
	bool GetEnabled() const { return m_bEnabled; }

	// 是否显示
	virtual void SetVisible(bool val) { m_bVisible = val; }
	bool GetVisible() const { return m_bVisible; }

	// 是否可以接受输入
	void SetCanFocus(bool val) { m_bCanFocus = val; }
	bool GetCanFocus() const { return m_bCanFocus; }

	//是否支持输入法输入
	virtual bool GetCanIMEFocus() const { return false; }
	
	// 是否可被选择
	void SetCanSelect(bool val) { m_bCanSelect = val; }
	bool GetCanSelect() const { return m_bCanSelect; }
	
	// 当前是否拥有输入焦点
	void SetFocused(bool val) { m_bFocused = val; }
	bool GetFocused() const { return m_bFocused; } 

	// 是否已捕获鼠标
	void SetCapture(bool val) { m_bCapture = val; }
	bool GetCapture() const { return m_bCapture; }

	// 字体名
	virtual void SetFontName(const char* val);
	const char* GetFontName() const { return m_strFontName.c_str(); }

	// 光标名
	void SetCursorName(const char* val);
	const char* GetCursorName() const { return m_strCursorName.c_str(); }
	
	// 浮动提示文字
	void SetHintText(const wchar_t* value);
	const wchar_t* GetHintText() const { return m_wsHintText.c_str(); }
	 // 获取转换后的,因为HintText本身可能是个IDNAME
    const wchar_t* GetRealHintText() const { return m_wsRealHintText.c_str(); }
	
	// 浮动提示类型
	void SetHintType(const char* value);
	const char* GetHintType() const { return m_strHintType.c_str(); }

	// 上下文菜单
	void SetContext(const char* value);
	const char* GetContext() const { return m_strContext.c_str(); }

	// 关联数据对象
	void SetDataID(const PERSISTID& value);
	PERSISTID GetDataID() const;

	// 是否异步加载
	void SetAsyncLoad(bool value);
	bool GetAsyncLoad() const;

	// TAB键移动的顺序
	void SetTabIndex(int value) { m_nTabIndex = value; }
	int GetTabIndex() const { return m_nTabIndex; }
	
	// 是否可用TAB将输入焦点移动到此控件
	void SetTabStop(bool value) { m_bTabStop = value; }
	bool GetTabStop() const { return m_bTabStop; }

	// 是否处于设计状态
	void SetDesignMode(bool value) { m_bDesignMode = value; }
	bool GetDesignMode() const { return m_bDesignMode; }

	// 是否有边框
	void SetNoFrame(bool value) { m_bNoFrame = value; }
	bool GetNoFrame() const { return m_bNoFrame; }
	
	// 是否自动调整大小
	void SetAutoSize(bool value);
	bool GetAutoSize() const;
	
	// 背景图片渲染模式
	void SetDrawModeString(const char* value);
	const char* GetDrawModeString() const;
	int GetDrawMode() const;
	
	// 背景图片
	void SetBackImage(const char* value);
	const char* GetBackImage() const { return m_BackImage.GetName(); }

	// 前景色字符串
	void SetForeColorString(const char* val);
	result_string GetForeColorString() const;

	// 背景色字符串
	void SetBackColorString(const char* val);
	result_string GetBackColorString() const;

	// 背景色字符串
	void SetBlendColorString(const char* val);
	result_string GetBlendColorString() const;

	// 字体阴影色/描边色
	void SetShadowColorString(const char* val);
	result_string GetShadowColorString() const;

    // 整个控件的透明度
    void SetAlpha(int val);
    int GetAlpha() const { return m_nAlpha; }

	// 线景色字符串
	void SetLineColorString(const char* val);
	result_string GetLineColorString() const;

	// 垂直锚定方式
	void SetVAnchorString(const char* val);
	const char* GetVAnchorString() const;

	// 水平锚定方式
	void SetHAnchorString(const char* val);
	const char* GetHAnchorString() const;

    // 透明测试,以免某些控件透明区域也响应消息
    void SetTestTrans(bool value) {	m_bTestTrans = value; }
    bool GetTestTrans() const {	return m_bTestTrans; }

	// 界面
	void SetGui(Gui* value) { m_pGui = value; }
	Gui* GetGui() const { return m_pGui; }
	
	// 父级容器
	void SetParentControl(IControl* value) { m_pParent = value; }
	IControl* GetParentControl() const { return m_pParent;	}
	
	// 窗口消息委托容器
	void SetDelegateControl(IControl* value) { m_pDelegate = value; }
	IControl* GetDelegateControl() const {	return m_pDelegate;	}

    // 鼠标移入的声音
    void SetInSound(const char* value);
    const char* GetInSound() const;

    // 鼠标移出的声音
    void SetOutSound(const char* value);
    const char* GetOutSound() const;

    // 鼠标点击的声音
    void SetClickSound(const char* value);
    const char* GetClickSound() const;

	// 是否指定对象的祖先
	bool IsAncestorOf(IControl* pControl);
	
	// 点是否在控件范围内
    bool InRectangle(int x, int y) const;
	
	// 获得点在哪个控件中
	IControl* GetInWhat(int x, int y);
	
	// 获得顶层容器（FORM）
	IControl* GetRootControl();
	// 顶级容器对象（FORM）
	PERSISTID GetRootID();

    // 获取此控件的直接父界面窗口Form
    PERSISTID GetParentFormID();

	// 父级容器对象
	PERSISTID GetParentID() const
	{
		if (NULL == m_pParent)
		{
			return PERSISTID();
		}

		return m_pParent->GetID();
	}

	// 获得顶级宿主控件
	IControl* GetMasterControl();
	// 获得顶级宿主对象
	PERSISTID GetMasterID();
	// 获得宿主对象
	PERSISTID GetDelegateID();
	// 根据对象号获得控件
	IControl* GetControl(const PERSISTID& id) const;
	
	// 背景贴图是否有效
	bool HasBackImage() { return m_BackImage.IsReady(); }
	// 释放背景贴图
	void RelaseBackImage();

	// 创建附属控件
	IControl* CreateSuborControl(IControl* parent, const char* name);
    // 删除附属控件
    bool RemoveSuborControl(IControl* pControl);

    // 重取m_nInnerAbsLeft,m_nInnerAbsTop
    void SetRefreshInnerAbsPos(bool value){ m_bRefreshInnerAbsPos = value; }
    bool GetRefreshInnerAbsPos(){ return m_bRefreshInnerAbsPos; }

	// 记录回调信息
	//bool AddCallback(const char* event, const char* func);
	//bool RemoveCallback(const char* event);
	//bool ClearCallback();
	//bool FindCallback(const char* event) const;
	//const char* GetCallbackFunc(const char* event) const;
	//size_t GetCallbackCount() const;
	//size_t GetCallbackList(IVarList& result) const;

    //是否禁用对enter的响应
    virtual void SetDisableEnter(bool value){ m_bIsDisableEnter = value; }
    virtual bool GetDisableEnter()const { return m_bIsDisableEnter; }

	// 设置界面回调
	void SetGuiHook(IGuiHook* value);
	IGuiHook* GetGuiHook() const { return m_pGuiHook; }

	// 是否对消息透明
	void SetTransparent(bool value);
	bool GetTransparent() const;

protected:
	int InnerGetAbsLeft() const { return m_nInnerAbsLeft; }
	int InnerGetAbsTop() const { return m_nInnerAbsTop; }

	int GetDrawModeByString(const char* value, 
		int nDefauleMode = CGuiConst::DRAWMODE_FITWINDOW) const;
	const char* GetStringByDrawMode(int nDrawMode)const;

	// 获得绘制器
	IPainter* GetPainter() const { return m_pPainter; }
	
	// 设置初始尺寸
	void SetDesignSize(int width, int height)
	{
        m_nWidth = width;
        m_nHeight = height;
	}

	void InnerSetLeft(int value);
	void InnerSetTop(int value);
	void InnerSetWidth(int value);
	void InnerSetHeight(int value);

	void InnerSetVAnchor(int value);
	void InnerSetHAnchor(int value);

	CImagePlayer& InnerGetBackImage() { return m_BackImage; }

	// 画背景贴图
	bool DrawBackImage(float seconds, int x1, int y1, int x2, int y2);

    virtual bool RefreshInnerAbsPos();

	// 是否对消息透明的缺省值
	void SetTransparentDefault(bool value) { m_bTransparentDefault = value; SetTransparent(value); }
	bool GetTransparentDefault() const { return m_bTransparentDefault; }

private:
    bool m_bRefreshInnerAbsPos;

protected:
    int m_nInnerAbsLeft;
    int m_nInnerAbsTop;

private:
	Gui* m_pGui;
	IPainter* m_pPainter;
	IGuiHook* m_pGuiHook;
	IControl* m_pParent;
	IControl* m_pDelegate;
	IEntity* m_pData;
	CColorData m_ForeColor;
	CColorData m_BackColor;
	CColorData m_LineColor;
	CColorData m_BlendColor;
	CColorData m_ShadowColor;
	core_string m_strName;
	unsigned int m_nHash;
	int m_nTop;
	int m_nLeft;
	int m_nWidth;
	int m_nHeight;
    int m_nAlpha;
	int m_nLoadMode;
	bool m_bDesignMode;
	bool m_bEnabled;
	bool m_bVisible;
	bool m_bCanFocus;
	bool m_bCanSelect;
	bool m_bFocused;
	bool m_bCapture;
    bool m_bLoadFinish;
    //是否禁用对enter的响应
    bool m_bIsDisableEnter;
	int m_nVAnchor;
	int m_nHAnchor;
	core_wstring m_wsText;
    core_wstring m_wsShowText;
	core_string m_strFontName;
	core_string m_strCursorName;
	core_wstring m_wsHintText;
    core_wstring m_wsRealHintText;
	core_string m_strHintType;
    core_string m_strContext;
    core_string m_strDataSource;
    core_string m_strInSound;
    core_string m_strOutSound;
    core_string m_strClickSound;
	int m_nTabIndex;
	bool m_bTabStop;
	bool m_bNoFrame;
	bool m_bAutoSize;
    bool m_bTestTrans;
	int m_nDrawMode;
	CImagePlayer m_BackImage;
	TArrayPod<IControl*, 1, TCoreAlloc> m_Subors;
	//callback_container_t* m_pCallbacks;
	// 是否透明的缺省值
	bool m_bTransparentDefault;

	// 是否透明
	bool m_bTransparent;
};

#endif // _I_CONTROL_H

