#ifndef _VISUAL_ICONTEXT_H
#define _VISUAL_ICONTEXT_H

#include "../public/Macros.h"


#include "i_resource.h"

class IEntity;
class IVisBase;
class ITexture;

class IRenderContext
{
public:
	// ����������
	enum { MAX_CLIP_PLANE = 4 };

	// ����Դ��Ӱͼ����
	enum { MAX_SHADOWMAP_NUM = 1 };

	// ����ֵ
	enum
	{
		I_FOG_EXP,				// �Ƿ���ָ����Ч 
		I_FOG_LINEAR,			// �Ƿ���������Ч
		I_UNDER_WATER,			// �Ƿ���ˮ��
		I_SKIN_SPHERE_AMBIENT,	// ��������ģ���Ƿ������򻷾���
		I_MODEL_SPHERE_AMBIENT,	// ��ͨģ���Ƿ������򻷾���
		I_ENABLE_ROLE_LIGHT,	// ������ɫ���Դ����
		I_ENABLE_ROLE_FALLOFF,	// ������ɫ������
		I_WATER_ENVMAP,			// ��ǰ�Ƿ�����Ⱦˮ��ĵ�Ӱ
		I_ENABLE_CAMERA_LIGHT,	// �����������Դ
		I_USE_CLIP_PLANE,		// ʹ�ü�����
		I_USE_TESSELLATION,		// ʹ�ñ���ϸ��
		I_HEIGHT_FOG,			// �Ƿ����߶���
		MAX_INT_NUM,
	};

	// ����ֵ
	enum 
	{
		F_FOG_HEIGHT,			// ������߶�
		F_FOG_START,			// ��ǰ���������ʼ����
		F_FOG_END,				// ��ǰ������Ľ�������
		F_FOG_DENSITY,			// ָ����Ũ��
		F_FOG_START_GROUND,		// ����������
		F_FOG_END_GROUND,		
		F_FOG_DENSITY_GROUND,		
		F_FOG_START_UNDERWATER,	// ˮ�µ������	
		F_FOG_END_UNDERWATER,
		F_FOG_DENSITY_UNDERWATER,		
		F_WIND_ANGLE,			// ����
		F_WIND_SPEED,			// ����
		F_AMBIENT_INTENSITY,	// ����������ϵ��
		F_DIFFUSE_INTENSITY,	// ����Դ����ϵ��
		F_SPECULAR_INTENSITY,	// �����������ϵ��
		F_ROLE_AMBIENT_FACTOR,	// ��ɫ�������������
		F_ROLE_DIFFUSE_FACTOR,	// ��ɫ�������������
		F_ROLE_LIGHT_RANGE,		// ��ɫ���Դ��Χ
		F_ROLE_LIGHT_INTENSITY,	// ��ɫ���Դ����
		F_ROLE_FALLOFF_INTENSITY, // ��ɫ����������
		F_ROLE_FALLOFF_POWER,	// ��ɫ������ָ��
		F_WATER_HEIGHT,			// ��Ⱦˮ�浹Ӱʱ��ˮ��߶�
		F_REFLECT_FACTOR,		// ��������ĳ˷�ϵ��
		F_TESS_FACTOR_EDGE,		// ����ϸ�ֲ���
		F_TESS_FACTOR_INSIDE,
		F_HEIGHT_FOG_START,		// ��ǰ�߶������ʼ�߶�
		F_HEIGHT_FOG_END,		// ��ǰ�߶���Ľ����߶�
		MAX_FLOAT_NUM,
	};

	// ��ɫֵ
	enum
	{
		C_AMBIENT_COLOR,		// ��������ɫ
		C_DIFFUSE_COLOR,		// ����Դ��ɫ
		C_SPECULAR_COLOR,		// ���������ɫ
		C_FOG_COLOR,			// ��ǰ��������ɫ
		C_FOG_COLOR_GROUND,		// �������������ɫ
		C_FOG_COLOR_UNDERWATER, // ˮ�µ���������ɫ
		C_FOG_EXP_COLOR,		// ָ������ɫ
		C_ROLE_LIGHT_COLOR,		// ��ɫ���Դ��ɫ
		C_ROLE_FALLOFF_COLOR,	// ��ɫ��������ɫ
		C_CAMERA_LIGHT_COLOR,	// �������Դ��ɫ
		C_HEIGHTFOG_COLOR,		// ��ǰ�߶�����ɫ
		MAX_COLOR_NUM,
	};

	// ����ֵ
	enum
	{
		V_LIGHT_DIRECTION,		// ����Դ�ķ���
		V_LIGHT_AMBIENT,		// ����Դambient
		V_LIGHT_DIFFUSE,		// ����Դdiffuse
		V_LIGHT_SPECULAR,		// ����Դspecular
		V_FOCUS_POSITION,		// ���������λ��
		V_FOCUS_RADIUS,			// ���������뾶
		V_ROLE_LIGHT_POSITION,	// ��ɫ���Դλ��
		V_FOG_CURRENT,			// ��ǰ��������ɫֵ
		V_FOG_EXP_CURRENT,		// ��ǰָ������ɫֵ
		V_CLIP_PLANE,			// ������
		V_HEIGHTFOG_CURRENT,	// ��ǰ�߶�����ɫֵ
		MAX_VECTOR_NUM,
	};
public:
	virtual ~IRenderContext(){}
	
	// �ͷ�
	virtual void Release() = 0;

	// �����Ӿ������
	virtual bool SetCameraView(const FmVec3& pos, 
		const FmVec3& dir)  = 0;
	// ����ͶӰ�������
	virtual bool SetCameraProj(float fov, float zn, float zf, float ratio)  = 0; 
	// �����Ӿ���
	virtual bool SetViewMatrix(const FmMat4& mat)  = 0;
	// ����ͶӰ����
	virtual bool SetProjMatrix(const FmMat4& mat)  = 0;
	// ���ɸ�������
	virtual void MakeHelperMatrix()  = 0;
	// ������ȾĿ��ߴ�
	virtual bool SetViewSize(int width, int height)  = 0;
	// �����ȾĿ��ߴ�
	virtual int GetViewWidth()  = 0;
	virtual int GetViewHeight()  = 0;

	// ���ɼ�����
	virtual void MakeClipPlane()  = 0;

	// ��͸�����彵����ʾ
	virtual void SetBlendDownLevel(int value)  = 0;
	virtual int GetBlendDownLevel()  = 0;
	virtual IColorRT* GetBlendColorRT()  = 0;
	virtual IDepthRT* GetBlendDepthRT()  = 0;
	
	// �Ƿ�Ҫ��Ⱦ����ͼ
	virtual void SetEnableRealizeRefraction(bool value)  = 0;
	virtual bool GetEnableRealizeRefraction()  = 0;
	virtual IColorRT* GetRefractionRT()  = 0;

	// �Ƿ�Ҫ������ʱ��Ⱦ������
	virtual void SetEnableRealizeTempColorRT(bool value)  = 0;
	virtual bool GetEnableRealizeTempColorRT() = 0;
	virtual IColorRT* GetRealizeTempColorRT() = 0;

	// �Ƿ�Ҫ������ʱ��Ⱦ��Ȼ���
	virtual void SetEnableRealizTempDepthRT(bool value)  = 0;
	virtual bool GetEnableRealizTempDepthRT() = 0;
	virtual IDepthRT* GetRealizeTempDepthRT() = 0;

	// �Ƿ���Clear 
	virtual void SetEnableClearColorOptimize(bool value) = 0;
	virtual bool GetEnableClearColorOptimize() = 0;

	// ���ɲ�������ʱ��Ȼ�����
	virtual void SetEnableRealizeTempDepthTex(bool value) = 0;
	virtual bool GetEnableRealizeTempDepthTex() = 0;
	virtual IDepthTex* GetRealizeTempDepthTex() = 0;

	// �ɲ�������ʱ���ģ�建����
	virtual void SetEnableRealizeTempDepthStencilRT(bool value) = 0;
	virtual bool GetEnableRealizeTempDepthStencilRT() = 0;
	virtual IDepthStencilRT* GetRealizeTempDepthStencilRT() = 0;

	// ��ʱģ�建����
	virtual void SetEnableRealizeTempStencilTex(bool value) = 0;
	virtual bool GetEnableRealizeTempStencilTex() = 0;
	virtual IStencilTex* GetRealizeTempStencilTex() = 0;

	// �Ƿ�Ҫ������ʱ������Ⱦ������
	virtual void SetEnableRealizeCopyTempColorRT(bool value)  = 0;
	virtual bool GetEnableRealizeCopyTempColorRT() = 0;
	virtual IColorRT* GetRealizeCopyTempColorRT() = 0;

	// �Ƿ�Ҫ������ʱ������Ⱦ��Ȼ���
	virtual void SetEnableRealizeCopyTempDepth(bool value)  = 0;
	virtual bool GetEnableRealizeCopyTempDepth() const  = 0;
	virtual IColorRT* GetRealizeCopyTempDepth() = 0;

	// ���ڲ���RT��Ⱦ�ĵ��Ժ��� ����debug�汾
	virtual void SetDebugColorRT(IColorRT* pColorRT)  = 0;
	virtual IColorRT* GetDebugColorRT()  = 0;

	// ���ڲ���RT��Ⱦ���Ժ��� ���� debug�汾
	virtual void SetDebugDepthRT(IDepthRT* pDepthRT)  = 0;
    virtual IDepthRT* GetDebugDepthRT()  = 0;

	// ���ڲ���RT��Ⱦ���Ժ��� ����debug�汾
	virtual void SetDebugDepthStencilRT(IDepthStencilRT* pDepthStencilRT)  = 0;
	virtual IDepthStencilRT* GetDebugDepthStencilRT() = 0;

	// ���ڲ���Texture��Ⱦ�ĵ��Ժ��� ����debug�汾
	virtual void SetDebugColorTexture(ITexture* pTex)  = 0;
    virtual ITexture* GetDebugTexture()  = 0;

	// ���ڲ���һЩ����ColorRT ����debug�汾Ŷ
	virtual void SetDebugDecodeColorRT(IColorRT* pColorRT)  = 0;
    virtual IColorRT* GetDebugDecodeColorRT()  = 0;

	// �Ƿ���ȫ�ֻ�����ͼ
	virtual void SetGlobalEnvMap(const char* name) = 0;
	virtual const char* GetGlobalEnvMap() = 0;
	virtual ITexture* GetGlobalEnvCubeTex() = 0;

	// �Ƿ�����̬��Ӱ
	virtual void SetEnableDynamicShadow(bool value) = 0;
	virtual bool GetEnableDynamicShadow() = 0;

	// �Ƿ�ʹ��Ӳ����Ӱͼ����
	virtual void SetPercentageCloserFilter(bool value) = 0;
	virtual bool GetPercentageCloserFilter() = 0;

	// �Ƿ�ʹ�÷�����Ӱͼ
	virtual void SetVarianceShadowMap(bool value) = 0;
	virtual bool GetVarianceShadowMap() = 0;

	// ������Ӱͼ����
	virtual void CalculateShadowMapMatrix() = 0;
	
	// �����Ӱͼ����
	virtual const FmMat4* GetShadowMapViewMatrix(int index) = 0;
	virtual const FmMat4* GetShadowMapProjMatrix(int index) = 0; 

	// ��Ӱͼ�Ƿ���Լ���ʹ�ã�����ˢ�£�
	virtual bool GetShadowMapNoFlush(int index) = 0;
	
	// ���б��ƫ��
	virtual void SetDepthSlope(int index, float value) = 0;
	virtual float GetDepthSlope(int index) = 0;

	// ��ȳ���ƫ��
	virtual void SetDepthBias(int index, float value) = 0;
	virtual float GetDepthBias(int index) = 0;

	// ��Ӱͼ����
	virtual void SetShadowMapCount(int value) = 0;
	virtual int GetShadowMapCount() = 0;
	
	// ��ǰ��Ӱͼ����
	virtual void SetShadowMapIndex(int value) = 0;
	virtual int GetShadowMapIndex() = 0;

	// ��Ӱͼ�����з�Ȩ��
	virtual void SetLogSplitWeight(float value) = 0;
	virtual float GetLogSplitWeight() = 0;

	// ��Ӱͼ�ߴ�
	virtual void SetShadowMapSize(unsigned int value) = 0;
	virtual unsigned int GetShadowMapSize() = 0;
	
	// ��Ӱͼ�������Ŀ������ľ���
	virtual void SetShadowMapCameraDistance(float value) = 0;
	virtual float GetShadowMapCameraDistance() = 0;

	// ��ӰͶӰ�����չ�ٷֱ�
	virtual void SetShadowExpandPercent(float value) = 0;
	virtual float GetShadowExpandPercent() = 0;

	// Զ������Ӱͼ�Ƿ�����ظ�ʹ��
	virtual void SetShadowMapReusable(bool value) = 0;
	virtual bool GetShadowMapReusable() = 0;

	// �����Ӱͼ��ȾĿ��
	virtual IColorRT* GetShadowMapRT(int index) = 0;
  virtual IDepthTex* GetShadowMapDS(int index) = 0;
  virtual IDepthRT* GetDepthRT() = 0;

	// �����Ļ�ռ䶯̬��Ӱ���ͼ
	virtual IColorRT* GetDynamicShadowRT() = 0;

	// ���Ƿ����
	virtual bool LayerExists(const char* name) = 0;
	// ��ղ�
	virtual bool LayerClear(const char* name) = 0;
	// ��Ӷ��󵽲�
	virtual bool LayerAddObject(const char* name, IVisBase* pObj) = 0;
	// �Ӳ�ɾ������
	virtual bool LayerRemoveObject(const char* name, IVisBase* pObj) = 0;
	// �Բ�����ѡ
	virtual bool LayerCull(const char* name, const FmPlane* planes, 
		size_t plane_num, const FmVec3& center, float radius, 
		CULL_FUNC cull_func, void* context) = 0;
	// �Բ�����ײ���
	virtual float LayerTrace(const char* name, const FmVec3& src, 
		const FmVec3& dst) = 0;
	virtual bool LayerTraceHitted(const char* name, const FmVec3& src, 
		const FmVec3& dst) = 0;

	// ���ó���Ψһ��ʵ��
	virtual bool SetUnique(IEntity* pEntity) = 0;
	// ��ó���Ψһ��ʵ��
	virtual IEntity* GetUnique(const char* entity_name) = 0;

	// ����ֵ
	virtual int GetInt(size_t index) const  = 0;
	
	virtual void SetInt(size_t index, int value) = 0;
	
	// ����ֵ
	virtual float GetFloat(size_t index) const = 0;
	
	virtual void SetFloat(size_t index, float value) = 0;

	// ��ɫֵ
	virtual unsigned int GetColor(size_t index) const = 0;

	virtual void SetColor(size_t index, unsigned int value) = 0;
	
	// ����ֵ
	virtual const FmVec3& GetVector(size_t index) const = 0;
	
	virtual void SetVector(size_t index, const FmVec3& value) = 0;
	
	// 4Ԫ����ֵ
	virtual const FmVec4& GetVector4(size_t index) const = 0;
	
	virtual void SetVector4(size_t index, const FmVec4& value) = 0;
	
	// �ο�λ��
	virtual void SetReferPosition(float x, float y, float z) = 0;
	
	virtual const FmVec3& GetReferPosition() const  = 0;

	// ��õ�ǰ���������
	virtual const camera_t& GetCamera() const = 0;
	// �����һ֡�����������
	virtual const camera_t& GetPrevCamera() const = 0;
	// ������һ֡�����������
	virtual void SavePrevCamera() = 0;

	// ��ü�����
	virtual const FmPlane* GetClipPlanes() const  = 0;
	// ��ü���������
	virtual int GetClipPlaneAmount() const = 0;

	// ����Ӿ�
	virtual void SetFarClipDistance(float value) = 0;
	virtual float GetFarClipDistance() const = 0;

	// ��Χ���Ƿ񳬹�����Ӿ�
	virtual bool IsFarClip(const FmVec3& pos, float radius) = 0;

	// ��Χ���Ƿ񳬹����ˮƽ�Ӿ�
	virtual bool IsFarClip2D(const FmVec3& pos, float radius) = 0;
	
	// ��Χ���Ƿ��ڼ�������
	virtual bool IsViewClip(const FmVec3& pos, float radius) = 0;

	// �Ƿ�Ҫ�������Ԥ��Ⱦ
	virtual void SetEnableEarlyZ(bool value) = 0;
	virtual bool GetEnableEarlyZ() const = 0;

	// ��̬��Ӱ��Ч����
	void SetShadowDistance(float value) { m_fShadowDistance = value; }
	float GetShadowDistance() const { return m_fShadowDistance; }

	// ���Դ������
	void SetMaxLightDistance(float value) { m_fMaxLightDistance = value; }
	float GetMaxLightDistance() const { return m_fMaxLightDistance; }

	// �Ƿ��������������ʱ�Ķ�̬��Ӱ��Ե����
	void SetEnableFixShadow(bool value) { m_bEnableFixShadow = value; }
	bool GetEnableFixShadow() const { return m_bEnableFixShadow; }

	// ���ö�̬��Ӱ�������������
	// vAdjustingRect.x = left
	// vAdjustingRect.y = right
	// vAdjustingRect.z = top
	// vAdjustingRect.w = bottom
	virtual void SetShadowAdjustingRect(FmVec4 vAdjustingRect) = 0;

	// ��ȡ��̬��Ӱ�������������
	// vResult.x = left
	// vResult.y = right
	// vResult.z = top
	// vResult.w = bottom
	virtual const FmVec4& GetShadowAdjustingRect() = 0;

protected:

	float m_fShadowDistance;
	float m_fMaxLightDistance;
	bool m_bEnableFixShadow;
};
#endif