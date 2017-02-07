//--------------------------------------------------------------------
// �ļ���:		particle_manager.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��9��26��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _PARTICLE_MANAGER_H
#define _PARTICLE_MANAGER_H

#include "../public/core_type.h"
#include "../visual/i_vis_base.h"
#include "../visual/shader_list.h"
#include "../utils/array_pod.h"
#include "../utils/tick_timer.h"
#include "../utils/pod_hash_map.h"  
#include "../visual/i_particle_manager.h"
// ���ӹ�����

class CParticleData;
class CParticleNode;
class CEffectData;
class IRenderContext;

class TParticleShaderAlloc
{
public:
	TParticleShaderAlloc() {}
	~TParticleShaderAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TParticleShaderAlloc& src) {}
};



class ParticleManager: public IParticleManager
{
public:
	enum FLAG_VSH_PARTICLE
	{
		PAR_DEPTHMAP,
		PAR_BILLBOARD_Y,
		PAR_BILLBOARD_NONE,
		PAR_HIGHLIGHT,
		PAR_FOGUSE,
		PAR_FOGLINEAR,
		PAR_FOGEXP,
		PAR_GPU,
		PAR_MAX,
	};

	struct particle_shader_handle
	{
		IShaderProgram*	 pShader;
		fm_int			tex_BaseMapHandle;
		fm_int			mat4ViewProj;
		fm_int			vec4ParticleColorHandle;
		fm_int			vec4FogColorHandle;
		fm_int			vec4FogParamHandle;
		fm_int			vec4FogExpParamHandle;
		fm_int			vec3ViewPostion;
		fm_int			vec3EmitterPos;
		fm_int			vec4RotateAxis;
		fm_int			vec4Time;
		fm_int			vec4Speed;
		fm_int			vec4Force;
		fm_int			vec4UV_Bias;
		fm_int			vec4TrackAlpha;
	};


	ParticleManager();
	virtual ~ParticleManager();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	// ���ó���������
	virtual void SetContext(IRenderContext* value);

	virtual void Update(float seconds);
	virtual void Realize();

	// �Ƿ�ɼ�
	virtual void SetVisible(bool value);
	virtual bool GetVisible() const;

	// ���ӽڵ�����
	int GetParticleNodeCount() const;
	// ������������
	int GetParticleDataCount() const { return (int)m_Datas.size(); }
	// ��Ч��������
	int GetEffectDataCount() const { return(int) m_EffectDatas.size(); }

	// ������ͼ�ļ�·��
	void SetTexturePath(const char* value);
	const char* GetTexturePath() const;

	// �Ƿ��첽����������ͼ
	void SetAsyncLoad(bool value) { m_bAsyncLoad = value; }
	bool GetAsyncLoad() const { return m_bAsyncLoad; }

	// LOD��ʼ����
	void SetLodBegin(float value) { m_fLodBegin = value; }
	float GetLodBegin() const { return m_fLodBegin; }

	// LOD��������
	void SetLodEnd(float value) { m_fLodEnd = value; }
	float GetLodEnd() const { return m_fLodEnd; }

	ParticleManager::particle_shader_handle* GetShaderHandle(size_t shader_flag, bool bnIsGpu=false);
	// ���������ɫ��������ֵ
	IVertexShader* GetParticleVS(size_t shader_flag);
	IPixelShader* GetParticlePS(size_t shader_flag);
	// �������������ɫ��������ֵ
	IVertexShader* GetDistortionVS(size_t shader_flag);
	IPixelShader* GetDistortionPS(size_t shader_flag);


	// ���¼������������ļ���Ϣ
	bool ReloadIni();

	// �������Ӳ�������
	PERSISTID CreateInfo();
	// ���������ļ��������Ӳ�������
	PERSISTID CreateInfoFromIni(const char* ini_name, const char* psname);
	
	// �������Ӳ����������Ӷ���
	PERSISTID Create(const PERSISTID& info_id);
	// ���������ļ��������Ӷ���
	PERSISTID CreateFromIni(const char* ini_name, const char* psname);
	// ɾ�����Ӷ���
	bool Delete(const PERSISTID& id);

	// ��ʱ����Ч���ݵĴ����������ӹ������й�-S-
	// �����Ч����
	CEffectData* CreateEffectData(const char* ini_name, const char* em_name);

	// ���¼���������Ч�����ļ���Ϣ
	bool ReloadEffectIni();

	// �����������ݵ�ʱ��
	void SetCacheTime(float value) { m_fCacheTime = value; }
	float GetCacheTime() const { return m_fCacheTime; }
	// ��ʱ����Ч���ݵĴ����������ӹ������й�-E-

private:
	// �������Ӳ���
	bool FindDataIndex(const char* ini_name, const char* ps_name, 
		size_t& index);
	// ������Ӳ���
	CParticleData* CreateData(const char* ini_name, const char* ps_name);
	// �������ӽڵ�
	CParticleNode* CreateParticle(CParticleData* pData);
	// ɾ�����ӽڵ�
	void DeleteParticle(CParticleNode* pNode);

	// ��ʱ����Ч���ݵĴ����������ӹ������й�-S-
	// ������Ч����
	bool FindEffectDataIndex(const char* ini_name, const char* em_name, 
		size_t& index);

	// ��ʱ���ж��
	bool CheckUnload();
	// ��ʱ����Ч���ݵĴ����������ӹ������й�-E-
private:
	TArrayPod<CParticleData*, 1, TCoreAlloc> m_Datas;
	CParticleNode* m_pFirst;
	CParticleNode* m_pLast;
	IRender*         m_pRender;
	core_string m_strTexturePath;
	bool m_bVisible;
	bool m_bAsyncLoad;
	float m_fLodBegin;
	float m_fLodEnd;
	float m_fWindAngle;
	float m_fWindSpeed;
	float m_fWindPower;
	FmVec3 m_vWindDirection;
	float  m_offset_seconds;

	TShaderList<IVertexShader, PAR_MAX> m_VSList;
	TShaderList<IPixelShader, PAR_MAX> m_PSList;

	TShaderList<IVertexShader, 5> m_DistortionVSList;
	TShaderList<IPixelShader, 5> m_DistortionPSList;

	// ��ʱ����Ч���ݵĴ����������ӹ������й� -S-
	TArrayPod<CEffectData*, 10, TCoreAlloc> m_EffectDatas;
	CTickTimer m_UnloadTimer;
	float m_fCacheTime;
	// ��ʱ����Ч���ݵĴ����������ӹ������й� -E-

	TPodHashMap<unsigned int, ParticleManager::particle_shader_handle*, TPodTraits<unsigned int>, TParticleShaderAlloc> m_ShaderMapHandle;
};

#endif // _PARTICLE_MANAGER_H
