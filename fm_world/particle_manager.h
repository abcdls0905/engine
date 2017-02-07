//--------------------------------------------------------------------
// 文件名:		particle_manager.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年9月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PARTICLE_MANAGER_H
#define _PARTICLE_MANAGER_H

#include "../public/core_type.h"
#include "../visual/i_vis_base.h"
#include "../visual/shader_list.h"
#include "../utils/array_pod.h"
#include "../utils/tick_timer.h"
#include "../utils/pod_hash_map.h"  
#include "../visual/i_particle_manager.h"
// 粒子管理器

class CParticleData;
class CParticleNode;
class CEffectData;
class IRenderContext;

class TParticleShaderAlloc
{
public:
	TParticleShaderAlloc() {}
	~TParticleShaderAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TParticleShaderAlloc& src) {}
};



class ParticleManager: public IParticleManager
{
public:
	enum FLAG_VSH_PARTICLE
	{
		PAR_DEPTHMAP,
		PAR_BILLBOARD_Y,
		PAR_BILLBOARD_NONE,
		PAR_HIGHLIGHT,
		PAR_FOGUSE,
		PAR_FOGLINEAR,
		PAR_FOGEXP,
		PAR_GPU,
		PAR_MAX,
	};

	struct particle_shader_handle
	{
		IShaderProgram*	 pShader;
		fm_int			tex_BaseMapHandle;
		fm_int			mat4ViewProj;
		fm_int			vec4ParticleColorHandle;
		fm_int			vec4FogColorHandle;
		fm_int			vec4FogParamHandle;
		fm_int			vec4FogExpParamHandle;
		fm_int			vec3ViewPostion;
		fm_int			vec3EmitterPos;
		fm_int			vec4RotateAxis;
		fm_int			vec4Time;
		fm_int			vec4Speed;
		fm_int			vec4Force;
		fm_int			vec4UV_Bias;
		fm_int			vec4TrackAlpha;
	};


	ParticleManager();
	virtual ~ParticleManager();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	// 设置场景上下文
	virtual void SetContext(IRenderContext* value);

	virtual void Update(float seconds);
	virtual void Realize();

	// 是否可见
	virtual void SetVisible(bool value);
	virtual bool GetVisible() const;

	// 粒子节点总数
	int GetParticleNodeCount() const;
	// 粒子数据总数
	int GetParticleDataCount() const { return (int)m_Datas.size(); }
	// 特效数据总数
	int GetEffectDataCount() const { return(int) m_EffectDatas.size(); }

	// 粒子贴图文件路径
	void SetTexturePath(const char* value);
	const char* GetTexturePath() const;

	// 是否异步加载粒子贴图
	void SetAsyncLoad(bool value) { m_bAsyncLoad = value; }
	bool GetAsyncLoad() const { return m_bAsyncLoad; }

	// LOD起始距离
	void SetLodBegin(float value) { m_fLodBegin = value; }
	float GetLodBegin() const { return m_fLodBegin; }

	// LOD结束距离
	void SetLodEnd(float value) { m_fLodEnd = value; }
	float GetLodEnd() const { return m_fLodEnd; }

	ParticleManager::particle_shader_handle* GetShaderHandle(size_t shader_flag, bool bnIsGpu=false);
	// 获得粒子着色器的索引值
	IVertexShader* GetParticleVS(size_t shader_flag);
	IPixelShader* GetParticlePS(size_t shader_flag);
	// 获得粒子折射着色器的索引值
	IVertexShader* GetDistortionVS(size_t shader_flag);
	IPixelShader* GetDistortionPS(size_t shader_flag);


	// 重新加载所有配置文件信息
	bool ReloadIni();

	// 创建粒子参数对象
	PERSISTID CreateInfo();
	// 根据配置文件创建粒子参数对象
	PERSISTID CreateInfoFromIni(const char* ini_name, const char* psname);
	
	// 根据粒子参数创建粒子对象
	PERSISTID Create(const PERSISTID& info_id);
	// 根据配置文件创建粒子对象
	PERSISTID CreateFromIni(const char* ini_name, const char* psname);
	// 删除粒子对象
	bool Delete(const PERSISTID& id);

	// 暂时将特效数据的创建等由粒子管理器托管-S-
	// 获得特效参数
	CEffectData* CreateEffectData(const char* ini_name, const char* em_name);

	// 重新加载所有特效配置文件信息
	bool ReloadEffectIni();

	// 保留缓冲数据的时间
	void SetCacheTime(float value) { m_fCacheTime = value; }
	float GetCacheTime() const { return m_fCacheTime; }
	// 暂时将特效数据的创建等由粒子管理器托管-E-

private:
	// 查找粒子参数
	bool FindDataIndex(const char* ini_name, const char* ps_name, 
		size_t& index);
	// 获得粒子参数
	CParticleData* CreateData(const char* ini_name, const char* ps_name);
	// 创建粒子节点
	CParticleNode* CreateParticle(CParticleData* pData);
	// 删除粒子节点
	void DeleteParticle(CParticleNode* pNode);

	// 暂时将特效数据的创建等由粒子管理器托管-S-
	// 查找特效参数
	bool FindEffectDataIndex(const char* ini_name, const char* em_name, 
		size_t& index);

	// 定时检查卸载
	bool CheckUnload();
	// 暂时将特效数据的创建等由粒子管理器托管-E-
private:
	TArrayPod<CParticleData*, 1, TCoreAlloc> m_Datas;
	CParticleNode* m_pFirst;
	CParticleNode* m_pLast;
	IRender*         m_pRender;
	core_string m_strTexturePath;
	bool m_bVisible;
	bool m_bAsyncLoad;
	float m_fLodBegin;
	float m_fLodEnd;
	float m_fWindAngle;
	float m_fWindSpeed;
	float m_fWindPower;
	FmVec3 m_vWindDirection;
	float  m_offset_seconds;

	TShaderList<IVertexShader, PAR_MAX> m_VSList;
	TShaderList<IPixelShader, PAR_MAX> m_PSList;

	TShaderList<IVertexShader, 5> m_DistortionVSList;
	TShaderList<IPixelShader, 5> m_DistortionPSList;

	// 暂时将特效数据的创建等由粒子管理器托管 -S-
	TArrayPod<CEffectData*, 10, TCoreAlloc> m_EffectDatas;
	CTickTimer m_UnloadTimer;
	float m_fCacheTime;
	// 暂时将特效数据的创建等由粒子管理器托管 -E-

	TPodHashMap<unsigned int, ParticleManager::particle_shader_handle*, TPodTraits<unsigned int>, TParticleShaderAlloc> m_ShaderMapHandle;
};

#endif // _PARTICLE_MANAGER_H
