//--------------------------------------------------------------------
// �ļ���:		whip.h
// ��  ��:		
// ˵  ��:		
// ��������:	2014��4��11��
// ������:		
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _WHIP_H
#define _WHIP_H

#include "../visual/i_vis_base.h"
#include "../public/core_type.h"
#include "../visual/i_resource.h"
#include "../visual/i_texture.h"
#include "../visual/i_render.h"
#include "../visual/i_whip_manager.h"
#include "whip_data.h"

// ����
#include <stdio.h>

class IIniFile;
class WhipManager;
struct whip_filter_shaderhandle_t;

class Whip: public IVisBase
{
private:
    static IRender* s_pRender;
    static IDynamicVB* s_pVBuffer;
    static void DrawWhip(void* data);

public:
	Whip();
	virtual ~Whip();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

    virtual void Update(float seconds);
    virtual void Realize();

	virtual bool Load();
	
	bool LoadIni(IIniFile* ini, const char* psname, const char* tex_path);

	virtual bool GetLoadFinished();

	virtual void SetVisible(bool value);
	virtual bool GetVisible() const;

	// ȡ����β��
	const char* GetName() const;

	// �첽����
	void SetAsyncLoad(bool value);
	bool GetAsyncLoad() const;

	// ��ͼ
	void SetTextureName(const char* value);
	const char* GetTextureName() const;

	// ������ɫ1
	void SetColorOneString(const char* value);
	result_string GetColorOneString() const;

	// ������ɫ2
	void SetColorTwoString(const char* value);
	result_string GetColorTwoString() const;

    // ������ɫ1
    void SetColorOne(unsigned int value);
    unsigned int GetColorOne() const;

    // ������ɫ2
    void SetColorTwo(unsigned int value);
    unsigned int GetColorTwo() const;

	// ����ʱ��
	void SetInTime(float value);
	float GetInTime() const;

	// ��ͣʱ��
	void SetPauseTime(float value);
	float GetPauseTime() const;

    // �˳�ʱ��
    void SetOutTime(float value);
    float GetOutTime() const;

	// ���
	void SetBladeWidth(float value);
	float GetBladeWidth() const;

    // �������淽��
    void SetLeftDir(const FmVec3& vLeft);
    const FmVec3& GetLeftDir() const;

    // ��ǰ����β��λ��
    const FmVec3& GetTailPosition() const;

	// ��ز���
	void SetParams(const FmVec4& params);
	const FmVec4& GetParams() const;

    // ��ʼ
    void SetStart(bool value);
    bool GetStart() const;

	// �Ƿ����
	void SetHighLight(bool value);
	bool GetHighLight() const;

	// �󶨵�ģ��
    PERSISTID GetAttackModelID() const { return m_attackID; }
    PERSISTID GetSufferModelID() const { return m_sufferID; }

    // �Ƿ����
    void SetComplete() { m_bComplete = true; }
	bool GetComplete() const { return m_bComplete; }
	
	// ����ģ����Ϣ
    bool SetModelInfo(
        const PERSISTID& attack_id, const char* attack_label, 
        const PERSISTID& suffer_id, const char* suffer_label);

	// ���ģ����Ϣ
	void GetModelInfo(IVarList& result);

    static void ReleaseVB();

    void SetMgr(WhipManager* pMgr)
    {
        m_pMgr = pMgr;
    }
	
    void BuildData(WhipType type, unsigned int slices);

private:
    bool CalculateAttackPosition();
    bool CalculateSufferPosition();

private:
    WhipManager* m_pMgr;
    WhipData* m_pData;
    WhipType m_type;

	core_string m_strName;
	core_string m_strTexture;

    PERSISTID m_attackID;   // С��
    PERSISTID m_sufferID;   // С��

	ITexture* m_pTex;

	bool m_bVisible;
    bool m_bReady;
	bool m_bAsyncLoad;
	bool m_bComplete;
	bool m_bStart;
	bool m_bHighLight;

	core_string m_strAttackLabel;
	core_string m_strSufferLabel;

    unsigned int m_nColorOne;
    unsigned int m_nColorTwo;

    struct render_states
    {
        FmMat4 mtxWVP;
        FmMat4 mtxWorld;
        float cam_pos[4];       // �������λ��
        float color_start[4];   // ������ɫ
        float color_end[4];     // ������ɫ
        float params[4];        // ��������

        float fog_param[4];
        float fog_color[4];
        float fog_exp_param[4];

        unsigned int vertices_count;
        void* vertices_buffer;

        bool bHighLight;
        int destBlendFunc;
    };

    struct whip_render_batch
    {
        whip_filter_shaderhandle_t* pShaderHandle;
        render_states renderStates;
        ITexture* pTexture;

        whip_render_batch()
        {
            pShaderHandle = NULL;
            pTexture = NULL;
        }
    };
    whip_render_batch m_shaderHandle;
};

#endif // _WHIP_H
