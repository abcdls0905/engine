//--------------------------------------------------------------------
// 文件名:		TimeAxis.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2012年8月6日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#include "time_axis.h"
#include "time_axis_macro.h"
#include "utility.h"
#include "helper.h"
#include "../visual/i_render.h"
#include "../public/core_log.h"
#include "camera_wrapper.h"
#include "object_loader.h"

/// entity: TimeAxis
/// \brief TimeAxis实体
/// \see 继承自IVisBase
DECLARE_ENTITY(TimeAxis, 0, IVisBase);

DECLARE_PROPERTY_GET(size_t, TimeAxis, LoadCount, GetLoadCount);
/// property: float BeginTime
/// \brief 开始时间
DECLARE_PROPERTY(float, TimeAxis, BeginTime, GetBeginTime, SetBeginTime);
/// property: float EndTime
/// \brief 结束时间
DECLARE_PROPERTY(float, TimeAxis, EndTime, GetEndTime, SetEndTime);
/// property: float FrameInterval
/// \brief 两帧间时长
DECLARE_PROPERTY(float, TimeAxis, FrameInterval, GetFrameInterval, SetFrameInterval);
/// property: bool Loop
/// \brief 是否循环播放
DECLARE_PROPERTY(bool, TimeAxis, Loop, GetLoop, SetLoop);

/*
/// method: void Play();
/// \brief 播放
DECLARE_METHOD_VOID_0(TimeAxis, Play);
/// method: void PauseNow();
/// \brief 暂停在当前时间
DECLARE_METHOD_VOID_0(TimeAxis, PauseNow);
/// method: void Pause(float time);
/// \brief 暂停在指定时间
/// \param time 暂停的时间
DECLARE_METHOD_VOID_1(TimeAxis, Pause, float);

/// method: bool AddObject(const char* object_name);
/// \brief 添加对象
/// \param object_name 对象名
DECLARE_METHOD_1(bool, TimeAxis, AddObject, const char*);
/// method: bool DeleteObject(const char* object_name);
/// \brief 删除对象
/// \param object_name 对象名
DECLARE_METHOD_1(bool, TimeAxis, DeleteObject, const char*);
/// method: bool IsObjectExist(const char* object_name);
/// \brief 对象是否存在
/// \param object_name 对象名
DECLARE_METHOD_1(bool, TimeAxis, IsObjectExist, const char*);
/// method: bool CopyObjectData(const char* src_object_name, const char* dst_object_name);
/// \brief 拷贝对象数据
/// \param src_object_name 源对象名
/// \param dst_object_name 目标对象名
DECLARE_METHOD_2(bool, TimeAxis, CopyObjectData, const char*, const char*);
/// method: bool ReName(const char* src_object_name, const char* new_object_name);
/// \brief 对象重命名
/// \param src_object_name 原对象名
/// \param new_object_name 新对象名
DECLARE_METHOD_2(bool, TimeAxis, ReName, const char*, const char*);
/// method: bool CopyControllerData(const char* src_object_name, const char* dst_object_name);
/// \brief 拷贝对象的控制器数据
/// \param src_object_name 源对象名
/// \param controller_type 控制器id
/// \param dst_object_name 目标对象名
DECLARE_METHOD_3(bool, TimeAxis, CopyControllerData, const char*, int, const char*);
/// method: void CopyKeyTime(const char* object_name, float time, float new_time);
/// \brief 拷贝某个控制的关键帧
/// \param time 被拷贝的帧
/// \param new_time 新的关键帧
DECLARE_METHOD_VOID_3(TimeAxis, CopyKeyTime, const char*, float, float);
/// method: void CopyControllerKeyTime(const char* object_name,
///         float time, float new_time);
/// \brief 拷贝某个控制的关键帧
/// \param controller_type 控制器id
/// \param time 被拷贝的帧
/// \param new_time 新的关键帧
DECLARE_METHOD_VOID_4(TimeAxis, CopyControllerKeyTime, const char*, int, float, float);
/// method: bool IsKeyFrameExist(const char* object_name, int controller_type, float frame_time)
/// \brief 拷贝对象的控制器数据
/// \param src_object_name 源对象名
/// \param controller_type 控制器id
/// \param dst_object_name 目标对象名
DECLARE_METHOD_3(bool, TimeAxis, IsKeyFrameExist, const char*, int, float);
/// method: bool SetObjectID(const char* object_name, const PERSISTID& value);
/// \brief 设置对象ID
/// \param object_name 对象名
/// \param value 对象ID
DECLARE_METHOD_2(bool, TimeAxis, SetObjectID, const char*, const PERSISTID&);
/// method: object GetObjectID(const char* object_name);
/// \brief 获得对象ID
/// \param object_name 对象名
DECLARE_METHOD_1(PERSISTID, TimeAxis, GetObjectID, const char*);

/// method: bool AddBezierKey(const char* object_name int, float, float);
/// \brief 增加或修改贝塞尔曲线类关键帧
/// \param object_name 对象名
/// \param controller_type 控制器类型
/// \param time 关键帧时间
/// \param value 关键帧值
DECLARE_METHOD_4(PERSISTID, TimeAxis, AddBezierKey, const char*, int, float, float);

/// method: int GetFrontBezierKey(const IVarList& param, IVarList& result);
/// \brief 获取要记录关键点的前一个关键点
DECLARE_METHOD_T(TimeAxis, GetFrontBezierKey);
/// method: void TimeAxis::GetAdjacencyBezierKeyData(const IVarList &args, IVarList &result)
/// \brief 获取要记录关键点的前一个关键点
DECLARE_METHOD_T(TimeAxis, GetAdjacencyBezierKeyData);
/// method: void ChangeComputeTypeKey(const char* object_name, int controller_type, float time, int type);
/// \brief 更改Bezier控制点的计算方式
/// \param object_name 对象名
/// \param controller_type 控制器类型
/// \param time 关键帧时间
/// \param type 控制点的计算方式
DECLARE_METHOD_VOID_4(TimeAxis, ChangeComputeTypeKey, const char*, int, float, int);
/// method: void CreateAllControlPoints(const char* object_name, int controller_type);
/// \brief 重新计算Bezier控制点
/// \param object_name 对象名
/// \param controller_type 控制器类型
DECLARE_METHOD_VOID_2(TimeAxis, CreateAllControlPoints, const char*, int);
/// method: void MoveKeyTimes(const char* object_name, float time_add);
/// \brief 移动对象的所有关键帧
/// \param object_name 对象名
/// \param 关键帧移动的时间
DECLARE_METHOD_VOID_2(TimeAxis, MoveKeyTimes, const char*, float);
/// method: void MoveControllerKeyTimes(const char* object_name, int controller_type,  float time_add);
/// \brief 移动某个控制器的所有关键帧
/// \param object_name 对象名
/// \param controller_type 控制器类型
/// \param 关键帧移动的时间
DECLARE_METHOD_VOID_3(TimeAxis, MoveControllerKeyTimes, const char*, int, float);

/// method: void MoveAllKeyValue(const char* object_name, int controller_type,  float value_add);
/// \brief 移动某个besize控制器的所有关键帧
/// \param object_name 对象名
/// \param controller_type 控制器类型
/// \param 关键帧移动的值
DECLARE_METHOD_VOID_3(TimeAxis, MoveAllKeyValue, const char*, int, float);
/// method: void MoveKey(const char* object_name, float time, float new_time);
/// \brief 移动对象的某时刻的关键帧
/// \param object_name 对象名
/// \param 时间
/// \param 移动到的时间
DECLARE_METHOD_VOID_3(TimeAxis, MoveKey, const char*, float, float);
/// method: void MoveControllerKey(const char* object_name, int controller_type,  float time_add);
/// \brief 移动对象的某个控制器的某时刻的关键帧
/// \param object_name 对象名
/// \param controller_type 控制器类型
/// \param 时间
/// \param 移动到的时间
DECLARE_METHOD_VOID_4(TimeAxis, MoveControllerKey, const char*, int, float, float);


/// method: bool SetKeyFrameBezierMode(const char* object_name, int controller_type, float time, float value, float left_time, float left_value, float right_time, float right_value);
/// \brief 增加或修改贝塞尔曲线类关键帧
/// \param object_name 对象名
/// \param controller_type 控制器类型
/// \param time 关键帧时间
/// \param value 关键帧值
/// \param left_time 前面的控制点
/// \param left_value 前面的控制点
/// \param right_time 后面的控制点
/// \param right_value 后面的控制点
DECLARE_METHOD_8(bool, TimeAxis, SetKeyFrameBezierMode, const char*, int, float, float, float, float, float, float);
/// method: bool SetKeyFrameOnceMode(const char* object_name, int controller_type, float time, const char* param)
/// \brief 增加或修改一次性触发事件的关键帧
/// \param object_name 对象名
/// \param controller_type 控制器类型
/// \param time 关键帧时间
/// \param param 关键帧值
DECLARE_METHOD_4(bool, TimeAxis, SetKeyFrameOnceMode, const char*, int, float, const char*);
/// method: bool SetKeyFrameLinearMode(const char* object_name, int controller_type, float time, float value)
/// \brief 增加或修改线性插值类关键帧
/// \param object_name 对象名
/// \param controller_type 控制器类型
/// \param time 关键帧时间
/// \param value 关键帧值
DECLARE_METHOD_4(bool, TimeAxis, SetKeyFrameLinearMode, const char*, int, float, float);
/// method: bool SetKeyFrameScaleMode(const char* object_name, int controller_type, float time, const char* action_name, float time_length, float speed);
/// \brief 增加或修改动作关键帧
/// \param object_name 对象名
/// \param controller_type 控制器类型
/// \param time 关键帧时间
/// \param action_name 动作名
/// \param time_length 动作时长
/// \param speed 播放速度
DECLARE_METHOD_6(bool, TimeAxis, SetKeyFrameScaleMode, const char*, int, float, const char*, float, float);

/// method: bool DeleteKey(const char* object_name, float time);
/// \brief 删除关键帧
/// \param object_name 对象名
/// \param time 关键帧时间
DECLARE_METHOD_VOID_2(TimeAxis, DeleteKey, const char*, float);

/// method: bool DeleteControllerKey(const char* object_name, int controller_type, float time);
/// \brief 删除关键帧
/// \param object_name 对象名
/// \param controller_type 控制器类型
/// \param time 关键帧时间
DECLARE_METHOD_3(bool, TimeAxis, DeleteControllerKey, const char*, int, float);
/// method: void ClearControllerKeyFrames(const char* object_name, int controller_type);
/// \brief 清空指定控制器的所有关键帧
/// \param object_name 对象名
/// \param controller_type 控制器类型
DECLARE_METHOD_VOID_2(TimeAxis, ClearControllerKeyFrames, const char*, int);
/// method: void ClearControllers(const char* object_name);
/// \brief 清空对象所有控制器的所有关键帧
/// \param object_name 对象名
DECLARE_METHOD_VOID_1(TimeAxis, ClearControllers, const char*);
///// method: void ClearKeyFrames(const char* object_name);
///// \brief 清空对象的所有关键帧
///// \param object_name 对象名
//DECLARE_METHOD_VOID_1(TimeAxis, ClearKeyFrames, const char*);
/// method: void SetPathVisible(const char* object_name, bool bVisible);
/// \brief 路径是否可见
/// \param object_name 对象名
/// \param bVisible 是否可见
DECLARE_METHOD_VOID_2(TimeAxis, SetPathVisible, const char*, bool);
/// method: bool BuildPathPoint(const char* object_name);
/// \brief 生成用于显示的路径点
/// \param object_name 对象名
DECLARE_METHOD_1(bool, TimeAxis, BuildPathPoint, const char*);
/// method: bool LoadObjectKeyFrame(const char* object_name, const char* file, bool bAsync);
/// \brief 加载对象控制器
/// \param object_name 对象名
/// \param path 文件路径
/// \param file 文件名
DECLARE_METHOD_3(bool, TimeAxis, LoadObjectKeyFrame, const char*, const char*, bool);
/// method: bool SaveObjectKeyFrame(const char* object_name, const char* file);
/// \brief 保存对象关键帧
/// \param object_name 对象名
/// \param path 文件路径
/// \param file 文件名
DECLARE_METHOD_2(bool, TimeAxis, SaveObjectKeyFrame, const char*, const char*);
/// method: bool ExportToMaxPath(const char* object_name, const char* file_name,
///									float x, float y, float z, float angle_y, int scale);
/// \brief 按关键帧方式导出
/// \param object_name 对象名
/// \param file_name 文件名
/// \param x 坐标系x
/// \param y 坐标系y
/// \param z 坐标系z
/// \param angle_y 坐标系的角度
/// \param scale 缩放倍数
DECLARE_METHOD_7(bool, TimeAxis, ExportToMaxPath, const char*, const char*,
					float, float, float, float, int);

/// method: bool ExportToMaxPathFrames(const char* object_name, const char* file_name,
///									float x, float y, float z, float angle_y, int scale);
/// \brief 将角度关键帧按逐帧方式导出
/// \param object_name 对象名
/// \param file_name 文件名
/// \param x 坐标系x
/// \param y 坐标系y
/// \param z 坐标系z
/// \param angle_y 坐标系的角度
/// \param scale 缩放倍数
DECLARE_METHOD_7(bool, TimeAxis, ExportToMaxPathFrames, const char*, const char*,
float, float, float, float, int);

/// method: bool SaveAllObject(const char* path);
/// \brief 保存所有对象
/// \param path 文件路径
DECLARE_METHOD_2(bool, TimeAxis, SaveAllObject, const char*, bool);
/// method: table GetControllerKeyTime(const char* object_name, int controller_type)
/// \brief 获得关键帧的时刻列表
/// \param object_name 对象名
/// \param controller_type 控制器类型
DECLARE_METHOD_T(TimeAxis, GetControllerKeyTime);
/// method: table GetControllerKeyCount(const char* object_name, int controller_type)
/// \brief 获得关键帧的时刻列表
/// \param object_name 对象名
/// \param controller_type 控制器类型
DECLARE_METHOD_2(int, TimeAxis, GetControllerKeyCount, const char*, int);
/// method: table GetKeyFrameListBezierMode(const char* object_name, int controller_type)
/// \brief 获得关键帧列表
/// \param object_name 对象名
/// \param controller_type 控制器类型
DECLARE_METHOD_T(TimeAxis, GetKeyFrameListBezierMode);
/// method: table GetKeyFrameListOnceMode(const char* object_name, int controller_type)
/// \brief 获得关键帧列表
/// \param object_name 对象名
/// \param controller_type 控制器类型
DECLARE_METHOD_T(TimeAxis, GetKeyFrameListOnceMode);
/// method: table GetKeyFrameListLinearMode(const char* object_name, int controller_type)
/// \brief 获得关键帧列表
/// \param object_name 对象名
/// \param controller_type 控制器类型
DECLARE_METHOD_T(TimeAxis, GetKeyFrameListLinearMode);
/// method: table GetKeyFrameListScaleMode(const char* object_name, int controller_type)
/// \brief 获得关键帧列表
/// \param object_name 对象名
/// \param controller_type 控制器类型
DECLARE_METHOD_T(TimeAxis, GetKeyFrameListScaleMode);
/// method: table GetControllers(const char* object_name)
/// \brief 获得对象所有控制器ID
/// \param object_name 对象名
DECLARE_METHOD_T(TimeAxis, GetControllers);
/// method: float GetValueBezierMode(const char* object_name, int controller_type, float time) const;
/// \brief 获取指定时间的控制器值
/// \param object_name 对象名
/// \param controller_type 控制器类型
/// \param time 关键帧时间
DECLARE_METHOD_3(float, TimeAxis, GetValueBezierMode, const char*, int, float);
/// method: const char* GetValueOnceMode(const char* object_name, int controller_type, float time) const;
/// \brief 获取指定时间的控制器值
/// \param object_name 对象名
/// \param controller_type 控制器类型
/// \param time 关键帧时间
//DECLARE_METHOD_3(const char*, TimeAxis, GetValueOnceMode, const char*, int, float);
/// method: void GetKeyFrameValue(const IVarList& args, IVarList& result);
/// \brief 获取控制器指定时间的关键帧
/// \param object_name 对象名
/// \param controller_type 控制器类型
/// \param time 关键帧时间(float类型的，帧数乘过FrameInterval的)
DECLARE_METHOD_T(TimeAxis, GetKeyFrameValue);
/// method: bool GetIsLocalPosition(const char* object_name) const;
/// \brief 获取存储轨迹的坐标类型（是否是相对坐标）
/// \param object_name 对象名
DECLARE_METHOD_1(bool, TimeAxis, GetIsLocalPosition, const char*);

/// method: table GetLocalOrigin(const char* object_name)
/// \brief 获取对象相对坐标系的坐标
/// \param object_name 对象名
DECLARE_METHOD_N(TimeAxis, GetLocalOrigin);
/// method: table SetLocalOrigin(const char* object_name, float x, float y, float z)
/// \brief 设置对象相对坐标系的坐标
/// \param object_name 对象名
/// \param x 坐标系坐标
/// \param y 坐标系坐标
/// \param z 坐标系坐标
DECLARE_METHOD_N(TimeAxis, SetLocalOrigin);
/// method: bool GetIsRotation(const char* object_name) const;
/// \brief 获取存储轨迹是否经过了旋转
/// \param object_name 对象名
DECLARE_METHOD_1(bool, TimeAxis, GetIsRotation, const char*);
/// method: bool GetRotationY(const char* object_name) const;
/// \brief 获取轨迹旋转
/// \param object_name 对象名
DECLARE_METHOD_1(bool, TimeAxis, GetRotationY, const char*);
/// method: bool SetRotationY(const char* object_name, float fAngle);
/// \brief 设置参考对象的类型,如果是actor需要旋转轨迹180
/// \param object_name 对象名
/// \param fAngle 需要旋转的角度
DECLARE_METHOD_VOID_2(TimeAxis, SetRotationY, const char*, float);
/// method: table GetRotation(const char* object_name)
/// \brief 获取对象相对坐标系的坐标
/// \param object_name 对象名
DECLARE_METHOD_N(TimeAxis, GetRotation);
/// method: table SetRotation(const char* object_name, float x, float y, float z)
/// \brief 设置对象相对坐标系的坐标
/// \param object_name 对象名
/// \param rx x轴旋转
/// \param ry y轴旋转
/// \param rz z轴旋转
DECLARE_METHOD_N(TimeAxis, SetRotation);
/// method: bool GetExtProp(const char* object_name) const;
/// \brief 获取对象的额外属性
/// \param object_name 对象名
DECLARE_METHOD_1(const char*, TimeAxis, GetExtProp, const char*);
/// method: table GetRotation(const char* object_name)
/// \brief 获取对象的经变化后的初始位置
/// \param object_name 对象名
DECLARE_METHOD_N(TimeAxis, GetInitialPosition);
/// method: bool PositionToLocal(const char* object_name, float x, float y, float z)
/// \brief 世界空间坐标换算成轨迹空间
/// \param object_name 对象名
/// \param x x坐标
/// \param y y坐标
/// \param z z坐标
DECLARE_METHOD_N(TimeAxis, PositionToLocal);
/// method: bool PositionToLocal(const char* object_name, float rx, float ry, float rz)
/// \brief 世界空间旋转换算成轨迹空间
/// \param object_name 对象名
/// \param rx x旋转
/// \param ry y旋转
/// \param rz z旋转
DECLARE_METHOD_N(TimeAxis, AngleToLocal);
/// method: bool LoadRandomData(const char* file_name) const;
/// \brief 加载随机数
/// \param file_name 文件名
DECLARE_METHOD_1(bool, TimeAxis, LoadRandomData, const char*);
/// method: bool SaveRandomData(const char* file_name) const;
/// \brief 保存随机数
/// \param file_name 文件名
DECLARE_METHOD_1(bool, TimeAxis, SaveRandomData, const char*);

/// event: int on_play(object TimeAxis, float time)
/// \brief 播放时回调
/// \param TimeAxis
/// \param time 当前播放时间
DECLARE_EVENT(on_play);
/// event: int on_play_over(object TimeAxis, float time)
/// \brief 播放结束时回调
/// \param TimeAxis
/// \param time 当前播放时间
DECLARE_EVENT(on_play_over);
/// event: int on_execute_frame(object TimeAxis, PERSISTID object, int controller_type, float time, param)
/// \brief 调用脚本执行当前帧的控制事件
/// \param TimeAxis
/// \param object 需要操作的对象
/// \param controller_type 控制器类型
/// \param time 当前播放时间
/// \param param 参数
DECLARE_EVENT(on_execute_frame);

/// event: int on_key_update(object TimeAxis, const char* object_name, int controller_type, ...)
/// \brief 调用脚本执行当前帧的控制事件
/// \param TimeAxis
/// \param object_name 需要操作的对象名字
/// \param controller_type 控制器的id
DECLARE_EVENT(on_key_update);

/// event: int on_add_object(object TimeAxis, const char* object_name)
/// \brief 调用脚本执行当前帧的控制事件
/// \param TimeAxis
/// \param object_name 需要增加对象的名字
DECLARE_EVENT(on_add_object);

/// event: int on_add_controller(object TimeAxis, const char* object_name, int controller_type, ...)
/// \brief 调用脚本执行当前帧的控制事件
/// \param TimeAxis
/// \param object_name 需要增加控制器的名字
/// \param controller_type 控制器的id
DECLARE_EVENT(on_add_controller);
*/

#define RAND_NUM 100
#define KEY_FRAME_RECT_RADIUS 6
#define SET_KEY_FRAME_VERTEX(vertex, point, _u, _v) vertex.pos = point; vertex.u = _u; vertex.v = _v;

float color_red[4] = {1.0f, 0.0f, 0.0f, 1.0f};
float color_green[4] = {0.0f, 1.0f, 0.0f, 1.0f};
float color_white[4] = {1.0f, 1.0f, 1.0f, 1.0f};
float g_wobble_rand[RAND_NUM] = {0};

sObjectData::sObjectData() 
{ 
	bShowPath = false;
	pFrameVBuffer = NULL;
	nFrameVBufferCount = 0;
	nFrameVCountUsed = 0;
	pKeyFrameVBuffer = NULL;
	nKeyFrameVBufferCount = 0;

	bLocalPosition = false;
	fOriginX = 0.f;
	fOriginY = 0.f;
	fOriginZ = 0.f;

	bRotation = false;

	fRAddY = 0.f;
	fRotationX = 0.f;
	fRotationY = 0.f;
	fRotationZ = 0.f;

	bDataChanged = false;
	pLoader = NULL;
}

sObjectData::~sObjectData()
{
    CORE_FREE(pFrameVBuffer, nFrameVBufferCount*sizeof(vertex_t));
	CORE_FREE(pKeyFrameVBuffer, nKeyFrameVBufferCount*sizeof(vertex_key_frame_t));

	KeyFrameControllers::const_iterator ite_ctrl = controllers.Begin();
	while(ite_ctrl != controllers.End())
	{
		CORE_DELETE(ite_ctrl.GetData());
		++ite_ctrl;
	}
	controllers.Clear();
}

IRender* TimeAxis::s_pRender = NULL;

TimeAxis::TimeAxis()
{
	m_fBeginTime = 0.0f;
	m_fCurTime = m_fBeginTime;
	m_fEndTime = m_fBeginTime + 20.0f;
	m_bPause = true;
	m_bLoop = false;
	m_nLastFrame = -1;
	m_fFrameInterval = 0.04f;
	m_nShader = -1;
	m_nShaderKeyFrame = -1;
	m_nLoadCount = 0;

    m_pFrameVBuffer = NULL;
    m_frameVBufferSize = 0;

    m_pKeyFrameVBuffer = NULL;
    m_keyFrameVBufferSize = 0;

    m_pShaderFrame = NULL;
    m_hMatWVP = 0;
    m_hDiffuse = 0;

    m_pShaderKeyFrame = NULL;
    m_hKeyMatWVP = 0;
    m_hKeyDiffuse = 0;
    m_hKeyTexelSize = 0;

    m_listBatchData = NULL;
    m_nBatchCount = 0;

    m_pRunCallbackFunc = 0;

	srand(0);
}

TimeAxis::~TimeAxis() 
{
    BatchData* ptr = m_listBatchData;
    BatchData* pNext;
    while (ptr)
    {
        pNext = ptr->pNext;
        assert(ptr->pShader == 0);
        CORE_DELETE(ptr);
        ptr = pNext;
    }
    m_listBatchData = NULL;
    m_nBatchCount = 0;
}

bool TimeAxis::Init(const IVarList& args)
{
    // 为保证IRender的引用计数正确,所以每次创建实体都要调用GetCore()->GetInterface("Render");
    s_pRender = (IRender*)GetCore()->GetInterface("Render");
    if (NULL == s_pRender)
    {
        return false;
    }

    IVertexShader* pVS = s_pRender->LoadVertexShader("design_key_frame.vsh", "main", "");
    IPixelShader* pPS = s_pRender->LoadPixelShader("design_key_frame.fsh", "main", "");
    const char* vbFrame[] = {"iPos"};
    m_pShaderFrame = s_pRender->CreateShaderProgram(pVS, pPS, vbFrame, 1);
    if (!m_pShaderFrame)
    {
        CORE_TRACE("(TimeAxis::Init) Load design_frame shader failed.");
        return false;
    }
    m_hMatWVP = m_pShaderFrame->GetParamOp()->FindParamIdByName("c_mtxWVP");
    m_hDiffuse = m_pShaderFrame->GetParamOp()->FindParamIdByName("c_vDiffuse");

    pVS = s_pRender->LoadVertexShader("design_key_frame.vsh", "main", "#define KEY_FRAME");
    const char* vbKeyFrame[] = {"iPos", "iTexCoord"};
    m_pShaderKeyFrame = s_pRender->CreateShaderProgram(pVS, pPS, vbKeyFrame, 2);
    if (!m_pShaderKeyFrame)
    {
        CORE_TRACE("(TimeAxis::Init) Load design_key_frame shader failed.");
        return false;
    }
    m_hKeyMatWVP = m_pShaderKeyFrame->GetParamOp()->FindParamIdByName("c_mtxWVP");
    m_hKeyDiffuse = m_pShaderKeyFrame->GetParamOp()->FindParamIdByName("c_vDiffuse");
    m_hKeyTexelSize = m_pShaderKeyFrame->GetParamOp()->FindParamIdByName("c_vTexelSize");

	KeyFrameController::SetTimeAxis(this);

	for (int i = 0; i < RAND_NUM; ++i)
	{
		g_wobble_rand[i] = float(rand()) / float(RAND_MAX);
		if (g_wobble_rand[i] < 0.1f)
		{
			g_wobble_rand[i] += 0.1f;
		}
	}


    GetCore()->AddExecute(this);


	return true;
}

bool TimeAxis::Shut()
{
	Objects::iterator ite_obj = m_objects.Begin();
	while (ite_obj != m_objects.End())
	{
		sObjectData *pObject = ite_obj.GetData();
		SAFE_RELEASE(pObject->pLoader);
		CORE_DELETE(pObject);
		++ite_obj;
	}
	m_objects.Clear();

    SAFE_RELEASE(m_pFrameVBuffer);
    m_frameVBufferSize = 0;
    SAFE_RELEASE(m_pKeyFrameVBuffer);
    m_keyFrameVBufferSize = 0;

    BatchData* ptr = m_listBatchData;
    while (ptr)
    {
        ptr->pShader = 0;
        ptr = ptr->pNext;
    }

    SAFE_RELEASE(m_pShaderFrame);
    SAFE_RELEASE(m_pShaderKeyFrame);

    SAFE_RELEASE(s_pRender);

    return true;
}

void TimeAxis::Play()
{ 
	InnerUpdate(m_fCurTime);
	m_bPause = false;
}

void TimeAxis::Pause(float time)
{
	m_bPause = true;
	m_fCurTime = time;

	InnerUpdate(m_fCurTime);
}

void TimeAxis::PauseNow()
{
	m_bPause = true;

	InnerUpdate(m_fCurTime);
}

void TimeAxis::Update(float seconds)
{
	if (m_bPause)
	{
		return;
	}

	if (m_fCurTime < m_fEndTime)
	{
		m_fCurTime += seconds;
		if (m_fCurTime > m_fEndTime)
		{
			m_fCurTime = m_fEndTime;
		}
	}

	bool bTimeOver = false;
	// 已经达到最大时间
	if (m_fCurTime >= m_fEndTime)
	{
		bTimeOver = true;

		if (m_bLoop)
		{
			// 执行最后一帧
			InnerUpdate(m_fEndTime);

			// 从头开始
			m_fCurTime = m_fBeginTime;
		}
		else
		{
			// 停在最后一帧
			// Pause会执行一次InnerUpdate
			Pause(m_fEndTime);
			fm_movie::TimeAxis_RunCallback(this, "on_play_over", CVarList()<<m_fCurTime);
		}
	}
	else
	{
		InnerUpdate(m_fCurTime);
	}

	// 每关键帧帧调用一次
	int nFrame = int(m_fCurTime / m_fFrameInterval + 0.00001f);
	if (m_nLastFrame != nFrame || bTimeOver)
	{
		m_nLastFrame = nFrame;

		fm_movie::TimeAxis_RunCallback(this, "on_play", CVarList()<<nFrame);
	}
}

IVisBase* TimeAxis::GetVisBase(const PERSISTID& id) const
{
	IEntity* pEntity = GetCore()->GetEntity(id);

	if (NULL == pEntity)
	{
		return NULL;
	}

	if (!pEntity->GetEntInfo()->IsKindOf("IVisBase"))
	{
		return NULL;
	}

	return (IVisBase*)pEntity;
}

void TimeAxis::InnerUpdate(float time)
{
	CVarList param;
	CVarList temp;

	bool			bPosKey;

	// 统计震动相关的关键帧
	float	fWobbleInfo[7];
	bool	bWobbleKey;
	float	fMinKey = RAND_MAX, fMaxKey = -RAND_MAX;

	Objects::iterator ite_obj = m_objects.Begin();
	while (ite_obj != m_objects.End())
	{
		sObjectData& object_data = *ite_obj.GetData();

		IVisBase* pObject = GetVisBase(object_data.ObjectID);
		if (NULL == pObject)
        {
#if 1
            // 测试轨迹是否运行正确
            memset(object_data.fPosInfo, 0, sizeof(object_data.fPosInfo));
            bPosKey = false;

            KeyFrameControllers& controllers = ite_obj.GetData()->controllers;
            KeyFrameControllers::iterator ite_ctrl = controllers.Begin();
            while (ite_ctrl != controllers.End())
            {
                KeyFrameController& controller = *ite_ctrl.GetData();
                int controller_type = controller.GetControllerType();

                if (controller_type >= CONTROLLER_TYPE_POS_X &&
                    controller_type <= CONTROLLER_TYPE_POS_Z)
                {
                    // 统计坐标
                    temp.Clear();
                    if(controller.GetKeyInfo(NULL, time, temp))
                    {
                        object_data.fPosInfo[controller_type - CONTROLLER_TYPE_POS_X] = temp.FloatVal(1);
                        bPosKey = true;
                    }
                }

                ++ite_ctrl;
            }

            if (object_data.bLocalPosition)
            {
                object_data.fPosInfo[0] += object_data.fOriginX;
                object_data.fPosInfo[1] += object_data.fOriginY;
                object_data.fPosInfo[2] += object_data.fOriginZ;
            }
#endif
            ++ite_obj;
			continue;
		}

        FmVec3*	pPosition = (FmVec3*)object_data.fPosInfo;
        FmVec3*	pAngle = (FmVec3*)(object_data.fPosInfo+3);

        bool bActor = pObject->GetEntInfo()->IsKindOf("Actor2");
        bool bCameraWrapper = pObject->GetEntInfo()->IsKindOf("CameraWrapper");

		param.Clear();
		param<< pObject->GetID()<< time;

		// 统计坐标和角度关键帧准备
		memset(object_data.fPosInfo, 0, sizeof(object_data.fPosInfo));
		bPosKey = false;

		// 统计震动相关的关键帧准备
		memset(fWobbleInfo, 0, sizeof(fWobbleInfo));
		bWobbleKey = false;

		KeyFrameControllers& controllers = ite_obj.GetData()->controllers;
		KeyFrameControllers::iterator ite_ctrl = controllers.Begin();
		while (ite_ctrl != controllers.End())
		{
			KeyFrameController& controller = *ite_ctrl.GetData();
			int controller_type = controller.GetControllerType();

			if (controller_type >= CONTROLLER_TYPE_POS_X &&
				controller_type <= CONTROLLER_TYPE_ANGLE_Z)
			{
				// 统计坐标和角度关键帧
				temp.Clear();
				if(controller.GetKeyInfo(pObject, time, temp))
				{
					object_data.fPosInfo[controller_type - CONTROLLER_TYPE_POS_X] = temp.FloatVal(1);
					bPosKey = true;
				}
			}
			else if(controller_type >= CONTROLLER_TYPE_WOBBLE_X &&
				controller_type <= CONTROLLER_TYPE_WOBBLE_RZ)
			{
				// 统计震动相关的关键帧
				temp.Clear();
				if(controller.GetKeyInfo(pObject, time, temp))
				{
					fWobbleInfo[controller_type - CONTROLLER_TYPE_WOBBLE_X] = temp.FloatVal(1);
					bWobbleKey = true;
				}

				if (controller.GetBegin() < fMinKey)
				{
					fMinKey = controller.GetBegin();
				}
				if (controller.GetEnd() > fMaxKey)
				{
					fMaxKey = controller.GetEnd();
				}
			}
			else
			{
				// 载入关键帧数据,准备发送
				controller.GetKeyInfo(pObject, time, param);
			}

			++ite_ctrl;
		}

		if (!bPosKey && object_data.bRotation)
		{
			// 帧对max对象没有关键帧,也刷新位置.
			*pPosition = object_data.initPos;
			bPosKey = true;
		}

		if (time <= fMinKey || time >= fMaxKey)
		{
			bWobbleKey = false;
		}

		if (bPosKey)
		{
			bool b_need_rotation = 
				(object_data.bLocalPosition && object_data.bRotation);
			bool b_target_camera = (bCameraWrapper && 
				((CameraWrapper *)pObject)->IsTarget());
			
			if (b_target_camera)
			{
				((CameraWrapper *)pObject)->UpdateTargetCameraKey(pPosition,
					pAngle, object_data, b_need_rotation);
			}			
			else if (b_need_rotation)
			{
				if (bActor)
				{
					// 给模型翻转180
					pAngle->y += FM_PIf;
					pAngle->x = -pAngle->x;
					pAngle->z = -pAngle->z;
				}

				RotationKey(pPosition, pAngle, object_data);
			}

			if (object_data.bLocalPosition)
			{
				pPosition->x += object_data.fOriginX;
				pPosition->y += object_data.fOriginY;
				pPosition->z += object_data.fOriginZ;
			}

			if (bWobbleKey)
			{
				// 震动关键帧的叠加处理
				FmVec3 vPos, vAngle;
					
				GetWobbleInfo(vPos, vAngle, time, fWobbleInfo);

				FmMat4 mat;
				FmMat4RotationYawPitchRoll(&mat, pAngle->y, pAngle->x, pAngle->z);
				FmVec3TransformCoord(&vPos, &vPos, &mat);

				*pPosition += vPos;
				*pAngle += vAngle;
			}

			// 此处之前可以对坐标和角度信息进行特殊处理
			param<< CONTROLLER_TYPE_POSITION_ANGLE<<
				object_data.fPosInfo[0]<< object_data.fPosInfo[1]<< object_data.fPosInfo[2]<< 
                object_data.fPosInfo[3]<< object_data.fPosInfo[4]<< object_data.fPosInfo[5];

            if (bCameraWrapper)
            {
                ((CameraWrapper *)pObject)->SetPosition(object_data.fPosInfo[0], object_data.fPosInfo[1], object_data.fPosInfo[2]);
                if (((CameraWrapper *)pObject)->GetRotateAble())
                {
                    ((CameraWrapper *)pObject)->SetAngle(object_data.fPosInfo[3], object_data.fPosInfo[4], object_data.fPosInfo[5]);
                }
            }
            else
            {
                pObject->SetPosition(object_data.fPosInfo[0], object_data.fPosInfo[1], object_data.fPosInfo[2]);
                pObject->SetAngle(object_data.fPosInfo[3], object_data.fPosInfo[4], object_data.fPosInfo[5]);
            }
		}

		if (param.GetCount() > 2)
		{
			//有帧数据才发送。
			fm_movie::TimeAxis_RunCallback(this, "on_execute_frame", param);
		}

		++ite_obj;
	}
}

bool TimeAxis::AddBezierKey(const char* object_name, int controller_type, float time, float value)
{
	sObjectData* pObjectData = InnerGetObjectData(object_name);
	// 如果还没有这个对象
	if (NULL == pObjectData)
	{
		//Assert(0);
		return false;
	}

	KeyFrameController* pController = GetController(*pObjectData, controller_type);

	// 如果还没有这种控制器则添加
	if (NULL == pController)
	{
		pController = AddController(*pObjectData, controller_type, 
			CONTROLLER_USAGE_MODE_BEZIER);
		if (NULL == pController)
		{
			CORE_TRACE("(TimeAxis::SetKeyFrameBezierMode) Create Controller failed.");
			return false;
		}
	}

	pObjectData->bDataChanged = true;

	pController->AddBezierKey(time, value);

	return true;
}

void TimeAxis::GetFrontBezierKey(const IVarList &param, IVarList &result)
{
	size_t count = param.GetCount();
	if (count != 3)
	{
		result << -1 << 0;
		return;
	}

	const char *object_name = param.StringVal(0);
	int controller_type = param.IntVal(1);
	float value_x = param.FloatVal(2);
	float value_y = 0;
	sObjectData* pObjectData = InnerGetObjectData(object_name);
	// 如果还没有这个对象
	if (NULL == pObjectData)
	{
		result << -1 << value_y;
		return;
	}

	KeyFrameController* pController = GetController(*pObjectData, controller_type);
	// 如果还没有这种控制器则添加
	if (NULL == pController)
	{
		result << -1 << value_y;
		return;
	}

	int index = pController->GetFrontBezierKey(value_x, value_y);
	result<< index << value_y;
}


typedef TPodHashMap<void*, int> InfoMap;
void TimeAxis::GetAdjacencyBezierKeyData(const IVarList &args, IVarList &result) const
{

	const char* strObjectID = args.StringVal(0);
	const sObjectData* pObjectData = InnerGetObjectData(strObjectID);
	if (NULL == pObjectData)
	{
		return;
	}

	int args_count = (int)args.GetCount();
	if (args_count < 2)
	{
		return;
	}

	// 传入的参数格式必须按照"控制器类型、关键帧时间"
	div_t result_div = div(args_count, 2);
	if (result_div.rem != 0)
	{
		return;
	}

	// 临近帧数据暂存
	CVarList array_frame_buf;
	// 邻接帧
	InfoMap adjacencyMap;
	// 关键帧
	InfoMap keyMap;

	for(int i=1; i < args_count; i += 2)
	{
		// 要取临近帧的关键帧信息
		int nControllerType = args.IntVal(i);
		float fKeyFrameTime = args.FloatVal(i+1);

		// 获取临近帧信息
		const KeyFrameController* pController = GetController(*pObjectData, nControllerType);
		if (NULL == pController)
		{
			continue;
		}

		if (CONTROLLER_USAGE_MODE_BEZIER != pController->GetUsageMode())
		{
			continue;
		}

		int nIndex = -1;
		void* pKeyFrame = (void*)pController->GetKeyFrameByTime(fKeyFrameTime, &nIndex);
		if (NULL == pKeyFrame)
		{
			continue;
		}

		// 插入关键帧
		InfoMap::iterator it_key_frame = keyMap.Find(pKeyFrame);
		if (it_key_frame != keyMap.End())
		{
			continue;
		}
		keyMap.Add(pKeyFrame, nIndex);


		// 要取临近帧的关键帧信息
		array_frame_buf.Clear();
		pController->GetAdjacencyBezierKey(fKeyFrameTime, array_frame_buf);
		
		size_t nAdFrameCount = array_frame_buf.GetCount();
		for (size_t j=0; j < nAdFrameCount; ++j)
		{
			void* pAdFrame = array_frame_buf.PointerVal(j);

			// 插入邻接点
			InfoMap::iterator it_ad_frame = adjacencyMap.Find(pAdFrame);
			if (it_ad_frame != adjacencyMap.End())
			{
				continue;
			}
			adjacencyMap.Add(pAdFrame, nControllerType);
		}
	}

	// 统计返回数据
	InfoMap::iterator it = adjacencyMap.Begin();
	InfoMap::iterator it_end = adjacencyMap.End();
	for (; it != it_end; ++it)
	{
		int controller_type = it.GetData();
		sKey* pKey = (sKey*)it.GetKey();
		InfoMap::iterator it_is_key = keyMap.Find(pKey);

		if (it_is_key != keyMap.End())
		{
			continue;
		}

		// 临近帧控制器类型
		result.AddInt(controller_type);
		// 临近帧帧数
		result.AddFloat(pKey->time);
	}
}


void TimeAxis::ChangeComputeTypeKey(const char* object_name, int controller_type, float time, int type)
{
	sObjectData* pObjectData = InnerGetObjectData(object_name);
	// 如果还没有这个对象
	if (NULL == pObjectData)
	{
		//Assert(0);
		return;
	}

	KeyFrameController* pController = GetController(*pObjectData, controller_type);
	if (NULL == pController)
	{
		return;
	}

	pController->ChangeComputeTypeKey(time, type);
}

void TimeAxis::CreateAllControlPoints(const char* object_name, int controller_type)
{
	sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return;
	}

	KeyFrameController* pController = GetController(*pObjectData, controller_type);
	if (NULL == pController)
	{
		return;
	}

	pController->CreateAllControlPoints();
}

void TimeAxis::MoveKeyTimes(const char* object_name, float time_add)
{
	sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return;
	}

	KeyFrameControllers::iterator ite_ctrl = pObjectData->controllers.Begin();
	while (ite_ctrl != pObjectData->controllers.End())
	{

		KeyFrameController* pController = ite_ctrl.GetData();
		if (NULL == pController)
		{
			++ite_ctrl;
			continue;
		}

		pController->MoveAllKeyTime(time_add);

		++ite_ctrl;
	}

}

void TimeAxis::MoveControllerKeyTimes(const char* object_name, int controller_type, float time_add)
{

	sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return;
	}

	KeyFrameController* pController = GetController(*pObjectData, controller_type);
	if (NULL == pController)
	{
		return;
	}

	pController->MoveAllKeyTime(time_add);
}

void TimeAxis::MoveAllKeyValue(const char* object_name, int controller_type, float value_add)
{

	sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return;
	}

	KeyFrameController* pController = GetController(*pObjectData, controller_type);
	if (NULL == pController)
	{
		return;
	}

	pController->MoveAllKeyValue(value_add);
}

void TimeAxis::MoveKey(const char* object_name, float time, float new_time)
{
	sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return;
	}

	KeyFrameControllers::iterator ite_ctrl = pObjectData->controllers.Begin();
	while (ite_ctrl != pObjectData->controllers.End())
	{

		KeyFrameController* pController = ite_ctrl.GetData();
		if (NULL == pController)
		{
			++ite_ctrl;
			continue;
		}

		pController->MoveKey(time, new_time);
		++ite_ctrl;
	}
}

void TimeAxis::MoveControllerKey(const char* object_name, int controller_type, float time, float new_time)
{
	sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return;
	}

	KeyFrameController* pController = GetController(*pObjectData, controller_type);
	if (NULL == pController)
	{
		return;
	}

	pController->MoveKey(time, new_time);
}

bool TimeAxis::SetKeyFrameBezierMode(const char* object_name, int controller_type, 
						   float time, float value, 
						   float left_time, float left_value,
						   float right_time, float right_value)
{
	sObjectData* pObjectData = InnerGetObjectData(object_name);
	// 如果还没有这个对象则添加
	if (NULL == pObjectData)
	{
		Assert(0);
		pObjectData = InnerAddObjectData(object_name);
	}

	KeyFrameController* pController = GetController(*pObjectData, controller_type);

	// 如果还没有这种控制器则添加
	if (NULL == pController)
	{
		pController = AddController(*pObjectData, controller_type, 
			CONTROLLER_USAGE_MODE_BEZIER);
		if (NULL == pController)
		{
			CORE_TRACE("(TimeAxis::SetKeyFrameBezierMode) Create Controller failed.");
			return false;
		}
	}

	pObjectData->bDataChanged = true;

	return pController->SetKeyFrameBezierMode(time, value, left_time, 
		left_value, right_time, right_value);
}

bool TimeAxis::SetKeyFrameLinearMode(const char* object_name, int controller_type, 
						   float time, float value)
{
	sObjectData* pObjectData = InnerGetObjectData(object_name);
	// 如果还没有这个对象则添加
	if (NULL == pObjectData)
	{
		Assert(0);
		pObjectData = InnerAddObjectData(object_name);
	}

	KeyFrameController* pController = GetController(*pObjectData, controller_type);

	// 如果还没有这种控制器则添加
	if (NULL == pController)
	{
		pController = AddController(*pObjectData, controller_type, 
			CONTROLLER_USAGE_MODE_LINEAE);
		if (NULL == pController)
		{
			CORE_TRACE("(TimeAxis::SetKeyFrameLinearMode) Create Controller failed.");
			return false;
		}
	}

	pObjectData->bDataChanged = true;

	return pController->SetKeyFrameLinearMode(time, value);
}

bool TimeAxis::SetKeyFrameOnceMode(const char* object_name, int controller_type, 
						 float time, const char* param)
{
	sObjectData* pObjectData = InnerGetObjectData(object_name);
	// 如果还没有这个对象则添加
	if (NULL == pObjectData)
	{
		Assert(0);
		pObjectData = InnerAddObjectData(object_name);
	}

	KeyFrameController* pController = GetController(*pObjectData, controller_type);

	// 如果还没有这种控制器则添加
	if (NULL == pController)
	{
		pController = AddController(*pObjectData, controller_type, 
			CONTROLLER_USAGE_MODE_ONCE);
		if (NULL == pController)
		{
			CORE_TRACE("(TimeAxis::SetKeyFrameOnceMode) Create Controller failed.");
			return false;
		}
	}

	pObjectData->bDataChanged = true;

	return pController->SetKeyFrameOnceMode(time, param);
}

bool TimeAxis::SetKeyFrameScaleMode(const char* object_name, int controller_type, 
	float time, const char* action_name, float time_length, float speed)
{
	sObjectData* pObjectData = InnerGetObjectData(object_name);
	// 如果还没有这个对象则添加
	if (NULL == pObjectData)
	{
		Assert(0);
		pObjectData = InnerAddObjectData(object_name);
	}

	KeyFrameController* pController = GetController(*pObjectData, controller_type);

	// 如果还没有这种控制器则添加
	if (NULL == pController)
	{
		pController = AddController(*pObjectData, controller_type, 
			CONTROLLER_USAGE_MODE_SCALE);
		if (NULL == pController)
		{
			CORE_TRACE("(TimeAxis::SetKeyFrameScaleMode) Create Controller failed.");
			return false;
		}
	}

	pObjectData->bDataChanged = true;

	return pController->SetKeyFrameScaleMode(time, action_name, time_length,
		speed);
}

void TimeAxis::DeleteKey(const char* object_name, float time)
{
	sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return;
	}

	KeyFrameControllers::iterator ite_ctrl = pObjectData->controllers.Begin();
	while (ite_ctrl != pObjectData->controllers.End())
	{

		KeyFrameController* pController = ite_ctrl.GetData();
		if (NULL == pController)
		{
			++ite_ctrl;
			continue;
		}

		pController->DeleteControllerKey(time);
		++ite_ctrl;
	}
}

bool TimeAxis::DeleteControllerKey(const char* object_name, int controller_type, 
							  float time)
{
	sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return false;
	}

	KeyFrameControllers& controllers = pObjectData->controllers;
	KeyFrameController *pController = NULL;
	if (!controllers.GetData(controller_type, pController))
	{
		return false;
	}

	pObjectData->bDataChanged = true;

	pController->DeleteControllerKey(time);
	return true;
}

bool TimeAxis::BuildPathPoint(const char* object_name)
{

	// 无效的帧间距
	if (m_fFrameInterval < 0.0001f)
	{
		Assert(0);
		return false;
	}

	sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return false;
	}

	KeyFrameControllers& controllers = pObjectData->controllers;

	// posx, posy, posz控制器必须都存在，否则无法计算轨迹
	KeyFrameController *p_ctrl_pos_x = NULL;
	if (!controllers.GetData(CONTROLLER_TYPE_POS_X, p_ctrl_pos_x))
	{
		//Assert(0);
		return false;
	}
	KeyFrameController *p_ctrl_pos_y = NULL;
	if (!controllers.GetData(CONTROLLER_TYPE_POS_Y, p_ctrl_pos_y))
	{
		//Assert(0);
		return false;
	}
	KeyFrameController *p_ctrl_pos_z = NULL;
	if (!controllers.GetData(CONTROLLER_TYPE_POS_Z, p_ctrl_pos_z))
	{
		//Assert(0);
		return false;
	}

	FmVec3 pos;
	float time;
	bool bNeedRotation = 
		(pObjectData->bLocalPosition && pObjectData->bRotation);

	// 统计所有控制器的所有关键帧时间
	TArrayPod<float, 1> all_key_frame_time;
	p_ctrl_pos_x->CollectPosKeyTime(all_key_frame_time);
	p_ctrl_pos_y->CollectPosKeyTime(all_key_frame_time);
	p_ctrl_pos_z->CollectPosKeyTime(all_key_frame_time);

	// 总关键帧个数
	unsigned int nKeyFrameCount = all_key_frame_time.size();
	if (nKeyFrameCount > 0)
	{
		// 一个关键帧点通过由4条线组成的方块表示,需要8个顶点
		unsigned int nKeyFrameVertexCount = nKeyFrameCount * 8;

		if (pObjectData->nKeyFrameVBufferCount != nKeyFrameVertexCount)
		{
			CORE_FREE(pObjectData->pKeyFrameVBuffer, pObjectData->nKeyFrameVBufferCount * sizeof(vertex_key_frame_t));
			pObjectData->pKeyFrameVBuffer = (vertex_key_frame_t*)CORE_ALLOC(nKeyFrameVertexCount * sizeof(vertex_key_frame_t));
			pObjectData->nKeyFrameVBufferCount = nKeyFrameVertexCount;

            if (m_keyFrameVBufferSize < nKeyFrameVertexCount * sizeof(vertex_key_frame_t))
            {
                BatchData* ptr = m_listBatchData;
                while (ptr)
                {
                    if (ptr->pVBuffer == m_pKeyFrameVBuffer)
                    {
                        ptr->pVBuffer = NULL;
                    }
                    ptr = ptr->pNext;
                }

                SAFE_RELEASE(m_pKeyFrameVBuffer);
                m_keyFrameVBufferSize = nKeyFrameVertexCount * sizeof(vertex_key_frame_t);
                m_pKeyFrameVBuffer = s_pRender->CreateDynamicVB(m_keyFrameVBufferSize, "TimeAxis::m_pKeyFrameVBuffer");
            }
		}

        // 设置初始位置
        memset(pObjectData->fPosInfo, 0, sizeof(pObjectData->fPosInfo));
        pObjectData->fPosInfo[0] = p_ctrl_pos_x->GetValueBezierMode(all_key_frame_time[0]);
        pObjectData->fPosInfo[1] = p_ctrl_pos_y->GetValueBezierMode(all_key_frame_time[0]);
        pObjectData->fPosInfo[2] = p_ctrl_pos_z->GetValueBezierMode(all_key_frame_time[0]);
        pObjectData->UpdateCurrentFrame();

		// 更新关键帧顶点数据
		vertex_key_frame_t* pKeyFrame = pObjectData->pKeyFrameVBuffer;
		for (size_t kf = 0; kf < nKeyFrameCount; ++kf)
		{
			float time = all_key_frame_time[kf];
			pos.x = p_ctrl_pos_x->GetValueBezierMode(time);
			pos.y = p_ctrl_pos_y->GetValueBezierMode(time);
			pos.z = p_ctrl_pos_z->GetValueBezierMode(time);
			
			if (bNeedRotation)
			{
				RotationKey(&pos, NULL, *pObjectData);
			}

			// (-1,-1)->(1,-1)
			SET_KEY_FRAME_VERTEX(pKeyFrame[kf*8], pos, 
				-KEY_FRAME_RECT_RADIUS, -KEY_FRAME_RECT_RADIUS);
			SET_KEY_FRAME_VERTEX(pKeyFrame[kf*8+1], pos, 
				KEY_FRAME_RECT_RADIUS, -KEY_FRAME_RECT_RADIUS);
			// (1,-1)->(1,1)
			SET_KEY_FRAME_VERTEX(pKeyFrame[kf*8+2], pos, 
				KEY_FRAME_RECT_RADIUS, -KEY_FRAME_RECT_RADIUS);
			SET_KEY_FRAME_VERTEX(pKeyFrame[kf*8+3], pos, 
				KEY_FRAME_RECT_RADIUS, KEY_FRAME_RECT_RADIUS);
			// (1,1)->(-1,1)
			SET_KEY_FRAME_VERTEX(pKeyFrame[kf*8+4], pos, 
				KEY_FRAME_RECT_RADIUS, KEY_FRAME_RECT_RADIUS);
			SET_KEY_FRAME_VERTEX(pKeyFrame[kf*8+5], pos, 
				-KEY_FRAME_RECT_RADIUS, KEY_FRAME_RECT_RADIUS);
			// (-1,1)->(-1,-1)
			SET_KEY_FRAME_VERTEX(pKeyFrame[kf*8+6], pos, 
				-KEY_FRAME_RECT_RADIUS, KEY_FRAME_RECT_RADIUS);
			SET_KEY_FRAME_VERTEX(pKeyFrame[kf*8+7], pos, 
				-KEY_FRAME_RECT_RADIUS, -KEY_FRAME_RECT_RADIUS);
		}

		// 取出最小的帧时刻和最大的帧时刻
		float begin_time = all_key_frame_time[0];
		float end_time = all_key_frame_time[nKeyFrameCount - 1];

		// 总时间
		float fTotalTime = end_time - begin_time;


		// 总时间不能为0
		if (0.0f >= fTotalTime)
		{
			//Assert(0);
			return false;
		}

		// 组成路径的直线个数
		float f_line_count = fTotalTime / m_fFrameInterval;
		size_t line_count = size_t(f_line_count);

		// 帧个数
		size_t frame_count = line_count + 1;

		// 如果只有一帧则不处理
		if (1 == frame_count)
		{
			Assert(0);
			return false;
		}


		// 填充曲线的线段
		if (pObjectData->nFrameVBufferCount != frame_count)
		{
			CORE_FREE(pObjectData->pFrameVBuffer, pObjectData->nFrameVBufferCount*sizeof(vertex_t));
			pObjectData->pFrameVBuffer = (vertex_t*)CORE_ALLOC(frame_count * sizeof(vertex_t));
			pObjectData->nFrameVBufferCount = frame_count;

            if (m_frameVBufferSize < frame_count * sizeof(vertex_t))
            {
                BatchData* ptr = m_listBatchData;
                while (ptr)
                {
                    if (ptr->pVBuffer == m_pFrameVBuffer)
                    {
                        ptr->pVBuffer = NULL;
                    }
                    ptr = ptr->pNext;
                }

                SAFE_RELEASE(m_pFrameVBuffer);
                m_frameVBufferSize = frame_count * sizeof(vertex_t);
                m_pFrameVBuffer = s_pRender->CreateDynamicVB(m_frameVBufferSize, "TimeAxis::m_pFrameVBuffer");
            }
		}

		// 更新每帧的顶点数据
		pObjectData->nFrameVCountUsed = 0;
		vertex_t* pLine = pObjectData->pFrameVBuffer;
		for (size_t f = 0; f < frame_count; ++f)
		{
			time = begin_time + f * m_fFrameInterval;
			pos.x = p_ctrl_pos_x->GetValueBezierMode(time);
			pos.y = p_ctrl_pos_y->GetValueBezierMode(time);
			pos.z = p_ctrl_pos_z->GetValueBezierMode(time);

			if (bNeedRotation)
			{
				RotationKey(&pos, NULL, *pObjectData);
			}

			if (f != 0)
			{
				// 去掉了坐标一样的帧顶点
				if (pos == pLine[pObjectData->nFrameVCountUsed - 1].pos)
				{
					continue;
				}
			}

			pLine[pObjectData->nFrameVCountUsed].pos = pos;
			++pObjectData->nFrameVCountUsed;
		}

#ifdef _DEBUG
		pObjectData->vFrameVBufferUsedDebug.resize(pObjectData->nFrameVCountUsed);
		memcpy(&pObjectData->vFrameVBufferUsedDebug[0], pLine, sizeof(vertex_t)*pObjectData->nFrameVCountUsed);
#endif

        m_fCurTime = 0.0f;
        m_fBeginTime = 0.0f;
        m_fEndTime = fTotalTime;
	}
	else
	{
		CORE_FREE(pObjectData->pKeyFrameVBuffer, pObjectData->nKeyFrameVBufferCount * sizeof(vertex_key_frame_t));	
		CORE_FREE(pObjectData->pFrameVBuffer, pObjectData->nFrameVBufferCount*sizeof(vertex_t));

#ifdef _DEBUG
		pObjectData->vFrameVBufferUsedDebug.clear();
#endif
		pObjectData->nKeyFrameVBufferCount = 0;
		pObjectData->nFrameVBufferCount = 0;
		pObjectData->nFrameVCountUsed = 0;
	}

	return true;
}

void sObjectData::UpdateCurrentFrame()
{
    FmVec3 pos(fPosInfo[0], fPosInfo[1], fPosInfo[2]);

    // (-1,-1)->(1,-1)
    SET_KEY_FRAME_VERTEX(currentFrame[0], pos, 
        -KEY_FRAME_RECT_RADIUS, -KEY_FRAME_RECT_RADIUS);
    SET_KEY_FRAME_VERTEX(currentFrame[1], pos, 
        KEY_FRAME_RECT_RADIUS, -KEY_FRAME_RECT_RADIUS);
    // (1,-1)->(1,1)
    SET_KEY_FRAME_VERTEX(currentFrame[2], pos, 
        KEY_FRAME_RECT_RADIUS, -KEY_FRAME_RECT_RADIUS);
    SET_KEY_FRAME_VERTEX(currentFrame[3], pos, 
        KEY_FRAME_RECT_RADIUS, KEY_FRAME_RECT_RADIUS);
    // (1,1)->(-1,1)
    SET_KEY_FRAME_VERTEX(currentFrame[4], pos, 
        KEY_FRAME_RECT_RADIUS, KEY_FRAME_RECT_RADIUS);
    SET_KEY_FRAME_VERTEX(currentFrame[5], pos, 
        -KEY_FRAME_RECT_RADIUS, KEY_FRAME_RECT_RADIUS);
    // (-1,1)->(-1,-1)
    SET_KEY_FRAME_VERTEX(currentFrame[6], pos, 
        -KEY_FRAME_RECT_RADIUS, KEY_FRAME_RECT_RADIUS);
    SET_KEY_FRAME_VERTEX(currentFrame[7], pos, 
        -KEY_FRAME_RECT_RADIUS, -KEY_FRAME_RECT_RADIUS);
}

void TimeAxis::UpdateBatchData(BatchData* pBatchData, bool keyFrame, const sObjectData& objectData)
{
    if (keyFrame)
    {
        pBatchData->pShader = m_pShaderKeyFrame;
        pBatchData->pVBuffer = m_pKeyFrameVBuffer;

        pBatchData->hMatWVP = m_hKeyMatWVP;
        pBatchData->hDiffuse = m_hKeyDiffuse;
        pBatchData->hTexelSize = m_hKeyTexelSize;

        pBatchData->pVData = objectData.pKeyFrameVBuffer;
        pBatchData->vSize = objectData.nKeyFrameVBufferCount * sizeof(vertex_key_frame_t);
    }
    else
    {
        pBatchData->pShader = m_pShaderFrame;
        pBatchData->pVBuffer = m_pFrameVBuffer;

        pBatchData->hMatWVP = m_hMatWVP;
        pBatchData->hDiffuse = m_hDiffuse;
        pBatchData->hTexelSize = -1;

        pBatchData->pVData = objectData.pFrameVBuffer;
        pBatchData->vSize = objectData.nFrameVCountUsed * sizeof(vertex_t);
    }
}

TimeAxis::BatchData* TimeAxis::GetBatchData()
{
    if (!m_listBatchData)
    {
        m_listBatchData = CORE_NEW(BatchData);
        m_nBatchCount = 1;
        return m_listBatchData;
    }

    BatchData* ptr = m_listBatchData;
    while (true)
    {
        if (ptr->pShader == 0)
        {
            return ptr;
        }

        if (ptr->pNext)
        {
            ptr = ptr->pNext;
        }
        else
        {
            ptr->pNext = CORE_NEW(BatchData);
            m_nBatchCount++;
            return ptr->pNext;
        }
    }
}

void TimeAxis::RenderBatch(void* pdata)
{
    BatchData* pthis = (BatchData*)pdata;
    if (!pthis->pShader)
    {
        return;
    }    

    // 更新顶点数据
    s_pRender->UpdateDynamicVB(pthis->pVBuffer, 0, 
        pthis->pVData, pthis->vSize);

    pthis->pShader->UsedShader();

    IShaderParamOp* pShaderParamOp = pthis->pShader->GetParamOp();
    pShaderParamOp->SetParamValue(pthis->hDiffuse,pthis->c_vDiffuse,false);

    pShaderParamOp->SetParamValue(pthis->hMatWVP,pthis->c_matWVP,false);
    if (pthis->hTexelSize != -1)
    {
        pShaderParamOp->SetParamValue(pthis->hTexelSize, pthis->c_vTexelSize, false);
    }

    IRenderDrawOp*  pDrawOp = pthis->pShader->GetRenderDrawOp();
    pDrawOp->SetVB(pthis->pVBuffer->GetBuffer());
    pDrawOp->SetIB(0);

    if (pthis->hTexelSize != -1)
    {
        pDrawOp->EnableVertexAttribArray(0, true);
        pDrawOp->SetVertexAttribPointer(0, 3, IRenderDrawOp::VERTEX_DATA_FLOAT, sizeof(vertex_key_frame_t), 0);
        pDrawOp->EnableVertexAttribArray(1, true);
        pDrawOp->SetVertexAttribPointer(1, 2, IRenderDrawOp::VERTEX_DATA_FLOAT, sizeof(vertex_key_frame_t), (void*)(3 * sizeof(float)));
    }
    else
    {
        pDrawOp->EnableVertexAttribArray(0, true);
        pDrawOp->SetVertexAttribPointer(0, 3, IRenderDrawOp::VERTEX_DATA_FLOAT, sizeof(vertex_t), 0);
    }

    // 渲染
    pDrawOp->DrawArrays(pthis->drawMode, 0, pthis->primitiveCount);

	pthis->pShader = NULL;
}

void TimeAxis::Realize()
{
	if (!GetVisible())
	{
		return;
	}

	IRenderContext* pContext = s_pRender->GetContext();
    int view_width = m_pContext->GetViewWidth();
    int view_height = m_pContext->GetViewHeight();
	float texel_size[4] = { 1.0f / float(view_width), 
		1.0f / float(view_height), 0.0f, 0.0f };

	FmMat4 matWorld;
	Objects::iterator ite_obj = m_objects.Begin();
	while (m_objects.End() != ite_obj)
	{
		sObjectData& object_data = *ite_obj.GetData();

		// 显示路径
		if (object_data.bShowPath)
		{
			const FmVec3& refer_pos = pContext->GetReferPosition();
			FmMatrixIdentity(&matWorld);

			matWorld._41 = object_data.fOriginX - refer_pos.x;
			matWorld._42 = object_data.fOriginY - refer_pos.y;
			matWorld._43 = object_data.fOriginZ - refer_pos.z;

			FmMat4 mtxWVP;
			FmMat4Multiply(&mtxWVP, &matWorld, 
                &pContext->GetCamera().mtxViewProjRelative);
            FmMat4Transpose(&mtxWVP, &mtxWVP);

			if (object_data.nFrameVCountUsed > 0)
			{
				if (object_data.nFrameVCountUsed > 1)
                {
					// 绘制帧间的线段
                    BatchData* pBatchData = GetBatchData();
                    UpdateBatchData(pBatchData, false, object_data);
                    memcpy(&pBatchData->c_matWVP, &mtxWVP, sizeof(FmMat4));
                    // 红色
                    memcpy(&pBatchData->c_vDiffuse, color_red, sizeof(FmVec4));
                    pBatchData->drawMode = IRenderDrawOp::DRAW_LINE_STRIP;
                    pBatchData->primitiveCount = object_data.nFrameVCountUsed;
                    s_pRender->GetSceneView()->AddSolidBatch(RenderBatch, pBatchData, "TimeAxis::Realize::LineStrip");

                    /*
					// 绘制帧间的线段
					DxSceneBatch* pBatchLine = s_pRender->AddBlendBatch("TimeAxis_Line");

					pBatchLine->SetShader(pShader);

					pBatchLine->SetVertexShaderConstantF(0, (float*)&mtxWVP, 4);
					// 红色
					pBatchLine->SetVertexShaderConstantF(4, color_red, 1);

					pBatchLine->SetFVF(D3DFVF_XYZ);
					//pBatchLine->SetTexture(0, NULL);
					pBatchLine->SetStreamSource(0, object_data.pFrameVBuffer->GetDxVertexBuffer(), 
						0, sizeof(vertex_t));

					pBatchLine->SetPrimitive(D3DPT_LINESTRIP, object_data.nFrameVCountUsed, 
						object_data.nFrameVCountUsed - 1);
                    */
				}

                BatchData* pBatchData = GetBatchData();
                UpdateBatchData(pBatchData, false, object_data);
                memcpy(&pBatchData->c_matWVP, &mtxWVP, sizeof(FmMat4));
				// 白色
                memcpy(&pBatchData->c_vDiffuse, color_white, sizeof(FmVec4));
                pBatchData->drawMode = IRenderDrawOp::DRAW_POINTS;
                pBatchData->primitiveCount = object_data.nFrameVCountUsed;
                s_pRender->GetSceneView()->AddSolidBatch(RenderBatch, pBatchData, "TimeAxis::Realize::Points");
                /*
				// 绘制帧顶点
				DxSceneBatch* pBatchPoint = s_pRender->AddTopmostBatch("TimeAxis_Point");

				pBatchPoint->SetShader(pShader);

				pBatchPoint->SetVertexShaderConstantF(0, (float*)&mtxWVP, 4);

				// 白色
				pBatchPoint->SetVertexShaderConstantF(4, color_white, 1);

				pBatchPoint->SetFVF(D3DFVF_XYZ);
				//pBatchPoint->SetTexture(0, NULL);
				pBatchPoint->SetStreamSource(0, object_data.pFrameVBuffer->GetDxVertexBuffer(), 
					0, sizeof(vertex_t));

				pBatchPoint->SetPrimitive(D3DPT_POINTLIST, object_data.nFrameVCountUsed, 
					object_data.nFrameVCountUsed);
				*/
			}

			if (object_data.nKeyFrameVBufferCount > 1)
            {
				// 绘制关键帧方块
                BatchData* pBatchData = GetBatchData();
                UpdateBatchData(pBatchData, true, object_data);
                memcpy(&pBatchData->c_matWVP, &mtxWVP, sizeof(FmMat4));
                // 红色
                memcpy(&pBatchData->c_vDiffuse, color_red, sizeof(FmVec4));
                memcpy(&pBatchData->c_vTexelSize, texel_size, sizeof(FmVec4));
                pBatchData->drawMode = IRenderDrawOp::DRAW_LINES;
                pBatchData->primitiveCount = object_data.nKeyFrameVBufferCount;
                s_pRender->GetSceneView()->AddSolidBatch(RenderBatch, pBatchData, "TimeAxis::Realize::Lines");

                /*
				// 绘制关键帧方块
				DxSceneBatch* pBatchRect = s_pRender->AddTopmostBatch("TimeAxis_Rect");
				pBatchRect->SetVertexShaderConstantF(0, (float*)&mtxWVP, 4);
				// 红色
				pBatchRect->SetVertexShaderConstantF(4, color_red, 1);
				pBatchRect->SetVertexShaderConstantF(5, texel_size, 1);

				pBatchRect->SetShader(pShaderKeyFrame);

				pBatchRect->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
				//pBatchRect->SetTexture(0, NULL);
				pBatchRect->SetStreamSource(0, object_data.pKeyFrameVBuffer->GetDxVertexBuffer(), 
					0, sizeof(vertex_key_frame_t));
				pBatchRect->SetPrimitive(D3DPT_LINELIST, object_data.nKeyFrameVBufferCount, 
					object_data.nKeyFrameVBufferCount/2);
                */

                // 画当前帧位置方块
                object_data.UpdateCurrentFrame();
                pBatchData = GetBatchData();

                pBatchData->pShader = m_pShaderKeyFrame;
                pBatchData->pVBuffer = m_pKeyFrameVBuffer;

                pBatchData->hMatWVP = m_hKeyMatWVP;
                pBatchData->hDiffuse = m_hKeyDiffuse;
                pBatchData->hTexelSize = m_hKeyTexelSize;

                pBatchData->pVData = object_data.currentFrame;
                pBatchData->vSize = 8 * sizeof(vertex_key_frame_t);

                memcpy(&pBatchData->c_matWVP, &mtxWVP, sizeof(FmMat4));
                // 绿色
                memcpy(&pBatchData->c_vDiffuse, color_green, sizeof(FmVec4));
                memcpy(&pBatchData->c_vTexelSize, texel_size, sizeof(FmVec4));
                pBatchData->drawMode = IRenderDrawOp::DRAW_LINES;
                pBatchData->primitiveCount = 8;
                s_pRender->GetSceneView()->AddSolidBatch(RenderBatch, pBatchData, "TimeAxis::Realize");
			}
		}

		++ite_obj;
	}
}

void TimeAxis::SetPathVisible(const char* object_name, bool bVisible)
{
	sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return;
	}

	pObjectData->bShowPath = bVisible;
}

bool TimeAxis::GetPathVisible(const char* object_name) const
{
	const sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return false;
	}

	return pObjectData->bShowPath;
}

sObjectData* TimeAxis::InnerGetObjectData(const char* object_name)
{
	Objects::iterator ite_obj = m_objects.Find(object_name);
	if (ite_obj == m_objects.End())
	{
		return NULL;
	}
	return ite_obj.GetData();
}

const sObjectData* TimeAxis::InnerGetObjectData(const char* object_name) const
{
	Objects::const_iterator ite_obj = m_objects.Find(object_name);
	if (ite_obj == m_objects.End())
	{
		return NULL;
	}

	return ite_obj.GetData();
}

sObjectData* TimeAxis::InnerAddObjectData(const char* object_name)
{
	sObjectData* pObjectData = InnerAddObjectDataNoInitCtrl(object_name);
	if (NULL == pObjectData)
	{
		return NULL;
	}

	return pObjectData;
}

sObjectData* TimeAxis::InnerAddObjectDataNoInitCtrl(const char* object_name)
{
	// 如果已经有了则删除
	DeleteObject(object_name);

	sObjectData* pObjectData = CORE_NEW(sObjectData);
	if (NULL == pObjectData)
	{
		Assert(0);
		return NULL;
	}

	pObjectData->strName = object_name;
	m_objects.Set(object_name, pObjectData);

	//发送增加关键帧消息
	fm_movie::TimeAxis_RunCallback(this, "on_add_object", CVarList()<< object_name);
	return pObjectData;
}

bool TimeAxis::GetLoadFinish()
{
	size_t count = m_objects.GetCount();
	Objects::iterator item = m_objects.Begin();
	m_nLoadCount = 0;

	while(item != m_objects.End())
	{

		sObjectData *pObject = item.GetData();
		if (NULL != pObject->pLoader)
		{
			++item;
			continue;
		}

		IVisBase *pVisBase = GetVisBase(pObject->ObjectID);
		if (pVisBase && !pVisBase->GetLoadFinish())
		{
			++item;
			continue;
		}

		++item;
		++m_nLoadCount;
	}

	return (m_nLoadCount == count);
}

size_t TimeAxis::GetLoadCount() const
{
	return m_nLoadCount;
}

bool TimeAxis::AddObject(const char* object_name)
{
	return InnerAddObjectData(object_name) != NULL;
}

bool TimeAxis::DeleteObject(const char* object_name)
{
	Objects::const_iterator ite_obj = m_objects.Find(object_name);
	if (ite_obj != m_objects.End())
	{
		sObjectData *pObject = ite_obj.GetData();
		SAFE_RELEASE(pObject->pLoader);
		CORE_DELETE(pObject);
		m_objects.Erase(ite_obj);
	}

	return true;
}

bool TimeAxis::IsObjectExist(const char* object_name)
{
	return InnerGetObjectData(object_name) != NULL;
}

bool TimeAxis::CopyObjectData(const char* src_object_name, const char* dst_object_name)
{
	sObjectData* pSrcObjectData = InnerGetObjectData(src_object_name);
	if (NULL == pSrcObjectData)
	{
		CORE_TRACE("(TimeAxis::CopyObjectData) Invalid source object name:");
		CORE_TRACE(src_object_name);
		return false;
	}

	sObjectData* pDstObjectData = InnerGetObjectData(dst_object_name);
	if (NULL == pDstObjectData)
	{
		CORE_TRACE("(TimeAxis::CopyObjectData) Invalid destination object name:");
		CORE_TRACE(dst_object_name);
		return false;
	}

	pDstObjectData->bDataChanged = true;
	pDstObjectData->bShowPath = pSrcObjectData->bShowPath;
	KeyFrameControllers::const_iterator ite_ctrl = pSrcObjectData->controllers.Begin();
	while (ite_ctrl != pSrcObjectData->controllers.End())
	{
		KeyFrameController *pController = NULL;
		pDstObjectData->controllers.GetData(ite_ctrl.GetKey(),pController);
		if ( NULL == pController)
		{
			pController = CORE_NEW(KeyFrameController);
			//new (pController) KeyFrameController;
			pDstObjectData->controllers.Add(ite_ctrl.GetKey(), pController);

			//发送增加控制器的消息
			fm_movie::TimeAxis_RunCallback(this, "on_add_controller", CVarList()<< dst_object_name << ite_ctrl.GetKey());

		}

		*pController = *(ite_ctrl.GetData());	
		++ite_ctrl;
	}

	pDstObjectData->bLocalPosition = pSrcObjectData->bLocalPosition;
	pDstObjectData->fOriginX = pSrcObjectData->fOriginX;
	pDstObjectData->fOriginY = pSrcObjectData->fOriginY;
	pDstObjectData->fOriginZ = pSrcObjectData->fOriginZ;
	pDstObjectData->initPos = pSrcObjectData->initPos;

	pDstObjectData->bRotation = pSrcObjectData->bRotation;
	pDstObjectData->fRAddY = pSrcObjectData->fRAddY;
	pDstObjectData->fRotationX = pSrcObjectData->fRotationX;
	pDstObjectData->fRotationY = pSrcObjectData->fRotationY;
	pDstObjectData->fRotationZ = pSrcObjectData->fRotationZ;
	pDstObjectData->strExtProp = pSrcObjectData->strExtProp;

	BuildPathPoint(dst_object_name);
	return true;
}

bool TimeAxis::ReName(const char* src_object_name, const char* new_object_name)
{
	sObjectData* pSrcObjectData = InnerGetObjectData(src_object_name);
	if (NULL == pSrcObjectData)
	{
		CORE_TRACE("(TimeAxis::ReName) Invalid source object name:");
		CORE_TRACE(src_object_name);
		return false;
	}

	if (strlen(new_object_name) < 1)
	{
		CORE_TRACE("(TimeAxis::ReName) New name empty");
		return false;
	}

	// 新名字的对象已经存在.
	sObjectData* pNewObjectData = InnerGetObjectData(new_object_name);
	if (NULL != pNewObjectData)
	{
		CORE_TRACE("(TimeAxis::ReName) New object name existed");
		CORE_TRACE(new_object_name);
		return false;
	}

	pSrcObjectData->strName = new_object_name;
	pSrcObjectData->bDataChanged = true;
	m_objects.Remove(src_object_name);
	m_objects.Set(new_object_name, pSrcObjectData);

	return true;
}

bool TimeAxis::CopyControllerData(const char* src_object_name, int controller_type, const char* dst_object_name)
{
	sObjectData* pSrcObjectData = InnerGetObjectData(src_object_name);
	if (NULL == pSrcObjectData)
	{
		CORE_TRACE("(TimeAxis::CopyObjectData) Invalid source object name:");
		CORE_TRACE(src_object_name);
		return false;
	}

	sObjectData* pDstObjectData = InnerGetObjectData(dst_object_name);
	if (NULL == pDstObjectData)
	{
		pDstObjectData = InnerAddObjectData(dst_object_name);
	}

	KeyFrameController *pController = NULL;
	if (!pSrcObjectData->controllers.GetData(controller_type, pController))
	{
		KeyFrameController *pNewController = CORE_NEW(KeyFrameController);
		*pNewController = *pController;

		pDstObjectData->controllers.Remove(controller_type);
		pDstObjectData->controllers.Add(controller_type, pNewController);
		pDstObjectData->bDataChanged = true;

		//发送增加控制器的消息
		fm_movie::TimeAxis_RunCallback(this, "on_add_controller", CVarList()<< dst_object_name << controller_type);
	}

	return false;
}

void  TimeAxis::CopyKeyTime(const char* object_name, float time, float new_time)
{

	sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return;
	}

	KeyFrameControllers::iterator ite_ctrl = pObjectData->controllers.Begin();
	while (ite_ctrl != pObjectData->controllers.End())
	{

		KeyFrameController* pController = ite_ctrl.GetData();
		if (NULL == pController)
		{
			++ite_ctrl;
			continue;
		}

		pController->CopyKey(time, new_time);
		++ite_ctrl;
	}
}

void TimeAxis::CopyControllerKeyTime(const char* object_name, int controller_type,
									 float time, float new_time)
{

	sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return;
	}

	KeyFrameController* pController = GetController(*pObjectData, controller_type);
	if (NULL != pController)
	{
		pController->CopyKey(time, new_time);
	}
}

bool TimeAxis::IsKeyFrameExist(const char* object_name, int controller_type, float frame_time) const
{
	const sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return false;
	}

	const KeyFrameController* pController = GetController(*pObjectData, controller_type);
	if (NULL == pController)
	{
		return false;
	}

	int nIndex = -1;
	const sKey* pKey = pController->GetKeyFrameByTime(frame_time, &nIndex);
	if (NULL == pKey)
	{
		return false;
	}

	return true;
}

bool TimeAxis::SetObjectID(const char* object_name, const PERSISTID& value)
{

	sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return false;
	}

	IVisBase* pObject = GetVisBase(value);
	if (NULL == pObject)
	{
		return false;
	}

	pObjectData->ObjectID = value;

	return true;
}

PERSISTID TimeAxis::GetObjectID(const char* object_name) const
{
	const sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return PERSISTID();
	}
	return pObjectData->ObjectID;
}


void TimeAxis::PositionToLocal(const IVarList& args, IVarList& result) const
{
	Assert(args.GetCount() >= 4);

	result.Clear();
	const char *object_name = args.StringVal(0);
	float x = args.FloatVal(1);
	float y = args.FloatVal(2);
	float z = args.FloatVal(3);
	const sObjectData* pObjectData = InnerGetObjectData(object_name);

	if (NULL != pObjectData)
	{
		// 对坐标进行求逆
		FmVec3 pos(x - pObjectData->fOriginX, y - pObjectData->fOriginY,
			z - pObjectData->fOriginZ);

		if (pObjectData->bRotation)
		{
			// 参照相对目标作旋转处理
			FmMat4	mat_ref;
			FmMatrixIdentity(&mat_ref);
			FmMat4RotationYawPitchRoll(&mat_ref, pObjectData->fRotationY, 
				pObjectData->fRotationX, pObjectData->fRotationZ);

			// 如果是max导入的轨迹,增加y轴180旋转
			FmMat4 mat;
			if(0.f != pObjectData->fRAddY)
			{	
				FmMatrixIdentity(&mat);
				FmMatrixRotationY(&mat, pObjectData->fRAddY);
				mat_ref = mat * mat_ref; 	
			}

			// 求出旋转的逆矩阵
			FmMat4	mat_ref_Inv;
			FmMatrixIdentity(&mat_ref_Inv);
			FmMatrixInverse(&mat_ref_Inv, &mat_ref);

			FmVec3TransformCoord(&pos, &pos, &mat_ref_Inv);

		}

		result<< pos.x << pos.y<< pos.z;
	}
	else
	{
		result<< x<< y<< z;
	}

}
void TimeAxis::AngleToLocal(const IVarList& args, IVarList& result) const
{

	Assert(args.GetCount() >= 4);

	result.Clear();
	const char *object_name = args.StringVal(0);
	float x = args.FloatVal(1);
	float y = args.FloatVal(2);
	float z = args.FloatVal(3);
	const sObjectData* pObjectData = InnerGetObjectData(object_name);

	if (NULL != pObjectData && pObjectData->bRotation)
	{
		// 参照相对目标作旋转处理
		FmMat4	mat_ref;
		FmMatrixIdentity(&mat_ref);
		FmMat4RotationYawPitchRoll(&mat_ref, pObjectData->fRotationY, 
			pObjectData->fRotationX, pObjectData->fRotationZ);


		// 如果是max导入的轨迹,增加y轴180旋转
		FmMat4 mat;
		if(0.f != pObjectData->fRAddY)
		{	
			FmMatrixIdentity(&mat);
			FmMatrixRotationY(&mat, pObjectData->fRAddY);
			mat_ref = mat * mat_ref; 	
		}

		// 求出旋转的逆矩阵
		FmMat4	mat_ref_Inv;
		FmMatrixIdentity(&mat_ref_Inv);
		FmMatrixInverse(&mat_ref_Inv, &mat_ref);


		// 世界旋转矩阵
		FmMatrixIdentity(&mat_ref);
		FmMat4RotationYawPitchRoll(&mat_ref, y, x, z);

		// 求得自己的旋转矩阵
		FmVec3 angle;
		mat = mat_ref * mat_ref_Inv;
		VisUtil_GetMatrixAngle(&angle, &mat);

		result<< angle.x<< angle.y<< angle.z;
	}
	else
	{
		result<< x<< y<< z;
	}

}

void TimeAxis::RotationKey(FmVec3 *pPos, FmVec3 *pAngle, const sObjectData& object_data)
{

	// 参照相对目标作旋转处理.
	FmMat4	mat_ref;
	FmMatrixIdentity(&mat_ref);
	FmMat4RotationYawPitchRoll(&mat_ref, object_data.fRotationY, 
		object_data.fRotationX, object_data.fRotationZ);

	// 是否有增加y轴旋转.
	FmMat4 mat;
	if(0.f != object_data.fRAddY)
	{	
		FmMatrixIdentity(&mat);
		FmMatrixRotationY(&mat, object_data.fRAddY);
		mat_ref = mat * mat_ref; 	
	}

	if (NULL != pPos)
	{
		// 计算旋转后的位移.
		FmVec3TransformCoord(pPos, pPos, &mat_ref);

	}

	if (NULL != pAngle)
	{
		// 计算旋转后的角度.
		FmMatrixIdentity(&mat);
		FmMat4RotationYawPitchRoll(&mat, pAngle->y, pAngle->x, pAngle->z);
		mat_ref = mat * mat_ref;  
		VisUtil_GetMatrixAngle(pAngle, &mat_ref);

	}
}

void TimeAxis::GetWobbleInfo(FmVec3 &pos, FmVec3 &angle, float time,
							 float *fWobbleInfo, size_t nInfoLen)
{
	// 频率*时间 = 当(三角函数)前角度位置
	float radianX = fWobbleInfo[3] * time;

	// 半周期内的index保证一样
	int	indexX = (int)(radianX / (FM_PIf));

	// Y方向的角度进行相位错位
	float radianY = radianX * (1.f + GetRandValue(indexX));
	int	indexY = (int)(radianY / (FM_PIf));

	// Z方向的角度进行相位错位
	float radianZ = radianX * (1.f + GetRandValue(indexY));
	int	indexZ = (int)(radianZ / (FM_PIf));

	pos.x = fWobbleInfo[0] * sinf(radianX) * GetRandValue(indexX);
	pos.y = fWobbleInfo[1] * sinf(radianY) * GetRandValue(indexY);
	pos.z = fWobbleInfo[2] * sinf(radianZ) * GetRandValue(indexZ);

	// 摇晃强度
	angle.x = (pos.y+ pos.z)/20.f * fWobbleInfo[4];
	angle.y = (pos.x+ pos.z)/20.f * fWobbleInfo[5];
	angle.z = (pos.x+ pos.y)/20.f * fWobbleInfo[6];
	
}

float TimeAxis::GetRandValue(int index)
{
	index = abs(index);

	index = index % RAND_NUM;

	return g_wobble_rand[index];
}


void TimeAxis::ClearControllerKeyFrames(const char* object_name, int controller_type)
{
	sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return;
	}

	KeyFrameController* pController = GetController(*pObjectData, 
		controller_type);
	if (NULL == pController)
	{
		return;
	}

	pObjectData->bDataChanged = true;

	pController->ClearKeyFrames();

	// 更新显示内容
	BuildPathPoint(object_name);
}

void TimeAxis::ClearControllers(const char* object_name)
{
	sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return;
	}
	KeyFrameControllers& controllers = pObjectData->controllers;
	KeyFrameControllers::iterator ite_ctrl = controllers.Begin();
	while (ite_ctrl != controllers.End())
	{
		KeyFrameController& controller = *ite_ctrl.GetData();
		controller.ClearKeyFrames();
		++ite_ctrl;
	}

	pObjectData->bDataChanged = true;

	// 更新显示内容
	BuildPathPoint(object_name);
}

void TimeAxis::GetControllerKeyTime(const IVarList& args, IVarList& result) const
{
	result.Clear();
	if (args.GetCount() != 2)
	{
		return;
	}
	const char* object_name = args.StringVal(0);
	int controller_type = args.IntVal(1);

	const sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return;
	}

	const KeyFrameController* pController = GetController(*pObjectData, 
		controller_type);
	if (NULL == pController)
	{
		return;
	}

	pController->GetControllerKeyTime(result);
}

int TimeAxis::GetControllerKeyCount(const char* object_name, int controller_type) const
{
	const sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return 0;
	}

	const KeyFrameController* pController = GetController(*pObjectData, controller_type);
	if (NULL == pController)
	{
		return 0;
	}

	return (int)pController->GetKeyCount();
}

int TimeAxis::GetKeyFrameListBezierMode(const IVarList& args, IVarList& result) const
{
	result.Clear();
	if (args.GetCount() != 2)
	{
		return 0;
	}
	const char* object_name = args.StringVal(0);
	int controller_type = args.IntVal(1);

	const sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return 0;
	}

	const KeyFrameController* pController = GetController(*pObjectData, 
		controller_type);
	if (NULL == pController)
	{
		return 0;
	}

	if (CONTROLLER_USAGE_MODE_BEZIER != pController->GetUsageMode())
	{
		CORE_TRACE("(TimeAxis::GetKeyFrameListBezierMode) The usage mode of controller is not bezier mode.");
		return 0;
	}

	return pController->GetKeyFrameListBezierMode(result);
}

int TimeAxis::GetKeyFrameListOnceMode(const IVarList& args, IVarList& result) const
{
	result.Clear();
	if (args.GetCount() != 2)
	{
		return 0;
	}
	const char* object_name = args.StringVal(0);
	int controller_type = args.IntVal(1);

	const sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return 0;
	}

	const KeyFrameController* pController = GetController(*pObjectData, 
		controller_type);
	if (NULL == pController)
	{
		return 0;
	}

	if (CONTROLLER_USAGE_MODE_ONCE != pController->GetUsageMode())
	{
		CORE_TRACE("(TimeAxis::GetKeyFrameListOnceMode) The usage mode of controller is not once mode.");
		return 0;
	}

	return pController->GetKeyFrameListOnceMode(result);
}

int TimeAxis::GetKeyFrameListLinearMode(const IVarList& args, IVarList& result) const
{
	result.Clear();
	if (args.GetCount() != 2)
	{
		return 0;
	}
	const char* object_name = args.StringVal(0);
	int controller_type = args.IntVal(1);

	const sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return 0;
	}

	const KeyFrameController* pController = GetController(*pObjectData, 
		controller_type);
	if (NULL == pController)
	{
		return 0;
	}

	if (CONTROLLER_USAGE_MODE_LINEAE != pController->GetUsageMode())
	{
		CORE_TRACE("(TimeAxis::GetKeyFrameListLinearMode) The usage mode of controller is not linear mode.");
		return 0;
	}

	return pController->GetKeyFrameListLinearMode(result);
}

int TimeAxis::GetKeyFrameListScaleMode(const IVarList& args, IVarList& result) const
{
	result.Clear();
	if (args.GetCount() != 2)
	{
		return 0;
	}
	const char* object_name = args.StringVal(0);
	int controller_type = args.IntVal(1);

	const sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return 0;
	}

	const KeyFrameController* pController = GetController(*pObjectData, 
		controller_type);
	if (NULL == pController)
	{
		return 0;
	}

	if (CONTROLLER_USAGE_MODE_SCALE != pController->GetUsageMode())
	{
		CORE_TRACE("(TimeAxis::GetKeyFrameListLinearMode) The usage mode of controller is not action mode.");
		return 0;
	}

	return pController->GetKeyFrameListScaleMode(result);
}

int TimeAxis::GetControllers(const IVarList& args, IVarList& result) const
{
	result.Clear();
	if (args.GetCount() != 1)
	{
		return 0;
	}
	const char* object_name = args.StringVal(0);

	const sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return 0;
	}

	KeyFrameControllers* pControllers = (KeyFrameControllers*)&(pObjectData->controllers);
	KeyFrameControllers::const_iterator ite_ctrl = pControllers->Begin();
	while (ite_ctrl != pControllers->End())
	{
		result.AddInt(ite_ctrl.GetKey());
		++ite_ctrl;
	}
	return result.GetCount();
}

float TimeAxis::GetValueBezierMode(const char* object_name, 
						 int controller_type, float time) const
{
	const sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return 0;
	}

	const KeyFrameController* pController = GetController(*pObjectData, controller_type);
	if (NULL == pController)

	{
		return 0;
	}

	if (pController->GetUsageMode() != CONTROLLER_USAGE_MODE_BEZIER)
	{
		return 0;
	}

	return pController->GetValueBezierMode(time);
}
//
//const char* TimeAxis::GetValueOnceMode(const char* object_name, int controller_type, float time) const
//{
//	const sObjectData* pObjectData = InnerGetObjectData(object_name);
//	if (NULL == pObjectData)
//	{
//		return "";
//	}
//
//	const KeyFrameController* pController = GetController(*pObjectData, controller_type);
//	if (NULL == pController)
//	{
//		return "";
//	}
//
//	if (pController->GetUsageMode() != CONTROLLER_USAGE_MODE_ONCE)
//	{
//		return "";
//	}
//	
//	const char* ret = pController->GetValueOnceMode(time);
//	if (NULL == ret)
//	{
//		return "";
//	}
//	return ret;
//}

void TimeAxis::GetKeyFrameValue(const IVarList& args, IVarList& result) const
{
	const char* strObjectID = args.StringVal(0);
	int nControllerType = args.IntVal(1);
	float fFrameTime = args.FloatVal(2);

	const sObjectData* pObjectData = InnerGetObjectData(strObjectID);
	if (NULL == pObjectData)
	{
		return;
	}

	const KeyFrameController* pController = GetController(*pObjectData, nControllerType);
	if (NULL == pController)
	{
		return;
	}

	pController->GetKeyFrameValue(fFrameTime, result);

}

bool TimeAxis::GetIsLocalPosition(const char* object_name) const
{
	const sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return false;
	}

	return pObjectData->bLocalPosition;
}

bool TimeAxis::GetIsRotation(const char* object_name) const
{
	const sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return false;
	}

	return pObjectData->bRotation;
}

float TimeAxis::GetRotationY(const char* object_name) const
{
	const sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return 0;
	}

	if (!pObjectData->bRotation)
	{
		return 0;
	}

	return pObjectData->fRAddY;
}

void TimeAxis::SetRotationY(const char* object_name, float fAngle)
{
	sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return;
	}

	if (!pObjectData->bRotation)
	{
		return;
	}

	pObjectData->fRAddY = fAngle;
	pObjectData->bRotation = (0.f != fAngle || pObjectData->bRotation);

	BuildPathPoint(object_name);
	PauseNow();
}


const char* TimeAxis::GetExtProp(const char* object_name) const
{
	const sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return "";
	}

	return pObjectData->strExtProp.c_str();
}

void TimeAxis::GetLocalOrigin(const IVarList &args, IVarList& result) const
{
	size_t count = args.GetCount();
	if (count == 0)
	{
		return;
	}

	const char* object_name = args.StringVal(0);
	if (NULL == object_name || strlen(object_name) < 1)
	{
		return;
	}

	const sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return;
	}

	result<< pObjectData->fOriginX<< pObjectData->fOriginY<< pObjectData->fOriginZ;

}


void TimeAxis::SetLocalOrigin(const IVarList &args, IVarList& result)
{
	size_t count = args.GetCount();
	if (count == 0)
	{
		return;
	}

	const char* object_name = args.StringVal(0);
	if (NULL == object_name || strlen(object_name) < 1)
	{
		return;
	}

	sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return;
	}

	if (count == 4)
	{
		pObjectData->fOriginX = args.FloatVal(1);
		pObjectData->fOriginY = args.FloatVal(2);
		pObjectData->fOriginZ = args.FloatVal(3);
		pObjectData->bLocalPosition = true;
	}

	result<< pObjectData->fOriginX<< pObjectData->fOriginY<< pObjectData->fOriginZ;

}


void TimeAxis::GetRotation(const IVarList &args, IVarList& result) const
{
	size_t count = args.GetCount();
	if (count == 0)
	{
		return;
	}

	const char* object_name = args.StringVal(0);
	if (NULL == object_name || strlen(object_name) < 1)
	{
		return;
	}

	const sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return;
	}

	result<< pObjectData->fRotationX<< pObjectData->fRotationY<< pObjectData->fRotationZ;

}


void TimeAxis::SetRotation(const IVarList &args, IVarList& result)
{
	size_t count = args.GetCount();
	if (count == 0)
	{
		return;
	}

	const char* object_name = args.StringVal(0);
	if (NULL == object_name || strlen(object_name) < 1)
	{
		return;
	}

	sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return;
	}

	if (count == 4)
	{
		pObjectData->fRotationX = args.FloatVal(1);
		pObjectData->fRotationY = args.FloatVal(2);
		pObjectData->fRotationZ = args.FloatVal(3);
		pObjectData->bRotation = true;
	}

	BuildPathPoint(object_name);

}

void TimeAxis::GetInitialPosition(const IVarList &args, IVarList& result)
{
	size_t count = args.GetCount();
	if (count == 0)
	{
		return;
	}

	const char* object_name = args.StringVal(0);
	if (NULL == object_name || strlen(object_name) < 1)
	{
		return;
	}

	const sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		return;
	}

	FmVec3 infos = pObjectData->initPos;
	bool b_need_rotation = 
		(pObjectData->bLocalPosition && pObjectData->bRotation);

	if (b_need_rotation)
	{
		RotationKey(&infos, NULL, *pObjectData);
	}

	if (pObjectData->bLocalPosition)
	{
		infos.x += pObjectData->fOriginX;
		infos.y += pObjectData->fOriginY;
		infos.z += pObjectData->fOriginZ;
	}

	result<< infos.x<< infos.y<< infos.z;

}


KeyFrameController* TimeAxis::GetController(const sObjectData& object_data, int controller_type)
{
	KeyFrameController* pControllers = NULL;
	if (!object_data.controllers.GetData(controller_type, pControllers))
	{
		return NULL;
	}

	return pControllers;
}

const KeyFrameController* TimeAxis::GetController(const sObjectData& object_data, int controller_type) const
{
	KeyFrameController* pControllers = NULL;
	if (!object_data.controllers.GetData(controller_type, pControllers))
	{
		return NULL;
	}

	return pControllers;
}

KeyFrameController* TimeAxis::AddController(sObjectData& object_data, int controller_type, 
									   int controller_usage_mode)
{

	KeyFrameController *pController = (KeyFrameController *)CORE_ALLOC(sizeof(KeyFrameController));

	new (pController) KeyFrameController(controller_type, controller_usage_mode, &object_data);

	object_data.controllers.Add(controller_type, pController);

	//发送增加控制器的消息
	fm_movie::TimeAxis_RunCallback(this, "on_add_controller", CVarList()<< object_data.strName.c_str() << controller_type);

	return pController;
}

