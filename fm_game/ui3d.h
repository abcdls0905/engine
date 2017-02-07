#pragma once

#include <vector>
#include "../visual/i_vis_base.h"
#include "../visual/i_render.h"
#include "../fm_gui/gui.h"
#include "../fm_gui/form.h"
#include "../fm_gui/designer.h"
//#include "Mesh.h"
//#include "render_ui.h"
//#include "animation.h"
#include "../visual/i_ui3d.h"

class UI3D : public IVisBase
{
	IUI3DCore* m_ui3d_core;
	IUI3DMesh* m_mesh;
	IUI3DAnimation* m_animation;
	IUI3DAnimation* m_animationback;
public:
	~UI3D();

	bool Init(const IVarList& args);

	bool Shut();

	void Realize();

	void Execute(float seconds);

	// ������Ϣ����
	bool MsgProc(unsigned int msg, size_t param1, size_t param2, 
		int& result);	

	// λ��
	virtual bool SetPosition(float x, float y, float z) { return m_mesh->SetPosition(x, y, z);}
	virtual FmVec3 GetPosition() const { return m_mesh->GetPosition(); }
	// ����
	virtual bool SetAngle(float x, float y, float z) {return  m_mesh->SetAngle(x, y, z);}
	virtual FmVec3 GetAngle() const { return m_mesh->GetAngle(); }
	// ����ϵ��
	virtual bool SetScale(float x, float y, float z) { return m_mesh->SetScale(x, y, z);}
	virtual FmVec3 GetScale() const { return m_mesh->GetScale(); }
};
