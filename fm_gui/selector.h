//--------------------------------------------------------------------
// �ļ���:		selector.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��16��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SELECTOR_H
#define _SELECTOR_H

#include "i_control.h"

// λ�úͳߴ�ѡ���

class Button;

class Selector: public IControl
{
public:
	Selector();
	virtual ~Selector();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// �Ƿ�������
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

	// ѡ��߿�Ŀ��
	void SetEdgeSize(int value);
	int GetEdgeSize() const;
	
	// ���ӳߴ�
	void SetGridSize(int value);
	int GetGridSize() const;

	// ����Ӷ���
	void SetSnapToGrid(bool value);
	bool GetSnapToGrid() const;

	// �״̬
	void SetActive(bool value);
	bool GetActive() const;
	
	// �󶨵��ؼ�
	void SetBindID(const PERSISTID& value);
	PERSISTID GetBindID() const;

	// ͬ���߿���󶨿ؼ�����һ�£�
	bool SyncFrame();

	// ���ñ߿�����ķ�Χ���Զ�������С��
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
