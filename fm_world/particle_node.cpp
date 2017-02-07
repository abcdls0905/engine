//--------------------------------------------------------------------
// 文件名:		particle_node.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年9月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "particle_node.h"
#include "particle_manager.h"
#include "../visual/i_texture.h"
#include "../visual/i_context.h"
#include "../visual/i_scene_view.h"
//#include "../visual/i_resource.h"
#include "model.h"
//#include "../visual/i_model_player.h"
//#include "../visual/vis_utils.h"

#include "../public/inlines.h"
#include "../public/auto_mem.h"
#include "../public/core_log.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>

#define VERTEX_ARRAY        0
#define VERTEX_COLOR_ARRAY  1
#define TEXCOORD_ARRAY      2

#define GPU_VERTEX          0
#define GPU_VERTEX_NORMAL   1
#define GPU_VERTEX_UV       2
#define GPU_VERTEX_POSDELTA 3
#define GPU_VERTEX_RANDOM0  4
#define GPU_VERTEX_RANDOM1  5
#define GPU_VERTEX_RANDOM2  6

#include "../public/portable.h"
#define performance_time Port_GetPerformanceTime

//#define PARTICLE_FVF (D3DFVF_XYZW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
//#define PARTICLE_GPU_FVF (D3DFVF_XYZW | D3DFVF_NORMAL | D3DFVF_TEX5)

// 随机数种子
static unsigned long s_nRandSeed = (unsigned long)time(NULL);

// 设置随机数种子
static inline void set_rand_seed(unsigned int value)
{
	s_nRandSeed = value;
}

// 返回随机整数
static inline int rand_int()
{
	s_nRandSeed = s_nRandSeed * 214013L + 2531011L;

	return (s_nRandSeed >> 16) & 0x7FFF;
}

// 返回随机颜色
static inline unsigned int rand_color()
{
	s_nRandSeed = (s_nRandSeed >> 16) * 214013L + 2531011L;

	return s_nRandSeed;
}

// 返回正负0.5之间的随机数
static inline float rand_half()
{
	s_nRandSeed = s_nRandSeed * 214013L + 2531011L;

	float value =  0.5F - float((s_nRandSeed >> 16) & 0x7FFF) / float(RAND_MAX_WIN);
	return value;
}

// 返回0到1之间的随机数
static inline float rand_one()
{
	s_nRandSeed = s_nRandSeed * 214013L + 2531011L;

	return float((s_nRandSeed >> 16) & 0x7FFF) / float(RAND_MAX_WIN);
}

// 返回0到1之间的随机数
//static inline float rand_one_s()
//{
//	unsigned int num = 0;
//	errno_t err = rand_s(&num);
//
//	return (float)num / float(UINT_MAX);
//}

static inline float trace_value(FmVec4* trackdata, float time)
{
	FmVec4* track0 = trackdata;

	track0 += 7;
	track0 += (time < track0->w) ? -4 : 4;
	track0 += (time < track0->w) ? -2 : 2;
	track0 += (time < track0->w) ? -1 : 1;
	track0 += (time < track0->w) ? -1 : 0;

	FmVec4* track1 = track0 + 1;
	float time0 = track0->w;
	float key_time = track1->w - time0;

	if (key_time > 0)
	{
		float blend_value = (time - time0) / key_time;

		return track0->x * (1 - blend_value) + track1->x * blend_value;
	}
	else
	{
		return track0->x;
	}
}

static inline float trace_speed(FmVec4* trackdata, float time)
{
	FmVec4* track0 = trackdata;

	track0 += 7;
	track0 += (time < track0->w) ? -4 : 4;
	track0 += (time < track0->w) ? -2 : 2;
	track0 += (time < track0->w) ? -1 : 1;
	track0 += (time < track0->w) ? -1 : 0;

	FmVec4* track1 = track0 + 1;
	FmVec4* pos0 = track0 + 16;
	float time0 = track0->w;
	float key_time = track1->w - time0;
	float current_time = time - time0;

	if (key_time > 0.0F)
	{
		float blend_value = current_time / key_time;
		float lastkey_speed = track0->x;
		float current_speed = track0->x * (1 - blend_value) 
			+ track1->x * blend_value;
		
		return pos0->x + (lastkey_speed + current_speed) * current_time * 0.5F;
	}
	else
	{
		return pos0->x + track0->x * current_time;
	}
}

static inline FmVec3 trace_speed_vector3(FmVec4* trackdata, float time)
{
	FmVec4* track0 = trackdata;

	track0 += 7;
	track0 += (time < track0->w) ? -4 : 4;
	track0 += (time < track0->w) ? -2 : 2;
	track0 += (time < track0->w) ? -1 : 1;
	track0 += (time < track0->w) ? -1 : 0;

	FmVec4* track1 = track0 + 1;
	//FmVec3 pos0 = *(FmVec3*)track0 + 16;D

	FmVec3 pos0 = * (FmVec3*)((VERTTYPE*)track0 + 16);

	float time0 = track0->w;
	float key_time = track1->w - time0;
	float current_time = time - time0;

	if (key_time > 0.0F)
	{
		float blend_value = current_time / key_time;
		FmVec3* lastkey_speed = (FmVec3*)track0;
		FmVec3* nextkey_speed = (FmVec3*)track1;
		
		return pos0 + (*lastkey_speed * 2 
			+ (*nextkey_speed - *lastkey_speed) * blend_value) 
			* current_time * 0.5F;
	}
	else
	{
		return pos0 + *(FmVec3*)track0 * current_time;
	}
}

// CParticleNode
CParticleNode* CParticleNode::NewInstance(ParticleManager* pManager, 
	IRender* pRender)
{
	CParticleNode* p = (CParticleNode*)CORE_ALLOC(sizeof(CParticleNode));
	
	new (p) CParticleNode(pManager, pRender);

	return p;
}

CParticleNode::CParticleNode(ParticleManager* pManager, IRender* pRender)
{
	Assert(pManager != NULL);
	Assert(pRender != NULL);

	m_pParticleManager = pManager;
	m_pRender = pRender;
	m_pNext = NULL;
	m_pPrev = NULL;
	m_pData = NULL;
	
	m_pVBBufferDataCPU = NULL;
	m_pVBBufferDataGPU = NULL;

	m_pVBuffer = NULL;
	m_pIBuffer = NULL;

	m_pTexture = NULL;
	m_pRandomColorTex = NULL;
	m_pRefractionTex = NULL;
	
	m_Particle = NULL;
	m_nParticleNum = 0;
	m_nDrawParticleNum = 0;
	m_nRandSeed = 0;
	m_nParticleColor = 0xFFFFFFFF;
	m_nParticlAppendColor = 0xFFFFFFFF;
	m_nTailCount = 1;
	m_fParticleLife = 1.0f;
	m_bRepeat = false;
	
	m_vLastEmitterPos.x = 0.0F;
	m_vLastEmitterPos.y = 0.0F;
	m_vLastEmitterPos.z = 0.0F;
	m_vEmitterPos.x = 0.0F;
	m_vEmitterPos.y = 0.0F;
	m_vEmitterPos.z = 0.0F;
	m_vEmitterDir.x = 0.0F;
	m_vEmitterDir.y = 1.0F;
	m_vEmitterDir.z = 0.0F;
	m_vEmitterAngle.x = 0.0F;
	m_vEmitterAngle.y = 0.0F;
	m_vEmitterAngle.z = 0.0F;
	m_vScale.x = 1.0F;
	m_vScale.y = 1.0F;
	m_vScale.z = 1.0F;
	m_vWindDirection.x = 0.0F;
	m_vWindDirection.y = 0.0F;
	m_vWindDirection.z = 0.0F;
	m_fWindPower = 0.0F;

	FmMat4Identity(&m_mtxLink);

	m_vBoxMin = FmVec3(0.0F, 0.0F, 0.0F);
	m_vBoxMax = FmVec3(0.0F, 0.0F, 0.0F);
	m_vCenter = FmVec3(0.0F, 0.0F, 0.0F);
	m_fRadius = 0.0F;
	
	m_fSystemLifeTime = 0.0F;
	m_fSpeedFactor = 1.0F;
	m_fLodEnd = 0.0F;
	m_fLodValue = 1.0F;
	m_fLastRefeshTime = 0.0F;
	m_fTimeAfterRelocate = 0.0F;
	m_bUpdateBoundBox = true;
	m_bUpdateScale = true;
	m_bVisible = true;
	m_bUnderWater = false;
	m_bEmitSwitch = true;
	m_bClipPlaneEnable = false;

	m_bSoftParticle = false;
	m_bWaveParticle = false;
	m_nComputeType = STATE_PARTICLE_CPU;
	m_nBlendType = STATE_BLEND_DEFAULT;
	m_nBillboardType = STATE_BILLBOARD_DEFAULT;

	m_nParticleEndTime = INT_MAX;
	m_dTimeCounter = 0.0;
	m_dTimeRealized = 0.0;

	m_bPause = false;
	m_bShowState = false;
	m_bComplete = false;
	m_bLinkEmitter = false;

	m_vLinkPos = FmVec3(0.0F, 0.0F, 0.0F);
	m_vLinkDirPos = FmVec3(0.0F, 1.0F, 0.0F);
	m_vClipPlane = FmVec4(0.0F, 1.0F, 0.0F, 0.0F);

	m_nHelperID = -1;
}

CParticleNode::~CParticleNode()
{
	if (m_Particle)
	{
		CORE_FREE(m_Particle, sizeof(particle_t) * m_nParticleNum); 
		m_Particle = NULL;
	}

	SAFE_RELEASE(m_pData);
	SAFE_RELEASE(m_pVBuffer);
	//SAFE_RELEASE(m_pDynamicVBuffer);
	SAFE_RELEASE(m_pIBuffer);

	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pRandomColorTex);
	SAFE_RELEASE(m_pRefractionTex);
	if(m_pVBBufferDataCPU)
		delete [] m_pVBBufferDataCPU;
	if(m_pVBBufferDataGPU)
		delete [] m_pVBBufferDataGPU;
}

void CParticleNode::Release()
{
	CORE_DELETE(this);
}

bool CParticleNode::GetLoadFinish()
{
	if (NULL == m_pTexture)
	{
		return true;
	}

	return m_pTexture->IsLoadComplete();
}

bool CParticleNode::Create(CParticleData* pdata, bool async_load)
{
	Assert(pdata != NULL);

	pdata->IncRefs();

	m_pData = pdata;
	m_nParticleNum = FM_MIN(pdata->m_nOneEmitNumber 
		* int(pdata->m_nParticleLife / pdata->m_fEmissionTime + 1),
		pdata->m_nParticleNum);
	m_nParticleColor = pdata->m_nParticleColor;
	m_fParticleLife = pdata->m_nParticleLife * 0.001F;
	SetRepeat(pdata->m_bRepeat);

	m_nComputeType = STATE_PARTICLE_GPU;

	if (pdata->m_strCalculateMode.length() >= 3)
	{
		const char* mode = pdata->m_strCalculateMode.c_str();

		if (strnicmp(mode, "GPU", 3) == 0)
		{
			m_nComputeType = STATE_PARTICLE_GPU;
		}
		else if (strnicmp(mode, "GLO", 3) == 0)
		{
			m_nComputeType = STATE_PARTICLE_GLOBAL;
		}
		else if (strnicmp(mode, "CPU", 3) == 0)
		{
			m_nComputeType = STATE_PARTICLE_CPU;
		}
	}

	if (pdata->m_bFlashTail || pdata->m_bTrackPointReturn)
	{
		// 有拖尾和回收标志的粒子只能用CPU运算
		m_nComputeType = STATE_PARTICLE_CPU;
	}
	else if (pdata->m_bLocalSpaceEmitter 
		&& m_nComputeType == STATE_PARTICLE_AUTO)
	{
		m_nComputeType = STATE_PARTICLE_GPU;
	}

	const char* effect = pdata->m_strEffectName.c_str();
	size_t effect_len = pdata->m_strEffectName.length();

	if (effect_len >= 5)
	{
		m_bSoftParticle = strnicmp(effect, "soft", 4) == 0;
		m_bWaveParticle = strnicmp(effect, "wave", 4) == 0;
	}
	else
	{
		m_bSoftParticle = false;
		m_bWaveParticle = false;
	}
	
	// 混合模式
	if (m_bSoftParticle || m_bWaveParticle)
	{
		if (stricmp(effect + 5, "particles_fire") == 0)
		{
			m_nBlendType = STATE_BLEND_ADD;
		}
		else if (stricmp(effect + 5, "multiply") == 0)
		{
			m_nBlendType = STATE_BLEND_MULTIPLY;
		}
		else
		{
			m_nBlendType = STATE_BLEND_DEFAULT;
		}
	}
	else
	{
		if (stricmp(effect, "particles_fire") == 0)
		{
			m_nBlendType = STATE_BLEND_ADD;
		}
		else if (stricmp(effect, "multiply") == 0)
		{
			m_nBlendType = STATE_BLEND_MULTIPLY;
		}
		else
		{
			m_nBlendType = STATE_BLEND_DEFAULT;
		}
	}

	// 公告板模式
	if (stricmp(pdata->m_strBillboardType.c_str(), "billboard_y") == 0)
	{
		m_nBillboardType = STATE_BILLBOARD_Y;
	}
	else if (stricmp(pdata->m_strBillboardType.c_str(), "none") == 0)
	{
		m_nBillboardType = STATE_BILLBOARD_NONE;
	}
	else
	{
		m_nBillboardType = STATE_BILLBOARD_DEFAULT;
	}

	Assert(NULL == m_Particle);

#ifdef _DEBUG
	if (m_nParticleNum > 4000)
	{
		char info[256];
		SafeSprintf(info, sizeof(info), "(CParticleNode::Create)(%s)(%s)"
			"particle num = %d", GetConfig(), GetName(), m_nParticleNum);
		CORE_TRACE(info);
	}
#endif // _DEBUG

	m_Particle = (particle_t*)CORE_ALLOC(sizeof(particle_t) * m_nParticleNum);
	memset(m_Particle, 0, m_nParticleNum * sizeof(particle_t));

	// 初始化粒子随机种子
	m_nRandSeed = rand_int();

	if (!pdata->m_strTextureName.empty())
	{
		m_pTexture = m_pRender->CreateTexture(
			pdata->m_strTextureName.c_str(), async_load);
		if (NULL == m_pTexture)
		{
			CORE_TRACE("(CParticleNode::Create)load texture failed");
			CORE_TRACE(pdata->m_strTextureName.c_str());
			return false;
		}
	}

	if (!pdata->m_strRandomColorTex.empty())
	{
		m_pRandomColorTex = m_pRender->CreateTexture(
			pdata->m_strRandomColorTex.c_str(), async_load);
		if (NULL == m_pRandomColorTex)
		{
			CORE_TRACE("(CParticleNode::Create)load texture failed");
			CORE_TRACE(pdata->m_strRandomColorTex.c_str());
			return false;
		}
	}

	if (!pdata->m_strRefractionTex.empty())
	{
		m_pRefractionTex = m_pRender->CreateTexture(
			pdata->m_strRefractionTex.c_str(), async_load);
		if (NULL == m_pRefractionTex)
		{
			CORE_TRACE("(CParticleNode::Create)load refraction tex failed");
			CORE_TRACE(pdata->m_strRefractionTex.c_str());
			return false;
		}
	}

	if (pdata->m_bStaticBoundBox)
	{
		m_vBoxMin = pdata->m_vBoxMin;
		m_vBoxMax = pdata->m_vBoxMax;
		m_vCenter = (m_vBoxMin + m_vBoxMax) * 0.5F;
		FmVec3 vec3sub;
		FmVec3Subtract(&vec3sub,&m_vBoxMax,&m_vBoxMin);
		m_fRadius = VisUtil_Vec3Length(&vec3sub) * 0.5F;
		// 已经设置了包围盒，不需要再动态更新
		m_bUpdateBoundBox = false;
	}

	m_nTailCount = pdata->m_bFlashTail ? FM_MAX(pdata->m_nTailLife, 1) : 1;

	unsigned int vertex_num = (2 + m_nTailCount * 2) * m_nParticleNum;
	unsigned int index_num = (m_nTailCount * 6) * m_nParticleNum;

	if (m_nComputeType == STATE_PARTICLE_CPU)
	{
		SAFE_RELEASE(m_pVBuffer);
		SAFE_RELEASE(m_pIBuffer);

		m_pVBuffer = m_pRender->CreateDynamicVB(
			vertex_num * sizeof(particle_vertex_cpu_t),"CParticleNode::Create STATE_PARTICLE_CPU");

		m_pVBBufferDataCPU = new particle_vertex_cpu_t[vertex_num];
		m_VBNum = vertex_num;

		if (NULL == m_pVBuffer)
		{
			CORE_TRACE("(CParticleNode::Create)create cpu VB failed");
			return false;
		}

		TAutoMem<unsigned short, 256, TCoreAlloc> auto_buf(index_num);
		unsigned short* pIndex = auto_buf.GetBuffer();
		unsigned short count = 0;

		for (int i = 0; i < m_nParticleNum; ++i)
		{
			for (int j = 0; j < m_nTailCount; j++)
			{
				*pIndex++ = count + 0;
				*pIndex++ = count + 1;
				*pIndex++ = count + 2;
				*pIndex++ = count + 1;
				*pIndex++ = count + 3;
				*pIndex++ = count + 2;
				count += 2;
			}

			count += 2;
		}

		m_pIBuffer = m_pRender->CreateStaticIB(auto_buf.GetBuffer(), 
			index_num * sizeof(unsigned short));

		if (NULL == m_pIBuffer)
		{
			CORE_TRACE("(CParticleNode::Create) (CPU mode) create cpu IB failed");
			return false;
		}
	}
	else
	{
		SAFE_RELEASE(m_pVBuffer);
		SAFE_RELEASE(m_pIBuffer);

		m_pVBuffer = m_pRender->CreateDynamicVB(
			vertex_num * sizeof(particle_vertex_gpu_t),"CParticleNode::Create STATE_PARTICLE_GPU");
		m_pVBBufferDataGPU = new particle_vertex_gpu_t[vertex_num];
		m_VBNum = vertex_num;
		if (NULL == m_pVBuffer)
		{
			CORE_TRACE("(CParticleNode::Create)create gpu VB failed");
			return false;
		}

		TAutoMem<unsigned short, 256, TCoreAlloc> auto_buf(index_num);
		unsigned short* pIndex = auto_buf.GetBuffer();
		unsigned short count = 0;
		for (int i = 0; i < m_nParticleNum; ++i)
		{
			*pIndex++ = count + 0;
			*pIndex++ = count + 1;
			*pIndex++ = count + 2;
			*pIndex++ = count + 1;
			*pIndex++ = count + 3;
			*pIndex++ = count + 2;
			count += 4;
		}

		m_pIBuffer = m_pRender->CreateStaticIB(auto_buf.GetBuffer(),
			index_num * sizeof(unsigned short));
		if (NULL == m_pIBuffer)
		{
			CORE_TRACE("(CParticleNode::Create) (GPU mode) create gpu IB failed");
			return false;
		}
	}

	m_dTimeRealized = m_pRender->GetCore()->GetTotalSeconds();

	return true;
}
 
void CParticleNode::UpdateCPUVertexBuffer(float percent)
{
	if (NULL == m_pVBuffer)
	{
		CORE_TRACE("(CParticleNode::UpdataCPUVertexBuffer)create vb failed");
		return;
	}

	int particle_num = m_nParticleNum;
	FmVec3 pos = FmVec3(0.0F, 0.0F, 0.0F);
	FmVec2 ipos[4];
	FmVec3 rpos[4];
	FmMat4 RMatrix;
	CParticleData* pdata = this->m_pData;
	int particle_frame = pdata->m_nParticleFrame;
	int texture_frame = pdata->m_nTextureFrame;
	float tex_frame_width = 1.0F / (particle_frame * texture_frame);
	float tex_frame_height = 1.0F / texture_frame;
	bool bb_y = m_nBillboardType == STATE_BILLBOARD_Y;
	bool bb_none = m_nBillboardType == STATE_BILLBOARD_NONE;
	IRenderContext* pContext = m_pRender->GetContext();
	FmVec3 vViewPos = pContext->GetCamera().vPosition;
	double total_time = double(m_dTimeCounter) * 1000;

	if (pdata->m_bLocalSpaceEmitter && pdata->m_bLocalAngleEmitter)
	{
		// 视点逆到本地空间
		FmVec3 WorldX(m_mtxLink._11, m_mtxLink._12, m_mtxLink._13);
		FmVec3 WorldY(m_mtxLink._21, m_mtxLink._22, m_mtxLink._23);
		FmVec3 WorldZ(m_mtxLink._31, m_mtxLink._32, m_mtxLink._33);
		FmVec3 WorldPos(m_mtxLink._41, m_mtxLink._42, m_mtxLink._43);

		vViewPos -= WorldPos;
		vViewPos = FmVec3(FmVec3Dot(&vViewPos, &WorldX),
			FmVec3Dot(&vViewPos, &WorldY), FmVec3Dot(&vViewPos, &WorldZ));
	}

	FmVec4* visibility_track = pdata->m_vVisibilityTrack;
	FmVec4* speed_track = pdata->m_vParticleSpeedTrack;
	FmVec4* wind_effect_track = pdata->m_vWindEffectTrack;
	FmVec4* size_track = pdata->m_vParticleSizeTrack;
	FmVec4* spin_track = pdata->m_vParticleSpinTrack;
	bool track_point_return = pdata->m_bTrackPointReturn;
	bool angle_link_emitter = pdata->m_bAngleLinkEmitter;
	float particle_life = (float)pdata->m_nParticleLife;
	float aspect_ratio = pdata->m_fAspectRatio;
	FmVec3 wind_vector = pdata->m_fWindEffect * m_fWindPower 
		* m_vWindDirection;
	int particle_end_time = m_nParticleEndTime;


	particle_vertex_cpu_t* pv = LockDynamicVB();

	for (int n = 0; n < particle_num; ++n)
	{
		particle_t* p = &m_Particle[n];
		float total_node_time = (float)total_time - p->time;
		float node_time = fmodf(total_node_time, particle_life);

		if (track_point_return)
		{
			node_time = particle_life - node_time;
		}

		float half_size = p->size * 0.5F;
		float angle = p->angle;
		unsigned int tcolor = p->color;
		FmVec4 color = VisUtil_ColorToVector4( tcolor );

		if ((total_node_time > 0.0F) && 
			((p->time + floor(total_node_time / particle_life) * particle_life) 
			< particle_end_time) && p->size > 0.001F)
		{
			// 粒子透明度
			float alpha = trace_value(visibility_track, node_time);

			color.w = color.w*alpha;

			// 粒子位置
			float speed = trace_speed(speed_track, node_time);
			
			pos = p->dir + p->chaos * pdata->m_fDirectionDeviation;
			FmVec3Normalize(&pos, &pos);
			pos *= speed * p->speed;
			pos += p->chaos * pdata->m_fChaosVal * node_time;

			// 风力
			FmVec3 force = trace_speed_vector3(wind_effect_track, node_time);

			pos += force.x * wind_vector;
			pos.y -= pdata->m_fGravity * p->weight * force.y;

			// 本地空间粒子位置偏移
			if (pdata->m_bLocalSpaceEmitter && !pdata->m_bLocalAngleEmitter)
			{
				pos += p->pos + m_vEmitterPos;
			}
			else
			{
				pos += p->pos + p->emitpos;
			}

			// 粒子大小
			half_size *= trace_value(size_track, node_time);

			// 粒子角度
			angle += trace_speed(spin_track, node_time) * p->spin;

			float half_width = half_size;
			float half_height = half_size * aspect_ratio;
			float sin_angle = sinf(angle);
			float cos_angle = cosf(angle);
			ipos[0].x = cos_angle * (-half_width) - sin_angle * (half_height);
			ipos[0].y = sin_angle * (-half_width) + cos_angle * (half_height);
			ipos[1].x = cos_angle * (-half_width) - sin_angle * (-half_height);
			ipos[1].y = sin_angle * (-half_width) + cos_angle * (-half_height);
			ipos[2].x = cos_angle * (half_width) - sin_angle * (-half_height);
			ipos[2].y = sin_angle * (half_width) + cos_angle * (-half_height);
			ipos[3].x = cos_angle * (half_width) - sin_angle * (half_height);
			ipos[3].y = sin_angle * (half_width) + cos_angle * (half_height);

			if (bb_y)
			{
				FmVec3 viewvec = pos - vViewPos;
				FmVec3 x_dir = FmVec3(0.0F, 1.0F ,0.0F);
				FmVec3 y_dir(0.0F, 1.0F, 0.0F);

				FmVec3Cross(&x_dir, &x_dir, &viewvec);
				FmVec3Normalize(&x_dir, &x_dir);

				rpos[0] = ipos[0].x * x_dir + ipos[0].y * y_dir;
				rpos[1] = ipos[1].x * x_dir + ipos[1].y * y_dir;
				rpos[2] = ipos[2].x * x_dir + ipos[2].y * y_dir;
				rpos[3] = ipos[3].x * x_dir + ipos[3].y * y_dir;
			}
			else if (bb_none)
			{
				if (angle_link_emitter)
				{
					FmVec3 y_dir = pos - p->pos - p->emitpos;
					FmVec3 x_dir = FmVec3(0.0F, 1.0F ,0.0F);

					FmVec3Normalize(&y_dir, &y_dir);
					FmVec3Cross(&x_dir, &x_dir, &y_dir);

					rpos[0] = ipos[0].x * x_dir + ipos[0].y * y_dir;
					rpos[1] = ipos[1].x * x_dir + ipos[1].y * y_dir;
					rpos[2] = ipos[2].x * x_dir + ipos[2].y * y_dir;
					rpos[3] = ipos[3].x * x_dir + ipos[3].y * y_dir;
				}
				else
				{
					rpos[0] = FmVec3(ipos[0].x, 0.0F, ipos[0].y);
					rpos[1] = FmVec3(ipos[1].x, 0.0F, ipos[1].y);
					rpos[2] = FmVec3(ipos[2].x, 0.0F, ipos[2].y);
					rpos[3] = FmVec3(ipos[3].x, 0.0F, ipos[3].y);
				}
			}
			else
			{
				FmVec3 viewvec = pos - vViewPos;
				FmVec3Normalize(&viewvec, &viewvec);


				FmVec3 x_dir = FmVec3(0.0F, 1.0F, 0.0F);
				FmVec3Cross(&x_dir, &x_dir, &viewvec);
				FmVec3Normalize(&x_dir, &x_dir);

				FmVec3 y_dir;
				FmVec3Cross(&y_dir, &viewvec, &x_dir);

				rpos[0] = ipos[0].x * x_dir + ipos[0].y * y_dir;
				rpos[1] = ipos[1].x * x_dir + ipos[1].y * y_dir;
				rpos[2] = ipos[2].x * x_dir + ipos[2].y * y_dir;
				rpos[3] = ipos[3].x * x_dir + ipos[3].y * y_dir;

			}

			if (m_bUpdateBoundBox)
			{
				FmVec3 localpos;

				if (pdata->m_bLocalSpaceEmitter 
					&& pdata->m_bLocalAngleEmitter)
				{
					FmVec3TransformCoord(&localpos, &pos, &m_mtxLink);
					localpos -= m_vEmitterPos;
				}
				else
				{
					localpos = pos - m_vEmitterPos;
				}

				float half_x = half_size;
				float half_y = half_x * aspect_ratio;
				float half_z = half_x;
				float min_x = localpos.x - half_x;
				float min_y = localpos.y - half_y;
				float min_z = localpos.z - half_z;
				float max_x = localpos.x + half_x;
				float max_y = localpos.y + half_y;
				float max_z = localpos.z + half_z;

				if (min_x < m_vBoxMin.x) { m_vBoxMin.x = min_x; }
				if (min_y < m_vBoxMin.y) { m_vBoxMin.y = min_y; }
				if (min_z < m_vBoxMin.z) { m_vBoxMin.z = min_z; }
				if (max_x > m_vBoxMax.x) { m_vBoxMax.x = max_x; }
				if (max_y > m_vBoxMax.y) { m_vBoxMax.y = max_y; }
				if (max_z > m_vBoxMax.z) { m_vBoxMax.z = max_z; }
			}
		}
		else
		{
			half_size = 0.0F;
			node_time = 0.0F;
			pos = p->emitpos;

			rpos[0] = rpos[1] = rpos[2] = rpos[3] = 
				FmVec3(0.0F, 0.0F, 0.0F);
		}

		// frame tu value
		int uframe = int(node_time * 0.03F) % particle_frame;
		float uval = float(uframe) / float(particle_frame);
		float tu = uval + (p->frame % texture_frame) * tex_frame_width;
		float _tv = (p->frame / texture_frame) * tex_frame_height;
		float tv = 1.0f- (_tv - floor(_tv));

		pv->pos = FmVec4(pos + rpos[0], half_size);
		pv->tu = tu;
		pv->tv = tv;
		pv->color = color;

		pv++;
		pv->pos = FmVec4(pos + rpos[3], half_size);
		pv->tu = tu + tex_frame_width;
		pv->tv = tv;
		pv->color = color;
		pv++;
		pv->pos = FmVec4(pos + rpos[1], half_size);
		pv->tu = tu;
		pv->tv = tv + tex_frame_height;
		pv->color = color;
		pv++;
		pv->pos = FmVec4(pos + rpos[2], half_size);
		pv->tu = tu + tex_frame_width;
		pv->tv = tv + tex_frame_height;
		pv->color = color;
		pv++;
	}

	UnlockDynamicVB();

	if (m_bUpdateBoundBox)
	{
		m_vCenter.x = (m_vBoxMin.x + m_vBoxMax.x) * 0.5F;
		m_vCenter.y = (m_vBoxMin.y + m_vBoxMax.y) * 0.5F;
		m_vCenter.z = (m_vBoxMin.z + m_vBoxMax.z) * 0.5F;

		float box_size_x = m_vBoxMax.x - m_vBoxMin.x;
		float box_size_y = m_vBoxMax.y - m_vBoxMin.y;
		float box_size_z = m_vBoxMax.z - m_vBoxMin.z;

		// 并非准确的半径，只为了提高效率
		m_fRadius = FM_MAX(box_size_x, FM_MAX(box_size_y, box_size_z)) * 0.5F;
	}

	m_nDrawParticleNum = particle_num;
}
 
CParticleNode::particle_vertex_cpu_t* CParticleNode::LockDynamicVB()
{
	return m_pVBBufferDataCPU;
}

void CParticleNode::UnlockDynamicVB()
{
	m_pRender->UpdateDynamicVB(m_pVBuffer,0,m_pVBBufferDataCPU,sizeof(particle_vertex_cpu_t)*m_VBNum);
}

 
void CParticleNode::UpdateFlashTailVertexBuffer(float percent)
{
	if (NULL == m_pVBuffer)
	{
		CORE_TRACE("(CParticleNode::Create)create vb failed");
		return;
	}

	int particle_num = (int)ceil(m_nParticleNum * percent);

	if (particle_num < 1)
	{
		m_nDrawParticleNum = 0;
		return;
	}
	
	FmMat4 RMatrix;
	CParticleData* pdata = this->m_pData;
	int particle_frame = pdata->m_nParticleFrame;
	int texture_frame = pdata->m_nTextureFrame;
	float tex_frame_width = 1.0F / (particle_frame * texture_frame);
	float tex_frame_height = 1.0F / texture_frame;
	IRenderContext* pContext = m_pRender->GetContext();
	FmVec3 vViewPos = pContext->GetCamera().vPosition;
	double total_time = double(m_dTimeCounter) * 1000;

	if (pdata->m_bLocalSpaceEmitter && pdata->m_bLocalAngleEmitter)
	{
		// 视点逆到本地空间
		FmVec3 WorldX(m_mtxLink._11, m_mtxLink._12, m_mtxLink._13);
		FmVec3 WorldY(m_mtxLink._21, m_mtxLink._22, m_mtxLink._23);
		FmVec3 WorldZ(m_mtxLink._31, m_mtxLink._32, m_mtxLink._33);
		FmVec3 WorldPos(m_mtxLink._41, m_mtxLink._42, m_mtxLink._43);

		vViewPos -= WorldPos;
		vViewPos = FmVec3(FmVec3Dot(&vViewPos, &WorldX),
			FmVec3Dot(&vViewPos, &WorldY), FmVec3Dot(&vViewPos, &WorldZ));
	}

	FmVec4* visibility_track = pdata->m_vVisibilityTrack;
	FmVec4* speed_track = pdata->m_vParticleSpeedTrack;
	FmVec4* wind_effect_track = pdata->m_vWindEffectTrack;
	FmVec4* size_track = pdata->m_vParticleSizeTrack;
	FmVec4* spin_track = pdata->m_vParticleSpinTrack;
	bool track_point_return = pdata->m_bTrackPointReturn;
	FmVec3 wind_vector = pdata->m_fWindEffect * m_fWindPower 
		* m_vWindDirection;

	CParticleNode::particle_vertex_cpu_t* pv  = LockDynamicVB();

	for (int n = 0; n < particle_num; ++n)
	{
		particle_t* p = &m_Particle[n];
		double total_node_time = total_time - p->time;
		float node_time = (float)fmod(total_node_time, pdata->m_nParticleLife);

		if (track_point_return)
		{
			node_time = pdata->m_nParticleLife - node_time;
		}

		FmVec3 pos = FmVec3(0.0F, 0.0F, 0.0F);
		FmVec3 x_dir = FmVec3(0.0F, 0.0F, 0.0F);
		FmVec3 y_dir = FmVec3(0.0F, 0.0F, 0.0F);

		FmVec4 color = VisUtil_ColorToVector4(p->color);

		if (0 == pdata->m_nTailLife)
		{
			float half_size = p->size * 0.5F;
			float angle = p->angle;

			if ((total_node_time > 0.0F) && 
				((p->time + floor(total_node_time / pdata->m_nParticleLife)
				* pdata->m_nParticleLife) < m_nParticleEndTime) 
				&& (node_time > 0.0F))
			{
				// 粒子透明度
				float alpha = trace_value(visibility_track, node_time);
				color.w*=alpha;

				/*
				color = (int(((color & 0xFF000000) >> 24) * alpha) << 24) 
					| (color & 0x00FFFFFF);
				*/

				// 粒子位置
				float speed = trace_speed(speed_track, node_time);

				// v此时为速度方向
				FmVec3 v = p->dir + p->chaos * pdata->m_fDirectionDeviation;
				FmVec3Normalize(&v, &v);
				pos = v * speed * p->speed;
				pos += p->chaos * pdata->m_fChaosVal * node_time;

				// v此时为该时刻的速度
				v *= trace_value(speed_track, node_time) * p->speed;
				v += p->chaos * pdata->m_fChaosVal;

				// 风力
				float force = trace_speed(wind_effect_track, node_time);

				pos += force * wind_vector;
				pos.y -= pdata->m_fGravity * p->weight * node_time;

				// v此时为该时刻考虑重力和风力后的速度
				v += trace_value(wind_effect_track, node_time) * wind_vector;
				v.y -= pdata->m_fGravity * p->weight;

				// 本地空间粒子位置偏移
				if (pdata->m_bLocalSpaceEmitter 
					&& !pdata->m_bLocalAngleEmitter)
				{
					pos += p->pos + m_vEmitterPos;
				}
				else
				{
					pos += p->pos + p->emitpos;
				}

				// 粒子大小
				half_size *= trace_value(size_track, node_time);

				FmVec3 viewvec = pos - vViewPos;

				FmVec3Normalize(&v, &v);
				FmVec3Cross(&x_dir, &viewvec, &v);
				FmVec3Normalize(&x_dir, &x_dir);
				x_dir *= half_size;
				y_dir = v * half_size * pdata->m_fAspectRatio;

				if (m_bUpdateBoundBox)
				{
					FmVec3 localpos;

					if (pdata->m_bLocalSpaceEmitter 
						&& pdata->m_bLocalAngleEmitter)
					{
						FmVec3TransformCoord(&localpos, &pos, &m_mtxLink);
						localpos -= m_vEmitterPos;
					}
					else
					{
						localpos = pos - m_vEmitterPos;
					}

					float half_x = half_size;
					float half_y = half_x;
					float half_z = half_x;

					float min_x = localpos.x - half_x;
					float min_y = localpos.y - half_y;
					float min_z = localpos.z - half_z;

					float max_x = localpos.x + half_x;
					float max_y = localpos.y + half_y;
					float max_z = localpos.z + half_z;

					if (min_x < m_vBoxMin.x) { m_vBoxMin.x = min_x; }
					if (min_y < m_vBoxMin.y) { m_vBoxMin.y = min_y; }
					if (min_z < m_vBoxMin.z) { m_vBoxMin.z = min_z; }

					if (max_x > m_vBoxMax.x) { m_vBoxMax.x = max_x; }
					if (max_y > m_vBoxMax.y) { m_vBoxMax.y = max_y; }
					if (max_z > m_vBoxMax.z) { m_vBoxMax.z = max_z; }
				}
			}
			else
			{
				half_size = 0.0F;
				node_time = 0.0F;
				pos = p->emitpos;
				color.w = 0.0f;//color & 0x00FFFFFF;
			}

			// frame tu value
			int uframe = int(node_time * 0.03F) % particle_frame;
			float uval = float(uframe) / float(particle_frame);
			float tu = uval + (p->frame % texture_frame) * tex_frame_width;
			float tv = (p->frame / texture_frame) * tex_frame_height;

			if (track_point_return)
			{
				pv->pos = FmVec4(pos - x_dir - y_dir, half_size);
				pv->tu = tu;
				pv->tv = tv;
				pv->color = color;
				pv++;
				pv->pos = FmVec4(pos + x_dir - y_dir, half_size);
				pv->tu = tu + tex_frame_width;
				pv->tv = tv;
				pv->color = color;
				pv++;
				pv->pos = FmVec4(pos - x_dir + y_dir, half_size);
				pv->tu = tu;
				pv->tv = tv + tex_frame_height;
				pv->color = color;
				pv++;
				pv->pos = FmVec4(pos + x_dir + y_dir, half_size);
				pv->tu = tu + tex_frame_width;
				pv->tv = tv + tex_frame_height;
				pv->color = color;
				pv++;
			}
			else
			{
				pv->pos = FmVec4(pos - x_dir + y_dir, half_size);
				pv->tu = tu;
				pv->tv = tv;
				pv->color = color;
				pv++;
				pv->pos = FmVec4(pos + x_dir + y_dir, half_size);
				pv->tu = tu + tex_frame_width;
				pv->tv = tv;
				pv->color = color;
				pv++;
				pv->pos = FmVec4(pos - x_dir - y_dir, half_size);
				pv->tu = tu;
				pv->tv = tv + tex_frame_height;
				pv->color = color;
				pv++;
				pv->pos = FmVec4(pos + x_dir - y_dir, half_size);
				pv->tu = tu + tex_frame_width;
				pv->tv = tv + tex_frame_height;
				pv->color = color;
				pv++;
			}
		}
		else
		{
			for (int i = 0; i <= m_nTailCount; i++)
			{
				float half_size = p->size * 0.5F;
				float angle = p->angle;

				if ((total_node_time > 0.0F) && ((total_node_time - node_time) 
					< m_nParticleEndTime) && (node_time > 0.0F))
				{
					// 粒子透明度
					float alpha = trace_value(visibility_track, node_time);

					color.w = alpha * color.w;

					//color = (int(((color & 0xFF000000) >> 24) * alpha) << 24) 
					//	| (color & 0x00FFFFFF);

					// 粒子位置
					float speed = trace_speed(speed_track, node_time);

					// v此时为速度方向
					FmVec3 v = p->dir + p->chaos * pdata->m_fDirectionDeviation;
					FmVec3Normalize(&v, &v);
					pos = v * speed * p->speed;
					pos += p->chaos * pdata->m_fChaosVal * node_time;

					// v此时为该时刻的速度
					v *= trace_value(speed_track, node_time) * p->speed;
					v += p->chaos * pdata->m_fChaosVal;

					// 风力
					float force = trace_speed(wind_effect_track, node_time);

					pos += force * wind_vector;
					pos.y -= pdata->m_fGravity * p->weight * node_time;

					// v此时为该时刻考虑重力和风力后的速度
					v += trace_value(wind_effect_track, node_time) * wind_vector;
					v.y -= pdata->m_fGravity * p->weight;

					// 粒子大小
					half_size *= trace_value(size_track, node_time);

					// 本地空间粒子位置偏移
					if (pdata->m_bLocalSpaceEmitter 
						&& !pdata->m_bLocalAngleEmitter)
					{
						pos += p->pos + m_vEmitterPos;
					}
					else
					{
						pos += p->pos + p->emitpos;
					}

					FmVec3 viewvec = pos - vViewPos;
					FmVec3 temp_dir;
					FmVec3Cross(&temp_dir, &viewvec, &v);
					FmVec3Normalize(&temp_dir, &temp_dir);

					x_dir = half_size * temp_dir;

					if (pdata->m_bFixupTailSize)
					{
						x_dir *= VisUtil_Vec3Length(&v) * 1000.0F;
					}

					if (m_bUpdateBoundBox)
					{
						FmVec3 localpos;

						if (pdata->m_bLocalSpaceEmitter 
							&& pdata->m_bLocalAngleEmitter)
						{
							FmVec3TransformCoord(&localpos, &pos, &m_mtxLink);
							localpos -= m_vEmitterPos;
						}
						else
						{
							localpos = pos - m_vEmitterPos;
						}

						float half_x = half_size;
						float half_y = half_x;
						float half_z = half_x;

						float min_x = localpos.x - half_x;
						float min_y = localpos.y - half_y;
						float min_z = localpos.z - half_z;

						float max_x = localpos.x + half_x;
						float max_y = localpos.y + half_y;
						float max_z = localpos.z + half_z;


						if (min_x < m_vBoxMin.x) { m_vBoxMin.x = min_x; }
						if (min_y < m_vBoxMin.y) { m_vBoxMin.y = min_y; }
						if (min_z < m_vBoxMin.z) { m_vBoxMin.z = min_z; }

						if (max_x > m_vBoxMax.x) { m_vBoxMax.x = max_x; }
						if (max_y > m_vBoxMax.y) { m_vBoxMax.y = max_y; }
						if (max_z > m_vBoxMax.z) { m_vBoxMax.z = max_z; }
					}
				}
				else
				{
					half_size = 0.0F;
					node_time = 0.0F;
					pos = p->emitpos;
					color.w = 0.0f;//color & 0x00FFFFFF;
				}

				// frame tu value
				int uframe = int(node_time * 0.03f) % particle_frame;
				float uval = float(uframe) / float(particle_frame);
				float tu = uval + (p->frame % texture_frame) * tex_frame_width;
				float tv = (p->frame / texture_frame) * tex_frame_height;

				if (track_point_return)
				{
					pv->pos = FmVec4(pos - x_dir, half_size);
					pv->tu = tu;
					pv->tv = tv + 1.0F - tex_frame_height * i / m_nTailCount;
					pv->color = color;
					pv++;
					pv->pos = FmVec4(pos + x_dir, half_size);
					pv->tu = tu + tex_frame_width;
					pv->tv = tv + 1.0F - tex_frame_height * i / m_nTailCount;
					pv->color = color;
					pv++;
				}
				else
				{
					pv->pos = FmVec4(pos - x_dir, half_size);
					pv->tu = tu;
					pv->tv = tv + tex_frame_height * i / m_nTailCount;
					pv->color = color;
					pv++;
					pv->pos = FmVec4(pos + x_dir, half_size);
					pv->tu = tu + tex_frame_width;
					pv->tv = tv + tex_frame_height * i / m_nTailCount;
					pv->color = color;
					pv++;
				}

				node_time -= half_size * pdata->m_fAspectRatio / m_nTailCount;
			}
		}
	}

	//m_pRender->UnlockDynamicVB(m_pVBuffer);
	UnlockDynamicVB();

	m_nDrawParticleNum = particle_num;

	if (m_bUpdateBoundBox)
	{
		m_vCenter.x = (m_vBoxMin.x + m_vBoxMax.x) * 0.5F;
		m_vCenter.y = (m_vBoxMin.y + m_vBoxMax.y) * 0.5F;
		m_vCenter.z = (m_vBoxMin.z + m_vBoxMax.z) * 0.5F;

		float box_size_x = m_vBoxMax.x - m_vBoxMin.x;
		float box_size_y = m_vBoxMax.y - m_vBoxMin.y;
		float box_size_z = m_vBoxMax.z - m_vBoxMin.z;

		// 并非准确的半径，只为了提高效率
		m_fRadius = FM_MAX(box_size_x, FM_MAX(box_size_y, box_size_z)) * 0.5F;
	}
}
 

int CParticleNode::UpdateGPUVertexBuffer()
{
	int particle_num = m_nParticleNum;

	if (particle_num < 1)
	{
		return 0;
	}

	FmVec3 ipos[4];
	CParticleData* pdata = m_pData;
	int particle_frame = pdata->m_nParticleFrame;
	int texture_frame = pdata->m_nTextureFrame;
	float tex_frame_width = 1.0F / (particle_frame * texture_frame);
	float tex_frame_height = 1.0F / texture_frame;

	particle_vertex_gpu_t* pv = m_pVBBufferDataGPU;

	if (NULL == pv)
	{
		return 0;
	}

	for (int n = 0; n < particle_num; ++n)
	{
		particle_t* p = &m_Particle[n];

		float half_size = p->size * 0.5f;
		float half_width = half_size;
		float half_height = half_size * pdata->m_fAspectRatio;

		ipos[0] = FmVec3(-half_width,  half_height, 0.0f);
		ipos[1] = FmVec3(-half_width, -half_height, 0.0f);
		ipos[2] = FmVec3( half_width, -half_height, 0.0f);
		ipos[3] = FmVec3( half_width,  half_height, 0.0f);





		FmVec4 pos(p->pos, p->time);
		FmVec3 dir(p->dir * p->speed);
		FmVec2 rand0(p->chaos.x, p->chaos.y);
		FmVec2 rand1(p->chaos.z, p->weight);
		FmVec2 rand2(p->angle, p->spin);

		float ubias = int((1.0f / tex_frame_height) * rand_one()) * tex_frame_height;
		float vbias = int((1.0f / tex_frame_height) * rand_one()) * tex_frame_height;

		pv[n*4  ] = particle_vertex_gpu_t(pos,dir,FmVec2(ipos[0].x,ipos[0].y),rand0,rand1,rand2,ubias,vbias);
		pv[n*4+1] = particle_vertex_gpu_t(pos,dir,FmVec2(ipos[3].x,ipos[3].y),rand0,rand1,rand2,ubias+tex_frame_width,vbias);
		pv[n*4+2] = particle_vertex_gpu_t(pos,dir,FmVec2(ipos[1].x,ipos[1].y),rand0,rand1,rand2,ubias,vbias+tex_frame_height);
		pv[n*4+3] = particle_vertex_gpu_t(pos,dir,FmVec2(ipos[2].x,ipos[2].y),rand0,rand1,rand2,ubias+tex_frame_width,vbias+tex_frame_height);
	}

	m_pRender->UpdateDynamicVB(m_pVBuffer,0,m_pVBBufferDataGPU,sizeof(particle_vertex_gpu_t)*m_VBNum);

	return particle_num;
}



void CParticleNode::UpdateGPUBoundBox()
{
	CParticleData* pdata = this->m_pData;
	int particle_num = m_nParticleNum;
	double total_time = double(m_dTimeCounter) * 1000;
	FmVec4* speed_track = pdata->m_vParticleSpeedTrack;
	FmVec4* wind_effect_track = pdata->m_vWindEffectTrack;
	FmVec4* size_track = pdata->m_vParticleSizeTrack;
	FmVec4* spin_track = pdata->m_vParticleSpinTrack;
	bool track_point_return = pdata->m_bTrackPointReturn;
	float particle_life = (float)pdata->m_nParticleLife;
	float aspect_ratio = pdata->m_fAspectRatio;
	FmVec3 wind_vector = pdata->m_fWindEffect * m_fWindPower 
		* m_vWindDirection;
	int particle_end_time = m_nParticleEndTime;
	FmVec3 localpos;

	for (int n = 0; n < particle_num; ++n)
	{
		particle_t* p = &m_Particle[n];
		float total_node_time = (float)total_time - p->time;
		float node_time = fmodf(total_node_time, particle_life);

		if (track_point_return)
		{
			node_time = particle_life - node_time;
		}

		float half_size = p->size * 0.5F;
		float angle = p->angle;

		if ((total_node_time > 0.0F) 
			&& ((p->time + floor(total_node_time / particle_life) 
			* particle_life) < particle_end_time))
		{
			// 粒子位置
			float speed = trace_speed(speed_track, node_time);
			FmVec3 pos = p->dir + p->chaos * pdata->m_fDirectionDeviation;
			
			FmVec3Normalize(&pos, &pos);
			pos *= speed * p->speed;
			pos += p->chaos * pdata->m_fChaosVal * node_time;

			// 风力
			float force = trace_speed(wind_effect_track, node_time);
			
			pos += force * wind_vector;
			pos.y -= pdata->m_fGravity * p->weight * node_time;

			// 本地空间粒子位置偏移
			if (pdata->m_bLocalSpaceEmitter && pdata->m_bLocalAngleEmitter)
			{
				pos += p->pos;
			}
			else if (pdata->m_bLocalSpaceEmitter 
				&& !pdata->m_bLocalAngleEmitter)
			{
				pos += p->pos + m_vEmitterPos;
			}
			else
			{
				pos += p->pos + p->emitpos;
			}

			// 粒子大小
			half_size *= trace_value(size_track, node_time);

			if (pdata->m_bLocalSpaceEmitter && pdata->m_bLocalAngleEmitter)
			{
				FmVec3TransformCoord(&localpos, &pos, &m_mtxLink);
				localpos -= m_vEmitterPos;
			}
			else
			{
				localpos = pos - m_vEmitterPos;
			}

			float half_x = half_size;
			float half_y = half_x * aspect_ratio;
			float half_z = half_x;
			float min_x = localpos.x - half_x;
			float min_y = localpos.y - half_y;
			float min_z = localpos.z - half_z;
			float max_x = localpos.x + half_x;
			float max_y = localpos.y + half_y;
			float max_z = localpos.z + half_z;

			if (min_x < m_vBoxMin.x) { m_vBoxMin.x = min_x; }
			if (min_y < m_vBoxMin.y) { m_vBoxMin.y = min_y; }
			if (min_z < m_vBoxMin.z) { m_vBoxMin.z = min_z; }
			if (max_x > m_vBoxMax.x) { m_vBoxMax.x = max_x; }
			if (max_y > m_vBoxMax.y) { m_vBoxMax.y = max_y; }
			if (max_z > m_vBoxMax.z) { m_vBoxMax.z = max_z; }
		}
	}

	m_vCenter.x = (m_vBoxMin.x + m_vBoxMax.x) * 0.5F;
	m_vCenter.y = (m_vBoxMin.y + m_vBoxMax.y) * 0.5F;
	m_vCenter.z = (m_vBoxMin.z + m_vBoxMax.z) * 0.5F;

	float box_size_x = m_vBoxMax.x - m_vBoxMin.x;
	float box_size_y = m_vBoxMax.y - m_vBoxMin.y;
	float box_size_z = m_vBoxMax.z - m_vBoxMin.z;

	// 并非准确的半径，只为了提高效率
	m_fRadius = FM_MAX(box_size_x, FM_MAX(box_size_y, box_size_z)) * 0.5F;
}

void CParticleNode::UpdateParticlePartData(particle_t* p)
{
	CParticleData* pdata = this->m_pData;

	p->weight = (pdata->m_fWeight + pdata->m_fWeightDeviation * rand_half()) 
		* (m_vScale.x + m_vScale.z) * 0.5F;
	p->size = (pdata->m_fInitSize + pdata->m_fSizeDeviation * rand_half()) 
		* (m_vScale.x + m_vScale.z) * 0.5F;

	float emit_radius_x = pdata->m_fEmitRadiusX;
	float emit_radius_y = pdata->m_fEmitRadiusY;
	float emit_radius_z = pdata->m_fEmitRadiusZ;

	if (pdata->m_EmitRadiusX[0].time >= 0)
	{
		emit_radius_x *= pdata->m_EmitRadiusX[0].value;
	}
	else
	{
		emit_radius_x = 0.0F;
	}

	if (pdata->m_EmitRadiusY[0].time >= 0)
	{
		emit_radius_y *= pdata->m_EmitRadiusY[0].value;
	}
	else
	{
		emit_radius_y = 0.0F;
	}

	if (pdata->m_EmitRadiusZ[0].time >= 0)
	{
		emit_radius_z *= pdata->m_EmitRadiusZ[0].value;
	}
	else
	{
		emit_radius_z = 0.0F;
	}

	p->pos = FmVec3(
		emit_radius_x * rand_half() * m_vScale.x,
		emit_radius_y * rand_half() * m_vScale.y,
		emit_radius_z * rand_half() * m_vScale.z);

	p->speed = (pdata->m_fInitSpeed + pdata->m_fSpeedDeviation * rand_half()) 
		* (1.0F - pdata->m_fSpeedChaos * rand_one()) * m_vScale.y;
	p->angle = 0.0F;
	p->spin = pdata->m_fSpin + pdata->m_fSpinDeviation * rand_half();
}

void CParticleNode::UpdateParticleData()
{
	set_rand_seed(m_nRandSeed);

	CParticleData* pdata = this->m_pData;
	particle_t* p = m_Particle;
	particle_t* last_p = p;

	bool random_color = !pdata->m_strRandomColorTex.empty();

	for (int n = 0; n < m_nParticleNum; n++)
	{
		// 随其它参数而变更的数据
		UpdateParticlePartData(p);

		// 只需初始化的动态数据
		if (random_color)
		{
			p->color = rand_color() | 0xFF000000;
		}
		else
		{
			p->color = 0xFFFFFFFF;
		}

		if (pdata->m_bRandomDirection)
		{
			switch (pdata->m_nOneAxisNotRand)
			{
			case CParticleData::AXIS_X: 
				p->dir = FmVec3(0.001F, rand_half(), rand_half());
				break;
			case CParticleData::AXIS_Y: 
				p->dir = FmVec3(rand_half(), 0.001F, rand_half());
				break;
			case CParticleData::AXIS_Z: 
				p->dir = FmVec3(rand_half(), rand_half(), 0.001F);
				break;
			default:
				p->dir = FmVec3(rand_half(), rand_half(), rand_half());
				break;
			}

			FmVec3Normalize(&p->dir, &p->dir);
		}
		else if(pdata->m_bLocalSpaceEmitter)
		{
			p->dir = FmVec3(0.0F, 1.0F, 0.0F);
		}
		else
		{
			p->dir = m_vEmitterDir;
		}

		// 无需更新的固定数据
		p->frame = rand_int();

		// 防止循环次数过多
		for (int i = 0; i < 16; i++)
		{
			p->chaos.x = rand_half();
			p->chaos.y = rand_half();
			p->chaos.z = rand_half();

			if (VisUtil_Vec3Length(&p->chaos) < 0.5F)
			{
				break;
			}
		}


		if (n % pdata->m_nOneEmitNumber == 0)
		{
			p->time = float(n / pdata->m_nOneEmitNumber) 
				* pdata->m_fEmissionTime + pdata->m_fEmissionTimeRand * rand_half();
		}
		else
		{
			p->time = last_p->time;
		}

		last_p = p;
		p++;
	}
}

// 转成CPU渲染：
void CParticleNode::UpdateComputeType()
{
	if (m_nComputeType == STATE_PARTICLE_AUTO)
	{
		// 将GPU粒子转换成CPU运算
		unsigned int vertex_num = (2 + m_nTailCount * 2) * m_nParticleNum;

		Assert(NULL == m_pVBuffer);

		SAFE_RELEASE(m_pVBuffer);

		m_pVBuffer = m_pRender->CreateDynamicVB(
			vertex_num * sizeof(particle_vertex_cpu_t),"CParticleNode::UpdateComputeType");

		if (NULL == m_pVBuffer)
		{
			return;
		}

		m_nComputeType = STATE_PARTICLE_CPU;
	}
}

bool CParticleNode::Delete()
{
	// 是否延迟删除
	if (m_pData->m_bDelayUnload)
	{
		// 粒子管理器关闭了显示，这里再打开显示
		m_bShowState = true;
		// 设置结束时间
		m_nParticleEndTime = (int)FM_MIN(m_dTimeCounter * 1000.0, 
			m_nParticleEndTime);
		// 不再重复发射
		m_bRepeat = false;
		// 停止发射
		m_bEmitSwitch = false;

		return false;
	}
	else
	{
		// 粒子可以立即删除
		m_bComplete = true;

		return true;
	}
}

void CParticleNode::Update(float seconds)
{
	//bool begine_frame_succeed = m_pRender->GetBeginFrameSucceed();
	bool begine_frame_succeed = true;

	if (!begine_frame_succeed)
	{
		double total_seconds = m_pRender->GetCore()->GetTotalSeconds();
		double delta_time = total_seconds - m_dTimeRealized;

		if (!m_bPause)
		{
			if (m_dTimeCounter <= 0.0)
			{
				// 乘以速度调节因子
				m_dTimeCounter += delta_time * m_fSpeedFactor;

				if (m_dTimeCounter > 0.0)
				{
					for (int n = 0; n < m_nParticleNum; ++n)
					{
						if (m_pData->m_bLocalSpaceEmitter)
						{
							m_Particle[n].emitpos = m_vLinkPos;
						}
						else
						{
							m_Particle[n].emitpos = m_vEmitterPos;
						}
					}

					// 初始化粒子数据
					UpdateParticleData();

					if (m_nComputeType != STATE_PARTICLE_CPU)
					{
						UpdateGPUVertexBuffer();
					}
				}
				else
				{
					return;
				}
			}
			else
			{
				// 乘以速度调节因子
				m_dTimeCounter += delta_time * m_fSpeedFactor;
			}
		}

		if (m_bLinkEmitter)
		{
			IVisBase* pLink = (IVisBase*)m_pRender->GetCore()->GetEntity(
				m_LinkObjectID);

			if (!pLink)
			{
				// 跟随对象消失
				if (Delete())
				{
					return;
				}
			}
		}

		if (!m_ParticleID.IsNull())
		{
			IEntity* pEntity = m_pRender->GetCore()->GetEntity(m_ParticleID);

			if (NULL == pEntity)
			{
				// 跟随对象消失
				if (Delete())
				{
					return;
				}
			}
		}

		// 检查粒子是否完成
		if (!m_bRepeat)
		{
			if ((m_dTimeCounter * 1000.0 - m_nParticleEndTime) 
				> (m_pData->m_nParticleLife))
			{
				m_bComplete = true;
				return;
			}
		}
	}
}

void CParticleNode::Realize(float offset_seconds)
{
	double total_seconds = m_pRender->GetCore()->GetTotalSeconds();
	double dt = total_seconds - m_dTimeRealized;
	m_dTimeRealized = total_seconds;

	CParticleData* pdata = m_pData;

	if (m_bLinkEmitter)
	{
		IVisBase* pLink = (IVisBase*)m_pRender->GetCore()->GetEntity(
			m_LinkObjectID);

		if (pLink)
		{
			if (!pLink->GetVisible())
			{
				// 跟随绑定对象不显示
				return;
			}

			if (m_nHelperID >= 0)
			{
				Model* pModel = (Model*)pLink;
				IModelPlayer* pModelPlayer = pModel->GetModelPlayer();

				FmMat4 mtxNode;
				FmMat4 mtxModel;

				pModelPlayer->GetLabelMatrix(m_nHelperID, mtxNode);
				pModel->GetWorldMatrix(mtxModel);

				// 当使用了non-uniform scaling，分解矩阵时可能失败
				FmMat4Multiply(&m_mtxLink, &mtxNode, &mtxModel);

				FmVec3 vDir(0.0F, 1.0F, 0.0F);
				
				FmVec3TransformNormal(&vDir, &vDir, &m_mtxLink);
				SetEmitterDir(vDir);

				FmVec3 vScale(1.0F, 1.0F, 1.0F);

				VisUtil_GetMatrixScale(&vScale, &m_mtxLink);
				SetScale(vScale);
			}
			else
			{
				FmVec3 pos = pLink->GetPosition();
				FmVec3 angle = pLink->GetAngle();

				VisUtil_MatrixRotationTranslation(&m_mtxLink, &angle, &pos);
			}

			FmVec3 vEmitterPos = m_vEmitterPos;

			FmVec3TransformCoord(&vEmitterPos, &m_vLinkPos, &m_mtxLink);
			if ( ! pdata->m_bLocalSpaceEmitter)
			{
				if (pdata->m_bLocalAngleEmitter)
				{
					FmVec3TransformCoord(&m_vEmitterDir, &m_vLinkDirPos, 
						&m_mtxLink);

					m_vEmitterDir = m_vEmitterDir - vEmitterPos;

					SetEmitterDir(m_vEmitterDir);
				}
				else if (pdata->m_bAngleLinkSpeed)
				{
					m_vEmitterDir = vEmitterPos - m_vLastEmitterPos;
					
					if (VisUtil_Vec3Length(&m_vEmitterDir) != 0.0F)
					{
						SetEmitterDir(m_vEmitterDir);
					}
				}
			}

			SetEmitterPos(vEmitterPos);
		}
		else
		{
			// 跟随对象消失
			if (Delete())
			{
				return;
			}
		}
	}

	if (!m_ParticleID.IsNull())
	{
		IEntity* pEntity = m_pRender->GetCore()->GetEntity(m_ParticleID);

		if (NULL == pEntity)
		{
			// 跟随对象消失
			if (Delete())
			{
				return;
			}
		}
	}

	// 检查粒子是否完成
	if (!m_bRepeat)
	{
		if ((m_dTimeCounter * 1000.0 - m_nParticleEndTime) 
			> (pdata->m_nParticleLife))
		{
			m_bComplete = true;
			return;
		}
	}

	// 乘以速度调节因子
	float delta_time = float(dt) * m_fSpeedFactor;

	if (m_fSystemLifeTime > 0)
	{
		m_fSystemLifeTime = m_fSystemLifeTime - delta_time;

		if (m_fSystemLifeTime <= 0)
		{
			if (Delete())
			{
				return;
			}
		}
	}

	if (!m_bVisible)
	{
		return;
	}

	if (!m_bShowState)
	{
		return;
	}

	if ((m_pTexture != NULL) && (!m_pTexture->IsLoadComplete()))
	{
		return;
	}

	if ((m_pRandomColorTex != NULL) && (!m_pRandomColorTex->IsLoadComplete()))
	{
		return;
	}

	if ((m_pRefractionTex != NULL) && (!m_pRefractionTex->IsLoadComplete()))
	{
		return;
	}


	if (!m_bPause)
	{
		if (m_dTimeCounter <= 0.0)
		{
			// 乘以速度调节因子
			//m_dTimeCounter += dt * m_fSpeedFactor;
			m_dTimeCounter += delta_time;

			if (m_dTimeCounter > 0.0)
			{
				for (int n = 0; n < m_nParticleNum; ++n)
				{
					if (m_pData->m_bLocalSpaceEmitter)
					{
						m_Particle[n].emitpos = m_vLinkPos;
					}
					else
					{
						m_Particle[n].emitpos = m_vEmitterPos;
					}
				}

				// 初始化粒子数据
				UpdateParticleData();

				if (m_nComputeType != STATE_PARTICLE_CPU)
				{
					UpdateGPUVertexBuffer();
				}
			}
			else
			{
				return;
			}
		}
		else
		{
			// 乘以速度调节因子
			m_dTimeCounter += delta_time;
		}
	}

	if (m_fLodValue <= 0.0F)
	{
		m_vLastEmitterPos = m_vEmitterPos;
		return;
	}

	if (m_bUpdateBoundBox)
	{
		m_fTimeAfterRelocate += delta_time;

		if (m_fTimeAfterRelocate > m_fParticleLife * 2)
		{
			m_bUpdateBoundBox = false;
			m_bUpdateScale = false;
		}
	}

	IRenderContext* pContext = m_pRender->GetContext();
	const FmMat4& mtxView = pContext->GetCamera().mtxView;
	
#ifdef PERFORMANCE_DEBUG
	double dRefreshParticle = performance_time();
#endif

	if (STATE_PARTICLE_GPU == m_nComputeType)
	{
		ProcessParticles(delta_time);
	}
	else
	{
		if (m_fLodValue < 0.99f)
		{
			// 降低粒子刷新的频率
			float fps = 5.0F + 25.0F * m_fLodValue;

			m_fLastRefeshTime += delta_time;

			if (m_fLastRefeshTime > 1.0F / fps)
			{
				ProcessParticles(m_fLastRefeshTime);
				m_fLastRefeshTime = 0.0F;
			}
		}
		else
		{
			ProcessParticles(delta_time);
		}
	}

	if (!m_pParticleManager->GetVisible())
	{
		return;
	}

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dParticleRefreshTime +=
		performance_time() - dRefreshParticle;
#endif

#ifdef PERFORMANCE_DEBUG
	double dRealizeParticle = performance_time();
#endif

	// 是否开启指数雾效
	bool fog_exp = pContext->GetInt(IRenderContext::I_FOG_EXP) != 0;
	// 是否开启线性雾效
	bool fog_linear = pContext->GetInt(IRenderContext::I_FOG_LINEAR) != 0;
	bool random_color = !pdata->m_strRandomColorTex.empty();
	bool color_ani = pdata->m_bColorAni;

	FmVec3 center = m_vEmitterPos + m_vCenter;
	float camera_z = mtxView._13 * center.x + mtxView._23 * center.y 
		+ mtxView._33 * center.z + mtxView._43;

	// 是否高亮
	bool soft_particle = m_bSoftParticle;
	bool bb_y = (STATE_BILLBOARD_Y == m_nBillboardType);
	bool bb_none = (STATE_BILLBOARD_NONE == m_nBillboardType);
	bool use_fog;

	if (fog_linear)
	{
		// 雾的起始位置
		float fog_start = pContext->GetFloat(IRenderContext::F_FOG_START);
		FmVec3 v = center - pContext->GetCamera().vPosition;

		// 是否在雾的范围内
		use_fog = (VisUtil_Vec3Length(&v) + m_fRadius) >= fog_start;
	}
	else
	{
		use_fog = false;
	}

	FillConstBuffer();
	if (m_nComputeType == STATE_PARTICLE_CPU)
	{
		// 实际要画的粒子数量
		if (m_nDrawParticleNum > 0)
		{
			if (m_pTexture)
			{
				bool depth_map = true;//pContext->GetEnableRealizeDepth();

				/*
				m_VSList.SetDefine(PAR_DEPTHMAP,       "#define PAR_DEPTHMAP  1\n"); // 深度图；
				m_VSList.SetDefine(PAR_BILLBOARD_Y,    "#define PAR_BILLBOARD_Y 1\n"); // 仅Y轴广告牌模式；
				m_VSList.SetDefine(PAR_BILLBOARD_NONE, "#define PAR_BILLBOARD_NONE 1\n"); // 广告牌模式；
				m_VSList.SetDefine(PAR_HIGHLIGHT,      "#define PAR_HIGHLIGHT 1\n"); // 高亮；
				m_VSList.SetDefine(PAR_FOGUSE,         "#define PAR_FOGUSE 1\n"); // 雾开关；
				m_VSList.SetDefine(PAR_FOGLINEAR,      "#define PAR_FOGLINEAR 1\n"); // 线性雾；
				m_VSList.SetDefine(PAR_FOGEXP,         "#define PAR_FOGEXP 1\n"); // 体积雾；
				m_VSList.SetDefine(PAR_GPU,            "#define PAR_GPU 1\n"); // 渲染模式；
				*/
				soft_particle &= depth_map;
				size_t isGPU = size_t(m_nComputeType==STATE_PARTICLE_GPU);

				size_t shader_flag
					= (size_t(depth_map))
					+ (size_t(m_nBillboardType==STATE_BILLBOARD_Y) << 1)
					+ (size_t(m_nBillboardType==STATE_BILLBOARD_NONE) << 2)
					+ (size_t(m_bSoftParticle) << 3)
					+ (size_t(use_fog) << 4)
					+ (size_t(fog_linear) << 5)
					+ (size_t(fog_exp) << 6)
					+ (size_t(isGPU) << 7);

				DrawParticle_CPU(m_nDrawParticleNum,m_pParticleManager->GetShaderHandle(shader_flag,(isGPU!=0)),m_nBlendType, 
					camera_z, depth_map);
			}

			//if (!pdata->m_strRefractionTex.empty())
			/*if (m_pRefractionTex)
			{
				size_t shader_flag = 0;
				IVertexShader* pVS = m_pParticleManager->GetDistortionVS(shader_flag);
				IPixelShader* pPS = m_pParticleManager->GetDistortionPS(shader_flag);

				// 画折射
				DrawDistortionMaterial_CPU(m_nDrawParticleNum, pVS, pPS);
			}*/
		}
	}
	else if ((m_nComputeType == STATE_PARTICLE_GPU)
		|| (m_nComputeType == STATE_PARTICLE_AUTO))
	{
		// 实际要画的粒子数量
		if (m_nParticleNum > 0)
		{
			if (m_pTexture)
			{	
				bool depth_map = true;//pContext->GetEnableRealizeDepth();

				soft_particle &= depth_map;
				size_t isGPU = size_t(m_nComputeType==STATE_PARTICLE_GPU);

				size_t shader_flag
					= (size_t(depth_map))
					+ (size_t(m_nBillboardType==STATE_BILLBOARD_Y) << 1)
					+ (size_t(m_nBillboardType==STATE_BILLBOARD_NONE) << 2)
					+ (size_t(m_bSoftParticle) << 3)
					+ (size_t(use_fog) << 4)
					+ (size_t(fog_linear) << 5)
					+ (size_t(fog_exp) << 6)
					+ (size_t(isGPU) << 7);

				DrawParticle_GPU(
					m_nParticleNum,
					m_pParticleManager->GetShaderHandle(shader_flag,(isGPU!=0)),
					m_nBlendType,
					camera_z,
					depth_map
					);
			}

			////if (!pdata->m_strRefractionTex.empty())
			//if (m_pRefractionTex)
			//{
			//	size_t shader_flag = size_t(bb_y)
			//		+ (size_t(bb_none) << 1)
			//		+ (size_t(true) << 2)
			//		+ (size_t(STATE_BLEND_ADD == m_nBlendType) << 3);
			//	IVertexShader* pVS = m_pParticleManager->GetDistortionVS(shader_flag);
			//	IPixelShader* pPS = m_pParticleManager->GetDistortionPS(shader_flag);

			//	DrawDistortionMaterial_GPU(pVS, pPS);
			//}
		}
	}
	else if (m_nComputeType == STATE_PARTICLE_GLOBAL)
	{
		// 实际要画的粒子数量
		if (m_nParticleNum > 0)
		{
			//if (!pdata->m_strTextureName.empty())
			if (m_pTexture)
			{	
				bool depth_map = true;//pContext->GetEnableRealizeDepth();
				size_t isGPU = size_t(m_nComputeType==STATE_PARTICLE_GPU);

				soft_particle &= depth_map;

				size_t shader_flag = size_t(use_fog) 
					+ (size_t(soft_particle) << 1) 
					+ (size_t(bb_y) << 2)
					+ (size_t(bb_none) << 3)
					+ (size_t(true) << 4)
					+ (size_t(STATE_BLEND_ADD == m_nBlendType) << 5)
					+ (size_t(fog_exp) << 6)
					+ (size_t(random_color) << 7)
					+ (size_t(true) << 8)
					+ (size_t(m_bWaveParticle) << 9)
					+ (size_t(m_bClipPlaneEnable) << 10)
					+ (size_t(color_ani) << 11);

				DrawParticle_GPU(m_nParticleNum,m_pParticleManager->GetShaderHandle(shader_flag,(isGPU!=0)),
					m_nBlendType, camera_z, depth_map);
			}

			////if (!pdata->m_strRefractionTex.empty())
			//if (m_pRefractionTex)
			//{
			//	size_t shader_flag = size_t(bb_y)
			//		+ (size_t(bb_none) << 1)
			//		+ (size_t(true) << 2)
			//		+ (size_t(STATE_BLEND_ADD == m_nBlendType) << 3)
			//		+ (size_t(true) << 4);
			//	IVertexShader* pVS = m_pParticleManager->GetDistortionVS(shader_flag);
			//	IPixelShader* pPS = m_pParticleManager->GetDistortionPS(shader_flag);

			//	DrawDistortionMaterial_GPU(pVS, pPS);
			//}
		}
	}

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dParticleRealizeTime +=
		performance_time() - dRealizeParticle;
#endif
}

void CParticleNode::ProcessParticles(float delta_time)
{
	CParticleData* pdata = this->m_pData;
	double total_time = double(m_dTimeCounter) * 1000;
	FmVec3 emitter_pos;
	FmVec3 last_emitter_pos;
	FmVec3 emitter_dir;

	if (pdata->m_bLocalSpaceEmitter)
	{
		emitter_pos = m_vLinkPos;
		last_emitter_pos = m_vLinkPos;
		emitter_dir = FmVec3(0.0F, 1.0F, 0.0F);
	}
	else
	{
		emitter_pos = m_vEmitterPos;
		last_emitter_pos = m_vLastEmitterPos;

		if (pdata->m_bLocalAngleEmitter)
		{
			emitter_dir = m_vEmitterDir;
		}
		else
		{
			emitter_dir = FmVec3(0.0F, 1.0F, 0.0F);
		}
	}

	if (m_bUpdateBoundBox && (m_fTimeAfterRelocate < m_fParticleLife))
	{
		m_vBoxMin = FmVec3(0.0F, 0.0F, 0.0F);
		m_vBoxMax = FmVec3(0.0F, 0.0F, 0.0F);
	}

	if (m_nComputeType == STATE_PARTICLE_CPU)
	{
		float avg_scale = (m_vScale.x + m_vScale.z) * 0.5F;
		float speed_scale = m_vScale.y;
		
		for (int n = 0; n < m_nParticleNum; ++n)
		{
			particle_t* p = &m_Particle[n];
			double node_total_time = total_time - p->time;
			double node_last_time = node_total_time - delta_time * 1000.0F;
			float node_time = (float)fmod(node_total_time, 
				pdata->m_nParticleLife);
			float blend_value = node_time / (delta_time * 1000);
			//当前循环次数
			int r_c = (int)floor(node_total_time / pdata->m_nParticleLife);
			//上帧循环次数
			int r_l = (int)floor(node_last_time / pdata->m_nParticleLife);

			if (r_c != r_l && r_l>0)//这里做一个保护
			{
				if (m_bUpdateBoundBox)
				{
					if (p->emitpos != emitter_pos)
					{
						p->emitpos = emitter_pos * (1.0F - blend_value) 
							+ last_emitter_pos * blend_value;
					}

					if (p->dir != emitter_dir && !pdata->m_bRandomDirection)
					{
						p->dir = emitter_dir;
					}

					if (m_bUpdateScale)
					{
						UpdateParticlePartData(p);
					}
				}

				if (m_bEmitSwitch)
				{
					if (FloatEqual(p->size, 0.0F))
					{
						float size = pdata->m_fSizeDeviation * rand_half();
						
						p->size = (pdata->m_fInitSize + size) * avg_scale;
					}
				}
				else
				{
					if (!FloatEqual(p->size, 0.0F))
					{
						p->size = 0.0F;
					}
				}
			}
		}
	}

	if (m_nComputeType == STATE_PARTICLE_CPU)
	{
		if (pdata->m_bFlashTail)
		{
 			UpdateFlashTailVertexBuffer(m_fLodValue);
		}
		else
		{
 			UpdateCPUVertexBuffer(m_fLodValue);
		}
	}
	else
	{
		if (m_bUpdateBoundBox)
		{
			UpdateGPUBoundBox();
		}
	}

	m_vLastEmitterPos = m_vEmitterPos;
}

void CParticleNode::SetSystemDelay(float value)
{
	// 粒子总计时为负数时粒子不发射
	m_dTimeCounter = -value;
}

void CParticleNode::SetEmitSwitch(bool value)
{
	if (m_bEmitSwitch != value)
	{
		// 将GPU粒子转换成CPU运算
		UpdateComputeType();

		m_bEmitSwitch = value;
	}
}

FmVec3 CParticleNode::GetBoxSize() const
{
	return m_vBoxMax - m_vBoxMin;
}

float CParticleNode::GetRadius() const
{
	if (m_dTimeCounter > 0.0F)
	{
		return m_fRadius;
	}
	else
	{
		return 0.0F;
	}
}

const char* CParticleNode::GetName() const
{
	return m_pData->GetName();
}

const char* CParticleNode::GetConfig() const
{
	return m_pData->GetIniName();
}

void CParticleNode::SetEmitterPos(const FmVec3& pos)
{
	if (m_dTimeCounter <= 0.0)
	{
		m_vLastEmitterPos = pos;
	}

	m_vEmitterPos = pos;

	if (pos != m_vLastEmitterPos)
	{
		if (!m_pData->m_bStaticBoundBox)
		{
			m_bUpdateBoundBox = true;
			m_fTimeAfterRelocate = 0.0F;
		}
		
		// 将GPU粒子转换成CPU运算
		UpdateComputeType();
	}
}

void CParticleNode::SetEmitterDir(const FmVec3& dir)
{
	FmVec3Normalize(&m_vEmitterDir, &dir);

	FmQuat quaternionTemp;
	float fPitch, fYaw, fRoll;

	VisUtil_QuaternionFromNormal(&quaternionTemp, &dir);
	VisUtil_QuaternionToPitchYawRoll(&quaternionTemp, &fPitch, &fYaw, &fRoll);

	m_vEmitterAngle = FmVec3(fPitch, fYaw, fRoll);

	if (!m_pData->m_bStaticBoundBox)
	{
		m_bUpdateBoundBox = true;
		m_fTimeAfterRelocate = 0.0F;
	}

	// 将GPU粒子转换成CPU运算
	UpdateComputeType();
}

void CParticleNode::SetEmitterAngle(const FmVec3& angle)
{
	const float sin_ax = sinf(angle.x);
	const float cos_ax = cosf(angle.x);
	const float sin_ay = sinf(angle.y);
	const float cos_ay = cosf(angle.y);
	const float sin_az = sinf(angle.z);
	const float cos_az = cosf(angle.z);
	
	m_vEmitterDir.x = -sin_az * cos_ay + cos_az * sin_ax * sin_ay;
	m_vEmitterDir.y = cos_az * cos_ax;
	m_vEmitterDir.z = -sin_az * -sin_ay + cos_az * sin_ax * cos_ay;
	m_vEmitterAngle = angle;

	if (!m_pData->m_bStaticBoundBox)
	{
		m_bUpdateBoundBox = true;
		m_fTimeAfterRelocate = 0.0F;
	}

	// 将GPU粒子转换成CPU运算
	UpdateComputeType();
}

void CParticleNode::SetScale(const FmVec3& scale)
{
	if (!FloatEqual(scale.x, m_vScale.x) || !FloatEqual(scale.y, m_vScale.y)
		|| !FloatEqual(scale.z, m_vScale.z))
	{
		m_vScale = scale;

		if (!m_pData->m_bStaticBoundBox)
		{
			m_bUpdateBoundBox = true;
			m_bUpdateScale = true;
			m_fTimeAfterRelocate = 0.0F;
		}

		// 将GPU粒子转换成CPU运算
		UpdateComputeType();
	}
}

const FmVec3& CParticleNode::GetScale() const
{
	return m_vScale;
}

void CParticleNode::SetRepeat(bool value)
{ 
	m_bRepeat = value; 
	
	if (!m_bRepeat)
	{
		m_nParticleEndTime = int(ceil((float)m_pData->m_nParticleNum 
			/ m_pData->m_nOneEmitNumber) * (m_pData->m_fEmissionTime));
	}
}

void CParticleNode::SetAlpha(unsigned int value)
{
	m_nParticlAppendColor = (value << 24 & 0xFF000000) 
		| (m_nParticlAppendColor & 0x00FFFFFF);
	m_nParticleColor = (((value * (m_pData->m_nParticleColor >> 24)) / 255) << 24) 
		| (m_nParticleColor & 0x00FFFFFF);
}

unsigned int CParticleNode::GetAlpha() const
{
	return (m_nParticlAppendColor >> 24) & 0xFF;
}

void CParticleNode::SetAppendColor(unsigned int value)
{
	m_nParticlAppendColor = value;
	
	int pcolor = m_pData->m_nParticleColor;

	m_nParticleColor =
		((((pcolor >> 24) & 0xFF) * ((value >> 24) & 0xFF) / 255) << 24) 
		| ((((pcolor >> 16) & 0xFF) * ((value >> 16) & 0xFF) / 255) << 16) 
		| ((((pcolor >> 8) & 0xFF) * ((value >> 8) & 0xFF) / 255) << 8) 
		| ((((pcolor >> 0) & 0xFF) * ((value >> 0) & 0xFF) / 255) << 0);
}

unsigned int CParticleNode::GetAppendColor() const
{
	return m_nParticlAppendColor;
}

bool CParticleNode::LinkToObject(const PERSISTID& id, 
	const FmVec3& link_pos)
{
	m_bLinkEmitter = true;
	m_LinkObjectID = id;
	m_vLinkPos = link_pos;
	m_vLinkDirPos = m_vLinkPos + m_vEmitterDir;
	
	IVisBase* pLink = (IVisBase*)m_pRender->GetCore()->GetEntity(
		m_LinkObjectID);

	if (NULL == pLink)
	{
		return false;
	}

	if (!pLink->GetEntInfo()->IsKindOf("IVisBase")) 
	{
		return false;
	}

	FmVec3 pos = pLink->GetPosition();
	FmVec3 angle = pLink->GetAngle();
	
	VisUtil_MatrixRotationTranslation(&m_mtxLink, &angle, &pos);
	FmVec3TransformCoord(&m_vEmitterPos, &m_vLinkPos, &m_mtxLink);

	return true;
}

bool CParticleNode::LinkToObjectPoint(const PERSISTID& id, 
	const char* helper_name)
{
	Assert(helper_name != NULL);

	m_bLinkEmitter = true;
	m_LinkObjectID = id;
	m_vLinkPos = FmVec3(0.0F, 0.0F, 0.0F);
	m_vLinkDirPos = FmVec3(0.0F, 1.0F, 0.0F);

	IVisBase* pLink = (IVisBase*)m_pRender->GetCore()->GetEntity(id);

	if (NULL == pLink)
	{
		return false;
	}

	if (!pLink->GetEntInfo()->IsKindOf("Model")) 
	{
		return false;
	}

	Model* pModel = (Model*)pLink;
	IModelPlayer* pModelPlayer = pModel->GetModelPlayer();

	if (!pModelPlayer->IsReady())
	{
		// 模型未加载完成
		CORE_TRACE("(CParticleNode::LinkToObjectPoint)model not load complete");
		return false;
	}

	m_nHelperID = pModelPlayer->FindLabelNameIndex(helper_name);

	if (-1 == m_nHelperID)
	{	
		CORE_TRACE("(CParticleNode::LinkToObjectPoint)not find helper");
		CORE_TRACE(helper_name);
		CORE_TRACE(pModelPlayer->GetModelFileName());
		return false;
	}

	FmMat4 mtxNode;

	pModelPlayer->GetLabelMatrix(m_nHelperID, mtxNode);

	FmQuat quaternionTemp;
	float fPitch, fYaw, fRoll;

	FmQuatRotationMat4(&quaternionTemp, &mtxNode);
	VisUtil_QuaternionToPitchYawRoll(&quaternionTemp, &fPitch, &fYaw, &fRoll);

	FmVec3 pos = FmVec3(mtxNode._41, mtxNode._42, mtxNode._43);
	FmVec3 angle = FmVec3(fPitch, fYaw, fRoll);
	
	VisUtil_MatrixRotationTranslation(&m_mtxLink, &angle, &pos);
	FmVec3TransformCoord(&m_vEmitterPos, &m_vLinkPos, &m_mtxLink);
	FmVec3TransformCoord(&m_vEmitterDir, &m_vLinkDirPos, &m_mtxLink);
	m_vEmitterDir = m_vEmitterDir - m_vEmitterPos;
	FmVec3Normalize(&m_vEmitterDir, &m_vEmitterDir);

	return true;
}

#define CB_VIEWPROJ			0
#define CB_PROJECTION		4
#define CB_EYE_POS			8
#define CB_ROTATE_AXIS		9
#define CB_EMITTER_POS		10
#define CB_TIME				11
#define CB_SPEED			12
#define CB_FORCE			13
#define CB_COLOR			14
#define CB_GLOBAL			15
#define CB_CLIPPLANE		16
#define CB_UV_BIAS			17
#define CB_FOG_COLOR		18
#define CB_FOG_PARAM		19
#define CB_FOG_EXP_PARAM	20
#define CB_PIXEL_SIZE		21
#define CB_TRACK_ALPHA		24

//这里填充粒子相关数据
void CParticleNode::FillConstBuffer()
{
	IRenderContext* pContext = m_pRender->GetContext();
	const camera_t& camera = pContext->GetCamera();

	FmVec3 vViewPos = camera.vPosition;
	FmMat4 mtxProj;

	FmMat4Transpose(&mtxProj, &camera.mtxProj);
	m_cPROJECTION = mtxProj;

	CParticleData* pdata = m_pData;
	bool use_cpu = (m_nComputeType == STATE_PARTICLE_CPU);

	if (pdata->m_bLocalSpaceEmitter && pdata->m_bLocalAngleEmitter)
	{
		if (!use_cpu)
		{
			float emitter_pos[4] = { m_vLinkPos.x, m_vLinkPos.y, m_vLinkPos.z, 0.0f };

			SetEmitterPos(emitter_pos);
		}

		FmMat4 mtxWVP;
		FmMat4Multiply(&mtxWVP, &m_mtxLink, &camera.mtxViewProj);
		FmMat4Transpose(&mtxWVP, &mtxWVP);
		m_cVIEWPROJ = mtxWVP;

		// 视点逆到本地空间
		FmVec3 WorldX(m_mtxLink._11, m_mtxLink._12, m_mtxLink._13);
		FmVec3 WorldY(m_mtxLink._21, m_mtxLink._22, m_mtxLink._23);
		FmVec3 WorldZ(m_mtxLink._31, m_mtxLink._32, m_mtxLink._33);
		FmVec3 WorldPos(m_mtxLink._41, m_mtxLink._42, m_mtxLink._43);

		vViewPos -= WorldPos;
		vViewPos = FmVec3(FmVec3Dot(&vViewPos, &WorldX),
			FmVec3Dot(&vViewPos, &WorldY), FmVec3Dot(&vViewPos, &WorldZ));

		m_cEYE_POS = vViewPos;

		// 粒子旋转轴
		//VisUtil_SetShaderConst(pConst, CB_ROTATE_AXIS,
		//	D3DXVECTOR4(0.0F, 1.0F, 0.0F, 0.0F) , 1);
		m_cROTATE_AXIS = FmVec4(0.0F, 1.0F, 0.0F, 0.0F);
	}
	else
	{
		if (!use_cpu)
		{
			float emitter_pos[4] = { m_vEmitterPos.x, m_vEmitterPos.y, 
				m_vEmitterPos.z, 0.0F };
			SetEmitterPos(emitter_pos);
		}

		FmMat4 mtxWVP;
		FmMat4Transpose(&mtxWVP, &camera.mtxViewProj);
		m_cVIEWPROJ = mtxWVP;
		m_cEYE_POS = vViewPos;

		// 粒子旋转轴
		if (pdata->m_bAngleLinkEmitter)
		{
			m_cROTATE_AXIS = FmVec4(m_vEmitterDir, 0.0F);
		}
		else
		{
			m_cROTATE_AXIS = FmVec4(0.0F, 1.0F, 0.0F, 0.0F) ;
		}
	}

	// 粒子颜色
	FmVec4 particle_color = VisUtil_ColorToVector4(m_nParticleColor);
	// 粒子亮度
	particle_color*= pdata->m_fLuminance;
	m_cCOLOR = particle_color;

	//开启雾效果
	if( pContext->GetInt(IRenderContext::I_FOG_LINEAR) != 0 )
	{
		float fog_start = pContext->GetFloat(IRenderContext::F_FOG_START);
		float fog_end = pContext->GetFloat(IRenderContext::F_FOG_END);
		FmVec4 fog_param( fog_start, 1.0F / (fog_end - fog_start), 0.0F, 0.0F );
		FmVec4 fog_color;
		if( m_nBlendType == STATE_BLEND_DEFAULT )
		{
			fog_color =  pContext->GetVector4(IRenderContext::V_FOG_CURRENT);
		}
		else if( m_nBlendType == STATE_BLEND_MULTIPLY )
		{
            fog_color = FmVec4(1.0F, 1.0F, 1.0F, 1.0F);
		}
		else
		{
			fog_color = FmVec4(0.0F, 0.0F, 0.0F, 0.0F);
		}

		m_cFOG_COLOR = fog_color;
		m_cFOG_PARAM = fog_param;
		m_bEnableFogLinear = true;
	}
	else
	{
		m_bEnableFogLinear = false;
	}

	//指数雾
	if( pContext->GetInt(IRenderContext::I_FOG_EXP) != 0 )
	{
		float fog_density = pContext->GetFloat(IRenderContext::F_FOG_DENSITY);
		FmVec4 fog_exp_param;
		if (m_nBlendType == STATE_BLEND_DEFAULT)
		{
			// 非默认混合方式下雾颜色应为指定色
			fog_exp_param = pContext->GetVector4(
				IRenderContext::V_FOG_EXP_CURRENT);
			fog_exp_param.w = fog_density;
		}
		else if (m_nBlendType == STATE_BLEND_MULTIPLY)
		{
			fog_exp_param = FmVec4(1.0F, 1.0F, 1.0F, fog_density);
		}
		else
		{
			fog_exp_param = FmVec4(0.0F, 0.0F, 0.0F, fog_density);
		}
		m_cFOG_EXP_PARAM = fog_exp_param;
		m_bEnableFogExp = true;
	}
	else
	{
		m_bEnableFogExp = false;
	}

}

// 绘制CPU粒子：
bool CParticleNode::DrawParticle_CPU(int num, ParticleManager::particle_shader_handle* pShaderHandle,int blend_type, float camera_z, bool depth_map)
{
	m_num = num;
	m_blend_type = blend_type;
	m_pRender->GetSceneView()->AddBlendBatch(RenderParticle_CPU, this,camera_z,"CParticleNode_CPU");
	m_pShaderHandle = pShaderHandle;

	return true;
}


// CPU批次处理：
void CParticleNode::RenderParticle_CPU(void* pdata)
{
	CParticleNode* pthis = (CParticleNode*)pdata;

	IShaderParamOp* pShaderOp = pthis->m_pShaderHandle->pShader->GetParamOp();
	IRenderDrawOp*  pRenderDrawOp = pthis->m_pShaderHandle->pShader->GetRenderDrawOp();
	IRenderStateOp* pRenderStateOp = pthis->m_pShaderHandle->pShader->GetRenderStateOp();
	int num = pthis->m_num;
	int blend_type = pthis->m_blend_type;
	int nTailCount = pthis->m_nTailCount;

	switch( blend_type )
	{
	case STATE_BLEND_DEFAULT:
		{
            //glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
			pRenderStateOp->SetBlendFunc( IRenderStateOp::BLEND_COLOR_ONE ,IRenderStateOp::BLEND_COLOR_ONE_MINUS_SRC_ALPHA );
		}break;
	case STATE_BLEND_ADD:
		{
			//glBlendFunc(GL_ONE, GL_ONE);
			pRenderStateOp->SetBlendFunc( IRenderStateOp::BLEND_COLOR_ONE,IRenderStateOp::BLEND_COLOR_ONE);
		}break;
	case STATE_BLEND_MULTIPLY://这里存在问题
		{
			//glBlendFunc( GL_ZERO ,GL_ONE_MINUS_SRC_COLOR);//暂时先瞎写一下吧，GL似乎不关于这个有点问题
			pRenderStateOp->SetBlendFunc( IRenderStateOp::BLEND_COLOR_ZERO,IRenderStateOp::BLEND_COLOR_ONE_MINUS_SRC_COLOR);
		}break;
	}

	pthis->m_pShaderHandle->pShader->UsedShader();
	
	pShaderOp->SetTexture2D(pthis->m_pShaderHandle->tex_BaseMapHandle,pthis->m_pTexture->GetCanUseShaderTex()->GetTexture());

 	pShaderOp->SetParamValue(pthis->m_pShaderHandle->mat4ViewProj,pthis->m_cVIEWPROJ);
 	pShaderOp->SetParamValue(pthis->m_pShaderHandle->vec4ParticleColorHandle,pthis->m_cCOLOR);

	if(pthis->m_bEnableFogLinear)
	{
		pShaderOp->SetParamValue(pthis->m_pShaderHandle->vec4FogColorHandle,pthis->m_cFOG_COLOR);
		pShaderOp->SetParamValue(pthis->m_pShaderHandle->vec4FogParamHandle,pthis->m_cFOG_PARAM);
	}

	if(pthis->m_bEnableFogExp)
	{
		pShaderOp->SetParamValue(pthis->m_pShaderHandle->vec4FogExpParamHandle,pthis->m_cFOG_EXP_PARAM);
	}
	
	pRenderDrawOp->SetVB(pthis->m_pVBuffer->GetBuffer());
	pRenderDrawOp->SetIB(pthis->m_pIBuffer->GetBuffer());

	pRenderDrawOp->EnableVertexAttribArray(VERTEX_ARRAY);
	pRenderDrawOp->EnableVertexAttribArray(VERTEX_COLOR_ARRAY);
	pRenderDrawOp->EnableVertexAttribArray(TEXCOORD_ARRAY);

	pRenderDrawOp->SetVertexAttribPointer(VERTEX_ARRAY, 4,IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(particle_vertex_cpu_t), 0);
	pRenderDrawOp->SetVertexAttribPointer(VERTEX_COLOR_ARRAY,4,IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(particle_vertex_cpu_t), (void*) (4 * sizeof(float)));
	pRenderDrawOp->SetVertexAttribPointer(TEXCOORD_ARRAY,2,IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(particle_vertex_cpu_t), (void*) (8 * sizeof(float)));

	pRenderDrawOp->DrawIndex(IRenderDrawOp::DRAW_TRIANGLES, num * nTailCount * 6 ,IRenderDrawOp::VERTEX_INDEX_UNSIGNED_SHORT, &((fm_short*)0)[0]);
}


// 绘制GPU粒子：
bool CParticleNode::DrawParticle_GPU(int num,ParticleManager::particle_shader_handle* pShaderHandle,
									 int blend_type, float camera_z, bool depth_map)
{
	m_num = num;
	m_blend_type = blend_type;
	m_pRender->GetSceneView()->AddBlendBatch(RenderParticle_GPU, this,camera_z,"CParticleNode_GPU");
	m_pShaderHandle = pShaderHandle;

	return true;
}


// GPU批次处理：
void CParticleNode::RenderParticle_GPU(void* pdata)
{
	CParticleNode* pthis = (CParticleNode*)pdata;

	IShaderParamOp* pShaderOp = pthis->m_pShaderHandle->pShader->GetParamOp();
	IRenderDrawOp*  pRenderDrawOp = pthis->m_pShaderHandle->pShader->GetRenderDrawOp();
	IRenderStateOp* pRenderStateOp = pthis->m_pShaderHandle->pShader->GetRenderStateOp();
	int num = pthis->m_num;
	int blend_type = pthis->m_blend_type;
	int nTailCount = pthis->m_nTailCount;

	pRenderStateOp->EnableCullFace(false);

	switch( blend_type )
	{
	case STATE_BLEND_DEFAULT:
		{
			pRenderStateOp->SetBlendFunc( IRenderStateOp::BLEND_COLOR_ONE ,IRenderStateOp::BLEND_COLOR_ONE_MINUS_SRC_ALPHA );
		}break;
	case STATE_BLEND_ADD:
		{
			pRenderStateOp->SetBlendFunc( IRenderStateOp::BLEND_COLOR_ONE,IRenderStateOp::BLEND_COLOR_ONE);
		}break;
	case STATE_BLEND_MULTIPLY://这里存在问题
		{
			//glBlendFunc( GL_ZERO ,GL_ONE_MINUS_SRC_COLOR);//暂时先瞎写一下吧，GL似乎不关于这个有点问题
			pRenderStateOp->SetBlendFunc( IRenderStateOp::BLEND_COLOR_ZERO,IRenderStateOp::BLEND_COLOR_ONE_MINUS_SRC_COLOR);
		}break;
	}
	pthis->m_pShaderHandle->pShader->UsedShader();

	// 粒子发射位置：
	if(pthis->m_pData->m_bLocalSpaceEmitter && pthis->m_pData->m_bLocalAngleEmitter)
	{
		pShaderOp->SetParamValue(pthis->m_pShaderHandle->vec3EmitterPos, pthis->m_vLinkPos);
	}
	else
	{
		pShaderOp->SetParamValue(pthis->m_pShaderHandle->vec3EmitterPos, pthis->m_vEmitterPos);
	}

	bool bn = false;

	// 时间参数：
	FmVec4 vec4TimeVal = FmVec4(
		float(pthis->m_dTimeCounter),			// 当前时间；
		float(pthis->m_pData->m_nParticleLife),	// 粒子生命周期；
		pthis->m_pData->m_fEmissionTime,		// 粒子出生时间；
		float(pthis->m_nParticleEndTime));		// 粒子结束时间；
	bn = pShaderOp->SetParamValue(pthis->m_pShaderHandle->vec4Time, vec4TimeVal);

	// 速度参数：
	FmVec4 vec4SpeedVal = FmVec4(pthis->m_pData->m_fDirectionDeviation, pthis->m_pData->m_fChaosVal, 0.0f, 0.0f );
	bn = pShaderOp->SetParamValue(pthis->m_pShaderHandle->vec4Speed, vec4SpeedVal);

	// 力量参数：
	FmVec3 vec3WindDir = pthis->m_fWindPower * pthis->m_vWindDirection * pthis->m_pData->m_fWindEffect;
	FmVec4 vec4ForceVal = FmVec4(vec3WindDir.x, vec3WindDir.y, vec3WindDir.z,
		pthis->m_pData->m_fGravity);
	bn = pShaderOp->SetParamValue(pthis->m_pShaderHandle->vec4Force, vec4ForceVal);

	// UV偏移：
	FmVec4 vec4UVBias = FmVec4(float(1.0f / pthis->m_pData->m_nParticleFrame), 0.0f, 0.0f, 0.0f);
	bn = pShaderOp->SetParamValue(pthis->m_pShaderHandle->vec4UV_Bias, vec4UVBias, 1);

	// 动画数据
	bn = pShaderOp->SetParamValue(pthis->m_pShaderHandle->vec4TrackAlpha, 128, (FmVec4*)(pthis->m_pData->m_vVisibilityTrack));

	// 粒子材质：
	bn = pShaderOp->SetTexture2D(pthis->m_pShaderHandle->tex_BaseMapHandle,pthis->m_pTexture->GetCanUseShaderTex()->GetTexture());

	// 视图矩阵：
 	bn = pShaderOp->SetParamValue(pthis->m_pShaderHandle->mat4ViewProj,pthis->m_cVIEWPROJ);

	// 眼睛坐标：
	bn = pShaderOp->SetParamValue(pthis->m_pShaderHandle->vec3ViewPostion,pthis->m_cEYE_POS);

	// 粒子旋转轴：
	bn = pShaderOp->SetParamValue(pthis->m_pShaderHandle->vec4RotateAxis,pthis->m_cROTATE_AXIS);

	// 粒子颜色：
 	bn = pShaderOp->SetParamValue(pthis->m_pShaderHandle->vec4ParticleColorHandle,pthis->m_cCOLOR);

	if(pthis->m_bEnableFogLinear)
	{
		// 线性雾参数：
		bn = pShaderOp->SetParamValue(pthis->m_pShaderHandle->vec4FogColorHandle,pthis->m_cFOG_COLOR);
		bn = pShaderOp->SetParamValue(pthis->m_pShaderHandle->vec4FogParamHandle,pthis->m_cFOG_PARAM);
	}

	if(pthis->m_bEnableFogExp)
	{
		// 体积雾参数：
		bn = pShaderOp->SetParamValue(pthis->m_pShaderHandle->vec4FogExpParamHandle,pthis->m_cFOG_EXP_PARAM);
	}





	// 设置缓存：
	pRenderDrawOp->SetVB(pthis->m_pVBuffer->GetBuffer());
	pRenderDrawOp->SetIB(pthis->m_pIBuffer->GetBuffer());

	// 功能开关：
	pRenderDrawOp->EnableVertexAttribArray(GPU_VERTEX);
	pRenderDrawOp->EnableVertexAttribArray(GPU_VERTEX_NORMAL);
	pRenderDrawOp->EnableVertexAttribArray(GPU_VERTEX_UV);
	pRenderDrawOp->EnableVertexAttribArray(GPU_VERTEX_POSDELTA);
	pRenderDrawOp->EnableVertexAttribArray(GPU_VERTEX_RANDOM0);
	pRenderDrawOp->EnableVertexAttribArray(GPU_VERTEX_RANDOM1);
	pRenderDrawOp->EnableVertexAttribArray(GPU_VERTEX_RANDOM2);

	// 顶点属性：
	pRenderDrawOp->SetVertexAttribPointer(GPU_VERTEX,          4, IRenderDrawOp::VERTEX_DATA_FLOAT, sizeof(particle_vertex_gpu_t), (void*)( 0));
	pRenderDrawOp->SetVertexAttribPointer(GPU_VERTEX_NORMAL,   3, IRenderDrawOp::VERTEX_DATA_FLOAT, sizeof(particle_vertex_gpu_t), (void*)( 4 * sizeof(float)));
	pRenderDrawOp->SetVertexAttribPointer(GPU_VERTEX_UV,       2, IRenderDrawOp::VERTEX_DATA_FLOAT, sizeof(particle_vertex_gpu_t), (void*)( 7 * sizeof(float)));
	pRenderDrawOp->SetVertexAttribPointer(GPU_VERTEX_POSDELTA, 2, IRenderDrawOp::VERTEX_DATA_FLOAT, sizeof(particle_vertex_gpu_t), (void*)( 9 * sizeof(float)));
	pRenderDrawOp->SetVertexAttribPointer(GPU_VERTEX_RANDOM0,  2, IRenderDrawOp::VERTEX_DATA_FLOAT, sizeof(particle_vertex_gpu_t), (void*)(11 * sizeof(float)));
	pRenderDrawOp->SetVertexAttribPointer(GPU_VERTEX_RANDOM1,  2, IRenderDrawOp::VERTEX_DATA_FLOAT, sizeof(particle_vertex_gpu_t), (void*)(13 * sizeof(float)));
	pRenderDrawOp->SetVertexAttribPointer(GPU_VERTEX_RANDOM2,  2, IRenderDrawOp::VERTEX_DATA_FLOAT, sizeof(particle_vertex_gpu_t), (void*)(15 * sizeof(float)));

	// 执行绘制：
	pRenderDrawOp->DrawIndex(IRenderDrawOp::DRAW_TRIANGLES, num * nTailCount * 6 ,IRenderDrawOp::VERTEX_INDEX_UNSIGNED_SHORT, &((fm_short*)0)[0]);
}


/*
bool CParticleNode::DrawParticle_GPU(IVertexShader* pVS, IPixelShader* pPS,
	int blend_type, float camera_z, bool depth_map)
{
	DxSceneBatch* pBatch;

	if (m_bWaveParticle)
	{
		pBatch = m_pRender->AddWaterWaveBatch("ParticleNode");
	}
	else
	{
		pBatch = m_pRender->AddBlendBatch("ParticleNode");
		pBatch->SetCameraZ(camera_z);
	}

	if (m_pTexture)
	{
		pBatch->SetPSTexture(0, m_pTexture->GetShaderTex());
	}
	else
	{
		pBatch->SetPSTexture(0, NULL);
	}

	if (depth_map)
	{
		IDxContext* pContext = m_pRender->GetContext();
		// 可以使用低分辨率的深度图
		IDxColorRT* depth_tex = pContext->GetHalfDepthRT();

		if (NULL == depth_tex)
		{
			depth_tex = pContext->GetDepthRT();
		}

		pBatch->SetPSTexture(1, depth_tex);
	}

	pBatch->SetInputLayout(m_pParticleManager->GetGpuParticleLayout());
	pBatch->SetVertexBuffer(0, m_pDynamicVBuffer, 
		sizeof(particle_vertex_gpu_t), 0);
	pBatch->SetIndexBuffer(m_pIBuffer, 0);
	pBatch->SetVSConstBuffer(0, m_pCB);
	pBatch->SetPSConstBuffer(0, m_pCB);
	//pBatch->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//pBatch->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pBatch->SetRasterState(m_pParticleManager->GetRasterState());

	switch (blend_type)
	{
	case STATE_BLEND_DEFAULT:
		//pBatch->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		//pBatch->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		pBatch->SetBlendState(m_pParticleManager->GetDefaultBlendState());
		
		if (!m_pData->m_bNotDownLevel)
		{
			// 可降级显示
			pBatch->SetFlags(DxSceneBatch::FLAG_DOWN_LEVEL);
		}
		break;
	case STATE_BLEND_ADD:
		//pBatch->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		//pBatch->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		pBatch->SetBlendState(m_pParticleManager->GetAddBlendState());
		
		if (!m_pData->m_bNotDownLevel)
		{
			// 可降级显示
			pBatch->SetFlags(DxSceneBatch::FLAG_DOWN_LEVEL);
		}
		break;
	case STATE_BLEND_MULTIPLY:
		//pBatch->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		//pBatch->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
		pBatch->SetBlendState(m_pParticleManager->GetMultiplyBlendState());
		break;
	}

//	IDxContext* pContext = m_pRender->GetContext();

	pBatch->SetVertexShader(pVS);
	pBatch->SetPixelShader(pPS);
	pBatch->SetPrimitive(DX_PRIMITIVE_TRIANGLELIST, m_nParticleNum * 4, 
		m_nParticleNum * 6);

//	// 设置WVP
//	const dx_camera_t& camera = pContext->GetCamera();
//	const FmMat4& mtxViewProj = camera.mtxViewProj;
//	FmVec3 vViewPos = camera.vPosition;
//	FmMat4 mtxProj;
//
//	FmMat4Transpose(&mtxProj, &pContext->GetCamera().mtxProj);
//
//	pBatch->SetVSConst(CV_PROJECTION, (float*)&mtxProj, 4);
//
//	CParticleData* pdata = m_pData;
//
//	if (pdata->m_bLocalSpaceEmitter && pdata->m_bLocalAngleEmitter)
//	{
//		float emitter_pos[4] = { m_vLinkPos.x, m_vLinkPos.y, m_vLinkPos.z, 
//			0.0F };
//
//		pBatch->SetVSConst(CV_EMITTER_POS, emitter_pos, 1);
//
//		FmMat4 mtxWVP;
//
//		FmMat4MultiplyTranspose(&mtxWVP, &m_mtxLink, &mtxViewProj);
//
//		pBatch->SetVSConst(CV_VIEWPROJ, (float*)&mtxWVP, 4);
//
//		// 视点逆到本地空间
//		FmVec3 WorldX(m_mtxLink._11, m_mtxLink._12, m_mtxLink._13);
//		FmVec3 WorldY(m_mtxLink._21, m_mtxLink._22, m_mtxLink._23);
//		FmVec3 WorldZ(m_mtxLink._31, m_mtxLink._32, m_mtxLink._33);
//		FmVec3 WorldPos(m_mtxLink._41, m_mtxLink._42, m_mtxLink._43);
//
//		vViewPos -= WorldPos;
//		vViewPos = FmVec3(FmVec3Dot(&vViewPos, &WorldX),
//			FmVec3Dot(&vViewPos, &WorldY), FmVec3Dot(&vViewPos, &WorldZ));
//
//		pBatch->SetVSConst(CV_EYE_POS, vViewPos, 1);
//
//		// 粒子旋转轴
//		pBatch->SetVSConst(CV_ROTATE_AXIS,
//			FmVec4(0.0F, 1.0F, 0.0F, 0.0F) , 1);
//	}
//	else
//	{
//		float emitter_pos[4] = { m_vEmitterPos.x, m_vEmitterPos.y, 
//			m_vEmitterPos.z, 0.0F };
//
//		pBatch->SetVSConst(CV_EMITTER_POS, emitter_pos, 1);
//
//		FmMat4 mtxWVP;
//
//		FmMat4Transpose(&mtxWVP, &mtxViewProj);
//
//		pBatch->SetVSConst(CV_VIEWPROJ, (float*)&mtxWVP, 4);
//		pBatch->SetVSConst(CV_EYE_POS, vViewPos, 1);
//
//		// 粒子旋转轴
//		if (pdata->m_bAngleLinkEmitter)
//		{
//			pBatch->SetVSConst(CV_ROTATE_AXIS, 
//				FmVec4(m_vEmitterDir, 0.0F), 1);
//		}
//		else
//		{
//			pBatch->SetVSConst(CV_ROTATE_AXIS,
//				FmVec4(0.0F, 1.0F, 0.0F, 0.0F) , 1);
//		}
//	}
//
//	float time_value[4] = { float(m_dTimeCounter), 
//		float(pdata->m_nParticleLife), pdata->m_fEmissionTime,
//		float(m_nParticleEndTime) };
//
//	pBatch->SetVSConst(CV_TIME, time_value, 1);
//
//	float speed_value[4] = { pdata->m_fDirectionDeviation, pdata->m_fChaosVal,
//		0.0F, 0.0F };
//
//	pBatch->SetVSConst(CV_SPEED, speed_value, 1);
//
//	FmVec4 force_value(
//		m_fWindPower * m_vWindDirection * pdata->m_fWindEffect, 
//		pdata->m_fGravity);
//
//	pBatch->SetVSConst(CV_FORCE, force_value, 1);
//
//	// 粒子颜色
//	D3DXCOLOR particle_color = m_nParticleColor;
//
//	// 粒子亮度
//	particle_color *= pdata->m_fLuminance;
//
//	pBatch->SetVSConst(CV_COLOR, particle_color, 1);
//
//	if (STATE_PARTICLE_GLOBAL == m_nComputeType)
//	{
//		// 全局粒子的有效视距
//		float sight_distance[4] = {
//			pContext->GetCamera().mtxViewInverse._13,
//			pContext->GetCamera().mtxViewInverse._23,
//			pContext->GetCamera().mtxViewInverse._33,
//			m_fRadius};
//
//		pBatch->SetVSConst(CV_GLOBAL, sight_distance, 1);
//	}
//
//	if (m_bClipPlaneEnable)
//	{
//		pBatch->SetVSConst(CV_CLIPPLANE, m_vClipPlane, 1);
//	}
//
//	// UV偏移
//	float uv_bias[4] = {float(1.0F / pdata->m_nParticleFrame), 0.0F, 0.0F, 
//		0.0F };
//
//	pBatch->SetVSConst(CV_UV_BIAS, (float*)uv_bias, 1);
//
//	// 动画数据
//	pBatch->SetVSConst(CV_TRACK_ALPHA, 
//		(float*)pdata->m_vVisibilityTrack, 128);
//
//	// Fog
//	if (pContext->GetInt(IDxContext::I_FOG_LINEAR) != 0)
//	{
//		//DWORD fog_color = pContext->GetColor(IDxContext::C_FOG_COLOR);
//		float fog_start = pContext->GetFloat(IDxContext::F_FOG_START);
//		float fog_end = pContext->GetFloat(IDxContext::F_FOG_END);
//		float fog_param[4] = { fog_start, fog_end - fog_start, 0.0F, 0.0F };
//		FmVec4 fog_color;
//
//		if (blend_type == STATE_BLEND_DEFAULT)
//		{
//			// 非默认混合方式下雾颜色应为指定色
//			//fFogColor = fog_color;
//			fog_color = pContext->GetVector4(IDxContext::V_FOG_CURRENT);
//		}
//		else if (blend_type == STATE_BLEND_MULTIPLY)
//		{
//			fog_color = FmVec4(1.0F, 1.0F, 1.0F, 1.0F);
//		}
//		else
//		{
//			fog_color = FmVec4(0.0F, 0.0F, 0.0F, 0.0F);
//		}
//
//		pBatch->SetPSConst(CP_FOG_COLOR, fog_color, 1);
//		pBatch->SetPSConst(CP_FOG_PARAM, fog_param, 1);
//	}
//
//	if (pContext->GetInt(IDxContext::I_FOG_EXP) != 0)
//	{
//		//DWORD fog_exp_color = pContext->GetColor(IDxContext::C_FOG_EXP_COLOR);
//		float fog_density = pContext->GetFloat(IDxContext::F_FOG_DENSITY);
//		FmVec4 fog_exp_param;
//
//		if (blend_type == STATE_BLEND_DEFAULT)
//		{
//			// 非默认混合方式下雾颜色应为指定色
//			//fFogColor = fog_exp_color;
//			fog_exp_param = pContext->GetVector4(
//				IDxContext::V_FOG_EXP_CURRENT);
//			fog_exp_param.w = fog_density;
//		}
//		else if (blend_type == STATE_BLEND_MULTIPLY)
//		{
//			fog_exp_param = FmVec4(1.0F, 1.0F, 1.0F, fog_density);
//		}
//		else
//		{
//			fog_exp_param = FmVec4(0.0F, 0.0F, 0.0F, fog_density);
//		}
//
//		//fFogColor.a = fog_density;
//		pBatch->SetPSConst(CP_FOG_EXP_PARAM, fog_exp_param, 1);
//	}
//
//	if (depth_map)
//	{
//		// 可以使用低分辨率的深度图
//		IDxColorRT* depth_tex = pContext->GetHalfDepthRT();
//
//		if (NULL == depth_tex)
//		{
//			depth_tex = pContext->GetDepthRT();
//		}
//
//		float pixelscale = float(1 << pContext->GetBlendDownLevel());
//		
//		pBatch->SetPSTexture(1, depth_tex);
//
//		const dx_viewport_t& vp = camera.Viewport;
//		float pixel_size[4] = { pixelscale / vp.fWidth, 
//			pixelscale / vp.fHeight, 0.0F, 0.0F }; 
//
//		pBatch->SetPSConst(CP_PIXEL_SIZE, pixel_size, 1);
//	}

	return true;
}
*/

/*
bool CParticleNode::DrawDistortionMaterial_GPU(IVertexShader* pVS, 
	IPixelShader* pPS)
{
	IDxContext* pContext = m_pRender->GetContext();

	if (!pContext->GetEnableRealizeRefraction())
	{
		return false;
	}

	DxSceneBatch* pBatch = m_pRender->AddRefractionBatch("Ref_ParticleNode");

	if (m_pRefractionTex)
	{
		pBatch->SetPSTexture(0, m_pRefractionTex->GetShaderTex());
	}
	else
	{
		pBatch->SetPSTexture(0, NULL);
	}

	pBatch->SetInputLayout(m_pParticleManager->GetGpuParticleLayout());
	pBatch->SetVertexBuffer(0, m_pDynamicVBuffer, 
		sizeof(particle_vertex_gpu_t), 0);
	pBatch->SetIndexBuffer(m_pIBuffer, 0);
	pBatch->SetVSConstBuffer(0, m_pCB);
	pBatch->SetPSConstBuffer(0, m_pCB);

	//pBatch->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//pBatch->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	//pBatch->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//pBatch->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pBatch->SetRasterState(m_pParticleManager->GetRasterState());
	pBatch->SetBlendState(m_pParticleManager->GetNormalBlendState());

	pBatch->SetVertexShader(pVS);
	pBatch->SetPixelShader(pPS);
	pBatch->SetPrimitive(DX_PRIMITIVE_TRIANGLELIST, m_nParticleNum * 4, 
		m_nParticleNum * 6);

//	// 设置WVP
//	const dx_camera_t& camera = pContext->GetCamera();
//	const FmMat4& mtxViewProj = camera.mtxViewProj;
//	FmVec3 vViewPos = camera.vPosition;
//	FmMat4 mtxProj;
//
//	FmMat4Transpose(&mtxProj, &pContext->GetCamera().mtxProj);
//
//	pBatch->SetVSConst(CV_PROJECTION, (float*)&mtxProj, 4);
//	
//	CParticleData* pdata = m_pData;
//
//	if (pdata->m_bLocalSpaceEmitter && pdata->m_bLocalAngleEmitter)
//	{
//		float emitter_pos[4] = { m_vLinkPos.x, m_vLinkPos.y, m_vLinkPos.z, 
//			0.0F };
//
//		pBatch->SetVSConst(CV_EMITTER_POS, emitter_pos, 1);
//
//		FmMat4 mtxWVP;
//
//		FmMat4MultiplyTranspose(&mtxWVP, &m_mtxLink, &mtxViewProj);
//
//		pBatch->SetVSConst(CV_VIEWPROJ, (float*)&mtxWVP, 4);
//
//		// 视点逆到本地空间
//		FmVec3 WorldX(m_mtxLink._11, m_mtxLink._12, m_mtxLink._13);
//		FmVec3 WorldY(m_mtxLink._21, m_mtxLink._22, m_mtxLink._23);
//		FmVec3 WorldZ(m_mtxLink._31, m_mtxLink._32, m_mtxLink._33);
//		FmVec3 WorldPos(m_mtxLink._41, m_mtxLink._42, m_mtxLink._43);
//
//		vViewPos -= WorldPos;
//		vViewPos = FmVec3(FmVec3Dot(&vViewPos, &WorldX),
//			FmVec3Dot(&vViewPos, &WorldY), FmVec3Dot(&vViewPos, &WorldZ));
//
//		pBatch->SetVSConst(CV_EYE_POS, vViewPos, 1);
//
//		// 粒子旋转轴
//		pBatch->SetVSConst(CV_ROTATE_AXIS,
//			FmVec4(0.0F, 1.0F, 0.0F, 0.0F) , 1);
//	}
//	else
//	{
//		float emitter_pos[4] = { m_vEmitterPos.x, m_vEmitterPos.y, 
//			m_vEmitterPos.z, 0.0F };
//
//		pBatch->SetVSConst(CV_EMITTER_POS, emitter_pos, 1);
//
//		FmMat4 mtxWVP;
//
//		FmMat4Transpose(&mtxWVP, &mtxViewProj);
//
//		pBatch->SetVSConst(CV_VIEWPROJ, (float*)&mtxWVP, 4);
//		pBatch->SetVSConst(CV_EYE_POS, vViewPos, 1);
//
//		// 粒子旋转轴
//		if (pdata->m_bAngleLinkEmitter)
//		{
//			pBatch->SetVSConst(CV_ROTATE_AXIS, 
//				FmVec4(m_vEmitterDir, 0.0F), 1);
//		}
//		else
//		{
//			pBatch->SetVSConst(CV_ROTATE_AXIS,
//				FmVec4(0.0F, 1.0F, 0.0F, 0.0F) , 1);
//		}
//	}
//
//	float time_value[4] = { float(m_dTimeCounter),
//		float(pdata->m_nParticleLife), pdata->m_fEmissionTime,
//		float(m_nParticleEndTime) };
//	
//	pBatch->SetVSConst(CV_TIME, time_value, 1);
//
//	float speed_value[4] = { pdata->m_fInitSpeed, pdata->m_fSpeedChaos,
//		pdata->m_fSpeedDeviation, pdata->m_fDirectionDeviation };
//
//	pBatch->SetVSConst(CV_SPEED, speed_value, 1);
//
//	FmVec4 force_value(
//		m_fWindPower * m_vWindDirection * pdata->m_fWindEffect, 
//		pdata->m_fGravity);
//	
//	pBatch->SetVSConst(CV_FORCE, force_value, 1);
//
//	// 折射强度
//	float distorion_intensity[4] = { 1.0F, 1.0F, 1.0F, 
//		pdata->m_fDistortionIntensity };
//
//	pBatch->SetVSConst(CV_COLOR, distorion_intensity, 1);
//
//	if (STATE_PARTICLE_GLOBAL == m_nComputeType)
//	{
//		// 全局粒子的有效视距
//		float sight_distance[4] = { 
//			pContext->GetCamera().mtxViewInverse._13,
//			pContext->GetCamera().mtxViewInverse._23,
//			pContext->GetCamera().mtxViewInverse._33,
//			m_fRadius };
//
//		pBatch->SetVSConst(CV_GLOBAL, sight_distance, 1);
//	}
//
//	// UV偏移
//	float uv_bias[4] = { float(1.0F / pdata->m_nParticleFrame), 0.0F, 0.0F, 
//		0.0F };
//
//	pBatch->SetVSConst(CV_UV_BIAS, (float*)uv_bias, 1);
//
//	// 动画数据
//	pBatch->SetVSConst(CV_TRACK_ALPHA, 
//		(float*)pdata->m_vVisibilityTrack, 128);
	return true;
}
*/

/*
bool CParticleNode::DrawDistortionMaterial_CPU(int num, IVertexShader* pVS, 
	IPixelShader* pPS)
{
	IDxContext* pContext = m_pRender->GetContext();

	if (!pContext->GetEnableRealizeRefraction())
	{
		return false;
	}

	CParticleData* pdata = m_pData;
	IDxTexture* pTex = m_pTexture;
	
	//if (!pdata->m_strRefractionTex.empty())
	if (m_pRefractionTex)
	{
		// 使用另外的折射贴图
		pTex = m_pRefractionTex;
	}

	DxSceneBatch* pBatch = m_pRender->AddRefractionBatch("Ref_ParticleNode");

	if (pTex)
	{
		pBatch->SetPSTexture(0, pTex->GetShaderTex());
	}
	else
	{
		pBatch->SetPSTexture(0, NULL);
	}

	pBatch->SetVertexShader(pVS);
	pBatch->SetPixelShader(pPS);
	pBatch->SetInputLayout(m_pParticleManager->GetParticleLayout());
	pBatch->SetVertexBuffer(0, m_pVBuffer, sizeof(particle_vertex_cpu_t), 0);
	pBatch->SetIndexBuffer(m_pIBuffer, 0);
	pBatch->SetVSConstBuffer(0, m_pCB);
	pBatch->SetPSConstBuffer(0, m_pCB);
	//pBatch->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//pBatch->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	//pBatch->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//pBatch->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pBatch->SetRasterState(m_pParticleManager->GetRasterState());
	pBatch->SetBlendState(m_pParticleManager->GetNormalBlendState());
	pBatch->SetPrimitive(DX_PRIMITIVE_TRIANGLELIST, num * 4, num * 6);

//	// 设置WVP
//	const dx_camera_t& camera = pContext->GetCamera();
//	const FmMat4& mtxViewProj = camera.mtxViewProj;
//	const FmMat4& mtxProj = camera.mtxProj;
//	FmVec3 vViewPos = camera.vPosition;
//	FmMat4 mtxTemp;
//
//	FmMat4Transpose(&mtxTemp, &mtxProj);
//
//	pBatch->SetVSConst(CV_PROJECTION, (float*)&mtxTemp, 4);
//
//	float distorion_intensity[4] = { 1.0F, 1.0F, 1.0F, 
//		pdata->m_fDistortionIntensity };
//
//	pBatch->SetVSConst(CV_COLOR, distorion_intensity, 1);
//
//	if (pdata->m_bLocalSpaceEmitter && pdata->m_bLocalAngleEmitter)
//	{
//		FmMat4 mtxWVP;
//
//		FmMat4Multiply(&mtxWVP, &m_mtxLink, &mtxViewProj);
//		FmMat4Transpose(&mtxWVP, &mtxWVP);
//
//		pBatch->SetVSConst(CV_VIEWPROJ, (float*)&mtxWVP, 4);
//
//		// 视点逆到本地空间
//		FmVec3 WorldX(m_mtxLink._11, m_mtxLink._12, m_mtxLink._13);
//		FmVec3 WorldY(m_mtxLink._21, m_mtxLink._22, m_mtxLink._23);
//		FmVec3 WorldZ(m_mtxLink._31, m_mtxLink._32, m_mtxLink._33);
//		FmVec3 WorldPos(m_mtxLink._41, m_mtxLink._42, m_mtxLink._43);
//
//		vViewPos -= WorldPos;
//		vViewPos = FmVec3(FmVec3Dot(&vViewPos, &WorldX),
//			FmVec3Dot(&vViewPos, &WorldY), FmVec3Dot(&vViewPos, &WorldZ));
//
//		pBatch->SetVSConst(CV_EYE_POS, vViewPos, 1);
//
//		// 粒子旋转轴
//		pBatch->SetVSConst(CV_ROTATE_AXIS,
//			FmVec4(0.0F, 1.0F, 0.0F, 0.0F) , 1);
//	}
//	else
//	{
//		FmMat4 mtxWVP;
//
//		FmMat4Transpose(&mtxWVP, &mtxViewProj);
//
//		pBatch->SetVSConst(CV_VIEWPROJ, (float*)&mtxWVP, 4);
//		pBatch->SetVSConst(CV_EYE_POS, vViewPos, 1);
//
//		// 粒子旋转轴
//		if (pdata->m_bAngleLinkEmitter)
//		{
//			pBatch->SetVSConst(CV_ROTATE_AXIS, 
//				FmVec4(m_vEmitterDir, 0.0F), 1);
//		}
//		else
//		{
//			pBatch->SetVSConst(CV_ROTATE_AXIS,
//				FmVec4(0.0F, 1.0F, 0.0F, 0.0F) , 1);
//		}
//	}
	return true;
}
*/
