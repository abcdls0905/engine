#include "../fm_gui/gui_input.h"

#include "../public/module.h"
#include "camera_3rd.h"
#include "global.h"
#include "utility.h"
#include "../visual/i_scene.h"
#include "../visual/i_camera.h"
#include "../visual/i_actor.h"

DECLARE_ENTITY(Camera3rd, 0, IEntity);

#define PITCH_MIN 0.0
#define PITCH_MAX (40.0F/180.0F * 3.14F)
#define FORWARD_MAX (30)
#define FORWARD_MIN (1)

bool Camera3rd::Init(const IVarList& args)
{
	/*
	GetCore()->AddMsgProc(this, WM_MOUSEMOVE);
	GetCore()->AddMsgProc(this, WM_RBUTTONDOWN);
	GetCore()->AddMsgProc(this, WM_RBUTTONUP);
	GetCore()->AddMsgProc(this, WM_MOUSEWHEEL);
	
	*/
	m_isdrag = false;
	memset(&m_angle, 0, sizeof(m_angle));
	memset(&m_angle_start, 0, sizeof(m_angle_start));
	memset(&m_mouse_start, 0, sizeof(m_mouse_start));
    m_angle = FmVec3(1.105,-8.910,0.0);
	/*
	GetCore()->AddExecute(this);
	updateCamera();*/
	return true;
}

void Camera3rd::updateCamera()
{
	FmVec3 pos = FmVec3(0.0, m_height, -m_forwardBack);
	
	FmMat4 mRotate;
	FmMat4RotationYawPitchRoll(&mRotate, m_angle.y, m_angle.x, 0.0);

	m_bindPos.x = pos.x * mRotate._11 + pos.y * mRotate._21 + pos.z * mRotate._31;
	m_bindPos.y = pos.x * mRotate._12 + pos.y * mRotate._22 + pos.z * mRotate._32;
	m_bindPos.z = pos.x * mRotate._13 + pos.y * mRotate._23 + pos.z * mRotate._33;
	
	GetCamera()->SetPosition(m_bindPos.x, m_bindPos.y, m_bindPos.z);
	GetCamera()->SetAngle(m_angle.x,  m_angle.y, 0.0);
}

bool Camera3rd::MsgProc(unsigned int msg, size_t param1, size_t param2, 
		int& result)
{
	switch(msg)
	{
	case WM_MOUSEMOVE:
		{
			if(m_isdrag)
			{
				long lMouseX	=	(LOWORD(param2));
				long lMouseY	=	(HIWORD(param2));
				m_mouse = FmVec2((float)lMouseX, (float)lMouseY);
				FmVec2 offset = FmVec2((float)lMouseX, (float)lMouseY) - m_mouse_start;
				const float SPIN_RATE = 3.14f * 2 / 1024.0f;
				m_angle.x = m_angle_start.x + offset.y * SPIN_RATE;
				if(m_angle.x > PITCH_MAX)
					m_angle.x = PITCH_MAX;
				else if(m_angle.x < PITCH_MIN)
					m_angle.x = PITCH_MIN;

				m_angle.y = m_angle_start.y + offset.x * SPIN_RATE;
				updateCamera();
			}
		}
		break;
	case WM_RBUTTONDOWN:
		{
			m_isdrag = true;
			long lMouseX	=	(LOWORD(param2));
			long lMouseY	=	(HIWORD(param2));
			m_mouse_start = FmVec2((float)lMouseX, (float)lMouseY);
			m_angle_start = m_angle;//GetCamera()->GetAngle();
			m_angle = m_angle_start;
		}
		break;

	case WM_RBUTTONUP:
		{
			m_isdrag = false;
			long lMouseX	=	(LOWORD(param2));
			long lMouseY	=	(HIWORD(param2));
			
		}
		break;
	case WM_MOUSEWHEEL:
		{
			#define WHEEL_DELTA      120      // Default value for rolling one notch
			WORD fwKeys = LOWORD(param1); 
			short zDelta = (short) HIWORD(param1); 
			short xPos = (short) LOWORD(param2); 
			short yPos = (short) HIWORD(param2); 
			if(zDelta<0) //向后
			{
				m_forwardBack ++;
				if(m_forwardBack > FORWARD_MAX)
					m_forwardBack = FORWARD_MAX;
			}
			else		//向上
			{
				m_forwardBack --;
				if(m_forwardBack < FORWARD_MIN)
					m_forwardBack = FORWARD_MIN;
			}
			updateCamera();
		}
		break;
	}
	return false;
}

void Camera3rd::Execute(float seconds)
{
	GetCamera()->SetBindPosition(m_bindPos.x, m_bindPos.y, m_bindPos.z);
	updateCamera();
}

void Camera3rd::OnEnable()
{
	GetCore()->AddExecute(this);
	GetCore()->AddMsgProc(this, WM_MOUSEMOVE);
	GetCore()->AddMsgProc(this, WM_RBUTTONDOWN);
	GetCore()->AddMsgProc(this, WM_RBUTTONUP);
	GetCore()->AddMsgProc(this, WM_MOUSEWHEEL);
	if(actor_getrole())
		GetCamera()->SetBindID(actor_getrole()->GetID());
	else
		GetCamera()->SetBindID(m_bindobj);
}

void Camera3rd::OnDisable()
{
	GetCore()->RemoveExecute(this);
	GetCore()->RemoveMsgProc(this, WM_MOUSEMOVE);
	GetCore()->RemoveMsgProc(this, WM_RBUTTONDOWN);
	GetCore()->RemoveMsgProc(this, WM_RBUTTONUP);
	GetCore()->RemoveMsgProc(this, WM_MOUSEWHEEL);
	m_bindobj = GetCamera()->GetBindID();
	GetCamera()->SetBindID(PERSISTID());
}