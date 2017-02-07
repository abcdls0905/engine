//--------------------------------------------------------------------
// 文件名:		i_vis_base.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _VISUAL_I_VIS_BASE_H
#define _VISUAL_I_VIS_BASE_H

#include "../public/module.h"


#include "vis_utils.h"

// 可视对象基类

class IRenderContext;

class IVisBase: public IEntity
{
public:
	IVisBase()
	{
		m_pContext = NULL;
		m_pUserData = NULL;
	}

	virtual ~IVisBase() = 0;

	// 更新状态
	virtual void Update(float seconds) {}
	// 正常渲染
	virtual void Realize() {}
	// 渲染阴影贴图
	virtual void RealizeShadowMap(){}

	// 加载数据
	virtual bool Load() { return true; }
	// 加载数据是否成功
	virtual bool GetLoaded() { return true; }
	// 是否完成加载或不在加载过程中（同步或异步）
	virtual bool GetLoadFinish() { return true; }

	// 位置
	virtual bool SetPosition(float x, float y, float z) { return false; }
	virtual FmVec3 GetPosition() const { return FmVec3(0.0F, 0.0F, 0.0F); }
	// 方向
	virtual bool SetAngle(float x, float y, float z) { return false; }
	virtual FmVec3 GetAngle() const { return FmVec3(0.0F, 0.0F, 0.0F); }
	// 缩放系数
	virtual bool SetScale(float x, float y, float z) { return false; }
	virtual FmVec3 GetScale() const { return FmVec3(1.0F, 1.0F, 1.0F); }
	
	// 获得世界坐标的中心点
	virtual FmVec3 GetCenter() 
	{ 
		return GetPosition() + FmVec3(0.0F, GetRadius(), 0.0F); 
	}

	// 获得包围盒尺寸
	virtual FmVec3 GetBoxSize() { return FmVec3(0.0F, 0.0F, 0.0F); }
	// 获得半径
	virtual float GetRadius() { return 0.0F; }
	
	// 世界转换矩阵
	virtual void SetWorldMatrix(const FmMat4& mat) {}
	virtual void GetWorldMatrix(FmMat4& mat)
	{
		FmVec3 position = GetPosition();
		FmVec3 angle = GetAngle();
		FmVec3 scale = GetScale();	
		FmQuat rotate;

		FmQuatRotationYawPitchRoll(&rotate, angle.y, angle.x, 
			angle.z);
		VisUtil_MatrixTransform(&mat, &scale, &rotate, &position);
	}

	// 碰撞测试详细信息
	virtual bool TraceDetail(const FmVec3& src, const FmVec3& dst, 
		trace_info_t& result) { return false; }

	// 碰撞测试是否有阻挡
	virtual bool TraceHitted(const FmVec3& src, const FmVec3& dst) 
	{ 
		trace_info_t ti;

		return TraceDetail(src, dst, ti);
	}

	// 球体范围碰撞
	virtual bool TraceSphere(const FmVec3& center, 
		float radius) { return false; }
	// 三角形碰撞检测
	virtual bool TraceTriangle(const FmVec3& v0, 
        const FmVec3& v1, const FmVec3& v2) { return false; }

	// 拣选
	virtual bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context) { return false; }
		
	// 是否可见
	virtual void SetVisible(bool value) {}
	virtual bool GetVisible() const { return true; }

	// 是否在水下
	virtual void SetUnderWater(bool value) {}
	virtual bool GetUnderWater() const { return false; }

	// 颜色
	virtual void SetColor(unsigned int value) {}
	virtual unsigned int GetColor() const { return 0; }

	// 细节等级
	virtual void SetLevelOfDetail(float value) {}
	virtual float GetLevelOfDetail() const { return 1.0F; }

	// 是否检测碰撞
	virtual void SetTraceEnable(bool value) {}
	virtual bool GetTraceEnable() const { return true; }

	// 是否可以剪裁
	virtual void SetCullEnable(bool value) {}
	virtual bool GetCullEnable() const { return true; }

	// 是否需要水面反射
	virtual void SetWaterReflect(bool value) {}
	virtual bool GetWaterReflect() const { return false; }

	// 记录临时的透明度变量（只供地形系统使用）
	virtual void SetAlphaValue(float value) {}
	virtual float GetAlphaValue() const { return 1.0F; }

	// 需要投射影子
	virtual void SetCastShadow(bool value) {}
	virtual bool GetCastShadow() const { return false; }

	// 需要接受投射影子
	virtual void SetReceiveShadow(bool value) {}
	virtual bool GetReceiveShadow() const { return false; }

	// 为了方便在脚本中读取
	float GetPositionX() const { return GetPosition().x; }
	float GetPositionY() const { return GetPosition().y; }
	float GetPositionZ() const { return GetPosition().z; }
	float GetAngleX() const { return GetAngle().x; }
	float GetAngleY() const { return GetAngle().y; }
	float GetAngleZ() const { return GetAngle().z; }
	float GetCenterX() { return GetCenter().x; }
	float GetCenterY() { return GetCenter().y; }
	float GetCenterZ() { return GetCenter().z; }
	float GetBoxSizeX() { return GetBoxSize().x; }
	float GetBoxSizeY() { return GetBoxSize().y; }
	float GetBoxSizeZ() { return GetBoxSize().z; }

	// 渲染器参数
	virtual void SetContext(IRenderContext* value) { m_pContext = value; }
	IRenderContext* GetContext() const { return m_pContext; }

	// 用户数据指针
	void SetUserData(void* value) { m_pUserData = value; }
	void* GetUserData() const { return m_pUserData; }

protected:
	IRenderContext* m_pContext;
	void* m_pUserData;
};

inline IVisBase::~IVisBase() {}

#endif // _VISUAL_I_VIS_BASE_H
