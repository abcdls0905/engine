//--------------------------------------------------------------------
// �ļ���:		combo_box.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _COMBO_BOX_H
#define _COMBO_BOX_H

#include "i_control.h"

// ����ѡ���

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

	// �Ƿ�������
	virtual bool GetLoadFinish();

	// ����
	virtual void SetText(const wchar_t* val);
	virtual const wchar_t* GetText() const;

	virtual int OnWidthChanged(int old);
	virtual int OnHeightChanged(int old);
	
	virtual int OnSuborEvent(IControl* subor, 
		int event, const IVarList& args);

	virtual bool RemoveTopLayer(const PERSISTID& id);

	// �õ����������ؼ�
	PERSISTID GetInputEdit() const;
	PERSISTID GetDropButton() const;
	PERSISTID GetDropListBox() const;

	// ������ť�Ŀ��
	void SetButtonWidth(int value);
	int GetButtonWidth() const;

	// �ڲ��б��ӿؼ����
	void SetDropDownWidth(int val);
	int GetDropDownWidth() const;

	// �ڲ��б��ӿؼ��߶�
	void SetDropDownHeight(int val);
	int GetDropDownHeight() const;

	// ���ý�����ѡ������
	void SetOnlySelect(bool val);
	bool GetOnlySelect() const;

	// �����б��Ƿ���ʾ
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
