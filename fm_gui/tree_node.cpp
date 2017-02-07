//--------------------------------------------------------------------
// 文件名:		tree_node.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "tree_node.h"
#include "tree_view.h"
#include "../public/inlines.h"
#include "../public/core_log.h"

/// \file tree_node.cpp
/// \brief 树节点

/// entity: TreeNode
/// \brief 树节点实体
DECLARE_ENTITY(TreeNode, 0, IEntity);

/// readonly: int Level
/// \brief 级别
DECLARE_PROPERTY_GET(int, TreeNode, Level, GetLevel);
/// readonly: object ParentNode
/// \brief 父节点对象
DECLARE_PROPERTY_GET(PERSISTID, TreeNode, ParentNode, GetParentNode);

/// property: wstring Text
/// \brief 节点文本
DECLARE_PROPERTY(const wchar_t*, TreeNode, Text, GetText, SetText);

/// property: string Font
/// \brief 节点字体名
DECLARE_PROPERTY(const char*, TreeNode, Font, GetFont, SetFont);

/// property: int Mark
/// \brief 节点文本
DECLARE_PROPERTY(int, TreeNode, Mark, GetMark, SetMark);
/// property: int ImageIndex
/// \brief 标记图片索引
DECLARE_PROPERTY(int, TreeNode, ImageIndex, GetImageIndex, SetImageIndex);
/// property: bool Expand
/// \brief 是否显示子节点
DECLARE_PROPERTY(bool, TreeNode, Expand, GetExpand, SetExpand);

/// property: bool ShowCoverImage
/// \brief 是否显示遮盖的图
DECLARE_PROPERTY(bool, TreeNode, ShowCoverImage, GetShowCoverImage,
	SetShowCoverImage);

/// property: string ForeColor
/// \brief 文本前景色
DECLARE_PROPERTY(result_string, TreeNode, ForeColor, GetForeColorString, 
	SetForeColorString);
/// property: string ShadowColor
/// \brief 描边/阴影色
DECLARE_PROPERTY(result_string, TreeNode, ShadowColor, GetShadowColorString, 
	SetShadowColorString);

// TreeNode

TreeNode* TreeNode::GetTreeNode(const IEntity* pEntity, const PERSISTID& id)
{
	TreeNode* node = (TreeNode*)pEntity->GetCore()->GetEntity(id);
	
	if (NULL == node)
	{
		return NULL;
	}
	
	if (!node->GetEntInfo()->IsKindOf("TreeNode"))
	{
		return NULL;
	}
	
	return node;
}

TreeNode::TreeNode()
{
	m_pParent = NULL;
	m_pTreeView = NULL; 
	m_nHash = 0;
	m_bExpand = false;
	m_nLevel = 0;
	m_nImageIndex = 0;
	m_ForeColor.SetValue(0);
	m_bShowCoverImage = false;
}

TreeNode::~TreeNode()
{
}

bool TreeNode::Init(const IVarList& args)
{
	return true;
}

bool TreeNode::Shut()
{
	ClearNode();
	
	return true;
}

void TreeNode::SetText(const wchar_t* value)
{
	Assert(value != NULL);

	m_wsText = value;
	m_nHash = GetHashValueCaseW(value);
	
	RefreshTreeView();
}

const wchar_t* TreeNode::GetText() const
{
	return m_wsText.c_str();
}

// 字体名
void TreeNode::SetFont(const char* value)
{
	m_szFont = value;
}

const char* TreeNode::GetFont() const
{
	return m_szFont.c_str();
}

// 节点标记值
void TreeNode::SetMark(const int nMark)
{
    m_nMark = nMark;
}

const int TreeNode::GetMark() const
{
    return m_nMark;
}

unsigned int TreeNode::GetHash() const
{
	return m_nHash;
}

void TreeNode::SetExpand(bool value)
{
	m_bExpand = value;
	
	RefreshTreeView();
}

bool TreeNode::GetExpand() const
{
	return m_bExpand;
}

void TreeNode::SetShowCoverImage(bool value)
{
	m_bShowCoverImage = value;
	
	RefreshTreeView();
}

bool TreeNode::GetShowCoverImage() const
{
	return m_bShowCoverImage;
}

void TreeNode::SetLevel(int value)
{
	m_nLevel = value;
}

int TreeNode::GetLevel() const
{
	return m_nLevel;
}

void TreeNode::SetImageIndex(int value)
{
	m_nImageIndex = value;
}

int TreeNode::GetImageIndex() const
{
	return m_nImageIndex;
}

void TreeNode::SetForeColorString(const char* value)
{
	Assert(value != NULL);

	m_ForeColor.SetName(m_pTreeView->GetGui(), value);
}

result_string TreeNode::GetForeColorString() const
{
	return m_ForeColor.GetName().c_str();
}

unsigned int TreeNode::GetForeColorValue() const
{
	return m_ForeColor.GetValue();
}

void TreeNode::ReleaseNode(TreeNode* node)
{
	if (node != NULL)
	{
		node->Release();
	}
}

bool TreeNode::FindNodeIndex(const PERSISTID& id, int& index) const
{
	for (size_t i = 0; i < m_Nodes.size(); i++)
	{
		if (m_Nodes[i]->GetID() == id)
		{
			index = (int)i;
			return true;
		}
	}
	
	return false;
}

// 字体描边色
void TreeNode::SetShadowColorString(const char* value)
{
	m_ShadowColor.SetName(m_pTreeView->GetGui(), value);
}

result_string TreeNode::GetShadowColorString() const
{
	return m_ShadowColor.GetName().c_str();
}

unsigned int TreeNode::GetShadowColorValue() const
{
	return m_ShadowColor.GetValue();
}

bool TreeNode::InsertNodeAt(TreeNode* node, int index)
{
	if (node->GetParent() != NULL)
	{
		CORE_TRACE("(TreeNode::InsertNodeAt)node already has parent");
		return false;
	}
	
	int k;
	
	if (FindNodeIndex(node->GetID(), k))
	{
		CORE_TRACE("(TreeNode::InsertNodeAt)node already is child");
		return false;
	}
	
	node->SetParent(this);
	node->SetTreeView(m_pTreeView);
	node->SetLevel(m_nLevel + 1);
	
	//m_Nodes.insert(m_Nodes.begin() + index, node);
	m_Nodes.insert(index, node);
	
	RefreshTreeView();
	
	return true;
}

PERSISTID TreeNode::CreateNode(const wchar_t* text)
{
	Assert(text != NULL);

	TreeNode* node = (TreeNode*)GetCore()->CreateEntity("TreeNode");
	
	if (NULL == node)
	{
		return PERSISTID();
	}
	
	node->SetText(text);
	
	if (!InsertNodeAt(node, (int)m_Nodes.size()))
	{
		return PERSISTID();
	}

	return node->GetID();
}

bool TreeNode::AddNode(const PERSISTID& id)
{
	TreeNode* node = GetTreeNode(this, id);
	
	if (NULL == node)
	{
		return false;
	}
	
	return InsertNodeAt(node, (int)m_Nodes.size());
}

bool TreeNode::InsertAfterNode(const PERSISTID& id, const PERSISTID& after)
{
	TreeNode* node = GetTreeNode(this, id);
	
	if (NULL == node)
	{
		return false;
	}
	
	int index;
	
	if (!FindNodeIndex(after, index))
	{
		return false;
	}
	
	return InsertNodeAt(node, index + 1);
}

bool TreeNode::InsertBeforeNode(const PERSISTID& id, const PERSISTID& before)
{
	TreeNode* node = GetTreeNode(this, id);
	
	if (NULL == node)
	{
		return false;
	}
	
	int index;

	if (!FindNodeIndex(before, index))
	{
		return false;
	}
	
	return InsertNodeAt(node, index);
}

bool TreeNode::RemoveNode(const PERSISTID& id)
{
	int index;
	
	if (!FindNodeIndex(id, index))
	{
		return false;
	}
	
	ReleaseNode(m_Nodes[index]);
	//m_Nodes.erase(m_Nodes.begin() + index);
	m_Nodes.remove(index);
	RefreshTreeView();
	
	return true;
}

bool TreeNode::ClearNode()
{
	for (size_t i = 0; i < m_Nodes.size(); i++)
	{
		ReleaseNode(m_Nodes[i]);
	}

	m_Nodes.clear();

	return true;
}

int TreeNode::GetNodeCount() const
{
	return (int)m_Nodes.size();
}

void TreeNode::GetNodeList(const IVarList& args, IVarList& result) const
{
	result.Clear();

	for (size_t i = 0; i < m_Nodes.size(); ++i)
	{
		result.AddObject(m_Nodes[i]->GetID());
	}
}

PERSISTID TreeNode::GetNode(int index) const
{
	if (size_t(index) >= m_Nodes.size())
	{
		return PERSISTID();
	}

	return m_Nodes[index]->GetID();
}

TreeNode* TreeNode::Get(int index) const
{
	if (size_t(index) >= m_Nodes.size())
	{
		return NULL;
	}
	
	return m_Nodes[index];
}

void TreeNode::SetParent(TreeNode* parent)
{
	m_pParent = parent;
}

TreeNode* TreeNode::GetParent() const
{
	return m_pParent;
}

void TreeNode::SetTreeView(TreeView* tree)
{
	m_pTreeView = tree;
}

TreeView* TreeNode::GetTreeView() const
{
	return m_pTreeView;
}

bool TreeNode::IsBack() const
{
	if (m_pParent == NULL)
	{
		return false;
	}

	return (m_pParent->Get(m_pParent->GetNodeCount() - 1) == this);
}

PERSISTID TreeNode::GetParentNode() const
{
	if (m_pParent == NULL)
	{
		return PERSISTID();
	}
	else
	{
		return m_pParent->GetID();
	}
}

PERSISTID TreeNode::FindNode(const wchar_t* text) const
{
	Assert(text != NULL);
	
	unsigned int hash = GetHashValueCaseW(text);
	
	for (size_t i = 0; i < m_Nodes.size(); i++)
	{
		if ((m_Nodes[i]->GetHash() == hash)
			&& (wcscmp(m_Nodes[i]->GetText(), text) == 0))
		{
			return m_Nodes[i]->GetID();
		}
	}
	
	return PERSISTID();
}

PERSISTID TreeNode::FindNodeByMark(const int nMark) const
{
    for (size_t i = 0; i < m_Nodes.size(); i++)
    {
        if (m_Nodes[i]->GetMark() == nMark)
        {
            return m_Nodes[i]->GetID();
        }
    }

    return PERSISTID();
}

bool TreeNode::ExpandAll()
{
	m_bExpand = true;

	for (size_t i = 0; i < m_Nodes.size(); i++)
	{
		m_Nodes[i]->ExpandAll();
	}

	RefreshTreeView();

	return true;
}

bool TreeNode::RefreshTreeView()
{
	if (m_pTreeView)
	{
		m_pTreeView->Refresh();
		return true;
	}

	if (m_pParent != NULL)
	{
		return m_pParent->RefreshTreeView();
	}

	return false;
}

