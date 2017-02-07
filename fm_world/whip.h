//--------------------------------------------------------------------
// 文件名:		whip.h
// 内  容:		
// 说  明:		
// 创建日期:	2014年4月11日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
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

// 鞭子
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

	// 取得拖尾名
	const char* GetName() const;

	// 异步加载
	void SetAsyncLoad(bool value);
	bool GetAsyncLoad() const;

	// 贴图
	void SetTextureName(const char* value);
	const char* GetTextureName() const;

	// 顶点颜色1
	void SetColorOneString(const char* value);
	result_string GetColorOneString() const;

	// 顶点颜色2
	void SetColorTwoString(const char* value);
	result_string GetColorTwoString() const;

    // 顶点颜色1
    void SetColorOne(unsigned int value);
    unsigned int GetColorOne() const;

    // 顶点颜色2
    void SetColorTwo(unsigned int value);
    unsigned int GetColorTwo() const;

	// 进入时间
	void SetInTime(float value);
	float GetInTime() const;

	// 暂停时间
	void SetPauseTime(float value);
	float GetPauseTime() const;

    // 退出时间
    void SetOutTime(float value);
    float GetOutTime() const;

	// 宽度
	void SetBladeWidth(float value);
	float GetBladeWidth() const;

    // 设置切面方向
    void SetLeftDir(const FmVec3& vLeft);
    const FmVec3& GetLeftDir() const;

    // 当前鞭子尾部位置
    const FmVec3& GetTailPosition() const;

	// 相关参数
	void SetParams(const FmVec4& params);
	const FmVec4& GetParams() const;

    // 开始
    void SetStart(bool value);
    bool GetStart() const;

	// 是否高亮
	void SetHighLight(bool value);
	bool GetHighLight() const;

	// 绑定的模型
    PERSISTID GetAttackModelID() const { return m_attackID; }
    PERSISTID GetSufferModelID() const { return m_sufferID; }

    // 是否完成
    void SetComplete() { m_bComplete = true; }
	bool GetComplete() const { return m_bComplete; }
	
	// 保存模型信息
    bool SetModelInfo(
        const PERSISTID& attack_id, const char* attack_label, 
        const PERSISTID& suffer_id, const char* suffer_label);

	// 获得模型信息
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

    PERSISTID m_attackID;   // 小攻
    PERSISTID m_sufferID;   // 小受

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
        float cam_pos[4];       // 摄像机的位置
        float color_start[4];   // 刀光颜色
        float color_end[4];     // 刀光颜色
        float params[4];        // 其他参数

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
