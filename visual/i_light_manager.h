#pragma once

#include "../visual/i_vis_base.h"

class ILightManager : public IVisBase
{
public:
	// ����Դ����
	virtual void SetMaxLightNum(int value) = 0;
	virtual int GetMaxLightNum() const = 0;

	// �Ƿ�������Դ
	virtual void SetSunLighting(bool value) = 0;
	virtual bool GetSunLighting() const = 0;

	// �Ƿ�����̬����ʱ��������Դ����
	virtual void SetRolePointLighting(bool value) = 0;
	virtual bool GetRolePointLighting() const = 0;

	// �Ƿ�����Դ��Ӱ
	virtual void SetEnableShadow(bool value) = 0;
	virtual bool GetEnableShadow() const = 0;

	// �Ƿ��Ż���Ӱͼ������
	virtual void SetShadowMapOptimal(bool value) = 0;
	virtual bool GetShadowMapOptimal() const = 0;

	// �Ƿ�ʹ����ȱ߽����
	virtual void SetUseDepthBoundTest(bool value) = 0;
	virtual bool GetUseDepthBoundTest() const = 0;

	// ���б��
	virtual void SetDepthSlope(float value) = 0;
	virtual float GetDepthSlope() const = 0;

	// ���ƫ��
	virtual void SetDepthBias(float value) = 0;
	virtual float GetDepthBias() const = 0;

	// ��Ӱͼ���˼���
	virtual void SetFilterLevel(int value) = 0;
	virtual int GetFilterLevel() const = 0;

	// ������Դ����
	virtual PERSISTID Create() = 0;
	// ���������ļ�������Դ����
	virtual PERSISTID CreateFromIni(const PERSISTID& id,const char* ini_name, 
		const char* light_name, const char* helper_name) = 0;
	// ɾ����Դ����
	virtual bool Delete(const PERSISTID& id) = 0;
	// ���ع�Դ�б�
	virtual void GetLightList(const IVarList& args, IVarList& result) = 0;

};