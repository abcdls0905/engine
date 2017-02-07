//--------------------------------------------------------------------
// 文件名:		track_bar.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月15日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TRACK_BAR_H
#define _TRACK_BAR_H

#include "i_control.h"

// 数量调节

class TrackBar: public IControl
{
private:
	enum ORIENT_TYPE
	{
		ORIENT_VERTICAL,	// 垂直方向
		ORIENT_HORIZON,		// 水平方向
	};

	enum { MIN_TRACK_SIZE = 8 };

public:
	TrackBar();
	virtual ~TrackBar();

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

	// 得到设置TrackBar的类型
	void SetOrient(const char* value);
	const char* GetOrient() const;

	// 得到或设置最小值
	void SetMinimum(int value);
	int GetMinimum() const;

	// 得到或设置最大值
	void SetMaximum(int value);
	int GetMaximum() const;
	
	// 得到或设置当前值
	void SetCurValue(int value);
	int GetCurValue() const;
	
	// 增加值
	void IncValue(int value);
	
private:
	void SwapWidthHeight();
	void CalcSuborRect();

	// 规范数值
	int NormalizeValue(int value) const
	{
		if (value < m_nMinimum)
		{
			return m_nMinimum;
		}

		if (value > m_nMaximum)
		{
			return m_nMaximum;
		}

		return value;
	}

protected:
    virtual bool RefreshInnerAbsPos();

private:
	IControl* m_pTrackBtn;
	int m_nOrient;
	int m_nMinimum;
	int m_nMaximum;
	int m_nValue;
};

#endif // _TRACK_BAR_H
