#pragma once

#include "../public/i_entity.h"
#include "../math/fm_math.h"
#include "camera_base.h"

class Camera3rd : public CameraBase
{
	FmVec2 m_mouse;
	FmVec3 m_angle;
	FmVec3 m_angle_start;
	FmVec2 m_mouse_start;
	FmVec3 m_bindPos;
	bool m_isdrag;
	float m_forwardBack;
	float m_height;
	void updateCamera();

	void OnEnable();
	void OnDisable();
public:
	Camera3rd()
		:m_forwardBack(10.0)
		,m_height(1.0f)
	{}

	bool Init(const IVarList& args);
	bool Shut(){return true;}
	bool MsgProc(unsigned int msg, size_t param1, size_t param2, 
		int& result);
	void Execute(float seconds);
};