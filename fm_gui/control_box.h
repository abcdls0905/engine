//--------------------------------------------------------------------
// �ļ���:		control_box.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��3��27��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _CONTROL_BOX_H
#define _CONTROL_BOX_H

#include "../public/core_mem.h"
#include "../utils/array_pod.h"
#include "i_scrollable.h"

// �ؼ��б��

class ControlBox: public IScrollable
{
private:
	enum { MIN_ITEM_HEIGHT = 4 };

	struct item_t
	{
		IControl* pControl;
	};
	
public:
	ControlBox();
	virtual ~ControlBox();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	
	// �Ƿ�������
	virtual bool GetLoadFinish();

	virtual int OnLButtonDown(int x, int y);
	virtual int OnKeyDown(unsigned int vk);
	virtual IControl* OnGetInWhat(int x, int y);
	
	// �Ƿ���ʾ��ѡ����
	void SetShowSelect(bool value);
	bool GetShowSelect() const;

	// ÿ�еĸ߶�
	void SetItemHeight(int value);
	int GetItemHeight() const;

	// ��ʼλ��
	void SetOffsetX(int value);
	int GetOffsetX() const;
	void SetOffsetY(int value);
	int GetOffsetY() const;
	
	// ����
	int GetItemCount() const;

	// ѡ�е��к�
	void SetSelectIndex(int value);
	int GetSelectIndex() const;

	// ѡ�еĶ���
	void SetSelectChapter(int value);
	int GetSelectChapter() const;
	
	// ѡ��ʱ����ɫ
	void SetSelectForeColorString(const char* value);
	result_string GetSelectForeColorString() const;
	
	// ѡ��ʱ�ı�����ɫ
	void SetSelectBackColorString(const char* value);
	result_string GetSelectBackColorString() const;
	
	// ѡ��ʱ�ı�����ͼ
	void SetSelectBackImage(const char* value);
	const char* GetSelectBackImage() const;
	
	// �Ƿ�ƽ������
	void SetScrollSmooth(bool value);
	bool GetScrollSmooth() const;

	// �ܵ�ƽ�������߶�
	int GetScrollHeight() const;
	// ƽ������ÿҳ�ĸ߶�
	int GetScrollPage() const;

	// ƽ�������Ķ���λ��
	void SetScrollTop(int value);
	int GetScrollTop() const;

	// ���ҿؼ�
	int FindControl(const PERSISTID& id) const;
	// ��ӿؼ�
	int AddControl(const PERSISTID& id);
	// ��ӿؼ�
	int AddControlByIndex(const PERSISTID& id, int index);
	// ɾ���ؼ�
	bool RemoveControl(const PERSISTID& id);
	// ɾ��ָ����
	bool RemoveByIndex(int index);
	// ���������
	bool ClearControl();
	// ���ָ���е��ַ���
	PERSISTID GetControl(int index) const;
	
private:
	virtual int GetVerticalTotal() const;
	virtual int GetVerticalCanDisp() const;
	virtual void SetVerticalValue(int value);
	virtual int GetVerticalValue() const;
	
	virtual int GetHorizonTotal() const;
	virtual int GetHorizonCanDisp() const;
	virtual void SetHorizonValue(int value);
	virtual int GetHorizonValue() const;
	
	virtual void PaintScrollRect(float seconds);
	
private:
	// ��ʾѡ����
	bool ShowSelectItem();
	// ��ǰ������ʾ������
	int GetCanDispCount() const;
	// ��õ�ǰ��Ҫ��ʾ������
	int GetDispItemCount() const;

	// �����ʾ��
	void ClearItems();

	// �����ܵ�ƽ�������ߴ�
	void CalcScrollTotal();
	
private:
	bool m_bShowSelect;
	bool m_bScrollSmooth;
	int m_nScrollHeight;
	int m_nScrollTop;
	int m_nScrollWidth;
	int m_nScrollLeft;
	int m_nItemHeight;
	int m_nOffsetX;
	int m_nOffsetY;
	int m_nTopIndex;
	int m_nSelectIndex;
	CColorData m_SelectForeColor;
	CColorData m_SelectBackColor;
	CImagePlayer m_SelectBackImage;
	TArrayPod<item_t, 1, TCoreAlloc> m_Items;
};

#endif // _CONTROL_BOX_H
