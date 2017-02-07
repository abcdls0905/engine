//--------------------------------------------------------------------
// 文件名:		lighter.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年11月5日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
/*
#ifndef _LIGHTER_H
#define _LIGHTER_H

#include "../public/module.h"
#include "../visual/dx_header.h"

// 光照渲染

class Render;
class CLightGeometry;
class CLightOctTree;
class CLightSet;
class CLightProcessor;

class Lighter: public IEntity  
{
public:
	Lighter();
	virtual ~Lighter();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	virtual void Execute(float seconds);

	// 使用模型顶点色
	void SetUseColor(bool value);
	bool GetUseColor() const;

	// 是否只渲染地面
	void SetOnlyGround(bool value);
	bool GetOnlyGround() const;
	
	// 是否保存遮挡信息
	void SetSaveOcclusion(bool value);
	bool GetSaveOcclusion() const;

	// 是否根据面积自动选择光贴图大小降级
	void SetLightMapDegrade(bool value);
	bool GetLightMapDegrade() const;

	// 物体光贴图大小
	void SetLightMapSize(int value);
	int GetLightMapSize() const;

	// 物体光贴图填充范围
	void SetLightMapFill(int value);
	int GetLightMapFill() const;

	// 物体光照颜色分辨率
	void SetLightPerUnit(size_t value);
	size_t GetLightPerUnit() const;

	// 区域光照文件路径
	void SetZonePath(const char* value);
	const char* GetZonePath() const;
	
	// 模型光照文件路径
	void SetModelPath(const char* value);
	const char* GetModelPath() const;
	
	// 模型光照文件后缀
	void SetModelSuffix(const char* value);
	const char* GetModelSuffix() const;
	
	// 开始生成投影
	void SetTraceShadows(bool value) { m_bTraceShadows = value; }
	bool GetTraceShadows() const { return m_bTraceShadows; }

	// 开始平滑阴影
	void SetSmoothShadows(bool value) { m_bSmoothShadows = value; }
	bool GetSmoothShadows() const { return m_bSmoothShadows; }

	// 开始模糊阴影 
	void SetBlurLight(bool value) { m_bBlurLight = value; }
	bool GetBlurLight() const { return m_bBlurLight; }

	// 重新模糊阴影
	void SetResetBlurLight(bool value) { m_bResetBlurLight = value; }
	bool GetResetBlurLight() const { return m_bResetBlurLight; }

	// 更新光照
	void SetUpdateLight(bool value) { m_bUpdateLight = value; }
	bool GetUpdateLight() const { return m_bUpdateLight; }

	// 是否使用天空光
	void SetUseSkyLight(bool value) { m_bUseSkyLight = value; }
	bool GetUseSkyLight() const { return m_bUseSkyLight; }

	// 是否使用点光源
	void SetUsePointLight(bool value) { m_bUsePointLight = value; }
	bool GetUsePointLight() const { return m_bUsePointLight; }

	// 是否使用法线进行平滑处理
	void SetSmoothNormal(bool value) { m_bSmoothNormal = value; }
	bool GetSmoothNormal() const { return m_bSmoothNormal; }

	// 平滑处理半径
	void SetSmoothRadius(float value) { m_fSmoothRadius = value; }
	float GetSmoothRadius() const { return m_fSmoothRadius; }

	// 模糊处理是否使用碰撞检测
	void SetBlurTrace(bool value) { m_bBlurTrace = value; }
	bool GetBlurTrace() const { return m_bBlurTrace; }

	// 模糊半径
	void SetBlurRadius(float value) { m_fBlurRadius = value; }
	float GetBlurRadius() const { return m_fBlurRadius; }

	// 模糊衰减
	void SetBlurAtt(float value) { m_fBlurAtt = value; }
	float GetBlurAtt() const { return m_fBlurAtt; }

	// 模糊角度值
	void SetBlurCos(float value) { m_fBlurCos = value; }
	float GetBlurCos() const { return m_fBlurCos; }

	// 模糊系数
	void SetBlurCoeff(float value) { m_fBlurCoeff = value; }
	float GetBlurCoeff() const { return m_fBlurCoeff; }
	
	// 产生阴影的最大距离
	void SetShadowDistance(float value) { m_fShadowDistance = value; }
	float GetShadowDistance() const { return m_fShadowDistance; }

	// 地形对象
	void SetTerrainID(const PERSISTID& id);
	PERSISTID GetTerrainID() const;

	// 渲染步骤是否完成
	bool GetShadowFinish() const;
	bool GetSmoothFinish() const;
	bool GetBlurFinish() const;

	// 渲染进度百分比
	float GetShadowPercent() const;
	float GetSmoothPercent() const;
	float GetBlurPercent() const;
	
	// 添加地形区域
	bool AddZone(int zone_index, int density, float add_size);
	// 添加地形区域做测试
	bool AddZoneTest(int zone_index, int density, 
		float add_size, int tex_scale);
	// 增加模型
	bool AddModel(const char* name, const PERSISTID& id, 
		const char* light_type, int light_map_size, int light_map_fill);
	// 增加光源
	bool AddLight(const PERSISTID& info);
	// 更新光源
	bool SetLight(const PERSISTID& info);

	// 预处理数据
	bool PrepareData();
	// 保存模型光照文件 
	bool SaveModelLight();
	// 保存地形光照文件
	bool SaveZoneLight();

private:
	Render* m_pRender;
	CLightGeometry* m_pGeometry;
	CLightOctTree* m_pOctTree;
	CLightSet* m_pLights;
	CLightProcessor* m_pProcessor;

	PERSISTID m_TerrainID;

	bool m_bTraceShadows;
	bool m_bSmoothShadows;
	bool m_bBlurLight;
	bool m_bResetBlurLight;
	bool m_bUpdateLight;

	bool m_bUseSkyLight;
	bool m_bUsePointLight;
	
	bool m_bSmoothNormal;
	float m_fSmoothRadius;
	
	bool m_bBlurTrace;
	float m_fBlurRadius;
	float m_fBlurAtt;
	float m_fBlurCos;
	float m_fBlurCoeff;

	float m_fShadowDistance;
};

#endif // _LIGHTER_H
*/
