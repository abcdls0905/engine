//--------------------------------------------------------------------
// 文件名:		tree_view.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "tree_view.h"
#include "tree_node.h"
#include "gui_input.h"
#include "image_list.h"
#include "scroll_bar.h"

/// \file tree_view.cpp
/// \brief 树形控件

/// entity: TreeView
/// \brief 树形控件实体
/// \see 继承自IScrollable
DECLARE_ENTITY(TreeView, 0, IScrollable);

/// property: int ItemHeight
/// \brief 节点项的高度
DECLARE_PROPERTY(int, TreeView, ItemHeight, GetItemHeight, SetItemHeight);
/// property: int TextOffsetX
/// \brief 文字起始位移
DECLARE_PROPERTY(int, TreeView, TextOffsetX, GetTextOffsetX, SetTextOffsetX);
/// property: int TextOffsetY
/// \brief 文字起始Y位移
DECLARE_PROPERTY(int, TreeView, TextOffsetY, GetTextOffsetY, SetTextOffsetY);
/// property: int LevelWidth
/// \brief 每个级别之间的间距
DECLARE_PROPERTY(int, TreeView, LevelWidth, GetLevelWidth, SetLevelWidth);

/// property: bool IsDrawRoot
/// \brief 是否绘制根节点对像
DECLARE_PROPERTY(bool, TreeView, IsNoDrawRoot, GetIsNoDrawRoot, 
	SetIsNoDrawRoot);
/// property: bool IsLeftClickExpandClose
/// \brief 是否左键点击展开、关闭节点
DECLARE_PROPERTY(bool, TreeView, IsLeftClickExpandClose, GetIsLeftClickExpandClose, 
	SetIsLeftClickExpandClose);

/// property: int ExpandBoxSize
/// \brief 扩展子节点标记的大小
DECLARE_PROPERTY(int, TreeView, ExpandBoxSize, GetExpandBoxSize, 
	SetExpandBoxSize);
/// property: string SelectForeColor
/// \brief 被选中文本的前景色
DECLARE_PROPERTY(result_string, TreeView, SelectForeColor, 
	GetSelectForeColorString, SetSelectForeColorString);
/// property: string SelectBackColor
/// \brief 被选中文本的背景色
DECLARE_PROPERTY(result_string, TreeView, SelectBackColor, 
	GetSelectBackColorString, SetSelectBackColorString);
/// property: string TreeLineColor
/// \brief 连线的颜色
DECLARE_PROPERTY(result_string, TreeView, TreeLineColor, 
	GetTreeLineColorString, SetTreeLineColorString);
/// property: object RootNode
/// \brief 根节点对象
DECLARE_PROPERTY(PERSISTID, TreeView, RootNode, GetRootNode, SetRootNode);

/// property: object SelectNode
/// \brief 当前被选择的节点对象
DECLARE_PROPERTY(PERSISTID, TreeView, SelectNode, GetSelectNode, 
	SetSelectNode);
/// property: object ImageList
/// \brief 节点图标列表对象
DECLARE_PROPERTY(PERSISTID, TreeView, ImageList, GetImageListID, 
	SetImageListID);

/// property: string NodeExpandDraw
/// \brief 节点展开时的图
DECLARE_PROPERTY(const char*, TreeView, NodeExpandDraw, GetNodeExpandDraw, 
	SetNodeExpandDraw);
/// property: string NodeCloseDraw
/// \brief 节点收起时的图
DECLARE_PROPERTY(const char*, TreeView, NodeCloseDraw, GetNodeCloseDraw, 
	SetNodeCloseDraw);
/// property: string SelectBackDraw
/// \brief 选中背景
DECLARE_PROPERTY(const char*, TreeView, SelectBackDraw, GetSelectBackDraw, 
	SetSelectBackDraw);

/// property: string NodeBackImage
/// \brief 节点背景
DECLARE_PROPERTY(const char*, TreeView, NodeBackImage, GetNodeBackImage, 
	SetNodeBackImage);
/// property: string NodeFocusImage
/// \brief 节点鼠标移入时的图
DECLARE_PROPERTY(const char*, TreeView, NodeFocusImage, GetNodeFocusImage, 
	SetNodeFocusImage);
/// property: string NodeCoverImage
/// \brief 节点遮盖的图
DECLARE_PROPERTY(const char*, TreeView, NodeCoverImage, GetNodeCoverImage, 
	SetNodeCoverImage);

/// event: int on_select_changed(object self, object node, object old_node)
/// \brief 选择节点时的回调
/// \param self 本控件
/// \param node 当前被选择节点
/// \param old_node 上一次被选择节点
DECLARE_EVENT(on_select_changed);
/// event: int on_select_double_click(object self, object node)
/// \brief 双击节点时的回调
/// \param self 本控件
/// \param node 被选择节点
DECLARE_EVENT(on_select_double_click);
/// event: int on_left_click(object self, object node)
/// \brief 鼠标左键点击节点时的回调
/// \param self 本控件
/// \param node 被选择节点
DECLARE_EVENT(on_left_click);
/// event: int on_right_click(object self, object node)
/// \brief 鼠标右键点击节点时的回调
/// \param self 本控件
/// \param node 被选择节点
DECLARE_EVENT(on_right_click);

/// event: int on_expand_changed(object self, object node)
/// \brief 展开扣收起效态改变
/// \param self 本控件
/// \param node 被选择节点
DECLARE_EVENT(on_expand_changed);

// TreeView

TreeView::TreeView()
{
	m_pRootNode = NULL;
	m_nItemHeight = 16;
	m_nTextOffsetX = 2;
	m_nTextOffsetY = 0;
	m_nLevelWidth = 18;
	m_nExpandBoxSize = 9;
	m_nTopIndex = 0;
    m_nScrollWidth = 0;
    m_nScrollLeft = 0;
	m_SelectForeColor.SetValue(CGuiConst::SELECT_FORE_COLOR);
	m_SelectBackColor.SetValue(CGuiConst::SELECT_BACK_COLOR);
	m_TreeLineColor.SetValue(TREE_LINE_COLOR);
    m_IsNoDrawRoot = false;
	m_IsLeftClickExpandClose = false;

	SetCanFocus(true);
	SetTabStop(true);
}

TreeView::~TreeView()
{
}

bool TreeView::Init(const IVarList& args)
{
	if (!IScrollable::Init(args))
	{
		return false;
	}

	SetHasVScroll(true);
	
	return true;
}

bool TreeView::Shut()
{
    m_NodeExpandDraw.Release();
    m_NodeCloseDraw.Release();
    m_ImageSelectBar.Release();
	m_NodeBackImage.Release();
	m_NodeFocusImage.Release();
	m_NodeCoverImage.Release();
	
	ReleaseRootNode();

	return IScrollable::Shut();
}

void TreeView::PrepareDesignInfo(IDesignInfo* di)
{
	IScrollable::PrepareDesignInfo(di);

	di->AddPropInfo("TabIndex", IDesignInfo::TYPE_INT, true, false);
	di->AddPropInfo("TabStop", IDesignInfo::TYPE_BOOL, true, false);
	di->AddPropInfo("IsNoDrawRoot", IDesignInfo::TYPE_BOOL, true, false);
	di->AddPropInfo("IsLeftClickExpandClose", IDesignInfo::TYPE_BOOL, true, false);

    CVar v(VTYPE_BOOL, true);
	di->AddPropInfo("Solid", IDesignInfo::TYPE_BOOL, true, true,
		&v);
	di->AddPropInfo("ScrollSize", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("AlwaysVScroll", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("AlwaysHScroll", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("HasHScroll", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("VScrollLeft", IDesignInfo::TYPE_BOOL, true, true);
	
	di->AddPropInfo("ItemHeight", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("TextOffsetX", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("TextOffsetY", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("LevelWidth", IDesignInfo::TYPE_INT, true, true);

    v = CVar(VTYPE_STRING, GuiUtil_IntToColor(CGuiConst::SELECT_FORE_COLOR).c_str());
	di->AddPropInfo("SelectForeColor", IDesignInfo::TYPE_COLOR, true, true, &v );
    v = CVar(VTYPE_STRING, GuiUtil_IntToColor(CGuiConst::SELECT_BACK_COLOR).c_str());
	di->AddPropInfo("SelectBackColor", IDesignInfo::TYPE_COLOR, true, true, &v);
    v = CVar(VTYPE_STRING, GuiUtil_IntToColor(TREE_LINE_COLOR).c_str());
	di->AddPropInfo("TreeLineColor", IDesignInfo::TYPE_COLOR, true, true, &v );
	
    di->AddPropInfo("NodeExpandDraw", IDesignInfo::TYPE_IMAGE, true, true);
    di->AddPropInfo("NodeCloseDraw", IDesignInfo::TYPE_IMAGE, true, true);
    di->AddPropInfo("SelectBackDraw", IDesignInfo::TYPE_IMAGE, true, true);
	
	di->AddPropInfo("NodeBackImage", IDesignInfo::TYPE_IMAGE, true, true);
	di->AddPropInfo("NodeFocusImage", IDesignInfo::TYPE_IMAGE, true, true);
	di->AddPropInfo("NodeCoverImage", IDesignInfo::TYPE_IMAGE, true, true);

	di->AddSuborInfo("HScrollBar", "ScrollBar");
	di->AddSuborInfo("VScrollBar", "ScrollBar");
	di->AddSuborInfo("CornerLabel", "Label");

	di->AddEventInfo("on_select_changed");
	di->AddEventInfo("on_select_double_click");
	di->AddEventInfo("on_left_click");
	di->AddEventInfo("on_right_click");
    di->AddEventInfo("on_expand_changed");
	di->AddEventInfo("on_mouse_in_node");
	di->AddEventInfo("on_mouse_out_node");
}

void TreeView::CreateImagePlayer(CImagePlayer& rImagePlayer)
{
	rImagePlayer.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

bool TreeView::GetLoadFinish()
{
    if (GetLoadFinishValue())
    {
        return true;
    }

	if (!m_NodeExpandDraw.IsLoadCompelte())
	{
		return false;
	}

	if (!m_NodeCloseDraw.IsLoadCompelte())
	{
		return false;
	}

	if (!m_ImageSelectBar.IsLoadCompelte())
	{
		return false;
	}
	
	if (!m_NodeBackImage.IsLoadCompelte())
	{
		return false;
	}

	if (!m_NodeFocusImage.IsLoadCompelte())
	{
		return false;
	}

	if (!m_NodeCoverImage.IsLoadCompelte())
	{
		return false;
	}

	return IScrollable::GetLoadFinish();
}

void TreeView::SetItemHeight(int value)
{
	if (value < MIN_ITEM_HEIGHT)
	{
		value = MIN_ITEM_HEIGHT;
	}

	m_nItemHeight = value;
}

int TreeView::GetItemHeight() const
{
	return m_nItemHeight;
}

void TreeView::SetTextOffsetX(int value)
{
	if (value < 0)
	{
		return;
	}
	
	m_nTextOffsetX = value;
}

int TreeView::GetTextOffsetX() const
{
	return m_nTextOffsetX;
}

void TreeView::SetTextOffsetY(int value)
{
	m_nTextOffsetY = value;
}

int TreeView::GetTextOffsetY() const
{
	return m_nTextOffsetY;
}

void TreeView::SetLevelWidth(int value)
{
	if (value < MIN_LEVEL_WIDTH)
	{
		value = MIN_LEVEL_WIDTH;
	}

	m_nLevelWidth = value;
}

int TreeView::GetLevelWidth() const
{
	return m_nLevelWidth;
}

void TreeView::SetExpandBoxSize(int value)
{
	if ((value > m_nLevelWidth) || (value < 5))
	{
		return;
	}
	
	m_nExpandBoxSize = value;
}

int TreeView::GetExpandBoxSize() const
{
	return m_nExpandBoxSize;
}

void TreeView::SetSelectForeColorString(const char* value)
{
	Assert(value != NULL);
	
	m_SelectForeColor.SetName(GetGui(), value);
}

result_string TreeView::GetSelectForeColorString() const
{
	return m_SelectForeColor.GetName().c_str();
}

void TreeView::SetSelectBackColorString(const char* value)
{
	Assert(value != NULL);
	
	m_SelectBackColor.SetName(GetGui(), value);
}

result_string TreeView::GetSelectBackColorString() const
{
	return m_SelectBackColor.GetName().c_str();
}

void TreeView::SetTreeLineColorString(const char* value)
{
	Assert(value != NULL);

	m_TreeLineColor.SetName(GetGui(), value);
}

result_string TreeView::GetTreeLineColorString() const
{
	return m_TreeLineColor.GetName().c_str();
}

void TreeView::SetRootNode(const PERSISTID& value)
{
	TreeNode* node = TreeNode::GetTreeNode(this, value);
	
	if (NULL == node)
	{
		return;
	}
	
	ReleaseRootNode();
	
	// 重新设置树的级别
	int level = 0;

	SetNodeLevel(node, level);
	
	m_pRootNode = node;
}

PERSISTID TreeView::GetRootNode() const
{
	if (NULL == m_pRootNode)
	{
		return PERSISTID();
	}
	else
	{
		return m_pRootNode->GetID();
	}
}

bool TreeView::ShowSelectItem(TreeNode* node)
{
	int select_index = GetShowNodeIndex(node);
	
	if (select_index == -1)
	{
		return false;
	}
	
	if (select_index == m_nTopIndex)
	{
		return true;
	}
	
	if (select_index < m_nTopIndex)
	{
		SetVerticalValue(select_index);
		return true;
	}
	
	int page_disp = GetCanDispCount();
	
	if (select_index < (m_nTopIndex + page_disp))
	{
		return true;
	}
	
	int new_index = select_index - (page_disp - 1);
	
	if (new_index < 0)
	{
		new_index = 0;
	}
	
	SetVerticalValue(new_index);
	
	return true;
}

void TreeView::SetSelectNode(const PERSISTID& value)
{
	TreeNode* node = TreeNode::GetTreeNode(this, value);

	if (NULL == node)
	{
		return;
	}
	
	if (node->GetTreeView() != this)
	{
		return;
	}

	PERSISTID old_value = m_SelectNode;
	
	m_SelectNode = value;
	
	ShowSelectItem(node);
	
	UpdateScroll();

	if (old_value != value)
	{
		if (GetEnabled())
		{
			GuiUtil_RunCallback(this, "on_select_changed", 
				CVarList() << value << old_value);
		}
	}
}

PERSISTID TreeView::GetSelectNode() const
{
	TreeNode* node = TreeNode::GetTreeNode(this, m_SelectNode);
	
	if (NULL == node)
	{
		return PERSISTID();
	}

	return m_SelectNode;
}

void TreeView::SetImageListID(const PERSISTID& value)
{
	IEntity* pEntity = GetCore()->GetEntity(value);

	if (NULL == pEntity)
	{
		return;
	}

	if (!pEntity->GetEntInfo()->IsKindOf("ImageList"))
	{
		return;
	}

	m_ImageListID = value;
}

PERSISTID TreeView::GetImageListID() const
{
	return m_ImageListID;
}

void TreeView::SetIsNoDrawRoot(bool IsDraw)
{
    m_IsNoDrawRoot = IsDraw;
}

bool TreeView::GetIsNoDrawRoot() const
{
    return m_IsNoDrawRoot;
}

void TreeView::SetIsLeftClickExpandClose(bool IsExpandClose)
{
	m_IsLeftClickExpandClose = IsExpandClose;
}

bool TreeView::GetIsLeftClickExpandClose() const
{
	return m_IsLeftClickExpandClose;
}

// 设置背中背景
void TreeView::SetSelectBackDraw(const char* value)
{
    m_ImageSelectBar.SetName(value);
    m_ImageSelectBar.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* TreeView::GetSelectBackDraw()
{
    return m_ImageSelectBar.GetName();
}

void TreeView::SetNodeExpandDraw(const char* value)
{
    m_NodeExpandDraw.SetName(value);
    m_NodeExpandDraw.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* TreeView::GetNodeExpandDraw()
{
    return m_NodeExpandDraw.GetName();
}

void TreeView::SetNodeCloseDraw(const char* value)
{
    m_NodeCloseDraw.SetName(value);
    m_NodeCloseDraw.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* TreeView::GetNodeCloseDraw()
{
    return m_NodeCloseDraw.GetName();
}

void TreeView::SetNodeBackImage(const char* value)
{
    m_NodeBackImage.SetName(value);
    m_NodeBackImage.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* TreeView::GetNodeBackImage()
{
	return m_NodeBackImage.GetName();
}

void TreeView::SetNodeFocusImage(const char* value)
{
    m_NodeFocusImage.SetName(value);
    m_NodeFocusImage.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* TreeView::GetNodeFocusImage()
{
    return m_NodeFocusImage.GetName();
}

void TreeView::SetNodeCoverImage(const char* value)
{
    m_NodeCoverImage.SetName(value);
    m_NodeCoverImage.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* TreeView::GetNodeCoverImage()
{
    return m_NodeCoverImage.GetName();
}

void TreeView::ReleaseRootNode()
{
	SAFE_RELEASE(m_pRootNode);
}

PERSISTID TreeView::CreateRootNode(const wchar_t* text)
{
	TreeNode* root = (TreeNode*)GetCore()->CreateEntity("TreeNode");
	
	if (NULL == root)
	{
		return PERSISTID();
	}

	root->SetText(text);
	root->SetTreeView(this);
	root->SetLevel(0);
	
	ReleaseRootNode();
	
	m_pRootNode = root;
	
	return m_pRootNode->GetID();
}

void TreeView::Refresh()
{
    //刷新控件宽度，判断是否要横向滚动
    {
        m_nScrollWidth = 0;
        m_nScrollLeft = 0;

        int x1 = GetContentLeft();
        int y1 = GetContentTop();

        ImageList* pImageList = (ImageList*)GetCore()->GetEntity(
			m_ImageListID);

        GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());

        int index = 0;

        CalcScrollTotal(pImageList, x1, y1, m_pRootNode, index);
    }

    CalcRootShowNodes();
	UpdateScroll();
}

int TreeView::CalcShowNodes(TreeNode* node, node_container_t& shows)
{
	if (NULL == node)
	{
		return 0;
	}
	
	shows.push_back(node);
	
	int count = 1;
	
	if (node->GetExpand())
	{
		int num = node->GetNodeCount();
		
		for (int i = 0; i < num; i++)
		{
			count += CalcShowNodes(node->Get(i), shows);
		}
	}
	
	return count;
}

int TreeView::CalcRootShowNodes()
{
	m_ShowNodes.clear();

	return CalcShowNodes(m_pRootNode, m_ShowNodes);
}

int TreeView::GetNodeRows(TreeNode* node) const
{
	if (NULL == node)
	{
		return 0;
	}
	
    int count = 1;
	
	if (node->GetExpand())
	{
		int num = node->GetNodeCount();

		for (int i = 0; i < num; i++)
		{
			count += GetNodeRows(node->Get(i));
		}
	}

	return count;
}

void TreeView::SetNodeLevel(TreeNode* node, int& level)
{
	Assert(node != NULL);

	node->SetLevel(level);

	level++;
	
	int num = node->GetNodeCount();

	for (int i = 0; i < num; i++)
	{
		SetNodeLevel(node->Get(i), level);
	}

	level--;
}

int TreeView::GetCanDispCount() const
{
	return GetContentHeight() / m_nItemHeight;
}

void TreeView::CalcScrollTotal(ImageList* pImageList, int x1, int y1, 
    TreeNode* node, int& index)
{
    if (NULL == node)
    {
        return;
    }

    if ((NULL == node->GetParent() && !m_IsNoDrawRoot) || 
        NULL != node->GetParent())
    {
        int x;
        int y;

        if (NULL == node->GetParent())
        {
            x = x1;
        }
        else
        {
            x = x1 + (node->GetLevel() - 1) * m_nLevelWidth;
        }

        y = y1 + (index - m_nTopIndex) * m_nItemHeight;

        if (node->GetParent() != NULL)
        {
            x += m_nLevelWidth;
        }

        int w = GetPainter()->GetTextWidth(node->GetText());

        if (m_nScrollWidth < x - x1 + w)
        {
            m_nScrollWidth = x - x1 + w;
        }
    }

    if (node->GetExpand())
    {
        int num = node->GetNodeCount();

        for (int i = 0; i < num; i++)
        {
            CalcScrollTotal(pImageList, x1, y1, node->Get(i), index);
        }
    }
}


int TreeView::GetVerticalTotal() const
{
	if (m_IsNoDrawRoot) 
	{
		int value = GetNodeRows(m_pRootNode);

		if (value > 0)
		{
			return  value - 1;
		}

		return 0;
		
	}
	else
	{
		return GetNodeRows(m_pRootNode);
	}
}

int TreeView::GetVerticalCanDisp() const
{
	return GetCanDispCount();
}

void TreeView::SetVerticalValue(int value)
{
	m_nTopIndex = value;
}

int TreeView::GetVerticalValue() const
{
	return m_nTopIndex;
}

int TreeView::GetHorizonTotal() const
{
	return m_nScrollWidth;
}

int TreeView::GetHorizonCanDisp() const
{
	return GetContentWidth();
}

void TreeView::SetHorizonValue(int value)
{
    m_nScrollLeft = value;
}

int TreeView::GetHorizonValue() const
{
	return m_nScrollLeft;
}

void TreeView::GetShowNodeList(const IVarList& args, IVarList& result) const
{
	result.Clear();

	if (m_pRootNode == NULL)
	{
		return;
	}

	AddShowNode(m_pRootNode, result);
}

void TreeView::AddShowNode(const TreeNode* pNode, IVarList& result) const
{
	if (pNode == NULL)
	{
		return;
	}

	result.AddObject(pNode->GetID());

	if (pNode->GetExpand())
	{
		for (int i = 0; i < pNode->GetNodeCount(); ++i)
		{
			PERSISTID sub_node = pNode->GetNode(i);

			TreeNode* pSubNode = (TreeNode*)GetCore()->GetEntity(sub_node);

			if (pSubNode != NULL)
			{
				AddShowNode(pSubNode, result);
			}
		}
	}
}

void TreeView::GetAllNodeList(const IVarList& args, IVarList& result) const
{
	result.Clear();

	if (m_pRootNode == NULL)
	{
		return;
	}

	AddShowNode(m_pRootNode, result);
}

void TreeView::AddAllNode(const TreeNode* pNode, IVarList& result) const
{
	if (pNode == NULL)
	{
		return;
	}

	result.AddObject(pNode->GetID());

	for (int i = 0; i < pNode->GetNodeCount(); ++i)
	{
		PERSISTID sub_node = pNode->GetNode(i);

		TreeNode* pSubNode = (TreeNode*)GetCore()->GetEntity(sub_node);

		if (pSubNode != NULL)
		{
			AddShowNode(pSubNode, result);
		}
	}
}
/*
int TreeView::GetNodeShowLine(PERSISTID node)
{
	if (m_pRootNode == NULL)
	{
		return -1
	}

	if (m_pRootNode->GetID() == node)
	{
		return 0;
	}

	TreeNode* pNode = (TreeNode*)GetCore()->GetEntity(node);
	if (pNode == NULL)
	{
		return -1;
	}

	int linecount = 0;

	while(pNode != NULL)
	{
		PERSISTID parent_node = pNode->GetParentNode();

		TreeNode* pParentNode = (TreeNode*)GetCore()->GetEntity(parent_node);

		if (pParentNode == NULL)
		{
			if (pParentNode->GetExpand())
			{
				for (int i = 0; i < pParentNode->GetNodeCount(); ++i)
				{
					if (pParentNode->GetNode(i) != node)
					{
						++linecount;
					}
				}
			}
			else
			{
				return -1;
			}

			node = parent_node;
			pNode = pParentNode;
		}
		else
		{
			return 0;
		}
	}

	return linecount;
}*/

bool TreeView::DrawNodeLine(ImageList* pImageList, int x1, int y1, 
	TreeNode* node, int& index)
{
    if (m_TreeLineColor.GetValue() < 0x00ffffff)
    {
        // 透明的话就不需要再画了
        return false;
    }

	if (NULL == node)
	{
		return false;
	}

	if (index >= m_nTopIndex)
	{
        if ((NULL == node->GetParent() && !m_IsNoDrawRoot) || 
            NULL != node->GetParent())
        {
		    int x;
            int y;

            if (NULL == node->GetParent())
	        {
		        x = x1;
	        }
	        else
	        {
		        x = x1 + (node->GetLevel() - 1) * m_nLevelWidth;
	        }

            y = y1 + (index - m_nTopIndex) * m_nItemHeight;
    		
    		
		    if (node->GetParent() != NULL)
		    {
			    GetPainter()->DrawLine(
				    x + m_nLevelWidth / 2, y + m_nItemHeight / 2,
				    x + m_nLevelWidth, y + m_nItemHeight / 2, 
				    m_TreeLineColor.GetValue());
    			
			    if (node->IsBack())
			    {
				    GetPainter()->DrawLine(x + m_nLevelWidth / 2, y,
					    x + m_nLevelWidth / 2, y + m_nItemHeight / 2, 
					    m_TreeLineColor.GetValue());
			    }
			    else
			    {
				    GetPainter()->DrawLine(x + m_nLevelWidth / 2, y,
					    x + m_nLevelWidth / 2, y + m_nItemHeight, 
					    m_TreeLineColor.GetValue());
			    }
    			
			    if (node->GetNodeCount() > 0)
			    {
				    const int EXPAND_SIZE = m_nExpandBoxSize;
    				
				    int x2 = x + (m_nLevelWidth - EXPAND_SIZE + 1) / 2;
				    int y2 = y + (m_nItemHeight - EXPAND_SIZE + 1) / 2;
    				
                    if (!m_NodeCloseDraw.IsReady() 
						|| !m_NodeExpandDraw.IsReady())
                    {
                        GetPainter()->FillRect(x2, y2, x2 + EXPAND_SIZE, 
							y2 + EXPAND_SIZE, GetBackColor());
                        GetPainter()->DrawRect(x2, y2, x2 + EXPAND_SIZE, 
							y2 + EXPAND_SIZE, m_TreeLineColor.GetValue());
                        GetPainter()->DrawLine(x2 + 1, y2 + EXPAND_SIZE / 2,
                            x2 + EXPAND_SIZE - 1, y2 + EXPAND_SIZE / 2, 
                            GetLineColor());

                        if (!node->GetExpand())
                        {
                            GetPainter()->DrawLine(x2 + EXPAND_SIZE / 2, 
								y2 + 1, x2 + EXPAND_SIZE / 2, 
								y2 + EXPAND_SIZE - 1, GetLineColor());
                        }
                    }

			    }
    			
			    x += m_nLevelWidth;
		    }

		    if (pImageList != NULL)
		    {
			    int image = node->GetImageIndex();

			    CImagePlayer* pImage = pImageList->GetImageByIndex(image);

			    if (pImage != NULL)
			    {
				    x += pImage->GetWidth();
			    }
		    }
        }
	}
	
    if ((NULL == node->GetParent() && !m_IsNoDrawRoot) || 
        NULL != node->GetParent())
    {
	    if (++index > (m_nTopIndex + GetCanDispCount()))
	    {
		    return true;
	    }
    }
	
	if (node->GetExpand())
	{
		int old_index = index;
		
		int num = node->GetNodeCount();
		
		for (int i = 0; i < num; i++)
		{
			DrawNodeLine(pImageList, x1, y1, node->Get(i), index);
		}

		int offset = index - old_index;

		if ((offset > 0) && (node->GetParent() != NULL) && (!node->IsBack()))
		{
			int x;
			
			if (NULL == node->GetParent())
			{
				x = x1;
			}
			else
			{
				x = x1 + (node->GetLevel() - 1) * m_nLevelWidth;
			}
			
			int y = y1 + (old_index - 1 - m_nTopIndex) * m_nItemHeight;
			
			GetPainter()->DrawLine(x + m_nLevelWidth / 2, 
				y + m_nItemHeight, x + m_nLevelWidth / 2, 
				y + m_nItemHeight + offset* m_nItemHeight, 
				m_TreeLineColor.GetValue());
		}
	}
	
	return true;
}

bool TreeView::DrawNodeText(ImageList* pImageList, int x1, int y1, 
	TreeNode* node, int& index)
{
	if (NULL == node)
	{
		return false;
	}

	GetGui()->SetFont(node->GetFont(), node->GetShadowColorValue());

	if (index >= m_nTopIndex)
	{
        if ((NULL == node->GetParent() && !m_IsNoDrawRoot) || 
            NULL != node->GetParent())
        {
		    int x;
            int y;

            if (NULL == node->GetParent())
            {
                x = x1;
            }
            else
            {
                x = x1 + (node->GetLevel() - 1) * m_nLevelWidth;
            }

            y = y1 + (index - m_nTopIndex) * m_nItemHeight;

            if (node->GetParent() != NULL)
            {
                x += m_nLevelWidth;
            }

            if (pImageList != NULL)
            {
                int image = node->GetImageIndex();

                CImagePlayer* pImage = pImageList->GetImageByIndex(image);

                if (pImage != NULL)
                {
                    x += pImage->GetWidth();
                }
            }

            unsigned int fore_color;

			// 计算节点背景图片的位置
			int left = InnerGetAbsLeft();
			int node_x1 = left;
			int node_y1 = y;
			int node_x2 = left + GetWidth();
			int node_y2 = y + m_nItemHeight;

			// 滚动条显示
			if (GetAlwaysVScroll() || (m_pVScrollBar && m_pVScrollBar->GetVisible()))
			{
				// 滚动条在右边显示
				if (!GetVScrollLeft())
				{
					node_x2 = left + GetWidth() - m_pVScrollBar->GetWidth();
				}
				// 滚动条在左边显示
				else
				{
					node_x1 = left + m_pVScrollBar->GetWidth();
					node_x2 = left + GetWidth();
				}
			}

            if (node->GetID() == m_SelectNode)
            {
                if (m_ImageSelectBar.IsReady())
                {
					m_ImageSelectBar.Draw(CGuiConst::DRAWMODE_FITWINDOW, 
						node_x1, node_y1, node_x2, node_y2);
                    
                    if (node->GetForeColorValue() > 0)
                    {
                        fore_color = node->GetForeColorValue();
                    }
                    else
                    {
                        fore_color = GetForeColor();
                    }
                }
                else
                {
                    int h = GetPainter()->GetTextHeight(node->GetText());
                    int w = GetPainter()->GetTextWidth(node->GetText());

                    GetPainter()->FillRect(x, y, x + w, y + h, 
						m_SelectBackColor.GetValue());

                    fore_color = m_SelectForeColor.GetValue();
                }
            }
			else if (node->GetID() == m_FocusNode)
			{
				if (m_NodeFocusImage.IsReady())
                {
					m_NodeFocusImage.Draw(CGuiConst::DRAWMODE_FITWINDOW, 
						node_x1, node_y1, node_x2, node_y2);
				}

                if (node->GetForeColorValue() > 0)
                {
                    fore_color = node->GetForeColorValue();
                }
                else
                {
                    fore_color = GetForeColor();
                }
			}
			else
			{
				if (m_NodeBackImage.IsReady())
                {
					m_NodeBackImage.Draw(CGuiConst::DRAWMODE_FITWINDOW, 
						node_x1, node_y1, node_x2, node_y2);
				}

                if (node->GetForeColorValue() > 0)
                {
                    fore_color = node->GetForeColorValue();
                }
                else
                {
                    fore_color = GetForeColor();
                }
			}

            GetPainter()->WriteText(x + m_nTextOffsetX, y + m_nTextOffsetY, fore_color, 
				node->GetText());

			if (node->GetShowCoverImage())
			{
				if (m_NodeCoverImage.IsReady())
				{
					m_NodeCoverImage.Draw(CGuiConst::DRAWMODE_FITWINDOW, 
						node_x1, node_y1, node_x2, node_y2);
				}
			}
        }
	}
	
    if ((NULL == node->GetParent() && !m_IsNoDrawRoot) || 
        NULL != node->GetParent())
    {
        if (++index > (m_nTopIndex + GetCanDispCount()))
	    {
		    return true;
	    }
    }
	
	if (node->GetExpand())
	{
		int num = node->GetNodeCount();
		
		for (int i = 0; i < num; i++)
		{
			DrawNodeText(pImageList, x1, y1, node->Get(i), index);
		}
	}

	return true;
}

bool TreeView::DrawExpandAndCloseImage(int x1, int y1, TreeNode* node, 
	int& index)
{
    if (NULL == node)
    {
        return false;
    }

    if (index >= m_nTopIndex)
    {
        if (NULL != node->GetParent())
        {
            int x;

            if (NULL == node->GetParent())
            {
                x = x1;
            }
            else
            {
                x = x1 + (node->GetLevel() - 1) * m_nLevelWidth;
            }

            int y = y1 + (index - m_nTopIndex) * m_nItemHeight;

            if (node->GetNodeCount() > 0 && m_NodeExpandDraw.IsReady() 
				&& m_NodeCloseDraw.IsReady())
            {
                if(node->GetExpand())
                {
                    int x2 = x + (m_nLevelWidth 
						- m_NodeExpandDraw.GetWidth() + 1) / 2;
                    int y2 = y + (m_nItemHeight 
						- m_NodeExpandDraw.GetHeight() + 1) / 2;

                    m_NodeExpandDraw.Draw(CGuiConst::DRAWMODE_TILE, x2, y2, 
                        x2 + m_NodeExpandDraw.GetWidth(), 
						y2 + m_NodeExpandDraw.GetHeight());
                }
                else
                {
                    int x2 = x + (m_nLevelWidth 
						- m_NodeCloseDraw.GetWidth() + 1) / 2;
                    int y2 = y + (m_nItemHeight 
						- m_NodeCloseDraw.GetHeight() + 1) / 2;

                    m_NodeCloseDraw.Draw(CGuiConst::DRAWMODE_TILE, x2, y2, 
                        x2 + m_NodeCloseDraw.GetWidth(), 
						y2 + m_NodeCloseDraw.GetHeight());
                }
            }
        }
    }


    if ((NULL == node->GetParent() && !m_IsNoDrawRoot) || 
        NULL != node->GetParent())
    {
        if (++index > (m_nTopIndex + GetCanDispCount()))
        {
            return true;
        }
    }


    if (node->GetExpand())
    {
        int num = node->GetNodeCount();

        for (int i = 0; i < num; i++)
        {
            DrawExpandAndCloseImage(x1, y1, node->Get(i), index);
        }
    }
    
    return true;
}

bool TreeView::DrawNodeImage(ImageList* pImageList, int x1, int y1, 
	TreeNode* node, int& index)
{
	if (NULL == node)
	{
		return false;
	}

	if (index >= m_nTopIndex)
	{
        if ((NULL == node->GetParent() && !m_IsNoDrawRoot) || 
            NULL != node->GetParent())
        {
	        int x;

	        if (NULL == node->GetParent())
	        {
		        x = x1;
	        }
	        else
	        {
		        x = x1 + (node->GetLevel() - 1) * m_nLevelWidth;
	        }

	        int y = y1 + (index - m_nTopIndex) * m_nItemHeight;

	        if (node->GetParent() != NULL)
	        {
		        x += m_nLevelWidth;
	        }
    		
	        if (pImageList != NULL)
	        {
		        int image = node->GetImageIndex();

		        CImagePlayer* pImage = pImageList->GetImageByIndex(image);
    			
		        if (pImage != NULL)
		        {
			        int w = pImage->GetWidth();

			        pImage->Draw(CGuiConst::DRAWMODE_TILE,
				        x, y, x + w, y + m_nItemHeight);
    		
			        x += w;
		        }
	        }
        }
	}
	
    if ((NULL == node->GetParent() && !m_IsNoDrawRoot) || 
        NULL != node->GetParent())
    {
	    if (++index > (m_nTopIndex + GetCanDispCount()))
	    {
		    return true;
	    }
    }
	

	if (node->GetExpand())
	{
		int num = node->GetNodeCount();
		
		for (int i = 0; i < num; i++)
		{
			DrawNodeImage(pImageList, x1, y1, node->Get(i), index);
		}
	}
	
	return true;
}

void TreeView::PaintScrollRect(float seconds)
{
	CalcRootShowNodes();
	UpdateScroll();
	
	int x1 = GetContentLeft();
	int y1 = GetContentTop();
	
    x1 = x1 - m_nScrollLeft;

	ImageList* pImageList = (ImageList*)GetCore()->GetEntity(m_ImageListID);
	
	GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());
	
	// 分开画线、文字和贴图，避免渲染状态频繁切换
    int index = 0;

    DrawNodeLine(pImageList, x1, y1, m_pRootNode, index);

    index = 0;

	DrawNodeText(pImageList, x1, y1, m_pRootNode, index);
	
    index = 0;

    DrawExpandAndCloseImage(x1, y1, m_pRootNode, index);

	if (pImageList != NULL)
	{
        index = 0;

        DrawNodeImage(pImageList, x1, y1, m_pRootNode, index);
	}
}

int TreeView::GetShowNodeIndex(TreeNode* node) const
{
	for (size_t i = 0; i < m_ShowNodes.size(); ++i)
	{
		if (node == m_ShowNodes[i])
		{
			return int(i);
		}
	}
	
	return -1;
}

TreeNode* TreeView::GetShowIndexNode(int index) const
{
	if (size_t(index) >= m_ShowNodes.size())
	{
		return NULL;
	}

	return m_ShowNodes[index];
}

TreeNode* TreeView::GetNodeByIndex(TreeNode* node, int node_index, 
	int& index) const
{
	if (node == NULL)
	{
		return NULL;
	}

	if (index == node_index)
	{
		return node;
	}

	index++;
	
	if (node->GetExpand())
	{
		int num = node->GetNodeCount();

		for (int i = 0; i < num; i++)
		{
			TreeNode* res = GetNodeByIndex(node->Get(i), node_index, index);

			if (res != NULL)
			{
				return res;
			}
		}
	}

	return NULL;
}

TreeNode* TreeView::GetInNode(int x, int y) const
{
	int t = GetContentTop();
	
	int num = GetCanDispCount() + 1;
	
	for (int i = 0; i < num; i++)
	{
		if ((y >= t) && (y <= (t + m_nItemHeight)))
		{
			int index = 0;

			if (!m_IsNoDrawRoot)
            {
                return GetNodeByIndex(m_pRootNode, m_nTopIndex + i, index);
            }
            else
            {
                return GetNodeByIndex(m_pRootNode, m_nTopIndex + i + 1, index);
            }
		}
		
		t += m_nItemHeight;
	}
	
	return NULL;
}

int TreeView::DoubleClickNode(TreeNode* node)
{
	if (node != NULL)
	{
		if (node->GetNodeCount() > 0)
		{
			node->SetExpand(!node->GetExpand());
		}
		
        GuiUtil_RunCallback(this, "on_expand_changed", 
			CVarList() << node->GetID());
        
		return GuiUtil_RunCallback(this, "on_select_double_click", 
			CVarList() << node->GetID());
	}
	
	return 0; 
}

int TreeView::OnLButtonDown(int x, int y)
{
	if (!GetEnabled())
	{
		return 0;
	}
	
	int x1 = GetContentLeft();
	
	TreeNode* node = GetInNode(x, y);
	
	if (node != NULL)
	{
		//int expand_x = InnerGetAbsLeft() 
		//	+ (node->GetLevel() - 1) * m_nLevelWidth;
		int expand_x = x1 + (node->GetLevel() - 1) * m_nLevelWidth;
		
		if ((node->GetParent() != NULL)
			&& (node != m_pRootNode) && (node->GetNodeCount() > 0)
			&& (x >= expand_x) && (x < (expand_x + m_nLevelWidth)))
		{
			node->SetExpand(!node->GetExpand());

            GuiUtil_RunCallback(this, "on_expand_changed", 
				CVarList() << node->GetID());

			return 1;
		}
		else
		{
			// 如果设置单选展开关闭节点，则展开关闭节点
			if (m_IsLeftClickExpandClose &&
				node->GetNodeCount() > 0)
			{
				node->SetExpand(!node->GetExpand());
			}

			if (m_SelectNode != node->GetID())
			{
				SetSelectNode(node->GetID());
			}
			
			return 1;
		}
	}
	
	return 1;
}

int TreeView::OnLButtonUp(int x, int y)
{
	TreeNode* node = GetInNode(x, y);
	
	if (node != NULL)
	{
		return GuiUtil_RunCallback(this, "on_left_click", 
			CVarList() << node->GetID());
	}

	return 0;
}

int TreeView::OnRButtonUp(int x, int y)
{
	TreeNode* node = GetInNode(x, y);
	
	if (node != NULL)
	{
		return GuiUtil_RunCallback(this, "on_right_click", 
			CVarList() << node->GetID() << x << y);
	}

	return 0;
}

int TreeView::OnLButtonDoubleClick(int x, int y) 
{ 
	TreeNode* node = GetInNode(x, y);
	
	DoubleClickNode(node);

	return 1;
}

int TreeView::OnKeyDown(unsigned int vk)
{
	TreeNode* select_node = TreeNode::GetTreeNode(this, m_SelectNode);
	
	switch (vk)
	{
	case CGuiInput::KEY_UP:
		{
			int old_index = GetShowNodeIndex(select_node);
			int new_index = old_index - 1;

			if (new_index < 0)
			{
				new_index = 0;
			}

			TreeNode* new_node = GetShowIndexNode(new_index);
				
			if (new_node)
			{
				SetSelectNode(new_node->GetID());
			}
			else
			{
				SetSelectNode(PERSISTID());
			}

			return 1;
		}
		break;
	case CGuiInput::KEY_DOWN:
		{
			int old_index = GetShowNodeIndex(select_node);
			int new_index = old_index + 1;
			
			if (size_t(new_index) >= m_ShowNodes.size())
			{
				new_index = (int)m_ShowNodes.size() - 1;
			}
			
			TreeNode* new_node = GetShowIndexNode(new_index);
			
			if (new_node)
			{
				SetSelectNode(new_node->GetID());
			}
			else
			{
				SetSelectNode(PERSISTID());
			}
			
			return 1;
		}
		break;
	case CGuiInput::KEY_LEFT:
		{
			if (select_node->GetExpand() 
				&& (select_node->GetNodeCount() > 0))
			{
				select_node->SetExpand(false);
			}
			else if (select_node->GetParent() != NULL)
			{
				SetSelectNode(select_node->GetParentNode());
			}

			return 1;
		}
		break;
	case CGuiInput::KEY_RIGHT:
		{
			if ((!select_node->GetExpand())
				&& (select_node->GetNodeCount() > 0))
			{
				select_node->SetExpand(true);
			}
			else if (select_node->GetNodeCount() > 0)
			{
				SetSelectNode(select_node->GetNode(0));
			}
			
			return 1;
		}
		break;
	case CGuiInput::KEY_PRIOR:
		{
			int page_disp = GetCanDispCount() - 1;
			
			if (page_disp < 1)
			{
				page_disp = 1;
			}
			
			int old_index = GetShowNodeIndex(select_node);
			int new_index = old_index - page_disp;
			
			if (new_index < 0)
			{
				new_index = 0;
			}
			
			TreeNode* new_node = GetShowIndexNode(new_index);
			
			if (new_node)
			{
				SetSelectNode(new_node->GetID());
			}
			else
			{
				SetSelectNode(PERSISTID());
			}
			
			return 1;
		}
		break;
	case CGuiInput::KEY_NEXT:
		{
			int page_disp = GetCanDispCount() - 1;
			
			if (page_disp < 1)
			{
				page_disp = 1;
			}
			
			int old_index = GetShowNodeIndex(select_node);
			int new_index = old_index + page_disp;
			
			if (size_t(new_index) >= m_ShowNodes.size())
			{
				new_index = (int)m_ShowNodes.size() - 1;
			}
			
			TreeNode* new_node = GetShowIndexNode(new_index);
			
			if (new_node)
			{
				SetSelectNode(new_node->GetID());
			}
			else
			{
				SetSelectNode(PERSISTID());
			}
			
			return 1;
		}
		break;
	case CGuiInput::KEY_HOME:
		{
			int new_index = 0;
			TreeNode* new_node = GetShowIndexNode(new_index);
			
			if (new_node)
			{
				SetSelectNode(new_node->GetID());
			}
			else
			{
				SetSelectNode(PERSISTID());
			}
			
			return 1;
		}
		break;
	case CGuiInput::KEY_END:
		{
			int new_index = (int)m_ShowNodes.size() - 1;
			TreeNode* new_node = GetShowIndexNode(new_index);
			
			if (new_node)
			{
				SetSelectNode(new_node->GetID());
			}
			else
			{
				SetSelectNode(PERSISTID());
			}
			
			return 1;
		}
		break;
	case CGuiInput::KEY_RETURN:
		{
			// 等效双击
			DoubleClickNode(select_node);

			return 1;
		}
		break;
	default:
		break;
	}
	
	return 0;
}

int TreeView::OnMouseMove(int x, int y)
{
	// 获得鼠标当前所在的节点
	TreeNode * node = GetInNode(x, y);

	if (node == NULL)
	{
		m_FocusNode = PERSISTID();
	}
	else
	{
		PERSISTID new_node = node->GetID();

		if (new_node != m_FocusNode)
		{
			if (m_FocusNode != PERSISTID())
			{
				GuiUtil_RunCallback(this, "on_mouse_out_node",
					CVarList() << m_FocusNode << x << y);
			}

			if (new_node != PERSISTID())
			{
				GuiUtil_RunCallback(this, "on_mouse_in_node", 
					CVarList() << new_node << x << y);
			}
			m_FocusNode = new_node;
		}
	}
	return 0;
}

int TreeView::OnLostCapture(const PERSISTID& get_id)
{
	IScrollable::OnLostCapture(get_id);

	if (m_FocusNode != PERSISTID())
	{
		GuiUtil_RunCallback(this, "on_mouse_out_node",
			CVarList() << m_FocusNode);
	}

	m_FocusNode = PERSISTID();

	return 0;
}