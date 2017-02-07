//--------------------------------------------------------------------
// �ļ���:		particle_node.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��9��26��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _PARTICLE_NODE_H
#define _PARTICLE_NODE_H

#include "../public/macros.h"


#include "../public/i_interface.h"
#include "../visual/i_render.h"
#include "../visual/i_resource.h"
#include "particle_data.h"
#include "particle_manager.h"
// ���ӽڵ�

class CParticleData;


class CParticleNode
{
public:
	static CParticleNode* NewInstance(ParticleManager* pParticleManager,
		IRender* pRender);

private:

	// CPU�������ԣ�
	struct particle_vertex_cpu_t
	{
		particle_vertex_cpu_t()
		{
			pos = FmVec4(0,0,0,0);
            color = FmVec4(0,0,0,0);
			tu = 0.0;
			tv = 0.0;
		}

		particle_vertex_cpu_t(FmVec4 _pos, FmVec4 _color, float _tu, 
			float _tv): pos(_pos), color(_color), tu(_tu), tv(_tv)
		{}

		FmVec4 pos;
		FmVec4 color;
		float tu;
		float tv;
	};

	// GPU�������ԣ�
	struct particle_vertex_gpu_t
	{
		particle_vertex_gpu_t()
		{
			pos = FmVec4(0.0f,0.0f,0.0f,0.0f);
			dir = FmVec3(0.0f,0.0f,0.0f);
			tu=tv=0.0f;
			posdelta=random0=random1=random2=FmVec2(0.0f,0.0f);
		}

		particle_vertex_gpu_t(FmVec4 _pos, FmVec3 _dir, 
			FmVec2 _posdelta, FmVec2 _rand0, FmVec2 _rand1, 
			FmVec2 _rand2, float _tu, float _tv):
			pos(_pos), dir(_dir), posdelta(_posdelta), random0(_rand0), 
			random1(_rand1), random2(_rand2), tu(_tu), tv(_tv)
			{}

		FmVec4 pos;
		FmVec3 dir;
		float tu;
		float tv;
		FmVec2 posdelta;
		FmVec2 random0;
		FmVec2 random1;
		FmVec2 random2;
	};

	// ���ӻ������ԣ�
	struct particle_t
	{
		FmVec3 emitpos;			// ����㣻
		FmVec3 pos;				// λ�ã�
		FmVec3 dir;				// ����
		FmVec3 chaos;			// ���ң�
		float speed;			// �ٶȣ�
		float size;				// ��С��
		float weight;			// ������
		float spin;				// ������
		float angle;			// �Ƕȣ�
		float time;				// ��δ֪ʱ�䣩��
		unsigned int frame;		// ���������ţ�
		unsigned int color;		// ��ɫ��
	};

	// ���Ӽ���ģʽ
	enum
	{
		STATE_PARTICLE_AUTO = 0,
		STATE_PARTICLE_CPU,
		STATE_PARTICLE_GPU,
		STATE_PARTICLE_GLOBAL
	};

	// ���ӹ����ģʽ
	enum
	{
		STATE_BILLBOARD_DEFAULT = 0,
		STATE_BILLBOARD_Y,
		STATE_BILLBOARD_NONE
	};

	// ���ӻ��ģʽ
	enum
	{
		STATE_BLEND_DEFAULT = 0,
		STATE_BLEND_ADD,
		STATE_BLEND_MULTIPLY
	};

public:
	CParticleNode(ParticleManager* pManager, IRender* pRender);
	~CParticleNode();

	// �ͷ�
	void Release();
	// ����
	bool Create(CParticleData* pData, bool async_load);
	// ����
	void Update(float seconds);
	// ��Ⱦ
	void Realize(float seconds);

	// �����Ƿ����
	bool GetLoadFinish();

	// ǰһ���ڵ�
	void SetPrevNode(CParticleNode* node) { m_pPrev = node; }
	CParticleNode* GetPrevNode() const { return m_pPrev; }

	// ��һ���ڵ�
	void SetNextNode(CParticleNode* node) { m_pNext = node; }
	CParticleNode* GetNextNode() const { return m_pNext; }

	// ���ĵ�λ��
	const FmVec3& GetCenter() const { return m_vCenter; }
	// ��Χ�гߴ�
	FmVec3 GetBoxSize() const;
	// �뾶
	float GetRadius() const;

	// ��������
	const char* GetName() const;
	// ���������ļ�
	const char* GetConfig() const;

	// ����λ��
	void SetEmitterPos(const FmVec3& pos);
	const FmVec3& GetEmitterPos() const { return m_vLastEmitterPos; }
	// ���䷽��
	void SetEmitterDir(const FmVec3& dir);
	const FmVec3& GetEmitterDir() const { return m_vEmitterDir; }
	// ����Ƕ�
	void SetEmitterAngle(const FmVec3& angle);
	const FmVec3& GetEmitterAngle() const { return m_vEmitterAngle; }

	// ����
	void SetScale(const FmVec3& scale);
	const FmVec3& GetScale() const;
	
	// ���÷���
	void SetWindPower(float value) { m_fWindPower = value; }
	// ���÷���
	void SetWindDirection(const FmVec3& value) { m_vWindDirection = value; }
	
	// �Ƿ����
	bool GetComplete() { return m_bComplete; }
	// ֹͣ
	void Stop() { m_bComplete = true; };
	// ��ͣ
	void Pause() { m_bPause = true; }
	// �ָ�
	void Resume() { m_bPause = false; }

	// ���Ӷ����
	void SetParticleID(const PERSISTID& value) { m_ParticleID = value; }
	PERSISTID GetParticleID() const { return m_ParticleID; }

	// ���巢����ʱ
	void SetSystemDelay(float value);
	float GetSystemDelay() const { return FM_MAX(-(float)m_dTimeCounter, 0.0F); }

	// ���ӷ��俪��
	void SetEmitSwitch(bool value);
	bool GetEmitSwitch()const { return m_bEmitSwitch; }
	
	// ��������ʱ��
	void SetSystemLifeTime(float value) { m_fSystemLifeTime = value; }
	float GetSystemLifeTime() const { return m_fSystemLifeTime; }

	// �ٶȵ�������
	void SetSpeedFactor(float value) { m_fSpeedFactor = value; }
	float GetSpeedFactor() const { return m_fSpeedFactor; }

	// �Ƿ�̬���°�Χ��
	void SetUpdateBoundBox(bool value) { m_bUpdateBoundBox = value; }
	bool GetUpdateBoundBox() const { return m_bUpdateBoundBox; }

	// �Ƿ�ɼ�
	void SetVisible(bool value) { m_bVisible = value; }
	bool GetVisible() const { return m_bVisible; }

	// �Ƿ���ˮ��
	void SetUnderWater(bool value) { m_bUnderWater = value; }
	bool GetUnderWater() const { return m_bUnderWater; }

	// �ü����Ƿ���Ч
	void SetClipPlaneEnable(bool value) { m_bClipPlaneEnable = value; }
	bool GetClipPlaneEnable() const { return m_bClipPlaneEnable; }

	// �ü���
	void SetClipPlane(const FmVec4& value) { m_vClipPlane = value; }

	// LOD��������
	void SetLodEnd(float value) { m_fLodEnd = value; }
	float GetLodEnd() const { return m_fLodEnd; }

	// LOD����
	void SetLodValue(float value) { m_fLodValue = value; }
	float GetLodValue() const { return m_fLodValue; }

	// �Ƿ��ظ�����
	void SetRepeat(bool value);
	bool GetRepeat() const { return m_bRepeat; }

	// ��ʾ״̬
	void SetShowState(bool value) { m_bShowState = value; }
	bool GetShowState() const { return m_bShowState; }

	// �Ƿ�ˮ�沨��
	bool GetWaveParticle() const { return m_bWaveParticle; }

	// ͸����
	void SetAlpha(unsigned int value);
	unsigned int GetAlpha() const;

	// ����ɫ
	void SetAppendColor(unsigned int value);
	unsigned int GetAppendColor() const;

	// ���ӵ���������
	bool LinkToObject(const PERSISTID& id, const FmVec3& link_pos);
	// ���ӵ���������ĸ�������
	bool LinkToObjectPoint(const PERSISTID& id, const char* helper_name);

	// ��õ�ǰ���Ӷ���
	PERSISTID GetBindID() const { return m_LinkObjectID; }

private:
	// ����CPU���㻺��
	void UpdateCPUVertexBuffer(float percent);
	// ����CPU��β���Ӷ��㻺��
	void UpdateFlashTailVertexBuffer(float percent);

	particle_vertex_cpu_t* LockDynamicVB();

	void UnlockDynamicVB();

	// ����GPU���㻺��
	int UpdateGPUVertexBuffer();
	// ����GPU���Ӱ�Χ��
	void UpdateGPUBoundBox();
	// ���µ�����������
	void UpdateParticlePartData(particle_t* p);
	// ������������
	void UpdateParticleData();
	// �ı����Ӽ���ģʽ
	void UpdateComputeType();
	// ɾ������(����ֵΪtrue��ʾ����ɾ����falseΪ�ӳ�ɾ��)
	bool Delete();

	// �������ӵı仯
	void ProcessParticles(float delta_time);

	// ���ó�������
	void FillConstBuffer();

	// ����CPU���ӣ�
	bool DrawParticle_CPU(int num,ParticleManager::particle_shader_handle* pShaderHandle,
		int blend_type, float camera_z, bool depth_map);

   	// CPU���δ���
	static void RenderParticle_CPU(void* pdata);

	// ����GPU���ӣ�
	bool DrawParticle_GPU(int num,ParticleManager::particle_shader_handle* pShaderHandle,
		int blend_type, float camera_z, bool depth_map);

	// GPU���δ���
	static void RenderParticle_GPU(void* pdata);

	// �軭Ť������(CPU)
	//bool DrawDistortionMaterial_CPU(int num, IVertexShader* pVS, IPixelShader* pPS);

	// �軭Ť������(GPU)
	//bool DrawDistortionMaterial_GPU(IVertexShader* pVS, IPixelShader* pPS);

	// ������Ⱦ����
	//static void RenderTest(void* pdata);
private:

	ParticleManager* m_pParticleManager;
	IRender* m_pRender;
	CParticleNode* m_pPrev;
	CParticleNode* m_pNext;
	CParticleData* m_pData;
	IDynamicVB* m_pVBuffer;
	IStaticIB* m_pIBuffer;

	/*
    IStaticVB*    m_pTestVB;
	IStaticIB*    m_pTestIB;
	*/
	ParticleManager::particle_shader_handle* m_pShaderHandle;

	particle_vertex_cpu_t* m_pVBBufferDataCPU;
	particle_vertex_gpu_t* m_pVBBufferDataGPU;

	unsigned int m_VBNum;
	int m_blend_type;
	int m_num;

	PERSISTID m_ParticleID;
	ITexture* m_pTexture;
	ITexture* m_pRandomColorTex;
	ITexture* m_pRefractionTex;
	particle_t* m_Particle;
	int m_nParticleNum;			// ��������
	int m_nDrawParticleNum;		// ʵ��Ҫ����������
	int m_nRandSeed;			// ���Ӽ��������
	unsigned int m_nParticleColor;		// ������ɫ
	unsigned int m_nParticlAppendColor;	// ���Ӹ�����ɫ
	int m_nTailCount;			// ��β����
	float m_fParticleLife;		// ������������
	bool m_bRepeat;				// �Ƿ��ظ�

	FmMat4 m_cVIEWPROJ;
	FmMat4 m_cPROJECTION;
	FmVec3 m_cEYE_POS;
	FmVec4 m_cCOLOR;//������ɫ����
	FmVec4 m_cROTATE_AXIS;
	FmVec4 m_cFOG_COLOR;
	FmVec4 m_cFOG_PARAM;
	FmVec4 m_cFOG_EXP_PARAM;

	bool m_bEnableFogLinear;
	bool m_bEnableFogExp;
	
	FmVec3 m_vLastEmitterPos;
	FmVec3 m_vEmitterPos;
	FmVec3 m_vEmitterDir;
	FmVec3 m_vEmitterAngle;
	FmVec3 m_vScale;
	FmVec3 m_vWindDirection;
	FmMat4 m_mtxLink;
	float m_fWindPower;
	FmVec3 m_vBoxMin;
	FmVec3 m_vBoxMax;
	FmVec3 m_vCenter;
	float m_fRadius;
	
	float m_fSystemLifeTime;	// ��������ʱ��
	float m_fSpeedFactor;		// �ٶȵ�������
	float m_fLodEnd;			// LOD��������
	float m_fLodValue;			// ��ǰLOD����
	float m_fLastRefeshTime;	// ��һ�θ��¾���ʱ��
	float m_fTimeAfterRelocate;
	bool m_bUpdateBoundBox;
	bool m_bUpdateScale;		// ������������
	bool m_bVisible;
	bool m_bUnderWater;
	bool m_bEmitSwitch;			// ���ӷ��俪��
	bool m_bClipPlaneEnable;	// �ü��濪��

	bool m_bSoftParticle;		// �Ƿ��������ӣ�������
	bool m_bWaveParticle;		// �Ƿ���ˮ�沨������
	int m_nComputeType;			// ���Ӽ���ģʽ
	int m_nBlendType;			// ���ӻ��ģʽ
	int m_nBillboardType;		// ���ӹ����ģʽ
	int m_nParticleEndTime;		// ���ӽ���ʱ��
	double m_dTimeCounter;
	double m_dTimeRealized;

	bool m_bPause;
	bool m_bShowState;
	bool m_bComplete;
	bool m_bLinkEmitter;
	PERSISTID m_LinkObjectID;
	FmVec3 m_vLinkPos;
	FmVec3 m_vLinkDirPos;
	FmVec4 m_vClipPlane;
	int m_nHelperID;
};

#endif // _PARTICLE_NODE_H
