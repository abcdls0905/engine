//--------------------------------------------------------------------
// �ļ���:		lighter.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��11��5��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
/*
#ifndef _LIGHTER_H
#define _LIGHTER_H

#include "../public/module.h"
#include "../visual/dx_header.h"

// ������Ⱦ

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

	// ʹ��ģ�Ͷ���ɫ
	void SetUseColor(bool value);
	bool GetUseColor() const;

	// �Ƿ�ֻ��Ⱦ����
	void SetOnlyGround(bool value);
	bool GetOnlyGround() const;
	
	// �Ƿ񱣴��ڵ���Ϣ
	void SetSaveOcclusion(bool value);
	bool GetSaveOcclusion() const;

	// �Ƿ��������Զ�ѡ�����ͼ��С����
	void SetLightMapDegrade(bool value);
	bool GetLightMapDegrade() const;

	// �������ͼ��С
	void SetLightMapSize(int value);
	int GetLightMapSize() const;

	// �������ͼ��䷶Χ
	void SetLightMapFill(int value);
	int GetLightMapFill() const;

	// ���������ɫ�ֱ���
	void SetLightPerUnit(size_t value);
	size_t GetLightPerUnit() const;

	// ��������ļ�·��
	void SetZonePath(const char* value);
	const char* GetZonePath() const;
	
	// ģ�͹����ļ�·��
	void SetModelPath(const char* value);
	const char* GetModelPath() const;
	
	// ģ�͹����ļ���׺
	void SetModelSuffix(const char* value);
	const char* GetModelSuffix() const;
	
	// ��ʼ����ͶӰ
	void SetTraceShadows(bool value) { m_bTraceShadows = value; }
	bool GetTraceShadows() const { return m_bTraceShadows; }

	// ��ʼƽ����Ӱ
	void SetSmoothShadows(bool value) { m_bSmoothShadows = value; }
	bool GetSmoothShadows() const { return m_bSmoothShadows; }

	// ��ʼģ����Ӱ 
	void SetBlurLight(bool value) { m_bBlurLight = value; }
	bool GetBlurLight() const { return m_bBlurLight; }

	// ����ģ����Ӱ
	void SetResetBlurLight(bool value) { m_bResetBlurLight = value; }
	bool GetResetBlurLight() const { return m_bResetBlurLight; }

	// ���¹���
	void SetUpdateLight(bool value) { m_bUpdateLight = value; }
	bool GetUpdateLight() const { return m_bUpdateLight; }

	// �Ƿ�ʹ����չ�
	void SetUseSkyLight(bool value) { m_bUseSkyLight = value; }
	bool GetUseSkyLight() const { return m_bUseSkyLight; }

	// �Ƿ�ʹ�õ��Դ
	void SetUsePointLight(bool value) { m_bUsePointLight = value; }
	bool GetUsePointLight() const { return m_bUsePointLight; }

	// �Ƿ�ʹ�÷��߽���ƽ������
	void SetSmoothNormal(bool value) { m_bSmoothNormal = value; }
	bool GetSmoothNormal() const { return m_bSmoothNormal; }

	// ƽ������뾶
	void SetSmoothRadius(float value) { m_fSmoothRadius = value; }
	float GetSmoothRadius() const { return m_fSmoothRadius; }

	// ģ�������Ƿ�ʹ����ײ���
	void SetBlurTrace(bool value) { m_bBlurTrace = value; }
	bool GetBlurTrace() const { return m_bBlurTrace; }

	// ģ���뾶
	void SetBlurRadius(float value) { m_fBlurRadius = value; }
	float GetBlurRadius() const { return m_fBlurRadius; }

	// ģ��˥��
	void SetBlurAtt(float value) { m_fBlurAtt = value; }
	float GetBlurAtt() const { return m_fBlurAtt; }

	// ģ���Ƕ�ֵ
	void SetBlurCos(float value) { m_fBlurCos = value; }
	float GetBlurCos() const { return m_fBlurCos; }

	// ģ��ϵ��
	void SetBlurCoeff(float value) { m_fBlurCoeff = value; }
	float GetBlurCoeff() const { return m_fBlurCoeff; }
	
	// ������Ӱ��������
	void SetShadowDistance(float value) { m_fShadowDistance = value; }
	float GetShadowDistance() const { return m_fShadowDistance; }

	// ���ζ���
	void SetTerrainID(const PERSISTID& id);
	PERSISTID GetTerrainID() const;

	// ��Ⱦ�����Ƿ����
	bool GetShadowFinish() const;
	bool GetSmoothFinish() const;
	bool GetBlurFinish() const;

	// ��Ⱦ���Ȱٷֱ�
	float GetShadowPercent() const;
	float GetSmoothPercent() const;
	float GetBlurPercent() const;
	
	// ��ӵ�������
	bool AddZone(int zone_index, int density, float add_size);
	// ��ӵ�������������
	bool AddZoneTest(int zone_index, int density, 
		float add_size, int tex_scale);
	// ����ģ��
	bool AddModel(const char* name, const PERSISTID& id, 
		const char* light_type, int light_map_size, int light_map_fill);
	// ���ӹ�Դ
	bool AddLight(const PERSISTID& info);
	// ���¹�Դ
	bool SetLight(const PERSISTID& info);

	// Ԥ��������
	bool PrepareData();
	// ����ģ�͹����ļ� 
	bool SaveModelLight();
	// ������ι����ļ�
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
