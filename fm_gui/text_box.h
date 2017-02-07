//--------------------------------------------------------------------
// �ļ���:		text_box.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��3��10��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TEXT_BOX_H
#define _TEXT_BOX_H

#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "i_scrollable.h"
#include "mark_string.h"

// �ı���ʾ��

class CMarkString;

class TextBox: public IScrollable
{
private:
	enum { MIN_ITEM_HEIGHT = 4 };

	// ���ֶ���
	struct chapter_t
	{
		core_wstring wsText;
		PERSISTID TagID;
		size_t nStart;
		size_t nCount;
	};

	struct item_t
	{
		CMarkString* pMarkStr;
		size_t nChapter;
		unsigned int nForeColor;
	};
	
public:
	TextBox();
	virtual ~TextBox();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	
	// �Ƿ�������
	virtual bool GetLoadFinish();

	// ���������״
	virtual int OnSetCursor(int x, int y);
	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonDoubleClick(int x, int y);
	virtual int OnKeyDown(unsigned int vk);

	// �Ƿ���ʾ��ѡ����
	void SetShowSelect(bool value);
	bool GetShowSelect() const;

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
	
	// ������������״
	void SetRefCursor(const char* value);
	const char* GetRefCursor() const;

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
	
	// ������ֶ���
	int AddChapter(const wchar_t* text);
	// ɾ��ָ������
	bool RemoveChapter(size_t index);
	bool RemoveChapterRange(size_t beg, size_t end);
	// ������ж���
	bool ClearChapter();
	// Ӧ���µĿؼ����ˢ�¶�������
	bool RefreshChapter();
	// ��ö�������
	int GetChapterCount() const;
	// ���ָ����������
	const wchar_t* GetChapter(size_t index) const;
	// ����Ĺ�������
	bool SetChapterTag(size_t index, const PERSISTID& id);
	PERSISTID GetChapterTag(size_t index) const;
	// ���ö���������ɫ
	bool SetChapterForeColor(size_t index, const char* color);

	// ��������
	bool LoadText(const wchar_t* text);
	// �����ı��ļ�
	bool LoadFile(const char* file);
	// ���浽�ı��ļ�
	bool SaveFile(const char* file);
	
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
	// ���ɶ���
	bool GenChapter(size_t index);

	// �����ܵ�ƽ�������ߴ�
	void CalcScrollTotal();

	// ������
	int ClickItem(int select_index);
	// ˫����
	int DoubleClickItem(int select_index);

private:
	bool m_bShowSelect;
	int m_nItemHeight;
	int m_nTextOffsetX;
	int m_nTopIndex;
	int m_nSelectIndex;
	CColorData m_SelectForeColor;
	CColorData m_SelectBackColor;
	CImagePlayer m_SelectBackImage;
	core_string m_strRefCursor;
	TArrayPod<item_t, 1, TCoreAlloc> m_Items;
	TArrayPod<chapter_t*, 1, TCoreAlloc> m_Chapters;
	bool m_bScrollSmooth;
	int m_nScrollHeight;
	int m_nScrollTop;
};

#endif // _TEXT_BOX_H
