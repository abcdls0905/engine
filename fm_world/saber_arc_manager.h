//--------------------------------------------------------------------
// 文件名:		saber_arc_manager.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年5月10日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SABER_ARC_MANAGER_H
#define _SABER_ARC_MANAGER_H

#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "../utils/pod_hash_map.h"
#include "../visual/i_vis_base.h"
#include "../visual/i_render.h"
#include "../visual/shader_list.h"

// 运动拖尾管理器

class SaberArc;

// 刀光
class TSaberArcShaderAlloc
{
public:
    TSaberArcShaderAlloc() {}
    ~TSaberArcShaderAlloc() {}

    // 申请内存
    void* Alloc(size_t size) { return CORE_ALLOC(size); }
    // 释放内存
    void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
    // 交换
    void Swap(TSaberArcShaderAlloc& src) {}
};

struct saber_arc_filter_shaderhandle_t
{
    IShaderProgram* pShader;
    fm_int hMtxWVP;
    fm_int hMtxWorld;
    fm_int hCamPos;      
    fm_int hBladeLength; 
    fm_int hColorStart;  
    fm_int hColorEnd;    
    fm_int hRefIntensity;
    fm_int hFogParam;
    fm_int hFogColor;
    fm_int hFogExpParam;
    fm_int hTex;
};

class SaberArcManager: public IVisBase
{
	IRender* m_pRender;
private:
	struct saber_arc_t
	{
		PERSISTID SaberArcID;
		//float fDistance;
	};

public:
	SaberArcManager();
	virtual ~SaberArcManager();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	// 设置场景上下文
	virtual void SetContext(IRenderContext* value);

	virtual void Update(float seconds);
	//virtual void Prepare(float offset_seconds);
	virtual void Realize();

	// 运动拖尾贴图文件路径
	void SetTexturePath(const char* value);
	const char* GetTexturePath() const;

	// 根据模型信息创建拖尾
	PERSISTID CreateFromModel(const PERSISTID& id, const char* start_label, 
		const char* end_label); 
	// 删除拖尾
	bool Delete(const PERSISTID& id);

	// 根据配置文件创建拖尾
	PERSISTID CreateFromIni(const PERSISTID& id,
		const char* ini_name, const char* arc_name,
		const char* start_label, const char* end_label);

    saber_arc_filter_shaderhandle_t* GetShaderHander(unsigned int flags);

private:
    bool LoadShader();

private:
	TArrayPod<saber_arc_t, 1, TCoreAlloc> m_SaberArcs;
	bool m_bAsyncLoad;
	core_string m_strTexturePath;

    TShaderList<IVertexShader, 5> m_VSList;
    TShaderList<IPixelShader, 5> m_PSList;

    TPodHashMap<unsigned int, saber_arc_filter_shaderhandle_t*, TPodTraits<unsigned int>,TSaberArcShaderAlloc> m_ShaderMap;

    float m_deltaTime;
};

#endif // _SABER_ARC_MANAGER_H
