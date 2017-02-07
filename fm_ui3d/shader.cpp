#include "shader.h"
#include "../visual/i_render.h"
#include "../visual/i_resource.h"
#include "../visual/shader_list.h"
#include "global.h"
_UI3D_BEGIN

static TShaderManyDefines<IVertexShader, UI3D_VS_MAX> s_VSList;
static TShaderManyDefines<IPixelShader, UI3D_PS_MAX> s_PSList;
Shader* Shader::s_inst = 0;

void Shader::Init()
{
	s_VSList.SetRender(g_fm_ui3d_gloabl.pRender);
	s_VSList.SetShaderName("ui3d.vsh", "main");
	s_VSList.SetDefine(UI3D_VS_TEX, "#define USE_TEX\n");
	s_VSList.GetShader(0);
	s_PSList.SetRender(g_fm_ui3d_gloabl.pRender);
	s_PSList.SetShaderName("ui3d.fsh", "main");
	s_PSList.SetDefine(UI3D_PS_TEX, "#define USE_TEX\n");
	s_PSList.GetShader(0);
}

Shader::~Shader()
{
	
}

void Shader::Destroy()
{
	s_VSList.Clear();
	s_PSList.Clear();
	if(s_inst)
	{
		delete(s_inst);
		s_inst = 0;
	}
}

IShaderProgram* Shader::GetShader(int vs_flags, int ps_flags)
{
	IVertexShader* pVS = s_VSList.GetShader(vs_flags);
	IPixelShader* pPS = s_PSList.GetShader(ps_flags);
	
	UI3DShaderKey key;
	key.ps = pPS;
	key.vs = pVS;

	if(m_shaders.Exists(key))
	{
		return m_shaders.Find(key).GetData();
	}

	IShaderProgram* pShaderProgram;
	const char* vertex_t_name[] = 
	{
		"iPos",
		"iUV",
	};

	pShaderProgram = g_fm_ui3d_gloabl.pRender->CreateShaderProgram(pVS, pPS, vertex_t_name, 2);
	m_shaders.Add(key, pShaderProgram);

	return pShaderProgram;
}

_UI3D_END