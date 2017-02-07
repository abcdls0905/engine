#ifndef _I_SKY_BOX_H
#define _I_SKY_BOX_H

#include "i_vis_base.h"

class ISkyBox: public IVisBase
{
public:
	virtual bool Load() = 0;

	// �Ƿ�������
	virtual bool GetLoadFinish() = 0;

	// λ��
	virtual bool SetPosition(float x, float y, float z) = 0;
	virtual FmVec3 GetPosition() const = 0;

	// ����
	virtual bool SetAngle(float x, float y, float z) = 0;
	virtual FmVec3 GetAngle() const = 0;

	// �Ƿ�ɼ�
	virtual void SetVisible(bool value) = 0;
	virtual bool GetVisible() const = 0;
	
	// �Ƿ��첽������ͼ
	virtual void SetAsyncLoad(bool value) = 0;
	virtual bool GetAsyncLoad() const = 0;
	
	// �Ƿ�ʹ����Ȳ���
	virtual void SetZEnable(bool value) = 0;
	virtual bool GetZEnable() const = 0;

	// �˷�����
	virtual void SetMulFactor(float value) = 0;
	virtual float GetMulFactor() const = 0;

	// Y����ת�ٶ�
	virtual void SetYawSpeed(float value) = 0;
	virtual float GetYawSpeed() const = 0;

	// ������ܵ���ͼ
	virtual void SetSideTex(const char* value) = 0;
	virtual const char* GetSideTex() const = 0;
	
	// ��ն�����ͼ
	virtual void SetUpTex(const char* value) = 0;
	virtual const char* GetUpTex() const = 0;

	// �����ʱ��
	virtual void SetFadeInTime(float value) = 0;
	virtual float GetFadeInTime() const = 0;

	// �Ƿ񽥱����
	virtual bool GetFadeInFinish() const = 0;

	// ������ͼ
	virtual bool UpdateTexture() = 0;
};

#endif