#ifndef _I_EFFECT_MODEL_H
#define _I_EFFECT_MODEL_H

#include "i_vis_base.h"

class IEffectModel: public IVisBase
{
public:
	virtual	void SetWorldMatrix(const FmMat4& mat) = 0;
	virtual void GetWorldMatrix(FmMat4& mat) = 0;

	// λ��
	virtual bool SetPosition(float x, float y, float z) = 0;
	virtual FmVec3 GetPosition() const = 0;

	// ����
	virtual bool SetAngle(float x, float y, float z) = 0;
	virtual FmVec3 GetAngle() const = 0;

	// ���ű���
	virtual bool SetScale(float x, float y, float z) = 0;
	virtual FmVec3 GetScale() const = 0;

	// ȫ�����ĵ�
	virtual FmVec3 GetCenter() = 0;
	// ��Χ�гߴ�
	virtual FmVec3 GetBoxSize() = 0;
	// �뾶
	virtual float GetRadius() = 0;

	// ��ɫ
	virtual void SetColorString(const char* value) = 0;
	virtual result_string GetColorString() const = 0;
	virtual void SetColor(unsigned int value) = 0;
	virtual unsigned int GetColor() const = 0;

	// �Ƿ�ɼ�
	virtual void SetVisible(bool value) = 0;
	virtual bool GetVisible() const = 0;

	// �Ƿ���ˮ��
	virtual void SetUnderWater(bool value) = 0;
	virtual bool GetUnderWater() const = 0;

	// ���������Ƿ�ɹ�
	virtual bool GetLoaded() = 0;
	// �Ƿ�������
	virtual bool GetLoadFinish() = 0;

	// ��¼��ʱ��͸���ȱ�����ֻ������ϵͳʹ�ã�
	virtual void SetAlphaValue(float value)  = 0;
	virtual float GetAlphaValue() const  = 0;

	//�Ƿ���ʾ����ģ���ϵ�����
	virtual void SetShowParticle(bool value) = 0;
	virtual bool GetShowParticle() = 0;

	// �����Χ�гߴ�ʱ�Ƿ�������ӵ�
	virtual void SetCalParticleSize(bool value) = 0;
	virtual bool GetCalParticleSize() = 0;

	// ��������
	virtual void SetLifeTime(float value) = 0;
	virtual float GetLifeTime() = 0;

	// �Ƿ��ظ�����
	virtual void SetLoop(bool bLoop) = 0;
	virtual bool GetLoop() = 0;

	// �Ƿ�һ���Բ�����Ч
	virtual void SetIsOneOff(bool bValue) = 0;
	virtual bool GetIsOneOff() = 0;

	// �����ٶ�
	virtual void SetSpeed(float fSpeed) = 0;
	virtual float GetSpeed() = 0;

	// �Ƿ�����ײ
	virtual void SetTraceEnable(bool value) = 0;
	virtual bool GetTraceEnable() = 0;

	// �Ƿ���Լ���
	virtual void SetCullEnable(bool value) = 0;
	virtual bool GetCullEnable() = 0;

	// �Ƿ���ײʱʹ�ø�����
	virtual void SetTraceBoxEnable(bool value) = 0;
	virtual bool GetTraceBoxEnable() = 0;

	// �Ƿ���Ҫˮ�淴��
	virtual void SetWaterReflect(bool value) = 0;
	virtual bool GetWaterReflect() const = 0;

	// �Ƿ��첽����
	virtual void SetAsyncLoad(bool value) = 0;
	virtual bool GetAsyncLoad() const = 0;

	// �Ƿ���ʾ��Χ��
	virtual void SetShowBoundBox(bool value) = 0;
	virtual bool GetShowBoundBox() const = 0;

	// ����ģ���ļ�
	virtual void SetModelFile(const char* model_file) = 0;
	virtual const char* GetModelFile() const = 0;

	// ����ģ�Ͳ���
	virtual void SetMaterialFile(const char* material_file) = 0;
	virtual const char* GetMaterialFile() const = 0;

	// ���������ļ�
	virtual void SetParticleFile(const char* file_name) = 0;
	virtual const char* GetParticleFile() const = 0;

	// ������β�ļ�
	virtual void SetSaberArcFile(const char* file_name) = 0;
	virtual const char* GetSaberArcFile() const = 0;

	// ���ù�Դ�ļ�
	virtual void SetLightFile(const char* file_name) = 0;
	virtual const char* GetLightFile() const = 0;

	// ȡ�õ�ǰʱ��
	virtual float GetCurrenTime() = 0;

	// ģ�Ͷ���
	virtual PERSISTID GetModelID() const = 0;
	virtual void SetModelID(const PERSISTID& id) = 0;

	// ��Чģ�������ļ���
	virtual const char* GetConfigFile() const = 0;
	// ��Чģ������
	virtual const char* GetName() const = 0;
    virtual void SetName(const char* name) = 0;

	// �ӵ�ǰ�������ݴ�����Ч
	virtual bool CreateFromSetting() = 0;

	// ͨ�������ļ�����
	virtual bool CreateFromIni(const char* ini_name, const char* em_name, 
		bool async_load) = 0;
	virtual bool CreateFromIniEx(const char* ini_name, const char* em_name, 
		bool async_load, const char* append_path) = 0;

	// ȡ�ø������б�
	virtual void GetHelperNameList(const IVarList& args, IVarList& result) = 0;
	// ȡ�ýڵ����
	virtual void GetCurrentNodeMatrix(int nNodeID, FmMat4* pTM) = 0;
	virtual bool GetCurrentNodeMatrix(const char* pNodeName, FmMat4* pTM) = 0;
	// ȡ�ýڵ���
	virtual int GetNodeFromName(const char* pszName) = 0;

	// ��ײ������ϸ��Ϣ
	virtual bool TraceDetail(const FmVec3& src, const FmVec3& dst,
		trace_info_t& result) = 0;
	// ��ײ�����Ƿ����赲
	virtual bool TraceHitted(const FmVec3& src, const FmVec3& dst) = 0;
	// ���巶Χ��ײ
	virtual bool TraceSphere(const FmVec3& center, float radius) = 0;
	// ��ѡ
	virtual bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context) = 0;

	// ������ӣ��ӳٴ������ӣ�
	virtual bool AddParticleParams(const char* particle_name, 
		const char* helper_name, int emit_open, int emit_close) = 0;
	// ������ӣ�ֱ�Ӵ������ӣ�
	virtual bool AddParticle(const char* particle_name, const char* helper_name,
		int emit_open, int emit_close) = 0;
	// ��������ɾ������
	virtual bool DeleteParticleFromIndex(int index) = 0;
	// ɾ�����ӣ�ָ���������ϵ��������ӣ�
	virtual bool DeleteParticleOnHelper(const char* helper_name) = 0;
	// ��������ɾ������
	virtual bool DeleteParticleFromName(const char* helper_name, const char* particle_name) = 0;
	// ��������ȡ�ø������ϵ�����
	virtual PERSISTID GetParticleFromName(const char* helper_name, 
		const char* particle_name) = 0;
	// ��������ȡ�ø������ϵ�����
	virtual PERSISTID GetParticleFromIndex(int index) const = 0;
	// ���������������ӣ��ؽ��رյ����ӣ�
	virtual bool StartParticleFromIndex(size_t index) = 0;
	// ���������ر����ӣ�ɾ�����ӵ�������Ϣ��
	virtual bool StopParticleFromIndex(size_t index) = 0;
	// ȡ����������
	virtual int GetParticleCount() = 0;
	// ���������Ϣ
	virtual void GetParticleInfoFromIndex(const IVarList& args, IVarList& result) = 0;
	// �������ӿ�����Ϣ
	virtual bool SetParticleEmit(const char* helper_name, int open_emit, 
		int close_emit) = 0;
	// ȡ�����ӿ�����Ϣ
	virtual void GetParticleEmit(const IVarList& args, IVarList& result) = 0;

	// ��ӵ������
	virtual bool AddSaberArcParams(const char* saberarc_name, 
		const char* helper_name0, const char* helper_name1, bool is_start) = 0;
	// ��ӵ�����Ϣ
	virtual bool AddSaberArc(PERSISTID& id) = 0;
	// ɾ������
	virtual bool DeleteSaberArc(int index) = 0;
	// ��ȡ����
	virtual PERSISTID GetSaberArc(int index) const = 0;
	// ��ȡ��������
	virtual int GetSaberArcCount() = 0;
	// ��õ�����Ϣ
	virtual void GetSaberArcInfo(const IVarList& args, IVarList& result) = 0;

	// ��ӹ�Դ�������ӳٴ�����Դ��
	virtual bool AddLightParams(const char* light_name, const char* helper_name, 
		int emit_open, int emit_close) = 0;
	// ��ӹ�Դ������ֱ�Ӵ�����Դ��
	virtual bool AddLight(const char* light_name, const char* helper_name, 
		int emit_open, int emit_close) = 0;
	// ��������ɾ����Դ
	virtual bool DeleteLightFromIndex(int index) = 0;
	// ɾ����Դ��ָ���������ϵ����й�Դ��
	virtual bool DeleteLightOnHelper(const char* helper_name) = 0;
	// ��������ɾ����Դ
	virtual bool DeleteLightFromName(const char* helper_name, const char* light_name) = 0;
	// ��������ȡ�ø������ϵĹ�Դ
	virtual PERSISTID GetLightFromName(const char* helper_name, const char* light_name) = 0;
	// ��������ȡ�ø������ϵĹ�Դ
	virtual PERSISTID GetLightFromIndex(int index) const = 0;
	// ��ȡ��Դ����
	virtual int GetLightCount() = 0;
	// ��ù�Դ��Ϣ
	virtual void GetLightInfo(const IVarList& args, IVarList& result) = 0;
	// ���ù�Դ������Ϣ
	virtual bool SetLightEmit(const char* helper_name, int open_emit, 
		int close_emit) = 0;
	// ȡ�ù�Դ������Ϣ
	virtual void GetLightEmit(const IVarList& args, IVarList& result) = 0;

	// ������Чģ�͵��ļ�
	virtual bool SaveEffectModel(const char* file_name, const char* effect_name) = 0;

	// ���ò�Ӱ
	virtual bool SetBrokenShadow(bool value) = 0;
	virtual PERSISTID GetBrokenShadow() const = 0;

	// ������������Ч����Ч����,ʹ���񱣳ָմ���ʱ��״̬
	virtual bool Clear() = 0;

	// ����ڴ�ռ��
	virtual size_t GetMemoryUsage() = 0;

    // ����,����Ԥ���صĶ���,��������Ϊ����״̬,������֡����
    virtual void SetSleep(bool value) = 0;
    virtual bool GetSleep() const = 0;

    // Ŀ��λ��
    virtual bool SetTarget(float x, float y, float z) = 0;
    virtual FmVec3 GetTarget() const = 0;
};

#endif
