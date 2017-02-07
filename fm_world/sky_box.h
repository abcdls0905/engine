//--------------------------------------------------------------------
// 文件名:		sky_box.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SKY_BOX_H
#define _SKY_BOX_H

#include "../public/core_type.h"
#include "../visual/i_texture.h"
#include "../visual/i_vis_base.h"
#include "../visual/shader_list.h"
#include "../utils/array_pod.h"
#include "../utils/pod_hash_map.h"
#include "../visual/i_sky_box.h"
#include "../visual/i_render.h"
#include "../visual/i_context.h"
#include "../visual/i_scene_view.h"

// 天空盒
class TSkyBoxShaderAlloc
{
public:
	TSkyBoxShaderAlloc() {}
	~TSkyBoxShaderAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TSkyBoxShaderAlloc& src) {}
};

class SkyBox: public ISkyBox
{
private:
	enum FLAG_SHIFT_ENUM
	{
		FLAG_CUBEMAP,
		FLAG_FOG,
		FLAG_FADEINTEXTURE,
		FLAG_MAX,
	};

	struct sky_vertex_t 
	{
		FmVec3 pos;
		float tu;
		float tv;
	};

    struct sky_box_filter_shaderhandle
    {
        IShaderProgram* pShader;
        fm_int hFogHeight;
        fm_int hFogColor;
        fm_int hMat4PMV;
        fm_int hTexSide;
        fm_int hTexUp;
    };

	//渲染参数状态
	struct sky_cb_object_t
	{
		bool bIsEnablefog;//是否开启天空雾
        bool bZEnable;//是否深度写入
		bool bCubeMap;//是否为cube图
		float c_FogHeight;//雾的高度
		FmVec4 c_FogColor;//雾的颜色
		FmMat4 c_mat4PMV;
	    ITexture*  pTexSide;//四周贴图
	    ITexture*  pTexUp;//顶贴图
		sky_box_filter_shaderhandle* pShaderHandle;
	    IStaticVB* pVBuffer;
	    IStaticIB* pIBuffer;
	};

	// 渲染函数
	static void RenderSky(void* pdata);

    static void RenderCub_Sky(void* pdata);
public:
	SkyBox();
	virtual ~SkyBox();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual bool Load();
	virtual void Update(float seconds);
	//正常绘制天空
	virtual void Realize();

	//正常绘制天空
	virtual void RealizeNormal();

    //绘制天空倒影
	virtual void RealizeReflect();

	// 是否加载完成
	virtual bool GetLoadFinish(); 

	// 位置
	virtual bool SetPosition(float x, float y, float z);
	virtual FmVec3 GetPosition() const;

	// 方向
	virtual bool SetAngle(float x, float y, float z);
	virtual FmVec3 GetAngle() const;

	// 是否可见
	virtual void SetVisible(bool value);
	virtual bool GetVisible() const;
	
	// 是否异步加载贴图
	void SetAsyncLoad(bool value);
	bool GetAsyncLoad() const;
	
	// 是否使用深度测试
	void SetZEnable(bool value);
	bool GetZEnable() const;

	// 乘法因子
	void SetMulFactor(float value);
	float GetMulFactor() const;

	// Y轴旋转速度
	void SetYawSpeed(float value);
	float GetYawSpeed() const;

	// 天空四周的贴图
	void SetSideTex(const char* value);
	const char* GetSideTex() const;
	
	// 天空顶的贴图
	void SetUpTex(const char* value);
	const char* GetUpTex() const;


	/*
	// 淡入的天空顶的贴图
	void SetFadeInUpTex(const char* value);
	const char* GetFadeInUpTex() const;

	// 淡入的天空四周的贴图
	void SetFadeInSideTex(const char* value);
	const char* GetFadeInSideTex() const;
	*/

	// 淡入的时间
	void SetFadeInTime(float value);
	float GetFadeInTime() const;

	// 是否渐变结束
	bool GetFadeInFinish() const;

	// 更新贴图
	bool UpdateTexture();

private:
	// 卸载资源
	bool Unload();
	// 加载贴图
	bool LoadTexture();
	// 释放贴图
	bool UnloadTexture();

	void UpdataRealize(sky_cb_object_t& renderdata);

private:
	IRender*    m_pRender;
	IStaticVB* m_pVBuffer;
	IStaticIB* m_pIBuffer;

	ITexture*  m_pSideTex;
	ITexture*  m_pUpTex;

	bool       m_bNvGPU;//是否是nv gpu

	/*
	ITexture* m_pFadeInUpTex;
	ITexture* m_pFadeInSideTex;
	*/

	FmVec3 m_vPosition;
	FmVec3 m_vAngle;
	FmMat4 m_matWVP;
	bool m_bVisible;
	bool m_bAsyncLoad;
	bool m_bZEnable;
	bool m_bCubeMap;
	float m_fMulFactor;
	float m_fYawSpeed;
	float m_fFadeInTime;
	float m_fFadeInElapseTime;

	core_string m_strSideTex;
	core_string m_strUpTex;

	/*
	core_string m_strFadeInUpTex;
	core_string m_strFadeInSideTex;
	*/

	FmVec4 m_fogcolor;
	float  m_fogheight;
	bool  m_bIsEnablefog;

	float m_offset_seconds;

	TShaderList<IVertexShader, FLAG_MAX> m_VSList;
	TShaderList<IPixelShader, FLAG_MAX> m_PSList;

	TPodHashMap<unsigned int, sky_box_filter_shaderhandle*, TPodTraits<unsigned int>,TSkyBoxShaderAlloc> m_ShaderMap;

	sky_cb_object_t m_Realize;
    sky_cb_object_t m_RealizeReflect;
};

#endif // _SKY_BOX_H
