//--------------------------------------------------------------------
// 文件名:		TreeViewEx.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2011年3月7日
// 创建人:		马志攀
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "tree_view_ex.h"
#include "tree_node_ex.h"
#include "gui_input.h"
#include "image_list.h"
#include "scroll_bar.h"

/// \file TreeViewEx.cpp
/// \brief 树形控件

/// entity: TreeViewEx
/// \brief 树形控件实体
/// \see 继承自TreeView
DECLARE_ENTITY(TreeViewEx, 0, TreeView);

#if 0
/// method: object GetNodeTop(PERSISTID id)
/// \brief 获取展开节点的高度
/// \param PERSISTID 节点ID
DECLARE_METHOD_1(int, TreeViewEx, GetNodeTop, PERSISTID);
#endif

// TreeViewEx

TreeViewEx::TreeViewEx()
{

}

TreeViewEx::~TreeViewEx()
{
}

bool TreeViewEx::Init(const IVarList& args)
{
	return TreeView::Init(args);
}

bool TreeViewEx::Shut()
{
	return TreeView::Shut();
}

void TreeViewEx::PrepareDesignInfo(IDesignInfo* di)
{
	IScrollable::PrepareDesignInfo(di);

	CVar varDefault;
	di->AddPropInfo("TabIndex", IDesignInfo::TYPE_INT, true, false);
	di->AddPropInfo("TabStop", IDesignInfo::TYPE_BOOL, true, false);
	di->AddPropInfo("IsNoDrawRoot", IDesignInfo::TYPE_BOOL, true, false);
	di->AddPropInfo("IsLeftClickExpandClose", IDesignInfo::TYPE_BOOL, true, false);

	di->AddPropInfo("Solid", IDesignInfo::TYPE_BOOL, true, true,
		&(varDefault.SetBool(true),varDefault));
	di->AddPropInfo("ScrollSize", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("AlwaysVScroll", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("AlwaysHScroll", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("HasHScroll", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("VScrollLeft", IDesignInfo::TYPE_BOOL, true, true);

	varDefault.SetString(GuiUtil_IntToColor(CGuiConst::SELECT_FORE_COLOR).c_str());
	di->AddPropInfo("SelectForeColor", IDesignInfo::TYPE_COLOR, true, true,&varDefault );

	varDefault.SetString(GuiUtil_IntToColor(CGuiConst::SELECT_BACK_COLOR).c_str());
	di->AddPropInfo("SelectBackColor", IDesignInfo::TYPE_COLOR, true, true,&varDefault);
	
    di->AddPropInfo("NodeExpandDraw", IDesignInfo::TYPE_IMAGE, true, true);
    di->AddPropInfo("NodeCloseDraw", IDesignInfo::TYPE_IMAGE, true, true);

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

PERSISTID TreeViewEx::CreateRootNode(const wchar_t* text)
{
	TreeNodeEx* root = (TreeNodeEx*)GetCore()->CreateEntity("TreeNodeEx");

	if (NULL == root)
	{
		return PERSISTID();
	}

    //root->SetCanDelByScript(false);
	root->SetText(text);
	root->SetTreeView(this);
	root->SetLevel(0);

	ReleaseRootNode();

	m_pRootNode = root;

	return m_pRootNode->GetID();
}

int TreeViewEx::CalcShowNodes(TreeNode* node, node_container_t& shows, bool bShow)
{
	if (NULL == node)
	{
		return 0;
	}
	
	int count = 0;
	if (bShow)
	{
		shows.push_back(node);
		++count;
	}

	if (node->GetExpand())
	{
		int num = node->GetNodeCount();
		
		for (int i = 0; i < num; i++)
		{
			count += CalcShowNodes(node->Get(i), shows, true);
		}
	}
	
	return count;
}

int TreeViewEx::CalcRootShowNodes()
{
	m_ShowNodes.clear();

	return CalcShowNodes(m_pRootNode, m_ShowNodes, !GetIsNoDrawRoot());
}

int TreeViewEx::GetCanDispCount() const
{
	int iContentHeight = GetContentHeight();
	int count = 0;

	for (int i = m_nTopIndex; i < (int)m_ShowNodes.size(); ++i)
	{
		TreeNodeEx* node = (TreeNodeEx*)m_ShowNodes[i];
		if (node != NULL)
		{
			int nItemTop = GetNodeTop(i);
			int nHeight = node->GetItemHeight() + node->GetNodeOffsetY();
			if ((nItemTop + nHeight) > iContentHeight)
			{
				return count;
			}
		}

		++count;
	}

	return count;
}

int TreeViewEx::GetVerticalCanDisp() const
{
	return GetCanDispCount();
}

void TreeViewEx::CalcScrollTotal(ImageList* pImageList, int x1, int y1, 
    TreeNode* node, int& index)
{
    if (NULL == node)
    {
        return;
    }

    TreeNodeEx* pNode = (TreeNodeEx*)node;

    if ((NULL == pNode->GetParent() && !GetIsNoDrawRoot()) || 
        NULL != pNode->GetParent())
    {
        int x;

        x = x1 + pNode->GetLevelWidth();

        int w = GetPainter()->GetTextWidth(pNode->GetText());

        if (m_nScrollWidth < x - x1 + w)
        {
            m_nScrollWidth = x - x1 + w;
        }
    }

    if (pNode->GetExpand())
    {
        int num = pNode->GetNodeCount();

        for (int i = 0; i < num; i++)
        {
            CalcScrollTotal(pImageList, x1, y1, (TreeNode*)pNode->Get(i), index);
        }
    }
}

bool TreeViewEx::DrawNodeText(ImageList* pImageList, int x1, int y1, 
	TreeNodeEx* node, int& index)
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
			int nItemHeight = node->GetItemHeight();
            int x = x1 + node->GetLevelWidth();
            int y = y1 + GetNodeTop(index);

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

            if (node->GetID() == m_SelectNode)
            {
				CImagePlayer& imagePlayer = node->GetNodeSelectImagePlayer();
                if (imagePlayer.IsReady())
                {
                    int left = InnerGetAbsLeft();
					int width = GetWidth();
					ScrollBar* pVScrollBar = (ScrollBar*)GetCore()->GetEntity(GetVScrollBar());
					if (pVScrollBar != NULL && pVScrollBar->GetVisible())
					{
						width = width - GetScrollSize();
					}

					imagePlayer.Draw(CGuiConst::DRAWMODE_FITWINDOW, 
						left, y, left + width, y + nItemHeight);
                    
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

                    GetPainter()->FillRect(x + node->GetTextOffsetX(), y + node->GetTextOffsetY(),
						x + node->GetTextOffsetX() + w, y + node->GetTextOffsetY() + h, m_SelectBackColor.GetValue());

                    fore_color = m_SelectForeColor.GetValue();
                }
            }
			else if (node->GetID() == m_FocusNode)
			{
				CImagePlayer& imagePlayer = node->GetNodeFocusImagePlayer();
				if (imagePlayer.IsReady())
                {
                    int left = InnerGetAbsLeft();
					int width = GetWidth();
					ScrollBar* pVScrollBar = (ScrollBar*)GetCore()->GetEntity(GetVScrollBar());
					if (pVScrollBar != NULL && pVScrollBar->GetVisible())
					{
						width = width - GetScrollSize();
					}

					imagePlayer.Draw(CGuiConst::DRAWMODE_FITWINDOW, 
						left, y, left + width, y + nItemHeight);
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
				CImagePlayer& imagePlayer = node->GetNodeBackImagePlayer();
				if (imagePlayer.IsReady())
                {
                    int left = InnerGetAbsLeft();
					int width = GetWidth();
					ScrollBar* pVScrollBar = (ScrollBar*)GetCore()->GetEntity(GetVScrollBar());
					if (pVScrollBar != NULL && pVScrollBar->GetVisible())
					{
						width = width - GetScrollSize();
					}

					imagePlayer.Draw(CGuiConst::DRAWMODE_FITWINDOW, 
						left, y, left + width, y + nItemHeight);
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

			int nTextX = x + node->GetTextOffsetX();
			int nTextY = y + node->GetTextOffsetY();

			GetPainter()->WriteText(nTextX, nTextY, fore_color, 
					node->GetText());

			if (node->GetShowCoverImage())
			{
				CImagePlayer& imagePlayer = node->GetNodeCoverImagePlayer();

				if (imagePlayer.IsReady())
				{
					int left = InnerGetAbsLeft();
					int width = GetWidth();
					ScrollBar* pVScrollBar = (ScrollBar*)GetCore()->GetEntity(GetVScrollBar());
					if (pVScrollBar != NULL && pVScrollBar->GetVisible())
					{
						width = width - GetScrollSize();
					}

					imagePlayer.Draw(CGuiConst::DRAWMODE_FITWINDOW, 
						left, y, left + width, y + nItemHeight);
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
			DrawNodeText(pImageList, x1, y1, (TreeNodeEx*)node->Get(i), index);
		}
	}

	return true;
}

bool TreeViewEx::DrawExpandAndCloseImage(int x1, int y1, TreeNodeEx* node, 
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
			int nLevelWidth = node->GetLevelWidth();
			int nItemHeight = node->GetItemHeight();
			int x = x1 + nLevelWidth;
            int y = y1 + GetNodeTop(index);

            if (node->GetNodeCount() > 0 && m_NodeExpandDraw.IsReady() 
				&& m_NodeCloseDraw.IsReady())
            {
                if(node->GetExpand())
                {
                    int x2 = x + node->GetExpandCloseOffsetX();
                    int y2 = y + node->GetExpandCloseOffsetY();

                    m_NodeExpandDraw.Draw(CGuiConst::DRAWMODE_TILE, x2, y2, 
                        x2 + m_NodeExpandDraw.GetWidth(), 
						y2 + m_NodeExpandDraw.GetHeight());
                }
                else
                {
                    int x2 = x + node->GetExpandCloseOffsetX();
                    int y2 = y + node->GetExpandCloseOffsetY();

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
            DrawExpandAndCloseImage(x1, y1, (TreeNodeEx*)node->Get(i), index);
        }
    }
    
    return true;
}

bool TreeViewEx::DrawNodeImage(ImageList* pImageList, int x1, int y1, 
							 TreeNodeEx* node, int& index)
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
			int x = x1 + node->GetLevelWidth() + node->GetNodeImageOffsetX();
			int y = y1 + GetNodeTop(index) + node->GetNodeImageOffsetY();

			if (pImageList != NULL)
			{
				int image = node->GetImageIndex();

				CImagePlayer* pImage = pImageList->GetImageByIndex(image);

				if (pImage != NULL)
				{
					int w = pImage->GetWidth();

					pImage->Draw(CGuiConst::DRAWMODE_TILE,
						x, y, x + w, y + m_nItemHeight);
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
			DrawNodeImage(pImageList, x1, y1, (TreeNodeEx*)node->Get(i), index);
		}
	}

	return true;
}

void TreeViewEx::PaintScrollRect(float seconds)
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

	DrawNodeText(pImageList, x1, y1, (TreeNodeEx*)m_pRootNode, index);
	
    index = 0;

    DrawExpandAndCloseImage(x1, y1, (TreeNodeEx*)m_pRootNode, index);

	if (pImageList != NULL)
	{
		index = 0;

		DrawNodeImage(pImageList, x1, y1, (TreeNodeEx*)m_pRootNode, index);
	}
}

int TreeViewEx::GetNodeTop(int index) const
{
	if (index <= m_nTopIndex)
	{
		return 0;
	}

	int iHeight = 0;
	for (int i = m_nTopIndex; i <= index; ++i)
	{
		TreeNodeEx* nodeold = (TreeNodeEx*)GetShowIndexNode(i - 1);
		TreeNodeEx* node = (TreeNodeEx*)GetShowIndexNode(i);
		if (i == index)
		{
			if (nodeold != NULL && node != NULL)
			{
				if (nodeold->GetLevel() == node->GetLevel())
				{
					iHeight += node->GetNodeOffsetY();
				}
			}

			continue;
		}

		if (nodeold != NULL && node != NULL)
		{
			if (nodeold->GetLevel() == node->GetLevel())
			{
				iHeight += (node->GetItemHeight() + node->GetNodeOffsetY());
				continue;
			}
		}

		if (node != NULL)
		{
			iHeight += node->GetItemHeight();
		}
	}

	return iHeight;
}

TreeNode* TreeViewEx::GetInNode(int x, int y) const
{
	int t = GetContentTop();

	int num = GetCanDispCount() + 1;

	for (int i = m_nTopIndex, j = 0; i < m_nTopIndex + num; i++, j++)
	{
		if (i >= (int)m_ShowNodes.size())
		{
			break;
		}

		TreeNodeEx* node = (TreeNodeEx*)m_ShowNodes[i];
		if (node == NULL)
		{
			break;
		}

		int nItemTop = GetNodeTop(i);
		int nItemHeight = node->GetItemHeight();
		if ((y >= t) && (y <= (t + nItemTop + nItemHeight)))
		{
			int index = 0;

			if (!m_IsNoDrawRoot)
			{
				return GetNodeByIndex(m_pRootNode, m_nTopIndex + j, index);
			}
			else
			{
				return GetNodeByIndex(m_pRootNode, m_nTopIndex + j + 1, index);
			}
		}
	}

	return NULL;
}

int TreeViewEx::GetNodeTop(PERSISTID id) const
{
	TreeNodeEx* pSubNode = (TreeNodeEx*)GetCore()->GetEntity(id);

	if (NULL == pSubNode)
	{
		return -1;
	}

	int num = GetCanDispCount() + 1;
	int top_index = m_nTopIndex;

	for (int i = top_index, j = 0; i < top_index + num; i++, j++)
	{
		if (i >= (int)m_ShowNodes.size())
		{
			break;
		}

		TreeNodeEx* node = (TreeNodeEx*)m_ShowNodes[i];

		if (node == NULL)
		{
			break;
		}

		int nItemTop = GetNodeTop(i);
		int nItemHeight = node->GetItemHeight();

		if(id == node->GetID())
		{
			return nItemTop;
		}
	}

	return -1;
}
int TreeViewEx::OnLButtonDown(int x, int y)
{
	if (!GetEnabled())
	{
		return 0;
	}
	
	int x1 = GetContentLeft();
	
	TreeNodeEx* node = (TreeNodeEx*)GetInNode(x, y);
	
	if (node != NULL)
	{
		int nLevelWidth = node->GetLevelWidth();
		int expand_x = x1 + nLevelWidth;
		
		if ((node->GetParent() != NULL)
			&& (node != m_pRootNode) && (node->GetNodeCount() > 0)
			&& (x >= expand_x) && (x < (expand_x + m_NodeExpandDraw.GetWidth())))
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

int TreeViewEx::OnLButtonUp(int x, int y)
{
	TreeNode* node = GetInNode(x, y);

	if (node != NULL)
	{
		return GuiUtil_RunCallback(this, "on_left_click", 
			CVarList() << node->GetID());
	}

	return 0;
}

int TreeViewEx::OnRButtonUp(int x, int y)
{
	TreeNode* node = GetInNode(x, y);

	if (node != NULL)
	{
		return GuiUtil_RunCallback(this, "on_right_click", 
			CVarList() << node->GetID() << x << y);
	}

	return 0;
}

int TreeViewEx::OnLButtonDoubleClick(int x, int y) 
{ 
	TreeNode* node = GetInNode(x, y);

	DoubleClickNode(node);

	return 1;
}

int TreeViewEx::OnMouseMove(int x, int y)
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