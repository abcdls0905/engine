//--------------------------------------------------------------------
// 文件名:		post_effect_down_sampler.h
// 内  容:		
// 说  明:		
// 创建日期:	 
// 创建人:		
// 版权所有:	
//--------------------------------------------------------------------


#ifndef _POST_EFFECT_DOWN_SAMPLER_H_
#define _POST_EFFECT_DOWN_SAMPLER_H_

#include "../public/module.h"
#include "../public/core_type.h"
#include "../visual/i_render.h"
#include "../visual/i_vis_base.h"
#include "../public/var_list.h"
#include "post_effect.h"

#include "../visual/shader_list.h"
#include "../utils/array_pod.h"
#include "../utils/pod_hash_map.h"


// 后处理效果
/*
所有的后处理都从post_effect中集成相关的资源信息，减少重复无效的调用

*/

class TPostEffectDownSamplerShaderAlloc
{
public:
	TPostEffectDownSamplerShaderAlloc() {}
	~TPostEffectDownSamplerShaderAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TPostEffectDownSamplerShaderAlloc& src) {}
};

class CPostEffectDownSampler: public CPostEffect
{
	struct post_effect_downsampler_cb_t
	{
		bool   bGradualEnable;
		bool   bAdjustEnable;
		bool   bAngleEnable;
		FmMat4 matViewProjI;
		FmVec3 ColorAdjustParam;
		FmVec4 GradualBaseColor;
		FmVec4 GradualColor;
		FmVec3 GradualParam;
        FmVec4 vecViewDirection;
		FmVec3 AngleParam;
		FmVec4 AngleColor;
		FmVec2 TexScaleUV;
	};

	struct post_effect_downsampler_shaderhandle_t
	{
		IShaderProgram* pShader;
		fm_int mat4ViewProjIHandle;
        fm_int vec4GradualColorHandle;
		fm_int vec3GradualParamHandle;
		fm_int vec4ViewDirectionHandle;
        fm_int vec3ColorAdjustParamHandle;
		fm_int vec4GradualBaseColorHandle;
		fm_int vec3AngleParamHandle;
		fm_int vec4AngleColorHandle;
		fm_int vec2TexScaleUVHandle;
        fm_int tex_colorHandle;
	};

	enum FLAG_SHIFT_ENUM
	{
        FLAG_GRADUAL,
		FLAG_ADJUST,
		FLAG_ANGLE,
		FLAG_MAX,
	};

public:

	CPostEffectDownSampler();
	virtual ~CPostEffectDownSampler();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

    virtual void Realize();
	// 是否可见 
	virtual void SetVisible(bool value) { m_bVisible = value; }
	virtual bool GetVisible() const { return m_bVisible; }

	// 垂直颜色渐变开关
	void SetGradualEnable(bool value);
	bool GetGradualEnable() const;

	// 垂直渐变起始
	void SetGradualStart(float value);
	float GetGradualStart() const;

	// 垂直渐变结束
	void SetGradualEnd(float value);
	float GetGradualEnd() const;

	// 垂直渐变衰减系数
	void SetGradualExp(float value);
	float GetGradualExp() const;

	// 垂直渐变颜色
	void SetGradualColor(const char* value);
	result_string GetGradualColor() const;

	// 颜色调节开关
	void SetAdjustEnable(bool value);
	bool GetAdjustEnable() const;

	// 基色
	void SetAdjustBaseColor(const char* value);
	result_string GetAdjustBaseColor() const;

	// 亮度
	void SetAdjustBrightness(float value);
	float GetAdjustBrightness() const;

	// 对比度
	void SetAdjustContrast(float value);
	float GetAdjustContrast() const;

	// 饱和度
	void SetAdjustSaturation(float value);
	float GetAdjustSaturation() const;

	// 中心渐变开关
	void SetAngleEnable(bool value);
	bool GetAngleEnable() const;

	// 中心渐变起始
	void SetAngleStart(float value);
	float GetAngleStart() const;

	// 中心渐变结束
	void SetAngleEnd(float value);
	float GetAngleEnd() const;

	// 中心渐变衰减系数
	void SetAngleExp(float value);
	float GetAngleExp() const;

	// 中心渐变颜色
	void SetAngleColor(const char* value);
	result_string GetAngleColor() const;
private:
	static void RenderPostDownSamplerEffect(void* pdata);
	CPostEffectDownSampler::post_effect_downsampler_shaderhandle_t* GetShaderHandle();

	void SetShaderConstValue(CPostEffectDownSampler::post_effect_downsampler_shaderhandle_t* pShaderHandle);
protected:
	bool m_bVisible;

	bool m_bGradualEnable;
	float m_fGradualStart;
	float m_fGradualEnd;
	float m_fGradualExp;
	unsigned int m_nGradualColor;

	bool m_bAdjustEnable;
	unsigned int m_nAdjustBaseColor;
	FmVec3 m_vColorAdjustParam;

	bool m_bAngleEnable;
	float m_fAngleStart;
	float m_fAngleEnd;
	float m_fAngleExp;
	unsigned int m_nAngleColor;

	IShaderProgram* m_pShader;
	FmMat4          m_matProj;

	TShaderList<IVertexShader, FLAG_MAX> m_VSList;
	TShaderList<IPixelShader, FLAG_MAX>  m_PSList;

	TPodHashMap<unsigned int, CPostEffectDownSampler::post_effect_downsampler_shaderhandle_t*, TPodTraits<unsigned int>, TPostEffectDownSamplerShaderAlloc> m_ShaderMapHandle;

	post_effect_downsampler_cb_t m_downsampler_cb;
};


#endif