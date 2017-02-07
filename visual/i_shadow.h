#ifndef _I_SHADOW_H_
#define _I_SHADOW_H_

#include "i_vis_base.h"

class IShadow: public IVisBase
{
public:
    //����
	virtual bool Load() = 0;
	// λ��
	virtual bool SetPosition(float x, float y, float z) = 0;
	virtual FmVec3 GetPosition() const = 0;

	// �Ƿ�ɼ�
	virtual void SetVisible(bool value) = 0;
	virtual bool GetVisible() const = 0;

	// �󶨵Ķ���
	virtual void SetBindID(const PERSISTID& value)= 0;
	virtual PERSISTID GetBindID() const= 0;

	// ��Ӱģ����
	virtual void SetLightDispersion(float value)= 0;
	virtual float GetLightDispersion() const= 0;

	// ��Ӱ������ɫ
	virtual void SetShadowTopColorString(const char* value)= 0;
	virtual result_string GetShadowTopColorString() const= 0;

	// ��Ӱ�ײ���ɫ
	virtual void SetShadowBottomColorString(const char* value)= 0;
	virtual result_string GetShadowBottomColorString() const= 0;

	// ʹ�ü�����Ӱ
	virtual void SetUseSimpleShadow(bool value)= 0;
	virtual bool GetUseSimpleShadow() const= 0;
};


#endif