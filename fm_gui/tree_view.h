//--------------------------------------------------------------------
// �ļ���:		tree_view.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��16��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TREE_VIEW_H
#define _TREE_VIEW_H

#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "i_scrollable.h"

// ��״�б�

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

	// �Ƿ�������
	virtual bool GetLoadFinish();

	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonUp(int x, int y);
	virtual int OnLButtonDoubleClick(int x, int y);
	virtual int OnRButtonUp(int x, int y);
	virtual int OnKeyDown(unsigned int vk);
	virtual int OnMouseMove(int x, int y);
	virtual int OnLostCapture(const PERSISTID& get_id);

	// �и߶�
	void SetItemHeight(int value);
	int GetItemHeight() const;

	// ������ʼλ��
	void SetTextOffsetX(int value);
	int GetTextOffsetX() const;

	// ������ʼYλ��
	void SetTextOffsetY(int value);
	int GetTextOffsetY() const;
	
	// ÿһ��֮��Ŀ�ȼ��
	void SetLevelWidth(int value);
	int GetLevelWidth() const;

	// ��չ�ӽڵ��ǵĴ�С
	void SetExpandBoxSize(int value);
	int GetExpandBoxSize() const;

	// ѡ��ʱ����ɫ
	void SetSelectForeColorString(const char* value);
	result_string GetSelectForeColorString() const;
	
	// ѡ��ʱ�ı�����ɫ
	void SetSelectBackColorString(const char* value);
	result_string GetSelectBackColorString() const;
	
	// �ߵ���ɫ
	void SetTreeLineColorString(const char* value);
	result_string GetTreeLineColorString() const;

	// ���ڵ�
	void SetRootNode(const PERSISTID& value);
	PERSISTID GetRootNode() const;
	
	// ѡ�еĽڵ�
	void SetSelectNode(const PERSISTID& value);
	PERSISTID GetSelectNode() const;

	// ͼԪ�б�
	void SetImageListID(const PERSISTID& value);
	PERSISTID GetImageListID() const;
	
    // �����Ƿ���Ƹ��ڵ�
    void SetIsNoDrawRoot(bool IsDraw);
    bool GetIsNoDrawRoot() const;

	// �����Ƿ�������չ�����رսڵ�
	void SetIsLeftClickExpandClose(bool IsExpandClose);
	bool GetIsLeftClickExpandClose() const;

    // ���ñ��б���
    void SetSelectBackDraw(const char* value);
    const char* GetSelectBackDraw();

    // չ��ʱ��-��ͼԪ
    void SetNodeExpandDraw(const char* value);
    const char* GetNodeExpandDraw();

    // ����ʱ��+��ͼԪ
    void SetNodeCloseDraw(const char* value);
    const char* GetNodeCloseDraw();
	
	// ���ýڵ㱳����ͼ
    void SetNodeBackImage(const char* value);
    const char* GetNodeBackImage();

	// ���ýڵ���궨λʱ����ͼ
    void SetNodeFocusImage(const char* value);
    const char* GetNodeFocusImage();

	// ���ýڵ��ڸ���ͼ
    void SetNodeCoverImage(const char* value);
    const char* GetNodeCoverImage();

	// �������ڵ�
	virtual PERSISTID CreateRootNode(const wchar_t* text);
	
	// ˢ��
	void Refresh();

	// ��ȡ��ǰ������ʾ�Ľڵ��б�
	void GetShowNodeList(const IVarList& args, IVarList& result) const;
	void GetAllNodeList(const IVarList& args, IVarList& result) const;

	// ��������ͼԪ
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

	// ���ָ���ڵ�Ŀ��Խڵ����
	int CalcShowNodes(TreeNode* node, node_container_t& shows);
	// ������п��Խڵ����
	virtual int CalcRootShowNodes();
	// ��ÿ���ʾ���������ڵ�����
	int GetNodeRows(TreeNode* node) const;
	virtual int GetCanDispCount() const;
    // �����ܵĺ���ƽ�������ߴ�
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
