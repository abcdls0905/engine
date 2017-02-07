//--------------------------------------------------------------------
// 文件名:		group_box.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _GROUP_BOX_H
#define _GROUP_BOX_H

#include "i_container.h"

// 控件框容器

class GroupBox: public IContainer
{
public:
	GroupBox();
	virtual ~GroupBox();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

    // 是否透明
    void SetTransparent(bool value);
    bool GetTransparent() const;

	// 是否要剪裁子控件
	void SetClipChildren(bool value) { m_bClipChildren = value; }
	bool GetClipChildren() const { return m_bClipChildren; }
	
    virtual int GetMinWidth() const { return 4; }
	virtual int GetMinHeight() const { return 4; }
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	virtual int OnLButtonDown(int x, int y);
	virtual int OnDragMove(int x, int y);
    virtual IControl* OnGetInWhat(int x, int y);

	// 是否固定
	void SetFixed(bool value);
	bool GetFixed() const;
	
private:
	bool m_bFixed;
	bool m_bTransparent;
	bool m_bClipChildren;
	int m_nClickX;
	int m_nClickY;
};

#endif // _GROUP_BOX_H
