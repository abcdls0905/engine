//--------------------------------------------------------------------
// 文件名:		time_axis.h
// 内  容:		
// 说  明:		
// 创建日期:	2012年8月6日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#ifndef __TIME_AXIS_H__
#define __TIME_AXIS_H__

#include "key_frame_controller.h"
#include "../visual/i_time_axis.h"
#include "../visual/i_render.h"
#include "../visual/i_res_loader.h"
#include "../utils/pod_hash_map.h"
#include "../utils/string_pod.h"

// 时间轴

class Pyramid;
class IDxDynamicVB;
class TiXmlDocument;
class TiXmlElement;

// 一般顶点数据
struct vertex_t 
{
	FmVec3 pos;
};

// 关键帧顶点数据
struct vertex_key_frame_t
{
	FmVec3 pos;
	float u, v;
};

// 属性控制器表
typedef TPodHashMap<int, KeyFrameController*, TPodTraits<int>, TCoreAlloc> KeyFrameControllers;
//TPodHashMap
// 对象数据
struct sObjectData
{
	sObjectData();
	~sObjectData();
	//
	// 基本属性
	result_string		strName;
	PERSISTID			ObjectID;
	KeyFrameControllers controllers;		// 各种属性的控制器
	bool				bShowPath;			// 是否显示路径

	bool				bLocalPosition;		// 是否存储的是相对坐标帧
	float				fOriginX;			// 相对坐标系的原点
	float				fOriginY;			// 相对坐标系的原点
	float				fOriginZ;			// 相对坐标系的原点
	FmVec3			    initPos;			// 初始的相对位置

	bool				bRotation;			// 轨迹是否有旋转
	float				fRotationX;
	float				fRotationY;
	float				fRotationZ;
	float				fRAddY;				// Y轴增加旋转度数

	result_string		strExtProp;			// 对象的附加参数

	// 是否有数据改动
	bool				bDataChanged;

	// 异步加载完成之后需要刷新的标记
	IResLoader*         pLoader;

    // 当前帧的位置与旋转
    float			    fPosInfo[6];

	// 用于调试时绘制的数据
	vertex_t*           pFrameVBuffer;		        // 帧顶点数据
#ifdef _DEBUG
	TArrayPod<vertex_t, 1> vFrameVBufferUsedDebug;	// 调试用的
#endif
	unsigned int        nFrameVBufferCount;		    // 帧顶点buffer的顶点个数
	unsigned int        nFrameVCountUsed;			// 实际使用的帧顶点个数，去掉了坐标一样的帧顶点

	vertex_key_frame_t* pKeyFrameVBuffer;	        // 关键帧顶点数据
	unsigned int        nKeyFrameVBufferCount;		// 关键帧顶点buffer的顶点个数

    vertex_key_frame_t  currentFrame[8];           // 当前帧的位置

    void UpdateCurrentFrame();
};

class TimeAxis: public ITimeAxis
{
private:
	// 动画里的所有对象
	typedef TStringPod<char, sObjectData*> Objects;

public:
	TimeAxis();
	virtual ~TimeAxis();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void Update(float seconds);
	virtual void Realize();

	// 播放
	void Play();
	// 暂停在指定时间
	void Pause(float time);
	// 暂停在当前时间
	void PauseNow();

	// 是否加载完成
	bool GetLoadFinish();
	size_t GetLoadCount() const;
	// 添加对象
	bool AddObject(const char* object_name);
	// 删除对象
	bool DeleteObject(const char* object_name);
	// 对象是否存在
	bool IsObjectExist(const char* object_name);
	// 拷贝对象数据
	bool CopyObjectData(const char* src_object_name, const char* dst_object_name);
	// 对象重命名
	bool ReName(const char* src_object_name, const char* new_object_name);
	// 将一个控制器的信息赋值到另一对象中
	bool CopyControllerData(const char* src_object_name, int controller_type, 
		const char* dst_object_name);
	// 拷贝关键帧
	void CopyKeyTime(const char* object_name, float time, float new_time);
	// 拷贝某个控制器的关键帧
	void CopyControllerKeyTime(const char* object_name, int controller_type,
		float time, float new_time);
	// 关键帧数据是否存在
	bool IsKeyFrameExist(const char* object_name, int controller_type, float frame_time) const;

	// 对象ID
	bool SetObjectID(const char* object_name, const PERSISTID& value);
	PERSISTID GetObjectID(const char* object_name) const;

	// 增加Bezier关键点
	bool AddBezierKey(const char* object_name, int controller_type, float time, float value);
	// 改变控制点的计算方式
	void ChangeComputeTypeKey(const char* object_name, int controller_type, float time, int type);
	// 重新计算所有的控制点
	void CreateAllControlPoints(const char* object_name, int controller_type);
	// 移动对象关键帧
	void MoveKeyTimes(const char* object_name, float time_add);
	// 移动控制器的关键帧
	void MoveControllerKeyTimes(const char* object_name, int controller_type, float time_add);
	// 移动Bezier曲线
	void MoveAllKeyValue(const char* object_name, int controller_type, float value_add);
	// 移动关键帧
	void MoveKey(const char* object_name, float time, float new_time);
	// 移动关键帧
	void MoveControllerKey(const char* object_name, int controller_type, float time, float new_time);

	// 获取要记录关键点的前一个关键点
	void GetFrontBezierKey(const IVarList &param, IVarList &result);

	// 获取所有传入关键帧信息的相邻关键帧数据(请保证使用完返回的关键帧指针前不修改或删除关键帧信息)
	void GetAdjacencyBezierKeyData(const IVarList &args, IVarList &result) const;

	// 增加或修改关键帧
	bool SetKeyFrameBezierMode(const char* object_name, int controller_type, 
		float time, float value, 
		float left_time, float left_value, 
		float right_time, float right_value);
	bool SetKeyFrameOnceMode(const char* object_name, int controller_type, 
		float time, const char* param);
	bool SetKeyFrameLinearMode(const char* object_name, int controller_type, 
		float time, float value);
	bool SetKeyFrameScaleMode(const char* object_name, int controller_type, 
		float time, const char* action_name, float time_length, float speed);

	// 删除某时刻的所有关键帧
	void DeleteKey(const char* object_name, float time);
	// 删除关键帧
	bool DeleteControllerKey(const char* object_name, int controller_type, 
		float time);
	// 清空指定控制器的所有关键帧
	void ClearControllerKeyFrames(const char* object_name, int controller_type);
	// 清空对象所有控制器的所有关键帧
	void ClearControllers(const char* object_name);
	// 获得关键帧列表
	void GetControllerKeyTime(const IVarList& args, IVarList& result) const;
	int GetControllerKeyCount(const char* object_name, int controller_type) const;
	int GetKeyFrameListBezierMode(const IVarList& args, IVarList& result) const;
	int GetKeyFrameListOnceMode(const IVarList& args, IVarList& result) const;
	int GetKeyFrameListLinearMode(const IVarList& args, IVarList& result) const;
	int GetKeyFrameListScaleMode(const IVarList& args, IVarList& result) const;
	// 获得对象所有控制器ID
	int GetControllers(const IVarList& args, IVarList& result) const;
	// 获取指定时间的控制器值
	float GetValueBezierMode(const char* object_name, int controller_type, 
		float time) const;
	//const char* GetValueOnceMode(const char* object_name, int controller_type, 
	//	float time) const;
	// 获取某一时刻关键帧帧数据
	void GetKeyFrameValue(const IVarList& args, IVarList& result) const;
	// 获取存储轨迹的坐标类型（是否是相对坐标）
	bool GetIsLocalPosition(const char* object_name) const;
	// 获取对象相对坐标系的坐标
	void GetLocalOrigin(const IVarList &args, IVarList& result) const;
	// 设置对象相对坐标系的坐标
	void SetLocalOrigin(const IVarList &args, IVarList& result);

	// 获取存储轨迹是否经过了旋转
	bool GetIsRotation(const char* object_name) const;
	// 获取轨迹旋转
	float GetRotationY(const char* object_name) const;
	// 设置轨迹旋转,如果是actor需要旋转轨迹180
	void SetRotationY(const char* object_name, float fAngle);
	// 获取轨迹的坐标系旋转角度
	void GetRotation(const IVarList& args, IVarList& result) const;
	// 设置轨迹的坐标系旋转角度
	void SetRotation(const IVarList& args, IVarList& result);

	// 获取对象的额外属性
	const char* GetExtProp(const char* object_name) const;
	// 获取初始状态的位置(主要解决目标摄像机未key的问题)
	void GetInitialPosition(const IVarList &args, IVarList& result);


	// 坐标转换
	void PositionToLocal(const IVarList& args, IVarList& result) const;
	// 角度转换
	void AngleToLocal(const IVarList& args, IVarList& result) const;

	// 是否循环播放
	void SetLoop(bool value) { m_bLoop = value; }
	bool GetLoop() const { return m_bLoop; }

	// 开始时间
	void SetBeginTime(float value) { m_fBeginTime = value; }
	float GetBeginTime() const { return m_fBeginTime; }
	// 结束时间
	void SetEndTime(float value) { m_fEndTime = value; }
	float GetEndTime() const { return m_fEndTime; }

	// 两帧间时长
	void SetFrameInterval(float value) { if (value > 0.0001f) { m_fFrameInterval = value; } }
	float GetFrameInterval() const { return m_fFrameInterval; }

	// 路径是否可见
	void SetPathVisible(const char* object_name, bool bVisible);
	bool GetPathVisible(const char* object_name) const;

	// 生成用于显示的路径点
	bool BuildPathPoint(const char* object_name);

	// 加载对象关键帧
	bool LoadObjectKeyFrame(const char* object_name, const char* file, 
		bool bAsync);

	// 保存对象关键帧
	bool SaveObjectKeyFrame(const char* object_name, const char* file);
		
	// 保存所有对象
	bool SaveAllObject(const char* path, bool bForce);

	// 将轨迹数据导成max格式.
	bool ChangeToMaxPath(sObjectData* pObjectData, float x, float y, float z, float angle_y, int scale);

	// 导出对象关键帧至max(增加缩放)
	bool ExportToMaxPath(const char* object_name, const char* file_name,
		float x, float y, float z, float angle_y, int scale);

	bool ExportToMaxPathFrames(const char* object_name, const char* file_name,
		float x, float y, float z, float angle_y, int scale);

	// 震动关键帧随机数设置.
	bool LoadRandomData(const char *file_name);
	bool SaveRandomData(const char *file_name);

    // 设置回调函数
    void SetCallbackFunc(time_axis_run_callback func)
    {
        m_pRunCallbackFunc = func;
    }

    time_axis_run_callback GetCallbackFunc()
    {
        return m_pRunCallbackFunc;
    }
public:

	// 获得控制器
	KeyFrameController* GetController(const sObjectData& object_data, int controller_type);

	// 获得控制器
	const KeyFrameController* GetController(const sObjectData& object_data, int controller_type) const;

	// 添加控制器
	KeyFrameController* AddController(sObjectData& object_data, 
		int controller_type, int controller_usage_mode);

private:
	IVisBase* GetVisBase(const PERSISTID& id) const;

	sObjectData* InnerGetObjectData(const char* object_name);
	const sObjectData* InnerGetObjectData(const char* object_name) const;
	sObjectData* InnerAddObjectData(const char* object_name);
	sObjectData* InnerAddObjectDataNoInitCtrl(const char* object_name);

	bool InnerSaveObjectKeyFrame(sObjectData* pObjectData, 
		const char* object_name, const char* file_path);

	void InnerUpdate(float time);

	void RotationKey(FmVec3 *pPos, FmVec3 *pAngle, 
		const sObjectData& object_data);

	void GetWobbleInfo(FmVec3 &pos, FmVec3 &angle, float time,
		float *fWobbleInfo, size_t nInfoLen = 7);
	
	float GetRandValue(int index);

	bool GetKeyData(sObjectData &object_data, IVisBase *pObject,  float time,
		float *fPosInfo, size_t nInfoLen = 6);

    // 批次渲染数据
    struct BatchData
    {
        IShaderProgram* pShader;
        IDynamicVB* pVBuffer;

        FmMat4 c_matWVP;
        FmVec4 c_vDiffuse;
        FmVec4 c_vTexelSize;

        fm_int hMatWVP;
        fm_int hDiffuse;
        fm_int hTexelSize;

        void* pVData;
        unsigned int vSize;

        IRenderDrawOp::DRAW_MODE drawMode;
        unsigned int primitiveCount;

        BatchData* pNext;

        BatchData()
        {
            pShader = NULL;
            pVBuffer = NULL;
            hMatWVP = 0;
            hDiffuse = 0;
            hTexelSize = 0;
            pVData = NULL;
            vSize = 0;
            primitiveCount = 0;
            pNext = NULL;
        }
    };

    BatchData* GetBatchData();

    void UpdateBatchData(BatchData* pBatchData, bool keyFrame, const sObjectData& objectData);

    // 渲染函数
    static void RenderBatch(void* pdata);

private:
    static IRender* s_pRender;

	float m_fCurTime;			// 当前时间
	float m_fBeginTime;			// 开始时间
	float m_fEndTime;			// 结束时间
	bool m_bPause;				// 是否暂停
	bool m_bLoop;				// 是否循环播放
	int m_nLastFrame;

	Objects m_objects;			// 动画里的所有对象

	float m_fFrameInterval;		// 两帧间时长

	int m_nShader;
	int m_nShaderKeyFrame;		// 绘制KeyFrame的shader

	size_t m_nLoadCount;

    IShaderProgram* m_pShaderFrame;
    fm_int m_hMatWVP;
    fm_int m_hDiffuse;

    IShaderProgram* m_pShaderKeyFrame;
    fm_int m_hKeyMatWVP;
    fm_int m_hKeyDiffuse;
    fm_int m_hKeyTexelSize;

    IDynamicVB* m_pFrameVBuffer;
    unsigned int m_frameVBufferSize;

    IDynamicVB* m_pKeyFrameVBuffer;
    unsigned int m_keyFrameVBufferSize;

    // 渲染批次
    BatchData* m_listBatchData;
    long m_nBatchCount;

    time_axis_run_callback m_pRunCallbackFunc;
};

#endif // __TIME_AXIS_H__
