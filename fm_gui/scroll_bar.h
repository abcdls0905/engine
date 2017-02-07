//--------------------------------------------------------------------
// �ļ���:		scroll_bar.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��16��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SCROLL_BAR_H
#define _SCROLL_BAR_H

#include "i_control.h"

// ������

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
	
	// �Ƿ�������
	virtual bool GetLoadFinish();

	virtual int GetMinWidth() const { return MIN_SCROLLBAR_SIZE; }
	virtual int GetMinHeight() const { return MIN_SCROLLBAR_SIZE; }
	
	virtual int OnWidthChanged(int old);
	virtual int OnHeightChanged(int old);
	virtual int OnSuborEvent(IControl* subor, 
		int event, const IVarList& args);

	// ��ø����ؼ�
	PERSISTID GetDecButton() const;
	PERSISTID GetIncButton() const;
	PERSISTID GetTrackButton() const;

	// ������ť�Ĵ�С
	void SetButtonSize(int value);
	int GetButtonSize() const;

	// �϶���ť�Ĵ�С
	void SetTrackSize(int value);
	int GetTrackSize() const;

	// �������������ķ�Χ������������ť֮��ķ�Χ
	void SetFullBarBack(bool value);
	bool GetFullBarBack() const;

	// ����
	void SetOrient(const char* value);
	const char* GetOrient() const;

	// ��Сֵ
	void SetMinimum(int value);
	int GetMinimum() const;

	// ���ֵ
	void SetMaximum(int value);
	int GetMaximum() const;

	// ��ǰֵ
	void SetCurValue(int value);
	int GetCurValue() const;
	
	// С�仯��λ
	void SetSmallChange(int value);
	int GetSmallChange() const;
	
	// ��仯��λ��Ӱ���ƶ���ť�Ĵ�С��
	void SetLargeChange(int value);
	int GetLargeChange() const;

	// �ָ�����ʼλ��
	bool Reset();

	// �ƶ�
	void IncValue(int value);

protected:
    virtual bool RefreshInnerAbsPos();
private:
	void CalcSuborRect();
	// ������ť��״
	void AdjustButtonStyle();
	// ������͸ߣ���ת��
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
