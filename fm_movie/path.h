//--------------------------------------------------------------------
// 文件名:		Path.h
// 内  容:		
// 说  明:		
// 创建日期:	2012年7月4日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#ifndef __PATH_H__
#define __PATH_H__

#include "../visual/i_visual.h"
#include <vector>

// 路径

class Path: public IVisual
{
private:
	// 路径插值方式
	enum InterpolationMode
	{
		InterpolationModeLinear = 0,	// 线性插值
		InterpolationModeCurve,			// 曲线插值，目前用的是Catmull-Rom
	};
	// 朝向
	enum OrientationMode
	{
		OrientationModeNull = 0,		// 不修改朝向
		OrientationModeTarget,			// 面向目标对象
		OrientationModeForward,			// 面向前进方向
	};

	struct path_vertex_t 
	{
		FmVec3 pos;
		unsigned int color;
	};

	struct key_point_t
	{
		key_point_t() { time = 0.0f; }
		FmVec3 pos;
		float time;						// 关键帧时间，单位是秒
	};

	enum { MAX_VERTEX_NUM = 8000 };

public:
	Path();
	virtual ~Path() {}

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void Update(float seconds);
	virtual void Realize(float offset_seconds);

	// 增加一个关键点
	size_t AddKeyPoint(float x, float y, float z, float time);
	// 删除一个关键点
	bool DelKeyPoint(size_t index);
	// 修改关键点的坐标
	bool SetKeyPointPosition(size_t index, float x, float y, float z);
	// 设置关键点的关键帧时间
	bool SetKeyPointTime(size_t index, float time);

	// 插值密度，即插值点的间距
	void SetPointDistance(float value);
	float GetPointDistance() const { return m_fPointDistance; }

	// 插值方式
	void SetInterpolationModeString(const char* value);
	std::string GetInterpolationModeString() const;

	// 路径控制的对象
	void SetBindObject(const PERSISTID& value);
	PERSISTID GetBindObject() const;

	// 朝向的对象
	void SetTargetObject(const PERSISTID& value);
	PERSISTID GetTargetObject() const;

	// 当前时间
	void SetCurTime(float value);
	float GetCurTime() const;

	// 总时间
	void SetTotalTime(float value);
	float GetTotalTime() const;

	// 是否暂停
	void SetPause(bool value);
	bool GetPause() const;

private:
	// 插值方式
	void InnerSetInterpolationMode(int value);
	// 重新生成插值点
	bool RebuildInterpolationPoint();
	// 把点添加到点队列里的通用函数
	void InnerAddPoint(const FmVec3& pos, unsigned int uColor,
		std::vector<path_vertex_t>& v);
	// 把点立方体添加到点队列里的通用函数
	void InnerAddPointCube(const FmVec3& pos, unsigned int uColor,
		float fCubeRadius, std::vector<path_vertex_t>& v);
	// 更新绑定对象
	void InnerUpdateObject();
	IVisBase* GetVisBase(const PERSISTID& id) const;

	// 根据朝向计算角度
	FmVec3* GetFaceAngle(FmVec3* pOut, FmVec3* pEye, FmVec3* pAt);

private:
	// 关键点
	std::vector<key_point_t> m_keyPoints;

	// 所有点，包括关键点和插值点
	std::vector<path_vertex_t> m_points;
	// 线
	std::vector<path_vertex_t> m_lines;
	// 关键点的小立方体模型
	std::vector<path_vertex_t> m_keyPointCube;

	// 插值方式
	int m_nInterpolationMode;
	// 插值密度，即插值点的间距
	float m_fPointDistance;

	// 朝向方式
	int m_nOrientationMode;
	// 朝向的对象
	PERSISTID m_targetObject;

	FmMat4 m_matWorld;
	unsigned int m_uLineColor;
	unsigned int m_uPointColor;
	unsigned int m_uKeyPointCubeColor;

	// 路径控制的对象
	PERSISTID m_bindObject;
	// 当前时间的关键点
	int m_nCurKeyPoint;
	// 当前时间
	float m_fCurTime;
	// 总时间
	float m_fTotalTime;
	// 是否暂停
	bool m_bPause;
};

#endif // __PATH_H__
