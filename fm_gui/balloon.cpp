//--------------------------------------------------------------------
// 文件名:		balloon.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年3月21日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "balloon.h"
#include "i_control.h"

/// \file balloon.cpp
/// \brief 气球

/// entity: Balloon
/// \brief 气球实体
DECLARE_ENTITY(Balloon, 0, IEntity);

/// property: string Name
/// \brief 控件名
DECLARE_PROPERTY(const char*, Balloon, Name, GetName, SetName);

/// property: object Control
/// \brief 控件对象
DECLARE_PROPERTY_GET(PERSISTID, Balloon, Control, GetControlID);
/// property: object Model
/// \brief 绑定的模型对象
DECLARE_PROPERTY_GET(PERSISTID, Balloon, Model, GetModelID);
/// property: float Depth
/// \brief 渲染深度
DECLARE_PROPERTY_GET(float, Balloon, Depth, GetDepth);

/// property: float BindX
/// \brief 绑定位置X
DECLARE_PROPERTY(float, Balloon, BindX, GetBindX, SetBindX);
/// property: float BindY
/// \brief 绑定位置Y
DECLARE_PROPERTY(float, Balloon, BindY, GetBindY, SetBindY);
/// property: float BindZ
/// \brief 绑定位置Z
DECLARE_PROPERTY(float, Balloon, BindZ, GetBindZ, SetBindZ);
/// property: float OffsetLeft
/// \brief 控件的水平相对偏移
DECLARE_PROPERTY(float, Balloon, OffsetLeft, GetOffsetLeft, SetOffsetLeft);
/// property: float OffsetTop
/// \brief 控件的垂直相对偏移
DECLARE_PROPERTY(float, Balloon, OffsetTop, GetOffsetTop, SetOffsetTop);
/// property: float NearDistance
/// \brief 最近距离
DECLARE_PROPERTY(float, Balloon, NearDistance, GetNearDistance, 
	SetNearDistance);
/// property: float FarDistance
/// \brief 最远距离
DECLARE_PROPERTY(float, Balloon, FarDistance, GetFarDistance, SetFarDistance);
/// property: bool MustVisible
/// \brief 是否强制显示
DECLARE_PROPERTY(bool, Balloon, MustVisible, GetMustVisible, SetMustVisible);

// Balloon

Balloon::Balloon()
{
	m_pPrev = NULL;
	m_pNext = NULL;
	m_fDepth = 0.0F;
	m_fBindX = 0.0F;
	m_fBindY = 0.0F;
	m_fBindZ = 0.0F;
	m_fOffsetLeft = 0.0F;
	m_fOffsetTop = 0.0F;
	m_fNearDistance = 1.0F;
	m_fFarDistance = 50.0F;
	m_bMustVisible = false;
    m_nType = -1;
	m_bTemped = false;
}

Balloon::~Balloon()
{
}

bool Balloon::Init(const IVarList& args)
{
	return true;
}

bool Balloon::Shut()
{
    GetCore()->DeleteEntity(m_ControlID);

	return true;
}

void Balloon::SetName(const char* val)
{
	Assert(val != NULL);

	m_strName = val;
}

const char* Balloon::GetName() const
{
	 return m_strName.c_str();
}


bool Balloon::GetLoadFinish()
{
    IControl* pControl = GetControl();

	if (pControl && !pControl->GetLoadFinish())
	{
		return false;
	}

	return true;
}

void Balloon::Attach(Balloon** first, Balloon** last)
{
	if (*first == 0) 
	{
		*first = this;
		*last = this;
		return;
	}
	
	(*last)->SetNextNode(this);
	
	SetPrevNode(*last);
	
	*last = this;
}

void Balloon::AttachTo(Balloon* link_after, Balloon** first, 
	Balloon** last)
{
	Balloon* t = link_after->GetNextNode();
	
	link_after->SetNextNode(this);
	
	SetNextNode(t);
	SetPrevNode(link_after);
	
	if (t)
	{
		t->SetPrevNode(this);
	} 
	else
	{
		*last = this;
	}
}

void Balloon::Detach(Balloon** first, Balloon** last)
{
	if (m_pPrev)
	{
		m_pPrev->SetNextNode(m_pNext);
	}
	
	if (m_pNext)
	{
		m_pNext->SetPrevNode(m_pPrev);
	}
	
	if (*first == this)
	{
		*first = m_pNext;
	}
	
	if (*last == this)
	{
		*last = m_pPrev;
	}
}

/*
bool Balloon::ProcessOrder(Balloon** first, Balloon** last)
{
	if (m_pNext) 
	{
		// 远处的先画
		if (m_fDepth < m_pNext->m_fDepth)
		{
			Detach(first, last);
			AttachTo(m_pNext, first, last);
			return true;
		}
	}

	return false;
}*/

void Balloon::SetControl(IControl* pControl)
{
	Assert(pControl != NULL);

	m_ControlID = pControl->GetID();
}

IControl* Balloon::GetControl() const
{
    return (IControl*)GetCore()->GetEntity(m_ControlID);
}

PERSISTID Balloon::GetControlID() const
{
    return m_ControlID;
}

void Balloon::SetModelID(const PERSISTID& value, const char*szNodeName/*=0*/)
{
	m_ModelID = value;
	m_strNodeName = szNodeName?szNodeName:"";
}

PERSISTID Balloon::GetModelID() const
{
	return m_ModelID;
}

const char* Balloon::GetNodeName()const
{
	return m_strNodeName.c_str();
}


void Balloon::SetDepth(float value)
{
	m_fDepth = value;
}

float Balloon::GetDepth() const
{
	return m_fDepth;
}

void Balloon::SetBindX(float value)
{
	m_fBindX = value;
}

float Balloon::GetBindX() const
{
	return m_fBindX;
}

void Balloon::SetBindY(float value)
{
	m_fBindY = value;
}

float Balloon::GetBindY() const
{
	return m_fBindY;
}

void Balloon::SetBindZ(float value)
{
	m_fBindZ = value;
}

float Balloon::GetBindZ() const
{
	return m_fBindZ;
}

void Balloon::SetOffsetLeft(float value)
{
	m_fOffsetLeft = value;
}

float Balloon::GetOffsetLeft() const
{
	return m_fOffsetLeft;
}

void Balloon::SetOffsetTop(float value)
{
	m_fOffsetTop = value;
}

float Balloon::GetOffsetTop() const
{
	return m_fOffsetTop;
}

void Balloon::SetNearDistance(float value)
{
	m_fNearDistance = value;
}

float Balloon::GetNearDistance() const
{
	return m_fNearDistance;
}

void Balloon::SetFarDistance(float value)
{
	m_fFarDistance = value;
}

float Balloon::GetFarDistance() const
{
	return m_fFarDistance;
}

void Balloon::SetMustVisible(bool value)
{
	m_bMustVisible = value;
}

bool Balloon::GetMustVisible() const
{
	return m_bMustVisible;
}

// 设置汽球类型
bool Balloon::SetType(int type)
{
    m_nType = type;
    return true;
}

int Balloon::GetType()
{
    return m_nType;
}