//--------------------------------------------------------------------
// 文件名:		form.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _FORM_H
#define _FORM_H

#include "i_container.h"
#include <string>

// 窗口

class GroupBox;
class ScrollBar;
class Label;

class Form: public IContainer
{
public:
	Form();
	virtual ~Form();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// 添加子控件
	virtual bool AddChild(IControl* child);

	virtual int OnEnter(const PERSISTID& parent_id);
	virtual int OnLeave(const PERSISTID& parent_id);
	virtual int OnLButtonDown(int x, int y);
	virtual int OnMouseWheel(int x, int y, int delta);
	virtual int OnDragMove(int x, int y);
	virtual int OnDragLeave(int x, int y);
	
	virtual int OnLeftChanged(int old);
	virtual int OnTopChanged(int old);
	virtual int OnWidthChanged(int old);
	virtual int OnHeightChanged(int old);
	virtual IControl* OnGetInWhat(int x, int y);
	
	virtual int OnSuborEvent(IControl* subor, int event, const IVarList& args);
	
	// 是否模式窗口
	bool GetIsModal() const;

	// 是否是主窗口
	void SetIsDesktop(bool value);
	bool GetIsDesktop() const;
	
	// 是否固定
	void SetFixed(bool value);
	bool GetFixed() const;

	// 是否透明
	void SetTransparent(bool value);
	bool GetTransparent() const;

	// 是否要剪裁子控件
	void SetClipChildren(bool value) { m_bClipChildren = value; }
	bool GetClipChildren() const { return m_bClipChildren; }

	// 是否限制任何部分不可拖动超出屏幕范围
	void SetLimitInScreen(bool value);
	bool GetLimitInScreen() const;
	
	// 显示辅助编辑的网格
	void SetShowGrid(bool value);
	bool GetShowGrid() const;

	// 显示事件是否回调
	void SetVisibleEvent(bool value);
	bool GetVisibleEvent() const;

	// 网格的尺寸
	void SetGridSize(int value);
	int GetGridSize() const;

	// 主菜单对象号
	void SetMainMenuID(const PERSISTID& value);
	PERSISTID GetMainMenuID() const;

	// 缺省执行控件对象号
	void SetDefaultID(const PERSISTID& value);
	PERSISTID GetDefaultID() const;

	// 获得附属控件
	PERSISTID GetPanel() const;
	PERSISTID GetHScrollBar() const;
	PERSISTID GetVScrollBar() const;
	PERSISTID GetCornerLabel() const;

	// 创建可滚动面板
	PERSISTID CreatePanel(int width, int height);
	// 重新设置滚动面板尺寸
	bool SetPanelSize(int width, int height);
	
	// 显示窗口
	bool Show();
	// 模态显示
	bool ShowModal();
	// 关闭窗口
	bool Close();

	// 获得下一个拥有输入焦点的控件
	IControl* GetNextFocus(IControl* focus);

	virtual void SetVisible(bool val);
	void SetFormFile(const char* strFormName);
	const char* GetFormFile();

	// 窗体名称
	void SetFormName(const char* strFormName);
	const char* GetFormName();

private:
	void PaintBackground(float seconds);

	// 内容框的起始绝对位置
	int GetContentLeft() const;
	int GetContentTop() const;
	// 内容框的大小
	int GetContentWidth() const;
	int GetContentHeight() const;
	
	unsigned int CalcScrollVisible() const;
	void CalcSuborRect();

	// 更新滚动条
	bool UpdateScroll();
	
	// 垂直滚动总数量
	int GetVerticalTotal() const;
	// 可以显示的数量
	int GetVerticalCanDisp() const;
	// 设置当前值
	void SetVerticalValue(int value);
	// 获取当前值
	int GetVerticalValue() const;
	// 水平滚动条	
	int GetHorizonTotal() const;
	int GetHorizonCanDisp() const;
	void SetHorizonValue(int value);
	int GetHorizonValue() const;
private:
	bool m_bIsDesktop;
	bool m_bFixed;
	bool m_bTransparent;
	bool m_bClipChildren;
	bool m_bLimitInScreen;
	int m_nClickX;
	int m_nClickY;
	int m_nGridSize;
	bool m_bShowGrid;
	bool m_bIsModal;
    bool m_bVisibleEvent;
	GroupBox* m_pPanel;
	ScrollBar* m_pHScrollBar;
	ScrollBar* m_pVScrollBar;
	Label* m_pCornerLabel;
	int m_nOffsetX;
	int m_nOffsetY;
	PERSISTID m_MainMenuID;
	PERSISTID m_DefaultID;

	std::string m_strFormFile;
	std::string m_strFormName;
};

#endif // _FORM_H
