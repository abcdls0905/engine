//--------------------------------------------------------------------
// �ļ���:		tree_node.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��16��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TREE_NODE_H
#define _TREE_NODE_H

#include "../public/module.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "color_data.h"

// ��״�б�ڵ�

class TreeView;

class TreeNode: public IEntity
{
private:
	typedef TArrayPod<TreeNode*, 1, TCoreAlloc> node_container_t;

public:
	static TreeNode* GetTreeNode(const IEntity* pEntity, const PERSISTID& id);

public:
	TreeNode();
	virtual ~TreeNode();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	// �ڵ�����
	void SetText(const wchar_t* value);
	const wchar_t* GetText() const;
	
	// ������
	void SetFont(const char* value);
	const char* GetFont() const;

    // �ڵ���ֵ
    void SetMark(const int nMark);
    const int GetMark() const;

	unsigned int GetHash() const;
	
	// �Ƿ���չ
	void SetExpand(bool value);
	bool GetExpand() const;

	// �Ƿ���ʾ�ڸ�ͼƬ
	void SetShowCoverImage(bool value);
	bool GetShowCoverImage() const;

	// ����
	void SetLevel(int value);
	int GetLevel() const;

	// ���ͼƬ����
	void SetImageIndex(int value);
	int GetImageIndex() const;
	
	// ǰ��ɫ
	void SetForeColorString(const char* value);
	result_string GetForeColorString() const;
	unsigned int GetForeColorValue() const;

	// �������ɫ
	void SetShadowColorString(const char* value);
	result_string GetShadowColorString() const;
	unsigned int GetShadowColorValue() const;

	// �����ӽڵ�
	virtual PERSISTID CreateNode(const wchar_t* text);
	// ����ӽڵ�
	bool AddNode(const PERSISTID& id);
	// �����ӽڵ�
	bool InsertAfterNode(const PERSISTID& id, const PERSISTID& after);
	bool InsertBeforeNode(const PERSISTID& id, const PERSISTID& before);
	// ɾ���ӽڵ�
	bool RemoveNode(const PERSISTID& id);
	// ��������ӽڵ�
	bool ClearNode();
	// ����ӽڵ�����
	int GetNodeCount() const;
	// ����ӽڵ��б�
	void GetNodeList(const IVarList& args, IVarList& result) const;
	// ����ӽڵ�
	PERSISTID GetNode(int index) const;
	// ��ø��ڵ�
	PERSISTID GetParentNode() const;
	// ���ҽڵ�
	PERSISTID FindNode(const wchar_t* text) const;
    // ͨ���Զ���Markֵ���ҽڵ�
    PERSISTID FindNodeByMark(const int nMark) const;
	// ��չȫ���ڵ�
	bool ExpandAll();

	// ָ�����ӽڵ�
	TreeNode* Get(int index) const;
	// ���ڵ�
	void SetParent(TreeNode* parent);
	TreeNode* GetParent() const;
	// �����ڵ����б�ؼ�
	void SetTreeView(TreeView* tree);
	TreeView* GetTreeView() const;
	// �Ƿ����һ���ڵ�
	bool IsBack() const;

protected:
	bool FindNodeIndex(const PERSISTID& id, int& index) const;
	void ReleaseNode(TreeNode* node);
	bool InsertNodeAt(TreeNode* node, int index);
	bool RefreshTreeView();
	
protected:
	node_container_t m_Nodes;
	TreeNode* m_pParent;
	TreeView* m_pTreeView;
	core_wstring m_wsText;
	core_string m_szFont;
	unsigned int m_nHash;
	int m_nLevel;
	int m_nImageIndex;
    int m_nMark;
	bool m_bExpand;
	CColorData m_ForeColor;
	CColorData m_ShadowColor;
	PERSISTID m_TagID;
	bool m_bShowCoverImage;
};

#endif // _TREE_NODE_H
