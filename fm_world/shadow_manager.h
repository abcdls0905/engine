//--------------------------------------------------------------------
// 文件名:		shadow_manager.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年4月2日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SHADOW_MANAGER_H
#define _SHADOW_MANAGER_H

#include "../visual/i_vis_base.h"
#include "../visual/i_resource.h"
#include "../utils/array_pod.h"
#include "../visual/i_render.h"
#include "../visual/i_context.h"
#include "../visual/i_scene_view.h"
#include "../visual/i_shadow_manager.h"

// 影子管理
 
class ShadowManager: public IShadowManager
{
public:
	// 简易影子的shadow声明
	struct shadow_shaderhandle_t
	{
		IShaderProgram* pShader;
		//shadow中的句柄常量
        fm_int c_mtxViewProjHandle;
		fm_int c_mtxLightMVPHandle;
		fm_int c_fLightDispersionHandle;
		fm_int c_TopColorHandle;
		fm_int c_BottomColorHandle;
	};


private:
	struct shadow_t
	{
		PERSISTID ShadowID;
		float fDistance;
		bool bShow;
	};

public:
	ShadowManager();
	virtual ~ShadowManager();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	// 设置场景上下文
	virtual void SetContext(IRenderContext* value);

	virtual bool Load();
	
	virtual void Update(float seconds);
	virtual void Realize();

	// 是否可见
	virtual void SetVisible(bool value);
	virtual bool GetVisible() const;

	// 最多影子数量
	void SetMaxShadowNum(int value);
	int GetMaxShadowNum() const;

	// 主角玩家对象的影子
	void SetPlayerShadowID(const PERSISTID& value);
	PERSISTID GetPlayerShadowID() const;

	// 贴图尺寸
	void SetTextureSize(int value);
	int GetTextureSize() const;

	// 设置是否只用简易阴影
	void SetUseSimpleShadow(bool value);
	bool GetUseSimpleShadow() const;

	// 创建影子对象
	PERSISTID Create(const PERSISTID& id);
	// 删除影子对象
	bool Delete(const PERSISTID& id);

	// 返回影子列表
	void GetShadowList(const IVarList& args, IVarList& result);

	// 获得着色器
	IShaderProgram* GetShadowShader() const { return m_pShadowShader; };

	ShadowManager::shadow_shaderhandle_t* GetSimpleShadowShaderHandle() { return m_pSimpleShadowShaderHandle; };

	IShaderProgram* GetSimpleShadowShader(){ return m_pSimpleShadowShader; };

	/*
	IDxInputLayout* GetInputLayout() const { return m_pInputLayout; }

	// 获得混合状态
	IDxBlendState* GetBlendState() const { return m_pBlendState; }

	// 获得采样器状态
	//IDxSamplerState* GetSampler() const { return m_pSampler; }
	*/

	// 获得深度缓冲
	IDepthRT* GetShadowDepth() const 
	{ 
		return m_pShadowDepth; 
	}

private:
	// 距离比较
	static int __cdecl DistanceCompare(const void* elem1, const void* elem2);

private:
	IRender*    m_pRender;

	TArrayPod<shadow_t, 8, TCoreAlloc> m_Shadows;
	bool m_bVisible;
	int m_nMaxShadowNum;
	PERSISTID m_PlayerShadowID;
	int m_nTextureSize;
	/*
	IDxInputLayout* m_pInputLayout;
	IDxBlendState* m_pBlendState;
	*/

	IShaderProgram* m_pSimpleShadowShader;

	ShadowManager::shadow_shaderhandle_t* m_pSimpleShadowShaderHandle;

	IShaderProgram* m_pShadowShader;
	
	bool m_bUseSimpleShadow;
	//IDxSamplerState* m_pSampler;
	IDepthRT* m_pShadowDepth;
	float     m_f_offset_seconds;
};
 

#endif // _SHADOW_MANAGER_H
