#pragma once

#include "../visual/i_vis_base.h"
#include "defines.h"
#include "../visual/i_ui3d_animation.h"
#include "../visual/i_ui3d_mesh.h"
_UI3D_BEGIN

class VisNode : public IUIVisNode
{
protected:
	FmMat4* m_mtxGlobal;
	FmMat4 m_mtxWorld;
	FmVec3 m_pos, m_angle, m_scale;
	virtual void OnChangedWorldMatrix(){};
	void BuildWorldMatrix();
	inline void SetGlobal(FmMat4* global) { m_mtxGlobal = global; BuildWorldMatrix(); }
public:
	VisNode();

	// 位置
	virtual bool SetPosition(float x, float y, float z) { return SetPosition(FmVec3(x, y, z)); }
	virtual bool SetPosition(const FmVec3& value);
	virtual FmVec3 GetPosition() const { return m_pos; }
	// 方向
	virtual bool SetAngle(float x, float y, float z) { return SetAngle(FmVec3(x, y, z)); }
	virtual bool SetAngle(const FmVec3& value);
	virtual FmVec3 GetAngle() const { return m_angle; }
	// 缩放系数
	virtual bool SetScale(float x, float y, float z) { return SetScale(FmVec3(x, y, z)); }
	virtual bool SetScale(const FmVec3& value);
	virtual FmVec3 GetScale() const { return m_scale; }
};

_UI3D_END