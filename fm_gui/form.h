//--------------------------------------------------------------------
// �ļ���:		form.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��16��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _FORM_H
#define _FORM_H

#include "i_container.h"
#include <string>

// ����

class GroupBox;
class ScrollBar;
class Label;

class Form: public IContainer
{
public:
	Form();
	virtual ~Form();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// ����ӿؼ�
	virtual bool AddChild(IControl* child);

	virtual int OnEnter(const PERSISTID& parent_id);
	virtual int OnLeave(const PERSISTID& parent_id);
	virtual int OnLButtonDown(int x, int y);
	virtual int OnMouseWheel(int x, int y, int delta);
	virtual int OnDragMove(int x, int y);
	virtual int OnDragLeave(int x, int y);
	
	virtual int OnLeftChanged(int old);
	virtual int OnTopChanged(int old);
	virtual int OnWidthChanged(int old);
	virtual int OnHeightChanged(int old);
	virtual IControl* OnGetInWhat(int x, int y);
	
	virtual int OnSuborEvent(IControl* subor, int event, const IVarList& args);
	
	// �Ƿ�ģʽ����
	bool GetIsModal() const;

	// �Ƿ���������
	void SetIsDesktop(bool value);
	bool GetIsDesktop() const;
	
	// �Ƿ�̶�
	void SetFixed(bool value);
	bool GetFixed() const;

	// �Ƿ�͸��
	void SetTransparent(bool value);
	bool GetTransparent() const;

	// �Ƿ�Ҫ�����ӿؼ�
	void SetClipChildren(bool value) { m_bClipChildren = value; }
	bool GetClipChildren() const { return m_bClipChildren; }

	// �Ƿ������κβ��ֲ����϶�������Ļ��Χ
	void SetLimitInScreen(bool value);
	bool GetLimitInScreen() const;
	
	// ��ʾ�����༭������
	void SetShowGrid(bool value);
	bool GetShowGrid() const;

	// ��ʾ�¼��Ƿ�ص�
	void SetVisibleEvent(bool value);
	bool GetVisibleEvent() const;

	// ����ĳߴ�
	void SetGridSize(int value);
	int GetGridSize() const;

	// ���˵������
	void SetMainMenuID(const PERSISTID& value);
	PERSISTID GetMainMenuID() const;

	// ȱʡִ�пؼ������
	void SetDefaultID(const PERSISTID& value);
	PERSISTID GetDefaultID() const;

	// ��ø����ؼ�
	PERSISTID GetPanel() const;
	PERSISTID GetHScrollBar() const;
	PERSISTID GetVScrollBar() const;
	PERSISTID GetCornerLabel() const;

	// �����ɹ������
	PERSISTID CreatePanel(int width, int height);
	// �������ù������ߴ�
	bool SetPanelSize(int width, int height);
	
	// ��ʾ����
	bool Show();
	// ģ̬��ʾ
	bool ShowModal();
	// �رմ���
	bool Close();

	// �����һ��ӵ�����뽹��Ŀؼ�
	IControl* GetNextFocus(IControl* focus);

	virtual void SetVisible(bool val);
	void SetFormFile(const char* strFormName);
	const char* GetFormFile();

	// ��������
	void SetFormName(const char* strFormName);
	const char* GetFormName();

private:
	void PaintBackground(float seconds);

	// ���ݿ����ʼ����λ��
	int GetContentLeft() const;
	int GetContentTop() const;
	// ���ݿ�Ĵ�С
	int GetContentWidth() const;
	int GetContentHeight() const;
	
	unsigned int CalcScrollVisible() const;
	void CalcSuborRect();

	// ���¹�����
	bool UpdateScroll();
	
	// ��ֱ����������
	int GetVerticalTotal() const;
	// ������ʾ������
	int GetVerticalCanDisp() const;
	// ���õ�ǰֵ
	void SetVerticalValue(int value);
	// ��ȡ��ǰֵ
	int GetVerticalValue() const;
	// ˮƽ������	
	int GetHorizonTotal() const;
	int GetHorizonCanDisp() const;
	void SetHorizonValue(int value);
	int GetHorizonValue() const;
private:
	bool m_bIsDesktop;
	bool m_bFixed;
	bool m_bTransparent;
	bool m_bClipChildren;
	bool m_bLimitInScreen;
	int m_nClickX;
	int m_nClickY;
	int m_nGridSize;
	bool m_bShowGrid;
	bool m_bIsModal;
    bool m_bVisibleEvent;
	GroupBox* m_pPanel;
	ScrollBar* m_pHScrollBar;
	ScrollBar* m_pVScrollBar;
	Label* m_pCornerLabel;
	int m_nOffsetX;
	int m_nOffsetY;
	PERSISTID m_MainMenuID;
	PERSISTID m_DefaultID;

	std::string m_strFormFile;
	std::string m_strFormName;
};

#endif // _FORM_H
