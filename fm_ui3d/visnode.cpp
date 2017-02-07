#include "visnode.h"

_UI3D_BEGIN

VisNode::VisNode()
:m_mtxGlobal(0)
{
	m_pos = FmVec3(0,0,0);
	m_angle = FmVec3(0,0,0);
	m_scale = FmVec3(1, 1, 1);
	BuildWorldMatrix();
}

bool VisNode::SetPosition(const FmVec3& value)
{
	m_pos = value; 
	BuildWorldMatrix();
	return true;
}

bool VisNode::SetScale(const FmVec3& value)
{
	m_scale = value;
	BuildWorldMatrix();
	return true;
}

bool VisNode::SetAngle(const FmVec3& value)
{
	m_angle = value;
	BuildWorldMatrix();
	return true;
}

void VisNode::BuildWorldMatrix()
{
	FmQuat rotate;
	FmQuatRotationYawPitchRoll(&rotate, m_angle.y, m_angle.x, m_angle.z);
	VisUtil_MatrixTransform(&m_mtxWorld, &m_scale, &rotate, &m_pos);
	if(m_mtxGlobal)
	{
		::FmMat4Multiply(&m_mtxWorld, &m_mtxWorld, m_mtxGlobal);
	}
	OnChangedWorldMatrix();
}

_UI3D_END