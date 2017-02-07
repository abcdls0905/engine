//--------------------------------------------------------------------
// �ļ���:		terrain_material.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��1��28��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TERRAIN_MATERIAL_H
#define _TERRAIN_MATERIAL_H

#include "../public/module.h"


#include "../visual/i_render.h"

// ���β���

class Terrain;

class TerrainMaterial: public IEntity
{
public:
	TerrainMaterial();
	virtual ~TerrainMaterial();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	void SetTerrain(Terrain* value) { m_pTerrain = value; }

	// ������
	void SetDiffuseAlpha(float value);
	float GetDiffuseAlpha() const;
	void SetDiffuseRed(float value);
	float GetDiffuseRed() const;
	void SetDiffuseGreen(float value);
	float GetDiffuseGreen() const;
	void SetDiffuseBlue(float value);
	float GetDiffuseBlue() const;
	
	// ��������
	void SetAmbientAlpha(float value);
	float GetAmbientAlpha() const;
	void SetAmbientRed(float value);
	float GetAmbientRed() const;
	void SetAmbientGreen(float value);
	float GetAmbientGreen() const;
	void SetAmbientBlue(float value);
	float GetAmbientBlue() const;
	
	// ���淴��
	void SetSpecularAlpha(float value);
	float GetSpecularAlpha() const;
	void SetSpecularRed(float value);
	float GetSpecularRed() const;
	void SetSpecularGreen(float value);
	float GetSpecularGreen() const;
	void SetSpecularBlue(float value);
	float GetSpecularBlue() const;

	// ���߷���
	void SetEmissiveAlpha(float value);
	float GetEmissiveAlpha() const;
	void SetEmissiveRed(float value);
	float GetEmissiveRed() const;
	void SetEmissiveGreen(float value);
	float GetEmissiveGreen() const;
	void SetEmissiveBlue(float value);
	float GetEmissiveBlue() const;

	// ���淴��ǿ��
	void SetSpecularPower(float value);
	float GetSpecularPower() const;

	// �Ƿ������淴��
	void SetSpecularEnable(bool value);
	bool GetSpecularEnable() const;
	
private:
	Terrain* m_pTerrain;
};

#endif // _TERRAIN_MATERIAL_H
