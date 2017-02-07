//--------------------------------------------------------------------
// 文件名:		splitter.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年4月23日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SPLITTER_H
#define _SPLITTER_H

#include "i_control.h"

// 平面分割器

class Adjuster;
class GroupBox;

class Splitter: public IControl
{
private:
	enum SPLITTER_ORIENTS
	{
		ORIENT_VERTICAL,	// 垂直分割
		ORIENT_HORIZON,		// 水平分割
	};

	enum { MIN_SPLITTER_SIZE = 16 };

public:
	Splitter();
	virtual ~Splitter();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);
	
	// 是否加载完成
	virtual bool GetLoadFinish();

	virtual int GetMinWidth() const { return MIN_SPLITTER_SIZE; }
	virtual int GetMinHeight() const { return MIN_SPLITTER_SIZE; }
	
	virtual int OnWidthChanged(int old);
	virtual int OnHeightChanged(int old);
	virtual int OnSuborEvent(IControl* subor, int event, const IVarList& args);
	
	// 获得附属控件
	PERSISTID GetAdjuster() const;
	PERSISTID GetPanel1() const;
	PERSISTID GetPanel2() const;

	// 方向
	void SetOrient(const char* value);
	const char* GetOrient() const;

	// 被分割块的最小尺寸
	void SetMinPanelSize(int value);
	int GetMinPanelSize() const;
	
	// 获得到边缘的距离
	void SetPaddingWidth(int value);
	int GetPaddingWidth() const;

	// 获得调节器的宽度
	void SetAdjusterWidth(int value);
	int GetAdjusterWidth() const;

	// 是否固定分割
	void SetAdjusterFixed(bool value);
	bool GetAdjusterFixed() const;
	
	// 鼠标是否在分割条范围内
	void SetMouseInSplitter(bool value);
	bool GetMouseInSplitter() const;

	// 重新计算分割块的大小
	bool ResetPanelSize();

private:
	// 设置附属控件布局
	void CalcSuborRect();
	// 获得被分割的尺寸
	int GetSplitSize() const;

private:
	Adjuster* m_pAdjuster;
	GroupBox* m_pPanel1;
	GroupBox* m_pPanel2;
	int m_nOrient;
	int m_nMinPanelSize;
	int m_nPaddingWidth;
	int m_nAdjusterWidth;
	bool m_bAdjusterFixed;
};

#endif // _SPLITTER_H
