//--------------------------------------------------------------------
// �ļ���:		menu.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��5��11��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _MENU_H
#define _MENU_H

#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "i_control.h"

// �˵�

class MenuItem;

class Menu: public IControl
{
private:
	// �ָ��ߵĸ߶�
	enum { SEPERATOR_HEIGHT = 8 };
	// �߾�
	enum { PADDING_WIDTH = 2 };
	// �Ӳ˵���־���ұ߾�
    enum { SUBFLAG_WIDTH = 8};

public:
	Menu();
	virtual ~Menu();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// �Ƿ�������
	virtual bool GetLoadFinish();

	virtual int OnLButtonUp(int x, int y);
	virtual int OnMouseMove(int x, int y);
    virtual int OnLButtonDown(int x, int y);
    virtual int OnDragMove(int x, int y);

	// �˵���ĸ߶�
	void SetItemHeight(int value);
	int GetItemHeight() const;

	// ���ͼ�����Ŀ��
	void SetLeftBarWidth(int value);
	int GetLeftBarWidth() const;

	// ѡ��ʱ����ɫ
	void SetSelectForeColorString(const char* value);
	result_string GetSelectForeColorString() const;
	
	// ѡ��ʱ�ı�����ɫ
	void SetSelectBackColorString(const char* value);
	result_string GetSelectBackColorString() const;

	// ѡ��ʱ�ı߿���ɫ
	void SetSelectBorderColorString(const char* value);
	result_string GetSelectBorderColorString() const;

    // 
    void SetSelectBarDraw(const char* pDraw);
    const char* GetSelectBarDraw();

	// ���ͼ�����ı�����ɫ
	void SetLeftBarBackColorString(const char* value);
	result_string GetLeftBarBackColorString() const;

	// 
	void SetLeftBarDraw(const char*pDraw);
	const char* GetLeftBarDraw();

	// �ָ�������ɫ
	void SetSplitColorString(const char* value);
	result_string GetSplitColorString() const;

	//
    void SetSplitDraw(const char* pDraw);
    const char* GetSplitDraw();

    void SetSubFlagDraw(const char* pDraw);
    const char* GetSubFlagDraw();

	void SetCheckedDraw(const char* pDraw);
	const char* GetCheckedDraw();

    void SetCenter(bool IsCenter);
    bool GetCenter();

	// ���ͼ����
	void SetLeftBar(bool bLeftBar);
	bool GetLeftBar();

	// �½��˵���
	PERSISTID NewItem(const char* name, const wchar_t* text);
	// �½�������˵���
	PERSISTID CreateItem(const char* name, const wchar_t* text);
	// ��Ӳ˵���
	bool AddItem(const PERSISTID& id);
	// ����˵���
	bool InsertItem(const PERSISTID& id, const char* before);
	// �Ƴ��˵���
	bool RemoveItem(const char* name);
	// ɾ���˵���
	bool DeleteItem(const char* name);
	// ��ղ˵���
	bool ClearItem();
	// ɾ�����в˵���
	bool DeleteAllItem();
	// ���Ҳ˵���
	PERSISTID FindItem(const char* name);
	// ��ò˵�������
	int GetItemCount() const;
	// ���ָ��λ�ò˵���
	PERSISTID GetItem(size_t index);

    void SetVisible(bool val);

private:
	MenuItem* GetMenuItem(const PERSISTID& id) const;

	// �Ƿ��Ƿָ���
	bool IsSeperator(const MenuItem* pItem) const;

	// ����ܵĲ˵��߶�
	int CalcTotalHeight() const;

	// ��ò˵����
	int CalcTotalWidth() const;

	// �����ĸ��˵�����
	MenuItem* GetInItem(int x, int y, int& itemRight, int& itemTop) const;
	
	bool FindItemIndex(const char* name, size_t& index) const;

private:
	int m_nItemHeight;				// ÿ���˵���ĸ߶�
	bool m_IsCenter;				// �˵������ֵ�ˮƽ���뷽ʽ�Ƿ���У�Ĭ��Ϊ�����
	bool m_bLeftBar;				// �Ƿ������ͼ����
	int m_nLeftBarWidth;			// ���ͼ�����Ŀ��

    int m_nDragOldX;
    int m_nDragOldY;

	CColorData m_SelectForeColor;	// ѡ�����ǰ��ɫ
	CColorData m_SelectBackColor;	// ѡ����ı���ɫ
	CColorData m_SelectBorderColor;	// ѡ����ı߿�ɫ
	CImagePlayer m_SelectBarDraw;	// ѡ����ͼԪ

	CColorData m_LeftBarBackColor;	// ͼ�����ı���ɫ
	CImagePlayer m_LeftBarDraw;		// ͼ����ͼԪ

	CColorData m_SplitColor;		// �ָ�������ɫ
	CImagePlayer m_SplitDraw;		// �ָ���ͼԪ
	CImagePlayer m_SubFlagDraw;		// �Ӳ˵���ͷ
	CImagePlayer m_CheckedDraw;		// ��Checked�Ĳ˵����ͼ��

	PERSISTID m_SelectItem;			// ��ǰ��ѡ��Ĳ˵���
    PERSISTID m_ShowSubMenu;		// ��ǰ��ʾ���Ӳ˵�

	TArrayPod<MenuItem*, 1, TCoreAlloc> m_Items; // �����Ӳ˵�
};

#endif // _MENU_H
