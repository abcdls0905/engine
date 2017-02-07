#ifndef _I_EFFECT_MODEL_H
#define _I_EFFECT_MODEL_H

#include "i_vis_base.h"

class IEffectModel: public IVisBase
{
public:
	virtual	void SetWorldMatrix(const FmMat4& mat) = 0;
	virtual void GetWorldMatrix(FmMat4& mat) = 0;

	// 位置
	virtual bool SetPosition(float x, float y, float z) = 0;
	virtual FmVec3 GetPosition() const = 0;

	// 方向
	virtual bool SetAngle(float x, float y, float z) = 0;
	virtual FmVec3 GetAngle() const = 0;

	// 缩放比例
	virtual bool SetScale(float x, float y, float z) = 0;
	virtual FmVec3 GetScale() const = 0;

	// 全局中心点
	virtual FmVec3 GetCenter() = 0;
	// 包围盒尺寸
	virtual FmVec3 GetBoxSize() = 0;
	// 半径
	virtual float GetRadius() = 0;

	// 颜色
	virtual void SetColorString(const char* value) = 0;
	virtual result_string GetColorString() const = 0;
	virtual void SetColor(unsigned int value) = 0;
	virtual unsigned int GetColor() const = 0;

	// 是否可见
	virtual void SetVisible(bool value) = 0;
	virtual bool GetVisible() const = 0;

	// 是否在水下
	virtual void SetUnderWater(bool value) = 0;
	virtual bool GetUnderWater() const = 0;

	// 加载数据是否成功
	virtual bool GetLoaded() = 0;
	// 是否加载完成
	virtual bool GetLoadFinish() = 0;

	// 记录临时的透明度变量（只供地形系统使用）
	virtual void SetAlphaValue(float value)  = 0;
	virtual float GetAlphaValue() const  = 0;

	//是否显示绑定在模型上的粒子
	virtual void SetShowParticle(bool value) = 0;
	virtual bool GetShowParticle() = 0;

	// 计算包围盒尺寸时是否计算粒子的
	virtual void SetCalParticleSize(bool value) = 0;
	virtual bool GetCalParticleSize() = 0;

	// 生命周期
	virtual void SetLifeTime(float value) = 0;
	virtual float GetLifeTime() = 0;

	// 是否重复播放
	virtual void SetLoop(bool bLoop) = 0;
	virtual bool GetLoop() = 0;

	// 是否一次性播放特效
	virtual void SetIsOneOff(bool bValue) = 0;
	virtual bool GetIsOneOff() = 0;

	// 播放速度
	virtual void SetSpeed(float fSpeed) = 0;
	virtual float GetSpeed() = 0;

	// 是否检测碰撞
	virtual void SetTraceEnable(bool value) = 0;
	virtual bool GetTraceEnable() = 0;

	// 是否可以剪裁
	virtual void SetCullEnable(bool value) = 0;
	virtual bool GetCullEnable() = 0;

	// 是否碰撞时使用辅助盒
	virtual void SetTraceBoxEnable(bool value) = 0;
	virtual bool GetTraceBoxEnable() = 0;

	// 是否需要水面反射
	virtual void SetWaterReflect(bool value) = 0;
	virtual bool GetWaterReflect() const = 0;

	// 是否异步加载
	virtual void SetAsyncLoad(bool value) = 0;
	virtual bool GetAsyncLoad() const = 0;

	// 是否显示包围盒
	virtual void SetShowBoundBox(bool value) = 0;
	virtual bool GetShowBoundBox() const = 0;

	// 设置模型文件
	virtual void SetModelFile(const char* model_file) = 0;
	virtual const char* GetModelFile() const = 0;

	// 设置模型材质
	virtual void SetMaterialFile(const char* material_file) = 0;
	virtual const char* GetMaterialFile() const = 0;

	// 设置粒子文件
	virtual void SetParticleFile(const char* file_name) = 0;
	virtual const char* GetParticleFile() const = 0;

	// 设置拖尾文件
	virtual void SetSaberArcFile(const char* file_name) = 0;
	virtual const char* GetSaberArcFile() const = 0;

	// 设置光源文件
	virtual void SetLightFile(const char* file_name) = 0;
	virtual const char* GetLightFile() const = 0;

	// 取得当前时间
	virtual float GetCurrenTime() = 0;

	// 模型对象
	virtual PERSISTID GetModelID() const = 0;
	virtual void SetModelID(const PERSISTID& id) = 0;

	// 特效模型配置文件名
	virtual const char* GetConfigFile() const = 0;
	// 特效模型名字
	virtual const char* GetName() const = 0;
    virtual void SetName(const char* name) = 0;

	// 从当前设置内容创建特效
	virtual bool CreateFromSetting() = 0;

	// 通过配置文件加载
	virtual bool CreateFromIni(const char* ini_name, const char* em_name, 
		bool async_load) = 0;
	virtual bool CreateFromIniEx(const char* ini_name, const char* em_name, 
		bool async_load, const char* append_path) = 0;

	// 取得辅助点列表
	virtual void GetHelperNameList(const IVarList& args, IVarList& result) = 0;
	// 取得节点矩阵
	virtual void GetCurrentNodeMatrix(int nNodeID, FmMat4* pTM) = 0;
	virtual bool GetCurrentNodeMatrix(const char* pNodeName, FmMat4* pTM) = 0;
	// 取得节点名
	virtual int GetNodeFromName(const char* pszName) = 0;

	// 碰撞测试详细信息
	virtual bool TraceDetail(const FmVec3& src, const FmVec3& dst,
		trace_info_t& result) = 0;
	// 碰撞测试是否有阻挡
	virtual bool TraceHitted(const FmVec3& src, const FmVec3& dst) = 0;
	// 球体范围碰撞
	virtual bool TraceSphere(const FmVec3& center, float radius) = 0;
	// 剪选
	virtual bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context) = 0;

	// 添加粒子（延迟创建粒子）
	virtual bool AddParticleParams(const char* particle_name, 
		const char* helper_name, int emit_open, int emit_close) = 0;
	// 添加粒子（直接创建粒子）
	virtual bool AddParticle(const char* particle_name, const char* helper_name,
		int emit_open, int emit_close) = 0;
	// 根据索引删除粒子
	virtual bool DeleteParticleFromIndex(int index) = 0;
	// 删除粒子（指定辅助点上的所有粒子）
	virtual bool DeleteParticleOnHelper(const char* helper_name) = 0;
	// 根据名字删除粒子
	virtual bool DeleteParticleFromName(const char* helper_name, const char* particle_name) = 0;
	// 根据名字取得辅助点上的粒子
	virtual PERSISTID GetParticleFromName(const char* helper_name, 
		const char* particle_name) = 0;
	// 根据索引取得辅助点上的粒子
	virtual PERSISTID GetParticleFromIndex(int index) const = 0;
	// 根据索引开启粒子（重建关闭的粒子）
	virtual bool StartParticleFromIndex(size_t index) = 0;
	// 根据索引关闭粒子（删除粒子但保留信息）
	virtual bool StopParticleFromIndex(size_t index) = 0;
	// 取得粒子数量
	virtual int GetParticleCount() = 0;
	// 获得粒子信息
	virtual void GetParticleInfoFromIndex(const IVarList& args, IVarList& result) = 0;
	// 设置粒子开关信息
	virtual bool SetParticleEmit(const char* helper_name, int open_emit, 
		int close_emit) = 0;
	// 取得粒子开关信息
	virtual void GetParticleEmit(const IVarList& args, IVarList& result) = 0;

	// 添加刀光参数
	virtual bool AddSaberArcParams(const char* saberarc_name, 
		const char* helper_name0, const char* helper_name1, bool is_start) = 0;
	// 添加刀光信息
	virtual bool AddSaberArc(PERSISTID& id) = 0;
	// 删除刀光
	virtual bool DeleteSaberArc(int index) = 0;
	// 获取刀光
	virtual PERSISTID GetSaberArc(int index) const = 0;
	// 获取刀光数量
	virtual int GetSaberArcCount() = 0;
	// 获得刀光信息
	virtual void GetSaberArcInfo(const IVarList& args, IVarList& result) = 0;

	// 添加光源参数（延迟创建光源）
	virtual bool AddLightParams(const char* light_name, const char* helper_name, 
		int emit_open, int emit_close) = 0;
	// 添加光源参数（直接创建光源）
	virtual bool AddLight(const char* light_name, const char* helper_name, 
		int emit_open, int emit_close) = 0;
	// 根据索引删除光源
	virtual bool DeleteLightFromIndex(int index) = 0;
	// 删除光源（指定辅助点上的所有光源）
	virtual bool DeleteLightOnHelper(const char* helper_name) = 0;
	// 根据名字删除光源
	virtual bool DeleteLightFromName(const char* helper_name, const char* light_name) = 0;
	// 根据名字取得辅助点上的光源
	virtual PERSISTID GetLightFromName(const char* helper_name, const char* light_name) = 0;
	// 根据索引取得辅助点上的光源
	virtual PERSISTID GetLightFromIndex(int index) const = 0;
	// 获取光源数量
	virtual int GetLightCount() = 0;
	// 获得光源信息
	virtual void GetLightInfo(const IVarList& args, IVarList& result) = 0;
	// 设置光源开关信息
	virtual bool SetLightEmit(const char* helper_name, int open_emit, 
		int close_emit) = 0;
	// 取得光源开关信息
	virtual void GetLightEmit(const IVarList& args, IVarList& result) = 0;

	// 保存特效模型到文件
	virtual bool SaveEffectModel(const char* file_name, const char* effect_name) = 0;

	// 设置残影
	virtual bool SetBrokenShadow(bool value) = 0;
	virtual PERSISTID GetBrokenShadow() const = 0;

	// 清理所有子特效和特效数据,使对像保持刚创建时的状态
	virtual bool Clear() = 0;

	// 获得内存占用
	virtual size_t GetMemoryUsage() = 0;

    // 休眠,对于预加载的对象,将其设置为休眠状态,将不做帧处理
    virtual void SetSleep(bool value) = 0;
    virtual bool GetSleep() const = 0;

    // 目标位置
    virtual bool SetTarget(float x, float y, float z) = 0;
    virtual FmVec3 GetTarget() const = 0;
};

#endif
