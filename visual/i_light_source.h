#pragma once

// 光源

class ILightSource: public IVisBase
{
public:
	enum LIGHT_TYPE_ENUM
	{
		LIGHT_TYPE_POINT,
		LIGHT_TYPE_SPOT,
		LIGHT_TYPE_BOX,
	};

public:
	// 灯光名字
	virtual void SetName(const char* value) = 0;
	virtual const char* GetName() const = 0;

	// 光源类型
	virtual void SetLightType(int value) = 0;
	virtual int GetLightType() const = 0;

	// 范围
	virtual void SetRange(float value) = 0;
	virtual float GetRange() const = 0;

	// 颜色
	virtual void SetColor(unsigned int value) = 0;
	virtual unsigned int GetColor() const = 0;

	// 亮度
	virtual void SetIntensity(float value) = 0;
	virtual float GetIntensity() const = 0;

	// 衰减系数0
	virtual void SetAttenu0(float value) = 0;
	virtual float GetAttenu0() const = 0;

	// 衰减系数1
	virtual void SetAttenu1(float value) = 0;
	virtual float GetAttenu1() const = 0;

	// 衰减系数2
	virtual void SetAttenu2(float value) = 0;
	virtual float GetAttenu2() const = 0;

	// 聚光灯内角度
	virtual void SetInnerDegree(float value) = 0;
	virtual float GetInnerDegree() const = 0;

	// 聚光灯外角度
	virtual void SetOuterDegree(float value) = 0;
	virtual float GetOuterDegree() const = 0;

	// 聚光灯下降系数
	virtual void SetFalloff(float value) = 0;
	virtual float GetFalloff() const = 0;

	// 盒子尺寸
	virtual const FmVec3& GetBoxScale() const = 0;
	virtual void SetBoxScaleX(float value) = 0;
	virtual float GetBoxScaleX() const = 0;
	virtual void SetBoxScaleY(float value) = 0;
	virtual float GetBoxScaleY() const = 0;
	virtual void SetBoxScaleZ(float value) = 0;
	virtual float GetBoxScaleZ() const = 0;

	// 灯光闪烁系数
	virtual void SetBlink(float value) = 0;
	virtual float GetBlink() const = 0;

	// 闪烁周期
	virtual void SetBlinkPeriod(float value) = 0;
	virtual float GetBlinkPeriod() const = 0;

	// 闪烁时刻
	virtual void SetBlinkTick(float value) = 0;
	virtual float GetBlinkTick() const = 0;

	// 获得绑定对象
	virtual PERSISTID GetBindID() const = 0;
	// 获得辅助点
	virtual const char* GetHelperName() const = 0;
	// 是否需要被删除
	virtual bool GetComplete() const = 0;

	// 阴影图尺寸
	virtual void SetShadowMapSize(int value) = 0;
	virtual int GetShadowMapSize() const = 0;
	
	// 是否强制每帧都要刷新阴影图
	virtual void SetShadowMapDynamic(bool value) = 0;
	virtual bool GetShadowMapDynamic() const = 0;

	// 是否显示包围盒
	virtual void SetShowBoundBox(bool value) = 0;
	virtual bool GetShowBoundBox() const = 0;

	// 是否显示光照范围
	virtual void SetShowRange(bool value) = 0;
	virtual bool GetShowRange() const = 0;

	// 是否异步加载
	virtual void SetAsyncLoad(bool value) = 0;
	virtual bool GetAsyncLoad() const = 0;

	// 当前在灯光范围的活动物体数量
	virtual void SetActiveRoles(int value) = 0;
	virtual int GetActiveRoles() const = 0;

	// 获得当前的亮度值
	virtual float GetCurIntensity() const = 0;

	// 阴影图
	virtual void ClearShadowTex() = 0;
	virtual void SetShadowTex(int index, IDepthRT* value) = 0;
	virtual IDepthRT* GetShadowTex(int index) const = 0;

	// 阴影图投影矩阵
	virtual void SetShadowViewMatrix(int index, const FmMat4& value) = 0;
	virtual const FmMat4& GetShadowViewMatrix(int index) const = 0;
	virtual void SetShadowProjMatrix(int index, const FmMat4& value) = 0;
	virtual const FmMat4& GetShadowProjMatrix(int index) const = 0;

	// 获得叠加贴图
 	virtual ITexture* GetOverlayTex() const = 0;

	// 光源类型
	virtual void SetLightTypeString(const char* value) = 0;
	virtual result_string GetLightTypeString() const = 0;
	
	// 颜色
	virtual void SetColorString(const char* value) = 0;
	virtual result_string GetColorString() const = 0;

	// 贴图
	virtual void SetTexture(const char* value) = 0;
	virtual const char* GetTexture() const = 0;

	// 是否在视野之内
	virtual bool IsViewClip() = 0;
	// 连接到其他对象
	virtual bool LinkToObject(const PERSISTID& id, float pos_x, float pos_y, 
		float pos_z) = 0;
	virtual  bool LinkToPoint(const PERSISTID& id, const char* helper_name) = 0;

};