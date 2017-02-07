//--------------------------------------------------------------------
// 文件名:		tree_view.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TREE_VIEW_H
#define _TREE_VIEW_H

#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "i_scrollable.h"

// 树状列表

class TreeNode;
class ImageList;

class TreeView: public IScrollable
{
private:
	enum { TREE_LINE_COLOR = 0xFFC0C0C0 };
	enum { MIN_ITEM_HEIGHT = 4 };
	enum { MIN_LEVEL_WIDTH = 10 };

public:
	TreeView();
	virtual ~TreeView();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void PrepareDesignInfo(IDesignInfo* di);

	// 是否加载完成
	virtual bool GetLoadFinish();

	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonUp(int x, int y);
	virtual int OnLButtonDoubleClick(int x, int y);
	virtual int OnRButtonUp(int x, int y);
	virtual int OnKeyDown(unsigned int vk);
	virtual int OnMouseMove(int x, int y);
	virtual int OnLostCapture(const PERSISTID& get_id);

	// 行高度
	void SetItemHeight(int value);
	int GetItemHeight() const;

	// 文字起始位移
	void SetTextOffsetX(int value);
	int GetTextOffsetX() const;

	// 文字起始Y位移
	void SetTextOffsetY(int value);
	int GetTextOffsetY() const;
	
	// 每一级之间的宽度间距
	void SetLevelWidth(int value);
	int GetLevelWidth() const;

	// 扩展子节点标记的大小
	void SetExpandBoxSize(int value);
	int GetExpandBoxSize() const;

	// 选中时的颜色
	void SetSelectForeColorString(const char* value);
	result_string GetSelectForeColorString() const;
	
	// 选中时的背景颜色
	void SetSelectBackColorString(const char* value);
	result_string GetSelectBackColorString() const;
	
	// 线的颜色
	void SetTreeLineColorString(const char* value);
	result_string GetTreeLineColorString() const;

	// 根节点
	void SetRootNode(const PERSISTID& value);
	PERSISTID GetRootNode() const;
	
	// 选中的节点
	void SetSelectNode(const PERSISTID& value);
	PERSISTID GetSelectNode() const;

	// 图元列表
	void SetImageListID(const PERSISTID& value);
	PERSISTID GetImageListID() const;
	
    // 设置是否绘制根节点
    void SetIsNoDrawRoot(bool IsDraw);
    bool GetIsNoDrawRoot() const;

	// 设置是否左键点击展开、关闭节点
	void SetIsLeftClickExpandClose(bool IsExpandClose);
	bool GetIsLeftClickExpandClose() const;

    // 设置背中背景
    void SetSelectBackDraw(const char* value);
    const char* GetSelectBackDraw();

    // 展开时的-号图元
    void SetNodeExpandDraw(const char* value);
    const char* GetNodeExpandDraw();

    // 收起时的+号图元
    void SetNodeCloseDraw(const char* value);
    const char* GetNodeCloseDraw();
	
	// 设置节点背景贴图
    void SetNodeBackImage(const char* value);
    const char* GetNodeBackImage();

	// 设置节点鼠标定位时的贴图
    void SetNodeFocusImage(const char* value);
    const char* GetNodeFocusImage();

	// 设置节点遮盖贴图
    void SetNodeCoverImage(const char* value);
    const char* GetNodeCoverImage();

	// 创建根节点
	virtual PERSISTID CreateRootNode(const wchar_t* text);
	
	// 刷新
	void Refresh();

	// 获取当前可以显示的节点列表
	void GetShowNodeList(const IVarList& args, IVarList& result) const;
	void GetAllNodeList(const IVarList& args, IVarList& result) const;

	// 创建界面图元
	void CreateImagePlayer(CImagePlayer& rImagePlayer);

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
	bool DrawNodeLine(ImageList* pImageList, int x1, int y1, TreeNode* node, 
		int& index);
	bool DrawNodeText(ImageList* pImageList, int x1, int y1, TreeNode* node, 
		int& index);
	bool DrawNodeImage(ImageList* pImageList, int x1, int y1, TreeNode* node, 
		int& index);
    bool DrawExpandAndCloseImage(int x1, int  y1, TreeNode* node, int& index);
	
protected:
	typedef TArrayPod<TreeNode*, 1, TCoreAlloc> node_container_t;

	void ReleaseRootNode();

	// 获得指定节点的可显节点队列
	int CalcShowNodes(TreeNode* node, node_container_t& shows);
	// 获得所有可显节点队列
	virtual int CalcRootShowNodes();
	// 获得可显示的行数（节点数）
	int GetNodeRows(TreeNode* node) const;
	virtual int GetCanDispCount() const;
    // 计算总的横向平滑滚动尺寸
    virtual void CalcScrollTotal(ImageList* pImageList, int x1, int y1, 
        TreeNode* node, int& index);

	bool ShowSelectItem(TreeNode* node);
	
	void SetNodeLevel(TreeNode* node, int& level);
	int GetShowNodeIndex(TreeNode* node) const;
	TreeNode* GetShowIndexNode(int index) const;
	TreeNode* GetNodeByIndex(TreeNode* node, int node_index, int& index) const;
	TreeNode* GetInNode(int x, int y) const;

	int DoubleClickNode(TreeNode* node);

	void AddShowNode(const TreeNode* pNode, IVarList& result) const;
	void AddAllNode(const TreeNode* pNode, IVarList& result) const;

protected:
	TreeNode* m_pRootNode;
	PERSISTID m_FocusNode;
	PERSISTID m_SelectNode;
	PERSISTID m_ImageListID;
	int m_nItemHeight;
	int m_nTextOffsetX;
	int m_nTextOffsetY;
	int m_nLevelWidth;
	int m_nExpandBoxSize;
	int m_nTopIndex;
    int m_nScrollWidth;
    int m_nScrollLeft;
	CColorData m_SelectForeColor;
	CColorData m_SelectBackColor;
	CColorData m_TreeLineColor;
	node_container_t m_ShowNodes;
    bool m_IsNoDrawRoot;
	bool m_IsLeftClickExpandClose;
    CImagePlayer m_NodeExpandDraw;
    CImagePlayer m_NodeCloseDraw;
    CImagePlayer m_ImageSelectBar;
	CImagePlayer m_NodeBackImage;
	CImagePlayer m_NodeFocusImage;
	CImagePlayer m_NodeCoverImage;
};

#endif // _TREE_VIEW_H
