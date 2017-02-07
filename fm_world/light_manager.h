//--------------------------------------------------------------------
// �ļ���:		light_manager.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��9��29��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _LIGHT_MANAGER_H
#define _LIGHT_MANAGER_H

#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../utils/array_pod.h"
#include "../visual/i_vis_base.h"
#include "../visual/i_resource.h"
#include "../visual/i_context.h"
#include "../visual/shader_list.h"
#include "../visual/i_render.h"
#include "../visual/i_light_manager.h"

// �ƹ����

class LightSource;

class LightManager: public ILightManager
{
private:
	enum POINT_PS_SHIFT_NUM
	{
		POINT_PS_SHADOW,
		POINT_PS_LOWFILTER,
		POINT_PS_HIGHFILTER,
		POINT_PS_VSM,
		POINT_PS_PCF,
		POINT_PS_ATTENU2,
		POINT_PS_STEREO,
		POINT_PS_MAX,
	};

	enum SPOT_PS_SHIFT_ENUM
	{
		SPOT_PS_SHADOW,
		SPOT_PS_LOWFILTER,
		SPOT_PS_HIGHFILTER,
		SPOT_PS_VSM,
		SPOT_PS_PCF,
		SPOT_PS_OVERLAY,
		SPOT_PS_ATTENU2,
		SPOT_PS_STEREO,
		SPOT_PS_MAX,
	};

	enum BOX_PS_SHIFT_ENUM
	{
		BOX_PS_ATTENU2,
		BOX_PS_STEREO,
		BOX_PS_MAX,
	};

	struct light_t
	{
		PERSISTID LightID;
		float fDistance;
	};

	struct view_light_t
	{
		PERSISTID LightID;
		float fDistance;
	};

	struct dummy_t
	{
		int nSize;
		int nFormat;
		IColorRT* pColorRT;
	};

	struct depth_t
	{
		int nSize;
		int nFormat;
		IDepthRT* pDepthRT;
	};

public:
	LightManager();
	virtual ~LightManager();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	// ���ó���������
	virtual void SetContext(IRenderContext* value);

	virtual void Update(float seconds);
	virtual void Realize();

	// ����Դ����
	void SetMaxLightNum(int value) { m_nMaxLightNum = value; }
	int GetMaxLightNum() const { return m_nMaxLightNum; }

	// ����Դ����
//	void SetMaxDistance(float value);
//	float GetMaxDistance() const;

	// �Ƿ�������Դ
	void SetSunLighting(bool value) { m_bSunLighting = value; }
	bool GetSunLighting() const { return m_bSunLighting; }

	// �Ƿ�����̬����ʱ��������Դ����
	void SetRolePointLighting(bool value) { m_bRolePointLighting = value; }
	bool GetRolePointLighting() const { return m_bRolePointLighting; }

	// �Ƿ�����Դ��Ӱ
	void SetEnableShadow(bool value) { m_bEnableShadow = value; }
	bool GetEnableShadow() const { return m_bEnableShadow; }

	// �Ƿ��Ż���Ӱͼ������
	void SetShadowMapOptimal(bool value) { m_bShadowMapOptimal = value; }
	bool GetShadowMapOptimal() const { return m_bShadowMapOptimal; }

	// �Ƿ�ʹ����ȱ߽����
	void SetUseDepthBoundTest(bool value) { m_bUseDepthBoundTest = value; }
	bool GetUseDepthBoundTest() const { return m_bUseDepthBoundTest; }

	// ���б��
	void SetDepthSlope(float value) { m_fDepthSlope = value; }
	float GetDepthSlope() const { return m_fDepthSlope; }

	// ���ƫ��
	void SetDepthBias(float value) { m_fDepthBias = value; }
	float GetDepthBias() const { return m_fDepthBias; }

	// ��Ӱͼ���˼���
	void SetFilterLevel(int value) { m_nFilterLevel = value; }
	int GetFilterLevel() const { return m_nFilterLevel; }

	// ������Դ����
	PERSISTID Create();
	// ���������ļ�������Դ����
	PERSISTID CreateFromIni(const PERSISTID& id,const char* ini_name, 
		const char* light_name, const char* helper_name);
	// ɾ����Դ����
	bool Delete(const PERSISTID& id);
	// ���ع�Դ�б�
	void GetLightList(const IVarList& args, IVarList& result);

	// �������ͶӰ��դ��״̬
	bool ClearShadowRasterState();

private:
	// Ӧ�õƹ����
	void ApplyLightParam();
	// �������������ĵ��Դ
	void CheckRoleLight();
	// ����ͶӰ��դ��״̬
	void CreateShadowRasterState();

	// ����Ƚ�
	static int DistanceCompare(const void* elem1, const void* elem2);

private:
	IRenderContext* m_pShadowContext;
	TArrayPod<light_t, 1, TCoreAlloc> m_Lights;
	TArrayPod<view_light_t, 1, TCoreAlloc> m_LightInView;
	IVertexShader* m_pPointVS;
	IVertexShader* m_pSpotVS;
	IVertexShader* m_pBoxVS;
	TShaderList<IPixelShader, POINT_PS_MAX> m_PointPSList;
	TShaderList<IPixelShader, SPOT_PS_MAX> m_SpotPSList;
	TShaderList<IPixelShader, BOX_PS_MAX> m_BoxPSList;
	IVertexShader* m_pLightMaskVS;
	IPixelShader* m_pLightMaskPS;
	//IRasterState* m_pLightMaskRasterState;
	//IDepthState* m_pLightMaskDepthState;
	//IBlendState* m_pLightMaskBlendState;
	//IRasterState* m_pLightDrawRasterState;
	//IDepthState* m_pLightDrawDepthState;
	//IBlendState* m_pLightDrawBlendState;
	//IRasterState* m_pShadowRasterState;
	int m_nMaxLightNum;
	bool m_bSunLighting;
	bool m_bRolePointLighting;
	bool m_bEnableShadow;
	bool m_bShadowMapOptimal;
	bool m_bUseDepthBoundTest;
	float m_fDepthSlope;
	float m_fDepthBias;
	int m_nFilterLevel;
	IStaticTex* m_pRandomTex;
	IRender* m_pRender;
};

#endif // _LIGHT_MANAGER_H
