//--------------------------------------------------------------------
// �ļ���:		dynamic_weather.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��6��25��
// ������:		 
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _DYNAMICWEATHER_H
#define _DYNAMICWEATHER_H

#include "../public/module.h"
#include "../public/core_type.h"


#include "../visual/i_render.h"

#include "../visual/i_context.h"
#include "../visual/i_vis_base.h"

#include "../visual/shader_list.h"
#include "../utils/array_pod.h"
#include "../utils/pod_hash_map.h"
/*
��̬����Ч��
*/

class TDynamicWeatherShaderAlloc
{
public:
	TDynamicWeatherShaderAlloc() {}
	~TDynamicWeatherShaderAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TDynamicWeatherShaderAlloc& src) {}
};
 

class DynamicWeather: public IVisBase
{
public:
	struct weather_parameters_t
	{
		weather_parameters_t(): fDensity(5.0f), fSpeed(8.0f), fLength(0.5f), 
			fWidth(0.05f), fAlpha(0.1f), fWind(0.0f), 
			fAlphaFadeSpeedBegin(10.0f), fAlphaFadeSpeedEnd(40.0f) 
		{
			//HeavySnow();
		  //HeavyRain();
			LightRain();
		}
		~weather_parameters_t() {};

		float fDensity;//�ܶ�
		float fSpeed;//�ٶ�
		float fLength;//���ӵĳ���
		float fWidth;//���ӵĿ��
		float fAlpha;//
		float fWind;//����
		float fAlphaFadeSpeedBegin;//alpha�����ٶ�
		float fAlphaFadeSpeedEnd;// alpha�����ٶ�

	public:
		//С��
		void LightRain()
		{
			fDensity = 3.0f;
			fSpeed = 4.5f;
			fLength = 0.15f;
			fWidth = 0.02f;
			fAlpha = 0.10f;
			fWind = 1.0f;
		}
		
		//����
		void HeavyRain()
		{
			fDensity = 10.0f;
			fSpeed = 8.0f;
			fLength = 0.25f;
			fWidth = 0.01f;
			fAlpha = 0.15f;
			fWind = 6.0f;
		}

		//Сѩ
		void LightSnow()
		{
			fDensity = 5.0f;
			fSpeed = 1.5f;
			fLength = 0.03f;
			fWidth = 0.03f;
			fAlpha = 0.3f;
			fWind = 1.0f;
		}

		//��ѩ
		void HeavySnow()
		{
			fDensity = 15.0f;
			fSpeed = 1.5f;
			fLength = 0.04f;
			fWidth = 0.04f;
			fAlpha = 0.3f;
			fWind = 5.0f;
		}

	};

	struct weather_occlusion_box_t
	{
		FmVec3 vMin;
		FmVec3 vMax;
		FmMat4 mTransform;
		FmMat4 mWorldToUnitCube;
		FmMat4 mUnitCubeToWorld;
	};

	enum 
	{
        MAX_RANDER_PASS_NUM = 20,
	};
public:
	struct weather_vertex_t 
	{
		FmVec3 position;
		FmVec2 uv;
	};

	struct dynamic_weather_shaderhandle_t
	{
		IShaderProgram* pShader;
		fm_int mat4ViewProjHandle;
        fm_int mat4ViewProjPrevHandle;
		fm_int vec4PositionOffsetHandle;
		fm_int vec4VelocityHandle;
		fm_int vec4Alpha;
        fm_int vec4ViewPositionHandle;
		fm_int vec4SizeScaleHandle;
		fm_int vec4LightingHandle;
		fm_int vec4ForwardHandle;
		fm_int mat4OcclusionMatrixHandle;
        fm_int tex_ColorHandle;
		fm_int tex_OcclusionHeightMapHandle;
	};

	struct dynamic_weather_cb_t
	{
		DynamicWeather::dynamic_weather_shaderhandle_t* pShaderCB;//����ǼĴ�������
		bool             bUseTex;
		bool             bOcclusion;
		IStaticVB*       pVBuffer;
        IStaticIB*       pIBuffer;
		ITexture*        pTexColor;
		IDynamicTex*     pTexOcclusion;
        IStaticTex*      pTexWhite;
		FmMat4 mat4ViewProj;
		FmMat4 mat4ViewProjPrev;
		FmVec4 vec4PositionOffset;
		FmVec4 vec4Velocity;
		FmVec4 vec4Alpha;
		FmVec4 vec4ViewPosition;
		FmVec4 vec4SizeScale;
		FmVec4 vec4Lighting;
		FmVec4 vec4Forward;
		FmMat4 mat4OcclusionMatrix;      
	};

	enum FLAG_SHIFT_ENUM
	{
        FLAG_NONE,
		FLAG_OCCLUSION,
		FLAG_MAX,
	};
public:
	DynamicWeather();
	virtual ~DynamicWeather();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual bool Load();

	virtual void Update(float seconds);
	virtual void Realize();

	// �Ƿ�������
	virtual bool GetLoadFinish(); 

	// ���û�������ɫ
	void SetLightColor(FmVec4 vLighting);

	FmVec4 GetLightColor() const;

	// ʹ��ʹ������
	void SetUseTexture(bool bUseTexture);

	bool GetUseTexture() const;
 

	// �Ƿ�ɼ�
	virtual void SetVisible(bool value);
	virtual bool GetVisible() const;

	// �Ƿ��첽������ͼ
	void SetAsyncLoad(bool value);
	bool GetAsyncLoad() const;

	// �������ӵ���ͼ
	void SetParticleNodeTex(const char* value);
	const char* GetParticleNodeTex() const;

	// ������ͼ
	bool UpdateTexture();

    // ������ײ��Χ�д�С
	void SetOcclusionBox(const FmVec3& vMin, const FmVec3& vMax, 
		const FmMat4& mTransform);
    const weather_occlusion_box_t& GetOcclusionBox() const { return m_OcclusionBox; }

    // ������ϸ����
	void SetDensity(float value);
	float GetDensity() const;

	void SetSpeed(float value);
	float GetSpeed() const;

	void SetLength(float value);
	float GetLength() const;

	void SetWidth(float value);
	float GetWidth() const;

	void SetAlpha(float value);
	float GetAlpha() const;

	void SetWind(float value);
	float GetWind() const;

    void SetAlphaFadeSpeedBegin(float value);
	float GetAlphaFadeSpeedBegin() const;

    void SetAlphaFadeSpeedEnd(float value);
	float GetAlphaFadeSpeedEnd() const;

	void SetOcclusion(bool value);
	bool GetOcclusion() const;

private:
	// ж����Դ
	bool Unload();
	// ��ʼ����������
	void InitVertex();
	// ������ͼ
	bool LoadTexture();
	// �ͷ���ͼ
	bool UnloadTexture();
	// ����Shader
	bool LoadShader();

	void CalculateOcclusionMatrix();

	bool Render();
	// set pass param
	void SetPassConstants(int iPass, float fAlpha,FmVec3 vForward,FmVec3 vCameraPosition);

	// ��ȡ��ǰshader
	DynamicWeather::dynamic_weather_shaderhandle_t* GetShaderHandle();

	static void RenderWeatherCallBack(void* pdata);
private:
	IRender* m_pRender;
	IStaticVB*        m_pVBuffer;
	IStaticIB*        m_pIBuffer;
	int                 m_nParticles;
	int                 m_nVertices;
	int                 m_nIndices;
    // occlusion box
    weather_occlusion_box_t m_OcclusionBox;
	// rendering paramerters
    weather_parameters_t m_Parameters;

	float			  	m_fParticleBox;//��������Ӱ�Χ�еĴ�С ���е����Ӷ������Χ������
	float				m_fTime;

	ITexture*         m_pTexture;//���������ײͼ
    IStaticTex*       m_pWhiteTexture;

    bool                m_bAsyncLoad;
    bool                m_bVisible;

	bool                m_bUseTexture;

	// for perlin noise wind function
	int                 m_iNoiseX;
	int                 m_iNoiseY;
	int                 m_iNoiseZ;

    FmVec4         m_vLighting;

    bool                m_bUseOcclusion;//ʹ��������ײͼ

	// number of passes and per pass parameters
	int                 m_nPasses;
 	float               m_fSpeedScale[MAX_RANDER_PASS_NUM];
	float               m_fSpeedColourScale;
	FmVec3         m_vWind[MAX_RANDER_PASS_NUM];
	FmVec3         m_vOffset[MAX_RANDER_PASS_NUM];
	FmVec3         m_vRandom[MAX_RANDER_PASS_NUM];  

	dynamic_weather_cb_t m_dynamic_weather_cb[MAX_RANDER_PASS_NUM];

	float               m_fAlphaFade;

 	TShaderList<IVertexShader,FLAG_MAX>      m_VSList;
	TShaderList<IPixelShader,FLAG_MAX>       m_PSList;

	TFastStr<char, 16, TCharTraits<char>, TCoreAlloc> m_strTexFilePath;
	TPodHashMap<unsigned int, DynamicWeather::dynamic_weather_shaderhandle_t*, TPodTraits<unsigned int>, TDynamicWeatherShaderAlloc> m_ShaderMapHandle;
};

#endif
 