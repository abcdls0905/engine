//--------------------------------------------------------------------
// �ļ���:		particle.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��9��27��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _PARTICLE_H
#define _PARTICLE_H

#include "../public/core_type.h"
#include "../visual/i_vis_base.h"
#include "../visual/i_particle.h"
// ����

class CParticleNode;

class Particle: public IParticle
{
public:
	Particle();
	virtual ~Particle();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void Realize();
	
	// �Ƿ�������
	virtual bool GetLoadFinish(); 

	// λ��
	virtual bool SetPosition(float x, float y, float z);
	virtual FmVec3 GetPosition() const;

	// ����
	virtual bool SetAngle(float x, float y, float z);
	virtual FmVec3 GetAngle() const;

	// ����
	virtual bool SetScale(float x, float y, float z);
	virtual FmVec3 GetScale() const;

	// ���ĵ�λ��
	virtual FmVec3 GetCenter();
	// ��Χ�гߴ�
	virtual FmVec3 GetBoxSize();
	// �뾶
	virtual float GetRadius();
	
	// ��ײ������ϸ��Ϣ
	virtual bool TraceDetail(const FmVec3& src, const FmVec3& dst, 
		trace_info_t& result);

	// �Ƿ�ɼ�
	virtual void SetVisible(bool value);
	virtual bool GetVisible() const;
	
	// �Ƿ�����ײ
	virtual void SetTraceEnable(bool value) { m_bTraceEnable = value; }
	virtual bool GetTraceEnable() const { return m_bTraceEnable; }

	// �Ƿ���ˮ��
	virtual void SetUnderWater(bool value);
	virtual bool GetUnderWater() const;

	// ������ɫ
	virtual void SetColor(unsigned int value);
	virtual unsigned int GetColor() const;

	// ��¼��ʱ��͸���ȱ�����ֻ������ϵͳʹ�ã�
	virtual void SetAlphaValue(float value) { m_fAlphaValue = value; }
	virtual float GetAlphaValue() const { return m_fAlphaValue; }

	// ���ӽڵ�
	void SetParticleNode(CParticleNode* value);
	CParticleNode* GetParticleNode() const { return m_pNode; }

	// ���Ӳ�������
	void SetInfoID(const PERSISTID& value) { m_InfoID = value; }
	PERSISTID GetInfoID() const { return m_InfoID; }

	// ��������
	const char* GetName() const;
	// ���������ļ�
	const char* GetConfig() const;
	// �󶨵Ķ���
	PERSISTID GetBindID() const;

	// �Ƿ�ˮ�沨��
	bool GetWaterWave() const;

	// ������ʱ
	void SetDelay(float value);
	float GetDelay() const;

	// ���ӷ��俪��
	void SetEmitSwitch(bool value);
	bool GetEmitSwitch() const;
	
	// ����ʱ��
	void SetLifeTime(float value);
	float GetLifeTime() const;

	// �����ٶȵ�������
	void SetSpeedFactor(float value);
	float GetSpeedFactor() const;

	// LOD��������
	void SetLodEnd(float value);
	float GetLodEnd() const;

	// �Ƿ��ظ�����
	void SetRepeat(bool value);
	bool GetRepeat() const;

	// �Ƿ�̬���°�Χ��
	void SetUpdateBoundBox(bool value);
	bool GetUpdateBoundBox() const;

	// �Ƿ���ʾ��Χ��
	void SetShowBoundBox(bool value) { m_bShowBoundBox = value; }
	bool GetShowBoundBox() const { return m_bShowBoundBox; }

	// ͸����
	void SetAlpha(unsigned int value);
	unsigned int GetAlpha() const;

	// ������ɫ
	void SetColorString(const char* value);
	result_string GetColorString() const;

	// �ü��濪��
	void SetClipPlaneEnable(bool value);
	bool GetClipPlaneEnable() const;

	// ���òü���
	bool SetClipPlane(float x, float y, float z, float w);

	// ֹͣ
	bool Stop();
	// ��ͣ
	bool Pause();
	// �ָ�
	bool Resume();
	
	// ���ӵ���������
	bool LinkToObject(const PERSISTID& id, float pos_x, float pos_y, 
		float pos_z);
	// ���ӵ���������ĸ�������
	bool LinkToObjectPoint(const PERSISTID& id, const char* helper_name);

private:
	core_string m_strName;
	CParticleNode* m_pNode;
	PERSISTID m_InfoID;
	bool m_bShowBoundBox;
	bool m_bTraceEnable;
	float m_fAlphaValue;
};

#endif // _PARTICLE_H
