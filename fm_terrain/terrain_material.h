//--------------------------------------------------------------------
// 文件名:		terrain_material.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年1月28日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TERRAIN_MATERIAL_H
#define _TERRAIN_MATERIAL_H

#include "../public/module.h"


#include "../visual/i_render.h"

// 地形材质

class Terrain;

class TerrainMaterial: public IEntity
{
public:
	TerrainMaterial();
	virtual ~TerrainMaterial();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	void SetTerrain(Terrain* value) { m_pTerrain = value; }

	// 漫反射
	void SetDiffuseAlpha(float value);
	float GetDiffuseAlpha() const;
	void SetDiffuseRed(float value);
	float GetDiffuseRed() const;
	void SetDiffuseGreen(float value);
	float GetDiffuseGreen() const;
	void SetDiffuseBlue(float value);
	float GetDiffuseBlue() const;
	
	// 环境反射
	void SetAmbientAlpha(float value);
	float GetAmbientAlpha() const;
	void SetAmbientRed(float value);
	float GetAmbientRed() const;
	void SetAmbientGreen(float value);
	float GetAmbientGreen() const;
	void SetAmbientBlue(float value);
	float GetAmbientBlue() const;
	
	// 镜面反射
	void SetSpecularAlpha(float value);
	float GetSpecularAlpha() const;
	void SetSpecularRed(float value);
	float GetSpecularRed() const;
	void SetSpecularGreen(float value);
	float GetSpecularGreen() const;
	void SetSpecularBlue(float value);
	float GetSpecularBlue() const;

	// 光线辐射
	void SetEmissiveAlpha(float value);
	float GetEmissiveAlpha() const;
	void SetEmissiveRed(float value);
	float GetEmissiveRed() const;
	void SetEmissiveGreen(float value);
	float GetEmissiveGreen() const;
	void SetEmissiveBlue(float value);
	float GetEmissiveBlue() const;

	// 镜面反射强度
	void SetSpecularPower(float value);
	float GetSpecularPower() const;

	// 是否开启镜面反射
	void SetSpecularEnable(bool value);
	bool GetSpecularEnable() const;
	
private:
	Terrain* m_pTerrain;
};

#endif // _TERRAIN_MATERIAL_H
