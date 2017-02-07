//--------------------------------------------------------------------
// 文件名:		KeyFrameController.h
// 内  容:		
// 说  明:		
// 创建日期:	2012年8月6日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#ifndef __KEY_FRAME_CONTROLLER_H__
#define __KEY_FRAME_CONTROLLER_H__

#include "../public/core_type.h"
#include "../visual/i_vis_base.h"
#include "../utils/array_pod.h"
#include "../math/fm_vector.h"
#include "../extern/rapidxml/rapidxml.hpp"

class ITimeAxis;

// 帧控制器类型
enum eKeyControllerType
{
	// 坐标角度控制器
	CONTROLLER_TYPE_POS_X = 0,
	CONTROLLER_TYPE_POS_Y = 1,
	CONTROLLER_TYPE_POS_Z = 2,
	CONTROLLER_TYPE_ANGLE_X = 3,
	CONTROLLER_TYPE_ANGLE_Y = 4,
	CONTROLLER_TYPE_ANGLE_Z = 5,
	CONTROLLER_TYPE_FOV_X = 6,

	// 震动相关控制器
	CONTROLLER_TYPE_WOBBLE_X = 50,
	CONTROLLER_TYPE_WOBBLE_Y = 51,
	CONTROLLER_TYPE_WOBBLE_Z = 52,
	CONTROLLER_TYPE_WOBBLE_F = 53,
	CONTROLLER_TYPE_WOBBLE_RX = 54,
	CONTROLLER_TYPE_WOBBLE_RY = 55,
	CONTROLLER_TYPE_WOBBLE_RZ = 56,

	// 坐标角度消息id
	CONTROLLER_TYPE_POSITION_ANGLE = 100,


};

// 控制器使用方式
enum eControllerUsageMode
{
	CONTROLLER_USAGE_MODE_BEZIER = 0,		// 贝塞尔曲线
	CONTROLLER_USAGE_MODE_ONCE,				// 每个关键帧只执行一次				
	CONTROLLER_USAGE_MODE_LINEAE,			// 线性插值，比用贝塞尔曲线实现的线性更快
	CONTROLLER_USAGE_MODE_SCALE,			// 动作关键帧

};

enum eKeyChangeType
{
	CHANGE_ADD_KEY = 1,
	CHANGE_DLETEKEY,
	CHANGE_COPY,
	CHANGE_MOVE_KEY,
	CHANGE_MOVE_ALL_KEY,
	
	// BEZIER
	CHANGE_MOVE_ALL_VALUE,
	CHANGE_COMPUTE_CURVE,
	CHANGE_ADD_BEZIER_KEY,
};
// BEZIER,生成控制点的类型

//SHOW_CURVE1
/*
1 低于或高于相邻两点,则控制线水平
2 除首尾点外,采用相邻点连线的平行线作为控制线
3 控制点超过前后点,则取相邻点的值
4 端点水平
*/

//SHOW_CURVE2
/*
1 除首尾点外,采用相邻点连线的平行线作为控制线
2 控制线斜率与相邻点距离成线性变化
3 端点与相邻点等角
*/
enum
{
	SHOW_DOC,     // 从外部获得的数据模式
	SHOW_CURVE1,  // 比较真实
	SHOW_CURVE2,  // 比较平滑
	SHOW_LINE,    // 直线模式
	SHOW_EDITE1,  // 编辑模式
	SHOW_EDITE2,  // 编辑模式

	SHOW_COUNT,
};

struct sKey
{

	virtual  const sKey* Copy(const sKey* pData);

	float	time;
	
};

struct Vec2
{
    float x;
    float y;
};

// Bezier
struct sKeyBezier : public sKey
{
	sKeyBezier();
	virtual  const sKey* Copy(const sKey* pData);
	FmVec2 GetSelfPoint() const { return FmVec2(time, value);}

	float	value;

	union
	{
		struct
		{
			// 前一个控制点
			float	left_time;
			float	left_value;

			// 后一个控制点
			float	right_time;
			float	right_value;
		};

		struct
		{
			Vec2 left;
			Vec2 right;
		};

	};

	
	// 控制线的斜率
	float       kl;
	float       kr;
	// 控制线的长度比
	float       sl;
	float       sr;
	// 控制线的模式
	char        mode;

	// 突变
	//bool	bStep;
};

// Once
struct sKeyOnce : public sKey
{
	virtual  const sKey* Copy(const sKey* pData);

	// 字符串参数
	result_string str_param;			
};

// Line
struct sKeyLine : public sKey
{
	virtual  const sKey* Copy(const sKey* pData);

	// 关键帧的值
	float value;					
};

// Scale
struct sKeyScale : public sKey
{
	virtual  const sKey* Copy(const sKey* pData);

	result_string str_param;		// 字符串参数
	float time_length;				// 动作关键帧时长
	float speed;					// 动作关键帧播放速度
};


// 关键帧控制器
struct sObjectData;
class TiXmlElement;
class KeyFrameController
{

	typedef TArrayPod<sKey *, 1, TCoreAlloc> Keys;
	
public:
	static void SetTimeAxis(ITimeAxis *pEntiy);
	
	KeyFrameController();
	KeyFrameController(int controller_type, int usage_mode, sObjectData* pOwner);
	~KeyFrameController();
	KeyFrameController(const KeyFrameController& right);
	KeyFrameController& operator=(const KeyFrameController& right);

	// 帧控制器类型
	int GetControllerType() const { return m_nControllerType; }

	// 控制器使用方式
	int GetUsageMode() const { return m_nUsageMode; }

	float GetBegin() const;
	float GetEnd() const;

	void AddBezierKey(float time, float value);
	int GetFrontBezierKey(float time, float &value) const;
	void GetAdjacencyBezierKey(float time, IVarList& result) const;
	const sKey* GetKeyFrameByTime(float time, int* index) const;

	// 设置关键帧的值
	// 如果还没有这个关键帧则自动添加
	bool SetKeyFrameBezierMode(float time, float value, 
		float left_time, float left_value, 
		float right_time, float right_value, bool bCallback = true);
	bool SetKeyFrameLinearMode(float time, float value, bool bCallback = true);
	bool SetKeyFrameOnceMode(float time, const char* param, bool bCallback = true);
	bool SetKeyFrameScaleMode(float time, const char* action_name, 
		float time_length, float speed, bool bCallback = true);

	// 删除关键帧
	void DeleteControllerKey(float time);
	// 清空关键帧
	void ClearKeyFrames();
	// 关键帧总数
	size_t GetKeyCount() const { return m_keyFrames.size(); }

	bool GetKeyInfo(IVisBase* pObject, float time, IVarList &result);

	// 获得指定帧的值
	float GetValueBezierMode(float time) const;
	float GetValueLinearMode(float time) const;
	const char* GetValueOnceMode(float time) const;
	bool GetValueScaleMode(float time, core_string& action_name, 
		float& action_time, float& speed);
	// 获取到某一关键帧的数据
	void GetKeyFrameValue(float time, IVarList& result) const;

	// 加载控制器
	bool LoadController(rapidxml::xml_node<>* xml_controller);
	// 保存控制器
	bool SaveController(rapidxml::xml_node<>* xml_controller);

	// 获得关键帧列表
	void GetControllerKeyTime(IVarList& result) const;
	int GetKeyFrameListBezierMode(IVarList& result) const;
	int GetKeyFrameListOnceMode(IVarList& result) const;
	int GetKeyFrameListLinearMode(IVarList& result) const;
	int GetKeyFrameListScaleMode(IVarList& result) const;


	// 创建所有关键点的左右控制点
	int CreateAllControlPoints();
	// 改变控制点的计算方式
	void ChangeComputeTypeKey(float time, int type);
	// 移动关键帧
	void MoveAllKeyTime(float time_add);
	// 平移关键帧的值
	void MoveAllKeyValue(float value_add);
	// 复制关键帧
	void CopyKey(float time, float new_time);
	// 移动某个关键帧
	void MoveKey(float time, float new_time);

	// 收集位移关键帧的关键帧时刻
	void CollectPosKeyTime(TArrayPod<float, 1> &array) const;

	// 获取帧
	sKeyBezier GetBezierKey(size_t index) const;

	// 设置帧
	bool SetBezierKey(size_t index, const sKeyBezier& key);
private:
	// 查找某时刻的关键帧, 返回可以插入的位置.
	sKey* FindKeyFrame(float fTime, int* pIndex = NULL) const;
	sKey* AddKey(float fTime, int nIndex = -1);
	void  DeleteKey(sKey *p);

	// 计算贝塞尔的值
	float GetBezierValue(float x, float c0x, float c0y, float c1x, float c1y, 
		float c2x, float c2y, float c3x, float c3y) const;


	// 加载CONTROLLER_USAGE_MODE_BEZIER控制器
	bool InnerLoadBezierController(rapidxml::xml_node<>* xml_controller);
	// 加载CONTROLLER_USAGE_MODE_LINEAE控制器
	bool InnerLoadLinearController(rapidxml::xml_node<>* xml_controller);
	// 加载CONTROLLER_USAGE_MODE_ONCE控制器
	bool InnerLoadOnceController(rapidxml::xml_node<>* xml_controller);
	// 加载CONTROLLER_USAGE_MODE_SCALE控制器
	bool InnerLoadScaleController(rapidxml::xml_node<>* xml_controller);

	// 保存CONTROLLER_USAGE_MODE_BEZIER控制器
	bool InnerSaveBezierController(rapidxml::xml_node<>* xml_controller);
	// 保存CONTROLLER_USAGE_MODE_LINEAE控制器
	bool InnerSaveLinearController(rapidxml::xml_node<>* xml_controller);
	// 保存CONTROLLER_USAGE_MODE_ONCE控制器
	bool InnerSaveOnceController(rapidxml::xml_node<>* xml_controller);
	// 保存CONTROLLER_USAGE_MODE_SCALE控制器
	bool InnerSaveScaleController(rapidxml::xml_node<>* xml_controller);

private:

	// 创建某关键点的控制点
	int CreateOneControlPoint(int index, bool send_msg = true);

	// 给定前后关键点,计算自己的左右控制点
	int ComputLeftAndRightPoint(sKeyBezier &front, sKeyBezier &self, sKeyBezier &next);
	// 由前一个关键点的控制点,求下一个关键点的控制点,(两个控制点关于两关键点中垂线对称)
	void GetSymmetryPoint(FmVec2 &out, FmVec2 &self, FmVec2 &other, FmVec2 &other_control);

	// 发送更新某个关键帧的消息
	void SendUpdateBezierKey(int nIndex);

private:
	static ITimeAxis* s_pTimeAxis;
	sObjectData*	m_pObject;

	int			m_nControllerType;
	int			m_nUsageMode;

	Keys		m_keyFrames;
	float		m_fLastTime;

	// 是否已经发送左,右端点的时刻数据
	bool		m_bFirstFrame;
	bool		m_bLastFrame;
};

#endif // __KEY_FRAME_CONTROLLER_H__
