//--------------------------------------------------------------------
// �ļ���:		TreeNodeEx.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2011��3��7��
// ������:		��־��
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "tree_node_ex.h"
#include "tree_view_ex.h"
#include "../public/inlines.h"
#include "../public/core_log.h"

/// \file TreeNodeEx.cpp
/// \brief ���ڵ�

/// entity: TreeNode
/// \brief ���ڵ�ʵ��
DECLARE_ENTITY(TreeNodeEx, 0, TreeNode);

/// property: string NodeBackImage
/// \brief �ڵ㱳����ͼ
DECLARE_PROPERTY(const char*, TreeNodeEx, NodeBackImage, GetNodeBackImage, 
	SetNodeBackImage);
/// property: string NodeFocusImage
/// \brief �ڵ㶨λʱ����ͼ
DECLARE_PROPERTY(const char*, TreeNodeEx, NodeFocusImage, GetNodeFocusImage, 
	SetNodeFocusImage);
/// property: string NodeSelectImage
/// \brief �ڵ㰴��ʱ����ͼ
DECLARE_PROPERTY(const char*, TreeNodeEx, NodeSelectImage, GetNodeSelectImage, 
	SetNodeSelectImage);
/// property: string NodeCoverImage
/// \brief �ڵ��ڸǵ�ͼ
DECLARE_PROPERTY(const char*, TreeNodeEx, NodeCoverImage, GetNodeCoverImage, 
	SetNodeCoverImage);
/// property: string DrawMode
/// \brief ����ͼԪ����ʾ��ʽ
DECLARE_PROPERTY(const char*, TreeNodeEx, DrawMode, GetDrawModeString, 
	SetDrawModeString);

/// property: int TextOffsetX
/// \brief ������ʼλ��
DECLARE_PROPERTY(int, TreeNodeEx, TextOffsetX, GetTextOffsetX, SetTextOffsetX);
/// property: int TextOffsetX
/// \brief ������ʼλ��
DECLARE_PROPERTY(int, TreeNodeEx, TextOffsetY, GetTextOffsetY, SetTextOffsetY);
/// property: int TextOffsetX
/// \brief ������ʼλ��
DECLARE_PROPERTY(int, TreeNodeEx, ExpandCloseOffsetX, GetExpandCloseOffsetX, 
SetExpandCloseOffsetX);
/// property: int TextOffsetX
/// \brief ������ʼλ��
DECLARE_PROPERTY(int, TreeNodeEx, ExpandCloseOffsetY, GetExpandCloseOffsetY, 
SetExpandCloseOffsetY);
/// property: int TextOffsetX
/// \brief ������ʼλ��
DECLARE_PROPERTY(int, TreeNodeEx, NodeImageOffsetX, GetNodeImageOffsetX, 
SetNodeImageOffsetX);
/// property: int TextOffsetX
/// \brief ������ʼλ��
DECLARE_PROPERTY(int, TreeNodeEx, NodeImageOffsetY, GetNodeImageOffsetY, 
SetNodeImageOffsetY);
/// property: int ItemHeight
/// \brief �ڵ���ĸ߶�
DECLARE_PROPERTY(int, TreeNodeEx, ItemHeight, GetItemHeight, SetItemHeight);
/// property: int LevelWidth
/// \brief ÿ������֮��ļ��
DECLARE_PROPERTY(int, TreeNodeEx, LevelWidth, GetLevelWidth, SetLevelWidth);
/// property: int TextOffsetX
/// \brief �ڵ���ʼλ��
DECLARE_PROPERTY(int, TreeNodeEx, NodeOffsetY, GetNodeOffsetY, SetNodeOffsetY);




// TreeNodeEx

TreeNodeEx::TreeNodeEx()
{
	m_nItemHeight = 20;
	m_nLevelWidth = 10;
	m_nTextOffsetX = 2;
	m_nTextOffsetY = 0;
	m_nExpandCloseOffsetX = 0;
	m_nExpandCloseOffsetY = 0;
	m_nNodeImageOffsetX = 0;
	m_nNodeImageOffsetY = 0;
	m_nNodeOffsetY = 0;
	m_nDrawMode = CGuiConst::DRAWMODE_TILE;
}

TreeNodeEx::~TreeNodeEx()
{
}

bool TreeNodeEx::Init(const IVarList& args)
{
	return true;
}

bool TreeNodeEx::Shut()
{	
	m_NodeBackImage.Release();
	m_NodeFocusImage.Release();
	m_NodeSelectImage.Release();
	m_NodeCoverImage.Release();

	return TreeNode::Shut();
}

PERSISTID TreeNodeEx::CreateNode(const wchar_t* text)
{
	Assert(text != NULL);

	TreeNodeEx* node = (TreeNodeEx*)GetCore()->CreateEntity("TreeNodeEx");

	if (NULL == node)
	{
		return PERSISTID();
	}

	node->SetText(text);

	if (!InsertNodeAt(node, m_Nodes.size()))
	{
		return PERSISTID();
	}

	return node->GetID();
}

void TreeNodeEx::SetNodeBackImage(const char* value)
{
	m_NodeBackImage.SetName(value);
	m_pTreeView->CreateImagePlayer(m_NodeBackImage);
}

const char* TreeNodeEx::GetNodeBackImage() const
{
	return m_NodeBackImage.GetName();
}

void TreeNodeEx::SetNodeFocusImage(const char* value)
{
	m_NodeFocusImage.SetName(value);
	m_pTreeView->CreateImagePlayer(m_NodeFocusImage);
}

const char* TreeNodeEx::GetNodeFocusImage() const
{
	return m_NodeFocusImage.GetName();
}

void TreeNodeEx::SetNodeSelectImage(const char* value)
{
	m_NodeSelectImage.SetName(value);
	m_pTreeView->CreateImagePlayer(m_NodeSelectImage);
}

const char* TreeNodeEx::GetNodeSelectImage() const
{
	return m_NodeSelectImage.GetName();
}

void TreeNodeEx::SetNodeCoverImage(const char* value)
{
    m_NodeCoverImage.SetName(value);
    m_pTreeView->CreateImagePlayer(m_NodeCoverImage);
}

const char* TreeNodeEx::GetNodeCoverImage()
{
    return m_NodeCoverImage.GetName();
}

void TreeNodeEx::SetDrawModeString(const char* value)
{
	Assert(value != NULL);

	if (stricmp(value, "Tile") == 0)
	{
		m_nDrawMode = CGuiConst::DRAWMODE_TILE;
	}
	else if (stricmp(value, "Center") == 0)
	{
		m_nDrawMode = CGuiConst::DRAWMODE_CENTER;
	}
	else if (stricmp(value, "FitWindow") == 0)
	{
		m_nDrawMode = CGuiConst::DRAWMODE_FITWINDOW;
	}
	else if (stricmp(value, "Expand") == 0)
	{
		m_nDrawMode = CGuiConst::DRAWMODE_EXPAND;
	}
	else if (stricmp(value, "ExpandH") == 0)
	{
		m_nDrawMode = CGuiConst::DRAWMODE_EXPANDH;
	}
	else if (stricmp(value, "ExpandV") == 0)
	{
		m_nDrawMode = CGuiConst::DRAWMODE_EXPANDV;
	}
}

const char* TreeNodeEx::GetDrawModeString() const
{
	switch (m_nDrawMode)
	{
	case CGuiConst::DRAWMODE_TILE:
		return "Tile";
	case CGuiConst::DRAWMODE_CENTER:
		return "Center";
	case CGuiConst::DRAWMODE_FITWINDOW:
		return "FitWindow";
	case CGuiConst::DRAWMODE_EXPAND:
		return "Expand";
	case CGuiConst::DRAWMODE_EXPANDH:
		return "ExpandH";
	case CGuiConst::DRAWMODE_EXPANDV:
		return "ExpandV";
	default:
		Assert(0);
		break;
	}

	return "";
}

int TreeNodeEx::GetDrawMode() const
{
	return m_nDrawMode;
}

void TreeNodeEx::SetTextOffsetX(int value)
{
	if (value < 0)
	{
		return;
	}

	m_nTextOffsetX = value;
}

int TreeNodeEx::GetTextOffsetX() const
{
	return m_nTextOffsetX;
}

void TreeNodeEx::SetTextOffsetY(int value)
{
	if (value < 0)
	{
		return;
	}

	m_nTextOffsetY = value;
}

int TreeNodeEx::GetTextOffsetY() const
{
	return m_nTextOffsetY;
}

int TreeNodeEx::GetNodeOffsetY() const
{
	return m_nNodeOffsetY;
}

void TreeNodeEx::SetNodeOffsetY(int value)
{
	m_nNodeOffsetY = value;
}

void TreeNodeEx::SetExpandCloseOffsetX(int value)
{
	if (value < 0)
	{
		return;
	}

	m_nExpandCloseOffsetX = value;
}

int TreeNodeEx::GetExpandCloseOffsetX() const
{
	return m_nExpandCloseOffsetX;
}

void TreeNodeEx::SetExpandCloseOffsetY(int value)
{
	if (value < 0)
	{
		return;
	}

	m_nExpandCloseOffsetY = value;
}

int TreeNodeEx::GetExpandCloseOffsetY() const
{
	return m_nExpandCloseOffsetY;
}

// ������ʼXλ��
void TreeNodeEx::SetNodeImageOffsetX(int value)
{
	if (value < 0)
	{
		return;
	}

	m_nNodeImageOffsetX = value;
}

int TreeNodeEx::GetNodeImageOffsetX() const
{
	return m_nNodeImageOffsetX;
}

// ������ʼYλ��
void TreeNodeEx::SetNodeImageOffsetY(int value)
{
	m_nNodeImageOffsetY = value;
}

int TreeNodeEx::GetNodeImageOffsetY() const
{
	return m_nNodeImageOffsetY;
}

void TreeNodeEx::SetItemHeight(int value)
{
	if (value < MIN_ITEM_HEIGHT)
	{
		return;
	}

	m_nItemHeight = value;
}

int TreeNodeEx::GetItemHeight() const
{
	return m_nItemHeight;
}

void TreeNodeEx::SetLevelWidth(int value)
{
	if (value < MIN_LEVEL_WIDTH)
	{
		value = MIN_LEVEL_WIDTH;
	}

	m_nLevelWidth = value;
}

int TreeNodeEx::GetLevelWidth() const
{
	return m_nLevelWidth;
}