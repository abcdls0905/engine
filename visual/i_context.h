#ifndef _VISUAL_ICONTEXT_H
#define _VISUAL_ICONTEXT_H

#include "../public/Macros.h"


#include "i_resource.h"

class IEntity;
class IVisBase;
class ITexture;

class IRenderContext
{
public:
	// 剪裁面数量
	enum { MAX_CLIP_PLANE = 4 };

	// 主光源阴影图数量
	enum { MAX_SHADOWMAP_NUM = 1 };

	// 整数值
	enum
	{
		I_FOG_EXP,				// 是否开启指数雾效 
		I_FOG_LINEAR,			// 是否开启线性雾效
		I_UNDER_WATER,			// 是否在水下
		I_SKIN_SPHERE_AMBIENT,	// 骨骼动画模型是否开启半球环境光
		I_MODEL_SPHERE_AMBIENT,	// 普通模型是否开启半球环境光
		I_ENABLE_ROLE_LIGHT,	// 开启角色点光源照明
		I_ENABLE_ROLE_FALLOFF,	// 开启角色轮廓光
		I_WATER_ENVMAP,			// 当前是否在渲染水面的倒影
		I_ENABLE_CAMERA_LIGHT,	// 开启摄像机光源
		I_USE_CLIP_PLANE,		// 使用剪裁面
		I_USE_TESSELLATION,		// 使用表面细分
		I_HEIGHT_FOG,			// 是否开启高度雾
		MAX_INT_NUM,
	};

	// 浮点值
	enum 
	{
		F_FOG_HEIGHT,			// 线性雾高度
		F_FOG_START,			// 当前线性雾的起始距离
		F_FOG_END,				// 当前线性雾的结束距离
		F_FOG_DENSITY,			// 指数雾浓度
		F_FOG_START_GROUND,		// 地面的雾参数
		F_FOG_END_GROUND,		
		F_FOG_DENSITY_GROUND,		
		F_FOG_START_UNDERWATER,	// 水下的雾参数	
		F_FOG_END_UNDERWATER,
		F_FOG_DENSITY_UNDERWATER,		
		F_WIND_ANGLE,			// 风向
		F_WIND_SPEED,			// 风速
		F_AMBIENT_INTENSITY,	// 环境光亮度系数
		F_DIFFUSE_INTENSITY,	// 主光源亮度系数
		F_SPECULAR_INTENSITY,	// 镜面光照亮度系数
		F_ROLE_AMBIENT_FACTOR,	// 角色环境光调节因子
		F_ROLE_DIFFUSE_FACTOR,	// 角色漫反射调节因子
		F_ROLE_LIGHT_RANGE,		// 角色点光源范围
		F_ROLE_LIGHT_INTENSITY,	// 角色点光源亮度
		F_ROLE_FALLOFF_INTENSITY, // 角色轮廓光亮度
		F_ROLE_FALLOFF_POWER,	// 角色轮廓光指数
		F_WATER_HEIGHT,			// 渲染水面倒影时的水面高度
		F_REFLECT_FACTOR,		// 环境反射的乘法系数
		F_TESS_FACTOR_EDGE,		// 表面细分参数
		F_TESS_FACTOR_INSIDE,
		F_HEIGHT_FOG_START,		// 当前高度雾的起始高度
		F_HEIGHT_FOG_END,		// 当前高度雾的结束高度
		MAX_FLOAT_NUM,
	};

	// 颜色值
	enum
	{
		C_AMBIENT_COLOR,		// 环境光颜色
		C_DIFFUSE_COLOR,		// 主光源颜色
		C_SPECULAR_COLOR,		// 镜面光照颜色
		C_FOG_COLOR,			// 当前线性雾颜色
		C_FOG_COLOR_GROUND,		// 地面的线性雾颜色
		C_FOG_COLOR_UNDERWATER, // 水下的线性雾颜色
		C_FOG_EXP_COLOR,		// 指数雾颜色
		C_ROLE_LIGHT_COLOR,		// 角色点光源颜色
		C_ROLE_FALLOFF_COLOR,	// 角色轮廓光颜色
		C_CAMERA_LIGHT_COLOR,	// 摄像机光源颜色
		C_HEIGHTFOG_COLOR,		// 当前高度雾颜色
		MAX_COLOR_NUM,
	};

	// 向量值
	enum
	{
		V_LIGHT_DIRECTION,		// 主光源的方向
		V_LIGHT_AMBIENT,		// 主光源ambient
		V_LIGHT_DIFFUSE,		// 主光源diffuse
		V_LIGHT_SPECULAR,		// 主光源specular
		V_FOCUS_POSITION,		// 摄像机焦点位置
		V_FOCUS_RADIUS,			// 摄像机焦点半径
		V_ROLE_LIGHT_POSITION,	// 角色点光源位置
		V_FOG_CURRENT,			// 当前线性雾颜色值
		V_FOG_EXP_CURRENT,		// 当前指数雾颜色值
		V_CLIP_PLANE,			// 剪裁面
		V_HEIGHTFOG_CURRENT,	// 当前高度雾颜色值
		MAX_VECTOR_NUM,
	};
public:
	virtual ~IRenderContext(){}
	
	// 释放
	virtual void Release() = 0;

	// 设置视矩阵参数
	virtual bool SetCameraView(const FmVec3& pos, 
		const FmVec3& dir)  = 0;
	// 设置投影矩阵参数
	virtual bool SetCameraProj(float fov, float zn, float zf, float ratio)  = 0; 
	// 设置视矩阵
	virtual bool SetViewMatrix(const FmMat4& mat)  = 0;
	// 设置投影矩阵
	virtual bool SetProjMatrix(const FmMat4& mat)  = 0;
	// 生成辅助矩阵
	virtual void MakeHelperMatrix()  = 0;
	// 设置渲染目标尺寸
	virtual bool SetViewSize(int width, int height)  = 0;
	// 获得渲染目标尺寸
	virtual int GetViewWidth()  = 0;
	virtual int GetViewHeight()  = 0;

	// 生成剪裁面
	virtual void MakeClipPlane()  = 0;

	// 半透明物体降级显示
	virtual void SetBlendDownLevel(int value)  = 0;
	virtual int GetBlendDownLevel()  = 0;
	virtual IColorRT* GetBlendColorRT()  = 0;
	virtual IDepthRT* GetBlendDepthRT()  = 0;
	
	// 是否要渲染折射图
	virtual void SetEnableRealizeRefraction(bool value)  = 0;
	virtual bool GetEnableRealizeRefraction()  = 0;
	virtual IColorRT* GetRefractionRT()  = 0;

	// 是否要开启临时渲染纹理缓冲
	virtual void SetEnableRealizeTempColorRT(bool value)  = 0;
	virtual bool GetEnableRealizeTempColorRT() = 0;
	virtual IColorRT* GetRealizeTempColorRT() = 0;

	// 是否要开启临时渲染深度缓冲
	virtual void SetEnableRealizTempDepthRT(bool value)  = 0;
	virtual bool GetEnableRealizTempDepthRT() = 0;
	virtual IDepthRT* GetRealizeTempDepthRT() = 0;

	// 是否开启Clear 
	virtual void SetEnableClearColorOptimize(bool value) = 0;
	virtual bool GetEnableClearColorOptimize() = 0;

	// 不可采样的零时深度缓冲区
	virtual void SetEnableRealizeTempDepthTex(bool value) = 0;
	virtual bool GetEnableRealizeTempDepthTex() = 0;
	virtual IDepthTex* GetRealizeTempDepthTex() = 0;

	// 可采样的临时深度模板缓冲区
	virtual void SetEnableRealizeTempDepthStencilRT(bool value) = 0;
	virtual bool GetEnableRealizeTempDepthStencilRT() = 0;
	virtual IDepthStencilRT* GetRealizeTempDepthStencilRT() = 0;

	// 临时模板缓冲区
	virtual void SetEnableRealizeTempStencilTex(bool value) = 0;
	virtual bool GetEnableRealizeTempStencilTex() = 0;
	virtual IStencilTex* GetRealizeTempStencilTex() = 0;

	// 是否要开启临时拷贝渲染纹理缓冲
	virtual void SetEnableRealizeCopyTempColorRT(bool value)  = 0;
	virtual bool GetEnableRealizeCopyTempColorRT() = 0;
	virtual IColorRT* GetRealizeCopyTempColorRT() = 0;

	// 是否要开启临时拷贝渲染深度缓冲
	virtual void SetEnableRealizeCopyTempDepth(bool value)  = 0;
	virtual bool GetEnableRealizeCopyTempDepth() const  = 0;
	virtual IColorRT* GetRealizeCopyTempDepth() = 0;

	// 用于测试RT渲染的调试函数 用于debug版本
	virtual void SetDebugColorRT(IColorRT* pColorRT)  = 0;
	virtual IColorRT* GetDebugColorRT()  = 0;

	// 用于测试RT渲染调试函数 用于 debug版本
	virtual void SetDebugDepthRT(IDepthRT* pDepthRT)  = 0;
    virtual IDepthRT* GetDebugDepthRT()  = 0;

	// 用于测试RT渲染调试函数 用于debug版本
	virtual void SetDebugDepthStencilRT(IDepthStencilRT* pDepthStencilRT)  = 0;
	virtual IDepthStencilRT* GetDebugDepthStencilRT() = 0;

	// 用于测试Texture渲染的调试函数 用于debug版本
	virtual void SetDebugColorTexture(ITexture* pTex)  = 0;
    virtual ITexture* GetDebugTexture()  = 0;

	// 用于测试一些解码ColorRT 用于debug版本哦
	virtual void SetDebugDecodeColorRT(IColorRT* pColorRT)  = 0;
    virtual IColorRT* GetDebugDecodeColorRT()  = 0;

	// 是否开启全局环境贴图
	virtual void SetGlobalEnvMap(const char* name) = 0;
	virtual const char* GetGlobalEnvMap() = 0;
	virtual ITexture* GetGlobalEnvCubeTex() = 0;

	// 是否开启动态阴影
	virtual void SetEnableDynamicShadow(bool value) = 0;
	virtual bool GetEnableDynamicShadow() = 0;

	// 是否使用硬件阴影图过滤
	virtual void SetPercentageCloserFilter(bool value) = 0;
	virtual bool GetPercentageCloserFilter() = 0;

	// 是否使用方差阴影图
	virtual void SetVarianceShadowMap(bool value) = 0;
	virtual bool GetVarianceShadowMap() = 0;

	// 计算阴影图矩阵
	virtual void CalculateShadowMapMatrix() = 0;
	
	// 获得阴影图矩阵
	virtual const FmMat4* GetShadowMapViewMatrix(int index) = 0;
	virtual const FmMat4* GetShadowMapProjMatrix(int index) = 0; 

	// 阴影图是否可以继续使用（无需刷新）
	virtual bool GetShadowMapNoFlush(int index) = 0;
	
	// 深度斜率偏置
	virtual void SetDepthSlope(int index, float value) = 0;
	virtual float GetDepthSlope(int index) = 0;

	// 深度常量偏置
	virtual void SetDepthBias(int index, float value) = 0;
	virtual float GetDepthBias(int index) = 0;

	// 阴影图数量
	virtual void SetShadowMapCount(int value) = 0;
	virtual int GetShadowMapCount() = 0;
	
	// 当前阴影图索引
	virtual void SetShadowMapIndex(int value) = 0;
	virtual int GetShadowMapIndex() = 0;

	// 阴影图对数切分权重
	virtual void SetLogSplitWeight(float value) = 0;
	virtual float GetLogSplitWeight() = 0;

	// 阴影图尺寸
	virtual void SetShadowMapSize(unsigned int value) = 0;
	virtual unsigned int GetShadowMapSize() = 0;
	
	// 阴影图摄像机与目标物体的距离
	virtual void SetShadowMapCameraDistance(float value) = 0;
	virtual float GetShadowMapCameraDistance() = 0;

	// 阴影投影面积扩展百分比
	virtual void SetShadowExpandPercent(float value) = 0;
	virtual float GetShadowExpandPercent() = 0;

	// 远处的阴影图是否可以重复使用
	virtual void SetShadowMapReusable(bool value) = 0;
	virtual bool GetShadowMapReusable() = 0;

	// 获得阴影图渲染目标
	virtual IColorRT* GetShadowMapRT(int index) = 0;
	virtual IDepthTex* GetShadowMapDS(int index) = 0;

	// 获得屏幕空间动态阴影结果图
	virtual IColorRT* GetDynamicShadowRT() = 0;

	// 层是否存在
	virtual bool LayerExists(const char* name) = 0;
	// 清空层
	virtual bool LayerClear(const char* name) = 0;
	// 添加对象到层
	virtual bool LayerAddObject(const char* name, IVisBase* pObj) = 0;
	// 从层删除对象
	virtual bool LayerRemoveObject(const char* name, IVisBase* pObj) = 0;
	// 对层做拣选
	virtual bool LayerCull(const char* name, const FmPlane* planes, 
		size_t plane_num, const FmVec3& center, float radius, 
		CULL_FUNC cull_func, void* context) = 0;
	// 对层做碰撞检测
	virtual float LayerTrace(const char* name, const FmVec3& src, 
		const FmVec3& dst) = 0;
	virtual bool LayerTraceHitted(const char* name, const FmVec3& src, 
		const FmVec3& dst) = 0;

	// 设置场景唯一的实体
	virtual bool SetUnique(IEntity* pEntity) = 0;
	// 获得场景唯一的实体
	virtual IEntity* GetUnique(const char* entity_name) = 0;

	// 整数值
	virtual int GetInt(size_t index) const  = 0;
	
	virtual void SetInt(size_t index, int value) = 0;
	
	// 浮点值
	virtual float GetFloat(size_t index) const = 0;
	
	virtual void SetFloat(size_t index, float value) = 0;

	// 颜色值
	virtual unsigned int GetColor(size_t index) const = 0;

	virtual void SetColor(size_t index, unsigned int value) = 0;
	
	// 向量值
	virtual const FmVec3& GetVector(size_t index) const = 0;
	
	virtual void SetVector(size_t index, const FmVec3& value) = 0;
	
	// 4元向量值
	virtual const FmVec4& GetVector4(size_t index) const = 0;
	
	virtual void SetVector4(size_t index, const FmVec4& value) = 0;
	
	// 参考位置
	virtual void SetReferPosition(float x, float y, float z) = 0;
	
	virtual const FmVec3& GetReferPosition() const  = 0;

	// 获得当前摄像机参数
	virtual const camera_t& GetCamera() const = 0;
	// 获得上一帧的摄像机参数
	virtual const camera_t& GetPrevCamera() const = 0;
	// 保存上一帧的摄像机参数
	virtual void SavePrevCamera() = 0;

	// 获得剪裁面
	virtual const FmPlane* GetClipPlanes() const  = 0;
	// 获得剪裁面数量
	virtual int GetClipPlaneAmount() const = 0;

	// 最大视距
	virtual void SetFarClipDistance(float value) = 0;
	virtual float GetFarClipDistance() const = 0;

	// 包围球是否超过最大视距
	virtual bool IsFarClip(const FmVec3& pos, float radius) = 0;

	// 包围球是否超过最大水平视距
	virtual bool IsFarClip2D(const FmVec3& pos, float radius) = 0;
	
	// 包围球是否在剪裁面外
	virtual bool IsViewClip(const FmVec3& pos, float radius) = 0;

	// 是否要开启深度预渲染
	virtual void SetEnableEarlyZ(bool value) = 0;
	virtual bool GetEnableEarlyZ() const = 0;

	// 动态阴影有效距离
	void SetShadowDistance(float value) { m_fShadowDistance = value; }
	float GetShadowDistance() const { return m_fShadowDistance; }

	// 点光源最大距离
	void SetMaxLightDistance(float value) { m_fMaxLightDistance = value; }
	float GetMaxLightDistance() const { return m_fMaxLightDistance; }

	// 是否修正开启抗锯齿时的动态阴影边缘问题
	void SetEnableFixShadow(bool value) { m_bEnableFixShadow = value; }
	bool GetEnableFixShadow() const { return m_bEnableFixShadow; }

	// 设置动态阴影区域的修正矩形
	// vAdjustingRect.x = left
	// vAdjustingRect.y = right
	// vAdjustingRect.z = top
	// vAdjustingRect.w = bottom
	virtual void SetShadowAdjustingRect(FmVec4 vAdjustingRect) = 0;

	// 获取动态阴影区域的修正矩形
	// vResult.x = left
	// vResult.y = right
	// vResult.z = top
	// vResult.w = bottom
	virtual const FmVec4& GetShadowAdjustingRect() = 0;

protected:

	float m_fShadowDistance;
	float m_fMaxLightDistance;
	bool m_bEnableFixShadow;
};
#endif