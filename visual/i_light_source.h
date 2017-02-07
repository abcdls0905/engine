#pragma once

// ��Դ

class ILightSource: public IVisBase
{
public:
	enum LIGHT_TYPE_ENUM
	{
		LIGHT_TYPE_POINT,
		LIGHT_TYPE_SPOT,
		LIGHT_TYPE_BOX,
	};

public:
	// �ƹ�����
	virtual void SetName(const char* value) = 0;
	virtual const char* GetName() const = 0;

	// ��Դ����
	virtual void SetLightType(int value) = 0;
	virtual int GetLightType() const = 0;

	// ��Χ
	virtual void SetRange(float value) = 0;
	virtual float GetRange() const = 0;

	// ��ɫ
	virtual void SetColor(unsigned int value) = 0;
	virtual unsigned int GetColor() const = 0;

	// ����
	virtual void SetIntensity(float value) = 0;
	virtual float GetIntensity() const = 0;

	// ˥��ϵ��0
	virtual void SetAttenu0(float value) = 0;
	virtual float GetAttenu0() const = 0;

	// ˥��ϵ��1
	virtual void SetAttenu1(float value) = 0;
	virtual float GetAttenu1() const = 0;

	// ˥��ϵ��2
	virtual void SetAttenu2(float value) = 0;
	virtual float GetAttenu2() const = 0;

	// �۹���ڽǶ�
	virtual void SetInnerDegree(float value) = 0;
	virtual float GetInnerDegree() const = 0;

	// �۹����Ƕ�
	virtual void SetOuterDegree(float value) = 0;
	virtual float GetOuterDegree() const = 0;

	// �۹���½�ϵ��
	virtual void SetFalloff(float value) = 0;
	virtual float GetFalloff() const = 0;

	// ���ӳߴ�
	virtual const FmVec3& GetBoxScale() const = 0;
	virtual void SetBoxScaleX(float value) = 0;
	virtual float GetBoxScaleX() const = 0;
	virtual void SetBoxScaleY(float value) = 0;
	virtual float GetBoxScaleY() const = 0;
	virtual void SetBoxScaleZ(float value) = 0;
	virtual float GetBoxScaleZ() const = 0;

	// �ƹ���˸ϵ��
	virtual void SetBlink(float value) = 0;
	virtual float GetBlink() const = 0;

	// ��˸����
	virtual void SetBlinkPeriod(float value) = 0;
	virtual float GetBlinkPeriod() const = 0;

	// ��˸ʱ��
	virtual void SetBlinkTick(float value) = 0;
	virtual float GetBlinkTick() const = 0;

	// ��ð󶨶���
	virtual PERSISTID GetBindID() const = 0;
	// ��ø�����
	virtual const char* GetHelperName() const = 0;
	// �Ƿ���Ҫ��ɾ��
	virtual bool GetComplete() const = 0;

	// ��Ӱͼ�ߴ�
	virtual void SetShadowMapSize(int value) = 0;
	virtual int GetShadowMapSize() const = 0;
	
	// �Ƿ�ǿ��ÿ֡��Ҫˢ����Ӱͼ
	virtual void SetShadowMapDynamic(bool value) = 0;
	virtual bool GetShadowMapDynamic() const = 0;

	// �Ƿ���ʾ��Χ��
	virtual void SetShowBoundBox(bool value) = 0;
	virtual bool GetShowBoundBox() const = 0;

	// �Ƿ���ʾ���շ�Χ
	virtual void SetShowRange(bool value) = 0;
	virtual bool GetShowRange() const = 0;

	// �Ƿ��첽����
	virtual void SetAsyncLoad(bool value) = 0;
	virtual bool GetAsyncLoad() const = 0;

	// ��ǰ�ڵƹⷶΧ�Ļ��������
	virtual void SetActiveRoles(int value) = 0;
	virtual int GetActiveRoles() const = 0;

	// ��õ�ǰ������ֵ
	virtual float GetCurIntensity() const = 0;

	// ��Ӱͼ
	virtual void ClearShadowTex() = 0;
	virtual void SetShadowTex(int index, IDepthRT* value) = 0;
	virtual IDepthRT* GetShadowTex(int index) const = 0;

	// ��ӰͼͶӰ����
	virtual void SetShadowViewMatrix(int index, const FmMat4& value) = 0;
	virtual const FmMat4& GetShadowViewMatrix(int index) const = 0;
	virtual void SetShadowProjMatrix(int index, const FmMat4& value) = 0;
	virtual const FmMat4& GetShadowProjMatrix(int index) const = 0;

	// ��õ�����ͼ
 	virtual ITexture* GetOverlayTex() const = 0;

	// ��Դ����
	virtual void SetLightTypeString(const char* value) = 0;
	virtual result_string GetLightTypeString() const = 0;
	
	// ��ɫ
	virtual void SetColorString(const char* value) = 0;
	virtual result_string GetColorString() const = 0;

	// ��ͼ
	virtual void SetTexture(const char* value) = 0;
	virtual const char* GetTexture() const = 0;

	// �Ƿ�����Ұ֮��
	virtual bool IsViewClip() = 0;
	// ���ӵ���������
	virtual bool LinkToObject(const PERSISTID& id, float pos_x, float pos_y, 
		float pos_z) = 0;
	virtual  bool LinkToPoint(const PERSISTID& id, const char* helper_name) = 0;

};