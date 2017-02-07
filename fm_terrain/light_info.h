//--------------------------------------------------------------------
// �ļ���:		light_info.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��11��6��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
/*
#ifndef _LIGHT_INFO_H
#define _LIGHT_INFO_H

#include "../public/module.h"
#include "../public/core_type.h"
#include "light_types.h"

// ��Դ����

struct light_data_t
{
	bool bTurnOn;
	light_t::Type nType;
	float fIntensity;
	float fColorRed;
	float fColorGreen;
	float fColorBlue;
	float fCosine;
	float fShadow;
	float fBright;
	float fContrast;
	float fGamma;
	float fAtt0;
	float fAtt1;
	float fAtt2;
	float fRange;
	float fInnerDegree;
	float fOuterDegree;
	float fFalloff;
	float fPosiX;
	float fPosiY;
	float fPosiZ;
	float fAngleX;
	float fAngleY;
	float fAngleZ;
	float fSizeX;
	float fSizeY;
	float fSizeZ;
};

// ��Դ��Ϣ

class LightInfo: public IEntity
{
public:
	LightInfo();
	virtual ~LightInfo();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// ����
	void SetName(const char* value);
	const char* GetName() const;

	// �Ƿ��
	void SetTurnOn(bool value) { m_Data.bTurnOn = value; }
	bool GetTurnOn() const { return m_Data.bTurnOn; }

	// ��Դ����
	void SetLightTypeString(const char* value);
	const char* GetLightTypeString() const;
	
	// ��ɫ����
	void SetIntensity(float value) { m_Data.fIntensity = value; }
	float GetIntensity() const { return m_Data.fIntensity; }

	// ��ɫ
	void SetColorRed(float value) { m_Data.fColorRed = value; }
	float GetColorRed() const { return m_Data.fColorRed; }

	// ��ɫ
	void SetColorGreen(float value) { m_Data.fColorGreen = value; }
	float GetColorGreen() const { return m_Data.fColorGreen; }

	// ��ɫ
	void SetColorBlue(float value) { m_Data.fColorBlue = value; }
	float GetColorBlue() const { return m_Data.fColorBlue; }

	// �Ƕ�����
	void SetCosine(float value) { m_Data.fCosine = value; }
	float GetCosine() const { return m_Data.fCosine; }

	// ͶӰ
	void SetShadow(float value) { m_Data.fShadow = value; }
	float GetShadow() const { return m_Data.fShadow; }

	// ����
	void SetBright(float value) { m_Data.fBright = value; }
	float GetBright() const { return m_Data.fBright; }

	// �Աȶ�
	void SetContrast(float value) { m_Data.fContrast = value; }
	float GetContrast() const { return m_Data.fContrast; }

	// Gamma����
	void SetGamma(float value) { m_Data.fGamma = value; }
	float GetGamma() const { return m_Data.fGamma; }

	// ˥��ϵ��0
	void SetAttenu0(float value) { m_Data.fAtt0 = value; }
	float GetAttenu0() const { return m_Data.fAtt0; }

	// ˥��ϵ��1
	void SetAttenu1(float value) { m_Data.fAtt1 = value; }
	float GetAttenu1() const { return m_Data.fAtt1; }

	// ˥��ϵ��2
	void SetAttenu2(float value) { m_Data.fAtt2 = value; }
	float GetAttenu2() const { return m_Data.fAtt2; }

	// ��Χ
	void SetRange(float value) { m_Data.fRange = value; }
	float GetRange() const { return m_Data.fRange; }

	// �۹���ڽ�
	void SetInnerDegree(float value) { m_Data.fInnerDegree = value; }
	float GetInnerDegree() const { return m_Data.fInnerDegree; }

	// �۹�����
	void SetOuterDegree(float value) { m_Data.fOuterDegree = value; }
	float GetOuterDegree() const { return m_Data.fOuterDegree; }

	// �۹��FALLOFF
	void SetFalloff(float value) { m_Data.fFalloff = value; }
	float GetFalloff() const { return m_Data.fFalloff; }

	// ��Դλ��
	void SetPosiX(float value) { m_Data.fPosiX = value; }
	float GetPosiX() const { return m_Data.fPosiX; }
	void SetPosiY(float value) { m_Data.fPosiY = value; }
	float GetPosiY() const { return m_Data.fPosiY; }
	void SetPosiZ(float value) { m_Data.fPosiZ = value; }
	float GetPosiZ() const { return m_Data.fPosiZ; }
	
	// ��Դ�Ƕ�
	void SetAngleX(float value) { m_Data.fAngleX = value; }
	float GetAngleX() const { return m_Data.fAngleX; }
	void SetAngleY(float value) { m_Data.fAngleY = value; }
	float GetAngleY() const { return m_Data.fAngleY; }
	void SetAngleZ(float value) { m_Data.fAngleZ = value; }
	float GetAngleZ() const { return m_Data.fAngleZ; }

	// ���ӹ�Դ�ߴ�
	void SetSizeX(float value) { m_Data.fSizeX = value; }
	float GetSizeX() const { return m_Data.fSizeX; }
	void SetSizeY(float value) { m_Data.fSizeY = value; }
	float GetSizeY() const { return m_Data.fSizeY; }
	void SetSizeZ(float value) { m_Data.fSizeZ = value; }
	float GetSizeZ() const { return m_Data.fSizeZ; }

	const light_data_t* GetData() const { return &m_Data; }

private:
	core_string m_strName;
	light_data_t m_Data;
};

#endif // _LIGHT_INFO_H
*/