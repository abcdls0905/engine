//--------------------------------------------------------------------
// �ļ���:		sky_box.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��5��26��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SKY_BOX_H
#define _SKY_BOX_H

#include "../public/core_type.h"
#include "../visual/i_texture.h"
#include "../visual/i_vis_base.h"
#include "../visual/shader_list.h"
#include "../utils/array_pod.h"
#include "../utils/pod_hash_map.h"
#include "../visual/i_sky_box.h"
#include "../visual/i_render.h"
#include "../visual/i_context.h"
#include "../visual/i_scene_view.h"

// ��պ�
class TSkyBoxShaderAlloc
{
public:
	TSkyBoxShaderAlloc() {}
	~TSkyBoxShaderAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TSkyBoxShaderAlloc& src) {}
};

class SkyBox: public ISkyBox
{
private:
	enum FLAG_SHIFT_ENUM
	{
		FLAG_CUBEMAP,
		FLAG_FOG,
		FLAG_FADEINTEXTURE,
		FLAG_MAX,
	};

	struct sky_vertex_t 
	{
		FmVec3 pos;
		float tu;
		float tv;
	};

    struct sky_box_filter_shaderhandle
    {
        IShaderProgram* pShader;
        fm_int hFogHeight;
        fm_int hFogColor;
        fm_int hMat4PMV;
        fm_int hTexSide;
        fm_int hTexUp;
    };

	//��Ⱦ����״̬
	struct sky_cb_object_t
	{
		bool bIsEnablefog;//�Ƿ��������
        bool bZEnable;//�Ƿ����д��
		bool bCubeMap;//�Ƿ�Ϊcubeͼ
		float c_FogHeight;//��ĸ߶�
		FmVec4 c_FogColor;//�����ɫ
		FmMat4 c_mat4PMV;
	    ITexture*  pTexSide;//������ͼ
	    ITexture*  pTexUp;//����ͼ
		sky_box_filter_shaderhandle* pShaderHandle;
	    IStaticVB* pVBuffer;
	    IStaticIB* pIBuffer;
	};

	// ��Ⱦ����
	static void RenderSky(void* pdata);

    static void RenderCub_Sky(void* pdata);
public:
	SkyBox();
	virtual ~SkyBox();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual bool Load();
	virtual void Update(float seconds);
	//�����������
	virtual void Realize();

	//�����������
	virtual void RealizeNormal();

    //������յ�Ӱ
	virtual void RealizeReflect();

	// �Ƿ�������
	virtual bool GetLoadFinish(); 

	// λ��
	virtual bool SetPosition(float x, float y, float z);
	virtual FmVec3 GetPosition() const;

	// ����
	virtual bool SetAngle(float x, float y, float z);
	virtual FmVec3 GetAngle() const;

	// �Ƿ�ɼ�
	virtual void SetVisible(bool value);
	virtual bool GetVisible() const;
	
	// �Ƿ��첽������ͼ
	void SetAsyncLoad(bool value);
	bool GetAsyncLoad() const;
	
	// �Ƿ�ʹ����Ȳ���
	void SetZEnable(bool value);
	bool GetZEnable() const;

	// �˷�����
	void SetMulFactor(float value);
	float GetMulFactor() const;

	// Y����ת�ٶ�
	void SetYawSpeed(float value);
	float GetYawSpeed() const;

	// ������ܵ���ͼ
	void SetSideTex(const char* value);
	const char* GetSideTex() const;
	
	// ��ն�����ͼ
	void SetUpTex(const char* value);
	const char* GetUpTex() const;


	/*
	// �������ն�����ͼ
	void SetFadeInUpTex(const char* value);
	const char* GetFadeInUpTex() const;

	// �����������ܵ���ͼ
	void SetFadeInSideTex(const char* value);
	const char* GetFadeInSideTex() const;
	*/

	// �����ʱ��
	void SetFadeInTime(float value);
	float GetFadeInTime() const;

	// �Ƿ񽥱����
	bool GetFadeInFinish() const;

	// ������ͼ
	bool UpdateTexture();

private:
	// ж����Դ
	bool Unload();
	// ������ͼ
	bool LoadTexture();
	// �ͷ���ͼ
	bool UnloadTexture();

	void UpdataRealize(sky_cb_object_t& renderdata);

private:
	IRender*    m_pRender;
	IStaticVB* m_pVBuffer;
	IStaticIB* m_pIBuffer;

	ITexture*  m_pSideTex;
	ITexture*  m_pUpTex;

	bool       m_bNvGPU;//�Ƿ���nv gpu

	/*
	ITexture* m_pFadeInUpTex;
	ITexture* m_pFadeInSideTex;
	*/

	FmVec3 m_vPosition;
	FmVec3 m_vAngle;
	FmMat4 m_matWVP;
	bool m_bVisible;
	bool m_bAsyncLoad;
	bool m_bZEnable;
	bool m_bCubeMap;
	float m_fMulFactor;
	float m_fYawSpeed;
	float m_fFadeInTime;
	float m_fFadeInElapseTime;

	core_string m_strSideTex;
	core_string m_strUpTex;

	/*
	core_string m_strFadeInUpTex;
	core_string m_strFadeInSideTex;
	*/

	FmVec4 m_fogcolor;
	float  m_fogheight;
	bool  m_bIsEnablefog;

	float m_offset_seconds;

	TShaderList<IVertexShader, FLAG_MAX> m_VSList;
	TShaderList<IPixelShader, FLAG_MAX> m_PSList;

	TPodHashMap<unsigned int, sky_box_filter_shaderhandle*, TPodTraits<unsigned int>,TSkyBoxShaderAlloc> m_ShaderMap;

	sky_cb_object_t m_Realize;
    sky_cb_object_t m_RealizeReflect;
};

#endif // _SKY_BOX_H
