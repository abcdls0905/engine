//--------------------------------------------------------------------
// 文件名:		context.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年1月24日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _CONTEXT_H
#define _CONTEXT_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "../utils/string_pod.h"
#include "../visual/i_resource.h"
#include "../visual/i_texture.h"
#include "../visual/i_vis_base.h"
#include "../visual/i_context.h"

class TContextAlloc
{
public:
	TContextAlloc() {}
	~TContextAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TContextAlloc& src) {}
};

// 渲染上下文参数

class Render;

class CContext : public IRenderContext
{
public:
	static CContext* NewInstance(Render* pRender);

private:
	struct layer_data_t
	{
		core_string strName;
		unsigned int nHash;
		TArrayPod<PERSISTID, 1, TContextAlloc> objects;
	};

public:
	CContext(Render* pRender);
	~CContext();

	// 是否开启Clear 
	void SetEnableClearColorOptimize(bool value);
	bool GetEnableClearColorOptimize();

	// 释放
	void Release();

	// 设置视矩阵参数
	bool SetCameraView(const FmVec3& pos, 
		const FmVec3& dir);
	// 设置投影矩阵参数
	bool SetCameraProj(float fov, float zn, float zf, float ratio); 
	// 设置视矩阵
	bool SetViewMatrix(const FmMat4& mat);
	// 设置投影矩阵
	bool SetProjMatrix(const FmMat4& mat);
	// 生成辅助矩阵
	void MakeHelperMatrix();
	// 设置渲染目标尺寸
	bool SetViewSize(int width, int height);
	// 获得渲染目标尺寸
	int GetViewWidth();
	int GetViewHeight();

	// 生成剪裁面
	void MakeClipPlane();

	// 半透明物体降级显示
	void SetBlendDownLevel(int value);
	int GetBlendDownLevel();
	IColorRT* GetBlendColorRT();
	IDepthRT* GetBlendDepthRT();

	// 是否要渲染折射图
	void SetEnableRealizeRefraction(bool value);
	bool GetEnableRealizeRefraction();
	IColorRT* GetRefractionRT();

	// 是否开启零食渲染RT
	// 是否要开启临时渲染纹理缓冲
	void SetEnableRealizeTempColorRT(bool value);
	bool GetEnableRealizeTempColorRT();
	IColorRT* GetRealizeTempColorRT();

	// 是否要开启临时渲染深度缓冲
	void SetEnableRealizTempDepthRT(bool value);
	bool GetEnableRealizTempDepthRT();
	IDepthRT* GetRealizeTempDepthRT();

	// 是否要开启临时渲染纹理缓冲
	void SetEnableRealizeCopyTempColorRT(bool value);
	bool GetEnableRealizeCopyTempColorRT();
	IColorRT* GetRealizeCopyTempColorRT();

	// 不可采样的零时深度缓冲区
	virtual void SetEnableRealizeTempDepthTex(bool value);
	virtual bool GetEnableRealizeTempDepthTex();
	virtual IDepthTex* GetRealizeTempDepthTex();

	// 可采样的零时深度模板缓冲区
	virtual void SetEnableRealizeTempDepthStencilRT(bool value);
	virtual bool GetEnableRealizeTempDepthStencilRT();
	virtual IDepthStencilRT* GetRealizeTempDepthStencilRT();

	// 临时模板缓冲区
	virtual void SetEnableRealizeTempStencilTex(bool value);
	virtual bool GetEnableRealizeTempStencilTex();
	virtual IStencilTex* GetRealizeTempStencilTex();

	// 是否要开启临时渲染深度缓冲
	virtual void SetEnableRealizeCopyTempDepth(bool value);
	virtual bool GetEnableRealizeCopyTempDepth() const;
	virtual IColorRT* GetRealizeCopyTempDepth();

	// 用于测试RT渲染的调试函数 用于debug版本
	void SetDebugColorRT(IColorRT* pColorRT);
	IColorRT* GetDebugColorRT();

	// 用于测试RT渲染调试函数 用于 debug版本
	void SetDebugDepthRT(IDepthRT* pDepthRT);
	IDepthRT* GetDebugDepthRT();

	// 用于测试Texture渲染的调试函数 用于debug版本
	void SetDebugColorTexture(ITexture* pTex);
	ITexture* GetDebugTexture();

	// 用于测试RT渲染调试函数 用于debug版本
	virtual void SetDebugDepthStencilRT(IDepthStencilRT* pDepthStencilRT);
	virtual IDepthStencilRT* GetDebugDepthStencilRT();

	// 用于测试一些解码ColorRT 用于debug版本哦
	virtual void SetDebugDecodeColorRT(IColorRT* pColorRT);
    virtual IColorRT* GetDebugDecodeColorRT();

	// 是否开启全局环境贴图
	void SetGlobalEnvMap(const char* name);
	const char* GetGlobalEnvMap();
	ITexture* GetGlobalEnvCubeTex();

	// 是否开启动态阴影
	void SetEnableDynamicShadow(bool value) ;
	bool GetEnableDynamicShadow() ;

	// 是否使用硬件阴影图过滤
	void SetPercentageCloserFilter(bool value) ;
	bool GetPercentageCloserFilter() ;

	// 是否使用方差阴影图
	void SetVarianceShadowMap(bool value) ;
	bool GetVarianceShadowMap();

	// 计算阴影图矩阵
	void CalculateShadowMapMatrix();

	// 获得阴影图矩阵
	const FmMat4* GetShadowMapViewMatrix(int index);
	const FmMat4* GetShadowMapProjMatrix(int index); 

	// 阴影图是否可以继续使用（无需刷新）
	bool GetShadowMapNoFlush(int index);

	// 深度斜率偏置
	void SetDepthSlope(int index, float value);
	float GetDepthSlope(int index) ;

	// 深度常量偏置
	void SetDepthBias(int index, float value) ;
	float GetDepthBias(int index) ;

	// 阴影图数量
	void SetShadowMapCount(int value) ;
	int GetShadowMapCount() ;

	// 当前阴影图索引
	void SetShadowMapIndex(int value) ;
	int GetShadowMapIndex() ;

	// 阴影图对数切分权重
	void SetLogSplitWeight(float value) ;
	float GetLogSplitWeight();

	// 阴影图尺寸
	void SetShadowMapSize(unsigned int value);
	unsigned int GetShadowMapSize();

	// 阴影图摄像机与目标物体的距离
	void SetShadowMapCameraDistance(float value) ;
	float GetShadowMapCameraDistance() ;

	// 阴影投影面积扩展百分比
	void SetShadowExpandPercent(float value) ;
	float GetShadowExpandPercent() ;

	// 远处的阴影图是否可以重复使用
	void SetShadowMapReusable(bool value);
	bool GetShadowMapReusable();

	// 获得阴影图渲染目标
	IColorRT* GetShadowMapRT(int index);
	IDepthTex* GetShadowMapDS(int index);

	// 获得屏幕空间动态阴影结果图
	IColorRT* GetDynamicShadowRT();
	// 层是否存在
	bool LayerExists(const char* name);
	// 清空层
	bool LayerClear(const char* name);
	// 添加对象到层
	bool LayerAddObject(const char* name, IVisBase* pObj);
	// 从层删除对象
	bool LayerRemoveObject(const char* name, IVisBase* pObj);
	// 对层做拣选
	bool LayerCull(const char* name, const FmPlane* planes, 
		size_t plane_num, const FmVec3& center, float radius, 
		CULL_FUNC cull_func, void* context);
	// 对层做碰撞检测
	float LayerTrace(const char* name, const FmVec3& src, 
		const FmVec3& dst);
	bool LayerTraceHitted(const char* name, const FmVec3& src, 
		const FmVec3& dst);

	// 设置场景唯一的实体
	bool SetUnique(IEntity* pEntity);
	// 获得场景唯一的实体
	IEntity* GetUnique(const char* entity_name);

	// 整数值
	int GetInt(size_t index) const 
	{
		Assert(index < MAX_INT_NUM);

		return m_IntValues[index];
	}

	void SetInt(size_t index, int value)
	{
		Assert(index < MAX_INT_NUM);

		m_IntValues[index] = value;
	}

	// 浮点值
	float GetFloat(size_t index) const
	{
		Assert(index < MAX_FLOAT_NUM);

		return m_FloatValues[index];
	}

	void SetFloat(size_t index, float value)
	{
		Assert(index < MAX_FLOAT_NUM);

		m_FloatValues[index] = value;
	}

	// 颜色值
	unsigned int GetColor(size_t index) const
	{
		Assert(index < MAX_COLOR_NUM);

		return m_ColorValues[index];
	}

	void SetColor(size_t index, unsigned int value)
	{
		Assert(index < MAX_COLOR_NUM);

		m_ColorValues[index] = value;
	}

	// 向量值
	const FmVec3& GetVector(size_t index) const
	{
		Assert(index < MAX_VECTOR_NUM);

		return *(FmVec3*)&m_VectorValues[index];
	}

	void SetVector(size_t index, const FmVec3& value)
	{
		Assert(index < MAX_VECTOR_NUM);

		*(FmVec3*)&m_VectorValues[index] = value;
	}

	// 4元向量值
	const FmVec4& GetVector4(size_t index) const
	{
		Assert(index < MAX_VECTOR_NUM);

		return m_VectorValues[index];
	}

	void SetVector4(size_t index, const FmVec4& value)
	{
		Assert(index < MAX_VECTOR_NUM);

		m_VectorValues[index] = value;
	}

	// 参考位置
	void SetReferPosition(float x, float y, float z)
	{
		m_vReferPosition = FmVec3(x, y, z);
	}

	const FmVec3& GetReferPosition() const { return m_vReferPosition; }

	// 获得当前摄像机参数
	const camera_t& GetCamera() const { return m_Camera; }
	// 获得上一帧的摄像机参数
	const camera_t& GetPrevCamera() const { return m_PrevCamera; }
	// 保存上一帧的摄像机参数
	void SavePrevCamera() {	m_PrevCamera = m_Camera; }

	// 获得剪裁面
	const FmPlane* GetClipPlanes() const { return m_ClipPlane; }
	// 获得剪裁面数量
	int GetClipPlaneAmount() const { return MAX_CLIP_PLANE; }

	// 最大视距
	void SetFarClipDistance(float value) { m_fFarClipDistance = value; }
	float GetFarClipDistance() const { return m_fFarClipDistance; }

	// 包围球是否超过最大视距
	bool IsFarClip(const FmVec3& pos, float radius)
	{
		const FmVec3& cam_pos = m_Camera.vPosition;
		float sx = pos.x - cam_pos.x;
		float sy = pos.y - cam_pos.y;
		float sz = pos.z - cam_pos.z;
		float dist = m_fFarClipDistance + radius;

		return (sx * sx + sy * sy + sz * sz) > (dist * dist);
	}

	// 包围球是否超过最大水平视距
	bool IsFarClip2D(const FmVec3& pos, float radius)
	{
		const FmVec3& cam_pos = m_Camera.vPosition;
		float sx = pos.x - cam_pos.x;
		float sz = pos.z - cam_pos.z;
		float dist = m_fFarClipDistance + radius;

		return (sx * sx + sz * sz) > (dist * dist);
	}

	// 包围球是否在剪裁面外
	bool IsViewClip(const FmVec3& pos, float radius)
	{
		for (size_t k = 0; k < MAX_CLIP_PLANE; ++k)
		{
			float len = FmPlaneDotCoord(&m_ClipPlane[k], &pos);

			if (len > radius)	
			{
				return true;
			}
		}

		return false;
	}

	// 是否要开启深度预渲染
	virtual void SetEnableEarlyZ(bool value);
	virtual bool GetEnableEarlyZ() const;

	virtual void SetShadowAdjustingRect(FmVec4 vAdjustingRect);
	virtual const FmVec4& GetShadowAdjustingRect();

private:
	// 获得渲染目标尺寸
	int GetRenderWidth() const;
	int GetRenderHeight() const;

	// 创建和释放渲染目标
	bool CreateBlendRT();
	bool ReleaseBlendRT();

	bool CreateRefractionRT();
	bool ReleaseRefractionRT();

	bool CreateCopyTempColorRT();
	bool ReleaseCopyTempColorRT();

	bool CreateCopyTempDepth();
	bool ReleaseCopyTempDepth();

	bool CreateShadowMapRT();
	bool ReleaseShadowMapRT();

	bool CreateTempColorRT();
	bool ReleaseTempColorRT();

	bool CreateTempDepthRT();
	bool ReleaseTempDepthRT();

	bool CreateTempDepthTex();
	bool ReleaseTempDepthTex();

	bool CreateTempDepthStencilRT();
	bool ReleaseTempDepthStencilRT();

	bool CreateTempStencilTex();
	bool ReleaseTempStencilTex();

	// 释放层
	void ReleaseLayer();
	// 回收层中的对象
	void CollectLayer();
	// 查找层中的对象
	bool FindLayerIndex(const char* name, size_t& index) const;

	// 获得视锥顶点
	void GetViewFrustumPointList(FmMat4 mtxView, float fFOV, float fAspect, float fNear, float fFar, FmVec3* vFrustum);
	// 获得中心点
	void GetPointListCenter(unsigned int nPointCount, const FmVec3* pPointList, FmVec3* center);

private:
	Render* m_pRender;
	bool m_bEnableEarlyZ;
	int m_nViewWidth;
	int m_nViewHeight;
	int m_nBlendDownLevel;
	bool m_bEnableClearColorOptimize;
	bool m_bEnableRealizeRefraction;
	bool m_bEnableRealizeCopyTempColorRT;
	bool m_bEnableRealizeCopyTempDepthRT;
	bool m_bEnableRealizeTempColorRT;
	bool m_bEnableRealizeTempDepthRT;
	bool m_bEnableRealizeTempDepthTex;
	bool m_bEnableRealizeTempDephtStencilRT;
	bool m_bEnableRealizeTempStencilTex;
	IColorRT* m_pBlendColorRT;
	IDepthRT* m_pBlendDepthRT;
	IColorRT* m_pRefractionRT;
	IColorRT* m_pRealizeCopyColorTempRT;
	IColorRT* m_pRealizeCopyTempDepthRT;
	IColorRT* m_pRealizeTempColorRT;
	IDepthRT* m_pRealizeTempDepthRT;
	IDepthTex* m_pRealizeTempDepthTex;
	IDepthStencilRT* m_pRealizeTempDepthStencilRT;
	IStencilTex* m_pRealizeTempStencilTex;

	float     m_fRealizeTempRTScale;

	IColorRT* m_pDebugColorRT;
	IDepthRT* m_pDebugDepthRT;
	ITexture* m_pDebugTex;
	IDepthStencilRT* m_pDebugDepthStencilRT;
	IColorRT* m_pDecodeColorRT;

	core_string m_strGlobalEnvMap;
	ITexture* m_pGlobalEnvMap;
 
	bool m_bEnableDynamicShadow;
	bool m_bPercentageCloserFilter;
	bool m_bVarianceShadowMap;
	float m_fLogSplitWeight;
	unsigned int m_nShadowMapSize;
	int m_nShadowMapCount;
	int m_nShadowMapIndex;
	float m_fCameraDistance;
	float m_fShadowExpandPercent;
	bool m_bShadowMapReusable;
	FmVec3 m_vShadowLightDir;
	FmMat4 m_mtxShadowMapView[MAX_SHADOWMAP_NUM];
	FmMat4 m_mtxShadowMapProj[MAX_SHADOWMAP_NUM];
	FmVec2 m_vShadowOrthoMin[MAX_SHADOWMAP_NUM];
	FmVec2 m_vShadowOrthoMax[MAX_SHADOWMAP_NUM];
	FmVec3 m_vShadowOrthoCenter[MAX_SHADOWMAP_NUM];
	int m_nShadowMapNoFlush[MAX_SHADOWMAP_NUM];
	float m_fDepthSlope[MAX_SHADOWMAP_NUM];
	float m_fDepthBias[MAX_SHADOWMAP_NUM];
	IColorRT* m_pShadowMapRT[MAX_SHADOWMAP_NUM];
	IDepthTex* m_pShadowMapDS[MAX_SHADOWMAP_NUM];
	IColorRT* m_pShadowRT;

	TArrayPod<layer_data_t*, 1, TContextAlloc> m_Layers;
	TStringPod<char, PERSISTID, TStringTraits<char>, 
		TContextAlloc> m_Uniques;
	int m_IntValues[MAX_INT_NUM];
	float m_FloatValues[MAX_FLOAT_NUM];
	unsigned int m_ColorValues[MAX_COLOR_NUM];
	FmVec4 m_VectorValues[MAX_VECTOR_NUM];
	FmVec3 m_vReferPosition;
	camera_t m_Camera;
	camera_t m_PrevCamera;
	float m_fFarClipDistance;
	FmPlane m_ClipPlane[MAX_CLIP_PLANE];

	FmVec4 m_vAdjustingRect;
};

#endif // _CONTEXT_H


/*
渲染关联器
将渲染相关的对象统一进行存档

比如说如要打开后处理，必须先将SetEnableRealizeTempRT 给打开


*/