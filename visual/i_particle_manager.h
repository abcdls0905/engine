#ifndef _I_PARTICLE_MANAGER_H
#define _I_PARTICLE_MANAGER_H

#include "../visual/i_vis_base.h"

class IParticleManager: public IVisBase
{
public:
	// 粒子节点总数
	virtual int GetParticleNodeCount() const = 0;
	// 粒子数据总数
	virtual int GetParticleDataCount() const  = 0;
	// 特效数据总数
	virtual int GetEffectDataCount() const  = 0;

	// 粒子贴图文件路径
	virtual void SetTexturePath(const char* value) = 0;
	virtual const char* GetTexturePath() const = 0;

	// 是否异步加载粒子贴图
	virtual void SetAsyncLoad(bool value)  = 0;
	virtual bool GetAsyncLoad() const  = 0;

	// LOD起始距离
	virtual void SetLodBegin(float value)  = 0;
	virtual float GetLodBegin() const  = 0;

	// LOD结束距离
	virtual void SetLodEnd(float value)  = 0;
	virtual float GetLodEnd() const  = 0;

	// 重新加载所有配置文件信息
	virtual bool ReloadIni() = 0;

	// 创建粒子参数对象
	virtual PERSISTID CreateInfo() = 0;
	// 根据配置文件创建粒子参数对象
	virtual PERSISTID CreateInfoFromIni(const char* ini_name, const char* psname) = 0;
	
	// 根据粒子参数创建粒子对象
	virtual PERSISTID Create(const PERSISTID& info_id) = 0;
	// 根据配置文件创建粒子对象
	virtual PERSISTID CreateFromIni(const char* ini_name, const char* psname) = 0;
	// 删除粒子对象
	virtual bool Delete(const PERSISTID& id) = 0;

	// 重新加载所有特效配置文件信息
	virtual bool ReloadEffectIni() = 0;

	// 保留缓冲数据的时间
	virtual void SetCacheTime(float value) = 0;
	virtual float GetCacheTime() const = 0;
};
#endif