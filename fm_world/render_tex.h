//--------------------------------------------------------------------
// 文件名:		render_tex.h
// 内  容:		
// 说  明:		
// 创建日期:	2012年12月8日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _RENDER_TEX_H
#define _RENDER_TEX_H

#include "../public/module.h"
#include "../public/core_type.h"
#include "../visual/i_resource.h"
#include "../visual/i_texture.h"

// 做为渲染目标的贴图

class IRender;
class IRenderContext;
class ISceneView;
class IVisBase;
class Weather;
class Camera;

class RenderTex: public IEntity
{
private:
	struct object_t
	{
		PERSISTID ObjectId;
		int nPrior;
		object_t* pNext;
	};
	
public:
	RenderTex();
	virtual ~RenderTex();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	bool Load();

	void Update(float seconds);
	void Realize();

	// 渲染器参数
	IRenderContext* GetContext() const { return m_pContext; }
	// 获得场景视图
	ISceneView* GetSceneView() const { return m_pSceneView; }

	// 获得天气对象
	PERSISTID GetWeatherID();
	Weather* GetWeather() const { return m_pWeather; }
	// 获得摄像机对象
	PERSISTID GetCameraID();
	Camera* GetCamera() const { return m_pCamera; }

	// 贴图名字
	void SetName(const char* value);
	const char* GetName() const;

	// 贴图宽度
	void SetWidth(int value);
	int GetWidth() const;

	// 贴图高度
	void SetHeight(int value);
	int GetHeight() const;

	// 背景颜色
	void SetBackColorString(const char* value);
	result_string GetBackColorString() const;
	
	// 远剪裁面的距离
	void SetFarClipDistance(float value);
	float GetFarClipDistance() const;

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

private:
	// 获得可见对象
	IVisBase* GetVisBase(const PERSISTID& id) const;

private:
	IRender* m_pRender;
	IRenderContext* m_pContext;
	ISceneView* m_pSceneView;
	Weather* m_pWeather;
	Camera* m_pCamera;
	object_t* m_pObjects;
	core_string m_strName;
	int m_nWidth;
	int m_nHeight;
	unsigned int m_nBackColor;
	ITexture* m_pRenderTex;
	IDepthRT* m_pDepthRT;
};

#endif // _RENDER_TEX_H
