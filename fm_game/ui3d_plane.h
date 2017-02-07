#pragma once

#include "global.h"
#include "../visual/i_vis_base.h"
#include "../visual/i_render.h"
#include "../visual/i_terrain.h"
#include "../public/portable.h"
#include "../visual/i_scene.h"
#include "../visual/i_camera.h"

class UIPlane
{
	struct Vertex
	{
		FmVec3 pos;
		FmVec2 uv;
	};
	Vertex m_vb_data[6];
	FmVec3 m_pos, m_angle, m_scale;
	ITexture* m_texture;
	IShaderProgram* m_shader;
	IStaticVB* m_vb;
	FmMat4 m_mtxWorld;
	FmMat4 m_mtxWorldParent;
	float m_alpha;
	inline void BuildWorldMatrix()
	{
		FmQuat rotate;
		FmQuatRotationYawPitchRoll(&rotate, m_angle.y, m_angle.x, m_angle.z);
		VisUtil_MatrixTransform(&m_mtxWorld, &m_scale, &rotate, &m_pos);
	}
	void CreateVB();
	void render();
	static void s_render(void* data);
public:
	UIPlane()
		:m_vb(0)
		,m_texture(0)
	{
		m_pos = FmVec3(0,0,0);
		m_angle = FmVec3(0,0,0);
		m_scale = FmVec3(1,1,1);
		BuildWorldMatrix();

		IVertexShader* pVS = g_render->LoadVertexShader("ui3d.vsh", "main", "");
		IPixelShader* pPS = g_render->LoadPixelShader("ui3d.fsh", "main", "");
		const char* vertex_t_name[] = 
		{
			"iPos",
			"iUV",
		};
		m_shader = g_render->CreateShaderProgram(pVS, pPS, vertex_t_name, 2);
		m_alpha = 0.70f;
	}

	~UIPlane()
	{
		SAFE_RELEASE(m_vb);
		SAFE_RELEASE(m_shader);
		SAFE_RELEASE(m_texture);
	}
	void SetParentWorldMatrix(const FmMat4& mtxWorld) { m_mtxWorldParent = mtxWorld; }
	void SetPosition(float x, float y, float z);
	void SetScale(float x, float y, float z);
	void SetAngle(float x, float y, float z);
	void SetTexture(ITexture* pTexture);
	ITexture* GetTexture() { return m_texture; }
	bool Pick(IRenderContext* pContext, int screen_x, int screen_y, FmVec2& uv, float& t);
	void Realize();
	void Build(float w, float h, float _maxu = 1.0f, float _maxv = 1.0f);
	void SetAlpha(float alpha) { m_alpha = alpha; }
};