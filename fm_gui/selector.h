//--------------------------------------------------------------------
// 文件名:		selector.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SELECTOR_H
#define _SELECTOR_H

#include "i_control.h"

// 位置和尺寸选择框

class Button;

class Selector: public IControl
{
public:
	Selector();
	virtual ~Selector();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// 是否加载完成
	virtual bool GetLoadFinish();

	virtual void Paint(float seconds);
	
	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonUp(int x, int y);
	virtual int OnDragMove(int x, int y);
	virtual int OnKeyDown(unsigned int vk);

	virtual int OnWidthChanged(int old);
	virtual int OnHeightChanged(int old);
	virtual int OnSuborEvent(IControl* subor, 
		int event, const IVarList& args);

	// 选择边框的宽度
	void SetEdgeSize(int value);
	int GetEdgeSize() const;
	
	// 格子尺寸
	void SetGridSize(int value);
	int GetGridSize() const;

	// 与格子对齐
	void SetSnapToGrid(bool value);
	bool GetSnapToGrid() const;

	// 活动状态
	void SetActive(bool value);
	bool GetActive() const;
	
	// 绑定到控件
	void SetBindID(const PERSISTID& value);
	PERSISTID GetBindID() const;

	// 同步边框（与绑定控件保持一致）
	bool SyncFrame();

	// 设置边框包含的范围（自动调整大小）
	bool SetFrameRegion(int left, int top, int right, int bottom);

private:
	int GetDelta() const;
	
	void CalcSuborRect();
	
	bool SetRectLeft(int value);
	bool SetRectTop(int value);
	bool SetRectWidth(int value);
	bool SetRectHeight(int value);

	bool MoveLeft(IControl* parent, 
		IControl* subor, int delta, const IVarList& args);
	bool MoveTop(IControl* parent, 
		IControl* subor, int delta, const IVarList& args);
	bool MoveRight(IControl* parent, 
		IControl* subor, int delta, const IVarList& args);
	bool MoveBottom(IControl* parent, 
		IControl* subor, int delta, const IVarList& args);
	
	int OnRectChanged();
protected:
    virtual bool RefreshInnerAbsPos();
private:
	int m_nClickX;
	int m_nClickY;
	int m_nEdgeSize;
	int m_nGridSize;
	bool m_bSnapToGrid;
	bool m_bActive;
	int m_nOldLeft;
	int m_nOldTop;
	int m_nOldWidth;
	int m_nOldHeight;
	PERSISTID m_BindID;
	Button* m_pLeftBtn;
	Button* m_pTopBtn;
	Button* m_pRightBtn;
	Button* m_pBottomBtn;
	Button* m_pLeftTopBtn;
	Button* m_pRightTopBtn;
	Button* m_pLeftBottomBtn;
	Button* m_pRightBottomBtn;
};

#endif // _SELECTOR_H
