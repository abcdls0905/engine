#pragma once

#include "../public/module.h"
#include "../visual/i_vis_base.h"

#include "render_ui.h"
#include "../visual/i_ui3d.h"

class ui3d_core : public IUI3DCore
{
	RenderUI* m_renderui;
public:
	virtual bool Init();

	virtual void AddToScene(IScene* pScene);

	virtual bool Shut();

	virtual IUI3DAnimation* CreateAnimation();

	virtual IUI3DMesh* CreateMesh();

	virtual int AddForm(const char* file, const PERSISTID & pid);

	virtual int AddForm(const char* file);

	virtual void RemoveForm(const char* file);

	virtual ITexture* GetFormTexture(int i);

	virtual const PERSISTID& GetForm(int i) const;

	virtual size_t GetFormCount()const;
};

class ui3d_control : public IVisBase
{
};