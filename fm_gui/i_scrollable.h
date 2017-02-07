//--------------------------------------------------------------------
// 文件名:		i_scrollable.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _I_SCROLLABLE_H
#define _I_SCROLLABLE_H

#include "i_control.h"

// 带滚动框的控件基类

class ScrollBar;
class Label;

class IScrollable: public IControl
{
private:
	enum { DEF_SCROLL_SIZE = 17 };

public:
	IScrollable();
	virtual ~IScrollable();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void Paint(float seconds);
    // 设置设计器信息
    virtual void PrepareDesignInfo(IDesignInfo* di);

	// 是否加载完成
	virtual bool GetLoadFinish();

	virtual int OnMouseWheel(int x, int y, int delta);
	virtual int OnWidthChanged(int old);
	virtual int OnHeightChanged(int old);
	virtual int OnSuborEvent(IControl* subor, 
		int event, const IVarList& args);

	// 是否选中滚动条
	IControl* OnGetScrollBar(int x, int y);
	
	// 获得附属控件
	PERSISTID GetHScrollBar() const;
	PERSISTID GetVScrollBar() const;
	PERSISTID GetCornerLabel() const;

	// 滚动条的大小
	void SetScrollSize(int value);
	int GetScrollSize() const;

	// 是否要剪裁子控件
	void SetClipChildren(bool value) { m_bClipChildren = value; }
	bool GetClipChildren() const { return m_bClipChildren; }

	// 永久显示垂直滚动条
	void SetAlwaysVScroll(bool value);
	bool GetAlwaysVScroll() const;

	// 永久显示水平滚动条
	void SetAlwaysHScroll(bool value);
	bool GetAlwaysHScroll() const;

	// 是否有垂直滚动条
	void SetHasVScroll(bool value);
	bool GetHasVScroll() const;

	// 是否有水平滚动条
	void SetHasHScroll(bool value);
	bool GetHasHScroll() const;

	// 垂直滚动条是否在左边
	void SetVScrollLeft(bool value);
	bool GetVScrollLeft() const;

	// 是否隐藏滚动条
	void SetHideScroll(bool value);
	bool GetHideScroll() const;

	// 是否填充背景色
	void SetSolid(bool value);
	bool GetSolid() const;

	// 开始更新滚动框内容，此后的修改不刷新滚动条
	bool BeginUpdate();
	// 结束更新滚动框内容，并刷新滚动条
	bool EndUpdate();

protected:
	// 内容框的起始绝对位置
	int GetContentLeft() const;
	int GetContentTop() const;
	// 内容框的大小
	int GetContentWidth() const;
	int GetContentHeight() const;

	// 更新滚动条
	bool UpdateScroll();

    virtual bool RefreshInnerAbsPos();

private:
	bool VScrollWillVisible() const;
	bool HScrollWillVisible() const;
	
	unsigned int CalcScrollVisible() const;

private:
	// 模板方法
	// 垂直滚动总数量
	virtual int GetVerticalTotal() const { return 10; }
	// 可以显示的数量
	virtual int GetVerticalCanDisp() const { return 10; }
	// 设置当前值
	virtual void SetVerticalValue(int value) {}
	// 获取当前值
	virtual int GetVerticalValue() const { return 0; }
	// 水平滚动条	
	virtual int GetHorizonTotal() const { return 10; }
	virtual int GetHorizonCanDisp() const { return 10; }
	virtual void SetHorizonValue(int value) {}
	virtual int GetHorizonValue() const { return 0; }
	// 派生类画滚动内容
	virtual void PaintScrollRect(float seconds) {}

public:
	ScrollBar* m_pVScrollBar;
	ScrollBar* m_pHScrollBar;
	
private:
	Label* m_pCornerLabel;
	int m_nScrollSize;
	bool m_bClipChildren;
	bool m_bAlwaysVScroll;
	bool m_bAlwaysHScroll;
	bool m_bHasVScroll;
	bool m_bHasHScroll;
	bool m_bVScrollLeft;
	bool m_bHideScroll;
	bool m_bSolid;
	bool m_bInUpdate;
};

#endif // _I_SCROLLABLE_H

