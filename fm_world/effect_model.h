//--------------------------------------------------------------------
// �ļ���:		effect_model.h
// ��  ��:		
// ˵  ��:		
// ��������:	2011��10��18��
// ������:		
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _EFFECT_MODEL_H
#define _EFFECT_MODEL_H

#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "../visual/i_vis_base.h"
#include "../visual/i_render.h"
#include "../visual/i_effect_model.h"

// ��Чģ��

class Model;
class ParticleManager;
class SaberArcManager;
class LightManager;
class CEffectData;

class EffectModel: public IEffectModel
{
	IRender* m_pRender;
private:
	// ����״̬
	enum LOAD_STATE_ENUM
	{
		LOADSTATE_WAIT,
		LOADSTATE_BEGIN,
		LOADSTATE_LOADCACHE,
		LOADSTATE_LOADING,
		LOADSTATE_SUCCEED,
		LOADSTATE_FAILED,
	};

	struct particle_info
	{
		core_string helper_name; // ��������
		core_string particle_name; // ������
		PERSISTID particle_id;		// ����ID
		int open_emit;				// �����ӷ��俪��
		int close_emit;				// �ر����ӷ��俪��
	};

	struct saberarc_info
	{
		core_string helper_name0;	// ������1
		core_string helper_name1;	// ������2
		core_string saberarc_name; // ������
		PERSISTID saberarc_id;		// ����ID
		bool is_saber_start;
	};

	struct light_info_t
	{
		core_string helper_name; // ��������
		core_string light_name; // �ƹ���
		PERSISTID light_id;			// �ƹ�ID
		int open_emit;				// �򿪵ƹⷢ�俪��
		int close_emit;				// �رյƹⷢ�俪��
	};

public:
	EffectModel();
	virtual ~EffectModel();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// ���ó���������
	virtual void SetContext(IRenderContext* value);

	virtual void Execute(float seconds);
	virtual void Update(float seconds);
	virtual void Realize();
	virtual void RealizeShadowMap();

	virtual	void SetWorldMatrix(const FmMat4& mat);
	virtual void GetWorldMatrix(FmMat4& mat);

	// λ��
	virtual bool SetPosition(float x, float y, float z);
	virtual FmVec3 GetPosition() const;

	// ����
	virtual bool SetAngle(float x, float y, float z);
	virtual FmVec3 GetAngle() const;

	// ���ű���
	virtual bool SetScale(float x, float y, float z);
	virtual FmVec3 GetScale() const;

    // Ŀ��λ��
    virtual bool SetTarget(float x, float y, float z);
    virtual FmVec3 GetTarget() const;

	// ȫ�����ĵ�
	virtual FmVec3 GetCenter();
	// ��Χ�гߴ�
	virtual FmVec3 GetBoxSize();
	// �뾶
	virtual float GetRadius();

	// ��ɫ
	void SetColorString(const char* value);
	result_string GetColorString() const;
	virtual void SetColor(unsigned int value);
	virtual unsigned int GetColor() const;

	// �Ƿ�ɼ�
	virtual void SetVisible(bool value);
	virtual bool GetVisible() const;

	// �Ƿ���ˮ��
	virtual void SetUnderWater(bool value);
	virtual bool GetUnderWater() const;

	// ���������Ƿ�ɹ�
	virtual bool GetLoaded();
	// �Ƿ�������
	virtual bool GetLoadFinish();

	// ��¼��ʱ��͸���ȱ�����ֻ������ϵͳʹ�ã�
	virtual void SetAlphaValue(float value) { m_fAlphaValue = value; }
	virtual float GetAlphaValue() const { return m_fAlphaValue; }

	//�Ƿ���ʾ����ģ���ϵ�����
	void SetShowParticle(bool value);
	bool GetShowParticle();

	// �����Χ�гߴ�ʱ�Ƿ�������ӵ�
	void SetCalParticleSize(bool value);
	bool GetCalParticleSize();

	// ��������
	void SetLifeTime(float value);
	float GetLifeTime();

	// �Ƿ��ظ�����
	void SetLoop(bool bLoop);
	bool GetLoop();

	// �Ƿ�һ���Բ�����Ч
	void SetIsOneOff(bool bValue);
	bool GetIsOneOff();

	// �����ٶ�
	void SetSpeed(float fSpeed);
	float GetSpeed();

	// �Ƿ�����ײ
	virtual void SetTraceEnable(bool value);
	virtual bool GetTraceEnable();

	// �Ƿ���Լ���
	virtual void SetCullEnable(bool value);
	virtual bool GetCullEnable();

	// �Ƿ���ײʱʹ�ø�����
	void SetTraceBoxEnable(bool value);
	bool GetTraceBoxEnable();

	// �Ƿ���Ҫˮ�淴��
	virtual void SetWaterReflect(bool value) { m_bWaterReflect = value; }
	virtual bool GetWaterReflect() const { return m_bWaterReflect; }

	// �Ƿ��첽����
	void SetAsyncLoad(bool value);
	bool GetAsyncLoad() const;

	// �Ƿ���ʾ��Χ��
	void SetShowBoundBox(bool value);
	bool GetShowBoundBox() const;

	// ����ģ���ļ�
	void SetModelFile(const char* model_file);
	const char* GetModelFile() const;

	// ����ģ�Ͳ���
	void SetMaterialFile(const char* material_file);
	const char* GetMaterialFile() const;

	// ���������ļ�
	void SetParticleFile(const char* file_name);
	const char* GetParticleFile() const;

	// ������β�ļ�
	void SetSaberArcFile(const char* file_name);
	const char* GetSaberArcFile() const;

	// ���ù�Դ�ļ�
	void SetLightFile(const char* file_name);
	const char* GetLightFile() const;

	// ȡ�õ�ǰʱ��
	float GetCurrenTime();

	// ģ�Ͷ���
	PERSISTID GetModelID() const;
	void SetModelID(const PERSISTID& id);

	// ��Чģ�������ļ���
	const char* GetConfigFile() const;
	// ��Чģ������
	const char* GetName() const;
    void SetName(const char* name);

	// �ӵ�ǰ�������ݴ�����Ч
	bool CreateFromSetting();

	// ͨ�������ļ�����
	bool CreateFromIni(const char* ini_name, const char* em_name, 
		bool async_load);
	bool CreateFromIniEx(const char* ini_name, const char* em_name, 
		bool async_load, const char* append_path);

	// ȡ�ø������б�
	void GetHelperNameList(const IVarList& args, IVarList& result);
	// ȡ�ýڵ����
	void GetCurrentNodeMatrix(int nNodeID, FmMat4* pTM);
	bool GetCurrentNodeMatrix(const char* pNodeName, FmMat4* pTM);
	// ȡ�ýڵ���
	int GetNodeFromName(const char* pszName);

	// ��ײ������ϸ��Ϣ
	virtual bool TraceDetail(const FmVec3& src, const FmVec3& dst,
		trace_info_t& result);
	// ��ײ�����Ƿ����赲
	virtual bool TraceHitted(const FmVec3& src, const FmVec3& dst);
	// ���巶Χ��ײ
    virtual bool TraceSphere(const FmVec3& center, float radius);
    // ��������ײ
    virtual bool TraceTriangle(const FmVec3& v0, const FmVec3& v1, const FmVec3& v2);
	// ��ѡ
	virtual bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context);

	// ������ӣ��ӳٴ������ӣ�
	bool AddParticleParams(const char* particle_name, 
		const char* helper_name, int emit_open, int emit_close);
	// ������ӣ�ֱ�Ӵ������ӣ�
	bool AddParticle(const char* particle_name, const char* helper_name,
		int emit_open, int emit_close);
	// ��������ɾ������
	bool DeleteParticleFromIndex(int index);
	// ɾ�����ӣ�ָ���������ϵ��������ӣ�
	bool DeleteParticleOnHelper(const char* helper_name);
	// ��������ɾ������
	bool DeleteParticleFromName(const char* helper_name, const char* particle_name);
	// ��������ȡ�ø������ϵ�����
	PERSISTID GetParticleFromName(const char* helper_name, 
		const char* particle_name);
	// ��������ȡ�ø������ϵ�����
	PERSISTID GetParticleFromIndex(int index) const;
	// ���������������ӣ��ؽ��رյ����ӣ�
	bool StartParticleFromIndex(size_t index);
	// ���������ر����ӣ�ɾ�����ӵ�������Ϣ��
	bool StopParticleFromIndex(size_t index);
	// ȡ����������
	int GetParticleCount();
	// ���������Ϣ
	void GetParticleInfoFromIndex(const IVarList& args, IVarList& result);
	// �������ӿ�����Ϣ
	bool SetParticleEmit(const char* helper_name, int open_emit, 
		int close_emit);
	// ȡ�����ӿ�����Ϣ
	void GetParticleEmit(const IVarList& args, IVarList& result);

	// ��ӵ������
	bool AddSaberArcParams(const char* saberarc_name, 
		const char* helper_name0, const char* helper_name1, bool is_start);
	// ��ӵ�����Ϣ
	bool AddSaberArc(PERSISTID& id);
	// ɾ������
	bool DeleteSaberArc(int index);
	// ��ȡ����
	PERSISTID GetSaberArc(int index) const;
	// ��ȡ��������
	int GetSaberArcCount();
	// ��õ�����Ϣ
	void GetSaberArcInfo(const IVarList& args, IVarList& result);

	// ��ӹ�Դ�������ӳٴ�����Դ��
	bool AddLightParams(const char* light_name, const char* helper_name, 
		int emit_open, int emit_close);
	// ��ӹ�Դ������ֱ�Ӵ�����Դ��
	bool AddLight(const char* light_name, const char* helper_name, 
		int emit_open, int emit_close);
	// ��������ɾ����Դ
	bool DeleteLightFromIndex(int index);
	// ɾ����Դ��ָ���������ϵ����й�Դ��
	bool DeleteLightOnHelper(const char* helper_name);
	// ��������ɾ����Դ
	bool DeleteLightFromName(const char* helper_name, const char* light_name);
	// ��������ȡ�ø������ϵĹ�Դ
	PERSISTID GetLightFromName(const char* helper_name, const char* light_name);
	// ��������ȡ�ø������ϵĹ�Դ
	PERSISTID GetLightFromIndex(int index) const;
	// ��ȡ��Դ����
	int GetLightCount();
	// ��ù�Դ��Ϣ
	void GetLightInfo(const IVarList& args, IVarList& result);
	// ���ù�Դ������Ϣ
	bool SetLightEmit(const char* helper_name, int open_emit, 
		int close_emit);
	// ȡ�ù�Դ������Ϣ
	void GetLightEmit(const IVarList& args, IVarList& result);

	// ������Чģ�͵��ļ�
	bool SaveEffectModel(const char* file_name, const char* effect_name);

	// ���ò�Ӱ
	bool SetBrokenShadow(bool value);
	PERSISTID GetBrokenShadow() const;

	// ������������Ч����Ч����,ʹ���񱣳ָմ���ʱ��״̬
	bool Clear();

	// ����ڴ�ռ��
	virtual size_t GetMemoryUsage();

    void SetSleep(bool value);

    bool GetSleep() const
    {
        return m_bSleep;
    }

private:
	bool CheckLoad(float seconds);
	// ж����Դ
	bool Unload();

	// ��������
	bool LoadData(const char* ini_name, const char* em_name, bool async_load, 
		const char* append_path);
	// �ӵ�ǰ�������ݴ�������
	bool CreateParticleFromSetting();
	// �ӵ�ǰ�������ݴ�������
	bool CreateSaberArcFromSetting();
	// �ӵ�ǰ�������ݴ�����Դ
	bool CreateLightFromSetting();
	// �ڸ������ϴ�������
	PERSISTID CreateParticleOnHelper(const char* particle_name, 
		const char* helper_name);
	PERSISTID CreateLightOnHelper(const char* light_name, const char* helper_name);
	// �ӳٴ�����Ч
	bool CreateEffect();

	// ������������λ��
	void UpdateLinkParticleEmit();
	// ���¹�Դ����λ��
	void UpdateLinkLightEmit();

	// ���°�Χ��
	void UpdateBoundingBox();

private:
	PERSISTID m_ParticleManagerID;
	PERSISTID m_LightManagerID;
	PERSISTID m_SaberArcManagerID;
	Model* m_pModel;				// ģ�Ͷ���ָ��
	FmMat4 m_mtxWorld;
	TArrayPod<particle_info*, 10, TCoreAlloc> m_ParticleInfo;	// ��������
	TArrayPod<saberarc_info*, 2, TCoreAlloc> m_SaberArcInfo;	// �˶���β
	TArrayPod<light_info_t*, 1, TCoreAlloc> m_LightInfo;		// �ƹ�

	core_string m_strAppendPath;	// ����·��
	core_string m_strModelFile;		// ��Чģ���ļ�
	core_string m_strMaterialFile;	// ��Чģ�Ͳ���
	core_string m_strParticleFile;	// �����ļ���
	core_string m_strSaberArcFile;	// �����ļ���
	core_string m_strLightFile;		// ��Դ�ļ���
	core_string m_strConfigFile;	// �����ļ���
	core_string m_strEffectName;	// ���ö���

	int m_nLoadState;
	bool m_bClipPlaneEnable;
	bool m_bNeedCreate;				// ��Ϊ�첽���ض���Ҫ�ӳٴ������ӻ���β
	bool m_bModelChanged;			// ģ����Դ�Ƿ����仯
	bool m_bVisible;				// ��Чģ���Ƿ�ɼ�
	bool m_bShowParticle;			// �Ƿ���ʾ����ģ���ϵ�����
	bool m_bCalParticleSize;		// �����Χ�гߴ�ʱ�Ƿ�������ӵ�
	bool m_bUpdateNodeMat;			// �󶨵�����Ƿ񱻸��¹�
	bool m_bLoop;					// �Ƿ��ظ�����
	bool m_bIsOneOff;				// �Ƿ�һ���Բ�����Ч
	bool m_bTraceEnable;			// �Ƿ������ײ����
	bool m_bCullEnable;				// �Ƿ���м���
	bool m_bTraceBoxEnable;			// �Ƿ���ײʱʹ�ø�����
	bool m_bWaterReflect;			// �Ƿ���Ҫˮ�淴��
	bool m_bAsyncLoad;				// �Ƿ��첽����
	bool m_bShowBoundBox;
	bool m_bUpdatedBoundingBox;		// �Ƿ��Ѿ����¹���Χ��
    bool m_bSleep;
	float m_fAlphaValue;
	float m_fCurrentTime;			// ��Чģ�͵�ǰ����ʱ��
	float m_fLifeTime;				// ��������
	float m_fSpeed;					// �����ٶ�
	unsigned int m_nColor;
	PERSISTID m_BrokenShadow;		// ��Ӱ����

    FmVec3 m_vTarget;

	FmVec3 m_vBoundMin;
	FmVec3 m_vBoundMax;
	FmVec3 m_vCenter;
	float m_fRadius;
};

#endif // _EFFECT_MODEL_H
