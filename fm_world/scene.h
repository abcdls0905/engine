//--------------------------------------------------------------------
// �ļ���:		scene.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��5��28��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SCENE_H
#define _SCENE_H

#include "../public/module.h"
#include "../visual/i_resource.h"
#include "../visual/i_scene.h"
#include "../visual/i_weather.h"
#include "camera.h"
#include "weather.h"
// ����

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

	// ��Ⱦ������
	IRenderContext* GetContext() const { return m_pContext; }
	// ��ó�����ͼ
	ISceneView* GetSceneView() const { return m_pSceneView; }

	// �����������
	PERSISTID GetWeatherID();
	IWeather* GetWeather() const { return m_pWeather; }
	// ������������
	PERSISTID GetCameraID();
	ICamera* GetCamera() const { return m_pCamera; }

	// Զ������ľ���
	void SetFarClipDistance(float value);
	float GetFarClipDistance() const;

	// ��͸�����彵����ʾ
	void SetBlendDownLevel(int value);
	int GetBlendDownLevel() const;

	// �Ƿ�Ҫ��Ⱦ����ͼ
	void SetEnableRealizeRefraction(bool value);
	bool GetEnableRealizeRefraction() const;

	// �Ƿ�Ҫ������ʱ��Ⱦ������
	void SetEnableRealizeTempColorRT(bool value);
	bool GetEnableRealizeTempColorRT() const;

	// �Ƿ���Ҫ������ʱ��Ⱦ�������
	void SetEnableRealizeTempDepthRT(bool value);
	bool GetEnableRealizeTempDepthRT() const;

	// �Ƿ�Ҫ������ʱ��Ⱦ������
	void SetEnableRealizeCopyTempColorRT(bool value);
	bool GetEnableRealizeCopyTempColorRT() const;

	// �Ƿ���Ҫ������ʱ��Ⱦ�������
	void SetEnableRealizeCopyTempDepthRT(bool value);
	bool GetEnableRealizeCopyTempDepthRT() const;

	// ��������ģ���Ƿ������򻷾���
	void SetSkinSphereAmbient(bool value);
	bool GetSkinSphereAmbient() const;

	// ��ͨģ���Ƿ������򻷾���
	void SetModelSphereAmbient(bool value);
	bool GetModelSphereAmbient() const;

	// ȫ�ֻ�����ͼ
	void SetGlobalEnvMap(const char* name);
	const char* GetGlobalEnvMap() const;

	// ��������ĳ˷�ϵ��
	void SetReflectFactor(float value);
	float GetReflectFactor() const;

	// �Ƿ���early-z�Ż�
	void SetEnableEarlyZ(bool value);
	bool GetEnableEarlyZ() const;

	// ������յ���ϵ��
	void SetRoleDiffuseFactor(float value);
	float GetRoleDiffuseFactor() const;
	void SetRoleAmbientFactor(float value);
	float GetRoleAmbientFactor() const;

	// �������������
	void SetEnableRoleFallOff(bool value);
	bool GetEnableRoleFallOff() const;
	void SetRoleFallOffColorString(const char* value);
	result_string GetRoleFallOffColorString() const;
	void SetRoleFallOffInten(float value);
	float GetRoleFallOffInten() const;
	void SetRoleFallOffPower(float value);
	float GetRoleFallOffPower() const;

	// �������Ӷ���
	PERSISTID Create(const char* name);
	// ɾ�����Ӷ���
	bool Delete(const PERSISTID& id);

	// ����Ψһʵ��
	bool SetUnique(const PERSISTID& obj);
	PERSISTID GetUnique(const char* entity_name);

	// ��ӿ��Ӷ���
	bool AddObject(const PERSISTID& obj, int priority);
	// ɾ�����Ӷ���
	bool RemoveObject(const PERSISTID& obj);
	// �����ȼ�ɾ������
	bool RemoveObjectByPrior(int priority);

	// ��Ӷ��󵽲�
	bool LayerAdd(const char* name, const PERSISTID& id);
	// �Ӳ��Ƴ�����
	bool LayerRemove(const char* name, const PERSISTID& id);

	// �Ƿ�Ҫ������̬��Ӱ
	void SetEnableDynamicShadow(bool value);
	bool GetEnableDynamicShadow() const;

	// ��ӰͶӰ�����չ�ٷֱ�
	void SetShadowExpandPercent(float value) ;
	float GetShadowExpandPercent() ;

	inline void SetSceneType(SCENETYPE type){ m_scenetype = type; }

	inline SCENETYPE GetSceneType()const { return m_scenetype; }

	virtual void SetShadowAdjustingRect(FmVec4 vAdjustingRect);
	virtual const FmVec4& GetShadowAdjustingRect();

private:

	// ж����Դ
	bool Unload();
	// ��ʾ��ȾĿ������
	void ShowRenderTaret();
	// ��ÿɼ�����
	IVisBase* GetVisBase(const PERSISTID& id) const;

	// ��Ӱʵ�����
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
