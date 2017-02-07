//--------------------------------------------------------------------
// 文件名:		tree_node.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TREE_NODE_H
#define _TREE_NODE_H

#include "../public/module.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "color_data.h"

// 树状列表节点

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
	
	// 节点文字
	void SetText(const wchar_t* value);
	const wchar_t* GetText() const;
	
	// 字体名
	void SetFont(const char* value);
	const char* GetFont() const;

    // 节点标记值
    void SetMark(const int nMark);
    const int GetMark() const;

	unsigned int GetHash() const;
	
	// 是否扩展
	void SetExpand(bool value);
	bool GetExpand() const;

	// 是否显示遮盖图片
	void SetShowCoverImage(bool value);
	bool GetShowCoverImage() const;

	// 级别
	void SetLevel(int value);
	int GetLevel() const;

	// 标记图片索引
	void SetImageIndex(int value);
	int GetImageIndex() const;
	
	// 前景色
	void SetForeColorString(const char* value);
	result_string GetForeColorString() const;
	unsigned int GetForeColorValue() const;

	// 字体描边色
	void SetShadowColorString(const char* value);
	result_string GetShadowColorString() const;
	unsigned int GetShadowColorValue() const;

	// 创建子节点
	virtual PERSISTID CreateNode(const wchar_t* text);
	// 添加子节点
	bool AddNode(const PERSISTID& id);
	// 插入子节点
	bool InsertAfterNode(const PERSISTID& id, const PERSISTID& after);
	bool InsertBeforeNode(const PERSISTID& id, const PERSISTID& before);
	// 删除子节点
	bool RemoveNode(const PERSISTID& id);
	// 清除所有子节点
	bool ClearNode();
	// 获得子节点数量
	int GetNodeCount() const;
	// 获得子节点列表
	void GetNodeList(const IVarList& args, IVarList& result) const;
	// 获得子节点
	PERSISTID GetNode(int index) const;
	// 获得父节点
	PERSISTID GetParentNode() const;
	// 查找节点
	PERSISTID FindNode(const wchar_t* text) const;
    // 通过自定义Mark值查找节点
    PERSISTID FindNodeByMark(const int nMark) const;
	// 扩展全部节点
	bool ExpandAll();

	// 指定的子节点
	TreeNode* Get(int index) const;
	// 父节点
	void SetParent(TreeNode* parent);
	TreeNode* GetParent() const;
	// 隶属于的树列表控件
	void SetTreeView(TreeView* tree);
	TreeView* GetTreeView() const;
	// 是否最后一个节点
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
