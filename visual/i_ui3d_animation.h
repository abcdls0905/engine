#pragma once

#include "i_ui3d_mesh.h"

class IUI3DActionBase
{
public:
	virtual void OnFrameStart() = 0;
	virtual void OnFrame(float per) = 0;
	virtual void OnFrameEnd() = 0;
};

class RotateAction : public IUI3DActionBase
{
	FmVec3 m_target;
	FmVec3 m_value;
	FmVec3 m_start;
	IUIVisNode* m_visobj;
	bool m_relative;
public:
	RotateAction(IUIVisNode* pVisNode, const FmVec3& target, bool relative = true )
	{
		m_relative = relative;
		m_visobj = pVisNode;
		m_value = target;
	}
private:
	void OnFrameStart()
	{
		m_start = m_visobj->GetAngle();
		if(m_relative)
			m_target = m_value + m_start;
		else 
			m_target = m_value;
	}

	void OnFrame(float per)
	{
		FmVec3 target = m_start + (m_target - m_start) * per;
		m_visobj->SetAngle(target.x, target.y, target.z);
	}

	void OnFrameEnd()
	{
		m_visobj->SetAngle(m_target.x, m_target.y, m_target.z);
	}
};

class ScaleAction : public IUI3DActionBase
{
	FmVec3 m_target;
	FmVec3 m_value;
	FmVec3 m_start;
	bool m_relative;
	IUIVisNode* m_pVisNode;
public:
	ScaleAction(IUIVisNode* pVisNode, const FmVec3& target, bool relative = true )
	{
		m_pVisNode = pVisNode;
		m_relative = relative;
		m_value = target;
	}
private:
	void OnFrameStart()
	{
		m_start = m_pVisNode->GetScale();
		if(m_relative)
			m_target = m_value + m_start;
		else
			m_target = m_value;
	}

	void OnFrame(float per)
	{
		FmVec3 target = m_start + (m_target - m_start) * per;
		m_pVisNode->SetScale(target.x, target.y, target.z);
	}

	void OnFrameEnd()
	{
		m_pVisNode->SetScale(m_target.x, m_target.y, m_target.z);
	}
};

class PositionAction : public IUI3DActionBase
{
	FmVec3 m_target;
	FmVec3 m_value;
	FmVec3 m_start;
	PERSISTID m_pid;
	bool m_relative;
	IUIVisNode* m_pVisNode;
public:
	PositionAction(IUIVisNode* pVisNode, const FmVec3& target, bool relative = true)
	{
		m_pVisNode = pVisNode;
		m_relative = relative;
		m_pVisNode = pVisNode;
		m_value = target;
	}
private:
	void OnFrameStart()
	{
		m_start = this->m_pVisNode->GetPosition();
		if(m_relative)
			m_target = m_value + m_start;
		else 
			m_target = m_value;
	}

	void OnFrame(float per)
	{
		FmVec3 target = m_start + (m_target - m_start) * per;
		m_pVisNode->SetPosition(target.x, target.y, target.z);
	}

	void OnFrameEnd()
	{
		m_pVisNode->SetPosition(m_target.x, m_target.y, m_target.z);
	}
};

class IUI3DAnimation
{
public:
	virtual ~IUI3DAnimation(){}

	virtual void Create(int maxframe = 30, float speed = 1) = 0;

	virtual void CreateFrameGroup(const char* name)  = 0;

	virtual void AddAction(IUI3DActionBase* pAction, int frame_num, int frame_skip = 0, const char* framegroup = 0) = 0;

	virtual void Play() = 0;

	virtual void Update(float time) = 0;
};