//--------------------------------------------------------------------
// 文件名:		combo_box.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _COMBO_BOX_H
#define _COMBO_BOX_H

#include "i_control.h"

// 下拉选择框

class Edit;
class Button;
class ListBox;

class ComboBox: public IControl
{
public:
	ComboBox();
	virtual ~ComboBox();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual int GetMinWidth() const { return 40; }
	virtual int GetMinHeight() const { return 8; }
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// 是否加载完成
	virtual bool GetLoadFinish();

	// 文字
	virtual void SetText(const wchar_t* val);
	virtual const wchar_t* GetText() const;

	virtual int OnWidthChanged(int old);
	virtual int OnHeightChanged(int old);
	
	virtual int OnSuborEvent(IControl* subor, 
		int event, const IVarList& args);

	virtual bool RemoveTopLayer(const PERSISTID& id);

	// 得到三个附属控件
	PERSISTID GetInputEdit() const;
	PERSISTID GetDropButton() const;
	PERSISTID GetDropListBox() const;

	// 下拉按钮的宽度
	void SetButtonWidth(int value);
	int GetButtonWidth() const;

	// 内部列表子控件宽度
	void SetDropDownWidth(int val);
	int GetDropDownWidth() const;

	// 内部列表子控件高度
	void SetDropDownHeight(int val);
	int GetDropDownHeight() const;

	// 设置仅下拉选择属性
	void SetOnlySelect(bool val);
	bool GetOnlySelect() const;

	// 下拉列表是否显示
	void SetDroppedDown(bool val);
	bool GetDroppedDown() const;

protected:
    virtual bool RefreshInnerAbsPos();

private:
	void CalcSuborRect();
	void CalcDropDownTop();
	void CalcDropDownWidth();

private:
	Edit* m_pInputEdit;
	Button* m_pDropButton;
	ListBox* m_pDropListBox;
	bool m_bDroppedDown;
	int m_nDropDownWidth;
	int m_nButtonWidth;
};

#endif // _COMBO_BOX_H
