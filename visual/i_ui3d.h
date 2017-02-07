#pragma once

#include "i_ui3d_animation.h"
#include "i_ui3d_mesh.h"

class IUI3DCore : public IInterface
{
public:
	~IUI3DCore(){};

	virtual void AddToScene(IScene* pScene) = 0;

	virtual IUI3DAnimation* CreateAnimation() = 0;

	virtual IUI3DMesh* CreateMesh() = 0;

	virtual int AddForm(const char* file, const PERSISTID & pid) = 0;

	virtual int AddForm(const char* file) = 0;

	virtual void RemoveForm(const char* file) = 0;

	virtual ITexture* GetFormTexture(int i) = 0;

	virtual const PERSISTID& GetForm(int i)const = 0;

	virtual size_t GetFormCount()const = 0;
};