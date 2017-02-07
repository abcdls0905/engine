#include "../fm_gui/gui_input.h"

#include "../public/module.h"
#include "camera_control.h"
#include "global.h"
#include "utility.h"
#include "../visual/i_camera.h"
#include "../visual/i_actor.h"



DECLARE_ENTITY(CameraControl, 0, IEntity);

bool CameraControl::Init(const IVarList& args)
{
	m_camera[CAMERA_3RD] = GetCore()->CreateEntity("Camera3rd")->GetID();
#ifdef _WIN32
	m_camera[CAMERA_FREE] = GetCore()->CreateEntity("WinFreeCamera")->GetID();
#endif
	//SetActive(m_camera[CAMERA_3RD]);
	GetCore()->AddMsgProc(this, WM_KEYDOWN);
	m_iCameraType = CAMERA_NONE;
	return true;
}

void CameraControl::SetActive(const PERSISTID& camera)
{
	if(m_active_camera == camera)
		return;
	
	CameraBase* pCamera = (CameraBase*)GetCore()->GetEntity(m_active_camera);
	if(pCamera)
		pCamera->SetEnable(false);

	pCamera = (CameraBase*)GetCore()->GetEntity(camera);
	if(pCamera)
		pCamera->SetEnable(true);
	m_active_camera = camera;
}

bool CameraControl::MsgProc(unsigned int msg, size_t param1, size_t param2, 
		int& result)
{
	switch(msg)
	{
	case WM_KEYDOWN:
		{
#ifdef _WIN32
			switch(param1)
			{
			case VK_F2:
				SetActive(m_camera[CAMERA_3RD]);
				break;
			case VK_F3:
				SetActive(m_camera[CAMERA_FREE]);
				break;
			}
#endif
		}
		break;
	}
	return false;
}


void CameraControl::SetActive(int active)
{
	m_iCameraType = active;
	if(active == CAMERA_3RD)
	{
		SetActive(m_camera[CAMERA_3RD]);
	}
	else if(active == CAMERA_FREE)
	{
		SetActive(m_camera[CAMERA_FREE]);
	}
}

int CameraControl::GetActiveType()
{
	return m_iCameraType;
}

CameraBase* CameraControl::GetActiveCamera()
{
  if (m_active_camera.IsNull())
    return NULL;
  return (CameraBase*)GetCore()->GetEntity(m_active_camera);
}
