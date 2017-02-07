//--------------------------------------------------------------------
// �ļ���:		i_scrollable.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��16��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _I_SCROLLABLE_H
#define _I_SCROLLABLE_H

#include "i_control.h"

// ��������Ŀؼ�����

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
    // �����������Ϣ
    virtual void PrepareDesignInfo(IDesignInfo* di);

	// �Ƿ�������
	virtual bool GetLoadFinish();

	virtual int OnMouseWheel(int x, int y, int delta);
	virtual int OnWidthChanged(int old);
	virtual int OnHeightChanged(int old);
	virtual int OnSuborEvent(IControl* subor, 
		int event, const IVarList& args);

	// �Ƿ�ѡ�й�����
	IControl* OnGetScrollBar(int x, int y);
	
	// ��ø����ؼ�
	PERSISTID GetHScrollBar() const;
	PERSISTID GetVScrollBar() const;
	PERSISTID GetCornerLabel() const;

	// �������Ĵ�С
	void SetScrollSize(int value);
	int GetScrollSize() const;

	// �Ƿ�Ҫ�����ӿؼ�
	void SetClipChildren(bool value) { m_bClipChildren = value; }
	bool GetClipChildren() const { return m_bClipChildren; }

	// ������ʾ��ֱ������
	void SetAlwaysVScroll(bool value);
	bool GetAlwaysVScroll() const;

	// ������ʾˮƽ������
	void SetAlwaysHScroll(bool value);
	bool GetAlwaysHScroll() const;

	// �Ƿ��д�ֱ������
	void SetHasVScroll(bool value);
	bool GetHasVScroll() const;

	// �Ƿ���ˮƽ������
	void SetHasHScroll(bool value);
	bool GetHasHScroll() const;

	// ��ֱ�������Ƿ������
	void SetVScrollLeft(bool value);
	bool GetVScrollLeft() const;

	// �Ƿ����ع�����
	void SetHideScroll(bool value);
	bool GetHideScroll() const;

	// �Ƿ���䱳��ɫ
	void SetSolid(bool value);
	bool GetSolid() const;

	// ��ʼ���¹��������ݣ��˺���޸Ĳ�ˢ�¹�����
	bool BeginUpdate();
	// �������¹��������ݣ���ˢ�¹�����
	bool EndUpdate();

protected:
	// ���ݿ����ʼ����λ��
	int GetContentLeft() const;
	int GetContentTop() const;
	// ���ݿ�Ĵ�С
	int GetContentWidth() const;
	int GetContentHeight() const;

	// ���¹�����
	bool UpdateScroll();

    virtual bool RefreshInnerAbsPos();

private:
	bool VScrollWillVisible() const;
	bool HScrollWillVisible() const;
	
	unsigned int CalcScrollVisible() const;

private:
	// ģ�巽��
	// ��ֱ����������
	virtual int GetVerticalTotal() const { return 10; }
	// ������ʾ������
	virtual int GetVerticalCanDisp() const { return 10; }
	// ���õ�ǰֵ
	virtual void SetVerticalValue(int value) {}
	// ��ȡ��ǰֵ
	virtual int GetVerticalValue() const { return 0; }
	// ˮƽ������	
	virtual int GetHorizonTotal() const { return 10; }
	virtual int GetHorizonCanDisp() const { return 10; }
	virtual void SetHorizonValue(int value) {}
	virtual int GetHorizonValue() const { return 0; }
	// �����໭��������
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

