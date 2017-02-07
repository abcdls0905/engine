//--------------------------------------------------------------------
// 文件名:		sun_glow.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SUN_GLOW_H
#define _SUN_GLOW_H

#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "../visual/i_vis_base.h"
#include "../visual/i_texture.h"
#include "../visual/i_resource.h"
#include "../visual/i_render.h"
#include "../visual/i_sun_glow.h"
// 太阳光晕

class SunGlow: public ISunGlow
{
private:
	// 最大光晕数量
	enum { MAX_FLARE_NUM = 128 };

	struct sunglow_vertex_t 
	{
		FmVec3 pos;
		FmVec4 diffuse;
		float tu;
		float tv;
	};

	struct sun_flare_t
	{
		FmVec3 vPos;
		float fSize;
		float fAngle;
		unsigned int nDiffuse;
		size_t nSubTex;
		float fDistance;
		unsigned int nColor;
	};

	struct sun_flare_shader_handle
	{
		IShaderProgram* pShader;
		fm_int tex_BaseMapHandle;
        fm_int mat4mtxWVP;
	};

public:
	SunGlow();
	virtual ~SunGlow();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual bool Load();
	
	virtual void Update(float seconds);
	virtual void Realize();

	// 是否加载完成
	virtual bool GetLoadFinish(); 

	// 阳光的尺寸
	void SetGlowSize(float value);
	float GetGlowSize() const;

	// 光晕的尺寸
	void SetFlareSize(float value);
	float GetFlareSize() const;
	
	// 阳光的距离
	void SetGlowDistance(float value);
	float GetGlowDistance() const;
	
	// 光晕的距离
	void SetFlareDistance(float value);
	float GetFlareDistance() const;

	// 阳光旋转速度
	void SetGlowRotateSpeed(float value);
	float GetGlowRotateSpeed() const;
	
	// 是否显示阳光
	void SetShowGlow(bool value);
	bool GetShowGlow() const;
	
	// 是否显示光晕
	void SetShowFlare(bool value);
	bool GetShowFlare() const;

	// 是否异步加载
	void SetAsyncLoad(bool value) { m_bAsyncLoad = value; }
	bool GetAsyncLoad() const { return m_bAsyncLoad; }
	
	// 太阳的贴图
	void SetGlowTex(const char* value);
	const char* GetGlowTex() const;

	// 光晕的贴图
	void SetFlareTex(const char* value);
	const char* GetFlareTex() const;

	// 更新贴图
	bool UpdateTexture();

	// 添加光晕数据
	bool AddFlare(float distance, float size, int sub_tex, const char* color);
	// 清空光晕数据
	bool ClearFlare();

private:
	// 加载贴图
	bool LoadTexture();
	// 释放贴图
	bool UnloadTexture();
	// 画太阳
	bool DrawSun(sun_flare_t* flares, size_t flare_num, size_t sub_tex_x, 
		size_t sub_tex_y, ITexture* pTexture);
	// 画光晕
	bool DrawFlares(sun_flare_t* flares, size_t flare_num, size_t sub_tex_x, 
		size_t sub_tex_y, ITexture* pTexture, IDynamicVB* pVBuffer);


	SunGlow::sunglow_vertex_t* LockVBuffer();

	void UnLockVBuffer();

	SunGlow::sunglow_vertex_t* LockSunVBuffer();

	void UnLockSunVBuffer();

	// 绘制太阳
	static void RenderSun(void* pdata);
	// 绘制光斑
    static void RenderFlares(void* pdata);

private:
	IRender* m_pRender;
	float m_fGlowSize;
	float m_fFlareSize;
	float m_fGlowDistance;
	float m_fFlareDistance;
	float m_fGlowRotateSpeed;
	bool m_bShowGlow;
	bool m_bShowFlare;
	bool m_bAsyncLoad;

	IDynamicVB* m_pSunVBuffer;
	IDynamicVB* m_pVBuffer;
	SunGlow::sunglow_vertex_t* m_pSunVBufferData;
	SunGlow::sunglow_vertex_t* m_pVBufferData;
	IStaticIB* m_pIBuffer;
 
	IVertexShader* m_pGlowVS;
	IPixelShader* m_pGlowPS;

	SunGlow::sun_flare_shader_handle* m_pShaderHandle;

	ITexture* m_pGlowTex;
	ITexture*	m_pFlareTex;
	core_string m_strGlowTex;
	core_string m_strFlareTex;
	bool m_bGlowVisible;
	bool m_bFlareVisible;
	float m_fGlowAlpha;
	float m_fFlareAlpha;
	TArrayPod<sun_flare_t, 1, TCoreAlloc> m_Flares;

	FmMat4 m_mtxWVP;
};

#endif // _SUN_GLOW_H
