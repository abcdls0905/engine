#pragma once
#include <string>
#include <vector>
#include "../math/fm_math.h"
#include "../visual/i_resource.h"
#include "defines.h"
#include "../visual/i_context.h"
//#include "visnode.h"
#include "../visual/i_ui3d_mesh.h"
_UI3D_BEGIN

class Node : public IUI3DNode
{
	friend class Mesh;
	std::string m_name;
	char * m_vb_data;
	int m_vb_size;
	int m_stride;
	short * m_ib_data;
	int m_ib_size;
	ITexture* m_tex0;
	IStaticVB* m_vb;
	IStaticIB* m_ib;
	IShaderProgram* m_shader;
	int m_fvf;
public:
	Node();
	~Node();
	void Create(int fvf, char * vb_data, int vb_data_size, short* ib_data, int ib_data_size);
	void SetTex0(ITexture* tex0) { m_tex0 = tex0; }
	bool Pick(IRenderContext* pContext, int screen_x, int screen_y, FmVec2& uv, float& t);
private:
	void CreateVBIB();
	void Draw();
	void BuildWorldMatrix();
	static void s_render(void* data);
	void render();
};


_UI3D_END