//--------------------------------------------------------------------
// 文件名:		light_source.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年9月29日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _LIGHT_SOURCE_H
#define _LIGHT_SOURCE_H

#include "../public/core_type.h"
#include "../visual/i_vis_base.h"
#include "../visual/i_resource.h"
#include "../visual/i_texture.h"
#include "../visual/i_render.h"
#include "../visual/i_light_source.h"

// 光源

class LightSource: public ILightSource
{
public:
	enum LIGHT_TYPE_ENUM
	{
		LIGHT_TYPE_POINT,
		LIGHT_TYPE_SPOT,
		LIGHT_TYPE_BOX,
	};

public:
	LightSource();
	virtual ~LightSource();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void Update(float seconds);
	virtual void Realize(float offset_seconds);

	// 是否加载完成
	virtual bool GetLoadFinish(); 

	// 位置
	virtual bool SetPosition(float x, float y, float z);
	virtual FmVec3 GetPosition() const;

	// 角度
	virtual bool SetAngle(float x, float y, float z);
	virtual FmVec3 GetAngle() const;

	// 包围盒尺寸
	virtual FmVec3 GetBoxSize();
	// 中心点位置
	virtual FmVec3 GetCenter();
	// 半径
	virtual float GetRadius();

	// 碰撞测试详细信息
	virtual bool TraceDetail(const FmVec3& src, const FmVec3& dst, 
		trace_info_t& result);

	// 是否可见
	virtual void SetVisible(bool value) { m_bVisible = value; }
	virtual bool GetVisible() const { return m_bVisible; }

	// 是否检测碰撞
	virtual void SetTraceEnable(bool value) { m_bTraceEnable = value; }
	virtual bool GetTraceEnable() const { return m_bTraceEnable; }

	// 灯光名字
	void SetName(const char* value) { m_strName = value; }
	const char* GetName() const { return m_strName.c_str(); }

	// 光源类型
	void SetLightType(int value) { m_nLightType = value; }
	int GetLightType() const { return m_nLightType; }

	// 范围
	void SetRange(float value) { m_fRange = value; }
	float GetRange() const { return m_fRange; }

	// 颜色
	void SetColor(unsigned int value) { m_nColor = value; }
	unsigned int GetColor() const { return m_nColor; }

	// 亮度
	void SetIntensity(float value) { m_fIntensity = value; }
	float GetIntensity() const { return m_fIntensity; }

	// 衰减系数0
	void SetAttenu0(float value) { m_fAttenu0 = value; }
	float GetAttenu0() const { return m_fAttenu0; }

	// 衰减系数1
	void SetAttenu1(float value) { m_fAttenu1 = value; }
	float GetAttenu1() const { return m_fAttenu1; }

	// 衰减系数2
	void SetAttenu2(float value) { m_fAttenu2 = value; }
	float GetAttenu2() const { return m_fAttenu2; }

	// 聚光灯内角度
	void SetInnerDegree(float value);
	float GetInnerDegree() const { return m_fInnerDegree; }

	// 聚光灯外角度
	void SetOuterDegree(float value);
	float GetOuterDegree() const { return m_fOuterDegree; }

	// 聚光灯下降系数
	void SetFalloff(float value) { m_fFalloff = value; }
	float GetFalloff() const { return m_fFalloff; }

	// 盒子尺寸
	const FmVec3& GetBoxScale() const { return m_vBoxScale; }
	void SetBoxScaleX(float value) { m_vBoxScale.x = value; }
	float GetBoxScaleX() const { return m_vBoxScale.x; }
	void SetBoxScaleY(float value) { m_vBoxScale.y = value; }
	float GetBoxScaleY() const { return m_vBoxScale.y; }
	void SetBoxScaleZ(float value) { m_vBoxScale.z = value; }
	float GetBoxScaleZ() const { return m_vBoxScale.z; }

	// 灯光闪烁系数
	void SetBlink(float value) { m_fBlink = value; }
	float GetBlink() const { return m_fBlink; }

	// 闪烁周期
	void SetBlinkPeriod(float value) { m_fBlinkPeriod = value; }
	float GetBlinkPeriod() const { return m_fBlinkPeriod; }

	// 闪烁时刻
	void SetBlinkTick(float value) { m_fBlinkTick = value; }
	float GetBlinkTick() const { return m_fBlinkTick; } 

	// 获得绑定对象
	PERSISTID GetBindID() const { return m_BindID; }
	// 获得辅助点
	const char* GetHelperName() const { return m_strHelperName.c_str(); }
	// 是否需要被删除
	bool GetComplete() const { return m_bComplete; }

	// 阴影图尺寸
	void SetShadowMapSize(int value);
	int GetShadowMapSize() const { return m_nShadowMapSize; }
	
	// 是否强制每帧都要刷新阴影图
	void SetShadowMapDynamic(bool value) { m_bShadowMapDynamic = value; }
	bool GetShadowMapDynamic() const { return m_bShadowMapDynamic; }

	// 是否显示包围盒
	void SetShowBoundBox(bool value) { m_bShowBoundBox = value; }
	bool GetShowBoundBox() const { return m_bShowBoundBox; }

	// 是否显示光照范围
	void SetShowRange(bool value) { m_bShowRange = value; }
	bool GetShowRange() const { return m_bShowRange; }

	// 是否异步加载
	void SetAsyncLoad(bool value) { m_bAsyncLoad = value; }
	bool GetAsyncLoad() const { return m_bAsyncLoad; }

	// 当前在灯光范围的活动物体数量
	void SetActiveRoles(int value) { m_nActiveRoles = value; }
	int GetActiveRoles() const { return m_nActiveRoles; }

	// 获得当前的亮度值
	float GetCurIntensity() const { return m_fCurIntensity; }

	// 是否聚光灯
	//bool IsSpotLight() const
	//{
	//	return (m_fOuterDegree >= 1.0F) && (m_fOuterDegree <= 120.0F)
	//		&& (m_fInnerDegree <= m_fOuterDegree);
	//}

	// 阴影图
	void ClearShadowTex();
	void SetShadowTex(int index, IDepthRT* value);
	IDepthRT* GetShadowTex(int index) const;

	// 阴影图投影矩阵
	void SetShadowViewMatrix(int index, const FmMat4& value);
	const FmMat4& GetShadowViewMatrix(int index) const;
	void SetShadowProjMatrix(int index, const FmMat4& value);
	const FmMat4& GetShadowProjMatrix(int index) const;

	// 获得叠加贴图
 	ITexture* GetOverlayTex() const;

	/*
	// 获得常量缓冲
	IDynamicCB* GetConstBuffer(size_t index, unsigned int size);

	// 投影常量缓冲
	void SetShadowCB(size_t index, IDxDynamicCB* value) 
	{ 
		Assert(index < 6);
		
		m_pShadowCB[index] = value; 
	}

	IDxDynamicCB* GetShadowCB(size_t index) const 
	{ 
		Assert(index < 6);

		return m_pShadowCB[index];
	}
	*/

	// 光源类型
	void SetLightTypeString(const char* value);
	result_string GetLightTypeString() const;
	
	// 颜色
	void SetColorString(const char* value);
	result_string GetColorString() const;

	// 贴图
	void SetTexture(const char* value);
	const char* GetTexture() const;

	// 是否在视野之内
	bool IsViewClip();
	// 连接到其他对象
	bool LinkToObject(const PERSISTID& id, float pos_x, float pos_y, 
		float pos_z);
	bool LinkToPoint(const PERSISTID& id, const char* helper_name);

private:
	// 更新绑定位置
	bool UpdateBind();

private:
	core_string m_strName;
	core_string m_strTexture;
	FmVec3 m_vPosition;
	FmVec3 m_vAngle;
	FmVec3 m_vBoxScale;
	int m_nLightType;
	unsigned int m_nColor;
	float m_fIntensity;
	float m_fRange;
	float m_fAttenu0;
	float m_fAttenu1;
	float m_fAttenu2;
	float m_fInnerDegree;
	float m_fOuterDegree;
	float m_fFalloff;
	float m_fCurIntensity;
	float m_fBlink;
	float m_fDistance;
	float m_fBlinkPeriod;
	float m_fBlinkTick;
	float m_fBlinkCounter;
	float m_fDistTime;
	PERSISTID m_BindID;
	int m_nHelperID;
	core_string m_strHelperName;
	FmVec3 m_vBindPos;
	int m_nShadowMapSize;
	bool m_bVisible;
	bool m_bTraceEnable;
	bool m_bComplete;
	bool m_bShowBoundBox;
	bool m_bShowRange;
	bool m_bAsyncLoad;
	bool m_bShadowMapDynamic;
	int m_nActiveRoles;
	ITexture* m_pTexture;
	/*
	IDxDynamicCB* m_pCB[6];
	IDxDynamicCB* m_pShadowCB[6];
	*/
	IDepthRT* m_pShadowTex[6];
	FmMat4 m_ShadowViewMatrix[6];
	FmMat4 m_ShadowProjMatrix[6];
	IRender* m_pRender;
};

#endif // _LIGHT_SOURCE_H
