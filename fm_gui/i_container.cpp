//--------------------------------------------------------------------
// 文件名:		i_container.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "i_container.h"
#include "../public/inlines.h"

/// \file i_container.cpp
/// \brief 控件容器

/// entity: IContainer
/// \brief 控件容器基类
/// \see 继承自IControl
DECLARE_ABSTRACT_ENTITY(IContainer, 0, IControl);

/// readonly: bool IsContainer
/// \brief 是否是容器
DECLARE_PROPERTY_GET(bool, IContainer, IsContainer, IsContainer);

/// property: int BlendAlpha
/// \brief 半透明混合
DECLARE_PROPERTY(int, IContainer, BlendAlpha, GetAlpha, SetAlpha);

/// event: int on_add(object self, object child)
/// \brief 添加子控件时的回调
/// \param self 本控件
/// \param child 子控件
DECLARE_EVENT(on_add);
/// event: int on_remove(object self, object child)
/// \brief 移除子控件时的回调
/// \param self 本控件
/// \param child 子控件
DECLARE_EVENT(on_remove);

// IContainer

IContainer::IContainer()
{
	m_bAddRemoveEvent = false;
}

IContainer::~IContainer()
{
}

bool IContainer::Init(const IVarList& args)
{
	if (!IControl::Init(args))
	{
		return false;
	}

	return true;
}

bool IContainer::Shut()
{
	DeleteAll();

	return IControl::Shut();
}

void IContainer::PaintTopLayer(float seconds)
{
	if (m_TopLayer.empty())
	{
		return;
	}
	
	size_t k = 0;
	
	while (k < m_TopLayer.size())
	{
		IControl* pControl = GetGui()->GetControl(m_TopLayer[k]);
		
		if (NULL == pControl)
		{
			m_TopLayer.remove(k);
		}
		else
		{
			pControl->Paint(seconds);
			++k;
		}
	}
}

void IContainer::Paint(float seconds)
{
	if (!GetVisible())
	{
		return;
	}

	if (!m_Children.empty())
	{
		size_t child_num = m_Children.size();
		IControl** children = &m_Children[0];

		for (size_t i = 0; i < child_num; ++i)
		{
			children[i]->Paint(seconds);
		}
	}

	// 画顶层控件
	PaintTopLayer(seconds);
}

void IContainer::PrepareDesignInfo(IDesignInfo* di)
{
    CVar v(VTYPE_INT, 255);
	IControl::PrepareDesignInfo(di);
    di->AddPropInfo("BlendAlpha", IDesignInfo::TYPE_INT, true, true, 
        &v);
	di->AddEventInfo("on_add");
	di->AddEventInfo("on_remove");
}

bool IContainer::GetLoadFinish()
{
    if (GetLoadFinishValue())
    {
        return true;
    }

	if (!m_Children.empty())
	{
		size_t child_num = m_Children.size();

		for (size_t i = 0; i < child_num; ++i)
		{
			if (!m_Children[i]->GetLoadFinish())
			{
				return false;
			}
		}
	}

	return IControl::GetLoadFinish();
}

bool IContainer::IsContainer() const
{
	return true;
}

IControl* IContainer::FindChild(const char* name) const
{
	Assert(name != NULL);

	unsigned int hash = GetHashValueCase(name);
	size_t child_num = m_Children.size();

	for (size_t i = 0; i < child_num; ++i)
	{
		IControl* p = m_Children[i];
		
		if ((p->GetHash() == hash) && (strcmp(p->GetName(), name) == 0))
		{
			return p;
		}
	}

	return NULL;
}

// 用控件名从本节点开始进行递归查找，包扩本节点
IControl* IContainer::QueryControl(const char* name)
{

	Assert(name != NULL);

	IControl* pControl = IControl::QueryControl(name);
	if(pControl)
		return pControl;

	size_t child_num = m_Children.size();

	for (size_t i = 0; i < child_num; ++i)
	{
		IControl* p = m_Children[i]->QueryControl(name);
		if(p)
			return p;
	}
	return NULL;
}

void IContainer::InitChild(IControl* child)
{
	Assert(child != NULL);
	
	child->SetParentControl(this);
}

void IContainer::PutChild(IControl* child)
{
	Assert(child != NULL);
	
	OnAdd(child->GetID());
	
	child->OnEnter(GetID());
}

bool IContainer::AddChild(IControl* child)
{
	Assert(child != NULL);

	if (child->GetParentControl() != NULL)
	{
		return false;
	}
	
	InitChild(child);
	m_Children.push_back(child);
	PutChild(child);
	
	return true;
}

bool IContainer::InsertAfterChild(IControl* child, IControl* after)
{
	Assert(child != NULL);
	Assert(after != NULL);
	
	if (child->GetParentControl() != NULL)
	{
		return false;
	}
	
	size_t k = 0;
	
	while (k < m_Children.size())
	{
		if (m_Children[k] == after)
		{
			InitChild(child);
			m_Children.insert(k + 1, child);
			PutChild(child);
			return true;
		}
		else
		{
			++k;
		}
	}
	
	return false;
}

bool IContainer::InsertBeforeChild(IControl* child, IControl* before)
{
	Assert(child != NULL);
	Assert(before != NULL);
	
	if (child->GetParentControl() != NULL)
	{
		return false;
	}
	
	size_t k = 0; 
	
	while (k < m_Children.size())
	{
		if (m_Children[k] == before)
		{
			InitChild(child);
			m_Children.insert(k, child);
			PutChild(child);
			return true;
		}
		else
		{
			++k;
		}
	}
	
	return false;
}

bool IContainer::RemoveChild(IControl* child)
{
	Assert(child != NULL);
	
	if (child->GetParentControl() == NULL)
	{
		return false;
	}
	
	size_t k = 0; 

	while (k < m_Children.size())
	{
		if (m_Children[k] == child)
		{	
			OnRemove(child->GetID());
			m_Children.remove(k);
			child->SetParentControl(NULL);
			child->OnLeave(GetID());
			return true;
		}
		else
		{
			++k;
		}
	}
	
	return false;
}

bool IContainer::RemoveChildNoEvent(IControl* child)
{
	Assert(child != NULL);
	
	if (child->GetParentControl() == NULL)
	{
		return false;
	}
	
	size_t k = 0; 

	while (k < m_Children.size())
	{
		if (m_Children[k] == child)
		{	
			//OnRemove(child->GetID());
			m_Children.remove(k);
			child->SetParentControl(NULL);
			//child->OnLeave(GetID());
			return true;
		}
		else
		{
			++k;
		}
	}
	
	return false;
}

void IContainer::ClearChild()
{
	size_t child_num = m_Children.size();
	
	for (size_t i = 0; i < child_num; ++i)
	{
		m_Children[i]->SetParentControl(NULL);
	}
	
	m_Children.clear();
}

void IContainer::DeleteAllChild()
{
	size_t child_num = m_Children.size();
	
	for (size_t i = 0; i < child_num; ++i)
	{
		IControl* pChild = m_Children[i];
		
		pChild->SetParentControl(NULL);
		pChild->Release();
	}
	
	m_Children.clear();
}

bool IContainer::SendToBack(IControl* child)
{
	Assert(child != NULL);
	
	size_t child_num = m_Children.size();
	
	for (size_t i = 0; i < child_num; ++i)
	{
		if (m_Children[i] == child)
		{
			m_Children.remove(i);
			m_Children.insert(0, child);
			return true;
		}
	}
	
	return true;
}

bool IContainer::BringToFront(IControl* child)
{
	Assert(child != NULL);
	
	size_t child_num = m_Children.size();
	
	for (size_t i = 0; i < child_num; ++i)
	{
		if (m_Children[i] == child)
		{
			m_Children.remove(i);
			m_Children.push_back(child);
			return true;
		}
	}
	
	return true;
}

bool IContainer::IsChildControl(IControl* child) const
{
	size_t child_num = m_Children.size();
	
	for (size_t i = 0; i < child_num; ++i)
	{
		if (m_Children[i]->GetID() == child->GetID())
		{
			return true;
		}
	}
	
	return false;
}

size_t IContainer::GetChildCount() const
{
	return m_Children.size();
}

IControl* IContainer::GetChildByIndex(size_t index) const
{
	if (index >= m_Children.size())
	{
		return NULL;
	}

	return m_Children[index];
}

size_t IContainer::GetChildList(IVarList& result) const
{
	result.Clear();
	
	size_t child_num = m_Children.size();
	
	for (size_t i = 0; i < child_num; ++i)
	{
		result.AddObject(m_Children[i]->GetID());
	}
	
	return result.GetCount();
}

int IContainer::GetTopControlList(const IVarList& param, IVarList& result)
{
    result.Clear();

	size_t top_num = m_TopLayer.size();

	for (size_t i = 0; i < top_num; ++i)
    {
        result.AddObject(m_TopLayer[i]);
    }

    return (int)result.GetCount();
}

int IContainer::GetChildControlList(const IVarList& param, IVarList& result)
{
    result.Clear();

	size_t child_num = m_Children.size();

	for (size_t i = 0; i < child_num; ++i)
    {
        result.AddObject(m_Children[i]->GetID());
    }

    return (int)result.GetCount();
}

void IContainer::SetAddRemoveEvent(bool value)
{
	m_bAddRemoveEvent = value;
}

bool IContainer::GetAddRemoveEvent() const
{
	return m_bAddRemoveEvent;
}

bool IContainer::Add(const PERSISTID& child)
{
	IControl* pControl = GetControl(child);
	
	if (NULL == pControl)
	{
		return false;
	}
	
	if (!AddChild(pControl))
	{
		return false;
	}
	
	return true;
}

bool IContainer::InsertAfter(const PERSISTID& child, 
	const PERSISTID& after)
{
	IControl* pControl = GetControl(child);
	
	if (NULL == pControl)
	{
		return false;
	}

	IControl* pAfter = GetControl(after);

	if (NULL == pAfter)
	{
		return false;
	}
	
	if (!InsertAfterChild(pControl, pAfter))
	{
		return false;
	}
	
	return true;
}

bool IContainer::InsertBefore(const PERSISTID& child, 
	const PERSISTID& before)
{
	IControl* pControl = GetControl(child);
	
	if (NULL == pControl)
	{
		return false;
	}
	
	IControl* pBefore = GetControl(before);
	
	if (NULL == pBefore)
	{
		return false;
	}
	
	if (!InsertBeforeChild(pControl, pBefore))
	{
		return false;
	}
	
	return true;
}

bool IContainer::Remove(const PERSISTID& child)
{
	IControl* pControl = GetControl(child);

	if (NULL == pControl)
	{
		return false;
	}

	if (!RemoveChild(pControl))
	{
		return false;
	}
	
	return true;
}

bool IContainer::DeleteAll()
{
	DeleteAllChild();

    // Top层仅引用控件，作显示用，并不能存放控件，控件应该放到child中,
    DeleteAllTopLayer();
	return true;
}

PERSISTID IContainer::Find(const char* name) const
{
	Assert(name != NULL);

	IControl* pControl = FindChild(name);

	if (NULL == pControl)
	{
		return PERSISTID();
	}
	
	return pControl->GetID();
}

bool IContainer::ToFront(const PERSISTID& child)
{
	IControl* pControl = GetControl(child);

	if (NULL == pControl)
	{
		return false;
	}

	if (!BringToFront(pControl))
	{
		return false;
	}
		
	return true;
}

bool IContainer::ToBack(const PERSISTID& child)
{
	IControl* pControl = GetControl(child);

	if (NULL == pControl)
	{
		return false;
	}

	if (!SendToBack(pControl))
	{
		return false;
	}
		
	return true;
}

bool IContainer::IsChild(const PERSISTID& id) const
{
	IControl* pControl = GetControl(id);
	
	if (NULL == pControl)
	{
		return false;
	}
	
	return IsChildControl(pControl);
}

bool IContainer::FindTopLayer(const PERSISTID& id)
{
	size_t top_num = m_TopLayer.size();
	
	for (size_t i = 0; i < top_num; ++i)
	{
		if (m_TopLayer[i] == id)
		{
			return true;
		}
	}
	
	return false;
}

bool IContainer::AddTopLayer(const PERSISTID& id)
{
	IControl* pControl = GetControl(id);

	if (NULL == pControl)
	{
		return false;
	}

	if (FindTopLayer(id))
	{
		return false;
	}
	
	m_TopLayer.push_back(id);

	return true;
}

bool IContainer::RemoveTopLayer(const PERSISTID& id)
{
	size_t top_num = m_TopLayer.size();

	for (size_t i = 0; i < top_num; ++i)
	{
		if (m_TopLayer[i] == id)
		{
			m_TopLayer.remove(i);
			return true;
		}
	}
	
	return false;
}

void IContainer::DeleteAllTopLayer()
{
	size_t top_num = m_TopLayer.size();

	for (size_t i = 0; i < top_num; ++i)
    {
		IControl* pControl = (IControl*)GetCore()->GetEntity(m_TopLayer[i]);

		// 仅删除和些界面关联的top_layer控件，无关联的，认为是外部的，不处理
		if (pControl && pControl->GetParentControl() == this)
		{
			GetGui()->Delete(m_TopLayer[i]);
		}
    }

    m_TopLayer.clear();
}

int IContainer::OnAdd(const PERSISTID& child_id)
{
	if (!m_bAddRemoveEvent)
	{
		return 0;
	}
	
	return GuiUtil_RunCallback(this, "on_add", CVarList() << child_id);
}

int IContainer::OnRemove(const PERSISTID& child_id)
{
	if (!m_bAddRemoveEvent)
	{
		return 0;
	}
	
	return GuiUtil_RunCallback(this, 
		"on_remove", CVarList() << child_id);
}

void IContainer::RefreshChildren()
{
	size_t k = 0;

	while (k < m_Children.size())
    {
        m_Children[k]->SetRefreshInnerAbsPos(true);
        ++k;
    }

	size_t i = 0;

	while (i < m_TopLayer.size())
    {
        IControl* pControl = (IControl*)GetCore()->GetEntity(m_TopLayer[i]);

        if (pControl != NULL)
        {
            pControl->SetRefreshInnerAbsPos(true);
        }

        ++i;
    }

}

int IContainer::OnWidthChanged(int old)
{
    SetRefreshInnerAbsPos(true);
	return 1;
}

int IContainer::OnHeightChanged(int old)
{
    SetRefreshInnerAbsPos(true);
	return 1;
}

// 左上角坐标改变
int IContainer::OnLeftChanged(int old)
{
    SetRefreshInnerAbsPos(true);
    return 1;
}

int IContainer::OnTopChanged(int old)
{
    SetRefreshInnerAbsPos(true);
    return 1;
}

IControl* IContainer::OnGetInWhat(int x, int y)
{
	// 顶层控件
	if (!m_TopLayer.empty())
	{
		int top_num = (int)m_TopLayer.size();
		PERSISTID* tops = &m_TopLayer[0];

		for (int i = top_num - 1; i >= 0; --i)
		{
			IControl* pChild = (IControl*)GetCore()->GetEntity(tops[i]);

			if (NULL == pChild)
			{
				continue;
			}

			IControl* pControl = pChild->GetInWhat(x, y);

			if (pControl != NULL)
			{
				return pControl;
			}
		}
	}
	
	// 子控件
	if (!m_Children.empty())
	{
		int child_num = (int)m_Children.size();
		IControl** children = &m_Children[0];

		for (int i = child_num - 1; i >= 0; --i)
		{
			IControl* pControl = children[i]->GetInWhat(x, y);

			if (pControl != NULL)
			{
				return pControl;
			}
		}
	}
	
	return IControl::OnGetInWhat(x, y);
}

bool IContainer::RefreshInnerAbsPos()
{
    if (GetRefreshInnerAbsPos())
    {
        m_nInnerAbsLeft = GetAbsLeft();
        m_nInnerAbsTop = GetAbsTop();
        RefreshChildren();
        SetRefreshInnerAbsPos(false);
    }

    return true;
}
