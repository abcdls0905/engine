#ifndef _I_PARTICLE_H
#define _I_PARTICLE_H

#include "../public/core_type.h"
#include "../visual/i_vis_base.h"

class IParticle: public IVisBase
{
public:
	
	// �Ƿ�������
	virtual bool GetLoadFinish() = 0;

	// λ��
	virtual bool SetPosition(float x, float y, float z) = 0;
	virtual FmVec3 GetPosition() const = 0;

	// ����
	virtual bool SetAngle(float x, float y, float z) = 0;
	virtual FmVec3 GetAngle() const = 0;

	// ����
	virtual bool SetScale(float x, float y, float z) = 0;
	virtual FmVec3 GetScale() const = 0;

	// ���ĵ�λ��
	virtual FmVec3 GetCenter() = 0;
	// ��Χ�гߴ�
	virtual FmVec3 GetBoxSize() = 0;
	// �뾶
	virtual float GetRadius() = 0;
	
	// ��ײ������ϸ��Ϣ
	virtual bool TraceDetail(const FmVec3& src, const FmVec3& dst, 
		trace_info_t& result) = 0;

	// �Ƿ�ɼ�
	virtual void SetVisible(bool value) = 0;
	virtual bool GetVisible() const = 0;
	
	// �Ƿ�����ײ
	virtual void SetTraceEnable(bool value) = 0;
	virtual bool GetTraceEnable() const = 0;

	// �Ƿ���ˮ��
	virtual void SetUnderWater(bool value) = 0;
	virtual bool GetUnderWater() const = 0;

	// ������ɫ
	virtual void SetColor(unsigned int value) = 0;
	virtual unsigned int GetColor() const = 0;

	// ��¼��ʱ��͸���ȱ�����ֻ������ϵͳʹ�ã�
	virtual void SetAlphaValue(float value)  = 0;
	virtual float GetAlphaValue() const = 0;

	// ���ӵ���������
	virtual bool LinkToObject(const PERSISTID& id, float pos_x, float pos_y, 
		float pos_z) = 0;
	// ���ӵ���������ĸ�������
	virtual bool LinkToObjectPoint(const PERSISTID& id, const char* helper_name) = 0;
};

#endif
