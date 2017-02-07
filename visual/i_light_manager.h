#pragma once

#include "../visual/i_vis_base.h"

class ILightManager : public IVisBase
{
public:
	// 最多光源数量
	virtual void SetMaxLightNum(int value) = 0;
	virtual int GetMaxLightNum() const = 0;

	// 是否开启主光源
	virtual void SetSunLighting(bool value) = 0;
	virtual bool GetSunLighting() const = 0;

	// 是否开启静态光照时的人物点光源照明
	virtual void SetRolePointLighting(bool value) = 0;
	virtual bool GetRolePointLighting() const = 0;

	// 是否开启光源阴影
	virtual void SetEnableShadow(bool value) = 0;
	virtual bool GetEnableShadow() const = 0;

	// 是否优化阴影图的生成
	virtual void SetShadowMapOptimal(bool value) = 0;
	virtual bool GetShadowMapOptimal() const = 0;

	// 是否使用深度边界测试
	virtual void SetUseDepthBoundTest(bool value) = 0;
	virtual bool GetUseDepthBoundTest() const = 0;

	// 深度斜率
	virtual void SetDepthSlope(float value) = 0;
	virtual float GetDepthSlope() const = 0;

	// 深度偏置
	virtual void SetDepthBias(float value) = 0;
	virtual float GetDepthBias() const = 0;

	// 阴影图过滤级别
	virtual void SetFilterLevel(int value) = 0;
	virtual int GetFilterLevel() const = 0;

	// 创建光源对象
	virtual PERSISTID Create() = 0;
	// 根据配置文件创建光源对象
	virtual PERSISTID CreateFromIni(const PERSISTID& id,const char* ini_name, 
		const char* light_name, const char* helper_name) = 0;
	// 删除光源对象
	virtual bool Delete(const PERSISTID& id) = 0;
	// 返回光源列表
	virtual void GetLightList(const IVarList& args, IVarList& result) = 0;

};