//--------------------------------------------------------------------
// 文件名:		particle_node.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年9月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PARTICLE_NODE_H
#define _PARTICLE_NODE_H

#include "../public/macros.h"


#include "../public/i_interface.h"
#include "../visual/i_render.h"
#include "../visual/i_resource.h"
#include "particle_data.h"
#include "particle_manager.h"
// 粒子节点

class CParticleData;


class CParticleNode
{
public:
	static CParticleNode* NewInstance(ParticleManager* pParticleManager,
		IRender* pRender);

private:

	// CPU粒子属性：
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

	// GPU粒子属性：
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

	// 粒子基本属性：
	struct particle_t
	{
		FmVec3 emitpos;			// 发射点；
		FmVec3 pos;				// 位置；
		FmVec3 dir;				// 朝向；
		FmVec3 chaos;			// 混乱；
		float speed;			// 速度；
		float size;				// 大小；
		float weight;			// 重量；
		float spin;				// 自旋；
		float angle;			// 角度；
		float time;				// （未知时间）；
		unsigned int frame;		// 材质随机块号；
		unsigned int color;		// 颜色；
	};

	// 粒子计算模式
	enum
	{
		STATE_PARTICLE_AUTO = 0,
		STATE_PARTICLE_CPU,
		STATE_PARTICLE_GPU,
		STATE_PARTICLE_GLOBAL
	};

	// 粒子公告板模式
	enum
	{
		STATE_BILLBOARD_DEFAULT = 0,
		STATE_BILLBOARD_Y,
		STATE_BILLBOARD_NONE
	};

	// 粒子混合模式
	enum
	{
		STATE_BLEND_DEFAULT = 0,
		STATE_BLEND_ADD,
		STATE_BLEND_MULTIPLY
	};

public:
	CParticleNode(ParticleManager* pManager, IRender* pRender);
	~CParticleNode();

	// 释放
	void Release();
	// 创建
	bool Create(CParticleData* pData, bool async_load);
	// 更新
	void Update(float seconds);
	// 渲染
	void Realize(float seconds);

	// 加载是否完成
	bool GetLoadFinish();

	// 前一个节点
	void SetPrevNode(CParticleNode* node) { m_pPrev = node; }
	CParticleNode* GetPrevNode() const { return m_pPrev; }

	// 后一个节点
	void SetNextNode(CParticleNode* node) { m_pNext = node; }
	CParticleNode* GetNextNode() const { return m_pNext; }

	// 中心点位置
	const FmVec3& GetCenter() const { return m_vCenter; }
	// 包围盒尺寸
	FmVec3 GetBoxSize() const;
	// 半径
	float GetRadius() const;

	// 粒子名字
	const char* GetName() const;
	// 粒子配置文件
	const char* GetConfig() const;

	// 发射位置
	void SetEmitterPos(const FmVec3& pos);
	const FmVec3& GetEmitterPos() const { return m_vLastEmitterPos; }
	// 发射方向
	void SetEmitterDir(const FmVec3& dir);
	const FmVec3& GetEmitterDir() const { return m_vEmitterDir; }
	// 发射角度
	void SetEmitterAngle(const FmVec3& angle);
	const FmVec3& GetEmitterAngle() const { return m_vEmitterAngle; }

	// 缩放
	void SetScale(const FmVec3& scale);
	const FmVec3& GetScale() const;
	
	// 设置风力
	void SetWindPower(float value) { m_fWindPower = value; }
	// 设置风向
	void SetWindDirection(const FmVec3& value) { m_vWindDirection = value; }
	
	// 是否完成
	bool GetComplete() { return m_bComplete; }
	// 停止
	void Stop() { m_bComplete = true; };
	// 暂停
	void Pause() { m_bPause = true; }
	// 恢复
	void Resume() { m_bPause = false; }

	// 粒子对象号
	void SetParticleID(const PERSISTID& value) { m_ParticleID = value; }
	PERSISTID GetParticleID() const { return m_ParticleID; }

	// 整体发射延时
	void SetSystemDelay(float value);
	float GetSystemDelay() const { return FM_MAX(-(float)m_dTimeCounter, 0.0F); }

	// 粒子发射开关
	void SetEmitSwitch(bool value);
	bool GetEmitSwitch()const { return m_bEmitSwitch; }
	
	// 整体生存时间
	void SetSystemLifeTime(float value) { m_fSystemLifeTime = value; }
	float GetSystemLifeTime() const { return m_fSystemLifeTime; }

	// 速度调节因子
	void SetSpeedFactor(float value) { m_fSpeedFactor = value; }
	float GetSpeedFactor() const { return m_fSpeedFactor; }

	// 是否动态更新包围盒
	void SetUpdateBoundBox(bool value) { m_bUpdateBoundBox = value; }
	bool GetUpdateBoundBox() const { return m_bUpdateBoundBox; }

	// 是否可见
	void SetVisible(bool value) { m_bVisible = value; }
	bool GetVisible() const { return m_bVisible; }

	// 是否在水下
	void SetUnderWater(bool value) { m_bUnderWater = value; }
	bool GetUnderWater() const { return m_bUnderWater; }

	// 裁剪面是否有效
	void SetClipPlaneEnable(bool value) { m_bClipPlaneEnable = value; }
	bool GetClipPlaneEnable() const { return m_bClipPlaneEnable; }

	// 裁剪面
	void SetClipPlane(const FmVec4& value) { m_vClipPlane = value; }

	// LOD结束距离
	void SetLodEnd(float value) { m_fLodEnd = value; }
	float GetLodEnd() const { return m_fLodEnd; }

	// LOD因子
	void SetLodValue(float value) { m_fLodValue = value; }
	float GetLodValue() const { return m_fLodValue; }

	// 是否重复发射
	void SetRepeat(bool value);
	bool GetRepeat() const { return m_bRepeat; }

	// 显示状态
	void SetShowState(bool value) { m_bShowState = value; }
	bool GetShowState() const { return m_bShowState; }

	// 是否水面波纹
	bool GetWaveParticle() const { return m_bWaveParticle; }

	// 透明度
	void SetAlpha(unsigned int value);
	unsigned int GetAlpha() const;

	// 附加色
	void SetAppendColor(unsigned int value);
	unsigned int GetAppendColor() const;

	// 连接到其他对象
	bool LinkToObject(const PERSISTID& id, const FmVec3& link_pos);
	// 连接到其他对象的辅助点上
	bool LinkToObjectPoint(const PERSISTID& id, const char* helper_name);

	// 获得当前连接对象
	PERSISTID GetBindID() const { return m_LinkObjectID; }

private:
	// 更新CPU顶点缓冲
	void UpdateCPUVertexBuffer(float percent);
	// 更新CPU拖尾粒子顶点缓冲
	void UpdateFlashTailVertexBuffer(float percent);

	particle_vertex_cpu_t* LockDynamicVB();

	void UnlockDynamicVB();

	// 更新GPU顶点缓冲
	int UpdateGPUVertexBuffer();
	// 更新GPU粒子包围盒
	void UpdateGPUBoundBox();
	// 更新单个粒子数据
	void UpdateParticlePartData(particle_t* p);
	// 更新粒子数据
	void UpdateParticleData();
	// 改变粒子计算模式
	void UpdateComputeType();
	// 删除粒子(返回值为true表示立即删除，false为延迟删除)
	bool Delete();

	// 处理粒子的变化
	void ProcessParticles(float delta_time);

	// 设置常量缓冲
	void FillConstBuffer();

	// 绘制CPU粒子：
	bool DrawParticle_CPU(int num,ParticleManager::particle_shader_handle* pShaderHandle,
		int blend_type, float camera_z, bool depth_map);

   	// CPU批次处理：
	static void RenderParticle_CPU(void* pdata);

	// 绘制GPU粒子：
	bool DrawParticle_GPU(int num,ParticleManager::particle_shader_handle* pShaderHandle,
		int blend_type, float camera_z, bool depth_map);

	// GPU批次处理：
	static void RenderParticle_GPU(void* pdata);

	// 描画扭曲材质(CPU)
	//bool DrawDistortionMaterial_CPU(int num, IVertexShader* pVS, IPixelShader* pPS);

	// 描画扭曲材质(GPU)
	//bool DrawDistortionMaterial_GPU(IVertexShader* pVS, IPixelShader* pPS);

	// 测试渲染函数
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
	int m_nParticleNum;			// 粒子数量
	int m_nDrawParticleNum;		// 实际要画的粒子数
	int m_nRandSeed;			// 粒子计算随机数
	unsigned int m_nParticleColor;		// 粒子颜色
	unsigned int m_nParticlAppendColor;	// 粒子附加颜色
	int m_nTailCount;			// 拖尾段数
	float m_fParticleLife;		// 粒子生命周期
	bool m_bRepeat;				// 是否重复

	FmMat4 m_cVIEWPROJ;
	FmMat4 m_cPROJECTION;
	FmVec3 m_cEYE_POS;
	FmVec4 m_cCOLOR;//顶点颜色控制
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
	
	float m_fSystemLifeTime;	// 整体生存时间
	float m_fSpeedFactor;		// 速度调节因子
	float m_fLodEnd;			// LOD结束距离
	float m_fLodValue;			// 当前LOD因子
	float m_fLastRefeshTime;	// 上一次更新经过时间
	float m_fTimeAfterRelocate;
	bool m_bUpdateBoundBox;
	bool m_bUpdateScale;		// 更新粒子缩放
	bool m_bVisible;
	bool m_bUnderWater;
	bool m_bEmitSwitch;			// 粒子发射开关
	bool m_bClipPlaneEnable;	// 裁剪面开关

	bool m_bSoftParticle;		// 是否开启柔粒子（高亮）
	bool m_bWaveParticle;		// 是否开启水面波纹粒子
	int m_nComputeType;			// 粒子计算模式
	int m_nBlendType;			// 粒子混合模式
	int m_nBillboardType;		// 粒子公告板模式
	int m_nParticleEndTime;		// 粒子结束时间
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
