//--------------------------------------------------------------------
// �ļ���:		post_effect_down_sampler.h
// ��  ��:		
// ˵  ��:		
// ��������:	 
// ������:		
// ��Ȩ����:	
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


// ����Ч��
/*
���еĺ�����post_effect�м�����ص���Դ��Ϣ�������ظ���Ч�ĵ���

*/

class TPostEffectDownSamplerShaderAlloc
{
public:
	TPostEffectDownSamplerShaderAlloc() {}
	~TPostEffectDownSamplerShaderAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
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
	// �Ƿ�ɼ� 
	virtual void SetVisible(bool value) { m_bVisible = value; }
	virtual bool GetVisible() const { return m_bVisible; }

	// ��ֱ��ɫ���俪��
	void SetGradualEnable(bool value);
	bool GetGradualEnable() const;

	// ��ֱ������ʼ
	void SetGradualStart(float value);
	float GetGradualStart() const;

	// ��ֱ�������
	void SetGradualEnd(float value);
	float GetGradualEnd() const;

	// ��ֱ����˥��ϵ��
	void SetGradualExp(float value);
	float GetGradualExp() const;

	// ��ֱ������ɫ
	void SetGradualColor(const char* value);
	result_string GetGradualColor() const;

	// ��ɫ���ڿ���
	void SetAdjustEnable(bool value);
	bool GetAdjustEnable() const;

	// ��ɫ
	void SetAdjustBaseColor(const char* value);
	result_string GetAdjustBaseColor() const;

	// ����
	void SetAdjustBrightness(float value);
	float GetAdjustBrightness() const;

	// �Աȶ�
	void SetAdjustContrast(float value);
	float GetAdjustContrast() const;

	// ���Ͷ�
	void SetAdjustSaturation(float value);
	float GetAdjustSaturation() const;

	// ���Ľ��俪��
	void SetAngleEnable(bool value);
	bool GetAngleEnable() const;

	// ���Ľ�����ʼ
	void SetAngleStart(float value);
	float GetAngleStart() const;

	// ���Ľ������
	void SetAngleEnd(float value);
	float GetAngleEnd() const;

	// ���Ľ���˥��ϵ��
	void SetAngleExp(float value);
	float GetAngleExp() const;

	// ���Ľ�����ɫ
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