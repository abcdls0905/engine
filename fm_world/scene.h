//--------------------------------------------------------------------
// 文件名:		scene.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月28日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SCENE_H
#define _SCENE_H

#include "../public/module.h"
#include "../visual/i_resource.h"
#include "../visual/i_scene.h"
#include "../visual/i_weather.h"
#include "camera.h"
#include "weather.h"
// 场景

class IRender;
class IRenderContext;
class ISceneView;
class IVisBase;
class PSSM;

class Scene: public IScene
{
	SCENETYPE m_scenetype;
private:
	struct object_t
	{
		PERSISTID ObjectId;
		int nPrior;
		object_t* pNext;
	};
	
public:
	Scene();
	virtual ~Scene();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	void Update(float seconds);
	void Realize();

	// 渲染器参数
	IRenderContext* GetContext() const { return m_pContext; }
	// 获得场景视图
	ISceneView* GetSceneView() const { return m_pSceneView; }

	// 获得天气对象
	PERSISTID GetWeatherID();
	IWeather* GetWeather() const { return m_pWeather; }
	// 获得摄像机对象
	PERSISTID GetCameraID();
	ICamera* GetCamera() const { return m_pCamera; }

	// 远剪裁面的距离
	void SetFarClipDistance(float value);
	float GetFarClipDistance() const;

	// 半透明物体降级显示
	void SetBlendDownLevel(int value);
	int GetBlendDownLevel() const;

	// 是否要渲染折射图
	void SetEnableRealizeRefraction(bool value);
	bool GetEnableRealizeRefraction() const;

	// 是否要开启临时渲染纹理缓冲
	void SetEnableRealizeTempColorRT(bool value);
	bool GetEnableRealizeTempColorRT() const;

	// 是否需要开启临时渲染深度纹理
	void SetEnableRealizeTempDepthRT(bool value);
	bool GetEnableRealizeTempDepthRT() const;

	// 是否要开启临时渲染纹理缓冲
	void SetEnableRealizeCopyTempColorRT(bool value);
	bool GetEnableRealizeCopyTempColorRT() const;

	// 是否需要开启临时渲染深度纹理
	void SetEnableRealizeCopyTempDepthRT(bool value);
	bool GetEnableRealizeCopyTempDepthRT() const;

	// 骨骼动画模型是否开启半球环境光
	void SetSkinSphereAmbient(bool value);
	bool GetSkinSphereAmbient() const;

	// 普通模型是否开启半球环境光
	void SetModelSphereAmbient(bool value);
	bool GetModelSphereAmbient() const;

	// 全局环境贴图
	void SetGlobalEnvMap(const char* name);
	const char* GetGlobalEnvMap() const;

	// 环境反射的乘法系数
	void SetReflectFactor(float value);
	float GetReflectFactor() const;

	// 是否开启early-z优化
	void SetEnableEarlyZ(bool value);
	bool GetEnableEarlyZ() const;

	// 人物光照调节系数
	void SetRoleDiffuseFactor(float value);
	float GetRoleDiffuseFactor() const;
	void SetRoleAmbientFactor(float value);
	float GetRoleAmbientFactor() const;

	// 人物轮廓光参数
	void SetEnableRoleFallOff(bool value);
	bool GetEnableRoleFallOff() const;
	void SetRoleFallOffColorString(const char* value);
	result_string GetRoleFallOffColorString() const;
	void SetRoleFallOffInten(float value);
	float GetRoleFallOffInten() const;
	void SetRoleFallOffPower(float value);
	float GetRoleFallOffPower() const;

	// 创建可视对象
	PERSISTID Create(const char* name);
	// 删除可视对象
	bool Delete(const PERSISTID& id);

	// 场景唯一实体
	bool SetUnique(const PERSISTID& obj);
	PERSISTID GetUnique(const char* entity_name);

	// 添加可视对象
	bool AddObject(const PERSISTID& obj, int priority);
	// 删除可视对象
	bool RemoveObject(const PERSISTID& obj);
	// 按优先级删除对象
	bool RemoveObjectByPrior(int priority);

	// 添加对象到层
	bool LayerAdd(const char* name, const PERSISTID& id);
	// 从层移除对象
	bool LayerRemove(const char* name, const PERSISTID& id);

	// 是否要开启动态阴影
	void SetEnableDynamicShadow(bool value);
	bool GetEnableDynamicShadow() const;

	// 阴影投影面积扩展百分比
	void SetShadowExpandPercent(float value) ;
	float GetShadowExpandPercent() ;

	inline void SetSceneType(SCENETYPE type){ m_scenetype = type; }

	inline SCENETYPE GetSceneType()const { return m_scenetype; }

	virtual void SetShadowAdjustingRect(FmVec4 vAdjustingRect);
	virtual const FmVec4& GetShadowAdjustingRect();

private:

	// 卸载资源
	bool Unload();
	// 显示渲染目标纹理
	void ShowRenderTaret();
	// 获得可见对象
	IVisBase* GetVisBase(const PERSISTID& id) const;

	// 阴影实体对象
	PERSISTID GetPSSM() const;
	PERSISTID CreatePSSM();
	bool DeletePSSM();

private:

	IRender* m_pRender;
	IRenderContext* m_pContext;
	ISceneView* m_pSceneView;
	Weather* m_pWeather;
	Camera* m_pCamera;
	object_t* m_pObjects;
	PSSM* m_pPSSM;
};	

#endif // _SCENE_H
