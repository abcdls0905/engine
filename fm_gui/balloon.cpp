//--------------------------------------------------------------------
// �ļ���:		balloon.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2008��3��21��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "balloon.h"
#include "i_control.h"

/// \file balloon.cpp
/// \brief ����

/// entity: Balloon
/// \brief ����ʵ��
DECLARE_ENTITY(Balloon, 0, IEntity);

/// property: string Name
/// \brief �ؼ���
DECLARE_PROPERTY(const char*, Balloon, Name, GetName, SetName);

/// property: object Control
/// \brief �ؼ�����
DECLARE_PROPERTY_GET(PERSISTID, Balloon, Control, GetControlID);
/// property: object Model
/// \brief �󶨵�ģ�Ͷ���
DECLARE_PROPERTY_GET(PERSISTID, Balloon, Model, GetModelID);
/// property: float Depth
/// \brief ��Ⱦ���
DECLARE_PROPERTY_GET(float, Balloon, Depth, GetDepth);

/// property: float BindX
/// \brief ��λ��X
DECLARE_PROPERTY(float, Balloon, BindX, GetBindX, SetBindX);
/// property: float BindY
/// \brief ��λ��Y
DECLARE_PROPERTY(float, Balloon, BindY, GetBindY, SetBindY);
/// property: float BindZ
/// \brief ��λ��Z
DECLARE_PROPERTY(float, Balloon, BindZ, GetBindZ, SetBindZ);
/// property: float OffsetLeft
/// \brief �ؼ���ˮƽ���ƫ��
DECLARE_PROPERTY(float, Balloon, OffsetLeft, GetOffsetLeft, SetOffsetLeft);
/// property: float OffsetTop
/// \brief �ؼ��Ĵ�ֱ���ƫ��
DECLARE_PROPERTY(float, Balloon, OffsetTop, GetOffsetTop, SetOffsetTop);
/// property: float NearDistance
/// \brief �������
DECLARE_PROPERTY(float, Balloon, NearDistance, GetNearDistance, 
	SetNearDistance);
/// property: float FarDistance
/// \brief ��Զ����
DECLARE_PROPERTY(float, Balloon, FarDistance, GetFarDistance, SetFarDistance);
/// property: bool MustVisible
/// \brief �Ƿ�ǿ����ʾ
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
		// Զ�����Ȼ�
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

// ������������
bool Balloon::SetType(int type)
{
    m_nType = type;
    return true;
}

int Balloon::GetType()
{
    return m_nType;
}