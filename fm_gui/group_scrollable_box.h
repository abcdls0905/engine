//--------------------------------------------------------------------
// 文件名:		GroupScrollableBox.h
// 内  容:		自定义面版内容,并拥有滚动条的控件容器控件
// 说  明:		
// 创建日期:	2008年9月10日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
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

	// 是否加载完成
	virtual bool GetLoadFinish();

    virtual int GetMinWidth() const { return 16; }
    virtual int GetMinHeight() const { return 16; }

	// 鼠标滚轮
	virtual int OnMouseWheel(int x, int y, int delta);

    // 子控件消息
    virtual int OnSuborEvent(IControl* subor, int event, const IVarList& args);
    virtual int OnWidthChanged(int old);
    virtual int OnHeightChanged(int old);
    // 添加子控件
    virtual int OnAdd(const PERSISTID& child_id);
    // 移除子控件
    virtual int OnRemove(const PERSISTID& child_id);
    virtual IControl* OnGetInWhat(int x, int y);

    // 这是严格意义上的Y重排,X不变
    bool ResetChildrenYPos();

    // 获取实际显示内容高度
    int GetContentHeight();

    // 获取子控件数量
    int GetChildControlCount() const;
    // 获得指定位置的子控件
    PERSISTID GetChildControlByIndex(int index);

    // 是否有垂直滚动条
    void SetHasVScroll(bool value);
    bool GetHasVScroll() const;

    // 是否有水平滚动条
    void SetHasHScroll(bool value);
    bool GetHasHScroll() const;

    // 垂直滚动条是否在左边
    void SetVScrollLeft(bool value);
    bool GetVScrollLeft() const;

    void SetScrollSize(int value);
    int GetScrollSize() const;

    bool GetAlwaysVScroll() const ;
    void SetAlwaysVScroll(bool value);

    bool GetIsEdit() { return m_bIsEdit; }
    void SetIsEdit(bool IsEdit);

    // 模板方法
    // 垂直滚动总数量
    virtual int GetVerticalTotal() const;
    // 可以显示的数量
    virtual int GetVerticalCanDisp() const;
    // 设置当前值
    virtual void SetVerticalValue(int value);
    // 获取当前值
    virtual int GetVerticalValue() const;

    PERSISTID GetVScrollBar();
    unsigned int CalcScrollVisible() const;

    bool UpdateScroll();

	// 鼠标左键按下响应
	virtual int OnLButtonDown(int x, int y);
	// 开始拖动响应
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