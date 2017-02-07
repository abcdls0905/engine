//--------------------------------------------------------------------
// �ļ���:		list_box.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��14��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _LIST_BOX_H
#define _LIST_BOX_H

#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "i_scrollable.h"

class TListBoxAlloc
{
public:
	TListBoxAlloc() {}
	~TListBoxAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TListBoxAlloc& src) {}
};

// �б��

class ListBox: public IScrollable
{
private:
	enum { MIN_ITEM_HEIGHT = 4 };

	struct item_t
	{
		core_wstring wsValue;
		PERSISTID TagID;
		int nWidth;
		unsigned int nColor;
	};

public:
	ListBox();
	virtual ~ListBox();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	
	// �Ƿ�������
	virtual bool GetLoadFinish();

	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonDoubleClick(int x, int y);
	virtual int OnMouseMove(int x, int y);
	virtual int OnKeyDown(unsigned int vk);

	// �Ƿ�����
	void SetSorted(bool value);
	bool GetSorted() const;
	
	// ��������ƶ���ѡȡ�ַ�������
	void SetMouseSelect(bool val);
	bool GetMouseSelect() const;

	// ÿ�еĸ߶�
	void SetItemHeight(int value);
	int GetItemHeight() const;

	// ������ʼλ��
	void SetTextOffsetX(int value);
	int GetTextOffsetX() const;
	
	// ����
	int GetItemCount() const;

	// ѡ�е��к�
	void SetSelectIndex(int value);
	int GetSelectIndex() const;

	// ѡ��ʱ����ɫ
	void SetSelectForeColorString(const char* value);
	result_string GetSelectForeColorString() const;
	
	// ѡ��ʱ�ı�����ɫ
	void SetSelectBackColorString(const char* value);
	result_string GetSelectBackColorString() const;
	
	// ѡ��ʱ�ı�����ͼ
	void SetSelectBackImage(const char* value);
	const char* GetSelectBackImage() const;
	
	// ѡ�е��ַ���
	void SetSelectString(const wchar_t* value);
	const wchar_t* GetSelectString() const;
	
	// �����ַ���
	int FindString(const wchar_t* item) const;
	// ����ַ���
	int AddString(const wchar_t* item);
	// ɾ���ַ���
	bool RemoveString(const wchar_t* item);
	// ɾ��ָ����
	bool RemoveByIndex(int index);
	// ���������
	bool ClearString();
	// ���ָ���е��ַ���
	const wchar_t* GetString(int index) const;
    // �޸�ָ���е��ַ���
    bool ChangeString(int index, const wchar_t* value);
	// �б������ɫ
	bool SetItemColor(int index, const char* value);
	result_string GetItemColor(int index) const;

	// �����ݵĹ�������
	bool SetTag(int index, const PERSISTID& id);
	PERSISTID GetTag(int index) const;
	
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
	// ��λ
	int GetInItem(int x, int y) const;

	// �����ܵ�ƽ�������ߴ�
	void CalcScrollTotal();
	
	// ʹ��������ʽ����ַ�
	int AddStringSort(const wchar_t* item, int beg, int end);
	// ����
	bool ProcessSort();

	// ������
	int ClickItem(int select_index);
	// ˫����
	int DoubleClickItem(int select_index);

private:
	bool m_bSorted;
	bool m_bMouseSelect;
	int m_nItemHeight;
	int m_nTextOffsetX;
	int m_nScrollWidth;
	int m_nScrollLeft;
	int m_nTopIndex;
	int m_nSelectIndex;
	CColorData m_SelectForeColor;
	CColorData m_SelectBackColor;
	CImagePlayer m_SelectBackImage;
	TArrayPod<item_t*, 1, TListBoxAlloc> m_Items;
};

#endif // _LIST_BOX_H
