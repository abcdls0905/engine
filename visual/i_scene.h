#ifndef I_SCENE_H
#define I_SCENE_H

#include "../public/module.h"
#include "../visual/i_resource.h"

class IRenderContext;
class IWeather;
class ICamera;
class ISceneView;
class IScene: public IEntity
{
public:
	enum  SCENETYPE{
		SCENETYPE_DEFAULT,
		SCENETYPE_GUI,
	};
	// 渲染器参数
	virtual IRenderContext* GetContext() const = 0;
	
	// 获得天气对象
	virtual PERSISTID GetWeatherID() = 0;
	
	virtual IWeather* GetWeather() const = 0;
	
	// 获得摄像机对象
	virtual PERSISTID GetCameraID() = 0;
	
	virtual ICamera* GetCamera() const = 0;

	// 远剪裁面的距离
	virtual void SetFarClipDistance(float value) = 0;
	virtual float GetFarClipDistance() const = 0;

	// 半透明物体降级显示
	virtual void SetBlendDownLevel(int value) = 0;
	virtual int GetBlendDownLevel() const = 0;

	// 是否要渲染折射图
	virtual void SetEnableRealizeRefraction(bool value) = 0;
	virtual bool GetEnableRealizeRefraction() const = 0;

	// 是否要开启临时渲染纹理缓冲
	virtual void SetEnableRealizeTempColorRT(bool value) = 0;
	virtual bool GetEnableRealizeTempColorRT() const = 0;

	// 是否需要开启临时渲染深度纹理
	virtual void SetEnableRealizeTempDepthRT(bool value) = 0;
	virtual bool GetEnableRealizeTempDepthRT() const = 0;


	// 是否要开启拷贝临时渲染纹理缓冲
	virtual void SetEnableRealizeCopyTempColorRT(bool value) = 0;
	virtual bool GetEnableRealizeCopyTempColorRT() const = 0;

	// 是否需要开拷贝启临时渲染深度纹理
	virtual void SetEnableRealizeCopyTempDepthRT(bool value) = 0;
	virtual bool GetEnableRealizeCopyTempDepthRT() const = 0;

	// 骨骼动画模型是否开启半球环境光
	virtual void SetSkinSphereAmbient(bool value) = 0;
	virtual bool GetSkinSphereAmbient() const = 0;

	// 普通模型是否开启半球环境光
	virtual void SetModelSphereAmbient(bool value) = 0;
	virtual bool GetModelSphereAmbient() const = 0;

	// 全局环境贴图
	virtual void SetGlobalEnvMap(const char* name) = 0;
	virtual const char* GetGlobalEnvMap() const = 0;

	// 环境反射的乘法系数
	virtual void SetReflectFactor(float value) = 0;
	virtual float GetReflectFactor() const = 0;

	// 人物光照调节系数
	virtual void SetRoleDiffuseFactor(float value) = 0;
	virtual float GetRoleDiffuseFactor() const = 0;
	virtual void SetRoleAmbientFactor(float value) = 0;
	virtual float GetRoleAmbientFactor() const = 0;

	// 人物轮廓光参数
	virtual void SetEnableRoleFallOff(bool value) = 0;
	virtual bool GetEnableRoleFallOff() const = 0;
	virtual void SetRoleFallOffColorString(const char* value) = 0;
	virtual result_string GetRoleFallOffColorString() const = 0;
	virtual void SetRoleFallOffInten(float value) = 0;
	virtual float GetRoleFallOffInten() const = 0;
	virtual void SetRoleFallOffPower(float value) = 0;
	virtual float GetRoleFallOffPower() const = 0;

	// 创建可视对象
	virtual PERSISTID Create(const char* name) = 0;
	// 删除可视对象
	virtual bool Delete(const PERSISTID& id) = 0;

	// 场景唯一实体
	virtual bool SetUnique(const PERSISTID& obj) = 0;
	virtual PERSISTID GetUnique(const char* entity_name) = 0;

	// 添加可视对象
	virtual bool AddObject(const PERSISTID& obj, int priority) = 0;
	// 删除可视对象
	virtual bool RemoveObject(const PERSISTID& obj) = 0;
	// 按优先级删除对象
	virtual bool RemoveObjectByPrior(int priority) = 0;

	// 添加对象到层
	virtual bool LayerAdd(const char* name, const PERSISTID& id) = 0;
	// 从层移除对象
	virtual bool LayerRemove(const char* name, const PERSISTID& id) = 0;

	// 是否要开启动态阴影
	virtual void SetEnableDynamicShadow(bool value) = 0;
	virtual bool GetEnableDynamicShadow() const = 0;

	// 阴影投影面积扩展百分比
	virtual void SetShadowExpandPercent(float value) = 0;
	virtual float GetShadowExpandPercent() = 0;

	// 是否开启early-z优化
	virtual void SetEnableEarlyZ(bool value) = 0;
	virtual bool GetEnableEarlyZ() const = 0;

	virtual void Update(float seconds) = 0;
	virtual void Realize() = 0;

	virtual ISceneView* GetSceneView() const = 0;

	virtual void SetSceneType(SCENETYPE type) = 0;

	virtual SCENETYPE GetSceneType()const = 0;

	// 设置动态阴影区域的修正矩形
	// vAdjustingRect.x = left
	// vAdjustingRect.y = right
	// vAdjustingRect.z = top
	// vAdjustingRect.w = bottom
	virtual void SetShadowAdjustingRect(FmVec4 vAdjustingRect) = 0;

	// 获取动态阴影区域的修正矩形
	// vResult.x = left
	// vResult.y = right
	// vResult.z = top
	// vResult.w = bottom
	virtual const FmVec4& GetShadowAdjustingRect() = 0;
};

#endif