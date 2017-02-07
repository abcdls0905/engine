//--------------------------------------------------------------------
// �ļ���:		effect_model.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2011��10��18��
// ������:		
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "effect_model.h"
#include "model.h"
#include "particle_manager.h"
#include "particle.h"
#include "saber_arc_manager.h"
#include "saber_arc.h"
#include "effect_data.h"
#include "light_manager.h"
#include "light_source.h"
#include "broken_shadow.h"
#include "actor.h"
#include "../visual/vis_utils.h"
#include "../public/i_ini_file.h"
#include "../public/var.h"
#include "../public/var_list.h"
#include "../public/inlines.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../visual/i_terrain.h"
//#include "../public/valid.h"

/// \file effect_model.cpp
/// \brief ��Чģ��

/// entity: EffectModel
/// \brief ��Чģ��ʵ��
/// \see �̳���IVisBase
DECLARE_ENTITY(EffectModel, 0, IVisBase);

/// property: bool Visible
/// \brief �Ƿ�ɼ�
DECLARE_PROPERTY(bool, EffectModel, Visible, GetVisible, SetVisible);
/// property: string Color
/// \brief ��ɫ
DECLARE_PROPERTY(result_string, EffectModel, Color, GetColorString, 
	SetColorString);
/// readonly: const char* Config
/// \brief ��Чģ�������ļ���
DECLARE_PROPERTY_GET(const char*, EffectModel, Config, GetConfigFile);
/// readonly: const char* Name
/// \brief ��Чģ������
DECLARE_PROPERTY(const char*, EffectModel, Name, GetName, SetName);
/// readonly: PERSISTID ModelID
/// \brief ģ��ID
DECLARE_PROPERTY(PERSISTID, EffectModel, ModelID, GetModelID, SetModelID);
/// property: bool ShowParticle
/// \brief �Ƿ���ʾ����
DECLARE_PROPERTY(bool, EffectModel, ShowParticle, GetShowParticle, 
	SetShowParticle);
/// property: bool CalParticleSize
/// \brief �����Χ�гߴ�ʱ�Ƿ�������ӵ�
DECLARE_PROPERTY(bool, EffectModel, CalParticleSize, GetCalParticleSize, 
	SetCalParticleSize);
/// property: float LifeTime
/// \brief ��������
DECLARE_PROPERTY(float, EffectModel, LifeTime, GetLifeTime, SetLifeTime);
/// property: bool Loop
/// \brief �ظ�����
DECLARE_PROPERTY(bool, EffectModel, Loop, GetLoop, SetLoop);
/// property: bool IsOneOff
/// \brief �Ƿ�һ���Բ�����Ч
DECLARE_PROPERTY(bool, EffectModel, IsOneOff, GetIsOneOff, SetIsOneOff);
/// property: float Speed
/// \brief �����ٶ�
DECLARE_PROPERTY(float, EffectModel, Speed, GetSpeed, SetSpeed);
/// property: bool TraceEnable
/// \brief �Ƿ������ײ���
DECLARE_PROPERTY(bool, EffectModel, TraceEnable, GetTraceEnable, 
	SetTraceEnable);
/// property: bool CullEnable
/// \brief �Ƿ���Լ���
DECLARE_PROPERTY(bool, EffectModel, CullEnable, GetCullEnable, SetCullEnable);
/// property: bool TraceBoxEnable
/// \brief �Ƿ���ײʱʹ�ø�����
DECLARE_PROPERTY(bool, EffectModel, TraceBoxEnable, GetTraceBoxEnable, 
	SetTraceBoxEnable);
/// property: bool WaterReflect
/// \brief �Ƿ���Ҫˮ�淴��
DECLARE_PROPERTY(bool, EffectModel, WaterReflect, GetWaterReflect, 
	SetWaterReflect);
/// property: bool AsyncLoad
/// \brief �Ƿ��첽����
DECLARE_PROPERTY(bool, EffectModel, AsyncLoad, GetAsyncLoad, SetAsyncLoad);
/// property: bool ShowBoundBox
/// \brief �Ƿ���ʾ��Χ��
DECLARE_PROPERTY(bool, EffectModel, ShowBoundBox, GetShowBoundBox, 
	SetShowBoundBox);

/// property: string ModelFile
/// \brief ��Чģ���ļ�
DECLARE_PROPERTY(const char*, EffectModel, ModelFile, GetModelFile, 
	SetModelFile);

/// property: string MaterialFile
/// \brief ��Чģ�Ͳ���
DECLARE_PROPERTY(const char*, EffectModel, MaterialFile, GetMaterialFile, 
	SetMaterialFile);

/// property: string ParticleFile
/// \brief �����ļ���
DECLARE_PROPERTY(const char*, EffectModel, ParticleFile, GetParticleFile, 
	SetParticleFile);
/// method: bool AddParticleParams(string particle_name, string helper_name,
///		int emit_open, int emit_close)
/// \brief ����Чģ����������Ӳ���������ͬ������
/// \param particle_name ������
/// \param helper_name ��������
/// \param emit_open ���ӿ��ش򿪵����
/// \param emit_close ���ӿ��عرյ����
/// \return �ɹ�����true,ʧ�ܷ���false
//DECLARE_METHOD_4(bool, EffectModel, AddParticleParams, const char*, 
//	const char*, int, int);
/// method: bool AddParticle(string particle_name, string helper_name,
///		int emit_open, int emit_close)
/// \brief ����Чģ����������Ӳ�������ͬʱ����
/// \param particle_name ������
/// \param helper_name ��������
/// \param emit_open ���ӿ��ش򿪵����
/// \param emit_close ���ӿ��عرյ����
/// \return �ɹ�����true,ʧ�ܷ���false
//DECLARE_METHOD_4(bool, EffectModel, AddParticle, const char*, const char*, 
//	int, int);
/// method: bool DeleteParticleFromIndex(int index);
/// \brief ��������ɾ������
/// \param index ����
/// \return �ɹ�����true,ʧ�ܷ���false
//DECLARE_METHOD_1(bool, EffectModel, DeleteParticleFromIndex, int);
/// method: bool DeleteParticleOnHelper(string helper_name);
/// \brief ɾ��ָ���������ϵ���������
/// \param helper_name ��������
/// \return �ɹ�����true,ʧ�ܷ���false
//DECLARE_METHOD_1(bool, EffectModel, DeleteParticleOnHelper, const char*);
/// method: bool DeleteParticleFromName(string helper_name, string particle_name);
/// \brief ��������ɾ������
/// \param helper_name ��������
/// \param particle_name ������
/// \return �ɹ�����true,ʧ�ܷ���false
//DECLARE_METHOD_2(bool, EffectModel, DeleteParticleFromName, const char*, const char*);
/// method: PERSISTID GetParticleFromName(string helper_name, string particle_name)
/// \brief ��������ȡ�ø������ϵ�����ID
/// \param helper_name ��������
/// \param particle_name ������
/// \return ����ID
//DECLARE_METHOD_2(PERSISTID, EffectModel, GetParticleFromName, const char*, const char*);
/// method: PERSISTID GetParticleFromIndex(int index)
/// \brief ��������ȡ�ø������ϵ�����ID
/// \param index ����
/// \return ����ID
//DECLARE_METHOD_1(PERSISTID, EffectModel, GetParticleFromIndex, int);
/// readonly: int ParticleCount
/// \brief ȡ����������
DECLARE_PROPERTY_GET(int, EffectModel, ParticleCount, GetParticleCount);
/// method: [helper_name, particle_name] GetParticleInfoFromIndex(size_t index)
/// \brief ȡ��������Ϣ
//DECLARE_METHOD_N(EffectModel, GetParticleInfoFromIndex);
/// method: bool SetParticleEmit(string helper_name, int open_emit, 
///		int close_emit)
/// \brief �������ӿ�ʼ������ֹͣ���������
/// \param helper_name ��������
/// \param open_emit ��ʼ��������
/// \param close_emit ֹͣ��������
/// \return �ɹ�����true,ʧ�ܷ���false
//DECLARE_METHOD_3(bool, EffectModel, SetParticleEmit, const char*, int, int);
/// method: [open_emit, open_emit] GetParticleEmit(string helper_name)
/// \brief ȡ�����ӿ�ʼ������ֹͣ���������
//DECLARE_METHOD_N(EffectModel, GetParticleEmit);

/// property: string SaberArcFile
/// \brief ��β�ļ���
DECLARE_PROPERTY(const char*, EffectModel, SaberArcFile, GetSaberArcFile, 
	SetSaberArcFile);
/// method: bool AddSaberArcParams(string saberarc_name, string helper_name0, 
///		string helper_name1, bool is_start)
/// \brief ��ӵ������
/// \param saberarc_name �������ö���
/// \param helper_name0 ָ�����⸨���������
/// \param helper_name1 ָ�����⸨�����յ���
/// \param is_start ָ�������Ƿ�ʼ
/// \return ��ӳɹ�����true,ʧ�ܷ���false
//DECLARE_METHOD_4(bool, EffectModel, AddSaberArcParams, const char*, 
//	const char*, const char*, bool);
/// method: bool AddSaberArc(PERSISTID id)
/// \brief ��ӵ���
/// \param id ����ID
/// \return ��ӳɹ�����true,ʧ�ܷ���false
//DECLARE_METHOD_1(bool, EffectModel, AddSaberArc, PERSISTID);
/// method: bool DeleteSaberArc(int index)
/// \brief ɾ������
/// \param index ��������
/// \return ɾ���ɹ�����true,ʧ�ܷ���false
///DECLARE_METHOD_1(bool, EffectModel, DeleteSaberArc, int);
/// method: PERSISTID GetSaberArc(int index)
/// \brief ��ȡ����
/// \param index �������
/// \return �ɹ����ص���ID
//DECLARE_METHOD_1(PERSISTID, EffectModel, GetSaberArc, int);
/// readonly: int SaberArcCount
/// \brief ��������
DECLARE_PROPERTY_GET(int, EffectModel, SaberArcCount, GetSaberArcCount);
/// method: [helper_name0, helper_name1, saberarc_name] GetSaberArcInfo(PERSISTID id)
/// \brief ��õ�����Ϣ
//DECLARE_METHOD_N(EffectModel, GetSaberArcInfo);

/// property: string LightFile
/// \brief ��Դ�ļ���
DECLARE_PROPERTY(const char*, EffectModel, LightFile, GetLightFile, 
	SetLightFile);
/// method: bool AddLightParams(string light_name, string helper_name)
/// \brief ����Чģ������ӹ�Դ����������ͬ������
/// \param light_name ��Դ���ö���
/// \param helper_name ��������
/// \param emit_open ��Դ���ش򿪵����
/// \param emit_close ��Դ���عرյ����
/// \return ��ӳɹ�����true,ʧ�ܷ���false
//DECLARE_METHOD_4(bool, EffectModel, AddLightParams, const char*, const char*, 
//	int, int);
/// method: bool AddLight(string light_name, string helper_name)
/// \brief ����Чģ������ӹ�Դ��������ͬʱ����
/// \param light_name ��Դ���ö���
/// \param helper_name ��������
/// \param emit_open ��Դ���ش򿪵����
/// \param emit_close ��Դ���عرյ����
/// \return ��ӳɹ�����true,ʧ�ܷ���false
//DECLARE_METHOD_4(bool, EffectModel, AddLight, const char*, const char*, int, 
//	int);
/// method: bool DeleteLightFromIndex(int index)
/// \brief ɾ����Դ
/// \param index ��Դ����
/// \return ɾ���ɹ�����true,ʧ�ܷ���false
//DECLARE_METHOD_1(bool, EffectModel, DeleteLightFromIndex, int);
/// method: bool DeleteLightOnHelper(string helper_name);
/// \brief ɾ��ָ���������ϵ����й�Դ
/// \param helper_name ��������
/// \return �ɹ�����true,ʧ�ܷ���false
//DECLARE_METHOD_1(bool, EffectModel, DeleteLightOnHelper, const char*);
/// method: bool DeleteLightFromName(string helper_name, string light_name);
/// \brief ��������ɾ����Դ
/// \param helper_name ��������
/// \param light_name ��Դ��
/// \return �ɹ�����true,ʧ�ܷ���false
//DECLARE_METHOD_2(bool, EffectModel, DeleteLightFromName, const char*, const char*);
/// method: PERSISTID GetLightFromName(int index)
/// \brief ��������ȡ�ø������ϵĹ�Դ
/// \param helper_name ��������
/// \param light_name ��Դ��
/// \return �ɹ����ع�ԴID
//DECLARE_METHOD_2(PERSISTID, EffectModel, GetLightFromName, const char*, const char*);
/// method: PERSISTID GetLightFromIndex(int index)
/// \brief ��ȡ��Դ
/// \param index ��Դ���
/// \return �ɹ����ع�ԴID
//DECLARE_METHOD_1(PERSISTID, EffectModel, GetLightFromIndex, int);
/// readonly: int LightCount
/// \brief ��Դ����
DECLARE_PROPERTY_GET(int, EffectModel, LightCount, GetLightCount);
/// method: [helper_name, light_name] GetLightInfo(PERSISTID id)
/// \brief ��ù�Դ��Ϣ
//DECLARE_METHOD_N(EffectModel, GetLightInfo);
/// method: bool SetLightEmit(string helper_name, int open_emit, 
///		int close_emit)
/// \brief ���ù�Դ��ʼ��ֹͣ������
/// \param helper_name ��������
/// \param open_emit ��ʼ����
/// \param close_emit ֹͣ����
/// \return �ɹ�����true,ʧ�ܷ���false
//DECLARE_METHOD_3(bool, EffectModel, SetLightEmit, const char*, int, int);
/// method: [open_emit, open_emit] GetParticleEmit(string helper_name)
/// \brief ȡ�ù�Դ��ʼ��ֹͣ������
//DECLARE_METHOD_N(EffectModel, GetLightEmit);

/// method: bool SetBrokenShadow(bool value)
/// \brief ���ò�Ӱ����
/// \param value ��Ӱ���ر�־
/// \return ���óɹ�����true,ʧ�ܷ���false
//DECLARE_METHOD_1(bool, EffectModel, SetBrokenShadow, bool);
/// method: PERSISTID GetBrokenShadow()
/// \brief ȡ�ò�Ӱ����ID
/// \return ���ز�ӰID
//DECLARE_METHOD_0(PERSISTID, EffectModel, GetBrokenShadow);

/// method: bool CreateFromSetting()
/// \brief ͨ���ڲ����ò���������Чģ��
/// \return ���سɹ�����true,ʧ�ܷ���false
//DECLARE_METHOD_0(bool, EffectModel, CreateFromSetting);

/// method: bool CreateFromIni(string ini_name, string em_name, 
///		bool async_load)
/// \brief ͨ�������ļ�������Чģ��
/// \param ini_name �����ļ���
/// \param em_name ��Чģ�����ö���
/// \param async_load �첽����
/// \return ���سɹ�����true,ʧ�ܷ���false
//DECLARE_METHOD_3(bool, EffectModel, CreateFromIni, const char*, const char*, 
//	bool);
/// method: bool CreateFromIniEx(string ini_name, string em_name, 
///		bool async_load, const char* append_path)
/// \brief ͨ�������ļ�������Чģ��
/// \param ini_name �����ļ���
/// \param em_name ��Чģ�����ö���
/// \param async_load �첽����
/// \param append_path ����·����
/// \return ���سɹ�����true,ʧ�ܷ���false
//DECLARE_METHOD_4(bool, EffectModel, CreateFromIniEx, const char*, const char*, 
//	bool, const char*);

/// method: table GetHelperNameList()
/// \brief ������еĸ�����������б�
//DECLARE_METHOD_T(EffectModel, GetHelperNameList);

/// method: bool EffectModel::SaveEffectModel(string file_name, 
///		string effect_name)
/// \brief ������Чģ��
/// \param file_name Ŀ���ļ���
/// \param effect_name ��Чģ����
/// \return �ɹ�����true,ʧ�ܷ���false
//DECLARE_METHOD_2(bool, EffectModel, SaveEffectModel, const char*, const char*);

// ���ȫ�ֿ���
static bool get_global_on_off(ICore* pCore, const char* name)
{
	IVar& v = pCore->GetGlobalValue(name);

	if (v.GetType() != VTYPE_BOOL)
	{
		return false;
	}

	return v.BoolVal();
}

// EffectModel

EffectModel::EffectModel()
{
	//m_pParticleManager = NULL;
	//m_pSaberArcManager = NULL;
	//m_pLightManager = NULL;
	m_pModel = NULL;
	m_nLoadState = LOADSTATE_WAIT;
	m_bClipPlaneEnable = false;
	m_bNeedCreate = false;
	m_bModelChanged = false;
	m_bVisible = true;
	m_bShowParticle = true;
	m_bCalParticleSize = true;
	m_bUpdateNodeMat = false;
	m_bLoop = true;
	m_bIsOneOff = true;
	m_bTraceEnable = true;
	m_bCullEnable = true;
	m_bTraceBoxEnable = true;
	m_bWaterReflect = false;
	m_bAsyncLoad = false;
	m_bShowBoundBox = false;
	m_bUpdatedBoundingBox = false;
    m_bSleep = false;
	m_fAlphaValue = 1.0F;
	m_fCurrentTime = 0.0F;
	m_fLifeTime = -1.0F;
	m_fSpeed = -1.0F;
	m_nColor = 0xFFFFFFFF;

    m_vTarget = FmVec3(0.0F, 0.0F, 0.0F);

	m_vBoundMin = FmVec3(0.0F, 0.0F, 0.0F);
	m_vBoundMax = FmVec3(0.0F, 0.0F, 0.0F);
	m_vCenter = FmVec3(0.0F, 0.0F, 0.0F);
	m_fRadius = 0.0F;

	FmMatrixIdentity(&m_mtxWorld);
}

EffectModel::~EffectModel()
{
	for (size_t i = 0; i < m_ParticleInfo.size(); ++i)
	{
		CORE_DELETE(m_ParticleInfo[i]);
	}

	for (size_t j = 0; j < m_SaberArcInfo.size(); ++j)
	{
		CORE_DELETE(m_SaberArcInfo[j]);
	}

	for (size_t k = 0; k < m_LightInfo.size(); ++k)
	{
		CORE_DELETE(m_LightInfo[k]);
	}
}

bool EffectModel::Init(const IVarList& args)
{
	m_pRender = (IRender*)GetCore()->GetInterface("Render");
	
	return true;
}

bool EffectModel::Shut()
{
	GetCore()->RemoveExecute(this);

	Unload();

	SAFE_RELEASE(m_pModel);

	return true;
}

void EffectModel::SetContext(IRenderContext* value)
{
	Assert(value != NULL);

	m_pContext = value;

	// �������ϵͳ����ָ��
	ParticleManager* pParticleManager = 
		(ParticleManager*)m_pContext->GetUnique("ParticleManager");

	if (pParticleManager)
	{
		m_ParticleManagerID = pParticleManager->GetID();
	}

	// ��ù�Դϵͳ����ָ��
	LightManager* pLightManager = 
		(LightManager*)m_pContext->GetUnique("LightManager");

	if (pLightManager)
	{
		m_LightManagerID = pLightManager->GetID();
	}

	// ���SaberArcManager����ָ��
	SaberArcManager* pSaberArcManager = 
		(SaberArcManager*)m_pContext->GetUnique("SaberArcManager");

	if (pSaberArcManager)
	{
		m_SaberArcManagerID = pSaberArcManager->GetID();
	}
}

bool EffectModel::Clear()
{
	Unload();

	SAFE_RELEASE(m_pModel);

	m_strAppendPath = "";
	m_strModelFile = "";
	m_strMaterialFile = "";
	m_strParticleFile = "";
	m_strSaberArcFile = "";
	m_strLightFile = "";
	m_strConfigFile = "";
	m_strEffectName = "";

	EffectModel();

	return true;
}

bool EffectModel::Unload()
{
	size_t nParticleCount = m_ParticleInfo.size();

	if (nParticleCount > 0)
	{
		for (size_t i = 0; i < nParticleCount; ++i)
		{
			IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(
				m_ParticleInfo[i]->particle_id);

			SAFE_RELEASE(pVisBase);
			CORE_DELETE(m_ParticleInfo[i]);
		}

		m_ParticleInfo.clear();
	}
	
	size_t nSaberArcCount = m_SaberArcInfo.size();

	if (nSaberArcCount > 0)
	{
		for (size_t i = 0; i < nSaberArcCount; ++i)
		{
			IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(
				m_SaberArcInfo[i]->saberarc_id);

			SAFE_RELEASE(pVisBase);
			CORE_DELETE(m_SaberArcInfo[i]);
		}

		m_SaberArcInfo.clear();
	}

	size_t nLightCount = m_LightInfo.size();

	if (nLightCount > 0)
	{
		for (size_t i = 0; i < nLightCount; ++i)
		{
			IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(
				m_LightInfo[i]->light_id);

			SAFE_RELEASE(pVisBase);
			CORE_DELETE(m_LightInfo[i]);
		}

		m_LightInfo.clear();
	}

	IVisBase* pBrokenShadow = (IVisBase*)GetCore()->GetEntity(m_BrokenShadow);

	if (pBrokenShadow != NULL)
	{
		pBrokenShadow->Release();
		m_BrokenShadow = PERSISTID();
	}

	return true;
}

void EffectModel::Execute(float seconds)
{
	CheckLoad(seconds);
}

void EffectModel::Update(float seconds)
{
	if (NULL == m_pModel)
	{
		return;
	}
	
	if (!m_pModel->GetLoadFinish())
	{
		return;
	}

	if (m_bNeedCreate)
	{
		CreateEffect();
		return;
	}

    if (m_bSleep)
    {
        return;
    }

	m_fCurrentTime += seconds;

	if (m_fLifeTime >= 0.0F)
	{
		if (m_fCurrentTime >= m_fLifeTime)
		{
			// �������ڽ���,ж��ģ��
			Release();
			return;
		}
	}

	if (!m_bLoop && m_bIsOneOff)
	{
		int nFrameCount = m_pModel->GetFrameCount();
		int nCurrentFrame = m_pModel->GetCurrentFrame();

		if (nCurrentFrame >= nFrameCount - 1)
		{
			// ���ظ���ʱ�򲥷����ж��ģ��
			Release();
			return;
		}
	}

	m_pModel->Update(seconds);

	if (!m_BrokenShadow.IsNull())
	{
		IVisBase* pBrokenShadow = (IVisBase*)GetCore()->GetEntity(
			m_BrokenShadow);

		if (pBrokenShadow != NULL)
		{
			// ���²�Ӱ
			pBrokenShadow->Update(seconds);

			if (((BrokenShadow*)pBrokenShadow)->GetComplete())
			{
				pBrokenShadow->Release();
				m_BrokenShadow = PERSISTID();
			}
		}
	}

	size_t particle_num = m_ParticleInfo.size();

	for (size_t i = 0; i < particle_num; ++i)
	{
		PERSISTID id = m_ParticleInfo[i]->particle_id;

		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(id);

		if (pVisBase)
		{
			pVisBase->Update(seconds);
		}
	}

	size_t light_num = m_LightInfo.size();

	for (size_t j = 0; j < light_num; ++j)
	{
		PERSISTID id = m_LightInfo[j]->light_id;
		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(id);

		if (pVisBase)
		{
			pVisBase->Update(seconds);
		}
	}
	
	m_bUpdatedBoundingBox = false;
}

void EffectModel::Realize()
{
	if (!m_bVisible)
	{
		return;
	}

	if (NULL == m_pModel)
	{
		return;
	}

	if(!m_pModel->GetLoadFinish())
	{
		return;
	}

	//m_pModel->SetColor(m_nColor);
	m_pModel->Realize();

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nEffectModelDrawCallNum += m_pModel->GetNodeCount();
#endif 

	if (m_bShowParticle)
	{
		size_t particle_num = m_ParticleInfo.size();

		if (particle_num > 0)
		{
#ifdef PERFORMANCE_DEBUG
			m_pRender->GetPerformance()->nEffectParticleDrawCallNum += particle_num;
#endif 

			UpdateLinkParticleEmit();

			for (size_t i = 0; i < particle_num; ++i)
			{
				PERSISTID id = m_ParticleInfo[i]->particle_id;

				IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(id);

				if (pVisBase)
				{
					pVisBase->Realize();
				}
			}
		}
	}


	size_t light_num = m_LightInfo.size();

	if (light_num > 0)
	{
		UpdateLinkLightEmit();

		for (size_t j = 0; j < light_num; ++j)
		{
			PERSISTID id = m_LightInfo[j]->light_id;

			IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(id);

			if (pVisBase)
			{
				pVisBase->Realize();
			}
		}
	}

	// ��Ӱ
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(m_BrokenShadow);

	if (pVisBase)
	{
		pVisBase->Realize();
	}

	//if (m_bShowBoundBox)
	//{
	//	FmMat4 mat;

	//	FmMatrixIdentity(&mat);

	//	VisUtil_DrawBoundBox(m_pRender, mat, GetCenter(), GetBoxSize(), 
	//		0xFFFFFFFF);
	//}
}

void EffectModel::RealizeShadowMap()
{
	if (!m_bVisible)
	{
		return;
	}

	if (NULL == m_pModel)
	{
		return;
	}

	if(!m_pModel->GetLoadFinish())
	{
		return;
	}

	m_pModel->RealizeShadowMap();
}

bool EffectModel::SetPosition(float x, float y, float z)
{
	//Assert(float3_is_valid(x, y, z));

	m_bUpdatedBoundingBox = false;

	m_mtxWorld._41 = x;
	m_mtxWorld._42 = y;
	m_mtxWorld._43 = z;

	if (m_pModel)
	{
		return m_pModel->SetPosition(x, y, z);
	}

	return false;
}

FmVec3 EffectModel::GetPosition() const
{
	if (m_pModel)
	{
		return m_pModel->GetPosition();
	}

	return FmVec3(m_mtxWorld._41, m_mtxWorld._42, 
		m_mtxWorld._43);
}

// Ŀ��λ��
bool EffectModel::SetTarget(float x, float y, float z)
{
    m_vTarget.x = x;
    m_vTarget.y = y;
    m_vTarget.z = z;

    return true;
}

FmVec3 EffectModel::GetTarget() const
{
    return m_vTarget;
}

bool EffectModel::SetAngle(float x, float y, float z)
{
	//Assert(float3_is_valid(x, y, z));

	m_bUpdatedBoundingBox = false;

	VisUtil_SetMatrixAngle(&m_mtxWorld, x, y, z);

	if (m_pModel)
	{
		return m_pModel->SetAngle(x, y, z);
	}

	return false;
}

FmVec3 EffectModel::GetAngle() const
{
	if (m_pModel)
	{
		return m_pModel->GetAngle();
	}

	FmVec3 angle;

	VisUtil_GetMatrixAngle(&angle, &m_mtxWorld);

	return angle;
}

bool EffectModel::SetScale(float x, float y, float z)
{
	//Assert(float3_is_valid(x, y, z));

	m_bUpdatedBoundingBox = false;

	VisUtil_SetMatrixScale(&m_mtxWorld, x, y, z);

	if (m_pModel)
	{
		size_t n = m_ParticleInfo.size();

		for (size_t i = 0; i < n; i++)
		{
			IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(
				m_ParticleInfo[i]->particle_id);

			if (pVisBase)
			{
				pVisBase->SetScale(x, y, z);
			}
		}

		return m_pModel->SetScale(x, y, z);
	}

	return false;
}

FmVec3 EffectModel::GetScale() const
{
	if (m_pModel)
	{
		return m_pModel->GetScale();
	}

	FmVec3 scale;

	VisUtil_GetMatrixScale(&scale, &m_mtxWorld);

	return scale;
}

FmVec3 EffectModel::GetCenter()
{
	if (!m_bUpdatedBoundingBox)
	{
		UpdateBoundingBox();
	}

	return m_vCenter;
}

FmVec3 EffectModel::GetBoxSize()
{
	if (!m_bUpdatedBoundingBox)
	{
		UpdateBoundingBox();
	}

	return m_vBoundMax - m_vBoundMin;
}

float EffectModel::GetRadius()
{
	if (!m_bUpdatedBoundingBox)
	{
		UpdateBoundingBox();
	}

	return m_fRadius;
}

void EffectModel::UpdateBoundingBox()
{
	// ������ɫģ��Ϊ������ɾ�updateboundingbox�����³��������
	if (!GetLoadFinish())
	{
		return;
	}

	FmVec3 v_min(1e8, 1e8, 1e8);
	FmVec3 v_max(-1e8, -1e8, -1e8);

	if (m_pModel)
	{
		FmVec3 center = m_pModel->GetCenter();
		FmVec3 box_size = m_pModel->GetBoxSize();
		v_min = center - box_size * 0.5F;
		v_max = center + box_size * 0.5F;
	}

	if (m_bShowParticle && m_bCalParticleSize)
	{
		FmVec3 v_min_temp(1e8, 1e8, 1e8);
		FmVec3 v_max_temp(-1e8, -1e8, -1e8);
		size_t particle_num = m_ParticleInfo.size();

		for (size_t i = 0; i < particle_num; ++i)
		{
			PERSISTID id = m_ParticleInfo[i]->particle_id;
			IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(id);

			if (pVisBase)
			{
				float radius = pVisBase->GetRadius();

				if (radius <= 0.0F)
				{
					// ���ӿ��ܻ�û�з���
					continue;
				}

				FmVec3 center = pVisBase->GetCenter();
				FmVec3 box_size = pVisBase->GetBoxSize();

				v_min_temp = center - box_size / 2.0F;
				v_max_temp = center + box_size / 2.0F;
				FmVec3Minimize(&v_min, &v_min, &v_min_temp);
				FmVec3Maximize(&v_max, &v_max, &v_max_temp);
			}
		}
	}

	if (v_max.x >= v_min.x)
	{
		m_vBoundMin = v_min;
		m_vBoundMax = v_max;
	}

	m_vCenter = (m_vBoundMax + m_vBoundMin) * 0.5F;
	FmVec3 len = m_vBoundMax - m_vBoundMin;
	m_fRadius = VisUtil_Vec3Length(&len) * 0.5F;
	m_bUpdatedBoundingBox = true;
}

void EffectModel::SetColorString(const char* value)
{
	Assert(value != NULL);

	unsigned int nColor = VisUtil_ColorToInt(value);

	SetColor(nColor);
}

result_string EffectModel::GetColorString() const
{
	return VisUtil_IntToColor(GetColor());
}

void EffectModel::SetColor(unsigned int value)
{
	m_nColor = value;

	if (m_pModel)
	{
		m_pModel->SetColor(value);
	}

	unsigned int particle_value = value;

	if (0 == particle_value)
	{
		particle_value =0xFFFFFFFF;
	}

	size_t particle_num = m_ParticleInfo.size();

	for (size_t i = 0; i < particle_num; ++i)
	{
		Particle* particle = (Particle*)GetCore()->GetEntity(
			m_ParticleInfo[i]->particle_id);

		if (particle)
		{
			particle->SetColor(particle_value);
		}
	}

	size_t light_num = m_LightInfo.size();

	for (size_t j = 0; j < light_num; ++j)
	{
		LightSource* pLight = (LightSource*)GetCore()->GetEntity(
			m_LightInfo[j]->light_id);

		if (pLight)
		{
			pLight->SetColor(value);
		}
	}
}

unsigned int EffectModel::GetColor() const
{
	//if (m_pModel)
	//{
	//	return m_pModel->GetColor();
	//}

	return m_nColor;
}

void EffectModel::SetVisible(bool value)
{
	m_bVisible = value;

    size_t nSaberArcCount = m_SaberArcInfo.size();

    if (nSaberArcCount > 0)
    {
        for (size_t i = 0; i < nSaberArcCount; ++i)
        {
            IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(
                m_SaberArcInfo[i]->saberarc_id);
            if (pVisBase)
            {
                pVisBase->SetVisible(value);
            }
        }
    }
}

bool EffectModel::GetVisible() const
{
	return m_bVisible;
}

void EffectModel::SetUnderWater(bool value)
{
	if (m_pModel)
	{
		m_pModel->SetUnderWater(value);
	}

	size_t particle_num = m_ParticleInfo.size();
	
	for (size_t i = 0 ; i < particle_num; ++i)
	{
		Particle* particle = (Particle*)GetCore()->GetEntity(
			m_ParticleInfo[i]->particle_id);

		if (particle)
		{
			particle->SetUnderWater(value);
		}
	}
}

bool EffectModel::GetUnderWater() const
{
	if (m_pModel)
	{
		return m_pModel->GetUnderWater();
	}

	return false;
}

bool EffectModel::GetLoaded()
{
	if (NULL == m_pModel)
	{
		return false;
	}

	if (!m_pModel->GetLoaded())
	{
		return false;
	}

	if (m_bNeedCreate)
	{
		CreateEffect();
		return false;
	}

	size_t nCount = m_ParticleInfo.size();

	for (size_t nIndex = 0; nIndex < nCount; ++nIndex)
	{
		if (m_ParticleInfo[nIndex]->particle_id.IsNull())
		{
			continue;
		}

		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(
			m_ParticleInfo[nIndex]->particle_id);

		if (NULL == pVisBase)
		{
			return false;
		}

		if (!pVisBase->GetLoaded())
		{
			return false;
		}
	}

	nCount = m_SaberArcInfo.size();

	for (size_t nIndex = 0; nIndex < nCount; ++nIndex)
	{
		if (m_SaberArcInfo[nIndex]->saberarc_id.IsNull())
		{
			continue;
		}

		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(
			m_SaberArcInfo[nIndex]->saberarc_id);

		if (NULL == pVisBase)
		{
			return false;
		}

		if (!pVisBase->GetLoaded())
		{
			return false;
		}
	}

	nCount = m_LightInfo.size();

	for (size_t nIndex = 0; nIndex < nCount; ++nIndex)
	{
		if (m_LightInfo[nIndex]->light_id.IsNull())
		{
			continue;
		}

		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(
			m_LightInfo[nIndex]->light_id);

		if (NULL == pVisBase)
		{
			return false;
		}

		if (!pVisBase->GetLoaded())
		{
			return false;
		}
	}

	return true;
}

bool EffectModel::GetLoadFinish()
{
	if (NULL == m_pModel)
	{
		return false;
	}

	if (!m_pModel->GetLoadFinish())
	{
		return false;
	}

	if (m_bNeedCreate)
	{
		CreateEffect();
		return false;
	}

	size_t nCount = m_ParticleInfo.size();

	for (size_t nIndex = 0; nIndex < nCount; ++nIndex)
	{
		if (m_ParticleInfo[nIndex]->particle_id.IsNull())
		{
			continue;
		}

		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(
			m_ParticleInfo[nIndex]->particle_id);

		if (NULL == pVisBase)
		{
			return false;
		}

		if (!pVisBase->GetLoadFinish())
		{
			return false;
		}
	}

	nCount = m_SaberArcInfo.size();

	for (size_t nIndex = 0; nIndex < nCount; ++nIndex)
	{
		if (m_SaberArcInfo[nIndex]->saberarc_id.IsNull())
		{
			continue;
		}

		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(
			m_SaberArcInfo[nIndex]->saberarc_id);

		if (NULL == pVisBase)
		{
			return false;
		}

		if (!pVisBase->GetLoadFinish())
		{
			return false;
		}
	}
	
	nCount = m_LightInfo.size();

	for (size_t nIndex = 0; nIndex < nCount; ++nIndex)
	{
		if (m_LightInfo[nIndex]->light_id.IsNull())
		{
			continue;
		}

		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(
			m_LightInfo[nIndex]->light_id);

		if (NULL == pVisBase)
		{
			return false;
		}

		if (!pVisBase->GetLoadFinish())
		{
			return false;
		}
	}

	return true;
}

void EffectModel::SetShowParticle(bool value)
{
	if (m_bShowParticle == value)
	{
		return;
	}

	m_bUpdatedBoundingBox = false;

	m_bShowParticle = value;
	
	size_t num = m_ParticleInfo.size();

	for (size_t i = 0 ; i < num ; ++i)
	{
		Particle* particle = (Particle*)GetCore()->GetEntity(
			m_ParticleInfo[i]->particle_id);

		if (particle)
		{
			particle->SetVisible(value);
		}
	}
}

bool EffectModel::GetShowParticle()
{
	return m_bShowParticle;
}

void EffectModel::SetCalParticleSize(bool value)
{
	m_bCalParticleSize = value;

	m_bUpdatedBoundingBox = false;
}

bool EffectModel::GetCalParticleSize()
{
	return m_bCalParticleSize;
}

float EffectModel::GetCurrenTime()
{
	return m_fCurrentTime;
}

PERSISTID EffectModel::GetModelID() const
{
	if (NULL == m_pModel)
	{
		return PERSISTID();
	}
	
	return m_pModel->GetID();
}

void EffectModel::SetModelID(const PERSISTID& id)
{
	if (id.IsNull())
	{
		return;
	}

	Unload();

	SAFE_RELEASE(m_pModel);

	m_pModel = (Model*)GetCore()->GetEntity(id);

	m_bUpdatedBoundingBox = false;
}

void EffectModel::SetLifeTime(float value)
{
	m_fLifeTime = value;
}

float EffectModel::GetLifeTime()
{
	return m_fLifeTime;
}

void EffectModel::SetLoop(bool value)
{
	m_bLoop = value;

	if (m_pModel)
	{
		m_pModel->SetLoop(m_bLoop);

		size_t nCount = m_ParticleInfo.size();
		
		for (size_t i = 0; i < nCount; ++i)
		{
			PERSISTID id = m_ParticleInfo[i]->particle_id;
			Particle* pParticle = (Particle*)GetCore()->GetEntity(id);

            if (pParticle != NULL)
            {
			    pParticle->SetRepeat(m_bLoop);
            }
		}
	}
}

bool EffectModel::GetLoop()
{
	return m_bLoop;
}

void EffectModel::SetIsOneOff(bool bValue)
{
	m_bIsOneOff = bValue;
}

bool EffectModel::GetIsOneOff()
{
	return m_bIsOneOff;
}

void EffectModel::SetSpeed(float fSpeed)
{
	m_fSpeed = fSpeed;

	if (m_pModel)
	{
		m_pModel->SetSpeed(fSpeed);

		size_t nCount = m_ParticleInfo.size();
		
		for (size_t i = 0; i < nCount; ++i)
		{
			PERSISTID id = m_ParticleInfo[i]->particle_id;
			Particle* pParticle = (Particle*)GetCore()->GetEntity(id);

            if (pParticle != NULL)
            {
                pParticle->SetSpeedFactor(fSpeed);
            }
		}
	}
}

float EffectModel::GetSpeed()
{
	return m_fSpeed;
}

void EffectModel::SetTraceEnable(bool value)
{
	m_bTraceEnable = value;
}

bool EffectModel::GetTraceEnable()
{
	return m_bTraceEnable;
}

void EffectModel::SetCullEnable(bool value)
{
	m_bCullEnable = value;
}

bool EffectModel::GetCullEnable()
{
	return m_bCullEnable;
}

void EffectModel::SetTraceBoxEnable(bool value)
{
	m_bTraceBoxEnable = value;
}

bool EffectModel::GetTraceBoxEnable()
{
	return m_bTraceBoxEnable;
}

void EffectModel::SetAsyncLoad(bool value)
{
	m_bAsyncLoad = value;
}

bool EffectModel::GetAsyncLoad() const
{
	return m_bAsyncLoad;
}

void EffectModel::SetShowBoundBox(bool value)
{
	m_bShowBoundBox = value;

	if (m_pModel)
	{
		m_pModel->SetShowBoundBox(value);
	}

	if (m_bShowParticle)
	{
		size_t particle_num = m_ParticleInfo.size();

		for (size_t i = 0; i < particle_num; ++i)
		{
			PERSISTID id = m_ParticleInfo[i]->particle_id;

			Particle* pVisBase = (Particle*)GetCore()->GetEntity(id);

			if (pVisBase)
			{
				pVisBase->SetShowBoundBox(value);
			}
		}
	}
}

bool EffectModel::GetShowBoundBox() const
{
	return m_bShowBoundBox;
}

void EffectModel::SetModelFile(const char* model_file)
{
	Assert(model_file != NULL);

	if (0 != m_strModelFile.compare(model_file))
	{
		m_bModelChanged = true;
	}

	m_strModelFile = model_file;
}

const char* EffectModel::GetModelFile() const
{
	return m_strModelFile.c_str();
}

void EffectModel::SetMaterialFile(const char* material_file)
{
	Assert(material_file != NULL);

	if (0 != m_strMaterialFile.compare(material_file))
	{
		m_bModelChanged = true;
	}

	m_strMaterialFile = material_file;
}

const char* EffectModel::GetMaterialFile() const
{
	return m_strMaterialFile.c_str();
}

void EffectModel::SetParticleFile(const char* file_name)
{
	Assert(file_name != NULL);

	m_strParticleFile = file_name;
}

const char* EffectModel::GetParticleFile() const
{
	return m_strParticleFile.c_str();
}

void EffectModel::SetSaberArcFile(const char* file_name)
{
	Assert(file_name != NULL);

	m_strSaberArcFile = file_name;
}

const char* EffectModel::GetSaberArcFile() const
{
	return m_strSaberArcFile.c_str();
}

void EffectModel::SetLightFile(const char* file_name)
{
	Assert(file_name != NULL);

	m_strLightFile = file_name;
}

const char* EffectModel::GetLightFile() const
{
	return m_strLightFile.c_str();
}

const char* EffectModel::GetConfigFile() const
{
	return m_strConfigFile.c_str();
}

const char* EffectModel::GetName() const
{
	return m_strEffectName.c_str();
}

void EffectModel::SetName(const char* name)
{
	m_strEffectName = name;
}

bool EffectModel::CheckLoad(float seconds)
{
	if (m_nLoadState == LOADSTATE_SUCCEED || m_nLoadState == LOADSTATE_FAILED)
	{
		GetCore()->RemoveExecute(this);

		if (m_nLoadState == LOADSTATE_FAILED)
		{
			// ж��ģ��
			Release();
		}

		return true;
	}

	IRender* pRender = m_pRender;

	if (NULL == pRender)
	{
		return false;
	}

	switch (m_nLoadState)
	{
	case LOADSTATE_WAIT:
		break;
	case LOADSTATE_BEGIN:
		Unload();

		{
			const char* config = m_strConfigFile.c_str();

			if (0 == config[0])
			{
				m_nLoadState = LOADSTATE_FAILED;
				break;
			}

			// �����Դ����·��
			char fname[256];

			SafeSprintf(fname, sizeof(fname), "%s%s", 
				pRender->GetPathPrefix(config), config);

			//if (pRender->FindCacheIni(fname))
			//{
			//	if (!pRender->GetCacheIniLoadFinish(fname))
			//	{
					// �ȴ������ļ��������
			//		m_nLoadState = LOADSTATE_LOADCACHE;
			//	}
			//	else
			//	{
					LoadData(m_strConfigFile.c_str(), m_strEffectName.c_str(), 
						m_bAsyncLoad, m_strAppendPath.c_str());

					if (CreateFromSetting())
					{
						m_nLoadState = LOADSTATE_LOADING;
					}
					else
					{
						m_nLoadState = LOADSTATE_FAILED;
					}
			//	}
			//}
			//else
			//{
			//	// �첽���������ļ�
			//	pRender->LoadCacheIni(fname, false, true);
			//	m_nLoadState= LOADSTATE_LOADCACHE;
			//}
		}
		break;
	case LOADSTATE_LOADCACHE:
		{
			//const char* config = m_strConfigFile.c_str();
			// �����Դ����·��
			//char fname[256];

			//SafeSprintf(fname, sizeof(fname), "%s%s", 
			//	pRender->GetPathPrefix(config), config);

			//if (pRender->FindCacheIni(fname))
			//{
			//	if (!pRender->GetCacheIniLoadFinish(fname))
			//	{
			//		// �ȴ������ļ��������
			//		break;
			//	}
			//}
		}

		LoadData(m_strConfigFile.c_str(), m_strEffectName.c_str(), 
			m_bAsyncLoad, m_strAppendPath.c_str());

		if (CreateFromSetting())
		{
			m_nLoadState = LOADSTATE_LOADING;
		}
		else
		{
			m_nLoadState = LOADSTATE_FAILED;
		}
		break;
	case LOADSTATE_LOADING:
		{
			if (NULL == m_pModel)
			{
				break;
			}

			if (!m_pModel->GetLoadFinish())
			{
				// ��δ�������
				break;
			}

			if (!m_pModel->GetLoaded())
			{
				// �������ʧ��
				m_nLoadState = LOADSTATE_FAILED;
				break;
			}

			if (m_bNeedCreate)
			{
				CreateEffect();
			}
		}
		m_nLoadState = LOADSTATE_SUCCEED;
		break;
	case LOADSTATE_SUCCEED:
		break;
	case LOADSTATE_FAILED:
		break;
	default:
		break;
	}

	return true;
}

bool EffectModel::CreateFromIni(const char* ini_name, const char* em_name, 
	bool async_load)
{
	return CreateFromIniEx(ini_name, em_name, async_load, "");
}

bool EffectModel::CreateFromIniEx(const char* ini_name, const char* em_name, 
	bool async_load, const char* append_path)
{
	m_strConfigFile = ini_name;
	m_strEffectName = em_name;
	m_strAppendPath = append_path;
	m_bAsyncLoad = async_load;

	if (async_load)
	{
		m_nLoadState = LOADSTATE_BEGIN;

		GetCore()->AddExecute(this);
	}
	else
	{
		if (!Unload())
		{
			return false;
		}

		if (!LoadData(ini_name, em_name, async_load, append_path))
		{
			return false;
		}

		if (!CreateFromSetting())
		{
			return false;
		}

		m_bUpdatedBoundingBox = false;
	}

	return true;
}

static inline int read_ini_integer(IIniFile* pIni, size_t sect_index, 
	const char* key, int def_value)
{
	size_t item_index;

	if (!pIni->FindSectionItemIndex(sect_index, key, item_index))
	{
		return def_value;
	}

	return atoi(pIni->GetSectionItemValue(sect_index, item_index));
}

static inline float read_ini_float(IIniFile* pIni, size_t sect_index, 
	const char* key, float def_value)
{
	size_t item_index;

	if (!pIni->FindSectionItemIndex(sect_index, key, item_index))
	{
		return def_value;
	}

	return (float)atof(pIni->GetSectionItemValue(sect_index, item_index));
}

static inline const char* read_ini_string(IIniFile* pIni, 
	size_t sect_index, const char* key, const char* def_value)
{
	size_t item_index;

	if (!pIni->FindSectionItemIndex(sect_index, key, item_index))
	{
		return def_value;
	}

	return pIni->GetSectionItemValue(sect_index, item_index);
}

bool EffectModel::LoadData(const char* ini_name, const char* em_name, 
	bool async_load, const char* append_path)
{
	Assert(ini_name != NULL);
	Assert(em_name != NULL);
	Assert(append_path != NULL);

	if (m_ParticleManagerID.IsNull())
	{
		return false;
	}

	ParticleManager* pParticleManager = 
		(ParticleManager*)GetCore()->GetEntity(m_ParticleManagerID);

	if (NULL == pParticleManager)
	{
		return false;
	}

	CEffectData* pEffectData = pParticleManager->CreateEffectData(ini_name, 
		em_name);

	if (NULL == pEffectData)
	{
		return false;
	}

	//m_strConfigFile = ini_name;
	//m_strEffectName = em_name;
	//m_strAppendPath = append_path;
	//m_bAsyncLoad = async_load;

	m_bModelChanged = false;
	SetModelFile(pEffectData->m_strModelFile.c_str());
	SetMaterialFile(pEffectData->m_strMaterialFile.c_str());
	m_strParticleFile = pEffectData->m_strParticleFile;
	m_strSaberArcFile = pEffectData->m_strSaberArcFile;
	m_strLightFile = pEffectData->m_strLightFile;
	m_bClipPlaneEnable = pEffectData->m_bClipPlaneEnable;

	size_t count = pEffectData->m_ParticleInfo.size();

	for (size_t i = 0; i < count; ++i)
	{
		CEffectData::particle_info_t* data_info = pEffectData->m_ParticleInfo[i];
		particle_info* par_info = CORE_NEW(particle_info);

		par_info->particle_id = PERSISTID();
		par_info->open_emit = data_info->open_emit;
		par_info->close_emit = data_info->close_emit;
		par_info->particle_name = data_info->particle_name;
		par_info->helper_name = data_info->helper_name;

		m_ParticleInfo.push_back(par_info);
	}

	count = pEffectData->m_SaberArcInfo.size();

	for (size_t i = 0; i < count; ++i)
	{
		CEffectData::saberarc_info_t* data_info = pEffectData->m_SaberArcInfo[i];
		saberarc_info* sa_info = CORE_NEW(saberarc_info);

		sa_info->helper_name0 = data_info->helper_name0;
		sa_info->helper_name1 = data_info->helper_name1;
		sa_info->saberarc_name = data_info->saberarc_name;
		sa_info->saberarc_id = PERSISTID();
		sa_info->is_saber_start = data_info->is_saber_start;

		m_SaberArcInfo.push_back(sa_info);
	}

	count = pEffectData->m_LightInfo.size();

	for (size_t i = 0; i < count; ++i)
	{
		CEffectData::light_info_t* data_info = pEffectData->m_LightInfo[i];
		light_info_t* light_info = CORE_NEW(light_info_t);

		light_info->helper_name = data_info->helper_name;
		light_info->light_name = data_info->light_name;
		light_info->light_id = PERSISTID();
		light_info->open_emit = data_info->open_emit;
		light_info->close_emit = data_info->close_emit;

		m_LightInfo.push_back(light_info);
	}

	return true;
}

PERSISTID EffectModel::CreateParticleOnHelper(const char* particle_name, 
	const char* helper_name)
{
	Assert(particle_name != NULL);
	Assert(helper_name != NULL);

	if (m_ParticleManagerID.IsNull())
	{
		return PERSISTID();
	}
	
	ParticleManager* pParticleManager = 
		(ParticleManager*)GetCore()->GetEntity(m_ParticleManagerID);

	if (NULL == pParticleManager)
	{
		return PERSISTID();
	}

	// �������Ӷ���
	char fname[256];

	SafeSprintf(fname, sizeof(fname), "%s%s", m_strAppendPath.c_str(),
		m_strParticleFile.c_str());

	PERSISTID particle_id = pParticleManager->CreateFromIni(fname, 
		particle_name);

	Particle* particle = (Particle*)GetCore()->GetEntity(particle_id);

	if (NULL == particle)
	{
		return PERSISTID();
	}

	// ��Ҫ������λ��
	particle->SetPosition(m_mtxWorld._41, m_mtxWorld._42, 
		m_mtxWorld._43);
	particle->SetClipPlaneEnable(m_bClipPlaneEnable);

	// �����Ӱ󶨵�ģ��ָ���ĸ�������
	if (!particle->LinkToObjectPoint(m_pModel->GetID(), helper_name))
	{
		// ��ʧ��
		pParticleManager->Delete(particle_id);
		return PERSISTID();
	}

    particle->SetRepeat(m_bLoop);

	m_bUpdatedBoundingBox = false;
	
	return particle_id;
}

PERSISTID EffectModel::CreateLightOnHelper(const char* light_name, const char* helper_name)
{
	Assert(light_name != NULL);
	Assert(helper_name != NULL);

	if (m_LightManagerID.IsNull())
	{
		return PERSISTID();
	}

	LightManager* pLightManager = 
		(LightManager*)GetCore()->GetEntity(m_LightManagerID);

	if (NULL == pLightManager)
	{
		return PERSISTID();
	}

	// ������Դ����
	char fname[256];

	SafeSprintf(fname, sizeof(fname), "%s%s", m_strAppendPath.c_str(),
		m_strLightFile.c_str());

	PERSISTID light_id = pLightManager->CreateFromIni(m_pModel->GetID(), 
		fname, light_name, helper_name);

	LightSource* pLight = (LightSource*)GetCore()->GetEntity(light_id);

	if (NULL == pLight)
	{
		return PERSISTID();
	}

	return light_id;
}

// ��õ���������ͼ�ļ�·����
static result_string get_tex_paths(ICore* pCore)
{
	if (NULL == pCore)
	{
		return result_string();
	}

	// ���Terrain����ָ��
	CVarList result;
	ITerrain* pTerrain = (ITerrain*)pCore->LookupEntity("Terrain");
	if(pTerrain == NULL)
	{
		return result_string();
	}

	return result_string(pTerrain->GetTexturePathString());
	/*
	if (!pCore->InvokeMethod(pTerrain, "GetTexturePathString", CVarList(), 
		result))
	{
		return result_string();
	}*/

	return result.StringVal(0);
}

// ͨ������������ģ��
bool EffectModel::CreateFromSetting()
{
	if (m_bModelChanged)
	{
		SAFE_RELEASE(m_pModel);

		Model* pModel = (Model*)GetCore()->CreateEntityArgs("Model", CVarList());

		pModel->SetContext(m_pContext);
		pModel->SetWorldMatrix(m_mtxWorld);

		char model_file_name[256];

		SafeSprintf(model_file_name, sizeof(model_file_name), "%s%s",
			m_strAppendPath.c_str(), m_strModelFile.c_str());

		pModel->SetModelFile(model_file_name);
		pModel->SetAsyncLoad(m_bAsyncLoad);

		result_string tex_paths = get_tex_paths(GetCore());

		pModel->SetTexPaths(tex_paths.c_str());

		if (!m_strMaterialFile.empty())
		{
			char material[256];

			SafeSprintf(material, sizeof(material), "%s%s", 
				m_strAppendPath.c_str(), m_strMaterialFile.c_str());

			pModel->SetMaterialFile(material);
		}

		// ����
		if (!pModel->Load())
		{
			CORE_TRACE("(EffectModel::CreateFromSetting)load failed");
			CORE_TRACE(model_file_name);
			pModel->Release();
			return false;
		}

		m_pModel = pModel;
		
	}

	if (NULL == m_pModel)
	{
		return false;
	}

	if (m_pModel->GetLoadFinish())
	{
		CreateEffect();
	}
	else
	{
		// ��Ϊģ�ͻ�û�м�����������Ҫ�ӳٴ������Ӻ���β
		m_bNeedCreate = true;
		m_nLoadState = LOADSTATE_LOADING;
	}

	m_bUpdatedBoundingBox = false;

	return true;
}

bool EffectModel::CreateParticleFromSetting()
{
	size_t particle_number = m_ParticleInfo.size();

	if (particle_number <= 0)
	{
		return false;
	}

	for (int i = (int)particle_number - 1; i >= 0; --i)
	{
		// ɾ��������
		IVisBase* pOldParticle = (IVisBase*)GetCore()->GetEntity(
			m_ParticleInfo[i]->particle_id);

		if (pOldParticle != NULL)
		{
			SAFE_RELEASE(pOldParticle);
			m_ParticleInfo[i]->particle_id = PERSISTID();
		}

		if (m_ParticleInfo[i]->open_emit <= 0)
		{
			// ����������
			PERSISTID particle_id = CreateParticleOnHelper(
				m_ParticleInfo[i]->particle_name.c_str(), 
				m_ParticleInfo[i]->helper_name.c_str());

			if (!particle_id.IsNull())
			{
				FmVec3 scale = GetScale();

				IVisBase* pParticle = (IVisBase*)GetCore()->GetEntity(
					particle_id);

				pParticle->SetScale(scale.x, scale.y, scale.z);

				m_ParticleInfo[i]->particle_id = particle_id;
			}
			else
			{
				/*
				CORE_TRACE("(EffectModel::CreateParticleFromSetting)"
				"Particle config error");
				CORE_TRACE(m_ParticleInfo[i]->particle_name.c_str());
				CORE_TRACE(m_ParticleInfo[i]->helper_name.c_str());
				*/

				CORE_TRACE_EX("(EffectModel::CreateParticleFromSetting Particle config error) particle_name:%s helper_name:%s ",m_ParticleInfo[i]->particle_name.c_str(),m_ParticleInfo[i]->helper_name.c_str());


				CORE_DELETE(m_ParticleInfo[i]);


				m_ParticleInfo.remove(i);
			}
		}
	}

	m_bUpdatedBoundingBox = false;

	return true;
}

bool EffectModel::CreateSaberArcFromSetting()
{
	size_t nSaberArcNum = m_SaberArcInfo.size();

	if (nSaberArcNum > 0)
	{
		if (m_SaberArcManagerID.IsNull())
		{
			return false;
		}

		SaberArcManager* pSaberArcManager = 
			(SaberArcManager*)GetCore()->GetEntity(m_SaberArcManagerID);

		if (NULL == pSaberArcManager)
		{
			return false;
		}

		for (int i = (int)nSaberArcNum - 1; i >= 0; --i)
		{
			// ɾ����SaberArc����
			SaberArc* pOldSaberArc = (SaberArc*)GetCore()->GetEntity(
				m_SaberArcInfo[i]->saberarc_id);

			if (pOldSaberArc != NULL)
			{
				SAFE_RELEASE(pOldSaberArc);
				m_SaberArcInfo[i]->saberarc_id = PERSISTID();
			}

			// ������SaberArc����
			PERSISTID SaberArcID = pSaberArcManager->CreateFromIni(
				m_pModel->GetID(), m_strSaberArcFile.c_str(), 
				m_SaberArcInfo[i]->saberarc_name.c_str(), 
				m_SaberArcInfo[i]->helper_name0.c_str(), 
				m_SaberArcInfo[i]->helper_name1.c_str());

			SaberArc* pSaberArc = (SaberArc*)GetCore()->GetEntity(SaberArcID);

			if (pSaberArc)
            {
                pSaberArc->SetVisible(m_bVisible);
                pSaberArc->SetSleep(m_bSleep);
                pSaberArc->SetTarget(m_vTarget);

				m_SaberArcInfo[i]->saberarc_id = SaberArcID;

				pSaberArc->SetStart(m_SaberArcInfo[i]->is_saber_start);
			}
			else
			{
				CORE_TRACE("(EffectModel::CreateSaberArcFromSetting)"
                    "saberarc config error");
                CORE_TRACE(m_strEffectName.c_str());
				CORE_TRACE(m_SaberArcInfo[i]->saberarc_name.c_str());
				CORE_TRACE(m_SaberArcInfo[i]->helper_name0.c_str());
				CORE_TRACE(m_SaberArcInfo[i]->helper_name1.c_str());
				CORE_DELETE(m_SaberArcInfo[i]);
				m_SaberArcInfo.remove(i);
			}
		}
	}

	m_bUpdatedBoundingBox = false;

	return true;
}

bool EffectModel::CreateLightFromSetting()
{
	size_t light_count = m_LightInfo.size();

	if (light_count <= 0)
	{
		return true;
	}

	if (m_LightManagerID.IsNull())
	{
		return false;
	}

	LightManager* pLightManager = 
		(LightManager*)GetCore()->GetEntity(m_LightManagerID);

	if (NULL == pLightManager)
	{
		return false;
	}

	char fname[256];

	SafeSprintf(fname, sizeof(fname), "%s%s", m_strAppendPath.c_str(),
		m_strLightFile.c_str());

	for (int i = (int)light_count - 1; i >= 0; --i)
	{
		// ɾ����LightSource����
		LightSource* pOldLight = (LightSource*)GetCore()->GetEntity(
			m_LightInfo[i]->light_id);

		if (pOldLight != NULL)
		{
			SAFE_RELEASE(pOldLight);
			m_LightInfo[i]->light_id = PERSISTID();
		}

		// ������LightSource����
		PERSISTID LightID = pLightManager->CreateFromIni(m_pModel->GetID(), 
			fname, m_LightInfo[i]->light_name.c_str(), 
			m_LightInfo[i]->helper_name.c_str());

		LightSource* pLight = (LightSource*)GetCore()->GetEntity(LightID);

		if (pLight)
		{
			m_LightInfo[i]->light_id = LightID;
		}
		else
		{
			CORE_TRACE("(EffectModel::CreateLightFromSetting)"
				"light config error");
			CORE_TRACE(m_LightInfo[i]->light_name.c_str());
			CORE_TRACE(m_LightInfo[i]->helper_name.c_str());
			CORE_DELETE(m_LightInfo[i]);
			m_LightInfo.remove(i);
		}
	}

	m_bUpdatedBoundingBox = false;

	return true;
}

bool EffectModel::CreateEffect()
{
	if (NULL == m_pModel)
	{
		return false;
	}

	if (!m_pModel->GetLoaded())
	{
		m_nLoadState = LOADSTATE_FAILED;
		m_bNeedCreate = false;
		return false;
	}

	if (m_fSpeed < 0.0F)
	{
		m_fSpeed = m_pModel->GetDefaultSpeed();
	}

	m_pModel->SetLoop(m_bLoop);
	m_pModel->SetSpeed(m_fSpeed);
	m_pModel->SetColor(m_nColor);

	CreateParticleFromSetting();
	CreateSaberArcFromSetting();
	CreateLightFromSetting();

	m_nLoadState = LOADSTATE_SUCCEED;
	m_bNeedCreate = false;
	m_bUpdatedBoundingBox = false;

	return true;
}

void EffectModel::UpdateLinkParticleEmit()
{
	// ȡ�ù���������
	size_t n = m_ParticleInfo.size();

	if (0 == n || NULL == m_pModel)
	{
		return;
	}

	// ��õ�ǰ���
	int nFrame = m_pModel->GetCurrentFrame();
	int nFrameCount = m_pModel->GetFrameCount();

	for (size_t i = 0; i < n; ++i)
	{
		particle_info* pInfo = m_ParticleInfo[i];

		if (pInfo->open_emit == pInfo->close_emit)
		{
			continue;
		}
		else
		{
			// �����ж���ȱ�ݣ��� else ���ע�͡���
			if (nFrame >= pInfo->open_emit && nFrame < pInfo->close_emit)
			{
				StartParticleFromIndex(i);
			}
			else
			{
				// ���ж�Ϊ�����ʼ֡��Ϊ��0����Ϊ��1�������⣬
				// 3DMax�ж���֡�Ŵӡ�0����ʼ������������������Ч���ӣ�֡��Ҳ�ӡ�0����ʼ��
				// ������������ʱ��ϲ����ĩ֡��Ϊ��10����
				// ����ϰ�����������ֵ����ʵ��ĩ֡=10ʱ�����������11֡��0-10����
				// �� res\ini\effect\effectmodel.ini ����ʱ����֡�ű�д����1-10�����ˡ�0����֡��
				// ���������е���ʱ���� nFrame==0 ʱ�����ӷ������ͱ�ɾ���ˡ�
				// nFrame==1ʱ�������´�������˷�������ͣ��new�ڴ��ֲ�ͣ��delete�ڴ档
				// �����ڴ˼Ӹ��жϣ���ֹ�ڴ治ͣ��ɾ����
				if( !(nFrame==0 && pInfo->open_emit==1) )
				{
					StopParticleFromIndex(i);
				}
			}
		}
	}
}

void EffectModel::UpdateLinkLightEmit()
{
	// ȡ�ù�����Դ��
	size_t n = m_LightInfo.size();

	if (0 == n || NULL == m_pModel)
	{
		return;
	}

	for (size_t i = 0; i < n; ++i)
	{
		light_info_t* pInfo = m_LightInfo[i];

		// ��õ�ǰ���
		int nFrame = m_pModel->GetCurrentFrame();

		// ȡ�ù�Դ����ָ��
		LightSource* pLight = (LightSource*)GetCore()->GetEntity(
			pInfo->light_id);

		if (NULL == pLight)
		{
			continue;
		}

		if (pInfo->open_emit == pInfo->close_emit)
		{
			pLight->SetVisible(true);
		} 
		else if (pInfo->open_emit < pInfo->close_emit)
		{
			if (-1 == pInfo->open_emit)
			{
				if (pInfo->close_emit >= nFrame)
				{
					pLight->SetVisible(true);
				} 
				else
				{
					pLight->SetVisible(false);
				}
			} 
			else
			{
				if (pInfo->close_emit >= nFrame
					&& pInfo->open_emit <= nFrame)
				{
					pLight->SetVisible(true);
				} 
				else
				{
					pLight->SetVisible(false);
				}
			}
		} 
		else
		{
			if (-1 == pInfo->close_emit)
			{
				if (pInfo->open_emit >= nFrame)
				{
					pLight->SetVisible(false);
				} 
				else
				{
					pLight->SetVisible(true);
				}
			} 
			else
			{
				if ((pInfo->close_emit <= nFrame)
					&& (pInfo->open_emit >= nFrame))
				{
					pLight->SetVisible(false);
				} 
				else
				{
					pLight->SetVisible(true);
				}
			}
		}
	}
}

void EffectModel::GetHelperNameList(const IVarList& args, IVarList& result)
{
	if (m_pModel)
	{
		m_pModel->GetHelperNameList(args, result);
	}
}

void EffectModel::GetCurrentNodeMatrix(int nNodeID, FmMat4* pTM)
{
	if (m_pModel)
	{
		m_pModel->GetCurrentNodeMatrix(nNodeID, pTM);
	}
}

bool EffectModel::GetCurrentNodeMatrix(const char* pNodeName, FmMat4* pTM)
{
	if (NULL == m_pModel)
	{
		return false;
	}

	return m_pModel->GetCurrentNodeMatrix(pNodeName, pTM);
}

int	EffectModel::GetNodeFromName(const char* pszName)
{
	if (NULL == m_pModel)
	{
		return -1;
	}

	return m_pModel->GetNodeFromName(pszName);
}

void EffectModel::SetWorldMatrix(const FmMat4& mat)
{
	//Assert(mtx_is_valid(mat));

	m_bUpdatedBoundingBox = false;

	m_mtxWorld = mat;

	if (NULL == m_pModel)
	{
		return;
	}

	m_pModel->SetWorldMatrix(mat);
}

void EffectModel::GetWorldMatrix(FmMat4& mat)
{
	if (NULL == m_pModel)
	{
		mat = m_mtxWorld;
		return;
	}

	m_pModel->GetWorldMatrix(mat);
}

bool EffectModel::TraceDetail(const FmVec3& src, const FmVec3& dst, 
	trace_info_t& result)
{
	if (!m_bVisible)
	{
		return false;
	}

	/*
	if (m_bTraceBoxEnable && m_pRender->GetEditorMode() 
		&& !get_global_on_off(GetCore(), "walk_generate_state"))
	{
		// Ϊ���ڱ༭��������ѡ�񣬲��Ҳ�������������ײ��ʱ��
		FmMat4 mat;
		FmVec3 position = GetPosition();

		FmMatrixTranslation(&mat, position.x, position.y, position.z);

		float scale = this->GetRadius();

		if (scale < 0.1F)
		{
			scale = 0.1F;
		}

		if (scale > 1.0F)
		{
			scale = 1.0F;
		}

		int old_hit_count = result.nHitCount;
		
		bool succeed = VisUtil_TraceBoundBox(mat, 
			FmVec3(0.0F, scale * 0.5F, 0.0F), 
			FmVec3(scale, scale, scale), src, dst, result);

		if (succeed)
		{
			// ������ײ���Ķ���ID
			if (result.nHitMax > 1)
			{
				for (int t = old_hit_count; t < result.nHitCount; ++t)
				{
					result.Hits[t].TraceID = this->GetID();

					// ������о���ת��
					trace_info_t::hit_t* pHit = &result.Hits[t];
					FmMat4 mtxWorldInverse;

					FmMatrixInverse(&mtxWorldInverse, &m_mtxWorld);
					FmVec3TransformCoord(&pHit->Vertex[0], &pHit->Vertex[0], 
						&mtxWorldInverse);
					FmVec3TransformCoord(&pHit->Vertex[1], &pHit->Vertex[1], 
						&mtxWorldInverse);
					FmVec3TransformCoord(&pHit->Vertex[2], &pHit->Vertex[2], 
						&mtxWorldInverse);
				}
			}
			else
			{
				result.Hits[0].TraceID = this->GetID();

				// ������о���ת��
				trace_info_t::hit_t* pHit = &result.Hits[0];
				FmMat4 mtxWorldInverse;

				FmMatrixInverse(&mtxWorldInverse, &m_mtxWorld);
				FmVec3TransformCoord(&pHit->Vertex[0], &pHit->Vertex[0], 
					&mtxWorldInverse);
				FmVec3TransformCoord(&pHit->Vertex[1], &pHit->Vertex[1], 
					&mtxWorldInverse);
				FmVec3TransformCoord(&pHit->Vertex[2], &pHit->Vertex[2], 
					&mtxWorldInverse);
			}

			return succeed;
		}
	}*/

	if (!m_bTraceEnable)
	{
		return false;
	}

	if (m_pModel)
	{
		int old_hit_count = result.nHitCount;
		
		if (m_pModel->TraceDetail(src, dst, result))
		{
			if (result.nHitMax > 1)
			{
				for (int t = old_hit_count; t < result.nHitCount; ++t)
				{
					result.Hits[t].TraceID = this->GetID();
				}
			}
			else
			{
				result.Hits[0].TraceID = this->GetID();
			}

			return true;
		}
	}

	return false;
}

bool EffectModel::TraceHitted(const FmVec3& src, const FmVec3& dst)
{
	if (!m_bVisible)
	{
		return false;
	}

	if ((NULL == m_pModel) || (!m_bTraceEnable))
	{
		return false;
	}

	return m_pModel->TraceHitted(src, dst);
}

bool EffectModel::TraceSphere(const FmVec3& center, float radius)
{
	if (!m_bVisible)
	{
		return false;
	}

	if (NULL == m_pModel)
	{
		return false;
	}

	return m_pModel->TraceSphere(center, radius);
}

// ��������ײ
bool EffectModel::TraceTriangle(const FmVec3& v0, const FmVec3& v1, const FmVec3& v2)
{
    if (!m_bVisible)
    {
        return false;
    }

    if (NULL == m_pModel)
    {
        return false;
    }

    return m_pModel->TraceTriangle(v0, v1, v2);    
}

bool EffectModel::Cull(const FmPlane* planes, size_t plane_num, 
	const FmVec3& center, float radius, CULL_FUNC cull_func, 
	void* context)
{
	if (!m_bVisible)
	{
		return false;
	}

	if ((NULL == m_pModel) || (!m_bCullEnable))
	{
		return false;
	}

	return m_pModel->Cull(planes, plane_num, center, radius, cull_func, 
		context);
}

bool EffectModel::AddParticleParams(const char* particle_name, 
	const char* helper_name, int emit_open, int emit_close)
{
	Assert(particle_name != NULL);
	Assert(helper_name != NULL);

	if (m_strParticleFile.empty())
	{
		return false;
	}
	
	size_t count = m_ParticleInfo.size();

	for (size_t i = 0; i < count; ++i)
	{
		if (0 == m_ParticleInfo[i]->helper_name.compare(helper_name) 
			&& 0 == m_ParticleInfo[i]->particle_name.compare(particle_name))
		{
			return false;
		}
	}

	particle_info* emit_info = CORE_NEW(particle_info);

	emit_info->helper_name = helper_name;
	emit_info->particle_name = particle_name;
	emit_info->particle_id = PERSISTID();
	emit_info->open_emit = emit_open;
	emit_info->close_emit = emit_close;

	m_ParticleInfo.push_back(emit_info);

	return true;
}

bool EffectModel::AddParticle(const char* particle_name, 
	const char* helper_name, int emit_open, int emit_close)
{
	Assert(particle_name != NULL);
	Assert(helper_name != NULL);

	if (m_strParticleFile.empty())
	{
		return false;
	}
	
	size_t count = m_ParticleInfo.size();

	for (size_t i = 0; i < count; ++i)
	{
		if (0 == m_ParticleInfo[i]->helper_name.compare(helper_name) 
			&& 0 == m_ParticleInfo[i]->particle_name.compare(particle_name))
		{
			return false;
		}
	}

	PERSISTID particle_id = CreateParticleOnHelper(particle_name, helper_name);

	if (particle_id.IsNull())
	{
		return false;
	}

	particle_info* emit_info = CORE_NEW(particle_info);

	emit_info->helper_name = helper_name;
	emit_info->particle_name = particle_name;
	emit_info->particle_id = particle_id;
	emit_info->open_emit = emit_open;
	emit_info->close_emit = emit_close;

	m_ParticleInfo.push_back(emit_info);

	m_bUpdatedBoundingBox = false;

	return true;
}

bool EffectModel::DeleteParticleFromIndex(int index)
{
	Assert((size_t)index < m_ParticleInfo.size());

	if (size_t(index) >= m_ParticleInfo.size())
	{
		return false;
	}

	if (!m_ParticleInfo[index]->particle_id.IsNull())
	{
		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(
			m_ParticleInfo[index]->particle_id);

		SAFE_RELEASE(pVisBase);
	}

	CORE_DELETE(m_ParticleInfo[index]);
	m_ParticleInfo.remove(index);
	m_bUpdatedBoundingBox = false;

	return true;
}

bool EffectModel::StartParticleFromIndex(size_t index)
{
	Assert((size_t)index < m_ParticleInfo.size());

	PERSISTID particle_id = m_ParticleInfo[index]->particle_id;

	if (particle_id.IsNull())
	{
		const char* particle_name = m_ParticleInfo[index]->particle_name.c_str();
		const char* helper_name = m_ParticleInfo[index]->helper_name.c_str();

		Assert(particle_name != NULL);
		Assert(helper_name != NULL);

		if (m_strParticleFile.empty())
		{
			return false;
		}

		PERSISTID particle_id = CreateParticleOnHelper(particle_name, helper_name);

		if (particle_id.IsNull())
		{
			return false;
		}

		FmVec3 scale = GetScale();

		IVisBase* pParticle = (IVisBase*)GetCore()->GetEntity(
			particle_id);

		pParticle->SetScale(scale.x, scale.y, scale.z);

		m_ParticleInfo[index]->particle_id = particle_id;

		m_bUpdatedBoundingBox = false;
	}

	return true;
}

bool EffectModel::StopParticleFromIndex(size_t index)
{
	Assert((size_t)index < m_ParticleInfo.size());

	if (!m_ParticleInfo[index]->particle_id.IsNull())
	{
		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(
			m_ParticleInfo[index]->particle_id);

		SAFE_RELEASE(pVisBase);

		m_ParticleInfo[index]->particle_id = PERSISTID();
		m_bUpdatedBoundingBox = false;
	}

	return true;
}

bool EffectModel::DeleteParticleOnHelper(const char* helper_name)
{
	size_t count = m_ParticleInfo.size();

	for (int i = int(count) - 1; i >= 0; --i)
	{
		if (0 == m_ParticleInfo[i]->helper_name.compare(helper_name))
		{
			DeleteParticleFromIndex(i);
		}
	}

	m_bUpdatedBoundingBox = false;

	return true;
}

bool EffectModel::DeleteParticleFromName(const char* helper_name, 
	const char* particle_name)
{
	size_t count = m_ParticleInfo.size();

	for (int i = int(count) - 1; i >= 0; --i)
	{
		if (0 == m_ParticleInfo[i]->helper_name.compare(helper_name) 
			&& 0 == m_ParticleInfo[i]->particle_name.compare(particle_name))
		{
			m_bUpdatedBoundingBox = false;
			return DeleteParticleFromIndex(i);
		}
	}

	return false;
}

PERSISTID EffectModel::GetParticleFromName(const char* helper_name, 
	const char* particle_name)
{
	size_t count = m_ParticleInfo.size();

	for (size_t i = 0; i < count; ++i)
	{
		if (0 == m_ParticleInfo[i]->helper_name.compare(helper_name) 
			&& 0 == m_ParticleInfo[i]->particle_name.compare(particle_name))
		{
			return m_ParticleInfo[i]->particle_id;
		}
	}

	return PERSISTID();
}

PERSISTID EffectModel::GetParticleFromIndex(int index) const
{
	Assert((size_t)index < m_ParticleInfo.size());

	if (size_t(index) >= m_ParticleInfo.size())
	{
		return PERSISTID();
	}

	return m_ParticleInfo[index]->particle_id;
}

int EffectModel::GetParticleCount()
{
	return (int)m_ParticleInfo.size();
}

void EffectModel::GetParticleInfoFromIndex(const IVarList& args, IVarList& result)
{
	size_t index = args.IntVal(0);

	Assert((size_t)index < m_ParticleInfo.size());

	if (size_t(index) >= m_ParticleInfo.size())
	{
		return;
	}

	particle_info* pParticle = m_ParticleInfo[index];

	result.AddString(pParticle->helper_name.c_str());
	result.AddString(pParticle->particle_name.c_str());
}

bool EffectModel::SetParticleEmit(const char* helper_name, int open_emit, 
	int close_emit)
{
	size_t count = m_ParticleInfo.size();

	for (size_t i = 0; i < count; ++i)
	{
		if (0 == m_ParticleInfo[i]->helper_name.compare(helper_name))
		{
			m_ParticleInfo[i]->open_emit = open_emit;
			m_ParticleInfo[i]->close_emit = close_emit;
		}
	}

	return true;
}

void EffectModel::GetParticleEmit(const IVarList& args, IVarList& result)
{
	const char* helper_name = args.StringVal(0);

	if (helper_name)
	{
		size_t count = m_ParticleInfo.size();

		for (size_t i = 0; i < count; ++i)
		{
			if (0 == m_ParticleInfo[i]->helper_name.compare(helper_name))
			{
				result.AddInt(m_ParticleInfo[i]->open_emit);
				result.AddInt(m_ParticleInfo[i]->close_emit);
				return;
			}
		}
	}
	
	result.AddInt(-1);
	result.AddInt(-1);
}

bool EffectModel::SaveEffectModel(const char* file_name, const char* effect_name)
{
	Assert(file_name != NULL);
	Assert(effect_name != NULL);

	if (NULL == m_pModel)
	{
		return false;
	}

	char fname[256];

	SafeSprintf(fname, sizeof(fname), "%s%s", 
		m_pRender->GetPathPrefix(file_name), file_name);

	IIniFile* pIni = GetCore()->CreateIniFile(fname);

	pIni->LoadFromFile();
	
	if (pIni->FindSection(effect_name))
	{
		pIni->DeleteSection(effect_name);
	}

	pIni->WriteString(effect_name, "Model", m_pModel->GetModelFile());

	for (size_t i = 0; i < m_ParticleInfo.size(); ++i)
	{
		char sKeyName[128];

		SafeSprintf(sKeyName, sizeof(sKeyName), "particle%d", i);
		pIni->WriteString(effect_name, sKeyName, 
			m_ParticleInfo[i]->particle_name.c_str());

		SafeSprintf(sKeyName, sizeof(sKeyName), "idhelper%d", i);
		pIni->WriteString(effect_name, sKeyName, 
			m_ParticleInfo[i]->helper_name.c_str());

		SafeSprintf(sKeyName, sizeof(sKeyName), "EmitOpen%d", i);
		pIni->WriteInteger(effect_name, sKeyName, 
			m_ParticleInfo[i]->open_emit);

		SafeSprintf(sKeyName, sizeof(sKeyName), "EmitClose%d", i);
		pIni->WriteInteger(effect_name, sKeyName, 
			m_ParticleInfo[i]->close_emit);
	}

	if (m_ParticleInfo.size() > 0)
	{
		pIni->WriteString(effect_name, "particle_file", 
			m_strParticleFile.c_str());
		pIni->WriteInteger(effect_name, "number", (int)m_ParticleInfo.size());
	}

	// ������β��Ϣ
	for (size_t i = 0; i < m_SaberArcInfo.size(); ++i)
	{
		char strItemName[32];

		SafeSprintf(strItemName, sizeof(strItemName), "saberarc%d_Point1", i);
		pIni->WriteString(effect_name, strItemName, 
			m_SaberArcInfo[i]->helper_name0.c_str());

		SafeSprintf(strItemName, sizeof(strItemName), "saberarc%d_Point2", i);
		pIni->WriteString(effect_name, strItemName, 
			m_SaberArcInfo[i]->helper_name1.c_str());

		SafeSprintf(strItemName, sizeof(strItemName), "saberarc%d_Name", i);
		pIni->WriteString(effect_name, strItemName, 
			m_SaberArcInfo[i]->saberarc_name.c_str());

		SafeSprintf(strItemName, sizeof(strItemName), "saberarc%d_IsStart", i);

		SaberArc* sa = (SaberArc*)GetCore()->GetEntity(
			m_SaberArcInfo[i]->saberarc_id);

		if (sa && sa->GetStart())
		{
			pIni->WriteString(effect_name, strItemName, "1");
		} 
		else
		{
			pIni->DeleteItem(effect_name, strItemName);
		}
	}

	if (m_SaberArcInfo.size() > 0)
	{
		pIni->WriteInteger(effect_name, "saberarc_number", 
			(int)m_SaberArcInfo.size());
		pIni->WriteString(effect_name, "saberarc_file", 
			m_strSaberArcFile.c_str());
	}

	// �����Դ����
	for (size_t i = 0; i < m_LightInfo.size(); ++i)
	{
		char sKeyName[128];

		SafeSprintf(sKeyName, sizeof(sKeyName), "light_name%d", i);
		pIni->WriteString(effect_name, sKeyName, 
			m_LightInfo[i]->light_name.c_str());

		SafeSprintf(sKeyName, sizeof(sKeyName), "light_helper%d", i);
		pIni->WriteString(effect_name, sKeyName, 
			m_LightInfo[i]->helper_name.c_str());

		SafeSprintf(sKeyName, sizeof(sKeyName), "light_visible_%d", i);
		pIni->WriteInteger(effect_name, sKeyName, m_LightInfo[i]->open_emit);

		SafeSprintf(sKeyName, sizeof(sKeyName), "light_invisible_%d", i);
		pIni->WriteInteger(effect_name, sKeyName, m_LightInfo[i]->close_emit);
	}

	if (m_LightInfo.size() > 0)
	{
		pIni->WriteString(effect_name, "light_file", m_strLightFile.c_str());
		pIni->WriteInteger(effect_name, "light_number", 
			(int)m_LightInfo.size());
	}

	bool succeed = pIni->SaveToFile();
	
	pIni->Release();
	
	return succeed;
}

bool EffectModel::AddSaberArcParams(const char* saberarc_name, 
	const char* helper_name0, const char* helper_name1, bool is_start)
{
	Assert(saberarc_name != NULL);
	Assert(helper_name0 != NULL);
	Assert(helper_name1 != NULL);

	if (m_strSaberArcFile.empty())
	{
		return false;
	}

	saberarc_info* pSaberArcInfo = CORE_NEW(saberarc_info);

	pSaberArcInfo->saberarc_id = PERSISTID();
	pSaberArcInfo->is_saber_start = is_start;
	pSaberArcInfo->saberarc_name = saberarc_name;
	pSaberArcInfo->helper_name0 = helper_name0;
	pSaberArcInfo->helper_name1 = helper_name1;

	m_SaberArcInfo.push_back(pSaberArcInfo);

	return true;
}

bool EffectModel::AddSaberArc(PERSISTID& id)
{
	SaberArc* sa = (SaberArc*)GetCore()->GetEntity(id);

	if (NULL == sa)
	{
		return false;
	}

	CVarList args, result;

	sa->GetHelperPointName(args, result);

	saberarc_info* pSaberArcInfo = CORE_NEW(saberarc_info);

	pSaberArcInfo->saberarc_id = id;
	pSaberArcInfo->saberarc_name = sa->GetName();
	pSaberArcInfo->helper_name0 = result.StringVal(0);
	pSaberArcInfo->helper_name1 = result.StringVal(1);

	m_SaberArcInfo.push_back(pSaberArcInfo);

	m_bUpdatedBoundingBox = false;

	return true;
}

bool EffectModel::DeleteSaberArc(int index)
{
	Assert((size_t)index < m_SaberArcInfo.size());

	if (size_t(index) >= m_SaberArcInfo.size())
	{
		return false;
	}

	if (!(m_SaberArcInfo[index]->saberarc_id).IsNull())
	{
		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(
			m_SaberArcInfo[index]->saberarc_id);

		SAFE_RELEASE(pVisBase);
	}

	CORE_DELETE(m_SaberArcInfo[index]);
	m_SaberArcInfo.remove(index);
	m_bUpdatedBoundingBox = false;

	return true;
}

PERSISTID EffectModel::GetSaberArc(int index) const
{
	Assert((size_t)index < m_SaberArcInfo.size());

	if (size_t(index) >= m_SaberArcInfo.size())
	{
		return PERSISTID();
	}

	return m_SaberArcInfo[index]->saberarc_id;
}

int EffectModel::GetSaberArcCount()
{
	return (int)m_SaberArcInfo.size();
}

void EffectModel::GetSaberArcInfo(const IVarList& args, IVarList& result)
{
	PERSISTID saberarc_id = args.ObjectVal(0);

	if (saberarc_id.IsNull())
	{
		return;
	}

	size_t count = m_SaberArcInfo.size();

	for (size_t i = 0; i < count; ++i)
	{
		saberarc_info* pSaberarc = m_SaberArcInfo[i];

		if (saberarc_id.nIdent == pSaberarc->saberarc_id.nIdent 
			&& saberarc_id.nSerial == pSaberarc->saberarc_id.nSerial)
		{
			result.AddString(pSaberarc->helper_name0.c_str());
			result.AddString(pSaberarc->helper_name1.c_str());
			result.AddString(pSaberarc->saberarc_name.c_str());
			return;
		}
	}
}

bool EffectModel::AddLightParams(const char* light_name, 
	const char* helper_name, int emit_open, int emit_close)
{
	Assert(light_name != NULL);
	Assert(helper_name != NULL);

	if (m_strLightFile.empty())
	{
		return false;
	}

	size_t count = m_LightInfo.size();

	for (size_t i = 0; i < count; ++i)
	{
		if (0 == m_LightInfo[i]->helper_name.compare(helper_name) 
			&& 0 == m_LightInfo[i]->light_name.compare(light_name))
		{
			return false;
		}
	}

	light_info_t* pLightInfo = CORE_NEW(light_info_t);

	pLightInfo->light_id = PERSISTID();
	pLightInfo->light_name = light_name;
	pLightInfo->helper_name = helper_name;
	pLightInfo->open_emit = emit_open;
	pLightInfo->close_emit = emit_close;

	m_LightInfo.push_back(pLightInfo);

	return true;
}

bool EffectModel::AddLight(const char* light_name, const char* helper_name, 
	int emit_open, int emit_close)
{
	Assert(light_name != NULL);
	Assert(helper_name != NULL);

	if (m_strLightFile.empty())
	{
		return false;
	}

	size_t count = m_LightInfo.size();

	for (size_t i = 0; i < count; ++i)
	{
		if (0 == m_LightInfo[i]->helper_name.compare(helper_name) 
			&& 0 == m_LightInfo[i]->light_name.compare(light_name))
		{
			return false;
		}
	}

	PERSISTID light_id = CreateLightOnHelper(light_name, helper_name);

	if (light_id.IsNull())
	{
		return false;
	}

	light_info_t* pLightInfo = CORE_NEW(light_info_t);

	pLightInfo->helper_name = helper_name;
	pLightInfo->light_name = light_name;
	pLightInfo->light_id = light_id;
	pLightInfo->open_emit = emit_open;
	pLightInfo->close_emit = emit_close;

	m_LightInfo.push_back(pLightInfo);

	m_bUpdatedBoundingBox = false;

	return true;
}

bool EffectModel::DeleteLightFromIndex(int index)
{
	Assert((size_t)index < m_LightInfo.size());

	if (size_t(index) >= m_LightInfo.size())
	{
		return false;
	}

	if (!m_LightInfo[index]->light_id.IsNull())
	{
		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(
			m_LightInfo[index]->light_id);

		SAFE_RELEASE(pVisBase);
	}

	CORE_DELETE(m_LightInfo[index]);
	m_LightInfo.remove(index);
	m_bUpdatedBoundingBox = false;

	return true;
}

bool EffectModel::DeleteLightOnHelper(const char* helper_name)
{
	size_t count = m_LightInfo.size();

	for (int i = int(count) - 1; i >= 0; --i)
	{
		if (0 == m_LightInfo[i]->helper_name.compare(helper_name))
		{
			DeleteLightFromIndex(i);
		}
	}

	m_bUpdatedBoundingBox = false;

	return true;
}

bool EffectModel::DeleteLightFromName(const char* helper_name, const char* light_name)
{
	size_t count = m_LightInfo.size();

	for (int i = int(count) - 1; i >= 0; --i)
	{
		if (0 == m_LightInfo[i]->helper_name.compare(helper_name) 
			&& 0 == m_LightInfo[i]->light_name.compare(light_name))
		{
			m_bUpdatedBoundingBox = false;
			return DeleteLightFromIndex(i);
		}
	}

	return false;
}

PERSISTID EffectModel::GetLightFromName(const char* helper_name, 
	const char* light_name)
{
	size_t count = m_LightInfo.size();

	for (size_t i = 0; i < count; ++i)
	{
		if (0 == m_LightInfo[i]->helper_name.compare(helper_name) 
			&& 0 == m_LightInfo[i]->light_name.compare(light_name))
		{
			return m_LightInfo[i]->light_id;
		}
	}

	return PERSISTID();
}

PERSISTID EffectModel::GetLightFromIndex(int index) const
{
	Assert((size_t)index < m_LightInfo.size());

	if (size_t(index) >= m_LightInfo.size())
	{
		return PERSISTID();
	}

	return m_LightInfo[index]->light_id;
}

int EffectModel::GetLightCount()
{
	return (int)m_LightInfo.size();
}

void EffectModel::GetLightInfo(const IVarList& args, IVarList& result)
{
	PERSISTID light_id = args.ObjectVal(0);

	if (light_id.IsNull())
	{
		return;
	}

	size_t count = m_LightInfo.size();

	for (size_t i = 0; i < count; ++i)
	{
		light_info_t* pLight = m_LightInfo[i];

		if (light_id.nIdent == pLight->light_id.nIdent 
			&& light_id.nSerial == pLight->light_id.nSerial)
		{
			result.AddString(pLight->helper_name.c_str());
			result.AddString(pLight->light_name.c_str());
			return;
		}
	}
}

bool EffectModel::SetLightEmit(const char* helper_name, int open_emit, 
	int close_emit)
{
	size_t count = m_LightInfo.size();

	for (size_t i = 0; i < count; ++i)
	{
		if (0 == m_LightInfo[i]->helper_name.compare(helper_name))
		{
			m_LightInfo[i]->open_emit = open_emit;
			m_LightInfo[i]->close_emit = close_emit;
		}
	}

	return true;
}

void EffectModel::GetLightEmit(const IVarList& args, IVarList& result)
{
	const char* helper_name = args.StringVal(0);

	if (helper_name)
	{
		size_t count = m_LightInfo.size();

		for (size_t i = 0; i < count; ++i)
		{
			if (0 == m_LightInfo[i]->helper_name.compare(helper_name))
			{
				result.AddInt(m_LightInfo[i]->open_emit);
				result.AddInt(m_LightInfo[i]->close_emit);
				return;
			}
		}
	}

	result.AddInt(-1);
	result.AddInt(-1);
}

bool EffectModel::SetBrokenShadow(bool value)
{
	BrokenShadow* pBrokenShadow = (BrokenShadow*)GetCore()->GetEntity(
		m_BrokenShadow);

	if (value && (pBrokenShadow == NULL) && (m_pModel != NULL))
	{
		// ����
		pBrokenShadow = (BrokenShadow*)GetCore()->CreateEntityArgs(
			"BrokenShadow", CVarList());

		if (pBrokenShadow != NULL)
		{
			pBrokenShadow->SetContext(m_pContext);
			pBrokenShadow->SetObject(m_pModel->GetID());
			pBrokenShadow->Load();

			m_BrokenShadow = pBrokenShadow->GetID();
		}
		else
		{
			return false;
		}
	}

	return true;
}

PERSISTID EffectModel::GetBrokenShadow() const
{
	return m_BrokenShadow;
}

size_t EffectModel::GetMemoryUsage()
{
	size_t size = sizeof(ParticleManager*) + sizeof(LightManager*) + 
		sizeof(SaberArcManager*) + sizeof(Model*) + sizeof(FmMat4);

	size += m_ParticleInfo.get_memory_usage();
	size += m_SaberArcInfo.get_memory_usage();
	size += m_LightInfo.get_memory_usage();

	size += m_strAppendPath.length() * sizeof(char);
	size += m_strModelFile.length() * sizeof(char);
	size += m_strMaterialFile.length() * sizeof(char);
	size += m_strParticleFile.length() * sizeof(char);
	size += m_strSaberArcFile.length() * sizeof(char);
	size += m_strLightFile.length() * sizeof(char);
	size += m_strConfigFile.length() * sizeof(char);
	size += m_strEffectName.length() * sizeof(char);

	size += sizeof(m_bNeedCreate) + sizeof(m_bModelChanged) 
		+ sizeof(m_bVisible) + sizeof(m_bShowParticle) 
		+ sizeof(m_bCalParticleSize) + sizeof(m_bUpdateNodeMat) 
		+ sizeof(m_bLoop) + sizeof(m_bIsOneOff) + sizeof(m_bTraceEnable) 
		+ sizeof(m_bCullEnable) + sizeof(m_bTraceBoxEnable) 
		+ sizeof(m_bWaterReflect) + sizeof(m_bAsyncLoad) 
		+ sizeof(m_bShowBoundBox) + sizeof(m_bUpdatedBoundingBox) + sizeof(m_bSleep) 
		+ sizeof(m_fAlphaValue) + sizeof(m_fCurrentTime) + sizeof(m_fLifeTime) 
		+ sizeof(m_fSpeed) + sizeof(m_BrokenShadow) + sizeof(m_vBoundMin) 
		+ sizeof(m_vBoundMax) + sizeof(m_vCenter) + sizeof(m_fRadius);

	return size;
}

void EffectModel::SetSleep(bool value)
{
    m_bSleep = value;

    size_t nSaberArcCount = m_SaberArcInfo.size();

    if (nSaberArcCount > 0)
    {
        for (size_t i = 0; i < nSaberArcCount; ++i)
        {
            SaberArc* pSaberArc = (SaberArc*)GetCore()->GetEntity(
                m_SaberArcInfo[i]->saberarc_id);
            if (pSaberArc)
            {
                pSaberArc->SetSleep(value);
            }
        }
    }
}
