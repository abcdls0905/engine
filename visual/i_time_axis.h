//--------------------------------------------------------------------
// 文件名:		i_time_axis.h
// 内  容:		时间轴接口
// 说  明:		
// 创建日期:	2014年1月13日
// 创建人:		叶峰
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef __i_time_axis_H__
#define __i_time_axis_H__

#include "../visual/i_vis_base.h"
#include "../visual/i_res_loader.h"

typedef int (*time_axis_run_callback)(IEntity* pEntity, const char* event, const IVarList& args);

class ITimeAxis: public IVisBase
{
public:
	// 播放
	virtual void Play() = 0;
	// 暂停在指定时间
	virtual void Pause(float time) = 0;
	// 暂停在当前时间
	virtual void PauseNow() = 0;

	// 是否加载完成
	virtual bool GetLoadFinish() = 0;
	virtual size_t GetLoadCount() const = 0;
	// 添加对象
	virtual bool AddObject(const char* object_name) = 0;
	// 删除对象
	virtual bool DeleteObject(const char* object_name) = 0;
	// 对象是否存在
	virtual bool IsObjectExist(const char* object_name) = 0;
	// 拷贝对象数据
	virtual bool CopyObjectData(const char* src_object_name, const char* dst_object_name) = 0;
	// 对象重命名
	virtual bool ReName(const char* src_object_name, const char* new_object_name) = 0;
	// 将一个控制器的信息赋值到另一对象中
	virtual bool CopyControllerData(const char* src_object_name, int controller_type, 
		const char* dst_object_name) = 0;
	// 拷贝关键帧
	virtual void CopyKeyTime(const char* object_name, float time, float new_time) = 0;
	// 拷贝某个控制器的关键帧
	virtual void CopyControllerKeyTime(const char* object_name, int controller_type,
		float time, float new_time) = 0;
	// 关键帧数据是否存在
	virtual bool IsKeyFrameExist(const char* object_name, int controller_type, float frame_time) const = 0;

	// 对象ID
	virtual bool SetObjectID(const char* object_name, const PERSISTID& value) = 0;
	virtual PERSISTID GetObjectID(const char* object_name) const = 0;

	// 增加Bezier关键点
	virtual bool AddBezierKey(const char* object_name, int controller_type, float time, float value) = 0;
	// 改变控制点的计算方式
	virtual void ChangeComputeTypeKey(const char* object_name, int controller_type, float time, int type) = 0;
	// 重新计算所有的控制点
	virtual void CreateAllControlPoints(const char* object_name, int controller_type) = 0;
	// 移动对象关键帧
	virtual void MoveKeyTimes(const char* object_name, float time_add) = 0;
	// 移动控制器的关键帧
	virtual void MoveControllerKeyTimes(const char* object_name, int controller_type, float time_add) = 0;
	// 移动Bezier曲线
	virtual void MoveAllKeyValue(const char* object_name, int controller_type, float value_add) = 0;
	// 移动关键帧
	virtual void MoveKey(const char* object_name, float time, float new_time) = 0;
	// 移动关键帧
	virtual void MoveControllerKey(const char* object_name, int controller_type, float time, float new_time) = 0;

	// 获取要记录关键点的前一个关键点
	virtual void GetFrontBezierKey(const IVarList &param, IVarList &result) = 0;

	// 获取所有传入关键帧信息的相邻关键帧数据(请保证使用完返回的关键帧指针前不修改或删除关键帧信息)
	virtual void GetAdjacencyBezierKeyData(const IVarList &args, IVarList &result) const = 0;

	// 增加或修改关键帧
	virtual bool SetKeyFrameBezierMode(const char* object_name, int controller_type, 
		float time, float value, 
		float left_time, float left_value, 
		float right_time, float right_value) = 0;
	virtual bool SetKeyFrameOnceMode(const char* object_name, int controller_type, 
		float time, const char* param) = 0;
	virtual bool SetKeyFrameLinearMode(const char* object_name, int controller_type, 
		float time, float value) = 0;
	virtual bool SetKeyFrameScaleMode(const char* object_name, int controller_type, 
		float time, const char* action_name, float time_length, float speed) = 0;

	// 删除某时刻的所有关键帧
	virtual void DeleteKey(const char* object_name, float time) = 0;
	// 删除关键帧
	virtual bool DeleteControllerKey(const char* object_name, int controller_type, 
		float time) = 0;
	// 清空指定控制器的所有关键帧
	virtual void ClearControllerKeyFrames(const char* object_name, int controller_type) = 0;
	// 清空对象所有控制器的所有关键帧
	virtual void ClearControllers(const char* object_name) = 0;
	// 获得关键帧列表
	virtual void GetControllerKeyTime(const IVarList& args, IVarList& result) const = 0;
	virtual int GetControllerKeyCount(const char* object_name, int controller_type) const = 0;
	virtual int GetKeyFrameListBezierMode(const IVarList& args, IVarList& result) const = 0;
	virtual int GetKeyFrameListOnceMode(const IVarList& args, IVarList& result) const = 0;
	virtual int GetKeyFrameListLinearMode(const IVarList& args, IVarList& result) const = 0;
	virtual int GetKeyFrameListScaleMode(const IVarList& args, IVarList& result) const = 0;
	// 获得对象所有控制器ID
	virtual int GetControllers(const IVarList& args, IVarList& result) const = 0;
	// 获取指定时间的控制器值
	virtual float GetValueBezierMode(const char* object_name, int controller_type, 
		float time) const = 0;
	//virtual const char* GetValueOnceMode(const char* object_name, int controller_type, 
	//	float time) const = 0;
	// 获取某一时刻关键帧帧数据
	virtual void GetKeyFrameValue(const IVarList& args, IVarList& result) const = 0;
	// 获取存储轨迹的坐标类型（是否是相对坐标）
	virtual bool GetIsLocalPosition(const char* object_name) const = 0;
	// 获取对象相对坐标系的坐标
	virtual void GetLocalOrigin(const IVarList &args, IVarList& result) const = 0;
	// 设置对象相对坐标系的坐标
	virtual void SetLocalOrigin(const IVarList &args, IVarList& result) = 0;

	// 获取存储轨迹是否经过了旋转
	virtual bool GetIsRotation(const char* object_name) const = 0;
	// 获取轨迹旋转
	virtual float GetRotationY(const char* object_name) const = 0;
	// 设置轨迹旋转,如果是actor需要旋转轨迹180
	virtual void SetRotationY(const char* object_name, float fAngle) = 0;
	// 获取轨迹的坐标系旋转角度
	virtual void GetRotation(const IVarList& args, IVarList& result) const = 0;
	// 设置轨迹的坐标系旋转角度
	virtual void SetRotation(const IVarList& args, IVarList& result) = 0;

	// 获取对象的额外属性
	virtual const char* GetExtProp(const char* object_name) const = 0;
	// 获取初始状态的位置(主要解决目标摄像机未key的问题)
	virtual void GetInitialPosition(const IVarList &args, IVarList& result) = 0;


	// 坐标转换
	virtual void PositionToLocal(const IVarList& args, IVarList& result) const = 0;
	// 角度转换
	virtual void AngleToLocal(const IVarList& args, IVarList& result) const = 0;

	// 是否循环播放
	virtual void SetLoop(bool value) = 0;
	virtual bool GetLoop() const = 0;

	// 开始时间
	virtual void SetBeginTime(float value) = 0;
	virtual float GetBeginTime() const = 0;
	// 结束时间
	virtual void SetEndTime(float value) = 0;
	virtual float GetEndTime() const = 0;

	// 两帧间时长
	virtual void SetFrameInterval(float value) = 0;
	virtual float GetFrameInterval() const = 0;

	// 路径是否可见
	virtual void SetPathVisible(const char* object_name, bool bVisible) = 0;
	virtual bool GetPathVisible(const char* object_name) const = 0;

	// 生成用于显示的路径点
	virtual bool BuildPathPoint(const char* object_name) = 0;

	// 加载对象关键帧
	virtual bool LoadObjectKeyFrame(const char* object_name, const char* file, 
		bool bAsync) = 0;

	// 保存对象关键帧
	virtual bool SaveObjectKeyFrame(const char* object_name, const char* file) = 0;
		
	// 保存所有对象
	virtual bool SaveAllObject(const char* path, bool bForce) = 0;

	// 导出对象关键帧至max(增加缩放)
	virtual bool ExportToMaxPath(const char* object_name, const char* file_name,
		float x, float y, float z, float angle_y, int scale) = 0;

	virtual bool ExportToMaxPathFrames(const char* object_name, const char* file_name,
		float x, float y, float z, float angle_y, int scale) = 0;

	// 震动关键帧随机数设置.
	virtual bool LoadRandomData(const char *file_name) = 0;
	virtual bool SaveRandomData(const char *file_name) = 0;

    // 设置回调函数
    virtual void SetCallbackFunc(time_axis_run_callback func) = 0;
    virtual time_axis_run_callback GetCallbackFunc() = 0;
};

#endif // __i_time_axis_H__
