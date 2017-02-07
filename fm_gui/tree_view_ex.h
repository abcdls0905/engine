//--------------------------------------------------------------------
// �ļ���:		TreeView.h
// ��  ��:		��չTreeView
// ˵  ��:		
// ��������:	2011��3��7��
// ������:		��־��
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TREEVIEWEX_H
#define _TREEVIEWEX_H

#include "tree_view.h"

// ��״�б�

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

	// �������ڵ�
	virtual PERSISTID CreateRootNode(const wchar_t* text);

	// ��ȡչ���ڵ�ĸ߶�
	int GetNodeTop(PERSISTID id) const;
private:
	virtual int GetVerticalCanDisp() const;
	virtual void PaintScrollRect(float seconds);

	bool DrawNodeText(ImageList* pImageList, int x1, int y1, TreeNodeEx* node, 
		int& index);
	bool DrawExpandAndCloseImage(int x1, int  y1, TreeNodeEx* node, int& index);
	bool DrawNodeImage(ImageList* pImageList, int x1, int y1, TreeNodeEx* node, 
		int& index);
	
	// ���ָ���ڵ�Ŀ��Խڵ����
	int CalcShowNodes(TreeNode* node, node_container_t& shows, bool bShow);
	// ������п��Խڵ����
	virtual int CalcRootShowNodes();
	virtual int GetCanDispCount() const;
    // �����ܵĺ���ƽ�������ߴ�
    virtual void CalcScrollTotal(ImageList* pImageList, int x1, int y1, 
        TreeNode* node, int& index);

	
	TreeNode* GetInNode(int x, int y) const;

	// �õ�չ���Ľڵ���ܸ߶�
	int GetNodeTop(int index) const;


};

#endif // _TREEVIEWEX_H
