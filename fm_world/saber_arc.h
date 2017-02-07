//--------------------------------------------------------------------
// 文件名:		saber_arc.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年5月10日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SABER_ARC_H
#define _SABER_ARC_H

#include "../visual/i_vis_base.h"
#include "../public/core_type.h"
#include "../visual/i_resource.h"
#include "../visual/i_texture.h"
#include "../visual/i_render.h"

// 运动拖尾
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

	// 取得拖尾名
	const char* GetName() const;

	// 异步加载
	void SetAsyncLoad(bool value);
	bool GetAsyncLoad() const;

	// 贴图
	void SetTextureName(const char* value);
	const char* GetTextureName() const;

	// 折射贴图
	void SetRefractionTextureName(const char* value);
	const char* GetRefractionTextureName() const;

	// 顶点颜色1
	void SetColorOneString(const char* value);
	result_string GetColorOneString() const;

	// 顶点颜色2
	void SetColorTwoString(const char* value);
	result_string GetColorTwoString() const;

	// 折射强度
	void SetRefractionIntensity(float value);
	float GetRefractionIntensity() const;

	// 持续时间
	void SetLifeTime(float value);
	float GetLifeTime() const;

	// 刀光扩散速度
	void SetSpeed(float value);
	float GetSpeed() const;

    // 刀光开始
    void SetStart(bool value);
    bool GetStart() const;

	// 是否高亮
	void SetHighLight(bool value);
	bool GetHighLight() const;

	// 是否保持UV静止
	void SetStaticUV(bool value);
	bool GetStaticUV() const;

	// 绑定的模型
	PERSISTID GetModelID() const { return m_ModelID; }
	// 是否完成
	bool GetComplete() const { return m_bComplete; }
	
	// 保存模型信息
	bool SetModelInfo(const PERSISTID& model_id, const char* start_name, 
		const char* end_name);

	// 获得拖尾所绑定的两个辅助点
	void GetHelperPointName(const IVarList& args, IVarList& result);

    static void ReleaseVB();

    void SetMgr(SaberArcManager* pMgr)
    {
        m_pMgr = pMgr;
    }

    // 目标位置
    void SetTarget(const FmVec3& vTarget)
    {
        m_vTarget = vTarget;
    }

    const FmVec3 GetTarget() const
    {
        return m_vTarget;
    }

private:
	// 初始化矩阵
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
    float m_fOffsetParam;       // 偏移参数 
    FmVec3 m_vOffsetDir;
    float m_fInterpolateDis;    // 插值距离
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
        float cam_pos[4];       // 摄像机的位置
        float blade_length[4];  // 刀光长度
        float color_start[4];   // 刀光颜色
        float color_end[4];     // 刀光颜色
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
