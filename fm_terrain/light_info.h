//--------------------------------------------------------------------
// 文件名:		light_info.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年11月6日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
/*
#ifndef _LIGHT_INFO_H
#define _LIGHT_INFO_H

#include "../public/module.h"
#include "../public/core_type.h"
#include "light_types.h"

// 光源数据

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

// 光源信息

class LightInfo: public IEntity
{
public:
	LightInfo();
	virtual ~LightInfo();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// 名字
	void SetName(const char* value);
	const char* GetName() const;

	// 是否打开
	void SetTurnOn(bool value) { m_Data.bTurnOn = value; }
	bool GetTurnOn() const { return m_Data.bTurnOn; }

	// 光源类型
	void SetLightTypeString(const char* value);
	const char* GetLightTypeString() const;
	
	// 颜色乘数
	void SetIntensity(float value) { m_Data.fIntensity = value; }
	float GetIntensity() const { return m_Data.fIntensity; }

	// 红色
	void SetColorRed(float value) { m_Data.fColorRed = value; }
	float GetColorRed() const { return m_Data.fColorRed; }

	// 绿色
	void SetColorGreen(float value) { m_Data.fColorGreen = value; }
	float GetColorGreen() const { return m_Data.fColorGreen; }

	// 蓝色
	void SetColorBlue(float value) { m_Data.fColorBlue = value; }
	float GetColorBlue() const { return m_Data.fColorBlue; }

	// 角度余弦
	void SetCosine(float value) { m_Data.fCosine = value; }
	float GetCosine() const { return m_Data.fCosine; }

	// 投影
	void SetShadow(float value) { m_Data.fShadow = value; }
	float GetShadow() const { return m_Data.fShadow; }

	// 亮度
	void SetBright(float value) { m_Data.fBright = value; }
	float GetBright() const { return m_Data.fBright; }

	// 对比度
	void SetContrast(float value) { m_Data.fContrast = value; }
	float GetContrast() const { return m_Data.fContrast; }

	// Gamma修正
	void SetGamma(float value) { m_Data.fGamma = value; }
	float GetGamma() const { return m_Data.fGamma; }

	// 衰减系数0
	void SetAttenu0(float value) { m_Data.fAtt0 = value; }
	float GetAttenu0() const { return m_Data.fAtt0; }

	// 衰减系数1
	void SetAttenu1(float value) { m_Data.fAtt1 = value; }
	float GetAttenu1() const { return m_Data.fAtt1; }

	// 衰减系数2
	void SetAttenu2(float value) { m_Data.fAtt2 = value; }
	float GetAttenu2() const { return m_Data.fAtt2; }

	// 范围
	void SetRange(float value) { m_Data.fRange = value; }
	float GetRange() const { return m_Data.fRange; }

	// 聚光灯内角
	void SetInnerDegree(float value) { m_Data.fInnerDegree = value; }
	float GetInnerDegree() const { return m_Data.fInnerDegree; }

	// 聚光灯外角
	void SetOuterDegree(float value) { m_Data.fOuterDegree = value; }
	float GetOuterDegree() const { return m_Data.fOuterDegree; }

	// 聚光灯FALLOFF
	void SetFalloff(float value) { m_Data.fFalloff = value; }
	float GetFalloff() const { return m_Data.fFalloff; }

	// 光源位置
	void SetPosiX(float value) { m_Data.fPosiX = value; }
	float GetPosiX() const { return m_Data.fPosiX; }
	void SetPosiY(float value) { m_Data.fPosiY = value; }
	float GetPosiY() const { return m_Data.fPosiY; }
	void SetPosiZ(float value) { m_Data.fPosiZ = value; }
	float GetPosiZ() const { return m_Data.fPosiZ; }
	
	// 光源角度
	void SetAngleX(float value) { m_Data.fAngleX = value; }
	float GetAngleX() const { return m_Data.fAngleX; }
	void SetAngleY(float value) { m_Data.fAngleY = value; }
	float GetAngleY() const { return m_Data.fAngleY; }
	void SetAngleZ(float value) { m_Data.fAngleZ = value; }
	float GetAngleZ() const { return m_Data.fAngleZ; }

	// 盒子光源尺寸
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