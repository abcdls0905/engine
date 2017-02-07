//--------------------------------------------------------------------
// 文件名:		scroll_bar.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SCROLL_BAR_H
#define _SCROLL_BAR_H

#include "i_control.h"

// 滚动条

class Button;

class ScrollBar: public IControl
{
private:
	enum SCROLLBAR_ORIENTS
	{
		ORIENT_VERTICAL,
		ORIENT_HORIZON,
	};

	enum { MIN_SCROLLBAR_SIZE = 4 };
	enum { DEF_BUTTON_SIZE = 15 };
	enum { MIN_TRACK_SIZE = 8 };
	enum { SCROLL_BACK_COLOR = 0xFFE0E0E0 };

public:
	ScrollBar();
	virtual ~ScrollBar();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);
	
	// 是否加载完成
	virtual bool GetLoadFinish();

	virtual int GetMinWidth() const { return MIN_SCROLLBAR_SIZE; }
	virtual int GetMinHeight() const { return MIN_SCROLLBAR_SIZE; }
	
	virtual int OnWidthChanged(int old);
	virtual int OnHeightChanged(int old);
	virtual int OnSuborEvent(IControl* subor, 
		int event, const IVarList& args);

	// 获得附属控件
	PERSISTID GetDecButton() const;
	PERSISTID GetIncButton() const;
	PERSISTID GetTrackButton() const;

	// 增减按钮的大小
	void SetButtonSize(int value);
	int GetButtonSize() const;

	// 拖动按钮的大小
	void SetTrackSize(int value);
	int GetTrackSize() const;

	// 背景是整个条的范围，还是两个按钮之间的范围
	void SetFullBarBack(bool value);
	bool GetFullBarBack() const;

	// 方向
	void SetOrient(const char* value);
	const char* GetOrient() const;

	// 最小值
	void SetMinimum(int value);
	int GetMinimum() const;

	// 最大值
	void SetMaximum(int value);
	int GetMaximum() const;

	// 当前值
	void SetCurValue(int value);
	int GetCurValue() const;
	
	// 小变化单位
	void SetSmallChange(int value);
	int GetSmallChange() const;
	
	// 大变化单位（影响移动按钮的大小）
	void SetLargeChange(int value);
	int GetLargeChange() const;

	// 恢复到初始位置
	bool Reset();

	// 移动
	void IncValue(int value);

protected:
    virtual bool RefreshInnerAbsPos();
private:
	void CalcSuborRect();
	// 决定按钮形状
	void AdjustButtonStyle();
	// 交换宽和高（倒转）
	void SwapWidthHeight();

private:
	Button* m_pDecBtn;
	Button* m_pIncBtn;
	Button* m_pTrackBtn;
    bool m_bFullBarBack;
	int m_nButtonSize;
	int m_nTrackSize;
	int m_nOrient;
	int m_nMinimum;
	int m_nMaximum;
	int m_nValue;
	int m_nSmallChange;
	int m_nLargeChange;
};

#endif // _SCROLL_BAR_H
