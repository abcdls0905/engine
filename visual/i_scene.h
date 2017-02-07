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
	// ��Ⱦ������
	virtual IRenderContext* GetContext() const = 0;
	
	// �����������
	virtual PERSISTID GetWeatherID() = 0;
	
	virtual IWeather* GetWeather() const = 0;
	
	// ������������
	virtual PERSISTID GetCameraID() = 0;
	
	virtual ICamera* GetCamera() const = 0;

	// Զ������ľ���
	virtual void SetFarClipDistance(float value) = 0;
	virtual float GetFarClipDistance() const = 0;

	// ��͸�����彵����ʾ
	virtual void SetBlendDownLevel(int value) = 0;
	virtual int GetBlendDownLevel() const = 0;

	// �Ƿ�Ҫ��Ⱦ����ͼ
	virtual void SetEnableRealizeRefraction(bool value) = 0;
	virtual bool GetEnableRealizeRefraction() const = 0;

	// �Ƿ�Ҫ������ʱ��Ⱦ������
	virtual void SetEnableRealizeTempColorRT(bool value) = 0;
	virtual bool GetEnableRealizeTempColorRT() const = 0;

	// �Ƿ���Ҫ������ʱ��Ⱦ�������
	virtual void SetEnableRealizeTempDepthRT(bool value) = 0;
	virtual bool GetEnableRealizeTempDepthRT() const = 0;


	// �Ƿ�Ҫ����������ʱ��Ⱦ������
	virtual void SetEnableRealizeCopyTempColorRT(bool value) = 0;
	virtual bool GetEnableRealizeCopyTempColorRT() const = 0;

	// �Ƿ���Ҫ����������ʱ��Ⱦ�������
	virtual void SetEnableRealizeCopyTempDepthRT(bool value) = 0;
	virtual bool GetEnableRealizeCopyTempDepthRT() const = 0;

	// ��������ģ���Ƿ������򻷾���
	virtual void SetSkinSphereAmbient(bool value) = 0;
	virtual bool GetSkinSphereAmbient() const = 0;

	// ��ͨģ���Ƿ������򻷾���
	virtual void SetModelSphereAmbient(bool value) = 0;
	virtual bool GetModelSphereAmbient() const = 0;

	// ȫ�ֻ�����ͼ
	virtual void SetGlobalEnvMap(const char* name) = 0;
	virtual const char* GetGlobalEnvMap() const = 0;

	// ��������ĳ˷�ϵ��
	virtual void SetReflectFactor(float value) = 0;
	virtual float GetReflectFactor() const = 0;

	// ������յ���ϵ��
	virtual void SetRoleDiffuseFactor(float value) = 0;
	virtual float GetRoleDiffuseFactor() const = 0;
	virtual void SetRoleAmbientFactor(float value) = 0;
	virtual float GetRoleAmbientFactor() const = 0;

	// �������������
	virtual void SetEnableRoleFallOff(bool value) = 0;
	virtual bool GetEnableRoleFallOff() const = 0;
	virtual void SetRoleFallOffColorString(const char* value) = 0;
	virtual result_string GetRoleFallOffColorString() const = 0;
	virtual void SetRoleFallOffInten(float value) = 0;
	virtual float GetRoleFallOffInten() const = 0;
	virtual void SetRoleFallOffPower(float value) = 0;
	virtual float GetRoleFallOffPower() const = 0;

	// �������Ӷ���
	virtual PERSISTID Create(const char* name) = 0;
	// ɾ�����Ӷ���
	virtual bool Delete(const PERSISTID& id) = 0;

	// ����Ψһʵ��
	virtual bool SetUnique(const PERSISTID& obj) = 0;
	virtual PERSISTID GetUnique(const char* entity_name) = 0;

	// ��ӿ��Ӷ���
	virtual bool AddObject(const PERSISTID& obj, int priority) = 0;
	// ɾ�����Ӷ���
	virtual bool RemoveObject(const PERSISTID& obj) = 0;
	// �����ȼ�ɾ������
	virtual bool RemoveObjectByPrior(int priority) = 0;

	// ��Ӷ��󵽲�
	virtual bool LayerAdd(const char* name, const PERSISTID& id) = 0;
	// �Ӳ��Ƴ�����
	virtual bool LayerRemove(const char* name, const PERSISTID& id) = 0;

	// �Ƿ�Ҫ������̬��Ӱ
	virtual void SetEnableDynamicShadow(bool value) = 0;
	virtual bool GetEnableDynamicShadow() const = 0;

	// ��ӰͶӰ�����չ�ٷֱ�
	virtual void SetShadowExpandPercent(float value) = 0;
	virtual float GetShadowExpandPercent() = 0;

	// �Ƿ���early-z�Ż�
	virtual void SetEnableEarlyZ(bool value) = 0;
	virtual bool GetEnableEarlyZ() const = 0;

	virtual void Update(float seconds) = 0;
	virtual void Realize() = 0;

	virtual ISceneView* GetSceneView() const = 0;

	virtual void SetSceneType(SCENETYPE type) = 0;

	virtual SCENETYPE GetSceneType()const = 0;

	// ���ö�̬��Ӱ�������������
	// vAdjustingRect.x = left
	// vAdjustingRect.y = right
	// vAdjustingRect.z = top
	// vAdjustingRect.w = bottom
	virtual void SetShadowAdjustingRect(FmVec4 vAdjustingRect) = 0;

	// ��ȡ��̬��Ӱ�������������
	// vResult.x = left
	// vResult.y = right
	// vResult.z = top
	// vResult.w = bottom
	virtual const FmVec4& GetShadowAdjustingRect() = 0;
};

#endif