#pragma once

#include "../public/i_entity.h"
#include "../math/fm_math.h"
#include "camera_base.h"

enum {CAMERA_NONE, CAMERA_3RD, CAMERA_FREE, MAX_CAMERA};

class CameraControl : public IEntity
{
	PERSISTID m_camera[MAX_CAMERA];
	PERSISTID m_active_camera;
	int m_iCameraType;
	void SetActive(const PERSISTID& camera);
public:
	bool Init(const IVarList& args);
	
	bool Shut(){return true;}
	
	bool MsgProc(unsigned int msg, size_t param1, size_t param2, 
		int& result);

	void SetActive(int active);
	int GetActiveType();
  CameraBase* GetActiveCamera();
};

