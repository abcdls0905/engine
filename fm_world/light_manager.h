//--------------------------------------------------------------------
// 文件名:		light_manager.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年9月29日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
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

// 灯光管理

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
	
	// 设置场景上下文
	virtual void SetContext(IRenderContext* value);

	virtual void Update(float seconds);
	virtual void Realize();

	// 最多光源数量
	void SetMaxLightNum(int value) { m_nMaxLightNum = value; }
	int GetMaxLightNum() const { return m_nMaxLightNum; }

	// 最大光源距离
//	void SetMaxDistance(float value);
//	float GetMaxDistance() const;

	// 是否开启主光源
	void SetSunLighting(bool value) { m_bSunLighting = value; }
	bool GetSunLighting() const { return m_bSunLighting; }

	// 是否开启静态光照时的人物点光源照明
	void SetRolePointLighting(bool value) { m_bRolePointLighting = value; }
	bool GetRolePointLighting() const { return m_bRolePointLighting; }

	// 是否开启光源阴影
	void SetEnableShadow(bool value) { m_bEnableShadow = value; }
	bool GetEnableShadow() const { return m_bEnableShadow; }

	// 是否优化阴影图的生成
	void SetShadowMapOptimal(bool value) { m_bShadowMapOptimal = value; }
	bool GetShadowMapOptimal() const { return m_bShadowMapOptimal; }

	// 是否使用深度边界测试
	void SetUseDepthBoundTest(bool value) { m_bUseDepthBoundTest = value; }
	bool GetUseDepthBoundTest() const { return m_bUseDepthBoundTest; }

	// 深度斜率
	void SetDepthSlope(float value) { m_fDepthSlope = value; }
	float GetDepthSlope() const { return m_fDepthSlope; }

	// 深度偏置
	void SetDepthBias(float value) { m_fDepthBias = value; }
	float GetDepthBias() const { return m_fDepthBias; }

	// 阴影图过滤级别
	void SetFilterLevel(int value) { m_nFilterLevel = value; }
	int GetFilterLevel() const { return m_nFilterLevel; }

	// 创建光源对象
	PERSISTID Create();
	// 根据配置文件创建光源对象
	PERSISTID CreateFromIni(const PERSISTID& id,const char* ini_name, 
		const char* light_name, const char* helper_name);
	// 删除光源对象
	bool Delete(const PERSISTID& id);
	// 返回光源列表
	void GetLightList(const IVarList& args, IVarList& result);

	// 清除所有投影光栅化状态
	bool ClearShadowRasterState();

private:
	// 应用灯光参数
	void ApplyLightParam();
	// 设置离玩家最近的点光源
	void CheckRoleLight();
	// 创建投影光栅化状态
	void CreateShadowRasterState();

	// 距离比较
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
