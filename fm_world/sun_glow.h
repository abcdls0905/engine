//--------------------------------------------------------------------
// �ļ���:		sun_glow.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��5��26��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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
// ̫������

class SunGlow: public ISunGlow
{
private:
	// ����������
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

	// �Ƿ�������
	virtual bool GetLoadFinish(); 

	// ����ĳߴ�
	void SetGlowSize(float value);
	float GetGlowSize() const;

	// ���εĳߴ�
	void SetFlareSize(float value);
	float GetFlareSize() const;
	
	// ����ľ���
	void SetGlowDistance(float value);
	float GetGlowDistance() const;
	
	// ���εľ���
	void SetFlareDistance(float value);
	float GetFlareDistance() const;

	// ������ת�ٶ�
	void SetGlowRotateSpeed(float value);
	float GetGlowRotateSpeed() const;
	
	// �Ƿ���ʾ����
	void SetShowGlow(bool value);
	bool GetShowGlow() const;
	
	// �Ƿ���ʾ����
	void SetShowFlare(bool value);
	bool GetShowFlare() const;

	// �Ƿ��첽����
	void SetAsyncLoad(bool value) { m_bAsyncLoad = value; }
	bool GetAsyncLoad() const { return m_bAsyncLoad; }
	
	// ̫������ͼ
	void SetGlowTex(const char* value);
	const char* GetGlowTex() const;

	// ���ε���ͼ
	void SetFlareTex(const char* value);
	const char* GetFlareTex() const;

	// ������ͼ
	bool UpdateTexture();

	// ��ӹ�������
	bool AddFlare(float distance, float size, int sub_tex, const char* color);
	// ��չ�������
	bool ClearFlare();

private:
	// ������ͼ
	bool LoadTexture();
	// �ͷ���ͼ
	bool UnloadTexture();
	// ��̫��
	bool DrawSun(sun_flare_t* flares, size_t flare_num, size_t sub_tex_x, 
		size_t sub_tex_y, ITexture* pTexture);
	// ������
	bool DrawFlares(sun_flare_t* flares, size_t flare_num, size_t sub_tex_x, 
		size_t sub_tex_y, ITexture* pTexture, IDynamicVB* pVBuffer);


	SunGlow::sunglow_vertex_t* LockVBuffer();

	void UnLockVBuffer();

	SunGlow::sunglow_vertex_t* LockSunVBuffer();

	void UnLockSunVBuffer();

	// ����̫��
	static void RenderSun(void* pdata);
	// ���ƹ��
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
