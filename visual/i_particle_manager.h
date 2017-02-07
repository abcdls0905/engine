#ifndef _I_PARTICLE_MANAGER_H
#define _I_PARTICLE_MANAGER_H

#include "../visual/i_vis_base.h"

class IParticleManager: public IVisBase
{
public:
	// ���ӽڵ�����
	virtual int GetParticleNodeCount() const = 0;
	// ������������
	virtual int GetParticleDataCount() const  = 0;
	// ��Ч��������
	virtual int GetEffectDataCount() const  = 0;

	// ������ͼ�ļ�·��
	virtual void SetTexturePath(const char* value) = 0;
	virtual const char* GetTexturePath() const = 0;

	// �Ƿ��첽����������ͼ
	virtual void SetAsyncLoad(bool value)  = 0;
	virtual bool GetAsyncLoad() const  = 0;

	// LOD��ʼ����
	virtual void SetLodBegin(float value)  = 0;
	virtual float GetLodBegin() const  = 0;

	// LOD��������
	virtual void SetLodEnd(float value)  = 0;
	virtual float GetLodEnd() const  = 0;

	// ���¼������������ļ���Ϣ
	virtual bool ReloadIni() = 0;

	// �������Ӳ�������
	virtual PERSISTID CreateInfo() = 0;
	// ���������ļ��������Ӳ�������
	virtual PERSISTID CreateInfoFromIni(const char* ini_name, const char* psname) = 0;
	
	// �������Ӳ����������Ӷ���
	virtual PERSISTID Create(const PERSISTID& info_id) = 0;
	// ���������ļ��������Ӷ���
	virtual PERSISTID CreateFromIni(const char* ini_name, const char* psname) = 0;
	// ɾ�����Ӷ���
	virtual bool Delete(const PERSISTID& id) = 0;

	// ���¼���������Ч�����ļ���Ϣ
	virtual bool ReloadEffectIni() = 0;

	// �����������ݵ�ʱ��
	virtual void SetCacheTime(float value) = 0;
	virtual float GetCacheTime() const = 0;
};
#endif