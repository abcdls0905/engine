//--------------------------------------------------------------------
// �ļ���:		shadow_manager.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��4��2��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

// Ӱ�ӹ���
 
class ShadowManager: public IShadowManager
{
public:
	// ����Ӱ�ӵ�shadow����
	struct shadow_shaderhandle_t
	{
		IShaderProgram* pShader;
		//shadow�еľ������
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
	
	// ���ó���������
	virtual void SetContext(IRenderContext* value);

	virtual bool Load();
	
	virtual void Update(float seconds);
	virtual void Realize();

	// �Ƿ�ɼ�
	virtual void SetVisible(bool value);
	virtual bool GetVisible() const;

	// ���Ӱ������
	void SetMaxShadowNum(int value);
	int GetMaxShadowNum() const;

	// ������Ҷ����Ӱ��
	void SetPlayerShadowID(const PERSISTID& value);
	PERSISTID GetPlayerShadowID() const;

	// ��ͼ�ߴ�
	void SetTextureSize(int value);
	int GetTextureSize() const;

	// �����Ƿ�ֻ�ü�����Ӱ
	void SetUseSimpleShadow(bool value);
	bool GetUseSimpleShadow() const;

	// ����Ӱ�Ӷ���
	PERSISTID Create(const PERSISTID& id);
	// ɾ��Ӱ�Ӷ���
	bool Delete(const PERSISTID& id);

	// ����Ӱ���б�
	void GetShadowList(const IVarList& args, IVarList& result);

	// �����ɫ��
	IShaderProgram* GetShadowShader() const { return m_pShadowShader; };

	ShadowManager::shadow_shaderhandle_t* GetSimpleShadowShaderHandle() { return m_pSimpleShadowShaderHandle; };

	IShaderProgram* GetSimpleShadowShader(){ return m_pSimpleShadowShader; };

	/*
	IDxInputLayout* GetInputLayout() const { return m_pInputLayout; }

	// ��û��״̬
	IDxBlendState* GetBlendState() const { return m_pBlendState; }

	// ��ò�����״̬
	//IDxSamplerState* GetSampler() const { return m_pSampler; }
	*/

	// �����Ȼ���
	IDepthRT* GetShadowDepth() const 
	{ 
		return m_pShadowDepth; 
	}

private:
	// ����Ƚ�
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
