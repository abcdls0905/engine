//--------------------------------------------------------------------
// �ļ���:		saber_arc.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��5��10��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SABER_ARC_H
#define _SABER_ARC_H

#include "../visual/i_vis_base.h"
#include "../public/core_type.h"
#include "../visual/i_resource.h"
#include "../visual/i_texture.h"
#include "../visual/i_render.h"

// �˶���β
#include <stdio.h>

class IIniFile;
class SaberArcManager;
struct saber_arc_filter_shaderhandle_t;

class SaberArc: public IVisBase
{
    static IRender* s_pRender;
    static IDynamicVB* s_pVBuffer;
    static void DrawSaberArc(void* data);

private:
	enum { WAY_LENGTH = 128 };

    struct arc_vertex_t
	{
		FmVec3 pos;
		FmVec3 tangent;
		FmVec3 binormal;
		float tu;
		float tv;
	};
	
public:
	SaberArc();
	virtual ~SaberArc();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual bool Load();
	
	bool LoadIni(IIniFile* ini, const char* psname, const char* tex_path);

	virtual void Update(float seconds);
	virtual void Realize();

	virtual bool GetLoadFinished();

	virtual void SetVisible(bool value);
	virtual bool GetVisible() const;

    void SetSleep(bool value);
    bool GetSleep() const;

	// ȡ����β��
	const char* GetName() const;

	// �첽����
	void SetAsyncLoad(bool value);
	bool GetAsyncLoad() const;

	// ��ͼ
	void SetTextureName(const char* value);
	const char* GetTextureName() const;

	// ������ͼ
	void SetRefractionTextureName(const char* value);
	const char* GetRefractionTextureName() const;

	// ������ɫ1
	void SetColorOneString(const char* value);
	result_string GetColorOneString() const;

	// ������ɫ2
	void SetColorTwoString(const char* value);
	result_string GetColorTwoString() const;

	// ����ǿ��
	void SetRefractionIntensity(float value);
	float GetRefractionIntensity() const;

	// ����ʱ��
	void SetLifeTime(float value);
	float GetLifeTime() const;

	// ������ɢ�ٶ�
	void SetSpeed(float value);
	float GetSpeed() const;

    // ���⿪ʼ
    void SetStart(bool value);
    bool GetStart() const;

	// �Ƿ����
	void SetHighLight(bool value);
	bool GetHighLight() const;

	// �Ƿ񱣳�UV��ֹ
	void SetStaticUV(bool value);
	bool GetStaticUV() const;

	// �󶨵�ģ��
	PERSISTID GetModelID() const { return m_ModelID; }
	// �Ƿ����
	bool GetComplete() const { return m_bComplete; }
	
	// ����ģ����Ϣ
	bool SetModelInfo(const PERSISTID& model_id, const char* start_name, 
		const char* end_name);

	// �����β���󶨵�����������
	void GetHelperPointName(const IVarList& args, IVarList& result);

    static void ReleaseVB();

    void SetMgr(SaberArcManager* pMgr)
    {
        m_pMgr = pMgr;
    }

    // Ŀ��λ��
    void SetTarget(const FmVec3& vTarget)
    {
        m_vTarget = vTarget;
    }

    const FmVec3 GetTarget() const
    {
        return m_vTarget;
    }

private:
	// ��ʼ������
	bool InitMatrix();

private:
    SaberArcManager* m_pMgr;
	FmVec3 m_vPosition;
    FmVec3 m_vTarget;
	core_string m_strName;
	core_string m_strTexture;
	core_string m_strRefractionTex;
	PERSISTID m_ModelID;
	ITexture* m_pTex;
	ITexture* m_pRefractionTex;
	bool m_bVisible;
    bool m_bSleep;
	bool m_bInitialized;
	bool m_bAsyncLoad;
	bool m_bComplete;
	bool m_bStart;
	bool m_bHighLight;
	bool m_bOnePoint;
    bool m_bStaticUV;
    bool m_bVBReady;
	unsigned int m_nColorOne;
	unsigned int m_nColorTwo;
	core_string m_strBladeStart;
	core_string m_strBladeEnd;
	float m_fBladeLength;
	float m_fLifeTime;
	float m_fSpeed;
    float m_fOffsetParam;       // ƫ�Ʋ��� 
    FmVec3 m_vOffsetDir;
    float m_fInterpolateDis;    // ��ֵ����
	float m_fRefractionIntensity;
    float m_fVertexTime[WAY_LENGTH];
	FmVec3 m_ControlPoints[WAY_LENGTH];
    FmVec3 m_ControlPointsDir[WAY_LENGTH];
    float m_PointsOffset[WAY_LENGTH];
	arc_vertex_t m_verticesBuffer[WAY_LENGTH * 2];
    unsigned int m_verticesCount;
	int m_nPointsNumber;

    struct render_states
    {
        FmMat4 mtxWVP;
        FmMat4 mtxWorld;
        float cam_pos[4];       // �������λ��
        float blade_length[4];  // ���ⳤ��
        float color_start[4];   // ������ɫ
        float color_end[4];     // ������ɫ
        float ref_intensity[4];
        float fog_param[4];
        float fog_color[4];
        float fog_exp_param[4];
        unsigned int vertices_count;
        void* vertices_buffer;
        bool* vb_ready_ptr;
        bool bHighLight;
        int destBlendFunc;
    };

    struct saber_arc_render_batch
    {
        saber_arc_filter_shaderhandle_t* pShaderHandle;
        render_states renderStates;
        ITexture* pTexture;

        saber_arc_render_batch()
        {
            pShaderHandle = NULL;
            pTexture = NULL;
        }
    };
    saber_arc_render_batch m_shaderHandle;
    saber_arc_render_batch m_refShaderHandle;
};

#endif // _SABER_ARC_H
