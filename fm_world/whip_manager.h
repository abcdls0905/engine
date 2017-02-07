//--------------------------------------------------------------------
// 文件名:		whip_manager.h
// 内  容:		
// 说  明:		
// 创建日期:	2014年4月11日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _WHIP_MANAGER_H
#define _WHIP_MANAGER_H

#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "../utils/pod_hash_map.h"
#include "../visual/i_vis_base.h"
#include "../visual/i_render.h"
#include "../visual/shader_list.h"
#include "../visual/i_whip_manager.h"

// 鞭子管理器

#define MAX_WHIP_COUNT 32

class Whip;

class TWhipShaderAlloc
{
public:
    TWhipShaderAlloc() {}
    ~TWhipShaderAlloc() {}

    // 申请内存
    void* Alloc(size_t size) { return CORE_ALLOC(size); }
    // 释放内存
    void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
    // 交换
    void Swap(TWhipShaderAlloc& src) {}
};

struct whip_filter_shaderhandle_t
{
    IShaderProgram* pShader;
    fm_int hMtxWVP;
    fm_int hMtxWorld; 
    fm_int hEyePos; 
    fm_int hColorStart;  
    fm_int hColorEnd;    
    fm_int hParams;

    fm_int hFogParam;
    fm_int hFogColor;
    fm_int hFogExpParam;
    fm_int hTex;
};

class WhipManager: public IWhipManager
{
public:
	WhipManager();
	virtual ~WhipManager();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	// 设置场景上下文
	virtual void SetContext(IRenderContext* value);

	virtual void Realize();

	// 运动贴图文件路径
	void SetTexturePath(const char* value);
	const char* GetTexturePath() const;

	// 根据模型信息创建拖尾
	PERSISTID CreateFromModel(
        WhipType type, 
        const FmVec3& vLeft,
        const char* szTexName,      // 贴图
        float inTime,               // 进入时间
        float pauseTime,            // 暂停时间
        float outTime,              // 退出时间
        unsigned int colorStart,    // 起始颜色
        unsigned int colorEnd,      // 结束颜色
        bool highlight,             // 是否高亮
        float bladeWidth,           // 宽度
        FmVec4 params,              // 其他参数
        const PERSISTID& attack_id, const char* attack_label, 
        const PERSISTID& suffer_id, const char* suffer_label); 

	// 根据配置文件创建拖尾
    PERSISTID CreateFromIni(const FmVec3& vLeft,
        const PERSISTID& attack_id, const char* attack_label, 
        const PERSISTID& suffer_id, const char* suffer_label,
		const char* ini_name, const char* arc_name);

	// 删除拖尾
	bool Delete(const PERSISTID& id);

    whip_filter_shaderhandle_t* GetShaderHander(unsigned int flags);

private:
    bool LoadShader();

    int GetEmptyWhip();

private:
    IRender* m_pRender;
	PERSISTID m_Whips[MAX_WHIP_COUNT];
	bool m_bAsyncLoad;
	core_string m_strTexturePath;

    TShaderList<IVertexShader, 4> m_VSList;
    TShaderList<IPixelShader, 4> m_PSList;

    TPodHashMap<unsigned int, whip_filter_shaderhandle_t*, TPodTraits<unsigned int>,TWhipShaderAlloc> m_ShaderMap;
};

#endif // _WHIP_MANAGER_H
