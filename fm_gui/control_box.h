//--------------------------------------------------------------------
// 文件名:		control_box.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年3月27日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _CONTROL_BOX_H
#define _CONTROL_BOX_H

#include "../public/core_mem.h"
#include "../utils/array_pod.h"
#include "i_scrollable.h"

// 控件列表框

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
	
	// 是否加载完成
	virtual bool GetLoadFinish();

	virtual int OnLButtonDown(int x, int y);
	virtual int OnKeyDown(unsigned int vk);
	virtual IControl* OnGetInWhat(int x, int y);
	
	// 是否显示被选中项
	void SetShowSelect(bool value);
	bool GetShowSelect() const;

	// 每行的高度
	void SetItemHeight(int value);
	int GetItemHeight() const;

	// 起始位移
	void SetOffsetX(int value);
	int GetOffsetX() const;
	void SetOffsetY(int value);
	int GetOffsetY() const;
	
	// 行数
	int GetItemCount() const;

	// 选中的行号
	void SetSelectIndex(int value);
	int GetSelectIndex() const;

	// 选中的段落
	void SetSelectChapter(int value);
	int GetSelectChapter() const;
	
	// 选中时的颜色
	void SetSelectForeColorString(const char* value);
	result_string GetSelectForeColorString() const;
	
	// 选中时的背景颜色
	void SetSelectBackColorString(const char* value);
	result_string GetSelectBackColorString() const;
	
	// 选中时的背景贴图
	void SetSelectBackImage(const char* value);
	const char* GetSelectBackImage() const;
	
	// 是否平滑滚动
	void SetScrollSmooth(bool value);
	bool GetScrollSmooth() const;

	// 总的平滑滚动高度
	int GetScrollHeight() const;
	// 平滑滚动每页的高度
	int GetScrollPage() const;

	// 平滑滚动的顶部位置
	void SetScrollTop(int value);
	int GetScrollTop() const;

	// 查找控件
	int FindControl(const PERSISTID& id) const;
	// 添加控件
	int AddControl(const PERSISTID& id);
	// 添加控件
	int AddControlByIndex(const PERSISTID& id, int index);
	// 删除控件
	bool RemoveControl(const PERSISTID& id);
	// 删除指定行
	bool RemoveByIndex(int index);
	// 清除所有行
	bool ClearControl();
	// 获得指定行的字符串
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
	// 显示选择项
	bool ShowSelectItem();
	// 当前可以显示项数量
	int GetCanDispCount() const;
	// 获得当前需要显示的行数
	int GetDispItemCount() const;

	// 清空显示项
	void ClearItems();

	// 计算总的平滑滚动尺寸
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
