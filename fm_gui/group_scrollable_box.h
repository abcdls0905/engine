//--------------------------------------------------------------------
// �ļ���:		GroupScrollableBox.h
// ��  ��:		�Զ����������,��ӵ�й������Ŀؼ������ؼ�
// ˵  ��:		
// ��������:	2008��9��10��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef GroupScrollableBox_h_2008_9_10
#define GroupScrollableBox_h_2008_9_10

#include "../public/core_mem.h"
#include "../utils/pod_hash_map.h"
#include "group_box.h"
#include "i_scrollable.h"

// GroupScrollableBox

class GroupScrollableBox : public GroupBox
{
public:
    GroupScrollableBox();
    virtual ~GroupScrollableBox();

    virtual bool Init(const IVarList& args);
    virtual bool Shut();

    virtual void PrepareDesignInfo(IDesignInfo* di);
    virtual void Paint(float seconds);

	// �Ƿ�������
	virtual bool GetLoadFinish();

    virtual int GetMinWidth() const { return 16; }
    virtual int GetMinHeight() const { return 16; }

	// ������
	virtual int OnMouseWheel(int x, int y, int delta);

    // �ӿؼ���Ϣ
    virtual int OnSuborEvent(IControl* subor, int event, const IVarList& args);
    virtual int OnWidthChanged(int old);
    virtual int OnHeightChanged(int old);
    // ����ӿؼ�
    virtual int OnAdd(const PERSISTID& child_id);
    // �Ƴ��ӿؼ�
    virtual int OnRemove(const PERSISTID& child_id);
    virtual IControl* OnGetInWhat(int x, int y);

    // �����ϸ������ϵ�Y����,X����
    bool ResetChildrenYPos();

    // ��ȡʵ����ʾ���ݸ߶�
    int GetContentHeight();

    // ��ȡ�ӿؼ�����
    int GetChildControlCount() const;
    // ���ָ��λ�õ��ӿؼ�
    PERSISTID GetChildControlByIndex(int index);

    // �Ƿ��д�ֱ������
    void SetHasVScroll(bool value);
    bool GetHasVScroll() const;

    // �Ƿ���ˮƽ������
    void SetHasHScroll(bool value);
    bool GetHasHScroll() const;

    // ��ֱ�������Ƿ������
    void SetVScrollLeft(bool value);
    bool GetVScrollLeft() const;

    void SetScrollSize(int value);
    int GetScrollSize() const;

    bool GetAlwaysVScroll() const ;
    void SetAlwaysVScroll(bool value);

    bool GetIsEdit() { return m_bIsEdit; }
    void SetIsEdit(bool IsEdit);

    // ģ�巽��
    // ��ֱ����������
    virtual int GetVerticalTotal() const;
    // ������ʾ������
    virtual int GetVerticalCanDisp() const;
    // ���õ�ǰֵ
    virtual void SetVerticalValue(int value);
    // ��ȡ��ǰֵ
    virtual int GetVerticalValue() const;

    PERSISTID GetVScrollBar();
    unsigned int CalcScrollVisible() const;

    bool UpdateScroll();

	// ������������Ӧ
	virtual int OnLButtonDown(int x, int y);
	// ��ʼ�϶���Ӧ
	virtual int OnDragMove(int x, int y);

protected:
    virtual bool RefreshInnerAbsPos();

private:
    int m_nScrollSize;
    bool m_bVScrollLeft;
    bool m_bHasVScroll;
    bool m_bAlwaysVScroll;
	ScrollBar* m_pVScrollBar;
	int m_nTopIndex;
    bool m_bIsEdit;

	int m_nDragOldX;
	int m_nDragOldY;

	TPodHashMap<PERSISTID, long, TPodTraits<PERSISTID>, TCoreAlloc> m_ChildrenYPos;
};

#endif //GroupScrollableBox_h_2008_9_10