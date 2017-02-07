//--------------------------------------------------------------------
// 文件名:		TreeView.h
// 内  容:		扩展TreeView
// 说  明:		
// 创建日期:	2011年3月7日
// 创建人:		马志攀
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TREEVIEWEX_H
#define _TREEVIEWEX_H

#include "tree_view.h"

// 树状列表

class TreeNodeEx;
class ImageList;

class TreeViewEx: public TreeView
{	
public:
	TreeViewEx();
	virtual ~TreeViewEx();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void PrepareDesignInfo(IDesignInfo* di);

	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonUp(int x, int y);
	virtual int OnLButtonDoubleClick(int x, int y);
	virtual int OnRButtonUp(int x, int y);
	virtual int OnMouseMove(int x, int y);

	// 创建根节点
	virtual PERSISTID CreateRootNode(const wchar_t* text);

	// 获取展开节点的高度
	int GetNodeTop(PERSISTID id) const;
private:
	virtual int GetVerticalCanDisp() const;
	virtual void PaintScrollRect(float seconds);

	bool DrawNodeText(ImageList* pImageList, int x1, int y1, TreeNodeEx* node, 
		int& index);
	bool DrawExpandAndCloseImage(int x1, int  y1, TreeNodeEx* node, int& index);
	bool DrawNodeImage(ImageList* pImageList, int x1, int y1, TreeNodeEx* node, 
		int& index);
	
	// 获得指定节点的可显节点队列
	int CalcShowNodes(TreeNode* node, node_container_t& shows, bool bShow);
	// 获得所有可显节点队列
	virtual int CalcRootShowNodes();
	virtual int GetCanDispCount() const;
    // 计算总的横向平滑滚动尺寸
    virtual void CalcScrollTotal(ImageList* pImageList, int x1, int y1, 
        TreeNode* node, int& index);

	
	TreeNode* GetInNode(int x, int y) const;

	// 得到展开的节点的总高度
	int GetNodeTop(int index) const;


};

#endif // _TREEVIEWEX_H
