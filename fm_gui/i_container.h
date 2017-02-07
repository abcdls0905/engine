//--------------------------------------------------------------------
// 文件名:		i_container.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _I_CONTAINER_H
#define _I_CONTAINER_H

#include "../public/core_mem.h"
#include "../utils/array_pod.h"
#include "i_control.h"

// 控件容器基类

class IContainer: public IControl
{
private:
	typedef TArrayPod<IControl*, 1, TCoreAlloc> child_container_t;
	typedef TArrayPod<PERSISTID, 1, TCoreAlloc> top_container_t;
	
public:
	IContainer();
	virtual ~IContainer() = 0;

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// 绘制控件
	virtual void Paint(float seconds);
	
	// 设置设计器信息
	virtual void PrepareDesignInfo(IDesignInfo* di);

	// 是否加载完成
	virtual bool GetLoadFinish();

	// 是否容器
	virtual bool IsContainer() const;
	// 用控件名查找
	virtual IControl* FindChild(const char* name) const;
	// 用控件名从本节点开始进行递归查找，包扩本节点
	virtual IControl* QueryControl(const char* name);
	// 添加子控件
	virtual bool AddChild(IControl* child);
	// 在指定控件之后插入
	virtual bool InsertAfterChild(IControl* child, IControl* after);
	// 在指定控件之前插入
	virtual bool InsertBeforeChild(IControl* child, IControl* before);
	// 移除子控件
	virtual bool RemoveChild(IControl* child);
    virtual bool RemoveChildNoEvent(IControl* child);
	// 清空子控件
	virtual void ClearChild();
	// 删除所有子控件
	virtual void DeleteAllChild();
	// 将子控件放到最后（ZOrder）
	virtual bool SendToBack(IControl* child);
	// 将子控件放到最前（ZOrder）
	virtual bool BringToFront(IControl* child);
	// 是否子控件
	virtual bool IsChildControl(IControl* child) const;
	// 获得子控件数量
	virtual size_t GetChildCount() const;
	// 获得指定位置的子控件
	virtual IControl* GetChildByIndex(size_t index) const;
	// 获得子控件ID列表
	virtual size_t GetChildList(IVarList& result) const;
	
	// 需要显示在最上层的控件
	virtual bool FindTopLayer(const PERSISTID& id);
	virtual bool AddTopLayer(const PERSISTID& id);
	virtual bool RemoveTopLayer(const PERSISTID& id);
	virtual void DeleteAllTopLayer();
	// 添加子控件
	virtual int OnAdd(const PERSISTID& child_id);
	// 移除子控件
	virtual int OnRemove(const PERSISTID& child_id);

	// 左上角坐标改变
	virtual int OnLeftChanged(int old);
	virtual int OnTopChanged(int old);
    // 
	virtual int OnWidthChanged(int old);
	virtual int OnHeightChanged(int old);

	virtual IControl* OnGetInWhat(int x, int y);

	// 是否产生添加/删除子控件的事件
	void SetAddRemoveEvent(bool value);
	bool GetAddRemoveEvent() const;
	
	// 添加子控件
	bool Add(const PERSISTID& child);
	// 插入子控件（在指定控件之后）
	bool InsertAfter(const PERSISTID& child, const PERSISTID& after);
	// 插入子控件（在指定控件之前）
	bool InsertBefore(const PERSISTID& child, const PERSISTID& before);
	// 移出子控件
	bool Remove(const PERSISTID& child);
	// 删除所有子控件
	bool DeleteAll();
	// 查找指定名字子控件
	PERSISTID Find(const char* name) const;
	// 将子控件移动最前
	bool ToFront(const PERSISTID& child);
	// 将子控件移动最后
	bool ToBack(const PERSISTID& child);
	// 是否子控件
	bool IsChild(const PERSISTID& id) const;
	
    // 获取顶层子控件列表
    int GetTopControlList(const IVarList& param, IVarList& result);
    // LUA获取容器的所有直接子控件
    int GetChildControlList(const IVarList& param, IVarList& result);

protected:
    virtual bool RefreshInnerAbsPos();

private:
	void InitChild(IControl* child);
	void PutChild(IControl* child);

protected:
	void PaintTopLayer(float seconds);
    void RefreshChildren();

	top_container_t m_TopLayer;

private:
	bool m_bAddRemoveEvent;
	child_container_t m_Children;
};

#endif // _I_CONTAINER_H

