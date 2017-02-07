#pragma once

#include "../public/module.h"
#include "../visual/i_camera.h"
#include "../public/i_core.h"
#include "global.h"
#include "../visual/i_scene.h"

class CameraBase: public IEntity
{
	bool m_enable;
	virtual void OnEnable(){}
	virtual void OnDisable(){}
protected:
	PERSISTID m_bindobj;
public:
	inline ICamera* GetCamera()
	{
		return (ICamera*) g_core->GetEntity(g_scene->GetCameraID());
	}
public:
	CameraBase()
		:m_enable(false)
	{}

	inline void SetEnable(bool enable)
	{
		if(enable != m_enable)
		{
			m_enable = enable;
			if(enable)
				OnEnable();
			else
				OnDisable();
		}
	}
	
	inline bool IsEnable()const { return m_enable; }
};