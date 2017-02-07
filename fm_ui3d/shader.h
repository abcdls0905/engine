#pragma once
#include <map>
#include "defines.h"
#include "../public/core_mem.h"
#include "../visual/i_resource.h"
#include "../utils/pod_hash_map.h" 

struct UI3DShaderKey
{
	void* ps;
	void* vs;
};

template<>
class TPodTraits<UI3DShaderKey>
{
public:
	static size_t GetHash(const UI3DShaderKey& value)
	{
		return size_t(value.ps) + size_t(value.vs);
	}

	static bool Equal(const UI3DShaderKey& s1, const UI3DShaderKey& s2)
	{
		return s1.ps == s2.ps && s1.vs == s2.vs;
	}
};

_UI3D_BEGIN

enum {
	UI3D_VS_TEX,
	UI3D_VS_MAX,
};

enum {
	UI3D_PS_TEX,
	UI3D_PS_MAX
};

 
class TUI3DBatchAlloc
{
public:
	TUI3DBatchAlloc() {}
	~TUI3DBatchAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TUI3DBatchAlloc& src) {}
};



class Shader
{
	static Shader* s_inst;
	typedef TPodHashMap<UI3DShaderKey, IShaderProgram*, TPodTraits<UI3DShaderKey>, TUI3DBatchAlloc> ShaderMap;
	ShaderMap m_shaders;
	//std::map<int64_t, IShaderProgram*> m_shaders;
	~Shader();
public:
	void Init();
	void Destroy();
	IShaderProgram* GetShader(int vs_flags, int ps_flags);
	static Shader& Inst()
	{
		if(s_inst == 0)
			s_inst = new Shader;

		return *s_inst;
	}
};

_UI3D_END