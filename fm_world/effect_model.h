//--------------------------------------------------------------------
// 文件名:		effect_model.h
// 内  容:		
// 说  明:		
// 创建日期:	2011年10月18日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _EFFECT_MODEL_H
#define _EFFECT_MODEL_H

#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "../visual/i_vis_base.h"
#include "../visual/i_render.h"
#include "../visual/i_effect_model.h"

// 特效模型

class Model;
class ParticleManager;
class SaberArcManager;
class LightManager;
class CEffectData;

class EffectModel: public IEffectModel
{
	IRender* m_pRender;
private:
	// 加载状态
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
		core_string helper_name; // 辅助点名
		core_string particle_name; // 粒子名
		PERSISTID particle_id;		// 粒子ID
		int open_emit;				// 打开粒子发射开关
		int close_emit;				// 关闭粒子发射开关
	};

	struct saberarc_info
	{
		core_string helper_name0;	// 辅助点1
		core_string helper_name1;	// 辅助点2
		core_string saberarc_name; // 刀光名
		PERSISTID saberarc_id;		// 刀光ID
		bool is_saber_start;
	};

	struct light_info_t
	{
		core_string helper_name; // 辅助点名
		core_string light_name; // 灯光名
		PERSISTID light_id;			// 灯光ID
		int open_emit;				// 打开灯光发射开关
		int close_emit;				// 关闭灯光发射开关
	};

public:
	EffectModel();
	virtual ~EffectModel();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// 设置场景上下文
	virtual void SetContext(IRenderContext* value);

	virtual void Execute(float seconds);
	virtual void Update(float seconds);
	virtual void Realize();
	virtual void RealizeShadowMap();

	virtual	void SetWorldMatrix(const FmMat4& mat);
	virtual void GetWorldMatrix(FmMat4& mat);

	// 位置
	virtual bool SetPosition(float x, float y, float z);
	virtual FmVec3 GetPosition() const;

	// 方向
	virtual bool SetAngle(float x, float y, float z);
	virtual FmVec3 GetAngle() const;

	// 缩放比例
	virtual bool SetScale(float x, float y, float z);
	virtual FmVec3 GetScale() const;

    // 目标位置
    virtual bool SetTarget(float x, float y, float z);
    virtual FmVec3 GetTarget() const;

	// 全局中心点
	virtual FmVec3 GetCenter();
	// 包围盒尺寸
	virtual FmVec3 GetBoxSize();
	// 半径
	virtual float GetRadius();

	// 颜色
	void SetColorString(const char* value);
	result_string GetColorString() const;
	virtual void SetColor(unsigned int value);
	virtual unsigned int GetColor() const;

	// 是否可见
	virtual void SetVisible(bool value);
	virtual bool GetVisible() const;

	// 是否在水下
	virtual void SetUnderWater(bool value);
	virtual bool GetUnderWater() const;

	// 加载数据是否成功
	virtual bool GetLoaded();
	// 是否加载完成
	virtual bool GetLoadFinish();

	// 记录临时的透明度变量（只供地形系统使用）
	virtual void SetAlphaValue(float value) { m_fAlphaValue = value; }
	virtual float GetAlphaValue() const { return m_fAlphaValue; }

	//是否显示绑定在模型上的粒子
	void SetShowParticle(bool value);
	bool GetShowParticle();

	// 计算包围盒尺寸时是否计算粒子的
	void SetCalParticleSize(bool value);
	bool GetCalParticleSize();

	// 生命周期
	void SetLifeTime(float value);
	float GetLifeTime();

	// 是否重复播放
	void SetLoop(bool bLoop);
	bool GetLoop();

	// 是否一次性播放特效
	void SetIsOneOff(bool bValue);
	bool GetIsOneOff();

	// 播放速度
	void SetSpeed(float fSpeed);
	float GetSpeed();

	// 是否检测碰撞
	virtual void SetTraceEnable(bool value);
	virtual bool GetTraceEnable();

	// 是否可以剪裁
	virtual void SetCullEnable(bool value);
	virtual bool GetCullEnable();

	// 是否碰撞时使用辅助盒
	void SetTraceBoxEnable(bool value);
	bool GetTraceBoxEnable();

	// 是否需要水面反射
	virtual void SetWaterReflect(bool value) { m_bWaterReflect = value; }
	virtual bool GetWaterReflect() const { return m_bWaterReflect; }

	// 是否异步加载
	void SetAsyncLoad(bool value);
	bool GetAsyncLoad() const;

	// 是否显示包围盒
	void SetShowBoundBox(bool value);
	bool GetShowBoundBox() const;

	// 设置模型文件
	void SetModelFile(const char* model_file);
	const char* GetModelFile() const;

	// 设置模型材质
	void SetMaterialFile(const char* material_file);
	const char* GetMaterialFile() const;

	// 设置粒子文件
	void SetParticleFile(const char* file_name);
	const char* GetParticleFile() const;

	// 设置拖尾文件
	void SetSaberArcFile(const char* file_name);
	const char* GetSaberArcFile() const;

	// 设置光源文件
	void SetLightFile(const char* file_name);
	const char* GetLightFile() const;

	// 取得当前时间
	float GetCurrenTime();

	// 模型对象
	PERSISTID GetModelID() const;
	void SetModelID(const PERSISTID& id);

	// 特效模型配置文件名
	const char* GetConfigFile() const;
	// 特效模型名字
	const char* GetName() const;
    void SetName(const char* name);

	// 从当前设置内容创建特效
	bool CreateFromSetting();

	// 通过配置文件加载
	bool CreateFromIni(const char* ini_name, const char* em_name, 
		bool async_load);
	bool CreateFromIniEx(const char* ini_name, const char* em_name, 
		bool async_load, const char* append_path);

	// 取得辅助点列表
	void GetHelperNameList(const IVarList& args, IVarList& result);
	// 取得节点矩阵
	void GetCurrentNodeMatrix(int nNodeID, FmMat4* pTM);
	bool GetCurrentNodeMatrix(const char* pNodeName, FmMat4* pTM);
	// 取得节点名
	int GetNodeFromName(const char* pszName);

	// 碰撞测试详细信息
	virtual bool TraceDetail(const FmVec3& src, const FmVec3& dst,
		trace_info_t& result);
	// 碰撞测试是否有阻挡
	virtual bool TraceHitted(const FmVec3& src, const FmVec3& dst);
	// 球体范围碰撞
    virtual bool TraceSphere(const FmVec3& center, float radius);
    // 三角形碰撞
    virtual bool TraceTriangle(const FmVec3& v0, const FmVec3& v1, const FmVec3& v2);
	// 剪选
	virtual bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context);

	// 添加粒子（延迟创建粒子）
	bool AddParticleParams(const char* particle_name, 
		const char* helper_name, int emit_open, int emit_close);
	// 添加粒子（直接创建粒子）
	bool AddParticle(const char* particle_name, const char* helper_name,
		int emit_open, int emit_close);
	// 根据索引删除粒子
	bool DeleteParticleFromIndex(int index);
	// 删除粒子（指定辅助点上的所有粒子）
	bool DeleteParticleOnHelper(const char* helper_name);
	// 根据名字删除粒子
	bool DeleteParticleFromName(const char* helper_name, const char* particle_name);
	// 根据名字取得辅助点上的粒子
	PERSISTID GetParticleFromName(const char* helper_name, 
		const char* particle_name);
	// 根据索引取得辅助点上的粒子
	PERSISTID GetParticleFromIndex(int index) const;
	// 根据索引开启粒子（重建关闭的粒子）
	bool StartParticleFromIndex(size_t index);
	// 根据索引关闭粒子（删除粒子但保留信息）
	bool StopParticleFromIndex(size_t index);
	// 取得粒子数量
	int GetParticleCount();
	// 获得粒子信息
	void GetParticleInfoFromIndex(const IVarList& args, IVarList& result);
	// 设置粒子开关信息
	bool SetParticleEmit(const char* helper_name, int open_emit, 
		int close_emit);
	// 取得粒子开关信息
	void GetParticleEmit(const IVarList& args, IVarList& result);

	// 添加刀光参数
	bool AddSaberArcParams(const char* saberarc_name, 
		const char* helper_name0, const char* helper_name1, bool is_start);
	// 添加刀光信息
	bool AddSaberArc(PERSISTID& id);
	// 删除刀光
	bool DeleteSaberArc(int index);
	// 获取刀光
	PERSISTID GetSaberArc(int index) const;
	// 获取刀光数量
	int GetSaberArcCount();
	// 获得刀光信息
	void GetSaberArcInfo(const IVarList& args, IVarList& result);

	// 添加光源参数（延迟创建光源）
	bool AddLightParams(const char* light_name, const char* helper_name, 
		int emit_open, int emit_close);
	// 添加光源参数（直接创建光源）
	bool AddLight(const char* light_name, const char* helper_name, 
		int emit_open, int emit_close);
	// 根据索引删除光源
	bool DeleteLightFromIndex(int index);
	// 删除光源（指定辅助点上的所有光源）
	bool DeleteLightOnHelper(const char* helper_name);
	// 根据名字删除光源
	bool DeleteLightFromName(const char* helper_name, const char* light_name);
	// 根据名字取得辅助点上的光源
	PERSISTID GetLightFromName(const char* helper_name, const char* light_name);
	// 根据索引取得辅助点上的光源
	PERSISTID GetLightFromIndex(int index) const;
	// 获取光源数量
	int GetLightCount();
	// 获得光源信息
	void GetLightInfo(const IVarList& args, IVarList& result);
	// 设置光源开关信息
	bool SetLightEmit(const char* helper_name, int open_emit, 
		int close_emit);
	// 取得光源开关信息
	void GetLightEmit(const IVarList& args, IVarList& result);

	// 保存特效模型到文件
	bool SaveEffectModel(const char* file_name, const char* effect_name);

	// 设置残影
	bool SetBrokenShadow(bool value);
	PERSISTID GetBrokenShadow() const;

	// 清理所有子特效和特效数据,使对像保持刚创建时的状态
	bool Clear();

	// 获得内存占用
	virtual size_t GetMemoryUsage();

    void SetSleep(bool value);

    bool GetSleep() const
    {
        return m_bSleep;
    }

private:
	bool CheckLoad(float seconds);
	// 卸载资源
	bool Unload();

	// 加载数据
	bool LoadData(const char* ini_name, const char* em_name, bool async_load, 
		const char* append_path);
	// 从当前设置内容创建粒子
	bool CreateParticleFromSetting();
	// 从当前设置内容创建刀光
	bool CreateSaberArcFromSetting();
	// 从当前设置内容创建光源
	bool CreateLightFromSetting();
	// 在辅助点上创建粒子
	PERSISTID CreateParticleOnHelper(const char* particle_name, 
		const char* helper_name);
	PERSISTID CreateLightOnHelper(const char* light_name, const char* helper_name);
	// 延迟创建特效
	bool CreateEffect();

	// 更新粒子连接位置
	void UpdateLinkParticleEmit();
	// 更新光源连接位置
	void UpdateLinkLightEmit();

	// 更新包围盒
	void UpdateBoundingBox();

private:
	PERSISTID m_ParticleManagerID;
	PERSISTID m_LightManagerID;
	PERSISTID m_SaberArcManagerID;
	Model* m_pModel;				// 模型对象指针
	FmMat4 m_mtxWorld;
	TArrayPod<particle_info*, 10, TCoreAlloc> m_ParticleInfo;	// 粒子容器
	TArrayPod<saberarc_info*, 2, TCoreAlloc> m_SaberArcInfo;	// 运动托尾
	TArrayPod<light_info_t*, 1, TCoreAlloc> m_LightInfo;		// 灯光

	core_string m_strAppendPath;	// 附加路径
	core_string m_strModelFile;		// 特效模型文件
	core_string m_strMaterialFile;	// 特效模型材质
	core_string m_strParticleFile;	// 粒子文件名
	core_string m_strSaberArcFile;	// 刀光文件名
	core_string m_strLightFile;		// 光源文件名
	core_string m_strConfigFile;	// 配置文件名
	core_string m_strEffectName;	// 配置段名

	int m_nLoadState;
	bool m_bClipPlaneEnable;
	bool m_bNeedCreate;				// 因为异步加载而需要延迟创建粒子或拖尾
	bool m_bModelChanged;			// 模型资源是否发生变化
	bool m_bVisible;				// 特效模型是否可见
	bool m_bShowParticle;			// 是否显示绑定在模型上的粒子
	bool m_bCalParticleSize;		// 计算包围盒尺寸时是否计算粒子的
	bool m_bUpdateNodeMat;			// 绑定点矩阵是否被更新过
	bool m_bLoop;					// 是否重复播放
	bool m_bIsOneOff;				// 是否一次性播放特效
	bool m_bTraceEnable;			// 是否进行碰撞测试
	bool m_bCullEnable;				// 是否进行剪裁
	bool m_bTraceBoxEnable;			// 是否碰撞时使用辅助盒
	bool m_bWaterReflect;			// 是否需要水面反射
	bool m_bAsyncLoad;				// 是否异步加载
	bool m_bShowBoundBox;
	bool m_bUpdatedBoundingBox;		// 是否已经更新过包围盒
    bool m_bSleep;
	float m_fAlphaValue;
	float m_fCurrentTime;			// 特效模型当前播放时间
	float m_fLifeTime;				// 生命周期
	float m_fSpeed;					// 播放速度
	unsigned int m_nColor;
	PERSISTID m_BrokenShadow;		// 残影对象

    FmVec3 m_vTarget;

	FmVec3 m_vBoundMin;
	FmVec3 m_vBoundMax;
	FmVec3 m_vCenter;
	float m_fRadius;
};

#endif // _EFFECT_MODEL_H
