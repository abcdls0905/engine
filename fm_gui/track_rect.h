//--------------------------------------------------------------------
// 文件名:		track_rect.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年1月22日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TRACK_RECT_H
#define _TRACK_RECT_H

#include "i_control.h"

// 数值调节框

class TrackRect: public IControl
{
public:
	TrackRect();
	virtual ~TrackRect();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// 是否加载完成
	virtual bool GetLoadFinish();

	virtual int OnLButtonDown(int x, int y);
	virtual int OnWidthChanged(int old);
	virtual int OnHeightChanged(int old);
	virtual int OnSuborEvent(IControl* subor, 
		int event, const IVarList& args);

	// 得到滑块子控件
	PERSISTID GetTrackButton() const;

	// 水平方向最小值
	void SetHorizonMin(int value);
	int GetHorizonMin() const;

	// 水平方向最大值
	void SetHorizonMax(int value);
	int GetHorizonMax() const;
	
	// 水平方向当前值
	void SetHorizonValue(int value);
	int GetHorizonValue() const;
	
	// 水平方向增加值
	void IncHorizonValue(int value);
	
	// 垂直方向最小值
	void SetVerticalMin(int value);
	int GetVerticalMin() const;

	// 垂直方向最大值
	void SetVerticalMax(int value);
	int GetVerticalMax() const;
	
	// 垂直方向当前值
	void SetVerticalValue(int value);
	int GetVerticalValue() const;
	
	// 垂直方向增加值
	void IncVerticalValue(int value);

protected:
    virtual bool RefreshInnerAbsPos();

private:
	void CalcSuborRect();

private:
	IControl* m_pTrackBtn;
	int m_nHorizonMin;
	int m_nHorizonMax;
	int m_nHorizonValue;
	int m_nVerticalMin;
	int m_nVerticalMax;
	int m_nVerticalValue;
};

#endif // _TRACK_RECT_H
